// selection_sort.h
#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

#include <vector>
#include <string>
#include <mpi.h>

class SelectionSort
{
public:
    SelectionSort();
    ~SelectionSort();
    void Sort(std::vector<int>& data);
    std::string GetName() const;

private:
    int _size;      
    int _rank;
	std::vector<std::vector<int>> _sortedSections;
};

#endif // SELECTION_SORT_H