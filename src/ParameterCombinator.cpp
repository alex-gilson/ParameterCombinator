
#include "ParameterCombinator.h"
#include <sstream>


void ParameterCombinator::CartesianRecurse(std::vector<std::vector<int64_t>> &accum, std::vector<int64_t> stack,
	std::vector<std::vector<int64_t>> sequences,int64_t index)
{
	std::vector<int64_t> sequence = sequences[index];
	for (int64_t i : sequence)
	{
		stack.push_back(i);
		if (index == 0) {
			accum.push_back(stack);
		}
		else {
			CartesianRecurse(accum, stack, sequences, index - 1);
		}
		stack.pop_back();
	}
}

std::vector<std::vector<int64_t>> ParameterCombinator::CartesianProduct(std::vector<std::vector<int64_t>>& sequences)
{
	std::vector<std::vector<int64_t>> accum;
	std::vector<int64_t> stack;
	if (sequences.size() > 0) {
		CartesianRecurse(accum, stack, sequences, sequences.size() - 1);
	}
	return accum;
}

ParameterCombinator::ParameterCombinator(parameterCombinations_t& paramCombs, dontCares_t& dontCares,
	parameterTypeMap_t& parameterTypeMap, printableParams_t& printableParameters)
	: paramCombs_(paramCombs)
	, parameterTypeMap_(parameterTypeMap)
	, printableParameters_(printableParameters)
{
	// Convert parameterCombinations_t to a vector of vector of ints
	std::vector<std::vector<int64_t>> sequences;
	for (auto& param : paramCombs) {
		std::vector<int64_t> seq;
		if (parameterTypeMap_["string"].count(param.first))
		{
			for (auto& val : param.second) {
				std::string* strAddress = &std::get<std::string>(val);
				seq.push_back(reinterpret_cast<int64_t&>(strAddress));
			}
		}
		else
		{
			for (auto& val : param.second) {
				seq.push_back(reinterpret_cast<int64_t&>(val));
			}
		}
		sequences.push_back(seq);
	}

	std::vector<std::vector<int64_t>> res = CartesianProduct(sequences);

	// Eliminate duplicates
	std::set<std::vector<int64_t>> resSet;
	for (auto& v : res) {
		resSet.insert(v);
	}

	// Create parameter combinations with the combinatorial generated above
	parameterCombinations_t newParamCombs;
	for (auto& set : resSet) {
		int i = 0;
		for (auto& param : paramCombs)
		{
			// Reverse the set and insert
			if (parameterTypeMap_["double"].count(param.first))
			{
				int64_t a = static_cast<int64_t>(set[set.size() - i - 1]);
				double b = reinterpret_cast<double&>(a);
				newParamCombs[param.first].push_back(b);
			}
			else if (parameterTypeMap_["string"].count(param.first))
			{
				int64_t a = static_cast<int64_t>(set[set.size() - i - 1]);
				std::string* b = reinterpret_cast<std::string*>(a);
				newParamCombs[param.first].push_back(*b);
			}
			else
			{
				auto a = static_cast<int>(set[set.size() - i - 1]);
				newParamCombs[param.first].push_back(a);
			}
			i++;
		}
	}

	// Remove repeated combinations taking into account don't care parameters
	ParameterInstanceSetCompare cmp(dontCares, parameterTypeMap_);
	parameterInstanceSet_ = std::make_unique<parameterInstanceSet_t>(cmp);

	size_t combSize = newParamCombs[newParamCombs.begin()->first].size();

	for (size_t paramIdx = 0; paramIdx < combSize; paramIdx++)
	{
		// Select the parameters to use for this iteration of the test
		parameterInstanceMap_t paramInstance;
		for (auto& [paramName, paramValues] : newParamCombs)
		{
			paramInstance[paramName] = paramValues[paramIdx];
		}
		// Remove paramter instance values that are irrelevant to the combination
		for (auto& dontCare : dontCares)
		{
			const std::string& dontCareKey = dontCare.first;
			std::string dontCareVal = std::get<std::string>(paramInstance[dontCareKey]);
			for (auto& paramName : dontCare.second[dontCareVal])
			{
				paramInstance.erase(paramName);
			}
		}
		parameterInstanceSet_->insert(paramInstance);
	}
}

ParameterCombinator::ParameterCombinator(const ParameterCombinator& other)
	: parameterTypeMap_(other.parameterTypeMap_)
	, printableParameters_(other.printableParameters_)
{
	parameterInstanceSet_ = std::make_unique<parameterInstanceSet_t>(*other.parameterInstanceSet_.get());
}

std::string ParameterCombinator::constructVariationName(const parameterInstanceMap_t& paramInstance)
{
	std::string variationName;
	for (auto [paramName, paramValue] : paramInstance)
	{
		if (!printableParameters_.count(paramName))
		{
			continue;
		}
		if (paramName == "inputFile" || paramName == "signal")
		{
			variationName += std::get<std::string>(paramValue) + "_";
		}
		else if (paramName == "algo")
		{
			variationName += paramName + "_" + std::get<std::string>(paramValue) + "_";
		} 
		else if (parameterTypeMap_["double"].count(paramName))
		{
			std::stringstream ss;
			ss << std::get<double>(paramValue) << std::scientific;
			variationName += paramName + "_" + ss.str() + "_";
		}
		else
		{
			variationName += paramName + "_" + std::to_string(std::get<int>(paramValue)) + "_";
		}
	}

	// Remove trailing "_"
	size_t lastIndex = variationName.find_last_of("_");
	variationName = variationName.substr(0, lastIndex);

	return variationName;

}

const parameterInstanceSet_t& ParameterCombinator::getParameterInstanceSet()
{
	return *parameterInstanceSet_.get();
}

const parameterCombinations_t* ParameterCombinator::getParameterCombinations()
{
	return &paramCombs_;
}

void ParameterCombinator::addParametersWithoutRecombining(parameterCombinations_t& paramCombs, dontCares_t& dontCares,
	std::string& dontCareKey, parameterTypeMap_t& parameterTypeMap)
{
	for (size_t paramIdx = 0; paramIdx < paramCombs["freq"].size(); paramIdx++)
	{
		// Copy all parameterInstanceMaps with the new paramCombs modification and reinsert it into the set
		for (auto parameterInstanceMap : *parameterInstanceSet_.get())
		{
			parameterInstanceMap["freq"] = paramCombs["freq"][paramIdx];
			parameterInstanceSet_->insert(parameterInstanceMap);
		}
		parameterInstanceMap_t paramInstance;
		for (auto& [paramName, paramValues] : paramCombs) {
			paramInstance[paramName] = paramValues[paramIdx];
		}
		parameterInstanceSet_->insert(paramInstance);
	}

}
