#include "Parameters.h"
#include <iostream>

namespace parameterCombinator
{

// ParameterBase operators

bool operator==(const ParameterBase& lhs, const ParameterBase& rhs) {
	bool typeIdIsEqual = typeid(lhs) == typeid(rhs);
	bool isValueEqual  = typeIdIsEqual && lhs.isEqual(rhs);
	return isValueEqual;
}

bool operator<(const ParameterBase& lhs, const ParameterBase& rhs) {
	if (typeid(lhs) == typeid(rhs))
	{
		return lhs.isLowerThan(rhs);
	}
	else
	{
		return typeid(lhs).hash_code() < typeid(rhs).hash_code();
	}
}

bool operator>(const ParameterBase& lhs, const ParameterBase& rhs) {
	if (typeid(lhs) == typeid(rhs))
	{
		return !lhs.isLowerThan(rhs) && !lhs.isEqual(rhs);
	}
	else
	{
		return typeid(lhs).hash_code() > typeid(rhs).hash_code();
	}
}

// Parameter operators

bool operator==(const Parameter& lhs, const Parameter& rhs)
{
	return *lhs.param_ == *rhs.param_;
}
bool operator!=(const Parameter& lhs, const Parameter& rhs)
{
	return !(*lhs.param_ == *rhs.param_);
}
bool operator<(const Parameter& lhs, const Parameter& rhs)
{
	return *lhs.param_ < *rhs.param_;
}
bool operator<=(const Parameter& lhs, const Parameter& rhs)
{
	return (*lhs.param_ < *rhs.param_) || (*lhs.param_ == *rhs.param_);
}
bool operator>(const Parameter& lhs, const Parameter& rhs)
{
	return *lhs.param_ > *rhs.param_;
}
bool operator>=(const Parameter& lhs, const Parameter& rhs)
{
	return (*lhs.param_ > *rhs.param_) || (*lhs.param_ == *rhs.param_);
}

// Function definitions

ParametersVec::ParametersVec()
{
}

ParametersVec::~ParametersVec()
{
}

Parameter::Parameter()
{
}

Parameter::~Parameter()
{
}

} // Namespace parameterCombinator
