#include "BucketSort.h"

#include <algorithm>
#include <iostream>
#include <mpi.h>


BucketSort::BucketSort() {
	// Initialize MPI environment
	MPI_Comm_size(MPI_COMM_WORLD, &_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
}

std::string BucketSort::GetName() const
{
	return "BucketSort";
}

void BucketSort::Sort(std::vector<int>& data)
{

	std::vector<int> local_data;
	int total_size = 0;
	if (_rank == 0) {
		total_size = data.size();
	}
	MPI_Bcast(&total_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	std::vector<int> send_counts(_size, total_size / _size);
	std::vector<int> displs(_size, 0);

	int remainder = total_size % _size;
	for (int i = 0; i < remainder; ++i) {
		send_counts[i]++;
	}
	for (int i = 1; i < _size; ++i) {
		displs[i] = displs[i - 1] + send_counts[i - 1];
	}

	int local_size = send_counts[_rank];
	local_data.resize(local_size);

	MPI_Scatterv(_rank == 0 ? data.data() : nullptr,
		send_counts.data(), displs.data(), MPI_INT,
		local_data.data(), local_size, MPI_INT,
		0, MPI_COMM_WORLD);

	int local_min = local_size > 0 ? local_data[0] : 0;
	int local_max = local_size > 0 ? local_data[0] : 0;

	for (int val : local_data) {
		if (val < local_min) local_min = val;
		if (val > local_max) local_max = val;
	}
	int global_min, global_max;
	MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
	MPI_Allreduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	double range = static_cast<double>(global_max - global_min + 1);
	double bucket_size = range / _size;

	std::vector<std::vector<int>> buckets(_size);
	for (int val : local_data) {
		int bucket_idx;
		if (val == global_max) {
			bucket_idx = _size - 1;
		}
		else {
			bucket_idx = static_cast<int>((val - global_min) / bucket_size);
		}
		buckets[bucket_idx].push_back(val);
	}

	std::vector<int> send_counts_buckets(_size), recv_counts_buckets(_size);
	for (int i = 0; i < _size; ++i) {
		send_counts_buckets[i] = buckets[i].size();
	}

	MPI_Alltoall(send_counts_buckets.data(), 1, MPI_INT,
		recv_counts_buckets.data(), 1, MPI_INT,
		MPI_COMM_WORLD);

	std::vector<int> send_displs(_size, 0), recv_displs(_size, 0);
	int total_send = send_counts_buckets[0];
	int total_recv = recv_counts_buckets[0];

	for (int i = 1; i < _size; ++i) {
		send_displs[i] = send_displs[i - 1] + send_counts_buckets[i - 1];
		recv_displs[i] = recv_displs[i - 1] + recv_counts_buckets[i - 1];
		total_send += send_counts_buckets[i];
		total_recv += recv_counts_buckets[i];
	}

	std::vector<int> send_buffer(total_send);
	std::vector<int> recv_buffer(total_recv);

	// Fill send buffer
	for (int i = 0, idx = 0; i < _size; ++i) {
		for (int val : buckets[i]) {
			send_buffer[idx++] = val;
		}
	}

	MPI_Alltoallv(send_buffer.data(), send_counts_buckets.data(), send_displs.data(), MPI_INT,
		recv_buffer.data(), recv_counts_buckets.data(), recv_displs.data(), MPI_INT,
		MPI_COMM_WORLD);

	std::ranges::sort(recv_buffer);

	int local_sorted_size = recv_buffer.size();
	std::vector<int> all_sizes(_size);

	MPI_Gather(&local_sorted_size, 1, MPI_INT,
		all_sizes.data(), 1, MPI_INT,
		0, MPI_COMM_WORLD);

	std::vector<int> displs_gather;
	int total_sorted = 0;
	if (_rank == 0) {
		displs_gather.resize(_size);
		displs_gather[0] = 0;
		total_sorted = all_sizes[0];

		for (int i = 1; i < _size; ++i) {
			displs_gather[i] = displs_gather[i - 1] + all_sizes[i - 1];
			total_sorted += all_sizes[i];
		}
		data.resize(total_sorted);
	}

	MPI_Gatherv(recv_buffer.data(), local_sorted_size, MPI_INT,
		_rank == 0 ? data.data() : nullptr,
		_rank == 0 ? all_sizes.data() : nullptr,
		_rank == 0 ? displs_gather.data() : nullptr,
		MPI_INT, 0, MPI_COMM_WORLD);
}
