// WormholeRenderer.cpp : Defines the entry point for the console application.
//

#include "vendor\daemonalchemist\atp-vector\src\vector.hpp"
#include "geodesic.hpp"
#include <iostream>
#include <fstream>

double PI = 3.1415926535897;

using namespace ATP::Wormhole::Ellis;

int main() {
	ATP::Math::Vector camera(1, 2, 3);	//Camera location
	double psi, theta;					//Viewing direction
	double roll;							//Camera roll

	//--Begin metric test trace--
		//Open output file
		std::ofstream outFile;
		outFile.open("data.csv");

		//Iterate until heading directly away from the wormhole
		outFile << "p,t,dp,dt,m,r,x,y,z\n";

		double p = -10.0;			//Location of camera in radial coordinates
		double t = PI;				//Location of camera in radial coordinates
		double T = 0.0317325 * PI;	//Viewing angle
		double w = 1.0;				//Wormhole throat width
		double ds = 0.01;			//Integration step size

		Geodesic g(p, t, T, w);

		//Set initial conditions
		Geodesic::Point start(g.reset());

		Geodesic::Point cur(start);
		while (fabs(cur.p()) <= fabs(start.p())) {
			cur = g.next(cur, ds);

			std::cout << cur.p() << "\t" << cur.t() << "\n";
			outFile << cur.p() << "," << cur.t() << "," << cur.dp() << "," << cur.dt() << "," << cur.m() << "," << cur.r() << "," << cur.x() << "," << cur.y() << "," << cur.z() << "\n";
		}
		outFile.close();

	//-- End metric test trace --

	//--Begin background render test--

		//Set initial camera location <p, t, T>
		//Set forward vector <p, t, T>
		//Set up vector <p, t, T>
		//Set aspect ratio
		//Set viewing angle
		//Calculate screen extents
		//Load background images
		//Iterate over screen pixels
			//Calculate ray vector in <p, t, T>
			//Calculate 2d ray vector <p, T>
			//Trace ray until dt/ds is small OR dp/ds == 0
			//Read off final T angle
			//Translate back into 3d direction
			//Read off corresponding pixel from appropriate background image
			//Antialias (if desired)
		//Save final image

	//-- End background render text --


	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

