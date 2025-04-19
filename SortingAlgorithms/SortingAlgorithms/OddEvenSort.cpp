#include "OddEvenSort.h"

#include <algorithm>
#include <iostream>
#include <mpi.h>
#include <ranges>

OddEvenSort::OddEvenSort()
{
	// Initialize MPI environment
	MPI_Comm_size(MPI_COMM_WORLD, &_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
}

void OddEvenSort::Sort(std::vector<int>& data, double& commTime)
{
	int n = data.size();
	double startTime;


	int localSize = n / _size;
    int localStart = _rank * localSize;
	std::vector<int> localData = std::vector<int>(data.begin() + localStart, data.begin() + localStart + localSize);
	std::ranges::sort(localData);

	startTime = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;

	int oddRank, evenRank;

	if (_rank % 2 == 0)
	{
		oddRank = _rank - 1;
		evenRank = _rank + 1;
	}
	else
	{
		oddRank = _rank + 1;
		evenRank = _rank - 1;
	}

	if (oddRank == -1 || oddRank == _size)
		oddRank = MPI_PROC_NULL;
	if (evenRank == -1 || evenRank == _size)
		evenRank = MPI_PROC_NULL;


    int p;
	std::vector<int> localData2(localSize);
    MPI_Status status;
    for (p = 0; p < _size - 1; 
        p++)
    {
        if (p % 2 == 1)
        {
			startTime = MPI_Wtime();
	        MPI_Sendrecv(localData.data(), localSize, MPI_INT, oddRank, 1, localData2.data(),
			   localSize, MPI_INT, oddRank, 1, MPI_COMM_WORLD, &status);
			commTime += MPI_Wtime() - startTime;
		}
        else
        {
			startTime = MPI_Wtime();
	        MPI_Sendrecv(localData.data(), localSize, MPI_INT, evenRank, 1, localData2.data(),
			   localSize, MPI_INT, evenRank, 1, MPI_COMM_WORLD, &status);
			commTime += MPI_Wtime() - startTime;
        }

        auto temp = std::vector<int>(localSize);
        for (int i = 0; i < localSize; i++)
        {
            temp[i] = localData[i];
        }
        if (status.MPI_SOURCE == MPI_PROC_NULL)
            continue;
        else if (_rank < status.MPI_SOURCE)
        {
            // store the smaller of the two
            int j, k;
            for (int i = j = k = 0; k < localSize; k++)
            {
                if (j == localSize || (i < localSize && temp[i] < localData2[j]))
                    localData[k] = temp[i++];
                else
                    localData[k] = localData2[j++];
            }
        }
        else
        {
            // store the larger of the two
            int j, k;
            for (int i = j = k = localSize - 1; k >= 0; k--)
            {
                if (j == -1 || (i >= 0 && temp[i] >= localData2[j]))
                    localData[k] = temp[i--];
                else
                    localData[k] = localData2[j--];
            }
        } 
    } 

	startTime = MPI_Wtime();
    MPI_Gather(localData.data(), localSize, MPI_INT, data.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;
}

std::string OddEvenSort::GetName() const
{
	return "OddEvenSort";
}
