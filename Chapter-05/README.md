# Interlude: Process API

## Homework (Simulation)

This simulation homework focuses on `fork.py`, a simple process creation simulator that shows how processes are related in a single “familial” tree. Read the relevant README for details about how to run the simulator

### Questions

1. Run `./fork.py -s 10` and see which actions are taken. Can you predict what the process tree looks like at each step? Use the `-c` flag to check your answers. Try some different random seeds(`-s`) or add more actions(`-a`) to get the hang of it.

   Answer: done.

2. One control the simulator gives you is the `fork_percentage`, controlled by the `-f` flag. The higher it is, the more likely the next action is a fork; the lower it is, the more likely the action is an exit. Run the simulator with a large number of actions (e.g., `-a 100`) and vary the fork percentage from 0.1 to 0.9. What do youthink the resulting final process trees will look like as the percentage changes? Check your answer with `-c`.

   Answer: done.

3. Now, switch the output by using the `-t` flag (e.g., run `./fork.py -t`). Given a set of process trees, can you tell which actions were taken?

   Answer: done.

4. One interesting thing to note is what happens when a child exits; what happens to its children in the process tree? To study this, let’s create a specific example: `./fork.py -A a+b,b+c,c+d,c+e,c-`. This example has process ’a’ create ’b’, which in turn creates ’c’,which then creates ’d’ and ’e’. However, then, ’c’ exits. What do you think the process tree should like after the exit? What if you use the `-R` flag? Learn more about what happens to orphaned processes on your own to add more context.

   ```shell
                              Process Tree:
                                  a
   
   Action: a forks b
                                  a
                                  └── b
   Action: b forks c
                                  a
                                  └── b
                                      └── c
   Action: c forks d
                                  a
                                  └── b
                                      └── c
                                          └── d
   Action: c forks e
                                  a
                                  └── b
                                      └── c
                                          ├── d
                                          └── e
   Action: c EXITS
                                  a
                                  ├── b
                                  ├── d
                                  └── e
   ```

   Answer: after remove c, d and e become the children of a.

5. One last flag to explore is the `-F` flag, which skips intermediate steps and only asks to fill in the final process tree. Run `./fork.py -F` and see if you can write down the final tree by looking at the series of actions generated. Use different random seeds to try this a few times.

   Answer: done.

6. Finally, use both `-t` and `-F` together. This shows the final process tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place? In which cases can you tell? In which can’t you tell? Try some different random seeds to delve into this question.

   Answer: done.

   

## Homework (Code)

In this homework, you are to gain some familiarity with the process management APIs about which you just read. Don’t worry – it’s even more fun than it sounds! You’ll in general be much better off if you find as much time as you can to write some code, so why not start now?



### Questions

1. Write a program that calls `fork()`. Before calling `fork()`, have the main process access a variable (e.g., `x`) and set its value to something (e.g., `100`). What value is the variable in the child process? What happens to the variable when both the child and parent change the value of `x`?

   ```c
   #include <stdio.h>
   #include <stdlib.h> // exit
   #include <sys/wait.h>
   #include <unistd.h> // fork
   
   int main() {
     int x = 100;
     int rc = fork();
     if (rc < 0) {
       // fork failed; exit
       fprintf(stderr, "fork failed\n");
       exit(EXIT_FAILURE);
     } else if (rc == 0) {
       // child process
       x = 101;
       printf("x in child process: %d\n", x);
     } else {
       // parent process
       x = 102;
       printf("x in parent process: %d\n", x);
       wait(NULL);
     }
     return 0;
   }
   ```

   Output:

   ```shell
   x in parent process: 102
   x in child process: 101
   ```

   The x in son process is the copy of x of parent process, both change will success.

2. Write a program that opens a file (with the `open()` system call) and then calls `fork()` to create a new process. Can both the child and parent access the file descriptor returned by `open()`? What happens when they are writing to the file concurrently, i.e., at the same time?

   ```c
   #include <stdio.h>  // fread, fopen, fclose
   #include <stdlib.h> // exit
   #include <sys/wait.h>
   #include <unistd.h> // fork
   
   #define errExit(msg)                                                           \
     do {                                                                         \
       perror(msg);                                                               \
       exit(EXIT_FAILURE);                                                        \
     } while (0)
   
   static void write_to_file(FILE *f, char *str) { // APUE 8.9
     char *ptr;
     int c;
     for (ptr = str; (c = *ptr++) != 0;) {
       if (fputc(c, f) != c)
         errExit("fputc");
       if (fflush(f) == EOF)
         errExit("fflush");
     }
   }
   
   int main() {
     // open the file
     FILE *f = fopen("./2.txt", "w+");
     if (f == NULL)
       errExit("fopen");
   
     pid_t cpid = fork();
     if (cpid < 0)
       errExit("fork");
     else if (cpid == 0) {
       // child process
       write_to_file(f, "child says hello.\n");
     } else {
       // parent process
       write_to_file(f, "parent says goodbye.\n");
   
       if (wait(NULL) == -1)
         errExit("wait");
   
       char buf[BUFSIZ];
       printf("file contents:\n");
       if (fseek(f, 0, 0) == -1)
         errExit("fseek");
       fread(buf, BUFSIZ, 1, f);
       printf("%s", buf);
       fclose(f);
     }
     return 0;
   }
   ```

   shell output:

   ```shell
   file contents:
   parent says goodbye.
   child says hello.
   ```

   2.txt content:

   ```shell
   parent says goodbye.
   child says hello.
   ```

   Answer: son and parent process both can access the same file descriptor, the final result is up to the system scheduler.

3. Write another program using `fork()`. The child process should print “hello”; the parent process should print “goodbye”. You should try to ensure that the child process always prints first; can you do this *without* calling **wait()** in the parent?

   Answer: can use vfork, kill, pipe

