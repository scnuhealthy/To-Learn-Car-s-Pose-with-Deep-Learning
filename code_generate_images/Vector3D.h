#ifndef PCL_MATH_VECTOR3D_H_
#define PCL_MATH_VECTOR3D_H_

#include <cmath>
#include <iostream>
using namespace std;

/// 3D Vector.
struct Vector3D {

public:
    Vector3D(double x1 = 0.0, double y1 = 0.0, double z1 = 0.0)
        : x(x1), y(y1), z(z1) {}


	void Reverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}

    bool operator ==(const Vector3D& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator !=(const Vector3D& rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }

    const Vector3D& operator +=(const Vector3D& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    const Vector3D& operator -=(const Vector3D& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    const Vector3D& operator *=(double value) {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    friend const Vector3D operator +(const Vector3D& lhs, const Vector3D& rhs) {
        return Vector3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    friend const Vector3D operator -(const Vector3D& lhs, const Vector3D& rhs) {
        return Vector3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    friend const Vector3D operator -(const Vector3D& rhs) {
        return Vector3D(-rhs.x, -rhs.y, -rhs.z);
    }

    friend const Vector3D operator *(double lhs, const Vector3D& rhs) {
        return Vector3D(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
    }

    friend const Vector3D operator *(const Vector3D& lhs, double rhs) {
        return Vector3D(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    }

    /**
     * Return the length of Vector2D.
     */
    double Length() const {
        return sqrt(x * x + y * y + z * z);
    }

    /**
     * Normalize the vector into the unit vector.
     */
    void Normalize() {
        double norm = Length();
        if (norm == 0.0) {
            x = y = z = 0.0;
            return;
        }

        norm = 1.0 / norm;
        x *= norm;
        y *= norm;
        z *= norm;
    }

    /**
     * Return the cross product of two vertors.
     */
    friend const Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2) {
        double x = v1.y * v2.z - v1.z * v2.y;
        double y = v1.z * v2.x - v1.x * v2.z;
        double z = v1.x * v2.y - v1.y * v2.x;
        return Vector3D(x, y, z);
    }

    /**
     * Return the dot product of two vertors.
     */
    friend double DotProduct(const Vector3D& v1, const Vector3D& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

	friend ostream &operator<<(ostream &os,const Vector3D &c)
	{
		os<<c.x<<" "<<c.y<<" "<<c.z<<endl;
		return os;
	}

    double x; /// X component of vector.
    double y; /// Y component of vector.
    double z; /// Z component of vector.
};

#endif
