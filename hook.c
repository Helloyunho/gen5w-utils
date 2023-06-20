#define O_WRONLY 0x0001
#define O_CREAT 0x0040
#define NULL 0x0

typedef unsigned int mode_t;

struct timeval {
    long tv_sec;  /* seconds */
    long tv_usec; /* microseconds */
};

struct rusage {
    struct timeval ru_utime; /* user time used */
    struct timeval ru_stime; /* system time used */
    long ru_maxrss;          /* maximum resident set size */
    long ru_ixrss;           /* integral shared memory size */
    long ru_idrss;           /* integral unshared data size */
    long ru_isrss;           /* integral unshared stack size */
    long ru_minflt;          /* page reclaims */
    long ru_majflt;          /* page faults */
    long ru_nswap;           /* swaps */
    long ru_inblock;         /* block input operations */
    long ru_oublock;         /* block output operations */
    long ru_msgsnd;          /* messages sent */
    long ru_msgrcv;          /* messages received */
    long ru_nsignals;        /* signals received */
    long ru_nvcsw;           /* voluntary context switches */
    long ru_nivcsw;          /* involuntary " */
};

static int write(int fd, const char *buf, unsigned int len) {
    register int ret asm("r0");
    register int _fd asm("r0") = fd;
    register const char *_buf asm("r1") = buf;
    register unsigned int _len asm("r2") = len;
    register int sys_write asm("r7") = 4;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_fd), "r"(_buf), "r"(_len), "r"(sys_write)
                 :);
    return ret;
}

static int exit(int code) {
    register int ret asm("r0");
    register int _code asm("r0") = code;
    register int sys_exit asm("r7") = 1;
    asm volatile("svc #0" : "=r"(ret) : "r"(_code), "r"(sys_exit) :);
    return ret;
}

static int execve(const char *filename, char *const argv[],
                  char *const envp[]) {
    register int ret asm("r0");
    register const char *_filename asm("r0") = filename;
    register char *const *_argv asm("r1") = argv;
    register char *const *_envp asm("r2") = envp;
    register int sys_execve asm("r7") = 11;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_filename), "r"(_argv), "r"(_envp), "r"(sys_execve)
                 :);
    return ret;
}

static int fork(void) {
    register int ret asm("r0");
    register int sys_fork asm("r7") = 2;
    asm volatile("svc #0" : "=r"(ret) : "r"(sys_fork) :);
    return ret;
}

static int wait4(int pid, int *status, int options, struct rusage *rusage) {
    register int ret asm("r0");
    register int _pid asm("r0") = pid;
    register int *_status asm("r1") = status;
    register int _options asm("r2") = options;
    register struct rusage *_rusage asm("r3") = rusage;
    register int sys_execve asm("r7") = 114;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_pid), "r"(_status), "r"(_options), "r"(_rusage),
                   "r"(sys_execve)
                 :);
    return ret;
}

static int open(const char *path, int flags, mode_t mode) {
    register int ret asm("r0");
    register const char *_path asm("r0") = path;
    register int _flags asm("r1") = flags;
    register mode_t _mode asm("r2") = mode;
    register int sys_open asm("r7") = 5;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_path), "r"(_flags), "r"(_mode), "r"(sys_open)
                 :);
    return ret;
}

static int close(int fd) {
    register int ret asm("r0");
    register int _fd asm("r0") = fd;
    register int sys_read asm("r7") = 6;
    asm volatile("svc #0" : "=r"(ret) : "r"(_fd), "r"(sys_read) :);
    return ret;
}

static int itoa(int value, char *sp, int radix) {
    char tmp[16];  // be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp) {
        i = v % radix;
        v /= radix;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign) {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp) *sp++ = *--tp;

    return len;
}

void hooked_func() {
    int log_fp;

    // Paths
    //@textify
    char log_filename[] = "/storage/usb0/XXlog.txt";
    //@textify
    char script_path[] = "/storage/usb0/run.sh";
    //@textify
    char sh_path[] = "/system/bin/sh";

    // Other
    //@textify
    char sanity_value[] = "Hello!\n";
    //@textify
    char space[] = "  ";
    //@textify
    char sanity_failed[] = "Sanity check failed\n";
    //@textify
    char run_sh[] = "Going to launch usb0/run.sh\n";
    //@textify
    char pid_info[] = "PID: ";
    //@textify
    char status_info[] = "Status: ";
    //@textify
    char sh[] = "sh";

    log_fp = open(log_filename, O_WRONLY | O_CREAT, 0777);
    if (log_fp < 0) {
        char error_str[15] = {0};
        itoa(log_fp, error_str, 10);
        write(1, error_str, sizeof(error_str) - 1);
        write(1, space, 2);
        write(1, sanity_failed, 21);
        goto LOG_END;
    }
    write(log_fp, sanity_value, sizeof(sanity_value) - 1);

    // Execute run.sh
    write(log_fp, run_sh, sizeof(run_sh) - 1);
    int pid = fork();
    if (pid == 0) {
        char *argv[] = {sh, script_path, NULL};
        execve(sh_path, argv, NULL);
        exit(0);
    }
    int status;
    write(log_fp, pid_info, sizeof(pid_info) - 1);
    char pid_str[15] = {0};
    itoa(pid, pid_str, 10);
    write(log_fp, pid_str, sizeof(pid_str) - 1);
    wait4(pid, &status, 0, NULL);
    char status_str[15] = {0};
    itoa(status, status_str, 10);
    write(log_fp, space, 2);
    write(log_fp, status_info, sizeof(status_info) - 1);
    write(log_fp, status_str, sizeof(status_str) - 1);
LOG_END:
    close(log_fp);
    return;
}
