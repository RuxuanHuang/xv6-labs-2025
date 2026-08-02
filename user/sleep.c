#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    // 判断命令行参数个数，必须传入1个数字，否则报错退出
    if (argc != 2) {
        printf("Usage: sleep ticks\n");
        exit(1);
    }

    int target_ticks = atoi(argv[1]);
    int start_tick = uptime();

    while (uptime() - start_tick < target_ticks) {
        pause(0);
    }

    exit(0);
}
