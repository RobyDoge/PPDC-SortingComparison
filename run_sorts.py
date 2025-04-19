import subprocess
from create_graphs import load_and_create_graphs
sorting_algorithms = [
    'RankSort',
    'SelectionSort',
    'OddEvenSort',
    'ShellSort',
    'BucketSort',

]

num_cpu = [2,4,8,16]


def run_sort(sort, cpu):
    exec_location = r"D:\1FACULTATE\Anu_III\PPDC\PPDC-SortingComparison\SortingAlgorithms\x64\Debug\SortingAlgorithms.exe"
    data_location = r"D:\1FACULTATE\Anu_III\PPDC\PPDC-SortingComparison\Data\10m_data.txt"
    cmd = [
        'mpiexec.exe',
        '-np', str(cpu),
        exec_location,
        sort,
        data_location,
    ]
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running {sort} with {cpu} CPUs:\n{e.stderr}")


def main():
    for sort in sorting_algorithms:
        for cpu in num_cpu:
            print(f"Running {sort} with {cpu} CPUs")
            run_sort(sort, cpu)
    fielpath = 'sort_results.json'
    load_and_create_graphs(fielpath)
    print("Graphs created successfully.")

if __name__ == "__main__":
    main()