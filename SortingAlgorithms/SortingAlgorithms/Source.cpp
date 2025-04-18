#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <fstream>
#include <mpi.h>
#include "SortFactory.h"

// Simple JSON writer
class JsonWriter {
private:
    std::ofstream _file;

public:
    JsonWriter(const std::string& filename) {
        _file.open(filename);
        _file << "{\n";
    }

    ~JsonWriter() {
        if (_file.is_open()) {
            _file << "\n}\n";
            _file.close();
        }
    }

    void addField(const std::string& key, const std::string& value, bool addComma = true) {
        _file << "  \"" << key << "\": \"" << value << "\"" << (addComma ? "," : "") << "\n";
    }
};


std::vector<int> ReadData(const std::string& filepath) {
	std::ifstream f(filepath);
	std::vector<int> data;

	if (!f.is_open()) {
		std::cerr << "Error opening file: " << filepath << std::endl;
		return data;
	}

    int number;
    int max_numbers = 999;
    while(f>>number && max_numbers)
    {
        max_numbers--;
		data.push_back(number);
    }
    f.close();
    return data;
    
}

bool IsSorted(const std::vector<int>& data) {
	for (size_t i = 1; i < data.size(); ++i) {
		if (data[i - 1] > data[i]) {
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parse command line arguments
    if (argc < 3 && rank == 0) {
        std::cerr << "Usage: " << argv[0] << " <sort_algorithm> [data_size=10000]" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    std::string algorithmName = argc > 1 ? argv[1] : "SelectionSort";
    int dataSize = argc > 2 ? std::stoi(argv[2]) : 10'000'000;
    std::string filepath = "../../Data/1m_data.txt";

    try {
        std::unique_ptr<SortAlgorithm> sorter = SortFactory::CreateSortAlgorithm(algorithmName);

        std::vector<int> data;
        if (rank == 0) 
        {
            data = ReadData(filepath);
			if (data.empty()) {
				std::cerr << "Error reading data from file: " << filepath << std::endl;
				MPI_Abort(MPI_COMM_WORLD, 1);
				return 1;
			}
        }

        std::cout << data.size();
        MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);


        if (rank != 0) {
            data.resize(dataSize);
        }

        MPI_Bcast(data.data(), dataSize, MPI_INT, 0, MPI_COMM_WORLD);

        double startTime = MPI_Wtime();
        sorter->Sort(data);
        double endTime = MPI_Wtime();

		if (rank == 0) 
        {
			bool sorted = IsSorted(data);
			std::cout << "Array is " << (sorted ? "correctly" : "not") << " sorted." << std::endl;
            if(!sorted)
            {
				std::cout << data.size() << " elements are not sorted correctly: ";
				for (const int& num : data)
				{
                    std::cout << num << " ";
				}
            }
		}
        
        if (rank == 0) {
            double elapsedTime = endTime - startTime;

            // Write results to JSON file
            std::string filename = "sort_results.json";
            JsonWriter writer(filename);
            writer.addField("algorithm", sorter->GetName());
            writer.addField("num_cpus", std::to_string(size));
            writer.addField("data_size", std::to_string(dataSize));
            writer.addField("time_seconds", std::to_string(elapsedTime), false);

            std::cout << "Sorting completed in " << elapsedTime << " seconds" << std::endl;
            std::cout << "Results written to " << filename << std::endl;

            // Check if sorting was successful (for verification)
            bool sorted = true;
            for (int i = 1; i < dataSize; i++) {
                if (data[i - 1] > data[i]) {
                    sorted = false;
                    break;
                }
            }
            std::cout << "Array is " << (sorted ? "correctly" : "not") << " sorted." << std::endl;
        }
    }
    catch (const std::exception& e) {
        if (rank == 0) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}