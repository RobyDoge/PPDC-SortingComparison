import subprocess
import time 


def run_sort(script_path, num_processes):
    command = [
        "mpiexec",
        "-n",
        str(num_processes),
        "python",
        script_path
    ]
    try:
        print(f"Running '{script_path}' with {num_processes} processes...")
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            print("MPI script executed successfully.")
            print("Output:\n", stdout.decode())
        else:
            print(f"MPI script failed with error code {process.returncode}:")
            print("Error:\n", stderr.decode())

    except FileNotFoundError:
        print("Error: mpiexec command not found. Make sure MPI is installed and in your PATH.")
    except Exception as e:
        print(f"An error occurred: {e}")



num_processes = [4,8,16,32]
sort_scripts = [
    "mpi_selection_sort.py",
    ]


def main():
    for script in sort_scripts:
        for num_process in num_processes:
            run_sort(script, num_process)
            time.sleep(5)
        print ("Finished running all processes for", script)
        print("-" * 50)
    print("All scripts executed successfully.")

          
        

