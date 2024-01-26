#include "mpi.h"
#include <math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>

int get_array_loc(int, int, int);
void set_array_value(int, int, int *, int);
void DFS_traverse(int **, int *, int, int);
void update_array(int *, int, int, int);
int get_avail_pos(int *, int, int);
void get_children(int, int *, int *, int);
void mem_copy(int *, int *, int, int);
void display(int, int, int, int *, int);

int n_ways_local = 0;
int prev_depth = 0;

int main(int argc, char *argv[])
{
	int myid, numprocs, tag = 1, sel_value = 0, N, n_col_per_proc = 0, n_ways = 0;
	double tt0, ttf;
	int *a, *a_bck, *sel, i = 0, n, fst_row_tr_count = 0;
	int **a_bck_array;
	//bool backtrack = false;
	

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	
	
	if (myid == 0)
		N = strtol(argv[1], NULL, 10);

	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//printf("[%d]>%d \n", myid, N);
	a_bck_array = (int**)calloc(N, sizeof(int));
	for (int i = 0; i<N;i++)
		a_bck_array[i] = (int*)calloc(N*N, sizeof(int));
    	a = (int*)calloc(N*N, sizeof(int));
	n = N/2;
	
	if (N % numprocs == 0)
		n_col_per_proc = N/numprocs;
	else
	{
		n_col_per_proc = N/numprocs + 1;
	}
	for (int j = 0; j < N; j++)
	{
		if  (j >= myid*n_col_per_proc && j < (myid+1)*n_col_per_proc)
			continue;
		else
			a[get_array_loc(N, 0, j)] = 1;
	}
	//display(N, N, myid, a, numprocs);
	
	for(int i = myid*n_col_per_proc; i < (myid+1)*n_col_per_proc && i < N; i++)
	{
		if(a[i] == 0)
			DFS_traverse(a_bck_array, a, i, N);
	}
	//printf("[%d]>n_ways_local : %d\n", myid, n_ways_local);
	MPI_Reduce(&n_ways_local, &n_ways, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (myid == 0) printf("Total no. of ways for %d queens : %d\n", N, n_ways);
	if(a!=NULL) free(a);
	for (int i = 0; i<N;i++)
		if(a_bck_array[i]!=NULL) free(a_bck_array[i]);
	if(a_bck_array!=NULL) free(a_bck_array);
	MPI_Finalize();
	return 0;
}

void DFS_traverse(int **a_bck_array, int *a, int v, int N)
{
	int col = v%N;
	int min_pos, max_pos;
	int cur_depth = v/N;
	prev_depth = cur_depth;

	a[v] = 1;
	//memcpy(a_bck_array[cur_depth], a, N*N*sizeof(int));
	mem_copy(a_bck_array[cur_depth], a, 0, N*N-1);

	for(int i = 0;i<N;i++)
	{
        	if(!a_bck_array[i])
        	{
            		printf("memory not allocated for %d : \n", i);
            		exit(0);
            	}
	}

	get_children(v, &min_pos, &max_pos, N);
	if (min_pos == -1)
	{
		n_ways_local++;
	}
	else
	{
        	bool ch_not_visited = false;
		update_array(a, col, cur_depth, N);
		for (int i=min_pos;i<=max_pos;i++)
		{
			if(a[i] == 0)
			{
				ch_not_visited = true;
				DFS_traverse(a_bck_array, a, i, N);
	    		}
		}
		if (!ch_not_visited)
            		//memcpy(a, a_bck_array[cur_depth], N*N*sizeof(int));
            		mem_copy(a, a_bck_array[cur_depth], 0, N*N-1);
		if (prev_depth > cur_depth)
		{
		    //memcpy(a, a_bck_array[cur_depth], N*N*sizeof(int));
		    mem_copy(a, a_bck_array[cur_depth], 0, N*N-1);
		    prev_depth = cur_depth;
		}
	}

}


void set_array_value(int N, int M, int *a, int value)
{
	for (int i=0;i<N;i++){
		for (int j = 0; j < M; j++) {
		    a[get_array_loc(M, i, j)] = value;
		}
	}
}

int get_array_loc(int M, int i, int j)
{
	return M * i + j;
}

void update_array(int *a, int col, int i, int N)
{
	int gap = 0;
	for (int row = i+1; row<N;row++)
	{
		gap++;
		a[get_array_loc(N, row, col)] = 1;
		if ((col - gap) >= 0)
			a[get_array_loc(N, row, col-gap)] = 1;
		if ((col + gap) < N)
			a[get_array_loc(N, row, col+gap)] = 1;
	}
}

int get_avail_pos(int *a, int row, int N)
{
	for (int j = 0;j<N;j++)
	{
		if (a[get_array_loc(N, row, j)] == 1)
			return j;
	}
	return -1;
}

void get_children(int i, int *min_pos, int *max_pos, int N)
{
	int row = i/N + 1;
	if (row >= N)
	{
		*min_pos = -1;
		*max_pos = -2;
	}
	else
	{
		*min_pos = N*row;
		*max_pos = N*(row+1)-1;
	}
}

void mem_copy(int *dest, int *src, int min_pos, int max_pos)
{
	for(int i=min_pos, j=0;i<=max_pos;i++, j++)
			dest[j] = src[i];
}

void display(int N, int M, int myid, int *a, int numprocs)
{
	for (int i=0;i<N;i++){
		for (int j = 0; j < M; j++) { 
		    printf("[%d]<%d,%d>: %d, ", myid,i,j, a[get_array_loc(M, i, j)]); 
		    //printf("%d, ", a[get_array_loc(M, i, j)]); 
		} 
		printf("\n"); 
	}
}












