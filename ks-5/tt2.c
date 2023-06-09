#include <unistd.h>
#include <stdio.h>
#include <termios.h>


static struct termios initial_settings, new_settings;
static int peek_character = -1;
void init_keyboard(void);
void close_keyboard(void);
int kbhit(void);
int readch(void);

void init_keyboard()
{
    tcgetattr(0, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag |= ICANON;
    new_settings.c_lflag |= ECHO;
    new_settings.c_lflag |= ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard()
{
    tcsetattr(0, TCSANOW, &initial_settings);
}

/*kbhit() 在执行时，检测是否有按键按下，有按下则返回非0值，没有按下则返回0，是非阻塞函数；*/
int kbhit()
{
    unsigned char ch;
    int nread;

    if (peek_character != -1)
        return 1;
    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0, &ch, 1);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    if (nread == 1)
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}

int readch()
{
    char ch;

    if (peek_character != -1)
    {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    int ret = read(0, &ch, 1);
    if (ret == 0)
        return 0;
    return ch;
}

int main()
{
    init_keyboard();
    while (1)
    {
        kbhit();
        printf("\n%d\n", readch());
    }
    close_keyboard();
    return 0;
}