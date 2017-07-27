#ifndef OBJCPP_SIGNAL_H_
#define OBJCPP_SIGNAL_H_

#include <vector>
#include <functional>

template <typename Arg1>
struct signal
{
public:
	void connect(const std::function<void(Arg1)> &slot)
	{
		_slots.push_back(slot);
	}

	void operator+=(const std::function<void(Arg1)> &slot)
	{
		connect(slot);
	}

	void invoke(const Arg1 &arg1)
	{
		for (auto i = begin(_slots); i != end(_slots); ++i)
			(*i)(arg1);
	}

	void operator()(const Arg1 &arg1)
	{
		invoke(arg1);
	}

private:
	std::vector<std::function<void(Arg1)>> _slots;
};

template <>
struct signal<void>
{
public:
	void connect(const std::function<void(void)> &slot)
	{
		_slots.push_back(slot);
	}

	void operator+=(const std::function<void(void)> &slot)
	{
		connect(slot);
	}

	void invoke()
	{
		for (auto i = begin(_slots); i != end(_slots); ++i)
			(*i)();
	}

	void operator()()
	{
		invoke();
	}

private:
	std::vector<std::function<void(void)>> _slots;
};



// FIXME: variadic templates
template <typename Arg1, typename Arg2>
struct signal2
{
public:
	void connect(const std::function<void(Arg1, Arg2)> &slot)
	{
		_slots.push_back(slot);
	}

	void operator+=(const std::function<void(Arg1, Arg2)> &slot)
	{
		connect(slot);
	}

	void invoke(const Arg1 &arg1, const Arg2 &arg2)
	{
		for (auto i = begin(_slots); i != end(_slots); ++i)
			(*i)(arg1, arg2);
	}

	void operator()(const Arg1 &arg1, const Arg2 &arg2)
	{
		invoke(arg1, arg2);
	}

	void clear(const std::function<void(Arg1, Arg2)> &slot)
	{
		_slots.clear();
		_slots.push_back(slot);
	}

private:
	std::vector<std::function<void(Arg1, Arg2)>> _slots;
};

#endif // OBJCPP_SIGNAL_H_