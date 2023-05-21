
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_CREAT  0x0200
#define EOF -1
#define R_OK 4
#define MAX_FILENAME 256
#define BUFF_SIZE 4098

typedef unsigned int mode_t;
typedef int off_t;

typedef struct dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
} dirent;

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

static int open(const char* path, int flags, mode_t mode) {
    register int ret asm("r0");
    register const char* _path asm("r0") = path;
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

static int getdents(unsigned int fd, struct dirent* dirp, unsigned int count) {
    register int ret asm("r0");
    register unsigned int _fd asm("r0") = fd;
    register struct dirent* _dirp asm("r1") = dirp;
    register unsigned int _count asm("r2") = count;
    register int sys_getdents asm("r7") = 141; // getdents syscall number
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_fd), "r"(_dirp), "r"(_count), "r"(sys_getdents)
                 :);
    return ret;
}

static int access(const char* path, int mode) {
    register int ret asm("r0");
    register const char* _path asm("r0") = path;
    register int _mode asm("r1") = mode;
    register int sys_access asm("r7") = 33;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_path), "r"(_mode), "r"(sys_access)
                 :);
    return ret;
}

static int fgetc(int fd) {
	char c;
	if (read(fd, &c, 1) == 1) {
		return c;
	} else {
		return EOF;
	}
}

static int fputc(char c, int fd) {
	write(fd, &c, 1);
}

void hooked_func() {
    int src_fp, dst_fp, len, err, nread;
	struct dirent *d;
    char buffer[BUFF_SIZE];
	char d_buffer[BUFF_SIZE];
	char src_path[] = "/system/etc/image.dat";
	char dst_path[] = "/storage/usb0/image.dat";
	char error_source[] = "E: source sec key open\n";
	char error_dest[] = "E: dest sec key open\n";
	char error_read[] = "E: reading sec key file\n";
	char error_write[] = "E: writing sec key file\n";
	char copy_success[] = "Copy sec key success\n";
	char readable[] = " is readable\n";
	char not_readable[] = " is not readable\n";

	// print directory permissions
	int dir_fd = open("/system/etc", O_RDONLY, 0);
	if (dir_fd < 0) {
        write(1, "Error opening etc directory\n", 28);
		return;
	}

	while (1) {
		nread = getdents(dir_fd, (struct dirent *)d_buffer, BUFF_SIZE);
		if (nread < 0) {
			write(1, "Error reading etc directory\n", 29);
			return;
		}
		if (nread == 0) { // end of directory
			break;
		}

		for (int bpos = 0; bpos < nread;) {
       		d = (struct dirent *) (d_buffer + bpos);

			int name_len = 0;
			while(name_len < MAX_FILENAME && d->d_name[name_len]) {
            	name_len++;
			}

            write(1, "d_name: ", 8);
			write(1, d->d_name, name_len);
			if (access(d->d_name, R_OK) == 0) {
				write(1, readable, sizeof(readable) - 1);
			} else {
				write(1, not_readable, sizeof(not_readable) - 1);
			}

			bpos += d->d_reclen;
		}
	}

	close(dir_fd);

    src_fp = open(src_path, O_RDONLY, 0);
    dst_fp = open(dst_path, O_WRONLY | O_CREAT, 0777);

    if (src_fp < 0) {
        write(1, error_source, sizeof(error_source) - 1);
        goto END;
	}

    if (dst_fp < 0) {
        write(1, error_dest, sizeof(error_dest) - 1);
        goto END;
    }

	int c, i = 0;
	write(1, "start copy========\n", 19);
	while((c = fgetc(src_fp+i)) != EOF) {
    	fputc(c, dst_fp+i);
		write(1, "  ", 2);
		write(1, (char *)&c, 1);
	}
	write(1, "\nend copy========\n", 18);

    write(1, copy_success, sizeof(copy_success) - 1);

END:
	write(1, "end========\n", 12);
    close(src_fp);
    close(dst_fp);
    return;
}
