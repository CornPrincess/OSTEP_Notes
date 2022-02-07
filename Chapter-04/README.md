# The Abstraction: The Process

## Overview

This program, called process-run.py, allows you to see how the state of a process state changes as it runs on a CPU. As described in the chapter, processes can be in a few different states:

```
RUNNING - the process is using the CPU right now
READY   - the process could be using the CPU right now
          but (alas) some other process is
WAITING - the process is waiting on I/O
          (e.g., it issued a request to a disk)
DONE    - the process is finished executing
```

## Homework (Simulation)

This program, `process-run.py`, allows you to see how process states change as programs run and either use the CPU (e.g., perform an addin struction) or do I/O (e.g., send a request to a disk and wait for it to complete). See the README for details.

### Questions

1. Run `process-run.py` with the following flags: `-l 5:100,5:100`. What should the CPU utilization be (e.g., the percent of time the CPU is in use?) Why do you know this? Use the `-c` and `-p` flags tosee if you were right.

   Answer: 100%, because there is no io process.

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 5:100,5:100 -c -p
   Time        PID: 0        PID: 1           CPU           IOs
     1        RUN:cpu         READY             1
     2        RUN:cpu         READY             1
     3        RUN:cpu         READY             1
     4        RUN:cpu         READY             1
     5        RUN:cpu         READY             1
     6           DONE       RUN:cpu             1
     7           DONE       RUN:cpu             1
     8           DONE       RUN:cpu             1
     9           DONE       RUN:cpu             1
    10           DONE       RUN:cpu             1
   
   Stats: Total Time 10
   Stats: CPU Busy 10 (100.00%)
   Stats: IO Busy  0 (0.00%)2.
   ```

2. Now run with these flags: `./process-run.py -l 4:100,1:0`.These flags specify one process with 4 instructions (all to use the CPU), and one that simply issues an I/O and waits for it to be done.How long does it take to complete both processes? Use `-c` and `-p` to find out if you were right.

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 4:100,1:0 -c -p
   Time        PID: 0        PID: 1           CPU           IOs
     1        RUN:cpu         READY             1
     2        RUN:cpu         READY             1
     3        RUN:cpu         READY             1
     4        RUN:cpu         READY             1
     5           DONE        RUN:io             1
     6           DONE       WAITING                           1
     7           DONE       WAITING                           1
     8           DONE       WAITING                           1
     9           DONE       WAITING                           1
    10           DONE       WAITING                           1
    11*          DONE   RUN:io_done             1
   
   Stats: Total Time 11
   Stats: CPU Busy 6 (54.55%)
   Stats: IO Busy  5 (45.45%)
   ```
   
   Answer: PID0: 4, PID1:7, total cost 11
3. Switch the order of the processes: `-l 1:0,4:100`. What happens now? Does switching the order matter? Why? (As always, use `-c` and `-p` to see if you were right) 

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 1:0,4:100 -c -p
   Time        PID: 0        PID: 1           CPU           IOs
     1         RUN:io         READY             1
     2        WAITING       RUN:cpu             1             1
     3        WAITING       RUN:cpu             1             1
     4        WAITING       RUN:cpu             1             1
     5        WAITING       RUN:cpu             1             1
     6        WAITING          DONE                           1
     7*   RUN:io_done          DONE             1
   
   Stats: Total Time 7
   Stats: CPU Busy 6 (85.71%)
   Stats: IO Busy  5 (71.43%)
   ```

   Answer: total time is 7, the order is mater, now process 1 runs when process 0 is waiting for IO completes, it saves time.

4. We’ll now explore some of the other flags. One important flag is `-S`, which determines how the system reacts when a process issues an I/O. With the flag set to `SWITCH_ON_END`, the system will NOT switch to another process while one is doing I/O, instead waiting until the process is completely finished. What happens when you run the following two processes (`-l 1:0,4:100 -c -S SWITCH_ON_END`), one doing I/O and the other doing CPU work?

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 1:0,4:100 -c -p -S SWITCH_ON_END
   Time        PID: 0        PID: 1           CPU           IOs
     1         RUN:io         READY             1
     2        WAITING         READY                           1
     3        WAITING         READY                           1
     4        WAITING         READY                           1
     5        WAITING         READY                           1
     6        WAITING         READY                           1
     7*   RUN:io_done         READY             1
     8           DONE       RUN:cpu             1
     9           DONE       RUN:cpu             1
    10           DONE       RUN:cpu             1
    11           DONE       RUN:cpu             1
   
   Stats: Total Time 11
   Stats: CPU Busy 6 (54.55%)
   Stats: IO Busy  5 (45.45%)
   ```

   Answer: process 1 will not run when process0 is waiting.

