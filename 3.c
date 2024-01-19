#include "mpi.h"
#include <math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>

int cal_live_neighbor(int *, int, int, int, int, int);
int get_array_loc(int, int, int);
int live_or_die(int, int);
void display(int, int, int, int *, int);

int main(int argc, char *argv[])
{
	int myid, numprocs, tag = 1, N, M, T;
	double tt0, ttf;
	const char *filename = "3_input.in";
	FILE *in_file;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	//int SIZE = 4;
	int nmt[3];
	int *a, *temp_a;
	int i = 0, j = 0, l = 0;
	struct stat sb;
	stat(filename, &sb);
	char *file_contents = malloc(sb.st_size);
	tt0 = MPI_Wtime();
	

	//printf("[%d]: Before Bcast, buf is %f\n", myid, a);
	/*if (myid == 0) 
	{
		in_file = fopen(filename,"r");
		struct stat sb;
		stat(filename, &sb);
		char *file_contents = malloc(sb.st_size);
		int i = 0, j = 0, l = 0;
		while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
			if (i == 0)
			{
				char *token = strtok(file_contents, " ");
				
				while (token != NULL)
				{
					nmt[l] = strtol(token, NULL, 10);
					token = strtok(NULL, " ");
					l++;
				}
				N = nmt[0];
				M = nmt[1];
				T = nmt[2];
				i++;
				//printf("[%d]: N,M,T is %d %d %d\n", myid, N,M,T);
				a = (int*)calloc(N*M, sizeof(int)); 
			}
			else
			{
				//printf("%d > %s\n", myid, file_contents);
				
				char *token = strtok(file_contents, " ");
				while (token != NULL)
				{
					a[j] = strtol(token, NULL, 10);
					printf("%d, ", a[j]); 
					token = strtok(NULL, " ");
					j++;
					//printf("<%d> j \n", j); 
				}
				printf("\n"); 
			}
			
		}
		fclose(in_file);
		free(file_contents);
	}*/
	/*for (int k = 0; k < N*M; ++k) { 
            //printf("[%d]: %d, ", myid, a[k]); 
            printf("%d, ", a[k]); 
        } */
        if (myid == 0)
        {
        	in_file = fopen(filename,"r");
		//struct stat sb;
		//stat(filename, &sb);
		//char *file_contents = malloc(sb.st_size);
		
		while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
			char *token = strtok(file_contents, " ");
			while (token != NULL)
			{
				nmt[l] = strtol(token, NULL, 10);
				token = strtok(NULL, " ");
				l++;
			}
			N = nmt[0];
			M = nmt[1];
			T = nmt[2];
			break;
		}
		//fclose(in_file);
		//free(file_contents);
        }
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);

	ttf = MPI_Wtime();
	//printf("%d > %10f\n", myid, (ttf-tt0));
	//printf("[%d]: N,M,T is %d %d %d\n", myid, N,M,T);
	
	a = (int*)calloc(N*M, sizeof(int)); 
	temp_a = (int*)calloc(N*M, sizeof(int)); 
	if (myid == 0)
        {
		while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
			char *token = strtok(file_contents, " ");
			while (token != NULL)
			{
				/*if (strtol(token, NULL, 10) == 1){
					a[j] = 1;
					j++;
				}*/
				
				a[j] = strtol(token, NULL, 10);
				printf("%d, ", a[j]); 
				j++;
				token = strtok(NULL, " ");
				
				//printf("<%d> j \n", j); 
			}
			printf("\n"); 
			
		}
		for (i = 1; i < numprocs; i++) {
			MPI_Send (a, N*M, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
		fclose(in_file);
		free(file_contents);
	}
	else 
		MPI_Recv (a, N*M, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
	
	memcpy(temp_a, a, N*M*sizeof(int));
	
	/*for (int k = 0; k < N*M; k++) { 
            printf("[%d]: %d, ", myid, a[k]); 
            //printf("%d, ", a[k]); 
        } 
        printf("\n"); 
        int sum = 0;
        for (int i = 0; i< N*M ; i++)
        	sum = sum + a[i];
        printf("[%d]: sum: %d \n", myid, sum); */
	//printf("[%d]: After Bcast, buf is %f\n", myid, a[2][2]);
	//display(N, M, myid, temp_a, numprocs);
	int live_nb = 0, k = 0;
	//int n_row_per_proc = ceil((float)N/numprocs);
	for (int i = myid; i < N; i += numprocs) {
		for (int j = 0; j < M ; j++){
			k = get_array_loc(N, i, j);
			live_nb = cal_live_neighbor(a, numprocs, i, j, N, M);
			//printf("<%d,%d> live_nb: %d \n", i,j, live_nb); 
			int q = live_or_die(live_nb, a[k]);
			//printf("<%d,%d> q: %d \n", i,j, q); 
			temp_a[k] = q;
		}
	}
	printf("\nAfter ----\n");
	display(N, M, myid, temp_a, numprocs);
        printf("\n"); 
        int sum = 0;
        for (int i = 0; i< N*M ; i++)
        	sum = sum + temp_a[i];
        printf("[%d]: sum: %d \n", myid, sum); 
	
	if (a != NULL) free(a);
	free(temp_a);
	MPI_Finalize();
	return 0;
}

int cal_live_neighbor(int *a, int numprocs, int i, int j, int N, int M)
{
	int sum = 0;
	// [i-1,j-1], [i-1,j], [i-1,j+1], [i,j+1], [i+1,j+1], [i+1,j], [i+1,j-1], [i,j-1]
	if (i > 0 && i < N-1)
	{
		if (j > 0 && j < M-1)
		{
			sum += a[get_array_loc(N, i-1, j-1)];
			sum += a[get_array_loc(N, i-1, j)];
			sum += a[get_array_loc(N, i-1, j+1)];
			sum += a[get_array_loc(N, i, j+1)];
			sum += a[get_array_loc(N, i+1, j+1)];
			sum += a[get_array_loc(N, i+1, j)];
			sum += a[get_array_loc(N, i+1, j-1)];
			sum += a[get_array_loc(N, i, j-1)];
		}
		else if (j > 0)
		{
			sum += a[get_array_loc(N, i-1, j-1)];
			sum += a[get_array_loc(N, i-1, j)];
			sum += a[get_array_loc(N, i+1, j)];
			sum += a[get_array_loc(N, i+1, j-1)];
			sum += a[get_array_loc(N, i, j-1)];
		}
		else
		{
			sum += a[get_array_loc(N, i-1, j)];
			sum += a[get_array_loc(N, i-1, j+1)];
			sum += a[get_array_loc(N, i, j+1)];
			sum += a[get_array_loc(N, i+1, j+1)];
			sum += a[get_array_loc(N, i+1, j)];

		}
	}
	else if (i > 0)
	{
		if (j > 0 && j < M-1)
		{
			sum += a[get_array_loc(N, i-1, j-1)];
			sum += a[get_array_loc(N, i-1, j)];
			sum += a[get_array_loc(N, i-1, j+1)];
			sum += a[get_array_loc(N, i, j+1)];
			sum += a[get_array_loc(N, i, j-1)];
		}
		else if (j > 0)
		{
			sum += a[get_array_loc(N, i-1, j-1)];
			sum += a[get_array_loc(N, i-1, j)];
			sum += a[get_array_loc(N, i, j-1)];
		}
		else
		{
			sum += a[get_array_loc(N, i-1, j)];
			sum += a[get_array_loc(N, i-1, j+1)];
			sum += a[get_array_loc(N, i, j+1)];

		}
	}
	else
	{
		if (j > 0 && j < M-1)
		{
			sum += a[get_array_loc(N, i, j+1)];
			sum += a[get_array_loc(N, i+1, j+1)];
			sum += a[get_array_loc(N, i+1, j)];
			sum += a[get_array_loc(N, i+1, j-1)];
			sum += a[get_array_loc(N, i, j-1)];
		}
		else if (j > 0)
		{
			sum += a[get_array_loc(N, i+1, j)];
			sum += a[get_array_loc(N, i+1, j-1)];
			sum += a[get_array_loc(N, i, j-1)];
		}
		else
		{
			sum += a[get_array_loc(N, i, j+1)];
			sum += a[get_array_loc(N, i+1, j+1)];
			sum += a[get_array_loc(N, i+1, j)];

		}

	}
	return sum;
}

int get_array_loc(int N, int i, int j)
{
	return N * i + j;
}

int live_or_die(int live_nb, int p)
{
	//printf("live_nb, a[k] : %d %d \n", live_nb, p); 
	if (p == 1){
		if (live_nb < 2)
			return 0;
		else if (live_nb == 2 || live_nb == 3)
			return 1;
		else
			return 0;

	}
	else{
		if (live_nb == 3)
			return 1;
		else
			return p;
	}
}

void display(int N, int M, int myid, int *a, int numprocs)
{
	for (int i=0;i<N;i++){
		for (int j = 0; j < M; j++) { 
		    printf("[%d]<%d,%d>: %d, ", myid,i,j, a[get_array_loc(N, i, j)]); 
		    //printf("%d, ", a[k]); 
		} 
		printf("\n"); 
	}
}



