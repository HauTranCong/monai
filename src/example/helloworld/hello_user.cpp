#include <iostream>
#include <bpf/libbpf.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <cstring>

volatile bool exiting = false;

void signal_handler(int sig) {
    exiting = true;
}

int main() {
    struct bpf_object *obj = nullptr;
    struct bpf_program *prog = nullptr;
    struct bpf_link *link = nullptr;  // Add this to store the attachment link
    int prog_fd = -1;

    signal(SIGINT, signal_handler);

    obj = bpf_object__open_file("hello_ebpf.o", nullptr);
    if (libbpf_get_error(obj)) {
        std::cerr << "Failed to open BPF object: " << strerror(errno) << "\n";
        return 1;
    }
    std::cout << "BPF object opened successfully\n";

    if (bpf_object__load(obj)) {
        std::cerr << "Failed to load BPF object: " << strerror(errno) << "\n";
        bpf_object__close(obj);
        return 1;
    }
    std::cout << "BPF object loaded successfully\n";

    prog = bpf_object__find_program_by_name(obj, "hello_world");
    if (!prog) {
        std::cerr << "Failed to find program 'hello_world'\n";
        bpf_object__close(obj);
        return 1;
    }
    std::cout << "Found program 'hello_world'\n";

    prog_fd = bpf_program__fd(prog);
    if (prog_fd < 0) {
        std::cerr << "Failed to get program FD: " << strerror(errno) << "\n";
        bpf_object__close(obj);
        return 1;
    }
    std::cout << "Program FD: " << prog_fd << "\n";

    // Attach the program and store the link
    link = bpf_program__attach(prog);
    if (libbpf_get_error(link)) {
        std::cerr << "Failed to attach program: " << strerror(errno) << "\n";
        bpf_object__close(obj);
        return 1;
    }
    std::cout << "Program attached successfully\n";

    std::cout << "Check /sys/kernel/debug/tracing/trace_pipe\n";
    std::cout << "Press Ctrl+C to exit...\n";

    while (!exiting) {
        sleep(1);
    }

    // Cleanup
    bpf_link__destroy(link);  // Destroy the link before closing the object
    bpf_object__close(obj);
    std::cout << "Exiting cleanly\n";
    return 0;
}