// WormholeRenderer.cpp : Defines the entry point for the console application.
//

#include "vendor\daemonalchemist\atp-vector\src\vector.hpp"
#include <iostream>
#include <fstream>

struct PolarPoint {
	float p;
	float t;
	float dp;
	float dt;
};

struct CartesianPoint {
	float r;
	float x;
	float y;
	float z;
};

PolarPoint getInitialConditions(float p, float t, float T, float w) {
	PolarPoint start;
	start.p = p;							//Location of camera in radial coordinates
	start.t = t;
	start.dp = cos(T);						//Initial value of dp/ds
	start.dt = sin(T) / (p * p + w * w);	//Initial value of dt/ds
	return start;
}

float getGeodesicConstant(PolarPoint& p, float w, float dir) {
	float b = p.p * p.p + w * w;	//Simplification
								
	//Determine characteristic constant (h) of the geodesic
	float h;
	if (p.dp != 0.0f) {
		float a = p.dt / p.dp;			//Simplification
		h = sqrt(a*a*b*b / (a*a*b + 1));
	}
	else {
		h = sqrt(b);
	}

	if (dir < 0.0f) {
		h = -h;
	}

	return h;
}

CartesianPoint getCartesianPoint(PolarPoint p, float w) {
	CartesianPoint cp;
	cp.r = sqrt(w * w + p.p * p.p);									//Cartesian radius
	cp.x = cp.r * cos(p.t);											//Cartesian x coord
	cp.y = cp.r * sin(p.t);											//Cartesian y coord
	cp.z = w * log(cp.r / w + sqrt(cp.r * cp.r / w / w - 1.0f));	//Cartesian z coord
	if (p.p < 0.0f) cp.z = -cp.z;
	return cp;
}

float m = 1.0f;  //TODO:  Bleh, global state :P
PolarPoint getNextPoint(PolarPoint cur, float h, float w, float ds) {
	PolarPoint next;

	//Get next t and p
	next.p = cur.p + cur.dp * ds;
	next.t = cur.t + cur.dt * ds;

	//Get new direction
	float c = w * w + next.p * next.p;	//Simplification
	float d = 1.0f - h * h / c;	//Simplification
	if (d < 0.0f) d = -d;
	next.dt = h / c;
	float dpOld = cur.dp;
	next.dp = m * sqrt(d);

	//Check for turnaround condition
	if (m > 0.0f && h*h > w*w && next.dp > dpOld && fabs(next.dp) < 0.01f) {
		m = -1.0f;
		next.dp = -next.dp;
	}

	return next;
}

float PI = 3.1415926535897f;

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
	
		float T = 0.2507875f * PI;						//Viewing angle
		float w = 1.0f;									//Wormhole throat width
		float ds = 0.01f;								//Integration step size
		float p = -10.0f;								//Location of camera in radial coordinates
		float t = PI;

		//Set initial conditions
		PolarPoint start = getInitialConditions(p, t, T, w);

		//Determine characteristic constant (h) of the geodesic
		float h = getGeodesicConstant(start, w, T);

		PolarPoint cur = start;
		while (fabs(cur.p) <= fabs(start.p)) {
			cur = getNextPoint(cur, h, w, ds);
			CartesianPoint cCur = getCartesianPoint(cur, w);

			std::cout << cur.p << "\t" << cur.t << "\n";
			outFile << cur.p << "," << cur.t << "," << cur.dp << "," << cur.dt << "," << cCur.r << "," << cCur.x << "," << cCur.y << "," << cCur.z << "\n";
		}
		outFile.close();

	//-- End metric test trace --


	//TODO:  Load geometry

	//TODO: Initialize spacetime

	//TODO:: Render scene

    return 0;
}

