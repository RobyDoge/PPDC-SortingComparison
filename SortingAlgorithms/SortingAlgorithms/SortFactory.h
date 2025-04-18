#ifndef SORT_FACTORY_H
#define SORT_FACTORY_H

#include <memory>
#include <string>
#include <vector>
#include "ISortingAlgorithm.h"

class SortFactory {
public:
    static std::unique_ptr<ISortingAlgorithm> CreateSortAlgorithm(const std::string& algorithmName);
};

#endif // SORT_FACTORY_H