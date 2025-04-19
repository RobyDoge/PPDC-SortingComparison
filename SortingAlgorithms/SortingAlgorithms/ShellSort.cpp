#include "ShellSort.h"

#include <algorithm>
#include <iostream>
#include <mpi.h>

ShellSort::ShellSort()
{
	// Initialize MPI environment
	MPI_Comm_size(MPI_COMM_WORLD, &_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
}

void ShellSort::Sort(std::vector<int>& data, double& commTime)
{

	int n = data.size();
	int localSize = n / _size;
	int localStart = _rank * localSize;
	std::vector<int> localData = std::vector<int>(data.begin() + localStart, data.begin() + localStart + localSize);

	for (int gap = localSize / 2; gap > 0; gap /= 2) {
		for (int i = gap; i < localSize; i++) {
			int temp = localData[i];
			int j;
			for (j = i; j >= gap && localData[j - gap] > temp; j -= gap) {
				localData[j] = localData[j - gap];
			}
			localData[j] = temp;
		}
		if (std::ranges::is_sorted(localData))
		{
			break;
		}
	}

	double startTime = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;


	std::vector<int> globalData;
	std::vector<int> recvCounts;
	std::vector<int> displs;

	// Only the root process needs to allocate the full array and setup counts/displacements
	if (_rank == 0) {
		globalData.resize(n);
		recvCounts.resize(_size);
		displs.resize(_size);

		// Calculate even distribution with remainder handling
		int baseCount = n / _size;
		int remainder = n % _size;

		int currentDisp = 0;
		for (int i = 0; i < _size; i++) {
			// First 'remainder' processes get one extra element
			recvCounts[i] = baseCount + (i < remainder ? 1 : 0);
			displs[i] = currentDisp;
			currentDisp += recvCounts[i];
		}
	}

	startTime = MPI_Wtime();
	MPI_Gatherv(localData.data(), localSize, MPI_INT,
		globalData.data(), recvCounts.data(), displs.data(),
		MPI_INT, 0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;

	if (_rank == 0) {
		int n = globalData.size();

		for (int gap = n / 2; gap > 0; gap /= 2) {
			for (int i = gap; i < n; i++) {
				int temp = globalData[i];

				int j;
				for (j = i; j >= gap && globalData[j - gap] > temp; j -= gap) {
					globalData[j] = globalData[j - gap];
				}

				globalData[j] = temp;
			}
			if (std::ranges::is_sorted(globalData))
			{
				break;
			}
		}
	}

	data = globalData;



}
