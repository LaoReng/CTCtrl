#pragma once

/**
* 函数模板类
*/
#include <iostream>
#include <functional>

class CFunctionBase
{
public:
	virtual ~CFunctionBase() {}
	virtual int operator()() { return -1; }
};

template<class _FUNCTION_, class... _ARGS_>
class CFunction
	: public CFunctionBase
{
public:
	CFunction(_FUNCTION_ func, _ARGS_... args)
	{
		m_func = std::bind(func, args...);
	}
	CFunction(const CFunction& f) {}
	virtual ~CFunction() {}
	virtual int operator()() { // 实现普通函数方式调用
		return m_func();
	}
	// 利用bind实现函数模板定义，实现任意函数的调用，但是这里要求返回值必须为int类型
	std::function<int()> m_func;
};

