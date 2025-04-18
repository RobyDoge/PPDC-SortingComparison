#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

#include <vector>
#include <string>
#include <mpi.h>
#include "ISortingAlgorithm.h"

class SelectionSort : public ISortingAlgorithm
{
public:
    SelectionSort();
	~SelectionSort() override = default; 
    void Sort(std::vector<int>& data) override;
    std::string GetName() const override;


private:
    static std::vector<int> MergeData(std::vector<std::vector<int>>& vectors);

private:
    int _size;      
    int _rank;
	std::vector<std::vector<int>> _sortedSections;
};

#endif // SELECTION_SORT_H