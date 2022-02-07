#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
   int i;
   for(i=0; i<2; i++){
      fork();
      // printf("-");
      printf("ppid=%d, pid=%d, i=%d \n", getppid(), getpid(), i);
   }
   sleep(10);
   // wait(NULL);
   // wait(NULL);

   // print -------- 8个
   return 0;
}