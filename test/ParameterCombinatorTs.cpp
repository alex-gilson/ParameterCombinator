#include "ParameterCombinator.h"
#include <iostream>

bool testSimpleCombination()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100, 120, 130 };
	paramCombs["airbag"]     = { 0, 1 };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "vehicle" };
	parameterTypeMap["int"]    = { "horsepower", "airbag" };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares{};

	ParameterCombinator paramCombinator(parameterTypeMap, printableParams);
	paramCombinator.combine(paramCombs, dontCares);

	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"airbag", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"airbag", 1}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"airbag", 0}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"airbag", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"airbag", 0}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"airbag", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"airbag", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}, {"airbag", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 1}},
	};

	const parameterInstanceSet_t* paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet->count(paramInstanceMap));
	}

	return failed;

}

bool testCombinationWithDontCare()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["vehicle"]        = { "car", "motorbike" };
	paramCombs["horsepower"]     = { 100, 130 };
	paramCombs["AC"]             = { 0, 1 };
	paramCombs["wind-protector"] = { 0, 1 };
	paramCombs["wing-length"]    = { 30.3 };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "vehicle" };
	parameterTypeMap["double"] = { "wing-length" };
	parameterTypeMap["int"]    = { "horsepower", "AC", "wind-protector" };

	// List of parameters to be ignored in the combination
	// TODO: support for dontCares with other key types
	dontCares_t dontCares =
	{
		{"vehicle",
			{
				{"car",
					{"wind-protector", "wing-length"}
				},
				{"motorbike",
					{"AC", "wing-length"}
				}
			}
		}
	};

	ParameterCombinator paramCombinator(parameterTypeMap, printableParams);
	paramCombinator.combine(paramCombs, dontCares);

	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"wind-protector", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"wind-protector", 1}},
	};

	const parameterInstanceSet_t* paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet->count(paramInstanceMap));
	}

	return failed;

}

bool testCombinationWithMultipleDontCares()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["vehicle"]          = { "car", "motorbike" };
	paramCombs["horsepower"]       = { 100, 130 };
	paramCombs["AC"]               = { 1 };
	paramCombs["wind-protector"]   = { 0 };
	paramCombs["wing-length"]      = { 30.3 };
	paramCombs["motor"]            = { "gasoline", "diesel", "electric" };
	paramCombs["fuel-consumption"] = { 2.3, 4.1 };
	paramCombs["nobody-cares"]     = { "1", "2", "3", "4" };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "vehicle", "motor", "nobody-cares"};
	parameterTypeMap["double"] = { "wing-length", "fuel-consumption" };
	parameterTypeMap["int"]    = { "horsepower", "AC", "wind-protector" };

	// List of parameters to be ignored in the combination
	// TODO: support for dontCares with other key types
	dontCares_t dontCares =
	{
		{"vehicle",
			{
				{"car",
					{"wind-protector", "wing-length"}
				},
				{"motorbike",
					{"AC", "wing-length"}
				}
			}
		},
		{"motor",
			{
				{"electric",
					{"fuel-consumption"}
				},
			}
		},
		{"nobody-cares",{}}
	};

	ParameterCombinator paramCombinator(parameterTypeMap, printableParams);
	paramCombinator.combine(paramCombs, dontCares);

	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "gasoline"}, {"fuel-consumption", 2.3}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "gasoline"}, {"fuel-consumption", 2.3}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "gasoline"}, {"fuel-consumption", 2.3}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "gasoline"}, {"fuel-consumption", 2.3}, {"wind-protector", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "gasoline"}, {"fuel-consumption", 4.1}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "gasoline"}, {"fuel-consumption", 4.1}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "gasoline"}, {"fuel-consumption", 4.1}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "gasoline"}, {"fuel-consumption", 4.1}, {"wind-protector", 0}},

		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "diesel"},   {"fuel-consumption", 2.3}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "diesel"},   {"fuel-consumption", 2.3}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "diesel"},   {"fuel-consumption", 2.3}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "diesel"},   {"fuel-consumption", 2.3}, {"wind-protector", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "diesel"},   {"fuel-consumption", 4.1}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "diesel"},   {"fuel-consumption", 4.1}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "diesel"},   {"fuel-consumption", 4.1}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "diesel"},   {"fuel-consumption", 4.1}, {"wind-protector", 0}},

		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "electric"}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "electric"}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "electric"}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "electric"}, {"wind-protector", 0}},
	};

	const parameterInstanceSet_t* paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet->count(paramInstanceMap));
	}

	return failed;

}

bool testSimpleRecombination()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100, 120, 130 };
	paramCombs["motor"]      = { "gasoline", "electric" };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "vehicle", "motor"};
	parameterTypeMap["int"]    = { "horsepower", "airbag" };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares{};

	ParameterCombinator paramCombinator(parameterTypeMap, printableParams);
	paramCombinator.combine(paramCombs, dontCares);

	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "electric"}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"motor", "gasoline"}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"motor", "electric"}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"motor", "gasoline"}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "electric"}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"motor", "gasoline"}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "electric"}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"motor", "gasoline"}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}, {"motor", "electric"}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}, {"motor", "gasoline"}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "electric"}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"motor", "gasoline"}},
	};

	const parameterInstanceSet_t* paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet->count(paramInstanceMap));
	}

	// Test that we can change dontCares
	dontCares =
	{
		{"motor",{}}
	};

	paramCombinator.combine(paramCombs, dontCares);

	expectedCombinations = std::vector<parameterInstanceMap_t>
	{
		{{"vehicle", "car"},       {"horsepower", 100}},
		{{"vehicle", "car"},       {"horsepower", 120}},
		{{"vehicle", "car"},       {"horsepower", 130}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}},
	};

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet->count(paramInstanceMap));
	}

	// Test that we can change paramCombs and that dontCares has an element that doesn't exist
	paramCombs.clear();

	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100 };

	paramCombinator.combine(paramCombs, dontCares);

	return failed;

}

int main()
{
	bool failed = false;

	failed = testSimpleCombination();
	if (failed) { std::cout << "testSimpleCombination failed" << std::endl; return failed; }

	failed = testCombinationWithDontCare();
	if (failed) { std::cout << "testCombinationWithDontCare failed" << std::endl; return failed; }

	failed = testCombinationWithMultipleDontCares();
	if (failed) { std::cout << "testCombinationWithMultipleDontCares failed" << std::endl; return failed; }

	failed = testSimpleRecombination();
	if (failed) { std::cout << "testSimpleRecombination failed" << std::endl; return failed; }

	return failed;
}
