// sort_factory.cpp
#include "SortFactory.h"
#include "SelectionSort.h"
#include <stdexcept>


class SelectionSortAdapter : public SortAlgorithm
{

public:
    void Sort(std::vector<int>& data) override
	{
        _selectSort.Sort(data);
    }

    std::string GetName() const override {
        return _selectSort.GetName();
    }
private:
    SelectionSort _selectSort;
};

// Factory method to create sort algorithms
std::unique_ptr<SortAlgorithm> SortFactory::CreateSortAlgorithm(const std::string& algorithmName)
{
    if (algorithmName == "SelectionSort") {
        return std::make_unique<SelectionSortAdapter>();
    }

    // Add more sort algorithms here as needed

    throw std::runtime_error("Unknown sorting algorithm: " + algorithmName);
}