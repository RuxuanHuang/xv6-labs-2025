#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

// exec_mode为0则打印路径，为1则执行命令
// cmd为命令路径，cmd_arg为命令参数数组，cmd_argc是参数个数
void find(char* path, char* target,int exec_mode,char*cmd,char**cmd_args,int cmd_argc)
{
  char buf[512], * p;
  int fd;
  struct dirent de;
  struct stat st;

  // 打开路径
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // 获取文件状态信息
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // 不是目录直接返回
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  // 检查路径长度
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    printf("find: path too long\n");
    close(fd);
    return;
  }

  // 路径拼接
  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  // 逐项读取
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    // 跳过无效目录项
    if (de.inum == 0)
      continue;

    // 在buf拼接完整路径path/filename
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    // 获取该目录项状态
    if (stat(buf, &st) < 0) {
      printf("find: cannot stat %s\n", buf);
      continue;
    }

    // 递归查找
    if (st.type == T_DIR) {
      if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
        find(buf, target,exec_mode,cmd,cmd_args,cmd_argc);
      }
    }

    // 文件名匹配
    if (strcmp(de.name, target) == 0&&st.type!=T_DIR) {
      if(exec_mode){
        char *new_argv[MAXARG];
        int i;
        for(i=0;i<cmd_argc;i++){
          new_argv[i]=cmd_args[i];
        }
        new_argv[i]=buf; //文件路径
        new_argv[i+1]=0;

        //fork子进程执行命令
        if(fork()==0){
          exec(cmd,new_argv);
          fprintf(2,"find: cannot execute %s\n",cmd);
          exit(1);
        }

        //父进程等待
        wait(0);
      }
      else{
        printf("%s\n", buf);
      } 
    }
    
  }

  close(fd);
}

int
main(int argc, char* argv[])
{
  if (argc < 3) {
    fprintf(2, "usage: find <dir> <filename>[-exec <cmd> [args...]]\n");
    exit(1);
  }

  int exec_mode=0;
  char *cmd=0;
  char **cmd_args=0;
  int cmd_argc=0;

  if(argc>3 && strcmp(argv[3],"-exec")==0){
    exec_mode=1;
    if(argc<5){
      fprintf(2,"find: -exec lacks command\n");
    }
    cmd=argv[4];
    cmd_argc=argc-4;
    cmd_args=argv+4;
  }

  find(argv[1], argv[2],exec_mode,cmd,cmd_args,cmd_argc);
  exit(0);
}
