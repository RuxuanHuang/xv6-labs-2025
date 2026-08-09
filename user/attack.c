#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  int size = 4096;  //  页大小
  char* mem = sbrk(size);   //通过sbrk分配内存

  if (mem == (char*)-1) {   //分配失败的情况
    exit(1);
  }

  //扫描并试图找寻
  //出现This may help.内容，锁定其首地址偏移16的位置
  int i = 0;
  while (i < size) {
    if (mem[i] == 'T') {
      if (!strcmp(&mem[i], "This may help.")&&i+16<size) {
        printf("%s", &mem[i + 16]);
      }
    }
    i++;
  }

  exit(0);
}
