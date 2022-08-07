#include "ParameterCombinator.h"
#include <iostream>

bool testSimpleCombination()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	dontCares_t             dontCares;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["A"] = { "one", "two" };
	paramCombs["B"] = { 1 };
	paramCombs["C"] = { 1.1, 2.2, 3.3 };

	// List of parameters to be ignored in the combination
	std::string dontCareKey = "";

	// Type of each of the parameters
	parameterTypeMap["string"] = { "A" };
	parameterTypeMap["double"] = { "C" };
	parameterTypeMap["int"]    = { "B" };

	ParameterCombinator paramCombinator(paramCombs, dontCares, dontCareKey, parameterTypeMap, printableParams);

	ParameterInstanceSetCompare cmp(dontCares, dontCareKey, parameterTypeMap);
	auto expectedCombinationsSet = std::make_unique<parameterInstanceSet_t>(cmp);

	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"A", "one"}, {"B", 1}, {"C", 1.1}},
		{{"A", "one"}, {"B", 1}, {"C", 2.2}},
		{{"A", "one"}, {"B", 1}, {"C", 3.3}},
		{{"A", "two"}, {"B", 1}, {"C", 1.1}},
		{{"A", "two"}, {"B", 1}, {"C", 2.2}},
		{{"A", "two"}, {"B", 1}, {"C", 3.3}},
	};

	const parameterInstanceSet_t& paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet.size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet.count(paramInstanceMap));
	}

	return failed;

}

bool testCombinationWithDontCare()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	dontCares_t             dontCares;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["A"] = { "one", "two" };
	paramCombs["B"] = { 1 };
	paramCombs["C"] = { 1.1, 2.2, 3.3 };
	paramCombs["D"] = { "three", "four"};

	// List of parameters to be ignored in the combination
	// TODO: support for dontCares with other key types
	std::string dontCareKey  = "A";
	dontCares["one"]         = { "C" };
	dontCares["two"]         = { "D" };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "A", "D" };
	parameterTypeMap["double"] = { "C" };
	parameterTypeMap["int"]    = { "B" };

	ParameterCombinator paramCombinator(paramCombs, dontCares, dontCareKey, parameterTypeMap, printableParams);

	ParameterInstanceSetCompare cmp(dontCares, dontCareKey, parameterTypeMap);
	auto expectedCombinationsSet = std::make_unique<parameterInstanceSet_t>(cmp);
	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"A", "one"}, {"B", 1}, {"D", "three"}},
		{{"A", "one"}, {"B", 1}, {"D", "four"}},
		{{"A", "two"}, {"B", 1}, {"C", 1.1}},
		{{"A", "two"}, {"B", 1}, {"C", 2.2}},
		{{"A", "two"}, {"B", 1}, {"C", 3.3}},
	};

	const parameterInstanceSet_t& paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet.size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet.count(paramInstanceMap));
	}

	return failed;

}

int main()
{
	bool failed;

	failed = testSimpleCombination();
	if (failed) std::cout << "testSimpleCombination failed" << std::endl;

	failed = testCombinationWithDontCare();
	if (failed) std::cout << "testCombinationWithDontCare failed" << std::endl;

    return failed;
}