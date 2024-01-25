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

int n_ways = 0;
int prev_depth = 0;

int main(int argc, char *argv[])
{
	int N = strtol(argv[1], NULL, 10);
	int myid, numprocs, tag = 1, sel_value = 0;
	double tt0, ttf;
	int *a, *a_bck, *sel, i = 0, n, fst_row_tr_count = 0;
	int **a_bck_array;
	//bool backtrack = false;
	

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	
	//printf("%d \n", N);
	
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	a_bck_array = (int**)calloc(N, sizeof(int));
	for (int i = 0; i<N;i++)
		a_bck_array[i] = (int*)calloc(N*N, sizeof(int));
    	a = (int*)calloc(N*N, sizeof(int));
	n = N/2;
	
	/*set_array_value(N, N, a, 1);
	set_array_value(1, N, sel, -1);
	
	while(i<N && i >= 0 && fst_row_tr_count < n)
	{
		if (i<N-1)
		{	
			if (i==0) fst_row_tr_count++;
			if (backtrack)
			
			sel_value = get_avail_pos(a, i, N);
			if (sel_value == -1) 
			{
				memcpy(a, a_bck, N*N*sizeof(int));
				i--;
				backtrack = true;
			}
			else 
			{
				sel[i] = sel_value;
				a[get_array_loc(N, i, sel_value)] = 0;
				update_array(a, sel, i, N);
				i++;
			}
			memcpy(a_bck, a, N*N*sizeof(int));
		}
		else
		{
			sel_value = get_avail_pos(a, i, N);
			if (sel_value == -1) 
			{
				memcpy(a, a_bck, N*N*sizeof(int));
				i--;
			}
			else 
			{
				n_ways++;
				//sel[i] = sel_value;
				a[get_array_loc(N, i, sel_value)] = 0;
				//update_array(a, sel, i, N);
			}
			memcpy(a_bck, a, N*N*sizeof(int));
		}
	}*/
	
	for(int i = 0;i<N;i++)
	{
		if(a[i] == 0)
			DFS_traverse(a_bck_array, a, i, N);
	}

	printf("total sum : %d\n", n_ways);
	//free(a);
	//free(a_bck_array);
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
	memcpy(a_bck_array[cur_depth], a, N*N*sizeof(int));

	for(int i = 0;i<N;i++)
	{
        if(!a_bck_array[i])
            printf("memory not allocated for %d : \n", i);
	}

	get_children(v, &min_pos, &max_pos, N);
	if (min_pos == -1)
	{
		n_ways++;
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
            memcpy(a, a_bck_array[cur_depth], N*N*sizeof(int));
        if (prev_depth > cur_depth)
        {
            memcpy(a, a_bck_array[cur_depth], N*N*sizeof(int));
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
















