from mpi4py import MPI
import numpy as np
import time

def selection_sort(arr):
    """Sequential selection sort implementation"""
    n = len(arr)
    for i in range(n):
        min_idx = i
        for j in range(i+1, n):
            if arr[j] < arr[min_idx]:
                min_idx = j
        arr[i], arr[min_idx] = arr[min_idx], arr[i]
    return arr

def parallel_selection_sort():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()
    
    # Root process read data and distributes it
    if rank == 0:
        data = np.loadtxt('1m_data.txt', dtype=int)
        n = len(data)

        print(f"Data Finish Being Read")

        # Record start time
        start_time = time.time()
        
        # Calculate chunk size for each process
        chunk_size = n // size
        print(f"Chunk size: {chunk_size}")
        remainder = n % size
        
        # Store the starting position for each chunk
        positions = [0]
        for i in range(size):
            positions.append(positions[-1] + chunk_size + (1 if i < remainder else 0))
        
        # Split data for each process
        chunks = [data[positions[i]:positions[i+1]] for i in range(size)]
        print(f"Data split into {len(chunks)} chunks")
    else:
        chunks = None
    
    # Scatter data to all processes
    local_chunk = comm.scatter(chunks, root=0)
    print(f"Process {rank} received chunk: {len(local_chunk)} elements")

    # Each process sorts its local chunk
    local_sorted = selection_sort(local_chunk)
    
    # Gather all sorted chunks back to the root
    sorted_chunks = comm.gather(local_sorted, root=0)
    
    if rank == 0:
        # Merge the sorted chunks
        final_sorted = []
        while any(len(chunk) > 0 for chunk in sorted_chunks):
            min_val = float('inf')
            min_idx = -1
            
            # Find the minimum value across all chunks
            for i in range(size):
                if len(sorted_chunks[i]) > 0 and sorted_chunks[i][0] < min_val:
                    min_val = sorted_chunks[i][0]
                    min_idx = i
            
            # Add the minimum value to the final sorted array
            final_sorted.append(min_val)
            # Remove the minimum value from its chunk
            sorted_chunks[min_idx] = sorted_chunks[min_idx][1:]
        
        # Record end time
        end_time = time.time()
        
        print(f"Sorted array (first 10 elements): {final_sorted[:10]}")
        print(f"Time taken: {end_time - start_time:.4f} seconds")
        print(f"Number of processes used: {size}")

if __name__ == "__main__":
    parallel_selection_sort()