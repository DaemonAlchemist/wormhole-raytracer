#include <math.h>

#include "geodesic.hpp"

namespace ATP
{
	namespace Wormhole
	{
		namespace Ellis
		{
			Geodesic::Point::Point(const Geodesic::Point& p) {
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
			double Geodesic::Point::r() const { return sqrt(_w * _w + _p * _p); }
			double Geodesic::Point::x() const { return r() * cos(_t); }
			double Geodesic::Point::y() const { return r() * sin(_t); }
			double Geodesic::Point::z() const {
				double z = _w * log(r() / _w + sqrt(r() * r() / _w / _w - 1.0f));	//Cartesian z coord
				if (_p < 0.0f) z = -z;
				return z;
			}
			double Geodesic::Point::w() const { return _w; }
			double Geodesic::Point::m() const { return _m; }
		}
	}
}
