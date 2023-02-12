#include "ParameterCombinator.h"
#include <iostream>
#include <cassert>

using namespace parameterCombinator;

struct point_t
{
	int x;
	int y;
	point_t(int a, int b) : x(a), y(b) {};
	bool operator<(const point_t& other) { return std::sqrt(x * x + y * y) < std::sqrt(other.x * other.x + other.y * other.y); }
	friend bool operator==(const point_t& lhs, const point_t& rhs);
};

bool operator==(const point_t& lhs, const point_t& rhs)
{
	int result = std::sqrt(lhs.x * lhs.x + lhs.y * lhs.y) == std::sqrt(rhs.x * rhs.x + rhs.y * rhs.y);
	return result;
}

void printDifferences(const parameterInstanceSet_t& expectedCombinations, const parameterInstanceSet_t* paramSet)
{
	std::cout << "Size of output: " << paramSet->size() << std::endl;
	std::cout << "Size of expected result: " << expectedCombinations.size() << std::endl;
	std::cout << "Elements expected in output but not found:" << std::endl;
	for (auto& combination : expectedCombinations)
	{
		if (!paramSet->count(combination))
		{
			for (auto& param : combination)
			{
				std::cout << param.first << ": " << param.second->toString() << ", ";
			}
			std::cout << std::endl;
		}
	}
	std::cout << "Elements in output not present in the expected result:" << std::endl;
	for (auto& combination : *paramSet)
	{
		if (!paramSet->count(combination))
		{
			for (auto& param : combination)
			{
				std::cout << param.first << ": " << param.second->toString() << ", ";
			}
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
}

bool checkEquality(const parameterInstanceSet_t& expectedCombinations, ParameterCombinator& paramCombinator)
{
	bool failed = false;
	const parameterInstanceSet_t* paramSet = paramCombinator.getParameterInstanceSet();

	failed |= !(expectedCombinations.size() == paramSet->size());

	for (auto& combination : expectedCombinations)
	{
		failed |= !(paramSet->count(combination));
	}
	for (auto& combination : *paramSet)
	{
		failed |= !(expectedCombinations.count(combination));
		int a = 3; (void)a;
	}
	if (failed)
	{
		printDifferences(expectedCombinations, paramSet);
	}
	return failed;
}

bool testSimpleCombination()
{
	// List of parameters to test
	parameterCombinations_t paramCombs;
	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100, 120, 130 };
	paramCombs["airbag"]     = { 0, 1 };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares{};

	ParameterCombinator paramCombinator;
	paramCombinator.combine(paramCombs, dontCares);
	
	const parameterInstanceSet_t expectedCombinations
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
	bool failed = false;
	// List of parameters to test
	parameterCombinations_t paramCombs;
	paramCombs["vehicle"]        = { "car", "motorbike" };
	paramCombs["horsepower"]     = { 100, 130 };
	paramCombs["AC"]             = { 0, 1 };
	paramCombs["wind-protector"] = { 0, 1 };
	paramCombs["wing-length"]    = { 30.3 };

	// List of parameters to be ignored in the combination
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

	ParameterCombinator paramCombinator1;
	paramCombinator1.combine(paramCombs, dontCares);

	const parameterInstanceSet_t expectedCombinations
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

	failed |= checkEquality(expectedCombinations, paramCombinator1);

	// Test that we can use any type of Parameter as a dontCare key.
	dontCares["horsepower"] = 
			{
				{130, {"wind-protector"}}
			};

	ParameterCombinator paramCombinator2;
	paramCombinator2.combine(paramCombs, dontCares);

	const parameterInstanceSet_t expectedCombinations2 =
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"wind-protector", 0}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}, {"wind-protector", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}},
	};

	failed |= checkEquality(expectedCombinations2, paramCombinator2);

	return failed;

}

