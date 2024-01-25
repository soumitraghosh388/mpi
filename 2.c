#include "mpi.h"
#include <math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>


void div_a_to_proc(int, int, int, int, int *, int *, MPI_Status);
void display(int, int, int, int *, int);
int get_array_loc(int, int, int);
void mem_copy(int *, int *, int, int);

int main(int argc, char *argv[])
{
	int myid, numprocs, tag = 1, N, i=0, j=0, n_row_per_proc=0;
	double tt0, ttf;
	int *a, *kth_row, *tmp_a, *tmp_a_cpy;
	FILE *in_file;
	const char *filename = "2_input.in";
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	tt0 = MPI_Wtime();
	
	struct stat sb;
	stat(filename, &sb);
	char *file_contents = malloc(sb.st_size);
	
	if (myid == 0)
        {
        	in_file = fopen(filename,"r");
		while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
			char *token = strtok(file_contents, " ");
			while (token != NULL)
			{
				N = strtol(token, NULL, 10);
				token = strtok(NULL, " ");
			}
			break;
		}
        }
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (N % numprocs == 0)
		n_row_per_proc = N/numprocs;
	else
	{
		n_row_per_proc = N/numprocs + 1;
	}
	
	a = (int*)calloc(N*N, sizeof(int)); 
	tmp_a = (int*)calloc(N*n_row_per_proc, sizeof(int));
	//tmp_a_cpy = (int*)calloc(N*n_row_per_proc, sizeof(int));
	
	if (myid == 0)
        {
		while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
			char *token = strtok(file_contents, " ");
			while (token != NULL)
			{
				a[j] = strtol(token, NULL, 10);
				printf("%d, ", a[j]); 
				j++;
				token = strtok(NULL, " ");
			}
			printf("\n"); 
			
		}
		/*for (i = 0; i < numprocs; i++) {
			for(int l=N*i*n_row_per_proc, m=0;l<N*(i+1)*n_row_per_proc;l++, m++)
			{
				if (i==0)
					tmp_a_cpy[m] = a[l];
				else
					tmp_a[m] = a[l];
			}
			//display(n_row_per_proc, N, i, tmp_a, numprocs);
			if (i>0) MPI_Send (tmp_a, N*n_row_per_proc, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
		tmp_a = tmp_a_cpy;*/
		fclose(in_file);
	}
	/*else 
		MPI_Recv (tmp_a, N*n_row_per_proc, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);*/
		
	div_a_to_proc(myid, N, n_row_per_proc, numprocs, tmp_a, a, status);
	if (file_contents != NULL) free(file_contents);
	//if (tmp_a != NULL) free(tmp_a);
	//if (tmp_a_cpy != NULL) free(tmp_a_cpy);
	
	/*for (int i=0;i<N;i++){
		for (int j = 0; j < N; j++) {
			if (a[get_array_loc(N, i, j)] == -1)
				a[get_array_loc(N, i, j)] = INFINITY;
		}
	}*/
	
	//display(1, N, myid, tmp_a, numprocs);
	
	kth_row = (int*)calloc(N, sizeof(int)); 
	
	for(int k = 0;k<N;k++){
		/*for(int l=N*k, m=0;l<N*(k+1);l++, m++)
			kth_row[m] = a[l];*/
		mem_copy(kth_row, a, N*k, N*(k+1)-1);
		MPI_Bcast(kth_row, N, MPI_INT, 0, MPI_COMM_WORLD);
		/*for (int i = myid*n_row_per_proc; i < (myid+1)*n_row_per_proc && i < N; i++) {
			for (int j = 0; j < N ; j++){
				int dist_ik = a[get_array_loc(N, i, k)];
				int dist_kj = a[get_array_loc(N, k, j)];
				if (dist_ik == -1 || dist_kj == -1)
					continue;
				else
				{
					if (a[get_array_loc(N, i, j)] == -1 || a[get_array_loc(N, i, j)] > (dist_ik + dist_kj))
						a[get_array_loc(N, i, j)] = dist_ik + dist_kj;
				}
				
			}
		}*/
		for (int i = 0; i < n_row_per_proc; i++) {
			for (int j = 0; j < N ; j++){
				int dist_ik = tmp_a[get_array_loc(N, i, k)];
				int dist_kj = kth_row[get_array_loc(N, 0, j)];
				if (dist_ik == -1 || dist_kj == -1)
					continue;
				else
				{
					if (tmp_a[get_array_loc(N, i, j)] == -1 || (tmp_a[get_array_loc(N, i, j)] > (dist_ik + dist_kj)))
						tmp_a[get_array_loc(N, i, j)] = dist_ik + dist_kj;
				}
				
			}
		}
		MPI_Gather (tmp_a, n_row_per_proc*N, MPI_INT, a, n_row_per_proc*N, MPI_INT, 0, MPI_COMM_WORLD);
		div_a_to_proc(myid, N, n_row_per_proc, numprocs, tmp_a, a, status);
	}
	MPI_Gather (tmp_a, n_row_per_proc*N, MPI_INT, a, n_row_per_proc*N, MPI_INT, 0, MPI_COMM_WORLD);
	if (myid == 0) display(N, N, 0, a, numprocs);
	
	ttf = MPI_Wtime();
	printf("%d > %10f\n", myid, (ttf-tt0));
	if (a != NULL) free(a);
	MPI_Finalize();
	return 0;
}

void display(int N, int M, int myid, int *a, int numprocs)
{
	for (int i=0;i<N;i++){
		for (int j = 0; j < M; j++) { 
		    printf("[%d]<%d,%d>: %d, ", myid,i,j, a[get_array_loc(M, i, j)]); 
		    //printf("%d, ", a[k]); 
		} 
		printf("\n"); 
	}
}

int get_array_loc(int M, int i, int j)
{
	return M * i + j;
}

void div_a_to_proc(int myid, int N, int n_row_per_proc, int numprocs, int *tmp_a, int *a, MPI_Status status)
{
	if (myid == 0)
        {
		int *tmp_a_cpy = (int*)calloc(N*n_row_per_proc, sizeof(int));
		for (int i = 0; i < numprocs; i++) {
			for(int l=N*i*n_row_per_proc, m=0;l<N*(i+1)*n_row_per_proc;l++, m++)
			{
				if (i==0)
					tmp_a_cpy[m] = a[l];
				else
					tmp_a[m] = a[l];
			}
			//display(n_row_per_proc, N, i, tmp_a, numprocs);
			if (i>0) MPI_Send (tmp_a, N*n_row_per_proc, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
		//tmp_a = tmp_a_cpy;
		mem_copy(tmp_a, tmp_a_cpy, 0, N*n_row_per_proc-1);
		if (tmp_a_cpy != NULL) free(tmp_a_cpy);
	}
	else 
		MPI_Recv (tmp_a, N*n_row_per_proc, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
}

void mem_copy(int *dest, int *src, int min_pos, int max_pos)
{
	for(int i=min_pos, j=0;i<=max_pos;i++, j++)
			dest[j] = src[i];
}






