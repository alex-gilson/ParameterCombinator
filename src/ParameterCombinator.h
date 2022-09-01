#pragma once
#include <unordered_map>
#include <string>
#include <map>
#include <set>
#include <variant>
#include <memory>
#include <vector>
#include <stdexcept>

using var_t = std::variant<int, float, double, std::string>;
using parameterInstanceMap_t  = std::map<std::string, var_t >;
using parameterCombinations_t = std::unordered_map<std::string, std::vector<var_t> >;
using stringSet_t = std::set<std::string>;
using stringSetMap_t = std::unordered_map<std::string, stringSet_t >;
using parameterTypeMap_t = stringSetMap_t;
using printableParams_t = stringSet_t;
using dontCares_t = std::unordered_map<std::string, stringSetMap_t>;

struct ParameterInstanceSetCompare
{
	dontCares_t dontCares_;
	parameterTypeMap_t parameterTypeMap_;
	ParameterInstanceSetCompare()
		: dontCares_()
		, parameterTypeMap_()
	{};
	ParameterInstanceSetCompare(const dontCares_t& dontCares, const parameterTypeMap_t& parameterTypeMap)
		: dontCares_(dontCares)
		, parameterTypeMap_(parameterTypeMap)
	{};
	ParameterInstanceSetCompare& operator=(const ParameterInstanceSetCompare& other)
	{
		dontCares_ = other.dontCares_;
		parameterTypeMap_ = other.parameterTypeMap_;
		return *this;
	}
	bool operator()(const parameterInstanceMap_t& a, const parameterInstanceMap_t& b) const
	{
		for (auto& param : a)
		{
			bool skip = false;
			for (auto& dontCare : dontCares_)
			{
				const std::string& dontCareKey = dontCare.first;
				if ((param.first == dontCareKey && dontCare.second.empty()) // Total don't care. This parameter is completely ignored
				|| (!dontCare.second.empty() && b.count(dontCareKey) && dontCare.second.count(std::get<std::string>(b.at(dontCareKey)))
				&&  dontCare.second.at(std::get<std::string>(b.at(dontCareKey))).count(param.first))) // Does b care about param.first?
				{
					skip = true;
					break;
				}
			}
			if (skip)
			{
				continue;
			}
			else
			{
				if (parameterTypeMap_.at("string").count(param.first))
				{
					if (std::get<std::string>(param.second) == std::get<std::string>(b.at(param.first)))
						continue;
					else
						return std::get<std::string>(param.second) < std::get<std::string>(b.at(param.first));
				}
				else if (parameterTypeMap_.at("double").count(param.first))
				{
					if (std::get<double>(param.second) == std::get<double>(b.at(param.first)))
						continue;
					else
						return std::get<double>(param.second) < std::get<double>(b.at(param.first));
				}
				else if (parameterTypeMap_.at("int").count(param.first))
				{
					if (std::get<int>(param.second) == std::get<int>(b.at(param.first)))
						continue;
					else
						return std::get<int>(param.second) < std::get<int>(b.at(param.first));
				}
				else
				{
					std::throw_with_nested(std::runtime_error(std::string{ param.first + " is of unkown type. Register it in parameterTypeMap." }));
				}
			}
		}
		return false;
	}
};

using parameterInstanceSet_t = std::set<parameterInstanceMap_t, ParameterInstanceSetCompare>;

class ParameterCombinator
{
public:

	ParameterCombinator(parameterTypeMap_t& parameterTypeMap, printableParams_t& printableParameters);
	ParameterCombinator(const ParameterCombinator& other);
	ParameterCombinator& operator=(const ParameterCombinator& other);
	const parameterInstanceSet_t* getParameterInstanceSet() const;
	void combine(parameterCombinations_t& paramCombs, dontCares_t& dontCares);
	void clearCombinations();
	static ParameterCombinator addCombinators(const ParameterCombinator& leftParamCombinator,
			const ParameterCombinator& rightParamCombinator, const dontCares_t& dontCare);
	parameterTypeMap_t parameterTypeMap_;
	printableParams_t printableParameters_;
private:
	std::vector<std::vector<int64_t>> CartesianProduct(std::vector<std::vector<int64_t>>& sequences);
	void CartesianRecurse(std::vector<std::vector<int64_t>>& accum, std::vector<int64_t> stack,
		std::vector<std::vector<int64_t>> sequences, int64_t index);

	std::unique_ptr<parameterInstanceSet_t> parameterInstanceSet_;

};


