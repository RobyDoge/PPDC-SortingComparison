#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <fstream>
#include <mpi.h>
#include "SortFactory.h"
#include "JsonWriter.h"
// Simple JSON writer

std::vector<int> ReadData(const std::string& filepath) {
	std::ifstream f(filepath);
	std::vector<int> data;

	if (!f.is_open()) {
		std::cerr << "Error opening file: " << filepath << std::endl;
		return data;
	}

    int number;
    int max_numbers = 1000;
    while(f>>number/* && max_numbers*/)
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

    std::string algorithmName = argc > 1 ? argv[1] : "SelectionSort";
    std::string filepath = "../../Data/1m_data.txt";
	int dataSize = filepath.contains("10") ? 10'000'000 : 1'000'000;
   

    try {
        std::unique_ptr<ISortingAlgorithm> sorter = SortFactory::CreateSortAlgorithm(algorithmName);

        std::vector<int> data;
        if (rank == 0) 
        {
            data = ReadData(filepath);
			if (data.empty()) {
				std::cerr << "Error reading data from file: " << filepath << std::endl;
				MPI_Abort(MPI_COMM_WORLD, 1);
				return 1;
			}
            std::cout<< "Data done reading\n"<<std::endl;
        }
        MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        

        if (rank != 0) {
            data.resize(dataSize);
        }

        MPI_Bcast(data.data(), dataSize, MPI_INT, 0, MPI_COMM_WORLD);

        double startTotalTime = MPI_Wtime();
        double commTime{};
        sorter->Sort(data,commTime);
        double endTotalTime = MPI_Wtime();

		if (rank == 0) 
        {
			bool sorted = IsSorted(data);
			std::cout << "Array is " << (sorted ? "correctly" : "not") << " sorted." << std::endl<<std::endl;
            if(!sorted)
            {
				std::cout << data.size() << " elements are not sorted correctly: ";
				for (const int& num : data)
				{
                    std::cout << num << " ";
				}
                MPI_Abort(MPI_COMM_WORLD, 1);
                return 1;
            }
		}
        
        if (rank == 0) {
            double elapsedTime = endTotalTime - startTotalTime;
            std::cout<<"Saving Info";
            // Write results to JSON file
            std::string filename = "sort_results.json";
            JsonWriter writer(filename);
			writer.AddEntry(sorter->GetName(), size, dataSize, elapsedTime, commTime);
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