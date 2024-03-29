![ParameterCombinator logo](https://github.com/alex-gilson/ParameterCombinator/blob/main/parametercombinator_logo.png)

# Description

ParameterCombinator is a library for parameter validation. It allows the tester to create all possible combinations for a set of parameter values. The user can also determine the relationship of parameters in case they have no casual effect between each other to avoid reduntant evaluations of an algorithm.

# Installation

First, set up your project for CMake. Then, in your project directory run:

```bash
git submodule add https://github.com/alex-gilson/ParameterCombinator.git
```

Insert the following line to you CMakeLists.txt:

```cmake
add_subdirectory(ParameterCombinator)
```

and

```cmake
target_link_libraries(<your-project-name> PUBLIC ParameterCombinator)
```

# Usage

To make use of the ParameterCombinator library add the header file to your code:

```C++
#include "ParameterCombinator.h"
```

Additionaly, you can also make use the namespace directly.

```C++
using namespace parameterCombinator;
```

As a first example, let's say we wish to make all possible combinations of vehicles with the horsepower and availability of airbag properties.

```C++
parameterCombinations_t paramCombs;
paramCombs["vehicle"]    = { "car", "motorbike" };
paramCombs["horsepower"] = { 100, 120, 130 };
paramCombs["airbag"]     = { 0, 1 };
```

We construct the class ParameterCombinator and tell it to combine our properties.

```C++
dontCares_t dontCares;
ParameterCombinator paramCombinator;
paramCombinator.combine(paramCombs, dontCares);
```
	
The resulting combination will be this:

```C++
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
{{"vehicle", "motorbike"}, {"horsepower", 130}, {"airbag", 1}}
```
		
Now, let's say we want to introduce more complex relationships between variables. Let's take the following case:

```C++
paramCombs["vehicle"]          = { "car", "motorbike" };
paramCombs["horsepower"]       = { 100, 130 };
paramCombs["AC"]               = { 1 };
paramCombs["wind-protector"]   = { 0 };
paramCombs["wing-length"]      = { 30.3 };
paramCombs["motor"]            = { "gasoline", "diesel", "electric" };
paramCombs["fuel-consumption"] = { 2.3, 4.1 };
paramCombs["nobody-cares"]     = { "1", "2", "3", "4" };
```
	
Cars don't care about whether there is a wind protector because that's a motorbike element. Motorbikes don't have AC. And also, neither of them have wings, therefore, wing-length is irrelevant to them. As a example of dummy parameters we use "nobody-cares". To establish these relationships we use the following structure:

```C++
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
```
	
After running the combination engine, we should end up with the following result:

```C++
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
```

"Dontcares" can contain relationships between parameters of any type.

```C++
dontCares["horsepower"] = 
{
	{130, {"wind-protector"}}
};
```
		
To make use of every instance of a parameter combination is as easy as using an iterator.

```C++
auto paramInstanceSet = paramCombinator.getParameterInstanceSet();

for (auto& paramInstance : *paramInstanceSet)
{
	auto vehicle    = getVal<const char*>(paramInstance, "vehicle");
	auto horsepower = getVal<int>(paramInstance, "horsepower");
	// Motorbikes don't have AC
	if (strcmp(vehicle, "motorbike"))
	{
		auto AC = getVal<bool>(paramInstance, "AC");
	}
	runTest(vehicle, horsepower);
}
```

