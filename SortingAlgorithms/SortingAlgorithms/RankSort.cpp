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

	std::vector<int> localRanks(n, -1);

	for (int i = 0; i < localSize; ++i) {
		int global_idx = _rank * localSize + i;
		int rank_val = 0;

		for (int j = 0; j < n; ++j) {
			if (data[j] < data[global_idx] ||
				(data[j] == data[global_idx] && j < global_idx)) {
				rank_val++;
			}
		}
	}

	std::vector<int> sortedData(n, -1);

	startTime = MPI_Wtime();
	MPI_Reduce(localRanks.data(), sortedData.data(), n, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;

	data = sortedData;

}


