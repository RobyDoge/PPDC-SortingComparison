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

		localRanks[rank_val] = data[global_idx];
		// Print progress every 20% with different colors
		if (i % (localData.size() / 5) == 0) {
			int progress = (i * 100) / localData.size();
			std::string color;
			std::string baseColor = "\033[0m"; // Reset color
			if (progress < 20) {
				color = "\033[31m"; // Red
			}
			else if (progress < 40) {
				color = "\033[33m"; // Yellow
			}
			else if (progress < 60) {
				color = "\033[32m"; // Green
			}
			else if (progress < 80) {
				color = "\033[36m"; // Cyan
			}
			else {
				color = "\033[34m"; // Blue
			}


			std::cout << "Rank " << _rank << " sorting progress: " << color
				<< progress << "%" << "\033[0m" << std::endl;
		}
	}

	std::vector<int> sortedData(n, -1);

	startTime = MPI_Wtime();
	MPI_Reduce(localRanks.data(), sortedData.data(), n, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;

	data = sortedData;

}


