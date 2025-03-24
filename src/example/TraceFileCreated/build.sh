clang -O2 -target bpf -D__TARGET_ARCH_x86 -c trace_file_create.c -o trace_file_create.o
g++ -o ebpf_loader ebpf_loader.cpp -lbpf

# dump the eBPF program
llvm-objdump -h trace_file_create.o | grep rodata

# enable tracing
echo 1 | sudo tee /sys/kernel/debug/tracing/tracing_on

# load the eBPF program
sudo setcap cap_bpf,cap_sys_admin+ep ./ebpf_loader
sudo ./ebpf_loader

# trace the eBPF program
# sudo cat /sys/kernel/debug/tracing/trace_pipe
# sudo cat /sys/kernel/debug/tracing/events/syscalls/sys_enter_openat/format