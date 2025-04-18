#ifndef SORT_FACTORY_H
#define SORT_FACTORY_H

#include <memory>
#include <string>
#include <vector>

// Forward declaration
class SelectionSort;

class SortAlgorithm {
public:
    virtual ~SortAlgorithm() = default;
    virtual void Sort(std::vector<int>& data) = 0;
    virtual std::string GetName() const = 0;
};

class SortFactory {
public:
    static std::unique_ptr<SortAlgorithm> CreateSortAlgorithm(const std::string& algorithmName);
};

#endif // SORT_FACTORY_H