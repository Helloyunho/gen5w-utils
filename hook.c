#define O_WRONLY 0x0001
#define O_CREAT 0x0040
#define NULL 0x0

typedef unsigned int mode_t;

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

static int strlen(const char *str) {
    const char *s;

    for (s = str; *s; ++s)
        ;
    return s - str;
}

static void strrev(char *str) {
    int i;
    int j;
    unsigned char a;
    unsigned len = strlen((const char *)str);
    for (i = 0, j = len - 1; i < j; i++, j--) {
        a = str[i];
        str[i] = str[j];
        str[j] = a;
    }
}

static int itoa(int num, char *str, int len, int base) {
    int sum = num;
    int i = 0;
    int digit;
    if (len == 0) return -1;
    do {
        digit = sum % base;
        if (digit < 0xA)
            str[i++] = '0' + digit;
        else
            str[i++] = 'A' + digit - 0xA;
        sum /= base;
    } while (sum && (i < (len - 1)));
    if (i == (len - 1) && sum) return -1;
    str[i] = '\0';
    strrev(str);
    return 0;
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
    char sh[] = "sh";

    log_fp = open(log_filename, O_WRONLY | O_CREAT, 0777);
    if (log_fp < 0) {
        char error_str[128] = {0};
        itoa(log_fp, error_str, sizeof(error_str), 10);
        write(1, error_str, sizeof(error_str) - 1);
        write(1, space, 2);
        write(1, sanity_failed, 21);
        goto LOG_END;
    }
    write(log_fp, sanity_value, sizeof(sanity_value) - 1);

    // Execute run.sh
    write(log_fp, run_sh, sizeof(run_sh) - 1);
    char *argv[] = {sh, script_path, NULL};
    execve(sh_path, argv, NULL);
LOG_END:
    close(log_fp);
    return;
}
