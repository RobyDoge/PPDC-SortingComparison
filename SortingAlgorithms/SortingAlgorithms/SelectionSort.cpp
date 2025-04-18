// selection_sort.cpp
#include "SelectionSort.h"
#include <algorithm>
#include <iostream>

SelectionSort::SelectionSort() {
    // Initialize MPI environment
    MPI_Comm_size(MPI_COMM_WORLD, &_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
}

SelectionSort::~SelectionSort() {
}

std::string SelectionSort::GetName() const
{
    return "SelectionSort";
}

void SelectionSort::Sort(std::vector<int>& data) {
    int n = data.size();

    int local_size = n / _size;
    int remainder = n % _size;
    int local_start = _rank * local_size + std::min(_rank, remainder);

    if (_rank < remainder) {
        local_size++;
    }

    for (int i = local_start; i < local_start + local_size; i++) {
        int min_idx = i;

        for (int j = i + 1; j < n; j++) {
            if (data[j] < data[min_idx]) {
                min_idx = j;
            }
        }

        if (min_idx != i) {
            std::swap(data[i], data[min_idx]);
        }

        // Print progress every 20%
        if ((i - local_start + 1) % (local_size / 5) == 0) {
            std::cout << "Rank " << _rank << " completed " 
                      << ((i - local_start + 1) * 100 / local_size) 
                      << "% of its sorting task." << std::endl;
        }

    }

    MPI_Bcast(data.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
}