// WormholeRenderer.cpp : Defines the entry point for the console application.
//

#include "vendor\daemonalchemist\atp-vector\src\vector.hpp"
#include <iostream>
#include <fstream>

int main()
{
	ATP::Math::Vector camera(1, 2, 3);	//Camera location
	float psi, theta;					//Viewing direction
	float roll;							//Camera roll

	//--Begin metric test trace--
		float PI = 3.1415926535897f;
		float p = -10.0f, t = 0.0f;		//Location of camera in radial coordinates
		float T = 0.2507875f * PI;			//Viewing angle
		//float T = 0.2507918f * PI;			//Viewing angle
		float w = 1.0f;					//Throat width

		//Determine initial direction
		float b = p * p + w * w;	//Simplification
		float dp = cos(T);			//Initial value of dp/ds
		float dt = sin(T) / b;		//Initial value of dt/ds
		float ds = 0.01f;			//Integration step size

		//Determine characteristic constant (h) of the geodesic
		float h;
		if (dp != 0.0f) {
			float a = dt / dp;			//Simplification
			h = sqrt(a*a*b*b / (a*a*b + 1));
		}
		else {
			h = sqrt(b);
		}

		//Open output file
		std::ofstream outFile;
		outFile.open("data.csv");
		
		//Iterate until heading directly away from the wormhole
		float pInitial = p;
		float m = 1.0f;
		outFile << "p,t,dp,dt,h,hh/(ww+pp),r,x,y,z\n";
		while (fabs(p) <= fabs(pInitial)) {
			float r = sqrt(w * w + p * p);							//Cartesian radius
			float x = r * cos(t);									//Cartesian x coord
			float y = r * sin(t);									//Cartesian y coord
			float z = w * log(r / w + sqrt(r*r / w / w - 1.0f));	//Cartesian z coord
			if (p < 0.0f) z = -z;

			//Get next t and p
			p += dp * ds;
			t += dt * ds;

			//Get new direction
			float c = w * w + p * p;	//Simplification
			float d = 1.0f - h * h / c;	//Simplification
			if (d < 0.0f) d = -d;
			dt = h / c;
			float dpOld = dp;
			dp = m * sqrt(d);

			//Check for turnaround condition
			if (m > 0.0f && h*h > w*w && dp > dpOld && fabs(dp) < 0.01f) {
				m = -1.0f;
				dp = -dp;
			}

			std::cout << p << "\t" << t << "\n";
			outFile << p << "," << t << "," << dp << "," << dt << "," << h << "," << (h*h/c) << "," << r << "," << x << "," << y << "," << z << "\n";
		}
		outFile.close();

	//-- End metric test trace --


	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

