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

//References
// [1] http://www.spacetimetravel.org/wurmlochflug/wurmlochflug.html
// [2] https://en.wikipedia.org/wiki/Ellis_wormhole

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
	double w = 1.0;		//Wormhole throat width
	double ds = 0.01;	//Integration step size


	//Setup a local cartesian system at the camera's location with orientation of the system following the camera's field of view
	Cartesian::System localCartesianSystem(
		ATP::Math::Vector(-5.0, 0.0, 0.0),	//Camera location
		ATP::Math::Vector::X,				//Forward |
		ATP::Math::Vector::Y,				//Left    |-- Adjust these as necessary to account for viewing direction
		ATP::Math::Vector::Z				//Up      |
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
			ATP::Math::Vector rayLocal(
				(double)(x - scrWidth / 2),
				dist,
				(double)(z - scrHeight / 2)
			);

			//Find the absolute direction of the ray
			ATP::Math::Vector rayAbsolute = cartesianToAbsolute(rayLocal, localCartesianSystem);

			//Define a cartesian coordinate system in the plane of the ray and global origin
			ATP::Math::Vector xAxis = -1.0 * localCartesianSystem.origin().normalize();		// The x axis points back through the origin
			ATP::Math::Vector zAxis = (xAxis ^ rayAbsolute).normalize();					// The ray is defined to be in the direction of +y, so x cross r is in the direction of +z
			ATP::Math::Vector yAxis = zAxis ^ xAxis;										// Get y from the calculated x and z
			Cartesian::System system2D(ATP::Math::Vector::Null, xAxis, yAxis, zAxis);		// Setup the coordinate system

			//Get the initial spacetime coordinates for the geodesic
			double p = sqrt(system2D.origin().length2() - w * w); // [1] From r^2 = p^2 + w^2
			double t = 0.0;
			double T = acos((xAxis * rayAbsolute) / (xAxis.length() * rayAbsolute.length()));	// From the definition of dot product a.b = |a||b|cos(T)
			
			//Trace ray until either:
			//	(dp < 0 && p < 0 || dp > 0 && p > 0) && ds is small, this means the ray is headed almost directly away from the wormhole
			//	dp == 0.0, The ray got stuck in the throat
			Geodesic::Point final = Geodesic(p, t, T, w).trace(
				ds,
				[](Geodesic::Point cur, Geodesic::Point start) {
					double threshold = 0.001;
					return
						(cur.dp() < 0 && cur.p() < 0 || cur.dp() > 0 && cur.p() > 0) && fabs(cur.dt()) < threshold  // The ray is headed almost directly away from the wormhole
						|| cur.dp() == 0.0;																			// The ray got stuck in the throat
				},
				[](Geodesic::Point cur, Geodesic::Point start) {
					//NOOP:  Currently we don't need to do anything here.  We just need the final point
				}
			);

			//Read off final coordinates
			double tFinal = final.t();
			double rFinal = sqrt(final.p() * final.p() + w * w);

			//Determine which side of the wormhole the ray got to
			unsigned int index = final.p() <= 0 ? 0 : 1;

			//Translate back into global spherical coordinates
			ATP::Math::Vector final2D(
				rFinal * cos(tFinal),
				rFinal * sin(tFinal),
				0.0
			);
			Spherical::Vector finalSpherical = cartesianToSpherical(final2D, system2D);

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

