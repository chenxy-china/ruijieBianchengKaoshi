
#include <stdio.h>
#include <termios.h>

int main()
{
    struct termios new_setting, init_setting;
    char pwd[16];

    tcgetattr(0, &init_setting);
    new_setting = init_setting;
    // get termios setting and save it
    new_setting.c_lflag &= ~ECHO; // 关闭回显
    tcsetattr(0, TCSANOW, &new_setting);

    printf("please enter password: \n");
    int ret = scanf("%s", pwd);
    if (ret == 0)
        return -1;
    printf("you enter %s\n", pwd);

    // restore the setting
    tcsetattr(0, TCSANOW, &init_setting);
    return 0;
}