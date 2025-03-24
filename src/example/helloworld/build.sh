# sudo ln -s /usr/include/x86_64-linux-gnu/asm /usr/include/asm

clang -O2 -target bpf -c hello_ebpf.c -o hello_ebpf.o
g++ -o hello_user hello_user.cpp -lbpf

# dump the eBPF program
# llvm-objdump -h o_ebpf.o | grep rodata

# enable tracing
echo 1 | sudo tee /sys/kernel/debug/tracing/tracing_on

# load the eBPF program
sudo setcap cap_bpf,cap_sys_admin+ep ./hello_user
sudo ./hello_user

# trace the eBPF program
# sudo cat /sys/kernel/debug/tracing/trace_pipe