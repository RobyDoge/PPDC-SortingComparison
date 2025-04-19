#include "RankSort.h"

#include <mpi.h>

RankSort::RankSort()
{
	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &_size);
}

void RankSort::Sort(std::vector<int>& data, double& commTime)
{
	int n = data.size();

	int localSize = n / _size;
	std::vector<int> localData(localSize);

	double startTime = MPI_Wtime();
	MPI_Scatter(data.data(), localSize, MPI_INT, localData.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;

	std::vector<int> localRanks(localSize, 0);

	for (int i = 0; i < localSize; ++i)
	{
		int global_i = _rank * localSize + i;
		for (int j = 0; j < n; ++j)
		{
			if (data[j] < data[global_i])
				localRanks[i]++;
		}
	}

	std::vector<int> globalRanking;
	if (_rank == 0)
		globalRanking.resize(n);

	startTime = MPI_Wtime();
	MPI_Gather(localRanks.data(), localSize, MPI_INT,
		globalRanking.data(), localSize, MPI_INT,
		0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;

	std::vector<int> sorted(n);
	if (_rank == 0)
	{
		std::vector<int> all_data(n);

		startTime = MPI_Wtime();
		MPI_Gather(localData.data(), localSize, MPI_INT, all_data.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);
		commTime += MPI_Wtime() - startTime;

		for (int i = 0; i < n; ++i)
		{
			sorted[globalRanking[i]] = data[i];
		}
		data = sorted;
	}
	else
	{
		startTime = MPI_Wtime();
		MPI_Gather(localData.data(), localSize, MPI_INT, nullptr, localSize, MPI_INT, 0, MPI_COMM_WORLD);
		commTime += MPI_Wtime() - startTime;	
	}
}




