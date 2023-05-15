static int write(int fd, const char* buf, unsigned int len) {
    register int ret asm("r0");
    register int _fd asm("r0") = fd;
    register const char* _buf asm("r1") = buf;
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

static int read(int fd, const char* buf, unsigned int len) {
    register int ret asm("r0");
    register int _fd asm("r0") = fd;
    register const char* _buf asm("r1") = buf;
    register unsigned int _len asm("r2") = len;
    register int sys_read asm("r7") = 3;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_fd), "r"(_buf), "r"(_len), "r"(sys_read)
                 :);
    return ret;
}

static int open(const char* path, unsigned int flag) {
    register int ret asm("r0");
    register const char* _path asm("r0") = path;
    register unsigned int _flag asm("r1") = flag;
    register int sys_read asm("r7") = 5;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_path), "r"(_flag), "r"(sys_read)
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

void hooked_func() {
    int src_fd, dst_fd, len, err;
    char buffer[4096];
    char *src_path = {0x2F, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D,
                      0x2F, 0x65, 0x74, 0x63, 0x2F, 0x69, 0x6D,
                      0x61, 0x67, 0x65, 0x2E, 0x64, 0x61, 0x74},
         *dst_path = {0x2F, 0x73, 0x74, 0x6F, 0x72, 0x61, 0x67, 0x65,
                      0x2F, 0x75, 0x73, 0x62, 0x30, 0x2F, 0x69, 0x6D,
                      0x61, 0x67, 0x65, 0x2E, 0x64, 0x61, 0x74},
         *error_source = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6F, 0x70, 0x65,
                          0x6E, 0x69, 0x6E, 0x67, 0x20, 0x73, 0x6F, 0x75, 0x72,
                          0x63, 0x65, 0x20, 0x66, 0x69, 0x6C, 0x65, 0x5C, 0x6E},
         *error_dest = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6F, 0x70,
                        0x65, 0x6E, 0x69, 0x6E, 0x67, 0x20, 0x64, 0x65,
                        0x73, 0x74, 0x69, 0x6E, 0x61, 0x74, 0x69, 0x6F,
                        0x6E, 0x20, 0x66, 0x69, 0x6C, 0x65, 0x5C, 0x6E},
         *error_read = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x72,
                        0x65, 0x61, 0x64, 0x69, 0x6E, 0x67, 0x20,
                        0x66, 0x69, 0x6C, 0x65, 0x2E, 0x5C, 0x6E},
         *error_write = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x77,
                         0x72, 0x69, 0x74, 0x69, 0x6E, 0x67, 0x20,
                         0x66, 0x69, 0x6C, 0x65, 0x2E, 0x5C, 0x6E},
         *copy_success = {0x43, 0x6F, 0x70, 0x79, 0x20, 0x73, 0x75,
                          0x63, 0x63, 0x65, 0x73, 0x73, 0x5C, 0x6E};

    src_fd = open(src_path, 0);
    dst_fd = open(dst_path, 0x40 | 0x1);

    if (src_fd < 0) {
        write(1, error_source, 26);
        goto END;
    }

    if (dst_fd < 0) {
        write(1, error_dest, 31);
        goto END;
    }

    while (1) {
        len = read(src_fd, buffer, 4096);
        if (len == -1) {
            write(1, error_read, 20);
            goto END;
        }

        if (len == 0) break;

        err = write(dst_fd, buffer, len);
        if (err == -1) {
            write(1, error_write, 20);
            goto END;
        }
    }

    write(1, copy_success, 13);

END:
    close(src_fd);
    close(dst_fd);
    return;
}