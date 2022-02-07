#include <stdio.h>
#include <stdlib.h>   // exit
#include <sys/wait.h> // wait
#include <unistd.h>   // fork

int main() {
  int rc = fork();
  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    pid_t wait_return = wait(NULL);
    printf("hello, I am child (pid:%d)\n", (int) getpid());
    printf("child wait return: %d\n", wait_return);
  } else {
    pid_t wait_return = wait(NULL);
    printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    printf("parent wait return: %d\n", wait_return);
  }
  return 0;
}