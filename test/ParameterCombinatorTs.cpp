#include "ParameterCombinator.h"

int main()
{
	parameterCombinations_t paramCombs;
	dontCares_t             dontCares;
	parameterTypeMap_t      parameterTypeMap;
	printableParams_t		printableParams;

	// List of parameters to test
	paramCombs["A"] = { "one", "two" };
	paramCombs["B"] = { 1 };
	paramCombs["C"] = { 1.1, 2.2, 3.3 };
	paramCombs["D"] = { "asdf" };

	// List of parameters that don't affect the algorithm
	std::string dontCareKey = "D";
	dontCares["se"]         = { "magTol" };
	dontCares["pv"]         = { "magTol" };

	// Type of each of the parameters
	parameterTypeMap["string"] = { "inputFile", "algo", "signal", "data"};
	parameterTypeMap["double"] = { "magTol" };
	parameterTypeMap["int"]    = { "freq", "steps", "hopA", "algo", "magTol", "buflen" };

	printableParams = { "signal", "steps", "algo", "buflen", "freq", "numSamp"};

	ParameterCombinator trainingData(paramCombs, dontCares, dontCareKey, parameterTypeMap, printableParams);
	ParameterCombinator expectedData = generateExpectedDataSetFromTrainingDataSet(trainingData);

	return expectedData;
    return 0;
}