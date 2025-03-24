#include <linux/bpf.h>
#include <linux/fcntl.h>
#include <bpf/bpf_helpers.h>

// Minimal pt_regs for x86_64
struct pt_regs {
    unsigned long r15;
    unsigned long r14;
    unsigned long r13;
    unsigned long r12;
    unsigned long bp;
    unsigned long bx;
    unsigned long r11;
    unsigned long r10;
    unsigned long r9;
    unsigned long r8;
    unsigned long ax;
    unsigned long cx;
    unsigned long dx;  // rdx
    unsigned long si;  // rsi
    unsigned long di;  // rdi
    unsigned long orig_ax;
    unsigned long ip;
    unsigned long cs;
    unsigned long flags;
    unsigned long sp;
    unsigned long ss;
};

char _license[] SEC("license") = "GPL";
unsigned int _version SEC("version") = 1;

static const char fmt[] SEC(".rodata") = "File created: %s (read: %d)\n";
static const char err_fmt[] SEC(".rodata") = "Read failed: %d\n";
// static const char ptr_fmt[] SEC(".rodata") = "Filename ptr: %lx\n";

SEC("kprobe/do_sys_openat2")
int trace_file_create(struct pt_regs *ctx) {
    // x86_64: rsi = filename, rdx = struct open_how *
    void *filename = (void *)ctx->si;         // rsi = filename
    struct open_how *how = (struct open_how *)ctx->dx;  // rdx = pointer to open_how

    int flags;
    int ret = bpf_probe_read_kernel(&flags, sizeof(flags), &how->flags);
    if (ret < 0) {
        bpf_trace_printk(err_fmt, sizeof(err_fmt), ret);
        return 0;
    }

    if (flags & O_CREAT) {
        // bpf_trace_printk(ptr_fmt, sizeof(ptr_fmt), (long)filename);
        char filename_buf[256];
        int read_len = bpf_probe_read_user_str(filename_buf, sizeof(filename_buf), filename);
        if (read_len > 0) {
            bpf_trace_printk(fmt, sizeof(fmt), filename_buf, read_len);
        } else {
            bpf_trace_printk(err_fmt, sizeof(err_fmt), read_len);
        }
    }

    return 0;
}


// #include <linux/bpf.h>
// #include <linux/fs.h>
// #include <linux/ptrace.h>
// #include <bpf/bpf_helpers.h>
// #include <bpf/bpf_tracing.h>

// static const char fmt[] SEC(".rodata") = "File created: %s (read: %d)\n";

// SEC("kprobe/do_sys_open")
// int trace_file_create(struct pt_regs *ctx) {
//     char filename[256];
//     int flags;
//     int read_len;

//     // Extract filename (second argument)
//     void *filename_ptr = (void *)PT_REGS_PARM2(ctx);
//     read_len = bpf_probe_read_user_str(filename, sizeof(filename), filename_ptr);

//     // Extract flags (third argument)
//     flags = PT_REGS_PARM3(ctx);

//     // Print output
//     bpf_trace_printk(fmt, sizeof(fmt), filename_ptr, read_len);
    
//     return 0;
// }

// char _license[] SEC("license") = "GPL";