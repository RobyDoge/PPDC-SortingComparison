#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <fstream>
#include <mpi.h>
#include "SortFactory.h"
#include "JsonWriter.h"


std::vector<int> ReadData(const std::string& filepath) {
	std::ifstream f(filepath);
	std::vector<int> data;

	if (!f.is_open()) {
		std::cerr << "Error opening file: " << filepath << std::endl;
		return data;
	}

    int number;
    while(f>>number)
    {
		data.push_back(number);
    }
    f.close();
    return data;
    
}

int IsSorted(const std::vector<int>& data) {
	for (size_t i = 1; i < data.size(); ++i) {
		if (data[i - 1] > data[i]) {
			return i;
		}
	}
	return -1;
}

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string algorithmName = argc > 1 ? argv[1] : "SelectionSort";
    std::string filepath = argc > 2 ? argv[2] : "../../Data/1m_data.txt";
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
            std::cout << "Data done reading\n" << std::endl;
        }

        double commTime{}, startTime = MPI_Wtime();
        MPI_Bcast(&dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		commTime += MPI_Wtime() - startTime;

        if (rank != 0) {
            data.resize(dataSize);
        }

		startTime = MPI_Wtime();
        MPI_Bcast(data.data(), dataSize, MPI_INT, 0, MPI_COMM_WORLD);
		commTime += MPI_Wtime() - startTime;

        double startTotalTime = MPI_Wtime();
        double sortCommTime{};
        sorter->Sort(data, sortCommTime);
        double endTotalTime = MPI_Wtime();
		if (rank == 0)
        {

			int sorted = IsSorted(data);
			std::cout << "Array is " << (sorted == -1 ? "correctly" : "not") << " sorted." <<std::endl;
            if(sorted != -1)
            {
                std::cout << "The problem happened at index: " << sorted << std::endl;
				for (int i = sorted - 2; i <= sorted + 2; i++)
				{
					std::cout << data[i] << " ";
				}
				std::cout << std::endl;

				//for (auto i : data)
				//{
				//	std::cout << i << " ";
				//}
				//std::cout << std::endl;


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
			writer.AddEntry(sorter->GetName(), size, dataSize, elapsedTime, commTime+sortCommTime, elapsedTime-sortCommTime);
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