import json
import matplotlib.pyplot as plt
import math

def create_graph(algorithm_name, cpus, times, theoretical_times):
    plt.figure(figsize=(10, 6))
    plt.plot(cpus, times, 'o-', label='Measured Total Time')
    plt.plot(cpus, theoretical_times, 'r--', label='Ideal Speedup (1/n)')

    plt.xlabel('Number of CPUs')
    plt.ylabel('Total Time (seconds)')
    plt.title(f'Performance of {algorithm_name} Algorithm')
    plt.legend()
    plt.grid(True)
    plt.xscale('log', base=2)
    plt.xticks(cpus, cpus)  # Show all CPU counts as ticks
    plt.tight_layout()
    plt.savefig(f'Graphs/{algorithm_name}_performance.png')

def create_graphs_for_all_algorithms(data):
    algorithms = set(entry['algorithm'] for entry in data)
    for algorithm_name in algorithms:
        filtered_data = [entry for entry in data if entry['algorithm'] == algorithm_name]
        filtered_data.sort(key=lambda x: x['numCpus'])

        cpus = [entry['numCpus'] for entry in filtered_data]
        times = [entry['totalTime'] for entry in filtered_data]

        # Theoretical speedup line
        baseline_cpus = cpus[0]
        baseline_time = times[0]
        theoretical_times = [baseline_time * (baseline_cpus / c) for c in cpus]

        create_graph(algorithm_name, cpus, times, theoretical_times)

def load_and_create_graphs(file_path):
    with open(file_path, 'r') as f:
        data = json.load(f)
    create_graphs_for_all_algorithms(data)

if __name__ == "__main__":
    file_path = r'D:\1FACULTATE\Anu_III\PPDC\PPDC-SortingComparison\SortingAlgorithms\SortingAlgorithms\sort_results.json'  # Path to old Json file
    load_and_create_graphs(file_path)
    print("Graphs created successfully.")