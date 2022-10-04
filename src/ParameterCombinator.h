#pragma once
#include "Parameters.h"
#include <memory>

class ParameterCombinator
{
public:

	ParameterCombinator();
	ParameterCombinator(const ParameterCombinator& other);
	ParameterCombinator& operator=(const ParameterCombinator& other);
	const parameterInstanceSet_t* getParameterInstanceSet() const;
	void combine(parameterCombinations_t& paramCombs, dontCares_t& dontCares);
	void clearCombinations();
private:
	std::vector<std::vector<Parameter>> CartesianProduct(std::vector<std::vector<Parameter>>& sequences);
	void CartesianRecurse(std::vector<std::vector<Parameter>>& accum, std::vector<Parameter> stack,
		std::vector<std::vector<Parameter>> sequences, int64_t index);

	std::shared_ptr<parameterInstanceSet_t> parameterInstanceSet_;

};


