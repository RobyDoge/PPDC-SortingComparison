#pragma once
#include "ISortingAlgorithm.h"


class ShellSort :
    public ISortingAlgorithm
{
public:
	ShellSort();
	~ShellSort() override = default;
	void Sort(std::vector<int>& data, double& commTime) override;
	std::string GetName() const override { return "ShellSort"; }

private:
	int _size;
	int _rank;
};

