#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#define SYSTEM_WAIT_USEC	(100*1000) // 100 milisec 

static char *conspath[] = {
    "/proc/self/fd/0",
    "/dev/tty",
    "/dev/tty0",
    "/dev/vc/0",
    "/dev/systty",
    "/dev/console",
    NULL
};

static int refresh_display( void ) {
    int ret;
    ret = system("/bin/chvt 7");
    usleep(SYSTEM_WAIT_USEC);
    ret = system("/usr/bin/find /sys/class/display/*/enable|/usr/bin/xargs -I {} /bin/sh -c '/bin/echo 0 > {}'");
    usleep(SYSTEM_WAIT_USEC);
    ret = system("/usr/bin/find /sys/class/display/*/enable|/usr/bin/xargs -I {} /bin/sh -c '/bin/echo 1 > {}'");
    usleep(SYSTEM_WAIT_USEC);
    return 0;
}

static int stop_surfaceflinger( void ) {
    int ret = system("/usr/bin/pkill -9 surfaceflinger  >/dev/null 2>/dev/null");
    usleep(SYSTEM_WAIT_USEC);
    if(ret == -1) return 1;
    else return 0;
}

static int start_surfaceflinger( void ) {
    int ret = 0;
    ret = stop_surfaceflinger();
    ret = system("/usr/bin/nohup /system/bin/surfaceflinger >/dev/null 2>/dev/null &");
    usleep(SYSTEM_WAIT_USEC);
    if(ret == -1) return 1;
    else return 0;
}

/*
 * getfd.c
 *
 * Get an fd for use with kbd/console ioctls.
 * We try several things because opening /dev/console will fail
 * if someone else used X (which does a chown on /dev/console).
 */

static int is_a_console(int fd) {
    char arg;

    arg = 0;
    return (isatty (fd)
        && ioctl(fd, KDGKBTYPE, &arg) == 0
        && ((arg == KB_101) || (arg == KB_84)));
}

static int open_a_console(const char *fnam) {
    int fd;

    /*
     * For ioctl purposes we only need some fd and permissions
     * do not matter. But setfont:activatemap() does a write.
     */
    fd = open(fnam, O_RDWR);
    if (fd < 0) fd = open(fnam, O_WRONLY);
    if (fd < 0) fd = open(fnam, O_RDONLY);
    if (fd < 0) return -1;
    return fd;
}

static int getfd(const char *fnam) {
    int fd, i;

    if (fnam) {
        if ((fd = open_a_console(fnam)) >= 0) {
            if (is_a_console(fd))
                return fd;
            close(fd);
        }
        fprintf(stderr, "%s: Couldn't open %s\n", __FILE__, fnam);
        //exit(1);
        return -1;
    }

    for (i = 0; conspath[i]; i++) {
        if ((fd = open_a_console(conspath[i])) >= 0) {
            if (is_a_console(fd))
                return fd;
            close(fd);
        }
    }

    for (fd = 0; fd < 3; fd++)
        if (is_a_console(fd))
            return fd;

    fprintf(stderr,
        "%s: Couldn't get a file descriptor referring to the console\n",
        __FILE__);

    /* total failure */
    //exit(1);
    return -1; 
}

static int chvt(int num) {
    int fd;
    fd = getfd(NULL);
    if( fd == -1 ) return 1;
    if (ioctl(fd,VT_ACTIVATE,num)) {
        perror("machybrisegl: VT_ACTIVATE");
        return(1);
    }
    if (ioctl(fd,VT_WAITACTIVE,num)) {
        perror("machybrisegl: VT_WAITACTIVE");
        return(1);
    }
    return(0);
}

static int initialized = 0;
static int cleanedup = 0;
static int surfaceflinger = 0;

static void check_surfaceflinger( void ) {
    surfaceflinger = 0;
    char* env_val = getenv("EGL_PLATFORM");
    if ((env_val != NULL) && (strcmp(env_val, "surfaceflinger") == 0))
        surfaceflinger = 1; 
    return;
}

static void
cleanup(void)
{
    //printf("cleanup!\n"); fflush(stdout);
    if(initialized && !cleanedup) {
        if(surfaceflinger) {
            if(stop_surfaceflinger()) {
                printf("machybrisegl: stop_surfaceflinger() failed!\n"); fflush(stdout);
            } else {
                printf("machybrisegl: stop_surfaceflinger() succeeded!\n"); fflush(stdout);
            }
        }
        //printf("only cleanup!\n"); fflush(stdout);
        if(chvt(7)) {
            printf("machybrisegl: chvt(7) failed!\n"); fflush(stdout);
        } else {
            printf("machybrisegl: chvt(7) succeeded!\n"); fflush(stdout);
        }
        refresh_display();
        cleanedup = 1;
    }
}

static jmp_buf fatal_cleanup;

static void catch_exit_signal(int signal) {
    //printf("catch_exit_signal (%d)!\n", signal); fflush(stdout);
    siglongjmp(fatal_cleanup,signal);
}

static struct sigaction act,old;

void catch_exit_signals(void) {
    int termsig;

    memset(&act,0,sizeof(act));
    act.sa_handler = catch_exit_signal;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act,&old);
    sigaction(SIGQUIT,&act,&old);
    sigaction(SIGTERM,&act,&old);

    sigaction(SIGABRT,&act,&old);
    sigaction(SIGTSTP,&act,&old);

    sigaction(SIGBUS, &act,&old);
    sigaction(SIGILL, &act,&old);
    sigaction(SIGSEGV,&act,&old);

    if (0 == (termsig = sigsetjmp(fatal_cleanup,0))) return;

    cleanup();
    //fprintf(stderr,"Oops: %s (%d)\n",sys_siglist[termsig], termsig); 
    //fflush(stderr);
    exit(42);
}

static void myinit() __attribute__((constructor));

static void myinit() {
    if(!initialized) {
        //printf("init!\n"); fflush(stdout);
        catch_exit_signals();
        signal(SIGTSTP,SIG_IGN);
        initialized = 1;

        if(chvt(10)) {
            printf("machybrisegl: chvt(10) failed!\n"); fflush(stdout);
        } else {
            printf("machybrisegl: chvt(10) succeeded!\n"); fflush(stdout);
        }
        check_surfaceflinger();
        if(surfaceflinger) {
            if(start_surfaceflinger()) {
                printf("machybrisegl: start_surfaceflinger() failed!\n"); fflush(stdout);
            } else {
                printf("machybrisegl: start_surfaceflinger() succeeded!\n"); fflush(stdout);
            }
        }
    }
}

static void myexit() __attribute__((destructor));

static void myexit() {
    //printf("myexit!\n"); fflush(stdout);
    cleanup();
}

/*
int main() {
    int i;
    for(i=0;i < 5;i++) {
        sleep( 1 );
        printf("."); fflush(stdout);
        if(i==3) { int* pi = NULL; *pi = 314; }
    } 
    printf("\n");
    fflush(stdout);
    return 0;
}
*/
