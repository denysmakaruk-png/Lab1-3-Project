#ifndef GEOMETRY_2D_HPP
#define GEOMETRY_2D_HPP

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <variant>
#include <iostream>

namespace Geometry {

    constexpr double EPS = 1e-7;
    constexpr double PI = 3.14159265358979323846;

    inline bool isZero(double x) {
        return std::fabs(x) < EPS;
    }

    inline bool areEqual(double a, double b) {
        return std::fabs(a - b) < EPS;
    }

    struct Point2D {
        double x;
        double y;

        Point2D(double x_ = 0.0, double y_ = 0.0) : x(x_), y(y_) {}

        double distanceTo(const Point2D& other) const {
            return std::hypot(x - other.x, y - other.y);
        }

        double distanceSquaredTo(const Point2D& other) const {
            double dx = x - other.x;
            double dy = y - other.y;
            return dx * dx + dy * dy;
        }

        bool operator==(const Point2D& other) const {
            return areEqual(x, other.x) && areEqual(y, other.y);
        }

        bool operator!=(const Point2D& other) const {
            return !(*this == other);
        }

        bool operator<(const Point2D& other) const {
            if (!areEqual(x, other.x)) return x < other.x;
            return y < other.y - EPS;
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << "(" << x << ", " << y << ")";
            return oss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const Point2D& p) {
            return os << p.toString();
        }
    };

    class Circle2D;

    class Line2D {
    private:
        double A, B, C;

        void normalize() {
            double norm = std::hypot(A, B);
            if (isZero(norm)) {
                throw std::invalid_argument("Invalid line: A and B cannot both be zero.");
            }
            A /= norm;
            B /= norm;
            C /= norm;
            if (A < -EPS || (isZero(A) && B < -EPS)) {
                A = -A;
                B = -B;
                C = -C;
            }
        }

    public:
        Line2D(double a = 1.0, double b = -1.0, double c = 0.0) : A(a), B(b), C(c) {
            normalize();
        }

        static Line2D fromPoints(const Point2D& p1, const Point2D& p2) {
            if (p1 == p2) throw std::invalid_argument("Points must be distinct to define a line.");
            double a = p2.y - p1.y;
            double b = p1.x - p2.x;
            double c = -(a * p1.x + b * p1.y);
            return Line2D(a, b, c);
        }

        double getA() const { return A; }
        double getB() const { return B; }
        double getC() const { return C; }

        double signedDistance(const Point2D& p) const {
            return A * p.x + B * p.y + C;
        }

        double distanceTo(const Point2D& p) const {
            return std::fabs(signedDistance(p));
        }

        bool contains(const Point2D& p) const {
            return isZero(signedDistance(p));
        }

        Point2D reflect(const Point2D& p) const {
            double d = signedDistance(p);
            return Point2D(p.x - 2.0 * A * d, p.y - 2.0 * B * d);
        }

        Line2D reflect(const Line2D& other) const {
            Point2D p1(-other.A * other.C, -other.B * other.C);
            Point2D p2(p1.x - other.B, p1.y + other.A);
            return Line2D::fromPoints(reflect(p1), reflect(p2));
        }

        std::vector<Point2D> intersect(const Line2D& other) const {
            double det = A * other.B - other.A * B;
            if (isZero(det)) {
                return {};
            }
            double x = (B * other.C - other.B * C) / det;
            double y = (C * other.A - other.C * A) / det;
            return { Point2D(x, y) };
        }

        double angleWith(const Line2D& other) const {
            double dot = std::fabs(A * other.A + B * other.B);
            if (dot > 1.0) dot = 1.0;
            return std::acos(dot);
        }

        bool operator==(const Line2D& other) const {
            return areEqual(A, other.A) && areEqual(B, other.B) && areEqual(C, other.C);
        }

