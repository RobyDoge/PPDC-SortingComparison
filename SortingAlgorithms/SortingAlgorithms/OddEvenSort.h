#pragma once
#include "ISortingAlgorithm.h"
class OddEvenSort :
    public ISortingAlgorithm
{
public:
	OddEvenSort();
	~OddEvenSort() override = default;
	void Sort(std::vector<int>& data, double& commTime) override;
	std::string GetName() const override;
private:
	int _size;
	int _rank;
};

