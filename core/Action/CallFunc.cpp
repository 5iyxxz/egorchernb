#include "..\e2daction.h"

e2d::CallFunc::CallFunc(const Function& func) :
	_func(func)
{
}

e2d::CallFunc * e2d::CallFunc::clone() const
{
	return new CallFunc(_func);
}

void e2d::CallFunc::_init()
{
}

void e2d::CallFunc::_update()
{
	_func();
	this->stop();
}
