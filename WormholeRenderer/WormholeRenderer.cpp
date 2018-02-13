// WormholeRenderer.cpp : Defines the entry point for the console application.
//

#include "vendor\daemonalchemist\atp-vector\src\vector.hpp"
#include "geodesic.hpp"
#include <iostream>
#include <fstream>

float PI = 3.1415926535897f;

using namespace ATP::Wormhole::Ellis;

int main()
{
	ATP::Math::Vector camera(1, 2, 3);	//Camera location
	float psi, theta;					//Viewing direction
	float roll;							//Camera roll

	//--Begin metric test trace--
		//Open output file
		std::ofstream outFile;
		outFile.open("data.csv");

		//Iterate until heading directly away from the wormhole
		outFile << "p,t,dp,dt,r,x,y,z\n";

		float p = -10.0f;			//Location of camera in radial coordinates
		float t = PI;				//Location of camera in radial coordinates
		float T = 0.2507875f * PI;	//Viewing angle
		float w = 1.0f;				//Wormhole throat width
		float ds = 0.01f;			//Integration step size

		Geodesic g(p, t, T, w);

		//Set initial conditions
		Geodesic::Point start(g.reset());

		Geodesic::Point cur(start);
		while (fabs(cur.p()) <= fabs(start.p())) {
			cur = g.next(cur, ds);

			std::cout << cur.p() << "\t" << cur.t() << "\n";
			outFile << cur.p() << "," << cur.t() << "," << cur.dp() << "," << cur.dt() << "," << cur.r() << "," << cur.x() << "," << cur.y() << "," << cur.z() << "\n";
		}
		outFile.close();

	//-- End metric test trace --


	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

