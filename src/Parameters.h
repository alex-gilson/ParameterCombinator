#pragma once
#include <unordered_map>
#include <string>
#include <map>
#include <set>
#include <variant>
#include <vector>
#include <stdexcept>
#include <typeinfo>
#include <iostream>
#include <functional>
#include <type_traits>
#include <optional>
#include <sstream>

namespace parameterCombinator
{

namespace CHECK
{
	struct No {};
	template<typename T, typename Arg> No operator== (const T&, const Arg&);

	template<typename T, typename Arg = T>
	struct EqualExists
	{
		enum { value = !std::is_same<decltype(*(T*)(0) == *(Arg*)(0)), No>::value };
	};
}


class ParameterBase {

public:

	virtual ~ParameterBase() {};
	virtual std::string toString() const = 0;

protected:

	friend bool operator==(const ParameterBase& lhs, const ParameterBase& rhs);
	friend bool  operator<(const ParameterBase& lhs, const ParameterBase& rhs);
	friend bool  operator>(const ParameterBase& lhs, const ParameterBase& rhs);

	virtual bool        isEqual(const ParameterBase& obj) const = 0;
	virtual bool    isLowerThan(const ParameterBase& obj) const = 0;

};

bool operator==(const ParameterBase& lhs, const ParameterBase& rhs);
bool  operator<(const ParameterBase& lhs, const ParameterBase& rhs);
bool  operator>(const ParameterBase& lhs, const ParameterBase& rhs);

template<typename T>
const int& getAddressOfVal(T& val)
{
	return reinterpret_cast<const int&>(val);
}

template <typename T, typename = void>
struct is_string
{
	static const bool value = false;
};

template <class T, class Traits, class Alloc>
struct is_string<std::basic_string<T, Traits, Alloc>, void>
{
	static const bool value = true;
};

template <class T, template <typename, typename, typename> class STRING>
struct is_string<T, STRING<T, std::char_traits<T>, std::allocator<T>>>
{
	static const bool value = true;
};

template<typename T>
class ParameterDerived : public ParameterBase {
public:
	ParameterDerived(T v) : val_(v) {}
	virtual ~ParameterDerived() {};
	T val_;
	std::string toString() const override
	{
		if constexpr (is_string<T>::value || std::is_same<const char*, std::remove_cv_t<T>>::value)
		{
			return val_;
		}
		else if constexpr (std::is_integral_v<T>)
		{
			return std::to_string(val_);
		}
		else if constexpr (std::is_floating_point<T>::value)
		{
			std::stringstream ss;
			ss << std::scientific << val_;
			return ss.str();
		}
		else
		{
			return std::to_string(getAddressOfVal(val_));
		}
	}
protected:
	virtual bool isEqual(const ParameterBase& obj) const override
	{
		auto v = static_cast<const ParameterDerived&>(obj);
		if constexpr (std::is_pointer<T>::value)
		{
			return *val_ == *v.val_;
		}
		return val_ == v.val_;
	}
	virtual bool isLowerThan(const ParameterBase& obj) const override
	{
		auto v = static_cast<const ParameterDerived&>(obj);
		if constexpr (std::is_pointer<T>::value)
		{
			return *val_ < *v.val_;
		}
		return val_ < v.val_;
	}
};

class Parameter
{
private:
	std::shared_ptr<const ParameterBase> param_;
public:
	Parameter();
	~Parameter();
	template<typename T>
	Parameter(T param)
	{
		param_ = std::make_shared<const ParameterDerived<T>>(param);
	}

	friend bool operator==(const Parameter& lhs, const Parameter& rhs);
	friend bool operator!=(const Parameter& lhs, const Parameter& rhs);
	friend bool  operator<(const Parameter& lhs, const Parameter& rhs);
	friend bool operator<=(const Parameter& lhs, const Parameter& rhs);
	friend bool  operator>(const Parameter& lhs, const Parameter& rhs);
	friend bool operator>=(const Parameter& lhs, const Parameter& rhs);

