#ifndef ELLIS_GEODESIC_H
#define ELLIS_GEODESIC_H

#include <functional>

namespace ATP
{
	namespace Wormhole
	{
		namespace Ellis
		{
			class Geodesic {
			public:
				#include "geodesic-point.hpp"

				Geodesic(double p, double t, double T, double w);

				Point reset();
				Point next(Point cur, double ds);

				Point trace(double ds, std::function<bool(Point, Point)> isDone, std::function<void(Point, Point)> body);

			private:
				double p, t, dp, dt, w, h;
			};
		}
	}
}

#endif
