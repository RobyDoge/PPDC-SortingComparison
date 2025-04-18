#pragma once
#ifndef BUCKETSORT_H
#define BUCKETSORT_H

#include <vector>
#include "ISortingAlgorithm.h"
#include <string>


class BucketSort  : public ISortingAlgorithm
{
public:
	BucketSort();
	~BucketSort() override = default;
	void Sort(std::vector<int>& data) override;
	std::string GetName() const override;
private:
	int _size;
	int _rank;
};


#endif
// BUCKETSORT_H
