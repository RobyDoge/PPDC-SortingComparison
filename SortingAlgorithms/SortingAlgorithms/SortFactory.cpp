// sort_factory.cpp
#include "SortFactory.h"
#include <stdexcept>

#include "SelectionSort.h"
#include "BucketSort.h"
#include "OddEvenSort.h"
#include "ShellSort.h"
#include "ISortingAlgorithm.h"
#include "RankSort.h"


std::unique_ptr<ISortingAlgorithm> SortFactory::CreateSortAlgorithm(const std::string& algorithmName)
{
    if (algorithmName == "SelectionSort") {
        return std::make_unique<SelectionSort>();
    }

	if (algorithmName == "BucketSort") {
		return std::make_unique<BucketSort>();
	}

	if (algorithmName == "OddEvenSort") {
		return std::make_unique<OddEvenSort>();
	}

	if (algorithmName == "ShellSort") {
		return std::make_unique<ShellSort>();
	}

	if (algorithmName == "RankSort") {
		return std::make_unique<RankSort>();
	}

    // Add more sort algorithms here as needed

    throw std::runtime_error("Unknown sorting algorithm: " + algorithmName);
}
