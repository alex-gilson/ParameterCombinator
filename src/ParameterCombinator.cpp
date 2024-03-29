
#include "ParameterCombinator.h"
#include <sstream>
#include <algorithm>

namespace parameterCombinator
{

	void ParameterCombinator::CartesianRecurse(std::vector<std::vector<Parameter>>& accum, std::vector<Parameter> stack,
		std::vector<std::vector<Parameter>> sequences, int64_t index)
	{
		std::vector<Parameter> sequence = sequences[index];
		for (auto i : sequence)
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

	std::vector<std::vector<Parameter>> ParameterCombinator::CartesianProduct(std::vector<std::vector<Parameter>>& sequences)
	{
		std::vector<std::vector<Parameter>> accum;
		std::vector<Parameter> stack;
		if (sequences.size() > 0) {
			CartesianRecurse(accum, stack, sequences, sequences.size() - 1);
		}
		return accum;
	}

	void ParameterCombinator::combine(parameterCombinations_t& paramCombs, dontCares_t& dontCares)
	{

		// Convert parameterCombinations_t to a vector of vector of ints
		std::vector<std::vector<Parameter>> sequences;
		std::vector<std::string> keyOrder;
		for (auto& param : paramCombs) {
			std::vector<Parameter> seq;
			keyOrder.push_back(param.first);
			for (auto& val : param.second) {
				seq.emplace_back(val);
			}
			sequences.push_back(seq);
		}

		std::vector<std::vector<Parameter>> combinations = CartesianProduct(sequences);

		// Remove repeated combinations taking into account don't care parameters
		ParameterInstanceSetCompare cmp(dontCares);
		*parameterInstanceSet_.get() = parameterInstanceSet_t(cmp);

		for (auto& combination : combinations)
		{
			parameterInstanceMap_t paramInstance;
			auto key = std::prev(keyOrder.end());
			for (auto& param : combination)
			{
				paramInstance[*key] = param;
				key = (key == keyOrder.begin()) ? std::prev(keyOrder.end()) : std::prev(key);
			}
			// Remove paramter instance values that are irrelevant to the combination
			for (auto& dontCare : dontCares)
			{
				const std::string& dontCareKey = dontCare.first;
				if (!paramInstance.count(dontCareKey))
				{
					continue;
				}
				Parameter dontCareVal = paramInstance[dontCareKey];
				// Accesing dontCare.second[dontCareVal] results in an increment in the size of the dontCare[dontCareVal].
				// This is problematic for total don't cares.
				// Check size first and only access it if size is larger or equal to one.
				if (dontCare.second.count(dontCareVal))
				{
					for (auto& paramName : dontCare.second[dontCareVal])
					{
						paramInstance.erase(paramName);
					}
				}
				// Total don't care
				if (!dontCare.second.size())
				{
					paramInstance.erase(dontCareKey);
				}
			}
			parameterInstanceSet_->insert(paramInstance);
		}
	}

	ParameterCombinator::ParameterCombinator()
	{
		ParameterInstanceSetCompare cmp(dontCares_t{});
		parameterInstanceSet_ = std::make_shared<parameterInstanceSet_t>(cmp);
	}

	ParameterCombinator::ParameterCombinator(const ParameterCombinator& other)
	{
		parameterInstanceSet_ = other.parameterInstanceSet_;
	}

	ParameterCombinator& ParameterCombinator::operator=(const ParameterCombinator& other)
	{
		parameterInstanceSet_ = other.parameterInstanceSet_;
		return *this;
	}

	void ParameterCombinator::clearCombinations()
	{
		parameterInstanceSet_.get()->clear();
	}

	const parameterInstanceSet_t* ParameterCombinator::getParameterInstanceSet() const
	{
		return parameterInstanceSet_.get();
	}

	void ParameterCombinator::addCombinations(ParameterCombinator& paramCombinator1, ParameterCombinator& paramCombinator2, const dontCares_t& dontCares)
	{
		const parameterInstanceSet_t* paramSet1 = paramCombinator1.getParameterInstanceSet();
		const parameterInstanceSet_t* paramSet2 = paramCombinator2.getParameterInstanceSet();

		ParameterInstanceSetCompare cmp(dontCares);
		parameterInstanceSet_t sumParamSet(cmp);

		for (auto& paramSet : { *paramSet1, *paramSet2 })
		{
			for (auto& paramInstance : paramSet)
			{
				sumParamSet.insert(paramInstance);
			}
		}
		*parameterInstanceSet_ = std::move(sumParamSet);
	}
	std::string ParameterCombinator::generateCombinationName(const parameterInstanceMap_t& paramInstance)
	{
		std::string combinationName;
		for (auto [paramName, paramValue] : paramInstance)
		{
			combinationName += paramName + "_" + paramValue->toString() + "_";
		}

		// Remove trailing "_"
		size_t lastIndex = combinationName.find_last_of("_");
		combinationName = combinationName.substr(0, lastIndex);

		std::replace(combinationName.begin(), combinationName.end(), '.', '_');

		return combinationName;
	}

} // Namespace parameterCombinator

