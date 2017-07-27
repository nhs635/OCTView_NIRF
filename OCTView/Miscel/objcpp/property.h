#ifndef OBJCPP_PROPERTY_H_
#define OBJCPP_PROPERTY_H_

#include "signal.h"

#include <string>
#include <functional>
#include <limits>
#include <iostream>

template <typename T>
struct property
{
	typedef T value_type;

	signal<value_type> valueChanged;
	bool readonly;

	property(const std::string &name, value_type default_value = value_type()) :
		_name(name), readonly(false)
	{
		set(default_value);
	}

	const value_type& get() const
	{
		return _value; 
	}

	void set(const value_type& new_value)
	{
		if (_value != new_value)
		{
			_value = new_value;

			// Fire valueChanged event
			valueChanged(new_value);
		}
	}

	const std::string& name() const
	{
		return _name;
	}

	// Syntatic sugars
	operator const value_type& () const
	{
		return _value;
	}

	const value_type& operator=(const value_type& new_value)
	{
		set(new_value);
		return _value;
	}

	const value_type& operator=(const property<T> &property)
	{
		set(property.get());
		return _value;
	}

private:
	std::string _name;
	T _value;
};

// template specialization for float
template <>
struct property<float>
{
	typedef float value_type;

	signal<value_type> valueChanged;
	bool readonly;

	property(const std::string &name) : 
		_name(name), readonly(false), _step(1.0f), _min(std::numeric_limits<value_type>::min()), _max(std::numeric_limits<value_type>::max())
	{
	}

	property(const std::string &name, 
		value_type default_value, 
		value_type step = 1.0f, 
		value_type min = std::numeric_limits<value_type>::min(), 
		value_type max = std::numeric_limits<value_type>::max()) : _name(name), readonly(false), _step(step), _min(min), _max(max)
	{
		set(default_value);
	}

	const value_type& get() const
	{
		return _value; 
	}

	void set(const value_type& new_value)
	{
		if (_value == new_value)
			return; // do not fire update event
		else if (new_value > max())
			_value = max();
		else if (new_value < min())
			_value = min();
		else
			_value = new_value;

		// Fire valueChanged event
		valueChanged(new_value);
	}

	// getters
	const std::string& name() const	{ return _name; }
	value_type step() const { return _step; }
	value_type min() const { return _min; }
	value_type max() const { return _max; }

	// Syntatic sugars
	operator const value_type& () const
	{
		return _value;
	}

	const value_type& operator=(const value_type& new_value)
	{
		set(new_value);
		return _value;
	}

	const value_type& operator=(const property<value_type> &property)
	{
		set(property.get());
		return _value;
	}

private:
	std::string _name;
	
	value_type _value;
	const value_type _step, _min, _max;
};

class Object;
class Context;

template<>
struct property<Object>
{
	typedef std::string value_type;

	signal<value_type> valueChanged;
	bool readonly;

	property(Context &context, const std::string &name, value_type default_value = value_type()) :
		_context(context), _name(name), readonly(false)
	{
		set(default_value);
	}

	const value_type& get() const
	{
		return _value; 
	}

	void set(const value_type& new_value)
	{
		if (_value != new_value)
		{
			_value = new_value;

			// Fire valueChanged event
			valueChanged(new_value);
		}
	}

	Context &getContext() const
	{
		return _context;
	}

	const std::string& name() const
	{
		return _name;
	}

	//// NO Syntatic sugars
	//operator const T& () const
	//{
	//	return _value;
	//}

	//const T& operator=(const T& new_value)
	//{
	//	set(new_value);
	//	return _value;
	//}

	//const T& operator=(const property<T> &property)
	//{
	//	set(property.get());
	//	return _value;
	//}

	// Get object pointer
	template <class ObjectType>
	ObjectType &getObject() const
	{
		return *_context.getObject<ObjectType>(_value);
	}

private:
	Context &_context;
	std::string _name;
	value_type _value;
};

struct operation
{
	template <class Class>
	operation(const std::string name, Class *instance, void (Class::*function)(void)) :
		_name(name), 
		_function(std::bind(function, instance))
	{
	}

	template <class Func>
	operation(const std::string name, Func function) :
		_name(name), 
		_function(function)
	{
	}

	void run() const
	{
		try
		{
			_function();
		}
		catch (std::exception &e)
		{
			std::cout << "[" << _name << "] Exception: " << e.what() << std::endl;
		}
	}

	const std::string& name() const
	{
		return _name;
	}

	// Syntatic sugars
	void operator() () const
	{
		run();
	}

private:
	std::string _name;
	std::function<void()> _function;
};

struct property_visitor
{
    virtual void visit(property<bool> &property) const = 0;
    virtual void visit(property<int> &property) const = 0;
    virtual void visit(property<float> &property) const = 0;
    virtual void visit(property<std::string> &property) const = 0;
    virtual void visit(property<Object> &property) const = 0;
    virtual void visit(operation &operation) const = 0;

    // syntatic sugars
    template <typename T>
    void operator() (property<T> &property) const
    {
        visit(property);
    }

    void operator() (operation &operation) const
    {
        visit(operation);
    }
};

#endif // OBJCPP_PROPERTY_H_