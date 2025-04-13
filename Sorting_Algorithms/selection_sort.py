from mpi4py import MPI
import numpy as np
import time
from save_result import safe_result


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

def parallel_sort():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()
    
    if rank == 0:
        filepath = 'D:/1FACULTATE/Anu_III/PPDC/PPDC-SortingComparison/Data/1m_data.txt'
        data = np.loadtxt(filepath, dtype=int)
        n = len(data)
        if n!= 1000000:
            raise ValueError(f"Data length is {n}, expected 1,000,000")
        print(f"Data Finish Being Read")
        print(size)
        # Record start time
        start_time = time.time()
        
        # Calculate chunk size for each process
        chunk_size = n // size
        remainder = n % size
        
        # Store the starting position for each chunk
        positions = [0]
        for i in range(size):
            positions.append(positions[-1] + chunk_size + (1 if i < remainder else 0))
        
        # Split data for each process
        chunks = [data[positions[i]:positions[i+1]] for i in range(size)]
    else:
        chunks = None
    
    # Scatter data to all processes
    local_chunk = comm.scatter(chunks, root=0)
    # Each process sorts its local chunk
    local_sorted = selection_sort(local_chunk)
    
    # Gather all sorted chunks back to the root
    sorted_chunks = comm.gather(local_sorted, root=0)
    
    if rank == 0:
        print(f"Data Finish Being Sorted")
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
        elapsed_time = end_time - start_time
        safe_result(elapsed_time, "Selection Sort", size)