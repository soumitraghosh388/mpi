#include "mpi.h"
#include <math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>


//void div_a_to_proc(int, int, int, int, int *, int *, MPI_Status);
void display(int, int, int, int *, int);
int get_array_loc(int, int, int);
void mem_copy(int *, int *, int, int);

int main(int argc, char *argv[])
{
	int myid, numprocs, tag = 1, N, i=0, j=0, n_row_per_proc=0;
	double tt0, ttf;
	int *a, *kth_row, *tmp_a, *tmp_a_cpy;
	//FILE *in_file;
	//const char *filename = "2_input.in";
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	tt0 = MPI_Wtime();
	
	//struct stat sb;
	//stat(filename, &sb);
	//char *file_contents = malloc(sb.st_size);
	
	if (myid == 0){
            scanf("%d", &N);
        }
	a = new int[N*N];
	tmp_a = new int[N*N];
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if(myid == 0)
	{
		for (int i = 0; i < N*N; ++i)
	        {
			scanf("%d",&a[i]); 
	        }
	}
	MPI_Bcast(a, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	/*if (myid == 0)
        {
        	//printf("Printing Input file content : \n");
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
		n_row_per_proc = N/numrocs + 1;
	}*/
	n_row_per_proc = N / numprocs;

	int myStartRow = myid * n_row_per_proc;
	int myEndRow = myStartRow + n_row_per_proc - 1;

	if (myid == numprocs - 1)
	{
		myEndRow = N - 1;
	}
	//a = (int*)calloc(N*N, sizeof(int)); 
	//tmp_a = (int*)calloc(N*n_row_per_proc, sizeof(int));
	kth_row = new int[N];
	//mem_copy(tmp_a, a, 0, N*N-1);
	for(int k = 0;k<N;k++)
	{
		mem_copy(tmp_a, a, 0, N*N-1);
                mem_copy(kth_row, a, N*k, N*(k+1)-1);
                MPI_Bcast(kth_row, N, MPI_INT, 0, MPI_COMM_WORLD);

                for (int i = myStartRow; i <= myEndRow; i++) {
                        for (int j = 0; j < N ; j++){
                                int dist_ik = a[get_array_loc(N, i, k)];
                                int dist_kj = kth_row[get_array_loc(N, 0, j)];
                                if (dist_ik == -1 || dist_kj == -1)
                                        continue;
                                else
                                {
                                        if (a[get_array_loc(N, i, j)] == -1 || (a[get_array_loc(N, i, j)] > (dist_ik + dist_kj)))
                                                tmp_a[get_array_loc(N, i, j)] = dist_ik + dist_kj;
                                }

                        }
                }
                //MPI_Gather (tmp_a, n_row_per_proc*N, MPI_INT, a, n_row_per_proc*N, MPI_INT, 0, MPI_COMM_WORLD);
                //div_a_to_proc(myid, N, n_row_per_proc, numprocs, tmp_a, a, status);
        	int* temp = a;
		a = tmp_a;
		tmp_a = temp;

		if (myid != 0)
	        {
	                for (int i = myStartRow; i <= myEndRow; ++i)
	                        MPI_Send(&a[i * N], N, MPI_INT, 0, 100, MPI_COMM_WORLD);
	        }
	        if (myid == 0)
	        {
	                for (int r = 1; r < numprocs; ++r)
	                {
	                        int iStartRow = r * n_row_per_proc;
	                        int iEndRow = iStartRow + n_row_per_proc - 1;
	                        if (r == numprocs - 1)
	                        {
	                                iEndRow = N - 1;
	                        }
	                        for (int i = iStartRow; i <= iEndRow; ++i)
	                        {
	                                MPI_Status status;
	                                MPI_Recv(&a[i * N], N, MPI_INT, r, 100, MPI_COMM_WORLD, &status);
	                        }
	                }
	        }
		MPI_Bcast(a, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	}
	
	//div_a_to_proc(myid, N, n_row_per_proc, numprocs, tmp_a, a, status);
	//if (file_contents != NULL) free(file_contents);

	//kth_row = (int*)calloc(N, sizeof(int)); 
	
	/*for(int k = 0;k<N;k++){

		mem_copy(kth_row, a, N*k, N*(k+1)-1);
		MPI_Bcast(kth_row, N, MPI_INT, 0, MPI_COMM_WORLD);

		for (int i = ; i < n_row_per_proc; i++) {
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
	MPI_Gather (tmp_a, n_row_per_proc*N, MPI_INT, a, n_row_per_proc*N, MPI_INT, 0, MPI_COMM_WORLD);*/
	if (myid == 0) 
	{
		//printf("\nOutput : \n");
		display(N, N, 0, a, numprocs);
	}
	ttf = MPI_Wtime();
	//printf("%d > %10f\n", myid, (ttf-tt0));
	//if (a != NULL) free(a);
	delete[] a;
	delete[] kth_row;
	delete[] tmp_a;
	MPI_Finalize();
	return 0;
}

void display(int N, int M, int myid, int *a, int numprocs)
{
	for (int i=0;i<N;i++){
		for (int j = 0; j < M; j++) { 
		    //printf("[%d]<%d,%d>: %d, ", myid,i,j, a[get_array_loc(M, i, j)]); 
		    printf("%d ", a[get_array_loc(M, i, j)]); 
		} 
		printf("\n"); 
	}
}

int get_array_loc(int M, int i, int j)
{
	return M * i + j;
}

/*void div_a_to_proc(int myid, int N, int n_row_per_proc, int numprocs, int *tmp_a, int *a, MPI_Status status)
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
}*/

void mem_copy(int *dest, int *src, int min_pos, int max_pos)
{
	for(int i=min_pos, j=0;i<=max_pos;i++, j++)
		dest[j] = src[i];
}






