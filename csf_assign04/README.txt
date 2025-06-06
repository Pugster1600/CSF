CONTRIBUTIONS
Johnny Shi and Matthew Schricker
TODO: write a brief summary of how each team member contributed to
the project.

Matthew wrote most of the code 
Johnny wrote the README

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.

Part 1:
Wrote 16777216 bytes to '/tmp/jshi61/data_16M.in'
Test run with threshold 2097152 - 1

real    0m0.387s
user    0m0.373s
sys     0m0.013s
Test run with threshold 1048576 - 2

real    0m0.245s
user    0m0.387s
sys     0m0.040s
Test run with threshold 524288 - 4

real    0m0.186s
user    0m0.464s
sys     0m0.043s
Test run with threshold 262144 - 8

real    0m0.145s
user    0m0.457s
sys     0m0.057s
Test run with threshold 131072 - 16

real    0m0.145s
user    0m0.469s
sys     0m0.075s
Test run with threshold 65536 - 32

real    0m0.129s
user    0m0.477s
sys     0m0.092s
Test run with threshold 32768 - 64

real    0m0.141s
user    0m0.510s
sys     0m0.121s
Test run with threshold 16384 - 128

real    0m0.160s
user    0m0.550s
sys     0m0.177s

Part 2:
When we fork multiple processes, the OS kernel determines which cores execute which child process. 
These cores execute in parallel but also require overhead from the OS kernel to create extra processes and context switch when cores fewer than processes.

We saw the results we did because as the number of child processes increased beyond the cores available, the overhead time required (such as creating the new process and 
context switch to different processes due to more processes than cores) was greater than the time saved from truly parallel execution. 
Additional processes are created until the amount of data is less than or equal to the parition size.
As we reduced the threshold size from 2097152 to 16384 (thus increasing number of processes), performance improved until around theshold 2621444. 
Thresholds smaller than 262144 saw diminishing returns. When we dive deeper, we see that we have 8 processes running at threshold 262144, and we also happen to have 8 cores on a ugrad machine. 
This suggests that when we increase the process count and when we have fewer processes than cores, the time saved from parallel sorting is worth 
the extra overhead required from the OS kernel to create extra processes and assign processes to cores.
However, when we have more processes than cores, the cores need to concurrently execute tasks (rather than purely parallel execution) and context switch between the processes.
The combination of additional overhead with context switches and not truly parallel execution is the reason we saw the results we did. 
