import json


file_path = 'results.json'

def safe_result(time, sort_type, num_processes):
    # Create a dictionary to hold the results
    result = {
        "sort_type": sort_type,
        "num_processes": num_processes,
        "time": time,
    }

    # Write the result to a JSON file
    with open(file_path, 'a') as f:
        json.dump(result, f)
        f.write('\n')