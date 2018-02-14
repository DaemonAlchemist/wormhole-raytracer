#include <math.h>

#include "geodesic.hpp"

namespace ATP
{
	namespace Wormhole
	{
		namespace Ellis
		{
			Geodesic::Geodesic(double p, double t, double T, double w)
			{
				this->p = p;
				this->t = t;
				this->w = w;

				double b = p * p + w * w;	//Simplification

				dp = cos(T);		//Initial value of dp/ds
				dt = sin(T) / b;	//Initial value of dt/ds
				
				//Determine characteristic constant (h) of the geodesic
				if (dp != 0.0f) {
					double a = dt / dp;			//Simplification
					h = sqrt(a*a*b*b / (a*a*b + 1));
				}
				else {
					h = sqrt(b);
				}

				if (T < 0.0f) {
					h = -h;
				}
			}

			Geodesic::Point Geodesic::reset() {
				return Geodesic::Point(p, t, dp, dt, w, 1.0f);
			}

			Geodesic::Point Geodesic::next(Point cur, double ds) {
				//Get next t and p
				double p = cur.p() + cur.dp() * ds;
				double t = cur.t() + cur.dt() * ds;

				//Get new direction
				double c = w * w + p * p;	//Simplification
				double d = 1.0f - h * h / c;	//Simplification
				if (d < 0.0f) d = -d;
				double dt = h / c;
				double dp = cur.m() * sqrt(d);

				//Check for turnaround condition
				double m = 1.0f;
				if (cur.m() > 0.0f && h*h > w*w && dp > cur.dp() && fabs(dp) < 0.01f) {
					m = -1.0f;
					dp = -dp;
				}

				return Geodesic::Point(p, t, dp, dt, w, m);
			}
		}
	}
}
