// WormholeRenderer.cpp : Defines the entry point for the console application.
//

#include "vendor\daemonalchemist\atp-vector\src\vector.hpp"
#include <iostream>

int main()
{
	ATP::Math::Vector v(1, 2, 3);

	std::cout << v.x << "\n" << v.y << "\n" << v.z;

    return 0;
}

