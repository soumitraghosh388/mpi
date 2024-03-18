#include <iostream>
#include <mpi.h>
using namespace std;

int neighbourCount(int* grid, int i, int j, const int width, const int height)
{
	int count = 0;
	if (i != 0)
	{
		if (grid[(i - 1) * width + j] == 1)
			count++;

		if (j != width - 1)
		{
			if (grid[(i - 1) * width + (j + 1)] == 1)
				count++;
		}

		if (j != 0)
		{
			if (grid[(i - 1) * width + (j - 1)] == 1)
				count++;

			
		}
	}

	if (j != 0)
	{
		if (grid[i * width + (j - 1)] == 1)
			count++;
	}

	if (j != width-1)
	{
		if (grid[i * width + (j + 1)] == 1)
			count++;
	}

	if (i != height - 1)
	{
		if (grid[(i + 1) * width + j] == 1)
			count++;

		if (j != width - 1)
		{
			if (grid[(i + 1) * width + (j + 1)] == 1)
				count++;

			
		}

		if (j != 0)
		{
			if (grid[(i + 1) * width + (j - 1)] == 1)
				count++;
		}
	}

	return count;
}


void printGrid(int *grid, int st, int end, const int width, const int height)
{
	for (int i = st; i <= end; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			printf("%d ", grid[i * width + j]);
		}
		printf("\n");
	}
}

int main()
{
	int rank, nprocs;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


	int width;
	int height;
	int iterations;

	//int* grid = new int[width * height];
	//int* nextGrid = new int[width * height];

	//int k = 0;
	/*for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			if (k % (width/3) == 0)
				grid[i * width + j] = 1;
			else
				grid[i * width + j] = 0;
			k++;
		}
	}

	if (rank == 0)
	{
		printf("Initial grid\n");
		printGrid(grid, 0, width - 1, width, height);
	}*/

        if (rank == 0){
            scanf("%d", &height);
            scanf("%d", &width);
            scanf("%d", &iterations);

        }

        MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* grid = new int[width * height];
        int* nextGrid = new int[width * height];
        int* temp_grid = NULL;
	//printf("%d %d %d\n", height, width, iterations);
	if(rank == 0)
	{
		for (int i = 0; i < height; ++i)
	        {
	                for (int j = 0; j < width; ++j)
	                {
	                        scanf("%d",&grid[i * width + j]); 
	                }
	        }
	}
	MPI_Bcast(grid, width*height, MPI_INT, 0, MPI_COMM_WORLD);
        /*for (int i = 0; i < height; ++i)
        {
                for (int j = 0; j < width; ++j)
                {
                        printf("%d ",grid[i * width + j]);
                }
		printf("\n");
        }*/
	int heightPerProc = height / nprocs;

	int myStartRow = rank * heightPerProc;
	int myEndRow = myStartRow + heightPerProc - 1;

	if (rank == nprocs - 1)
	{
		myEndRow = height - 1;
	}
	
	//temp_grid = new int[(myEndRow-myStartRow+1)*width];
	for (int it = 0; it < iterations; ++it)
	{
		int k = 0;
		for (int i = myStartRow; i <= myEndRow; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				int neigh_count = neighbourCount(grid, i, j, width, height);

				if (grid[i * width + j] == 1 && neigh_count < 2)
				{
					nextGrid[i * width + j] = 0;
				}
				else if (grid[i * width + j] == 1 && neigh_count > 3)
				{
					nextGrid[i * width + j] = 0;
				}

				else if (grid[i * width + j] == 1 && (neigh_count == 2 || neigh_count == 3))
				{
					nextGrid[i * width + j] = 1;
				}
				else if (grid[i * width + j] == 0 && neigh_count == 3)
				{
					nextGrid[i * width + j] = 1;
				}
				else
				{
					nextGrid[i * width + j] = grid[i * width + j];
				}
				//temp_grid[k] = nextGrid[i * width + j];
				//k++;
			}
		}

		int* temp = grid;
		grid = nextGrid;
		nextGrid = temp;


		/*if (rank % 2 == 0)
		{
			if (rank != nprocs - 1)
			{
				MPI_Send(&grid[myEndRow * width], width, MPI_INT, rank + 1, 100, MPI_COMM_WORLD);
			}

			if (rank != 0)
			{
				MPI_Status status;
				MPI_Recv(&grid[(myStartRow - 1) * width], width, MPI_INT, rank - 1, 100, MPI_COMM_WORLD, &status);
			}
		}
		else
		{
			if (rank != 0)
			{
				MPI_Status status;
				MPI_Recv(&grid[(myStartRow - 1) * width], width, MPI_INT, rank - 1, 100, MPI_COMM_WORLD, &status);

			}

			if (rank != nprocs - 1)
			{
				MPI_Send(&grid[myEndRow * width], width, MPI_INT, rank + 1, 100, MPI_COMM_WORLD);
			}
		}*/
		if (rank != 0)
		{
			for (int i = myStartRow; i <= myEndRow; ++i)
				MPI_Send(&grid[i * width], width, MPI_INT, 0, 100, MPI_COMM_WORLD);
		}
		if (rank == 0)
		{
			for (int r = 1; r < nprocs; ++r)
			{
				int iStartRow = r * heightPerProc;
				int iEndRow = iStartRow + heightPerProc - 1;
				if (r == nprocs - 1)
				{
					iEndRow = height - 1;
				}
				for (int i = iStartRow; i <= iEndRow; ++i)
				{
					MPI_Status status;
					MPI_Recv(&grid[i * width], width, MPI_INT, r, 100, MPI_COMM_WORLD, &status);
				}
			}
		}
		
		//MPI_Gather (temp_grid, (myEndRow-myStartRow)*width, MPI_INT, grid, (myEndRow-myStartRow)*width, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(grid, width*height, MPI_INT, 0, MPI_COMM_WORLD);
		
		/*if (rank == 0) 
		{
			printf("\n iter : %d\n", it);
			printGrid(grid, 0, height-1, width, height);
		}*/
		//MPI_Allgather(temp_grid, (myEndRow-myStartRow+1)*width, MPI_INT, grid, (myEndRow-myStartRow+1)*width, MPI_INT, MPI_COMM_WORLD);
	}


	//printf("Final grid %d---%d\n", myStartRow, myEndRow);
	if (rank == 0) printGrid(grid, 0, height-1, width, height);

	delete[] grid;
	delete[] nextGrid;
	//delete[] temp_grid;

	MPI_Finalize();

	return 0;
}