5. Now, run the same processes, but with the switching behavior set to switch to another process whenever one is WAITING for I/O (`-l 1:0,4:100 -c -S SWITCH_ON_IO`). What happens now? Use `-c` and `-p` to confirm that you are right.

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 1:0,4:100 -c -p -S SWITCH_ON_IO
   Time        PID: 0        PID: 1           CPU           IOs
     1         RUN:io         READY             1
     2        WAITING       RUN:cpu             1             1
     3        WAITING       RUN:cpu             1             1
     4        WAITING       RUN:cpu             1             1
     5        WAITING       RUN:cpu             1             1
     6        WAITING          DONE                           1
     7*   RUN:io_done          DONE             1
   
   Stats: Total Time 7
   Stats: CPU Busy 6 (85.71%)
   Stats: IO Busy  5 (71.43%)
   ```

   Answer: process 1 will run when process 0 is waiting for IO.

6. One other important behavior is what to do when an I/O completes. With `-I IO_RUN_LATER`, when an I/O completes, the process that issued it is not necessarily run right away; rather, whatever was running at the time keeps running. What happens when you run this combination of processes? (Run `./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p`) Are system resources being effectively utilized?

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p
   Time        PID: 0        PID: 1        PID: 2        PID: 3           CPU           IOs
     1         RUN:io         READY         READY         READY             1
     2        WAITING       RUN:cpu         READY         READY             1             1
     3        WAITING       RUN:cpu         READY         READY             1             1
     4        WAITING       RUN:cpu         READY         READY             1             1
     5        WAITING       RUN:cpu         READY         READY             1             1
     6        WAITING       RUN:cpu         READY         READY             1             1
     7*         READY          DONE       RUN:cpu         READY             1
     8          READY          DONE       RUN:cpu         READY             1
     9          READY          DONE       RUN:cpu         READY             1
    10          READY          DONE       RUN:cpu         READY             1
    11          READY          DONE       RUN:cpu         READY             1
    12          READY          DONE          DONE       RUN:cpu             1
    13          READY          DONE          DONE       RUN:cpu             1
    14          READY          DONE          DONE       RUN:cpu             1
    15          READY          DONE          DONE       RUN:cpu             1
    16          READY          DONE          DONE       RUN:cpu             1
    17    RUN:io_done          DONE          DONE          DONE             1
    18         RUN:io          DONE          DONE          DONE             1
    19        WAITING          DONE          DONE          DONE                           1
    20        WAITING          DONE          DONE          DONE                           1
    21        WAITING          DONE          DONE          DONE                           1
    22        WAITING          DONE          DONE          DONE                           1
    23        WAITING          DONE          DONE          DONE                           1
    24*   RUN:io_done          DONE          DONE          DONE             1
    25         RUN:io          DONE          DONE          DONE             1
    26        WAITING          DONE          DONE          DONE                           1
    27        WAITING          DONE          DONE          DONE                           1
    28        WAITING          DONE          DONE          DONE                           1
    29        WAITING          DONE          DONE          DONE                           1
    30        WAITING          DONE          DONE          DONE                           1
    31*   RUN:io_done          DONE          DONE          DONE             1
   
   Stats: Total Time 31
   Stats: CPU Busy 21 (67.74%)
   Stats: IO Busy  15 (48.39%)
   ```

   Answer: this way is not effecitve. process1 run the first IO, then waits other process done to runs the remain IO.

7. Now run the same processes, but with `-I IO_RUN_IMMEDIATE` set, which immediately runs the process that issued the I/O. How does this behavior differ? Why might running a process that just completed an I/O again be a good idea?

   ```shell
   zhoutianbin@1L-ZHOUTIANBIN Chapter-04 % python process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_IMMEDIATE -c -p
   Time        PID: 0        PID: 1        PID: 2        PID: 3           CPU           IOs
     1         RUN:io         READY         READY         READY             1
     2        WAITING       RUN:cpu         READY         READY             1             1
     3        WAITING       RUN:cpu         READY         READY             1             1
     4        WAITING       RUN:cpu         READY         READY             1             1
     5        WAITING       RUN:cpu         READY         READY             1             1
     6        WAITING       RUN:cpu         READY         READY             1             1
     7*   RUN:io_done          DONE         READY         READY             1
     8         RUN:io          DONE         READY         READY             1
     9        WAITING          DONE       RUN:cpu         READY             1             1
    10        WAITING          DONE       RUN:cpu         READY             1             1
    11        WAITING          DONE       RUN:cpu         READY             1             1
    12        WAITING          DONE       RUN:cpu         READY             1             1
    13        WAITING          DONE       RUN:cpu         READY             1             1
    14*   RUN:io_done          DONE          DONE         READY             1
    15         RUN:io          DONE          DONE         READY             1
    16        WAITING          DONE          DONE       RUN:cpu             1             1
    17        WAITING          DONE          DONE       RUN:cpu             1             1
    18        WAITING          DONE          DONE       RUN:cpu             1             1
    19        WAITING          DONE          DONE       RUN:cpu             1             1
    20        WAITING          DONE          DONE       RUN:cpu             1             1
    21*   RUN:io_done          DONE          DONE          DONE             1
   
   Stats: Total Time 21
   Stats: CPU Busy 21 (100.00%)
   Stats: IO Busy  15 (71.43%)
   ```

   Answer: IO_RUN_IMMEDIATE is a good way, when process1 is waiting, other process can run.

8. Now run with some randomly generated processes: `-s 1 -l 3:50,3:50` or `-s 2 -l 3:50,3:50` or `-s 3 -l 3:50,3:50`. See if you can predict how the trace will turn out. What happens when you use the flag `-I IO_RUN_IMMEDIATE` vs. `-I IO_RUN_LATER`? What happens when you use `-S SWITCH_ON_IO` vs. `-S SWITCH_ON_END`?