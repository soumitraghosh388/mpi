Problem 1B (N Queens Problem)

Input format : command line argument
Compilation step : mpicc 1B.c
Execution step : mpiexec -np 1 ./a.out 4
Bugs : working for all N for numprocess = 1. For N not divisible by the no. of processes, it is not working. For numprocess = N, it works fine from 1 to 5, but for 6 above it shows issues with memory  allocation.

Time Complexity : N + N + (N * N)/P + communication_time
Message Complexity : O(1)
Space Complexity : N * N * N + N * N = O(N^3) (there is a scope to improve it to O(N^2))
Performance scaling : For N = 10, for P=1 and P=10 the real time is 0.084 and 0.067 secs. Similar decreasing trend has been found for other N values as well.


Problem 2 (Floyd-Warshall algorithm)

Input format : 2_input.in
Compilation step : mpicc 2.c
Execution step : mpiexec -np 1 ./a.out
Bugs : Working for P<=N, but failing for P>N cases.

Time Complexity : N * N + (N * N * N) / P + communication_time
Message Complexity : N * n_row_per_proc * P + N * (N + N * n_row_per_proc + N * n_row_per_proc)
Space Complexity : N * N + N * n_row_per_proc + N * n_row_per_proc + N = O(N^2)
Performance scaling : For N = 5, for P = 1 to 3, real time 0.056, 0.053, 0.044 secs respectively. then from 5 onwards it increased from 0.046 to 0.064 for P=10. This increase is due to the fact that communication overtakes computation.


Problem 3 (Game of Life)

Input file : 3_input.in
Compilation step : mpicc 3.c
Execution step : mpiexec -np 1 ./a.out
Bugs : working for all T generations and for processes 1, 2, 3, 5 and 10. For the processes 4, 6, 7, 8, 9 and 11 onwards its getting stuck or showing memory leak. The probable reason for this is that when no. of rows is not divisible by the no. of processes or when the no. of processes is greater than the no. of rows. In 2nd case we can abort the extra processes as one of the solution.

Time Complexity : N * M + T * ((N * M * 8 * c) / P) + communication_time (where N = rows, M = cols, T = generations, P = no. of process, c = constant)
Message Complexity : N * M + n_row_per_proc * M + N * M  = 3 * N * M (where n_row_per_proc = N/numprocess) 
Space Complexity : N * M + n_row_per_proc * M
Performance scaling : for N = M = 10, T = 7 , the real time for np = 1, 2, 3, 5 and 10 are 0.051, 0.048, 0.048, 0.039 and 0.047 secs respectively. The increase in time from np = 5 to 10 is due to the fact that communication overtakes computation.

Note : All experiments have been done in single node due to resource constraint but can be explored to multiple node with varying no. of processes per node parameter. 


