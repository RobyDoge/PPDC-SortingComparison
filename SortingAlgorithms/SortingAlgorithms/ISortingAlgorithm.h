#pragma once
#include <vector>
#include <__msvc_ostream.hpp>

class ISortingAlgorithm
{
public:
	virtual ~ISortingAlgorithm() = default;
	virtual void Sort(std::vector<int>& data, double& commTime) = 0;
	virtual std::string GetName() const = 0;
};
