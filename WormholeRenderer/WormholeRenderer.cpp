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
#include "vendor\dtschump\CImg\CImg.h"

using namespace cimg_library;

typedef CImg<unsigned char> Image;

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

	//Setup a local cartesian system at the camera's location with orientation of the system following the camera's field of view
	Cartesian::System localCartesianSystem(
		ATP::Math::Vector(-5.0, 0.0, 0.0),	//Camera location
		ATP::Math::Vector::X,				//Forward |
		ATP::Math::Vector::Y,				//Left    |-- Adjust these as necessary to account for viewing direction
		ATP::Math::Vector::Z				//Up      |
	);

	//Set resolution
	int scrWidth = 640;
	int scrHeight = 480;

	//Set viewing angle
	double viewingAngle = PI / 2.0;

	//Calculate screen distance
	double dist = scrWidth / (2.0 * tan(viewingAngle / 2.0));

	//Load background images via CImg
	unsigned int imgWidth = 1280;
	unsigned int imgHeight = 640;
	Image galaxy1("../../../textures/galaxy1.jpg");
	Image galaxy2("../../../textures/galaxy2.jpg");
	Image finalImage(scrWidth, scrHeight, 1, 3);

	//Iterate over screen pixels
	for (int y = 0; y < scrWidth; y++) {
		std::cout << ".";
		for (int z = 0; z < scrHeight; z++) {
			//Calculate ray vector in <p, t, T>
			ATP::Math::Vector rayLocal(
				dist,
				(double)(scrWidth / 2 - y),
				(double)(scrHeight / 2 - z)
			);

			//Find the absolute direction of the ray
			ATP::Math::Vector rayAbsolute = cartesianToCartesian(
				rayLocal,
				localCartesianSystem,
				Cartesian::System(
					localCartesianSystem.origin(),
					ATP::Math::Vector::X,
					ATP::Math::Vector::Y,
					ATP::Math::Vector::Z
				)
			).normalize();

			//Define a cartesian coordinate system in the plane of the ray and global origin
			ATP::Math::Vector xAxis = -1.0 * localCartesianSystem.origin().normalize();		// The x axis points back through the origin
			ATP::Math::Vector zAxis = (xAxis ^ rayAbsolute).normalize();					// The ray is defined to be in the direction of +y, so x cross r is in the direction of +z
			ATP::Math::Vector yAxis = zAxis ^ xAxis;										// Get y from the calculated x and z
			Cartesian::System system2D(ATP::Math::Vector::Null, xAxis, yAxis, zAxis);		// Setup the coordinate system

			//Get the initial spacetime coordinates for the geodesic
			double p = sqrt(localCartesianSystem.origin().length2() - w * w); // [1] From r^2 = p^2 + w^2
			double t = 0.0;
			double T = acos((xAxis * rayAbsolute) / (xAxis.length() * rayAbsolute.length()));	// From the definition of dot product a.b = |a||b|cos(T)
			
			//Trace ray until either:
			//	|pCurrent| >> |pInitial|, the ray is much farther away from the wormhole than it started
			//	dp == 0.0, The ray got stuck in the throat
			Geodesic::Point final = Geodesic(p, t, T, w).trace(
				[](Geodesic::Point cur) {return cur.r() / 10.0; },
				[](Geodesic::Point cur, Geodesic::Point start) {
					//TODO:  Add geometry intersection checks
					double threshold = 10.0;
					return
						fabs(cur.p() / start.p()) > threshold	// The ray is far away from the wormhole compared to the camera
						|| cur.dp() == 0.0;						// The ray got stuck in the throat
				}
			);

			//Determine which side of the wormhole the ray is on
			unsigned int index = final.p() <= 0 ? 0 : 1;

			//Translate back into global spherical coordinates
			ATP::Math::Vector final2D(
				final.r() * cos(final.t()),
				final.r() * sin(final.t()),
				0.0
			);
			Spherical::Vector finalSpherical = cartesianToSpherical(final2D, system2D);

			//Read off corresponding pixel from appropriate background image
			int yFinal = (unsigned int)(finalSpherical.theta / PI * (double)imgHeight);
			int xFinal = (unsigned int)(finalSpherical.psi / (2.0 * PI) * (double)imgWidth) + imgWidth / 2;

			for (unsigned int channel = 0; channel < 3; channel++) {
				finalImage(y, z, channel) = index ? galaxy1(xFinal, yFinal, channel) : galaxy2(xFinal, yFinal, channel);
			}

			//Antialias (if desired)
		}
	}

	//Save final image via CImg
	finalImage.save("render.jpg");
}

int main() {
	ATP::Math::Vector camera(1, 2, 3);	//Camera location
	double psi, theta;					//Viewing direction
	double roll;							//Camera roll

	//traceTest();
	backgroundRenderTest();

	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

