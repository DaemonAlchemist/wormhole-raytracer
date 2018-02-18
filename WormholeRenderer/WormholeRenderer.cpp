// WormholeRenderer.cpp : Defines the entry point for the console application.
//

#include "vendor\daemonalchemist\atp-vector\src\vector.hpp"
#include "vendor\daemonalchemist\atp-coordinate-systems\src\spherical-vector.hpp"
#include "vendor\daemonalchemist\atp-coordinate-systems\src\spherical.hpp"
#include "vendor\daemonalchemist\atp-coordinate-systems\src\cartesian.hpp"
#include "vendor\daemonalchemist\atp-coordinate-systems\src\conversion.hpp"
#include "geodesic.hpp"
#include <iostream>
#include <fstream>

double PI = 3.1415926535897;

using namespace ATP::Wormhole::Ellis;
using namespace ATP::Math::CoordinateSystems;

void traceTest() {
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

	Geodesic(p, t, T, w).trace(
		ds,
		[](Geodesic::Point cur, Geodesic::Point start) {return fabs(cur.p()) > fabs(start.p()); },
		[&](Geodesic::Point cur, Geodesic::Point start) {
		std::cout << cur.p() << "\t" << cur.t() << "\n";
		outFile << cur.p() << "," << cur.t() << "," << cur.dp() << "," << cur.dt() << "," << cur.m() << "," << cur.r() << "," << cur.x() << "," << cur.y() << "," << cur.z() << "\n";
	}
	);
	outFile.close();
}

void backgroundRenderTest() {
	//Set initial camera location <p, t, T>
	ATP::Math::Vector camera(-5.0, 0.0, 0.0);

	//Set forward vector <p, t, T>
	ATP::Math::Vector forward(1.0, 0.0, 0.0);

	//Set right vector <p, t, T>
	ATP::Math::Vector right(0.0, 1.0, 0.0);

	//Set up local spherical coordinate system
	Spherical::System localSphericalSystem(camera, right, forward);

	//Read off the local cartesian system
	Cartesian::System localCartesianSystem(
		localSphericalSystem.origin(),
		localSphericalSystem.xAxis(),
		localSphericalSystem.yAxis(),
		localSphericalSystem.zAxis()
	);

	//Set resolution
	unsigned int scrWidth = 640;
	unsigned int scrHeight = 480;

	//Set viewing angle
	double viewingAngle = PI / 2.0;

	//Calculate screen distance
	double dist = scrWidth / (2.0 * tan(viewingAngle / 2.0));

	//TODO: Load background images

	//Iterate over screen pixels
	for (unsigned int x = 0; x < scrWidth; x++) {
		for (unsigned int z = 0; z < scrHeight; z++) {
			//Calculate ray vector in <p, t, T>
			ATP::Math::Vector rayCartesian(
				(double)(x - scrWidth / 2),
				dist,
				(double)(z - scrHeight / 2)
			);

			//Define a cartesian coordinate system in the plane of the ray and global origin
			ATP::Math::Vector xAxis = ATP::Math::Vector(-rayCartesian.x, 0.0, 0.0).normalize();
			ATP::Math::Vector zAxis = (xAxis ^ rayCartesian).normalize();
			ATP::Math::Vector yAxis = zAxis ^ xAxis;
			Cartesian::System system2D(localCartesianSystem.origin(), xAxis, yAxis, zAxis);

			//Calculate 2d ray vector <p, T>
			//Trace ray until dt/ds is small OR dp/ds == 0
			//Read off final T angle
			//Translate back into 3d direction
			//Read off corresponding pixel from appropriate background image
			//Antialias (if desired)
		}
	}
	//Save final image
}

int main() {
	ATP::Math::Vector camera(1, 2, 3);	//Camera location
	double psi, theta;					//Viewing direction
	double roll;							//Camera roll

	traceTest();
	//backgroundRenderTest();

	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

