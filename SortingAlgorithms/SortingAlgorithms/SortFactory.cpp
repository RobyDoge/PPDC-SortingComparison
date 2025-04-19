// sort_factory.cpp
#include "SortFactory.h"
#include <stdexcept>

#include "SelectionSort.h"
#include "BucketSort.h"
#include "OddEvenSort.h"
#include "ISortingAlgorithm.h"


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

    // Add more sort algorithms here as needed

    throw std::runtime_error("Unknown sorting algorithm: " + algorithmName);
}