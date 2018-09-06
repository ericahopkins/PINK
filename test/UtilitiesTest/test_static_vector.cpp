/*
 * test_static_vector.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: doserbd
 */

#include <iostream>
#include <vector>

struct A
{
    std::vector<int> foo() const
    {
        static std::vector<int> v;
        v.push_back(42);
        return v;
    }
};

int main(void)
{
	{
		A a;
		{
			auto&& v = a.foo();
			std::cout << v.size() << std::endl;
		}
		{
			auto&& v = a.foo();
			std::cout << v.size() << std::endl;
		}
	}
	{
		A a;
		{
			auto&& v = a.foo();
			std::cout << v.size() << std::endl;
		}
		{
			auto&& v = a.foo();
			std::cout << v.size() << std::endl;
		}
	}
}
