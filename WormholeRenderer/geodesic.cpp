#include <math.h>
#include <iostream>

#include "geodesic.hpp"

//References
// [1] http://www.spacetimetravel.org/wurmlochflug/wurmlochflug.html
// [2] https://en.wikipedia.org/wiki/Ellis_wormhole

namespace ATP
{
	namespace Wormhole
	{
		namespace Ellis
		{
			Geodesic::Geodesic(double p, double t, double T, double w) {
				this->p = p;
				this->t = t;
				this->w = w;

				double b = p * p + w * w;	//Simplification
				double r = sqrt(b);			//Simplification [1]

				dp = cos(T);		//Initial value of dp/ds
				dt = sin(T) / r;	//Initial value of dt/ds
				
				//Determine characteristic constant (h) of the geodesic [2]
				float m = T > 0.0f ? 1.0 : -1.0;
				if (dp != 0.0f) {
					double a = dt / dp;			//Simplification
					h = m * sqrt(a*a*b*b / (a*a*b + 1));
				}
				else {
					h = m * sqrt(b);
				}
			}

			Geodesic::Point Geodesic::reset() {
				return Geodesic::Point(p, t, dp, dt, w, 1.0f);
			}

			Geodesic::Point Geodesic::next(Point cur, double ds) {
				//Get next t and p
				double p = cur.p() + cur.dp() * ds;
				double t = cur.t() + cur.dt() * ds;

				//Get new direction [2]
				double c = w * w + p * p;	//Simplification
				double d = 1.0f - h * h / c;	//Simplification
				if (d < 0.0f) d = -d;
				double dt = h / c;
				double dp = cur.m() * sqrt(d);

				//Check for turnaround condition [2]
				double m = cur.m();
				if (m > 0.0 && h*h > w*w && dp > cur.dp() && fabs(dp) < 0.01f) {
					m = -1.0;
					dp = -dp;
				}

				return Geodesic::Point(p, t, dp, dt, w, m);
			}

			Geodesic::Point Geodesic::trace(
				std::function<double(Geodesic::Point)> ds,
				std::function<bool(Geodesic::Point, Geodesic::Point)> isDone,
				std::function<void(Geodesic::Point, Geodesic::Point)> body
			) {
				Geodesic::Point start = reset();
				Geodesic::Point cur = start;
				while (!isDone(cur, start)) {
					body(cur, start);
					cur = next(cur, ds(cur));
				}

				return cur;
			}

			Geodesic::Point Geodesic::trace(
				std::function<double(Geodesic::Point)> ds,
				std::function<bool(Geodesic::Point, Geodesic::Point)> isDone
			) {
				return trace(ds, isDone, [](Geodesic::Point cur, Geodesic::Point start) {});
			}

			Geodesic::Point Geodesic::trace(
				double ds,
				std::function<bool(Geodesic::Point, Geodesic::Point)> isDone,
				std::function<void(Geodesic::Point, Geodesic::Point)> body
			) {
				return trace([&](Geodesic::Point cur) {return ds; }, isDone, body);
			}

			Geodesic::Point Geodesic::trace(
				double ds,
				std::function<bool(Geodesic::Point, Geodesic::Point)> isDone
			) {
				return trace(ds, isDone, [](Geodesic::Point cur, Geodesic::Point start) {});
			}
		}
	}
}