4. Write a program that calls `fork()` and then calls some form of `exec()` to run the program `/bin/ls`. See if you can try all of the variants of `exec()`, including(on Linux)`execl()`, `execle()`, `execlp()`, `execv()`, `execvp()`, and `execvpe()`. Why do you think there are so many variants of the same basic call?

   ```c
   #define _GNU_SOURCE // for execvpe()
   #if __has_include(<features.h>)
   #include <features.h>
   #endif
   #include <fcntl.h>  // open
   #include <stdio.h>  // perror
   #include <stdlib.h> // exit
   #include <sys/wait.h>
   #include <unistd.h> // fork, exec*, fexecve
   
   #define errExit(msg)                                                           \
     do {                                                                         \
       perror(msg);                                                               \
       exit(EXIT_FAILURE);                                                        \
     } while (0)
   
   int main() { // APUE 8.10
     char *argv[] = {"ls", "-l", "-a", "-h", (char *)NULL};
     char *envp[] = {"PATH=/bin:/usr/bin", NULL};
   
     pid_t cpid = fork();
     if (cpid == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("execl:\n");
       if (execl("/bin/ls", "ls", "-l", "-a", "-h", (char *)NULL) == -1)
         errExit("execl");
     }
   
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nexeclp:\n");
       if (execlp("ls", "ls", "-l", "-a", "-h", (char *)NULL) == -1)
         errExit("execlp");
     }
   
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nexecle:\n");
       if (execle("/bin/ls", "ls", "-l", "-a", "-h", (char *)NULL, envp) == -1)
         errExit("execle");
     }
   
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nexecv:\n");
       if (execv("/bin/ls", argv) == -1)
         errExit("execv");
     }
   
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nexecvp:\n");
       if (execvp("ls", argv) == -1)
         errExit("execvp");
     }
   
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nexecve:\n");
       if (execve("/bin/ls", argv, envp) == -1) // system call
         errExit("execve");
     }
   
   #ifdef __GNU_LIBRARY__
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nexecvpe:\n");
       if (execvpe("ls", argv, envp) == -1)
         errExit("execvpe");
     }
   
     if (wait(NULL) == -1)
       errExit("wait");
     if ((cpid = fork()) == -1)
       errExit("fork");
     else if (cpid == 0) {
       printf("\nfexecve:\n");
       int fd = 0;
       if ((fd = open("/bin/ls", O_PATH)) == -1)
         errExit("open");
       if (fexecve(fd, argv, envp) == -1)
         errExit("fexecve");
     }
   #endif
   
     return 0;
   }
   ```

   

5. Now write a program that uses `wait()` to wait for the child process to finish in the parent. What does `wait()` return? What happens if you use `wait()` in the child?

   ```c
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
   ```

   - wait, if success, return the pid of terminated child process; if fail, return -1
   - will return -1

6. Write a slight modification of the previous program,this time using `waitpid()` instead of wait(). When would `waitpid()` be useful?

   ```c
   #include <stdio.h>
   #include <stdlib.h>   // exit
   #include <sys/wait.h> // waitpid
   #include <unistd.h>   // fork
   
   int main() {
     int wstatus;
     pid_t rc = fork();
     if (rc < 0) {
       // fork failed; exit
       fprintf(stderr, "fork failed\n");
       exit(EXIT_FAILURE);
     } else if (rc == 0) {
       pid_t waitpid_return = waitpid(-1, NULL, WUNTRACED | WCONTINUED);
       printf("child wait return: %d\n", waitpid_return);
     } else {
       pid_t waitpid_return = waitpid(rc, &wstatus, WUNTRACED | WCONTINUED);
       printf("parent wait return: %d\n", waitpid_return);
       printf("if the child terminated normally: %s\n",
              WIFEXITED(wstatus) ? "true" : "false");
     }
     return 0;
   }
   ```

   

7. Write a program that creates a child process, and then in the child closes standard output (`STDOUT_FILENO`). What happens if the child calls `printf()` to print some output after closing the descriptor?

   ```shell
   #include <stdio.h>
   #include <stdlib.h> // exit
   #include <unistd.h> // fork
   #include <sys/wait.h>
   
   int main() {
     pid_t rc = fork();
     if (rc < 0) {
       // fork failed; exit
       fprintf(stderr, "fork failed\n");
       exit(EXIT_FAILURE);
     } else if (rc == 0) {
       close(STDOUT_FILENO);
       printf("child calls printf().\n");
     } else {
       wait(NULL);
       printf("parent calls printf().\n");
     }
     return 0;
   }
   ```

   shell output:

   ```shell
   parent calls printf().
   ```

   Answer: the printf of the cild process has no output.

8. Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the `pipe()` system call.

   ```c
   #include<stdio.h>
   #include<unistd.h>
   #include<stdlib.h>
   
   int main() {
   	int pi[2];
   	int p = pipe(pi);
   	if(p < 0) {
   		fprintf(stderr, "pipe failed");
   		exit(1);
   	}
   	int i = 0;
   	int rc[2];
   	char buf[256];
   	for(i=0;i<2;++i) {
   		rc[i] = fork();
   		if (rc[i] < 0) {
   			fprintf(stderr, "fork failed");
   			exit(1);
   		} else if (rc[i] == 0) {
   			switch(i) {
   			case 0:
   				dup2(pi[1], STDOUT_FILENO);
   				puts("child input");
   				break;
   			case 1:
   				dup2(pi[0], STDIN_FILENO);
   				gets(buf);
   				printf("child0 out (%s) in the child1\n", buf);
   				return 2;
   			}
   			break;
   		}
   	}
   	waitpid(rc[0], NULL, 0);
   	waitpid(rc[1], NULL, 0);
   	return 0;
   }
   ```

   