	const Parameter& operator=(const Parameter& other)
	{
		if (this == &other)
		{
			return *this;
		}
		param_ = other.param_;
		return *this;
	}

	const ParameterBase& operator*() const
	{
		return *param_;
	}
	const ParameterBase* operator->() const
	{
		return param_.get();
	}
};

bool operator==(const Parameter& lhs, const Parameter& rhs);
bool operator!=(const Parameter& lhs, const Parameter& rhs);
bool operator <(const Parameter& lhs, const Parameter& rhs);
bool operator<=(const Parameter& lhs, const Parameter& rhs);
bool operator >(const Parameter& lhs, const Parameter& rhs);
bool operator>=(const Parameter& lhs, const Parameter& rhs);

struct ParameterHasher
{
	std::size_t operator()(const Parameter& key) const
	{
		return std::hash<int>()(getAddressOfVal(*key));
	}
};

class ParametersVec
{
public:
	ParametersVec();
	~ParametersVec();
	template<typename T>
	ParametersVec(std::initializer_list<T> vals)
	{
		static_assert(CHECK::EqualExists<T>::value, "Error, ParameterDerived must be comparable. Define a '==' operator for your class.");
		for (auto& val : vals)
		{
			parametersVec_.emplace_back(val);
		}
	}
	template<typename T>
	void push_back(T val)
	{
		parametersVec_.push_back(val);
	}
	auto begin()
	{
		return parametersVec_.begin();
	}
	auto end()
	{
		return parametersVec_.end();
	}
	Parameter& operator[](int idx)
	{
		return parametersVec_[idx];
	}
private:
	std::vector<Parameter> parametersVec_;
};


using parameterInstanceMap_t  = std::map<std::string, Parameter >;
using parameterCombinations_t = std::unordered_map<std::string, ParametersVec>;
using stringSet_t = std::set<std::string>;
using stringSetMap_t = std::unordered_map<std::string, stringSet_t >;
using dontCares_t = std::unordered_map<std::string, std::unordered_map<Parameter, std::set<std::string> , ParameterHasher> >;

template<typename T>
auto getVal(const Parameter& param)
{
	const ParameterBase* paramBase = &(*param);
	const ParameterDerived<T>* paramDerived = dynamic_cast<const ParameterDerived<T>*>(paramBase);
	if (!paramDerived)
	{
		throw std::invalid_argument("Given template type does not match value type");
	}
	return paramDerived->val_;
}

template<typename T>
auto getVal(const parameterInstanceMap_t& paramInstance, const std::string& key)
{
	if (!paramInstance.count(key))
	{
		throw std::invalid_argument("Parameter does not exist for this parameterInstance.");
	}
	return getVal<T>(paramInstance.at(key));
}

struct ParameterInstanceSetCompare
{
	dontCares_t dontCares_;
	ParameterInstanceSetCompare()
		: dontCares_()
	{};
	ParameterInstanceSetCompare(const dontCares_t& dontCares)
		: dontCares_(dontCares)
	{};
	ParameterInstanceSetCompare& operator=(const ParameterInstanceSetCompare& other)
	{
		dontCares_ = other.dontCares_;
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
				|| (!dontCare.second.empty() && b.count(dontCareKey) && dontCare.second.count(b.at(dontCareKey))
				&&  dontCare.second.at(b.at(dontCareKey)).count(param.first))) // Does b care about param.first?
				{
					skip = true;
					break;
				}
			}
			if (skip || (b.count(param.first) && (param.second == b.at(param.first)))
				|| (!b.count(param.first) && a.count(param.first))) // Compare paramInstances of different sizes (happens at addCombinations)
			{
				continue;
			}
			else
			{
				return b.count(param.first) ? (param.second < b.at(param.first)) : false;
			}
		}
		return false;
	}
};

using parameterInstanceSet_t = std::set<parameterInstanceMap_t, ParameterInstanceSetCompare>;

} // Namespace parameterCombinator
