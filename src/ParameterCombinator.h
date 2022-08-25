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
using stringSetMap_t = std::unordered_map<std::string, std::set<std::string> >;
using parameterTypeMap_t = stringSetMap_t;
using printableParams_t = std::set<std::string>;
using dontCares_t = std::unordered_map<std::string, std::unordered_map<std::string, stringSet_t>>;

// If stringSetMap_t is empty, ignore all comparisons of dontCareKey_t
// Else just the ones specific to that dontCareKey_t element
//using dontCares_t = std::vector<std::pair<dontCareKey_t, stringSetMap_t > >;

struct ParameterInstanceSetCompare
{
	const dontCares_t dontCares_;
	const parameterTypeMap_t parameterTypeMap_;
	ParameterInstanceSetCompare()
		: dontCares_()
		, parameterTypeMap_()
	{};
	ParameterInstanceSetCompare(const dontCares_t& dontCares, const parameterTypeMap_t& parameterTypeMap)
		: dontCares_(dontCares)
		, parameterTypeMap_(parameterTypeMap)
	{};
	bool operator()(const parameterInstanceMap_t& a, const parameterInstanceMap_t& b) const
	{
		for (auto& param : a)
		{
			bool skip = false;
			for (auto& dontCare : dontCares_)
			{
				const std::string& dontCareKey = dontCare.first;
				if ((param.first == dontCareKey && dontCare.second.empty()) // Total don't care. This parameter is completely ignored
				|| (!dontCare.second.empty() && dontCare.second.count(std::get<std::string>(b.at(dontCareKey))) // Check if dontCareKey exists in map
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

	ParameterCombinator(parameterCombinations_t& paramCombs, dontCares_t& dontCares,
		parameterTypeMap_t& parameterTypeMap, printableParams_t& printableParameters);
	ParameterCombinator(const ParameterCombinator& other);
	std::string constructVariationName(const parameterInstanceMap_t& paramInstance);
	const parameterInstanceSet_t& getParameterInstanceSet();
	const parameterCombinations_t* getParameterCombinations();
	void addParametersWithoutRecombining(parameterCombinations_t& paramCombs, dontCares_t& dontCares,
		std::string& dontCareKey, parameterTypeMap_t& parameterTypeMap);
private:
	std::vector<std::vector<int64_t>> CartesianProduct(std::vector<std::vector<int64_t>>& sequences);
	void CartesianRecurse(std::vector<std::vector<int64_t>>& accum, std::vector<int64_t> stack,
		std::vector<std::vector<int64_t>> sequences, int64_t index);

	parameterCombinations_t paramCombs_;
	parameterTypeMap_t parameterTypeMap_;
	printableParams_t printableParameters_;
	std::unique_ptr<parameterInstanceSet_t> parameterInstanceSet_;

};
