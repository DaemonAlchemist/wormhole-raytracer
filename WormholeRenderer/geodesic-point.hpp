class Point {
public:
	Point() {}
	Point(const Point& p);
	Point(double p, double t, double dp, double dt, double w, double m, double mInitial);
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
	double mInitial() const;

private:
	double _p, _t, _dp, _dt, _w, _m, _mInitial;
};
