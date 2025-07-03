#pragma once
#include<cmath>
#include<vector>
#include<map>
#include<algorithm>
#include<climits>
using namespace std;

template <typename T>
T norm(T x, T y)
{
	return sqrt(x * x + y * y);
}
template <typename T>
T norm(T x, T y, T z)
{
	return sqrt(x * x + y * y + z * z);
}
struct Point2d
{
	int x{}, y{};
	Point2d();
	Point2d(int a, int b);
	Point2d operator+(const Point2d& A);
	Point2d operator-(const Point2d& A); 
	bool operator==(const Point2d& A);
	bool operator<(const Point2d& other) const {
		return (x < other.x) || (x == other.x && y < other.y);
	}
};


struct Side
{
	Point2d p1{}, p2{};
	double modulus{-1};
	Side();
	Side(Point2d A, Point2d B);
	int dot(Side& A);
	static double angle( Side& A, Side& B);
	int cross(Side& A);
};

struct Point3d
{
	int x{}, y{}, z{};
	Point3d();
	Point3d(int a, int b, int c);
	Point3d operator-(const Point3d& A);
	Point3d cross(const Point3d& o);
	long dot(const Point3d& o);

};
// Face struct stores the indices of its vertices a,b,c
// It also stores an outward-facing normal vector norm
struct face {
	int a, b, c;
	Point3d norm;
};

//helper functions
// finds area of triangle formed using three vertices. This is used for initialising the first face.
int area(Point3d& p1, Point3d& p2, Point3d& p3); 