        bool operator<(const Line2D& other) const {
            if (!areEqual(A, other.A)) return A < other.A;
            if (!areEqual(B, other.B)) return B < other.B;
            return C < other.C - EPS;
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3)
                << "Line(" << A << "*x + " << B << "*y + " << C << " = 0)";
            return oss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const Line2D& l) {
            return os << l.toString();
        }
    };

    using InvertedShape = std::variant<Line2D, Circle2D>;

    class Circle2D {
    private:
        Point2D center;
        double radius;

    public:
        Circle2D(const Point2D& c = Point2D(0, 0), double r = 1.0) : center(c), radius(r) {
            if (r <= EPS) throw std::invalid_argument("Circle radius must be positive.");
        }

        Point2D getCenter() const { return center; }
        double getRadius() const { return radius; }

        double area() const { return PI * radius * radius; }
        double circumference() const { return 2.0 * PI * radius; }

        bool containsPoint(const Point2D& p) const {
            return areEqual(center.distanceTo(p), radius);
        }

        Circle2D reflectAcross(const Line2D& line) const {
            return Circle2D(line.reflect(center), radius);
        }

        std::vector<Point2D> intersect(const Line2D& line) const {
            double d = line.signedDistance(center);
            double dist = std::fabs(d);
            if (dist > radius + EPS) return {};

            Point2D proj(center.x - line.getA() * d, center.y - line.getB() * d);
            if (areEqual(dist, radius)) {
                return { proj };
            }

            double h = std::sqrt((std::max)(0.0, radius * radius - d * d));
            double dx = -line.getB() * h;
            double dy = line.getA() * h;
            return {
                Point2D(proj.x + dx, proj.y + dy),
                Point2D(proj.x - dx, proj.y - dy)
            };
        }

        std::vector<Point2D> intersect(const Circle2D& other) const {
            double d = center.distanceTo(other.center);
            if (d > radius + other.radius + EPS) return {};
            if (d < std::fabs(radius - other.radius) - EPS) return {};
            if (isZero(d) && areEqual(radius, other.radius)) return {};

            double a = (radius * radius - other.radius * other.radius + d * d) / (2.0 * d);
            double h = std::sqrt((std::max)(0.0, radius * radius - a * a));

            double x2 = center.x + a * (other.center.x - center.x) / d;
            double y2 = center.y + a * (other.center.y - center.y) / d;

            if (isZero(h)) {
                return { Point2D(x2, y2) };
            }

            double rx = -(other.center.y - center.y) * (h / d);
            double ry = (other.center.x - center.x) * (h / d);

            return {
                Point2D(x2 + rx, y2 + ry),
                Point2D(x2 - rx, y2 - ry)
            };
        }

        Point2D invert(const Point2D& p) const {
            double d2 = center.distanceSquaredTo(p);
            if (isZero(d2)) {
                throw std::domain_error("Inversion of circle center goes to infinity.");
            }
            double factor = (radius * radius) / d2;
            return Point2D(
                center.x + factor * (p.x - center.x),
                center.y + factor * (p.y - center.y)
            );
        }

        InvertedShape invert(const Line2D& line) const {
            double d = line.signedDistance(center);
            if (isZero(d)) {
                return line;
            }
            Point2D foot(center.x - line.getA() * d, center.y - line.getB() * d);
            Point2D footInv = invert(foot);
            Point2D newCenter((center.x + footInv.x) * 0.5, (center.y + footInv.y) * 0.5);
            double newRadius = center.distanceTo(footInv) * 0.5;
            return Circle2D(newCenter, newRadius);
        }

        InvertedShape invert(const Circle2D& other) const {
            double d = center.distanceTo(other.center);
            if (areEqual(d, other.radius)) {
                Point2D antipode(2.0 * other.center.x - center.x, 2.0 * other.center.y - center.y);
                Point2D pInv = invert(antipode);
                double nx = antipode.x - center.x;
                double ny = antipode.y - center.y;
                double c = -(nx * pInv.x + ny * pInv.y);
                return Line2D(nx, ny, c);
            }

            double power = d * d - other.radius * other.radius;
            double scale = (radius * radius) / power;
            Point2D newCenter(
                center.x + scale * (other.center.x - center.x),
                center.y + scale * (other.center.y - center.y)
            );
            double newRadius = std::fabs(scale) * other.radius;
            return Circle2D(newCenter, newRadius);
        }

        double angleWith(const Circle2D& other) const {
            auto pts = intersect(other);
            if (pts.empty()) {
                throw std::logic_error("Circles do not intersect; angle is undefined.");
            }
            double d = center.distanceTo(other.center);
            double cosTheta = std::fabs((radius * radius + other.radius * other.radius - d * d) / (2.0 * radius * other.radius));
            if (cosTheta > 1.0) cosTheta = 1.0;
            return std::acos(cosTheta);
        }

        double angleWith(const Line2D& line) const {
            double dist = line.distanceTo(center);
            if (dist > radius + EPS) {
                throw std::logic_error("Circle and line do not intersect; angle is undefined.");
            }
            double sinTheta = dist / radius;
            if (sinTheta > 1.0) sinTheta = 1.0;
            return std::asin(sinTheta);
        }

        bool operator==(const Circle2D& other) const {
            return center == other.center && areEqual(radius, other.radius);
        }

        bool operator<(const Circle2D& other) const {
            if (!areEqual(radius, other.radius)) return radius < other.radius;
            return center < other.center;
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3)
                << "Circle(center=" << center.toString() << ", r=" << radius << ")";
            return oss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const Circle2D& c) {
            return os << c.toString();
        }
    };

}

#endif