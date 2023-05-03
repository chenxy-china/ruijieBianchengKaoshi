#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

/*bash 中捕获每一个输入字符*/
int main(void)
{
    int fd;
    fd_set readfds;
    unsigned char c;
    int result;
    struct termios ts, ots;

    fd = STDIN_FILENO;
         
    tcgetattr(fd, &ts);
    ots = ts;
    ts.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fd, TCSAFLUSH, &ts);

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
                
    while (1) {
        result = pselect (1, &readfds, NULL, NULL, NULL, NULL);

        result = read(fd, &c, 1);

        if (result != 1) {
            break;
        }

		int ret = write(STDOUT_FILENO, &c, 1);	
        if(ret == 0)
            continue;
    }
    tcsetattr(fd, TCSAFLUSH, &ots);
    return 0;
}