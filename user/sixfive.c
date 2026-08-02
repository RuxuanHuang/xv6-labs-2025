#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void deal_with_current(int* len,char *buf)
{
    if (*len > 0) {
        buf[*len] = '\0';
        // 将字符串转为整数
        int num = 0;
        for (int i = 0; i < *len; i++) {
            num = num * 10 + (buf[i] - '0');
        }
        // 判断是否是 5 或 6 的倍数
        if (num % 5 == 0 || num % 6 == 0) {
            printf("%d\n", num);
        }
        *len = 0;   // 重置长度
    }
}

int main(int argc, char *argv[])
{
    // 检查参数：需要一个文件名
    if (argc < 2) {
        printf("Usage: sixfive <Filename1> [Filename2...]\n");
        exit(1);
    }

    for(int i=1;i<argc;i++){
        // 用 open 系统调用打开文件，只读模式
        int fd = open(argv[i], 0);
        if (fd < 0) {
            printf("sixfive: Fail to open the file %s\n", argv[1]);
            continue;
        }

        char *seps = " -\r\t\n./,";
        char buf[1024];      // 暂存当前正在读取的数字字符串
        int  len = 0;      // 当前数字字符串的长度
        int  in_num = 1;    // in_num 标记"已遇到分隔符，可以开始接收数字"
        char c;            // 当前读取的单个字符
        int  n;            // read 返回的字节数

        // 逐字符读取文件
        while ((n = read(fd, &c, 1)) > 0) {
            if (c >= '0' && c <= '9') {
                if (in_num) {
                    buf[len++] = c;
                }
            } 
            else if (strchr(seps, c)) {
                deal_with_current(&len,buf);
                in_num = 1;   // 分隔符之后，下一个数字是合法的
            } 
            else {
                len = 0;
                in_num = 0;
            }
        }
        // 处理文件末尾可能残留的最后一个数字
        deal_with_current(&len,buf);
        close(fd);
    }
    exit(0);
}