bool testCombinationWithMultipleDontCares()
{
	// List of parameters to test
	parameterCombinations_t paramCombs;
	paramCombs["vehicle"]          = { "car", "motorbike" };
	paramCombs["horsepower"]       = { 100, 130 };
	paramCombs["AC"]               = { 1 };
	paramCombs["wind-protector"]   = { 0 };
	paramCombs["wing-length"]      = { 30.3 };
	paramCombs["motor"]            = { "gasoline", "diesel", "electric" };
	paramCombs["fuel-consumption"] = { 2.3, 4.1 };
	paramCombs["nobody-cares"]     = { "1", "2", "3", "4" };

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

	ParameterCombinator paramCombinator;
	paramCombinator.combine(paramCombs, dontCares);

	auto paramInstanceSet = paramCombinator.getParameterInstanceSet();
	(void)paramInstanceSet;

	const parameterInstanceSet_t expectedCombinations
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
	// List of parameters to test
	parameterCombinations_t paramCombs;
	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100, 120, 130 };
	paramCombs["motor"]      = { "gasoline", "electric" };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares{};

	ParameterCombinator paramCombinator;
	paramCombinator.combine(paramCombs, dontCares);

	const parameterInstanceSet_t expectedCombinations
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

	const parameterInstanceSet_t expectedCombinations2
	{
		{{"vehicle", "car"},       {"horsepower", 100}},
		{{"vehicle", "car"},       {"horsepower", 120}},
		{{"vehicle", "car"},       {"horsepower", 130}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}},
	};

	failed |= checkEquality(expectedCombinations2, paramCombinator);

	// Test that we can change paramCombs and that dontCares has an element that doesn't exist
	paramCombs.clear();

	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100 };

	paramCombinator.combine(paramCombs, dontCares);

	const parameterInstanceSet_t expectedCombinations3
	{
		{{"vehicle", "car"},       {"horsepower", 100}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}},
	};

	failed |= checkEquality(expectedCombinations3, paramCombinator);

	return failed;

}

void testParameter()
{
	// Test parameters of the same value
	{
		Parameter a = 4;
		Parameter b = 4;
		assert(a == b);
		assert(!(a != b));
		assert(a >= b);
		assert(a <= b);
	}
	// Test parameters of the same type but different value
	{
		Parameter a = 3;
		Parameter b = 4;
		assert(a != b);
		assert(a < b);
		assert(b > a);
		assert(a <= b);
		assert(b >= a);
	}
	// Test parameters of different type
	{
		Parameter a = 'a';
		Parameter b = 2;
		assert(a != b);
		assert(a < b || a > b);
		if (a < b)
		{
			assert(a <= b);
			assert(!(a >= b));
		}
		else
		{
			assert(a >= b);
			assert(!(a <= b));
		}
	}
	// Test == and != operators for Parameter class
	{
		Parameter a = 3;
		Parameter b = a;
		assert(a == b);
		a = 2;
		assert(a != b);
	}
	// Test == and != operators for Parameters of type const char*
	{
		Parameter a = "asdf";
		Parameter b = a;
		Parameter c = "asdf2";
		Parameter d = "asdf";
		assert(a == b);
		assert(a != c);
		assert(a == d);
	}
	// Test that Parameters of pointers are treated correctly
	{
		int value = 4;
		Parameter a = &value;
		Parameter b = a;
		Parameter c = 4;
		assert(a == b);
		assert(a != c); // Different because one is an int refernce and the other one an int
	}
	// Test that Parameters works with custom classes
	{
		point_t p  = { 1, 2 };
		point_t p2 = { 1, 2 };
		point_t p3 = { 1, 3 };
		Parameter a = p;
		Parameter b = p2;
		Parameter c = p3;
		assert(a == b);
		assert(a != p3);
	}
	// Test parameterInstanceMap_t
	{
		Parameter a = 3;
		parameterInstanceMap_t map;
		map["a"] = a;
		assert(map.count("a"));
		assert(!map.count("b"));
	}
	// Test parameterInstanceSet_t
	{
		Parameter a = 3;
		Parameter b = 4;
		Parameter c = 5;
		parameterInstanceMap_t map1, map2;
		parameterInstanceSet_t paramSet;
		map1["a"] = a;
		map1["b"] = b;
		map2["a"] = a;
		map2["b"] = c;
		paramSet.insert(map1);
		assert(paramSet.count(map1));
		assert(!paramSet.count(map2));
		// Test that the search goes by value and not by address
		map2["b"] = b;
		assert(paramSet.count(map2));
	}
}

