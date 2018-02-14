#ifndef ELLIS_GEODESIC_H
#define ELLIS_GEODESIC_H

namespace ATP
{
	namespace Wormhole
	{
		namespace Ellis
		{
			class Geodesic {
			public:
				class Point {
				public:
					Point() {}
					Point(const Point& p);
					Point(double p, double t, double dp, double dt, double w, double m);
					Point& operator=(const Point& p);

					double p() const;
					double t() const;
					double dp() const;
					double dt() const;
					double r() const;
					double x() const;
					double y() const;
					double z() const;
					double w() const;
					double m() const;

				private:
					double _p, _t, _dp, _dt, _w, _m;
				};

				Geodesic(double p, double t, double T, double w);

				Point reset();
				Point next(Point cur, double ds);

			private:
				double p, t, dp, dt, w, h;
			};
		}
	}
}

#endif
