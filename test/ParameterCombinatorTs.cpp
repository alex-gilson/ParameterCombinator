#include "ParameterCombinator.h"
#include <iostream>

bool checkEquality(std::vector<parameterInstanceMap_t>& expectedCombinations, ParameterCombinator& paramCombinator)
{
	bool failed = false;
	const parameterInstanceSet_t* paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& paramInstanceMap : expectedCombinations)
	{
		failed |= !(paramSet->count(paramInstanceMap));
	}
	return failed;
}

bool testSimpleCombination()
{
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

	return checkEquality(expectedCombinations, paramCombinator);

}

bool testCombinationWithDontCare()
{
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

	return checkEquality(expectedCombinations, paramCombinator);

}

bool testCombinationWithMultipleDontCares()
{
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

	return checkEquality(expectedCombinations, paramCombinator);

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

	failed |= checkEquality(expectedCombinations, paramCombinator);

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

	failed |= checkEquality(expectedCombinations, paramCombinator);

	// Test that we can change paramCombs and that dontCares has an element that doesn't exist
	paramCombs.clear();

	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100 };

	paramCombinator.combine(paramCombs, dontCares);

	expectedCombinations = std::vector<parameterInstanceMap_t>
	{
		{{"vehicle", "car"},       {"horsepower", 100}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}},
	};

	failed |= checkEquality(expectedCombinations, paramCombinator);

	return failed;

}

bool testCombinationAddition()
{
	bool failed = false;
	parameterCombinations_t paramCombs;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["vehicle"]    = { "car" };
	paramCombs["horsepower"] = { 100, 120 };
	paramCombs["airbag"]     = { 0, 1 };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "vehicle" };
	parameterTypeMap["int"]    = { "horsepower", "airbag" };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares{};

	ParameterCombinator paramCombinator1(parameterTypeMap, printableParams);
	paramCombinator1.combine(paramCombs, dontCares);

	std::vector<parameterInstanceMap_t> expectedCombinations
	{
		{{"vehicle", "car"}, {"horsepower", 100}, {"airbag", 0}},
		{{"vehicle", "car"}, {"horsepower", 100}, {"airbag", 1}},
		{{"vehicle", "car"}, {"horsepower", 120}, {"airbag", 0}},
		{{"vehicle", "car"}, {"horsepower", 120}, {"airbag", 1}},
	};

	failed |= checkEquality(expectedCombinations, paramCombinator1);

	paramCombs.clear();
	paramCombs["vehicle"]    = { "motorbike" };
	paramCombs["horsepower"] = { 100, 130 };
	paramCombs["airbag"]     = { 0, 1 };

	ParameterCombinator paramCombinator2(parameterTypeMap, printableParams);
	paramCombinator2.combine(paramCombs, dontCares);

	expectedCombinations = std::vector<parameterInstanceMap_t>
	{
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"airbag", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 1}},
	};

	failed |= checkEquality(expectedCombinations, paramCombinator2);

	// Test addition maintaining the same dontCares
	ParameterCombinator paramCombinator3 = ParameterCombinator::addCombinators(paramCombinator1, paramCombinator2, dontCares);

	expectedCombinations = std::vector<parameterInstanceMap_t>
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"airbag", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"airbag", 1}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"airbag", 0}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"airbag", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"airbag", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 1}},
	};

	failed |= checkEquality(expectedCombinations, paramCombinator3);

	//dontCares =
	//{
	//	{"vehicle",
	//		{
	//			{"motorbike",
	//				{"airbag"}
	//			},
	//		}
	//	},
	//};

	//// Test addition with a different dontCares
	//ParameterCombinator paramCombinator4 = ParameterCombinator::addCombinators(paramCombinator1, paramCombinator2, dontCares);

	//expectedCombinations = std::vector<parameterInstanceMap_t>
	//{
	//	{{"vehicle", "car"},      {"horsepower", 100}, {"airbag", 0}},
	//	{{"vehicle", "car"},      {"horsepower", 100}, {"airbag", 1}},
	//	{{"vehicle", "car"},      {"horsepower", 130}, {"airbag", 0}},
	//	{{"vehicle", "car"},      {"horsepower", 130}, {"airbag", 1}},
	//	{{"vehicle", "motorbike"},{"horsepower", 100}},
	//	{{"vehicle", "motorbike"},{"horsepower", 120}},
	//};

	//failed |= checkEquality(expectedCombinations, paramCombinator4);

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

	failed = testCombinationAddition();
	if (failed) { std::cout << "testCombinationAddition failed" << std::endl; return failed; }

	return failed;
}