bool testIteration()
{
	parameterCombinations_t paramCombs;
	paramCombs["vehicle"]    = { "car", "motorbike" };
	paramCombs["horsepower"] = { 100, 120, 130 };
	paramCombs["AC"]         = { false, true };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares =
	{
		{"vehicle",
			{
				{"motorbike",
					{"AC"}
				}
			}
		}
	};

	ParameterCombinator paramCombinator;
	paramCombinator.combine(paramCombs, dontCares);

	auto paramInstanceSet = paramCombinator.getParameterInstanceSet();

	for (auto& paramInstance : *paramInstanceSet)
	{
		auto vehicle    = std::string{ getVal<const char*>(paramInstance, "vehicle") };
		auto horsepower = getVal<int>(paramInstance, "horsepower");
		(void)horsepower;
		// Motorbikes don't have AC
		if (vehicle != "motorbike")
		{
			auto AC = getVal<bool>(paramInstance, "AC");
			(void)AC;
		}
	}
	return false;
}

bool testAddition()
{
	bool failed = false;
	// List of parameters to test
	parameterCombinations_t paramCombs;
	paramCombs["vehicle"]        = { "car", "motorbike" };
	paramCombs["horsepower"]     = { 100, 130 };
	paramCombs["AC"]             = { 0, 1 };

	// List of parameters to be ignored in the combination
	dontCares_t dontCares =
	{
		{"vehicle",
			{
				{"motorbike",
					{"AC"}
				}
			}
		}
	};

	ParameterCombinator paramCombinator1;
	paramCombinator1.combine(paramCombs, dontCares);

	const parameterInstanceSet_t expectedCombinations
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}},
	};

	failed |= checkEquality(expectedCombinations, paramCombinator1);

	parameterCombinations_t paramCombs2;
	paramCombs2["vehicle"]        = { "car", "motorbike" };
	paramCombs2["horsepower"]     = { 120, 140 };
	paramCombs2["AC"]             = { 0, 1 };

	ParameterCombinator paramCombinator2;
	paramCombinator2.combine(paramCombs2, dontCares);

	const parameterInstanceSet_t expectedCombinations2
	{
		{{"vehicle", "car"},       {"horsepower", 120}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 140}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 140}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}},
		{{"vehicle", "motorbike"}, {"horsepower", 140}},
	};

	failed |= checkEquality(expectedCombinations2, paramCombinator2);

	ParameterCombinator paramCombinator3;
	paramCombinator3.addCombinations(paramCombinator1, paramCombinator2, dontCares);

	const parameterInstanceSet_t expectedCombinations3
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 100}},
		{{"vehicle", "motorbike"}, {"horsepower", 130}},

		{{"vehicle", "car"},       {"horsepower", 120}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 140}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 140}, {"AC", 1}},
		{{"vehicle", "motorbike"}, {"horsepower", 120}},
		{{"vehicle", "motorbike"}, {"horsepower", 140}},
	};

	failed |= checkEquality(expectedCombinations3, paramCombinator3);

	paramCombinator3.addCombinations(paramCombinator1, paramCombinator2, dontCares_t{});
	failed |= checkEquality(expectedCombinations3, paramCombinator3);

	dontCares_t dontCares2 =
	{
		{"vehicle",
			{
				{"motorbike",
					{"horsepower", "AC"}
				}
			}
		}
	};

	const parameterInstanceSet_t expectedCombinations4
	{
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 100}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 130}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 140}, {"AC", 0}},
		{{"vehicle", "car"},       {"horsepower", 120}, {"AC", 1}},
		{{"vehicle", "car"},       {"horsepower", 140}, {"AC", 1}},
		{{"vehicle", "motorbike"}},
	};

	//paramCombinator3.addCombinations(paramCombinator1, paramCombinator2, dontCares2);
	//failed |= checkEquality(expectedCombinations4, paramCombinator3);

	return failed;

}

bool testGenerateCombinationName()
{
	parameterInstanceMap_t paramInstance;
	ParameterCombinator paramComb;
	paramInstance["vehicle"] = "car";
	paramInstance["airbag"] = true;
	paramInstance["horsepower"] = 120.2342;
	paramInstance["motor"] = std::string{ "diesel" };

	std::string combinationName = paramComb.generateCombinationName(paramInstance);

	bool failed = !(combinationName == "airbag_1_horsepower_1_202342e+02_motor_diesel_vehicle_car");

	return failed;
}

int main()
{
	testParameter();
	assert(!testSimpleCombination());
	assert(!testCombinationWithDontCare());
	assert(!testCombinationWithMultipleDontCares());
	assert(!testSimpleRecombination());
	assert(!testAddition());
	assert(!testGenerateCombinationName());
	testIteration();

	return 0;
}

