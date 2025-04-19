#pragma once
#include <iostream>

#include "ISortingAlgorithm.h"
class RankSort :
    public ISortingAlgorithm
{
public:
	RankSort();
	~RankSort() override = default;
	void Sort(std::vector<int>& data, double& commTime) override;
	std::string GetName() const override { return "RankSort"; }
	
private:
	int _rank;
	int _size;
};

