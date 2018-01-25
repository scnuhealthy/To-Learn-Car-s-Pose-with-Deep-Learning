#pragma once
#ifndef QUATERNION_H_
#define QUATERNION_H_

#include <cmath>
#include <iostream>
#include "Vector3D.h"
#include "angle.h"

using namespace std;

/// Quaternion.
/**
 * Unit quaternions provide a convenient mathematical notation for representing
 * orientations and rotations of objects in three dimensions
 */
class Quaternion {
public:
    Quaternion(double w1 = 0.0, double x1 = 0.0,
               double y1 = 0.0, double z1 = 0.0)
        : w(w1), x(x1), y(y1), z(z1) {}

    /**
     * Construct quaternion from a rotation angle(in radian) and rotation axis.
     */
    Quaternion(double rotation_angle, const Vector3D& rotation_axis) {
        double c = cos(0.5 * rotation_angle);
        double s = sin(0.5 * rotation_angle);

        double norm = s / rotation_axis.Length();

        w = c;
        x = rotation_axis.x * norm;
        y = rotation_axis.y * norm;
        z = rotation_axis.z * norm;
    }

	Quaternion(const Vector3D& tmp)
	{
		this->w = 0;
		this->x = tmp.x;
		this->y = tmp.y;
		this->z = tmp.z;
	}

    Quaternion(const Vector3D& v1, const Vector3D& v2) {
            if (v1 == -v2) {
                // 180 degree rotation around any axis (z-axis used here).
                x = 0.0;
                y = 0.0;
                z = 1.0;
                w = 0.0;
                return ;
            }

            Vector3D rotation_axis = CrossProduct(v1, v2);
            double phi = Angle::RadianBetween(v1, v2);
			//cout<<phi<<endl;
			//cout<<cos(phi)<<endl;
            *this = Quaternion(phi, rotation_axis);
        }

    /**
     * Return the inverse of this quaternion.
     */
    const Quaternion Inverse() const {
        return Quaternion(w, -x, -y, -z);
    }

    /**
     * Normalize the quaternion into the unit quaternion.
     */
    void Normalize() {
        double norm = Length();
        if (norm == 0.0) {
            w = 1.0;
            x = y = z = 0.0;
            return;
        }

        norm = 1.0 / norm;
        w *= norm;
        x *= norm;
        y *= norm;
        z *= norm;
    }

    /**
     * Length of quaternion.
     */
    double Length() const {
        return sqrt(w * w + x * x + y * y + z * z);
    }

    /**
     * Convert quaternion to a rotation angle(in degrees) and rotation axis.
     */
    void ToRotation(double* rotation_angle, Vector3D* rotation_axis) const {
        *rotation_angle = 2.0 * acos(w);

        if (*rotation_angle == 0.0) {
            rotation_axis->x = rotation_axis->y = 0.0;
            rotation_axis->z = 1.0;
            return;
        }

        rotation_axis->x = x;
        rotation_axis->y = y;
        rotation_axis->z = z;
        rotation_axis->Normalize();
    }

    /**
     * this *= lhs.
     */
    Quaternion& operator *=(const Quaternion& rhs) {
        return Multiply(*this, rhs);
    }

    /**
     * Set this quaternion to two quaternions' product, and return the product.
     */
    Quaternion& Multiply(const Quaternion& a, const Quaternion& b) {
        double w1 = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
        double x1 = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
        double y1 = a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z;
        double z1 = a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x;
        w = w1;
        x = x1;
        y = y1;
        z = z1;
        return *this;
    }

    /**
     * return c = lhs * rhs.
     */
    friend const Quaternion operator *(const Quaternion& lhs,
                                       const Quaternion& rhs) {
        Quaternion c;
        return c.Multiply(lhs, rhs);
    }

	friend Vector3D Rotate(Vector3D& point, const Quaternion& rotation) {
		Quaternion q(0.0, point.x, point.y, point.z);
		q = rotation * q * rotation.Inverse();
		Vector3D t;
		t.x = q.x;
		t.y = q.y;
		t.z = q.z;
		return t;
	}

    double w, x, y, z;
};

#endif
