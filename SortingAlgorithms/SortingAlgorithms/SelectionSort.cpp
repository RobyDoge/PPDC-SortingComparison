// selection_sort.cpp
#include "SelectionSort.h"
#include <algorithm>
#include <iostream>
#include <queue>

SelectionSort::SelectionSort() {
    // Initialize MPI environment
    MPI_Comm_size(MPI_COMM_WORLD, &_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
}

std::string SelectionSort::GetName() const
{
    return "SelectionSort";
}

void SelectionSort::Sort(std::vector<int>& data, double& commTime)
{
    int n = data.size();
    
    double startTime;

    int local_size = n / _size;
    int remainder = n % _size;
    int local_start = _rank * local_size + std::min(_rank, remainder);
    if (_rank < remainder) {
        local_size++;
    }
    std::vector<int> localData = std::vector<int>(data.begin() + local_start, data.begin() + local_start + local_size);

	for (int i = 0; i < localData.size() - 1; i++) {
        int min_idx = i;

        for (int j = i + 1; j < localData.size(); j++) {
            if (localData[j] < localData[min_idx]) {
                min_idx = j;
            }
        }

        if (min_idx != i) {
            std::swap(localData[i], localData[min_idx]);
        }
    }

    std::vector<std::vector<int>> globalMatrix;
	if (_rank == 0)
	{
		globalMatrix.resize(_size);
        std::vector<int> sizes(_size);
        sizes[0] = local_size;

        for (int i = 1; i < _size; ++i) {
			startTime = MPI_Wtime();
            MPI_Recv(&sizes[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			commTime += MPI_Wtime() - startTime;
        }

        globalMatrix.resize(_size);
        for (int i = 0; i < _size; ++i) {
            globalMatrix[i].resize(sizes[i]);
        }

        for (int i = 0; i < local_size; ++i) {
            globalMatrix[0][i] = localData[i];
        }

        for (int i = 1; i < _size; ++i) {
			startTime = MPI_Wtime();
            MPI_Recv(globalMatrix[i].data(), sizes[i], MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			commTime += MPI_Wtime() - startTime;
        }
	}
    else
    {
		startTime = MPI_Wtime();
        MPI_Send(&local_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(localData.data(), local_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
		commTime += MPI_Wtime() - startTime;
    }

    if (_rank == 0)
    {
		std::cout << "\nRank 0 received all sorted sections."<< std::endl;
        // Merge all sorted sections into one
        data = MergeData(globalMatrix);
    }

	startTime = MPI_Wtime();
    MPI_Bcast(data.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
	commTime += MPI_Wtime() - startTime;
}

std::vector<int> SelectionSort::MergeData(const std::vector<std::vector<int>>& vectors)
{
    struct Element {
        int value;
        int vector_index;
        int element_index;

        // Custom comparison for the min-heap
        bool operator>(const Element& other) const {
            return value > other.value;
        }
    };

    std::priority_queue<Element, std::vector<Element>, std::greater<>> min_heap;
    std::vector<int> merged_vector;

    // Initialize the min-heap with the first element of each non-empty vector
    for (int i = 0; i < vectors.size(); ++i) {
        if (!vectors[i].empty()) {
            min_heap.push({ vectors[i][0], i, 0 });
        }
    }

    // Process the min-heap until it's empty
    while (!min_heap.empty()) {
        Element current = min_heap.top();
        min_heap.pop();

        merged_vector.push_back(current.value);

        int vec_index = current.vector_index;
        int elem_index = current.element_index;

        if (elem_index + 1 < vectors[vec_index].size()) {
            min_heap.push({ vectors[vec_index][elem_index + 1], vec_index, elem_index + 1 });
        }
    }

    return merged_vector;

	
}
