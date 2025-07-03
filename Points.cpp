#include "Points.h"


//helper functions
// finds area of triangle formed using three vertices. This is used for initialising the first face.
int area(Point3d& p1, Point3d& p2, Point3d& p3) {
	Point3d ans = (p2 - p1).cross(p3 - p1);
	int a = ans.x;
	int b = ans.y;
	int c = ans.z;
	int area = a * a + b * b + c * c;
	return area;
}


//2-D point
Point2d::Point2d() = default;
Point2d::Point2d(int a, int b) : x(a), y(b) {}
Point2d Point2d::operator+(const Point2d& A) 
{
	Point2d res(x + A.x, y + A.y);
	return res;
}
Point2d Point2d::operator-(const Point2d& A)
{
	Point2d res(x - A.x, y - A.y);
	return res;
}
bool Point2d::operator==(const Point2d& A)
{
	return (x == A.x) && (y == A.y);
}

//2-D side
Side::Side()=default;
Side::Side(Point2d A, Point2d B) :p1(A), p2(B) { modulus = norm((p1.x - p2.x), (p1.y - p2.y)); }
int Side::dot(Side& A) {
	return (p2.x - p1.x) * (A.p2.x - A.p1.x) + (p2.y - p1.y) * (A.p2.y - A.p1.y);
}
double Side::angle(Side& A, Side& B) 
{
	double ans = acos((A.dot(B) * 1.0) / (A.modulus * B.modulus));
	return ans;
}

int Side::cross(Side& A) {
	return ((p2.x-p1.x) * (A.p2.y-A.p1.y)) - ((p2.y-p1.y) * (A.p2.x-A.p1.x));
}






// 3-D point

Point3d::Point3d(int a, int b, int c) : x(a), y(b), z(c) {}
Point3d::Point3d() = default;
Point3d Point3d::operator-(const Point3d& o) {
	return Point3d(x - o.x, y - o.y, z - o.z);
}

Point3d Point3d::cross(const Point3d& o) {
	return Point3d(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
}
long Point3d::dot(const Point3d& o){
	return x * o.x + y * o.y + z * o.z;
}


