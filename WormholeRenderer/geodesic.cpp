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

			Geodesic::Point::Point(float p, float t, float dp, float dt, float w, float m)
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

			float Geodesic::Point::p() const { return _p; }
			float Geodesic::Point::t() const { return _t; }
			float Geodesic::Point::dp() const { return _dp; }
			float Geodesic::Point::dt() const { return _dt; }
			float Geodesic::Point::r() const {return (float) sqrt(_w * _w + _p * _p);}
			float Geodesic::Point::x() const {return r() * (float)cos(_t);}
			float Geodesic::Point::y() const { return r() * (float)sin(_t); }
			float Geodesic::Point::z() const {
				float z = _w * (float)log(r() / _w + (float)sqrt(r() * r() / _w / _w - 1.0f));	//Cartesian z coord
				if (_p < 0.0f) z = -z;
				return z;
			}
			float Geodesic::Point::w() const { return _w; }
			float Geodesic::Point::m() const { return _m; }

			Geodesic::Geodesic(float p, float t, float T, float w)
			{
				this->p = p;
				this->t = t;
				this->w = w;

				float b = p * p + w * w;	//Simplification

				dp = (float)cos(T);		//Initial value of dp/ds
				dt = (float)sin(T) / b;	//Initial value of dt/ds
				
				//Determine characteristic constant (h) of the geodesic
				if (dp != 0.0f) {
					float a = dt / dp;			//Simplification
					h = (float)sqrt(a*a*b*b / (a*a*b + 1));
				}
				else {
					h = (float)sqrt(b);
				}

				if (T < 0.0f) {
					h = -h;
				}
			}

			Geodesic::Point Geodesic::reset() {
				return Geodesic::Point(p, t, dp, dt, w, 1.0f);
			}

			Geodesic::Point Geodesic::next(Point cur, float ds) {
				//Get next t and p
				float p = cur.p() + cur.dp() * ds;
				float t = cur.t() + cur.dt() * ds;

				//Get new direction
				float c = w * w + p * p;	//Simplification
				float d = 1.0f - h * h / c;	//Simplification
				if (d < 0.0f) d = -d;
				float dt = h / c;
				float dp = cur.m() * (float)sqrt(d);

				//Check for turnaround condition
				float m = 1.0f;
				if (cur.m() > 0.0f && h*h > w*w && dp > cur.dp() && fabs(dp) < 0.01f) {
					m = -1.0f;
					dp = -dp;
				}

				return Geodesic::Point(p, t, dp, dt, w, m);
			}
		}
	}
}
