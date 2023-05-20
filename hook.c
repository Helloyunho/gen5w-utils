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
    char src_path[22], dst_path[24], error_source[16], error_dest[14],
        error_read[14], error_write[15], copy_success[14];
    // char *src_path = {0x2F, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D,
    //                   0x2F, 0x65, 0x74, 0x63, 0x2F, 0x69, 0x6D,
    //                   0x61, 0x67, 0x65, 0x2E, 0x64, 0x61, 0x74},
    //      *dst_path = {0x2F, 0x73, 0x74, 0x6F, 0x72, 0x61, 0x67, 0x65,
    //                   0x2F, 0x75, 0x73, 0x62, 0x30, 0x2F, 0x69, 0x6D,
    //                   0x61, 0x67, 0x65, 0x2E, 0x64, 0x61, 0x74},
    //      *error_source = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6F, 0x70,
    //      0x65,
    //                       0x6E, 0x69, 0x6E, 0x67, 0x20, 0x73, 0x6F, 0x75,
    //                       0x72, 0x63, 0x65, 0x20, 0x66, 0x69, 0x6C, 0x65,
    //                       0x0A},
    //      *error_dest = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6F, 0x70,
    //                     0x65, 0x6E, 0x69, 0x6E, 0x67, 0x20, 0x64, 0x65,
    //                     0x73, 0x74, 0x69, 0x6E, 0x61, 0x74, 0x69, 0x6F,
    //                     0x6E, 0x20, 0x66, 0x69, 0x6C, 0x65, 0x0A},
    //      *error_read = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x72,
    //                     0x65, 0x61, 0x64, 0x69, 0x6E, 0x67, 0x20,
    //                     0x66, 0x69, 0x6C, 0x65, 0x0A},
    //      *error_write = {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x77,
    //                      0x72, 0x69, 0x74, 0x69, 0x6E, 0x67, 0x20,
    //                      0x66, 0x69, 0x6C, 0x65, 0x0A},
    //      *copy_success = {0x43, 0x6F, 0x70, 0x79, 0x20, 0x73, 0x75,
    //                       0x63, 0x63, 0x65, 0x73, 0x73, 0x0A};

    src_path[0] = '/';
    src_path[1] = 's';
    src_path[2] = 'y';
    src_path[3] = 's';
    src_path[4] = 't';
    src_path[5] = 'e';
    src_path[6] = 'm';
    src_path[7] = '/';
    src_path[8] = 'e';
    src_path[9] = 't';
    src_path[10] = 'c';
    src_path[11] = '/';
    src_path[12] = 'i';
    src_path[13] = 'm';
    src_path[14] = 'a';
    src_path[15] = 'g';
    src_path[16] = 'e';
    src_path[17] = '.';
    src_path[18] = 'd';
    src_path[19] = 'a';
    src_path[20] = 't';
    src_path[21] = '\0';

    dst_path[0] = '/';
    dst_path[1] = 's';
    dst_path[2] = 't';
    dst_path[3] = 'o';
    dst_path[4] = 'r';
    dst_path[5] = 'a';
    dst_path[6] = 'g';
    dst_path[7] = 'e';
    dst_path[8] = '/';
    dst_path[9] = 'u';
    dst_path[10] = 's';
    dst_path[11] = 'b';
    dst_path[12] = '0';
    dst_path[13] = '/';
    dst_path[14] = 'i';
    dst_path[15] = 'm';
    dst_path[16] = 'a';
    dst_path[17] = 'g';
    dst_path[18] = 'e';
    dst_path[19] = '.';
    dst_path[20] = 'd';
    dst_path[21] = 'a';
    dst_path[22] = 't';
    dst_path[23] = '\0';

    error_source[0] = 'E';
    error_source[1] = ':';
    error_source[2] = ' ';
    error_source[3] = 's';
    error_source[4] = 'o';
    error_source[5] = 'u';
    error_source[6] = 'r';
    error_source[7] = 'c';
    error_source[8] = 'e';
    error_source[9] = ' ';
    error_source[10] = 'o';
    error_source[11] = 'p';
    error_source[12] = 'e';
    error_source[13] = 'n';
    error_source[14] = '\n';
    error_source[15] = '\0';

    error_dest[0] = 'E';
    error_dest[1] = ':';
    error_dest[2] = ' ';
    error_dest[3] = 'd';
    error_dest[4] = 'e';
    error_dest[5] = 's';
    error_dest[6] = 't';
    error_dest[7] = ' ';
    error_dest[8] = 'o';
    error_dest[9] = 'p';
    error_dest[10] = 'e';
    error_dest[11] = 'n';
    error_dest[12] = '\n';
    error_dest[13] = '\0';

    error_read[0] = 'E';
    error_read[1] = ':';
    error_read[2] = ' ';
    error_read[3] = 'r';
    error_read[4] = 'e';
    error_read[5] = 'a';
    error_read[6] = 'd';
    error_read[7] = ' ';
    error_read[8] = 'f';
    error_read[9] = 'i';
    error_read[10] = 'l';
    error_read[11] = 'e';
    error_read[12] = '\n';
    error_read[13] = '\0';

    error_write[0] = 'E';
    error_write[1] = ':';
    error_write[2] = ' ';
    error_write[3] = 'w';
    error_write[4] = 'r';
    error_write[5] = 'i';
    error_write[6] = 't';
    error_write[7] = 'e';
    error_write[8] = ' ';
    error_write[9] = 'f';
    error_write[10] = 'i';
    error_write[11] = 'l';
    error_write[12] = 'e';
    error_write[13] = '\n';
    error_write[14] = '\0';

    copy_success[0] = 'C';
    copy_success[1] = 'o';
    copy_success[2] = 'p';
    copy_success[3] = 'y';
    copy_success[4] = ' ';
    copy_success[5] = 's';
    copy_success[6] = 'u';
    copy_success[7] = 'c';
    copy_success[8] = 'c';
    copy_success[9] = 'e';
    copy_success[10] = 's';
    copy_success[11] = 's';
    copy_success[12] = '\n';
    copy_success[13] = '\0';

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
            write(1, error_read, 19);
            goto END;
        }

        if (len == 0) break;

        err = write(dst_fd, buffer, len);
        if (err == -1) {
            write(1, error_write, 19);
            goto END;
        }
    }

    write(1, copy_success, 13);

END:
    close(src_fd);
    close(dst_fd);
    return;
}