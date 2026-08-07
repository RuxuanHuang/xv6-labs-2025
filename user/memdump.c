#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  // 用局部指针 p 遍历 data，根据 fmt 中的格式字符决定如何解释内存
  char* p = data;

  for (int i = 0; fmt[i]; i++) {
    switch (fmt[i]) {
      case 'i':
        // i: 接下来 4 字节作为 32 位有符号整数，十进制输出
        printf("%d\n", *(int*)p);
        p += 4;
        break;

      case 'p': {
        // p: 接下来 8 字节作为 64 位整数，十六进制输出
        uint64 v = *(uint64*)p;
        int started = 0;
        
        // 从最高位到最低位，每4位一个十六进制数字
        for (int j = 28; j >= 0; j -= 4) {
          int digit = (v >> j) & 0xF;
          // 跳过前导零，但至少输出一位（当值为0时）
          if (digit || started || j == 0) {
            started = 1;
            if (digit < 10)
              printf("%c", '0' + digit);
            else
              printf("%c", 'A' + digit - 10);
          }
        }
        printf("\n");
        p += 8;
        break;
      }

      case 'h':
        // h: 接下来 2 字节作为 16 位有符号整数，十进制输出
        printf("%d\n", *(short*)p);
        p += 2;
        break;

      case 'c':
        // c: 接下来 1 字节作为 ASCII 字符输出
        printf("%c\n", *p);
        p += 1;
        break;

      case 's':
        // s: 接下来 8 字节是一个指向 C 字符串的 64 位指针，打印该字符串
        printf("%s\n", *(char**)p);
        p += 8;
        break;

      case 'S':
        // S: 剩余所有数据视为以 \0 结尾的 C 字符串输出
        printf("%s\n", p);
        return;  

      default:
        break;
    }
  }

}
