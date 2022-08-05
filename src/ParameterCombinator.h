#pragma once
#include <unordered_map>
#include <string>
#include <map>
#include <set>
#include <variant>
#include <memory>
#include <vector>

#ifdef USE_DOUBLE
typedef double my_float;
#else
typedef float my_float;
#endif

using var_t = std::variant<int, my_float, std::string>;
using parameterInstanceMap_t  = std::map<std::string, var_t >;
using parameterCombinations_t = std::unordered_map<std::string, std::vector<var_t> >;
using stringSetMap_t = std::unordered_map<std::string, std::set<std::string> >;
using parameterTypeMap_t = stringSetMap_t;
using printableParams_t = std::set<std::string>;
using dontCareKey_t = std::string;
// If stringSetMap_t is empty, ignore all comparisons of dontCareKey_t
// Else just the ones specific to that dontCareKey_t element
//using dontCares_t = std::vector<std::pair<dontCareKey_t, stringSetMap_t > >;

using dontCares_t = stringSetMap_t;

struct ParameterInstanceSetCompare
{
	const dontCares_t dontCares_;
	const std::string dontCareKey_;
	const parameterTypeMap_t parameterTypeMap_;
	ParameterInstanceSetCompare()
		: dontCares_()
		, dontCareKey_()
		, parameterTypeMap_()
	{};
	ParameterInstanceSetCompare(const dontCares_t& dontCares, const std::string& dontCareKey, const parameterTypeMap_t& parameterTypeMap)
		: dontCares_(dontCares)
		, dontCareKey_(dontCareKey)
		, parameterTypeMap_(parameterTypeMap)
	{};
	bool operator()(const parameterInstanceMap_t& a, const parameterInstanceMap_t& b) const
	{
		for (auto& param : a)
		{
			// If dontCares_ is empty, ignore all comparisons of the dontCareKey
			// Else just the ones specific to that dontCareKey element
			//if (!dontCareKey_.empty() && dontCares_.at(std::get<std::string>(b.at(dontCareKey_))).count(param.first))
			if ((dontCares_.empty() && dontCareKey_ == param.first) ||
			   (!dontCareKey_.empty() && dontCares_.at(std::get<std::string>(b.at(dontCareKey_))).count(param.first)))
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
				if (parameterTypeMap_.at("double").count(param.first))
				{
					if (std::get<my_float>(param.second) == std::get<my_float>(b.at(param.first)))
						continue;
					else
						return std::get<my_float>(param.second) < std::get<my_float>(b.at(param.first));
				}
				else
				{
					if (std::get<int>(param.second) == std::get<int>(b.at(param.first)))
						continue;
					else
						return std::get<int>(param.second) < std::get<int>(b.at(param.first));
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

	ParameterCombinator(parameterCombinations_t& paramCombs, dontCares_t& dontCares, std::string& dontCareKey,
		parameterTypeMap_t& parameterTypeMap, printableParams_t& printableParameters);
	ParameterCombinator(const ParameterCombinator& other);
	std::string constructVariationName(const parameterInstanceMap_t& paramInstance);
	const parameterInstanceSet_t* getParameterInstanceSet();
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
