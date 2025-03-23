#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

char _license[] SEC("license") = "GPL";
unsigned int _version SEC("version") = 1;

// Define the string in a custom .rodata section
static const char msg[] SEC(".rodata") = "Hello, World!\n";

SEC("tracepoint/syscalls/sys_enter_execve")
int hello_world(void *ctx) {
    bpf_trace_printk(msg, sizeof(msg));
    return 0;
}