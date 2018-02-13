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
					Point(float p, float t, float dp, float dt, float w, float m);
					Point& operator=(const Point& p);

					float p() const;
					float t() const;
					float dp() const;
					float dt() const;
					float r() const;
					float x() const;
					float y() const;
					float z() const;
					float w() const;
					float m() const;

				private:
					float _p, _t, _dp, _dt, _w, _m;
				};

				Geodesic(float p, float t, float T, float w);

				Point reset();
				Point next(Point cur, float ds);

			private:
				float p, t, dp, dt, w, h;
			};
		}
	}
}

#endif
