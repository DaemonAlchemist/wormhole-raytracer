#include <math.h>

#include "geodesic.hpp"

namespace ATP
{
	namespace Wormhole
	{
		namespace Ellis
		{
			Geodesic::Point::Point(const Geodesic::Point& p){
				(*this) = p;
			}

			Geodesic::Point::Point(double p, double t, double dp, double dt, double w, double m)
			{
				_p = p;
				_t = t;
				_dp = dp;
				_dt = dt;
				_w = w;
				_m = m;
			}

			Geodesic::Point& Geodesic::Point::operator=(const Point& p) {
				_p = p.p();
				_t = p.t();
				_dp = p.dp();
				_dt = p.dt();
				_w = p.w();
				_m = p.m();
				return *this;
			}

			double Geodesic::Point::p() const { return _p; }
			double Geodesic::Point::t() const { return _t; }
			double Geodesic::Point::dp() const { return _dp; }
			double Geodesic::Point::dt() const { return _dt; }
			double Geodesic::Point::r() const {return sqrt(_w * _w + _p * _p);}
			double Geodesic::Point::x() const {return r() * cos(_t);}
			double Geodesic::Point::y() const { return r() * sin(_t); }
			double Geodesic::Point::z() const {
				double z = _w * log(r() / _w + sqrt(r() * r() / _w / _w - 1.0f));	//Cartesian z coord
				if (_p < 0.0f) z = -z;
				return z;
			}
			double Geodesic::Point::w() const { return _w; }
			double Geodesic::Point::m() const { return _m; }

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
