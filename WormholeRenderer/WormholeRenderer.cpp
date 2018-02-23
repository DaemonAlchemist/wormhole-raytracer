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
#include <string>
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

void backgroundRenderTest(double offset, std::string outputFileName, bool debug = false) {
	double w = 1.0;		//Wormhole throat width

	//Setup a local cartesian system at the camera's location with orientation of the system following the camera's field of view
	Cartesian::System localCartesianSystem(
		ATP::Math::Vector(-offset, 0.0, 0.0),	//Camera location
		ATP::Math::Vector::X,				//Forward |
		ATP::Math::Vector::Y,				//Left    |-- Adjust these as necessary to account for viewing direction
		ATP::Math::Vector::Z				//Up      |
	);

	//Set resolution
	int scrWidth = 1024;
	int scrHeight = 1024;

	//Set viewing angle
	double viewingAngle = PI / 2.0;

	//Calculate screen distance
	double dist = scrWidth / (2.0 * tan(viewingAngle / 2.0));

	//Load background images via CImg
	std::cout << "Loading image 1\n";
	Image galaxy1("../../../textures/galaxy2.jpg");
	std::cout << galaxy1.width() << "x" << galaxy1.height() << "\n";
	std::cout << "Loading image 2\n";
	Image galaxy2("../../../textures/galaxy1.jpg");
	std::cout << galaxy2.width() << "x" << galaxy2.height() << "\n";
	Image finalImage(scrWidth, scrHeight, 1, 3);

	//Open debug file
	std::ofstream outFile;
	if(debug) outFile.open("debug.csv");

	//Iterate until heading directly away from the wormhole
	if(debug) outFile << "x,y,dx,dy,dz,dx(abs),dy(abs),dz(abs),x.x,x.y,x.z,y.x,y.y,y.z,z.x,z.y,z.z,pInitial,tInitial,T,pFinal,tFinal,rFinal,x,y,z,r,theta,psi\n";

	//Iterate over screen pixels
	#pragma loop(hint_parallel(0))
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
			//TODO:  Handle edge case where |z| == 0.0
			ATP::Math::Vector xAxis = -1.0 * localCartesianSystem.origin().normalize();		// The x axis points back through the origin
			ATP::Math::Vector zAxis = (xAxis ^ rayAbsolute).normalize();					// The ray is defined to be in the direction of +y, so x cross r is in the direction of +z
			ATP::Math::Vector yAxis = zAxis ^ xAxis;										// Get y from the calculated x and z
			Cartesian::System system2D(ATP::Math::Vector::Null, xAxis, yAxis, zAxis);		// Setup the coordinate system

			//Get the initial spacetime coordinates for the geodesic
			//TODO:  Handle case where camera is on the opposite side of the wormhole, such that p < 0.0
			double p = -sqrt(localCartesianSystem.origin().length2() - w * w); // [1] From r^2 = p^2 + w^2
			double t = PI;
			double T = acos((xAxis * rayAbsolute) / (xAxis.length() * rayAbsolute.length()));	// From the definition of dot product a.b = |a||b|cos(T)
			
			//Trace ray until either:
			//	|pCurrent| >> |pInitial|, the ray is much farther away from the wormhole than it started
			//	dp == 0.0, The ray got stuck in the throat
			Geodesic::Point final = Geodesic(p, t, T, w).trace(
				0.01,
				[](Geodesic::Point cur, Geodesic::Point start) {
					//TODO:  Add geometry intersection checks
					double threshold = 10.0;
					return
						fabs(cur.p() / start.p()) > threshold	// The ray is far away from the wormhole compared to the camera
						|| cur.dp() == 0.0;						// The ray got stuck in the throat
				}
			);

			//Translate back into global spherical coordinates
			ATP::Math::Vector final2D(
				final.r() * cos(final.t()),
				final.r() * sin(final.t()),
				0.0
			);
			Spherical::Vector finalSpherical = cartesianToSpherical(final2D, system2D);

			//Determine which side of the wormhole the ray is on
			Image* finalBackground = final.p() <= 0 ? &galaxy1 : &galaxy2;

			//Read off corresponding pixel from appropriate background image
			unsigned int imgHeight = finalBackground->height();
			unsigned int imgWidth = finalBackground->width();
			int yFinal = (unsigned int)(finalSpherical.theta / PI * (double)imgHeight);
			int xFinal = (unsigned int)(finalSpherical.psi / (2.0 * PI) * (double)imgWidth) + imgWidth / 2;

			for (unsigned int channel = 0; channel < 3; channel++) {
				finalImage(y, z, channel) = (*finalBackground)(xFinal, yFinal, channel);
			}

			if (debug) {
				outFile << y << "," << z << ",";
				outFile << rayLocal.x << "," << rayLocal.y << "," << rayLocal.z << ",";
				outFile << rayAbsolute.x << "," << rayAbsolute.y << "," << rayAbsolute.z << ",";
				outFile << xAxis.x << "," << xAxis.y << "," << xAxis.z << ",";
				outFile << yAxis.x << "," << yAxis.y << "," << yAxis.z << ",";
				outFile << zAxis.x << "," << zAxis.y << "," << zAxis.z << ",";
				outFile << p << "," << t << "," << T << ",";
				outFile << final.p() << "," << final.t() << "," << final.r() << ",";
				outFile << final2D.x << "," << final2D.y << "," << final2D.z << ",";
				outFile << finalSpherical.r << "," << finalSpherical.theta << "," << finalSpherical.psi << "\n";
			}

			//Antialias (if desired)
		}
	}

	//Save final image via CImg
	finalImage.save(outputFileName.c_str());
	if(debug) outFile.close();
}

int main() {
	//traceTest();
	//Loop on offset for animation
	unsigned int i = 0;
	for (double d = -20.0; d <= 0.0; d += 0.2) {
		std::string index = std::to_string(i);
		while (index.length() < 3) index = std::string("0") + index;
		std::string fileName = std::string("render") + index + std::string(".jpg");
		backgroundRenderTest(d, fileName, false);
		i++;
	}

	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

