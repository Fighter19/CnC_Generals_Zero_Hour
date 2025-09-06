/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* $Header: /Commando/Code/wwmath/vector2.h 24    7/06/01 9:43a Byon_g $ */
/*********************************************************************************************** 
 ***                  Confidential - Westwood Studios                                        *** 
 *********************************************************************************************** 
 *                                                                                             * 
 *                 Project Name : Westwood 3D                                                  * 
 *                                                                                             * 
 *                    File Name : VECTOR2.H                                                    * 
 *                                                                                             * 
 *                   Programmer : Greg Hjelstrom                                               * 
 *                                                                                             * 
 *                   Start Date : 02/24/97                                                     *  
 *                                                                                             * 
 *                  Last Update : February 24, 1997 [GH]                                       * 
 *                                                                                             * 
 *---------------------------------------------------------------------------------------------* 
 * Functions:                                                                                  * 
 *   Scalar Division Operator -- Divide a vector by a scalar                                   * 
 *   Scalar Multiply Operator -- Multiply a vector by a scalar                                 * 
 *   Vector Addition Operator -- Add two vectors                                               * 
 *   Vector Subtraction Operator -- Subract two vectors                                        * 
 *   Vector Inner Product Operator -- Compute the inner or dot product                         * 
 *   Vector Equality Operator -- Detemine if two vectors are identical                         * 
 *   Equal_Within_Epsilon -- Determine if two vectors are identical within                     * 
 *   Vector Inequality Operator -- Detemine if two vectors are identical                       * 
 *   Swap -- swap two Vector2's                                                                * 
 *   Vector2::Is_Valid -- Verifies that all components are valid CustomFloats                        *
 *	  Vector2::Update_Min -- sets each component of the vector to the min of this and a.        *
 *	  Vector2::Update_Max -- sets each component of the vector to the max of this and a.        *
 *   Vector2::Scale -- multiply components of a vector by independant scaling factors.			  *
 *   Vector2::Lerp -- linearly interpolates two Vector2's                                      *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef VECTOR2_H
#define VECTOR2_H

#include "always.h"
#include "wwmath.h"
#include <math.h>


/*
** 2-Dimensional Vectors
*/

class Vector2
{

public:

	union {
		CustomFloat	X;
		CustomFloat U;
	};

	union {
		CustomFloat	Y;
		CustomFloat V;
	};

	// Constructors
	WWINLINE Vector2(void) {};
	WWINLINE Vector2(const Vector2 & v) { X = v.X; Y = v.Y; }
	WWINLINE Vector2(CustomFloat x, CustomFloat y) { X = x; Y = y; }
	WWINLINE Vector2(const CustomFloat vector[2]) { X = vector[0]; Y = vector[1]; }
	
	
	// Assignment
	WWINLINE Vector2 & operator = (const Vector2 & v) { X = v[0]; Y = v[1]; return *this; }	
	WWINLINE void	Set(CustomFloat x, CustomFloat y) { X = x; Y = y; }
	WWINLINE void	Set(const Vector2 & v) { X = v.X; Y = v.Y; }

	// Array access
	WWINLINE CustomFloat &	operator [](int i) { return (&X)[i]; }     
	WWINLINE const CustomFloat &  operator [](int i) const { return (&X)[i]; }  

	// normalize, compute length
	WWINLINE void	Normalize(void);
	WWINLINE CustomFloat Length(void) const;
	WWINLINE CustomFloat Length2(void) const;

	// unary operators
	WWINLINE Vector2 operator-() const { return Vector2(-X,-Y); } 
	WWINLINE Vector2 operator+() const { return *this; } 

	WWINLINE Vector2 & operator += (const Vector2 & v) { X += v.X; Y += v.Y; return *this; }	
	WWINLINE Vector2 & operator -= (const Vector2 & v) { X -= v.X; Y -= v.Y; return *this; }		
	WWINLINE Vector2 & operator *= (CustomFloat k) { X = (CustomFloat)(X*k); Y=(CustomFloat)(Y*k); return *this; }
	WWINLINE Vector2 & operator /= (CustomFloat k) { k=(CustomFloat)1.0f/k; X*=k; Y*=k; return *this; }

	// scalar multiplication, division
	WWINLINE friend Vector2 operator * (const Vector2 &a,CustomFloat k);
	WWINLINE friend Vector2 operator * (CustomFloat k,const Vector2 &a);
 	WWINLINE friend Vector2 operator / (const Vector2 &a,CustomFloat k);

	// vector addition,subtraction
	WWINLINE friend Vector2 operator + (const Vector2 &a,const Vector2 &b);
	WWINLINE friend Vector2 operator - (const Vector2 &a,const Vector2 &b);

	// dot product / inner product
	WWINLINE friend CustomFloat operator * (const Vector2 &a,const Vector2 &b);
	static WWINLINE CustomFloat Dot_Product(const Vector2 &a,const Vector2 &b);

	// dot product between a and perpendicular vector to b
	static CustomFloat Perp_Dot_Product(const Vector2 &a,const Vector2 &b);
	
	// Equality operators
	friend bool operator == (const Vector2 &a,const Vector2 &b);
	friend bool operator != (const Vector2 &a,const Vector2 &b);
   WWINLINE friend bool Equal_Within_Epsilon(const Vector2 &a,const Vector2 &b,CustomFloat epsilon);

	// Rotation
	WWINLINE void Rotate(CustomFloat theta);
	WWINLINE void Rotate(CustomFloat s, CustomFloat c);
	WWINLINE bool Rotate_Towards_Vector(Vector2 &target, CustomFloat max_theta, bool & positive_turn);
	WWINLINE bool Rotate_Towards_Vector(Vector2 &target, CustomFloat max_s, CustomFloat max_c, bool & positive_turn);

	// verify that none of the members of this vector are invalid CustomFloats
	WWINLINE bool Is_Valid(void) const;

	// make this vector the min or max of itself and the passed vector
	WWINLINE void Update_Min (const Vector2 & a);
	WWINLINE void Update_Max (const Vector2 & a);
	WWINLINE void Scale (CustomFloat a, CustomFloat b);
	WWINLINE void Scale (const Vector2 & a);

	static WWINLINE CustomFloat Distance(const Vector2 &p1, const Vector2 &p2);
	static WWINLINE CustomFloat Quick_Distance(const Vector2 &p1, const Vector2 &p2);

	// interpolate between two Vector2's
	static void Lerp(const Vector2 & a,const Vector2 & b,CustomFloat t,Vector2 * set_result);
};


/************************************************************************** 
 * Scalar Multiply Operator -- Multiply a vector by a scalar              * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *   02/24/1997 GH  : Created.                                            * 
 *========================================================================*/
WWINLINE Vector2 operator * (const Vector2 &a,CustomFloat k)
{
	CustomFloat a0k(a[0] * k);
	CustomFloat a1k(a[1] * k);
	return Vector2(a0k,a1k);
}

WWINLINE Vector2 operator * (CustomFloat k, const Vector2 &a)
{
	CustomFloat a0k(a[0] * k);
	CustomFloat a1k(a[1] * k);
	return Vector2(a0k,a1k);
}

/************************************************************************** 
 * Scalar Division Operator -- Divide a vector by a scalar                * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE Vector2 operator / (const Vector2 &a,CustomFloat k)
{
	CustomFloat ook=(CustomFloat)1.0f/k;
	CustomFloat a0ook(a[0] * ook);
	CustomFloat a1ook(a[1] * ook);
	return Vector2(a0ook,a1ook);
}

/************************************************************************** 
 * Vector Addition Operator -- Add two vectors                            * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *   02/24/1997 GH  : Created.                                            * 
 *========================================================================*/
WWINLINE Vector2 operator + (const Vector2 &a,const Vector2 &b)
{
	return Vector2(
							a.X + b.X,
							a.Y + b.Y
						);
}

/************************************************************************** 
 * Vector Subtraction Operator -- Subract two vectors                     * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *   02/24/1997 GH  : Created.                                            * 
 *========================================================================*/
WWINLINE Vector2 operator - (const Vector2 &a,const Vector2 &b)
{
	return Vector2(
							a.X - b.X,
							a.Y - b.Y
						);
}

/************************************************************************** 
 * Vector Inner Product -- Compute the inner or dot product of two vector * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE CustomFloat operator * (const Vector2 &a,const Vector2 &b)
{
	return	a.X*b.X + a.Y*b.Y;
}

WWINLINE CustomFloat Vector2::Dot_Product(const Vector2 &a,const Vector2 &b)
{
	return a*b;
}

WWINLINE CustomFloat Vector2::Perp_Dot_Product(const Vector2 &a,const Vector2 &b)
{
	return a.X * -b.Y + a.Y * b.X;
}

/************************************************************************** 
 * Vector Equality Operator -- Detemine if two vectors are identical      * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool operator == (const Vector2 &a,const Vector2 &b)
{
	bool a0b0(a[0] == b[0]);
	bool a1b1(a[1] == b[1]);
	return ( a0b0 & a1b1);
}

/************************************************************************** 
 * Vector Inequality Operator -- Detemine if two vectors are identical    * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool operator != (const Vector2 &a,const Vector2 &b)
{
	bool a0b0(a[0] != b[0]);
	bool a1b1(a[1] != b[1]);
	return ( a0b0 | a1b1);
}

/************************************************************************** 
 * Equal_Within_Epsilon -- Determine if two vectors are identical within e* 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool Equal_Within_Epsilon(const Vector2 &a,const Vector2 &b,CustomFloat epsilon)
{
   return( (WWMath::Fabs(a.X - b.X) < epsilon) && (WWMath::Fabs(a.Y - b.Y) < epsilon) );
}

/************************************************************************** 
 * Vector2::Normalize -- Normalizes the vector.                           * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE void Vector2::Normalize()
{
	CustomFloat len2 = Length2();
	if (len2 != 0.0f) {
		CustomFloat oolen = WWMath::Inv_Sqrt(len2);
		X *= oolen;
		Y *= oolen;
	}
}

WWINLINE Vector2 Normalize(const Vector2 & vec)
{
	CustomFloat len2 = vec.Length2();
	if (len2 != 0.0f) {
		CustomFloat oolen = WWMath::Inv_Sqrt(len2);
		return vec / oolen;
	}
	return Vector2(0.0f,0.0f);
}

/************************************************************************** 
 * Vector2::Length -- Returns the length of the vector                    * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE CustomFloat Vector2::Length() const
{
	return (CustomFloat)WWMath::Sqrt(Length2());
}

/************************************************************************** 
 * Vector2::Length -- Returns the square of the length of the vector      * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE CustomFloat Vector2::Length2() const
{
	return (X*X + Y*Y);
}

/************************************************************************** 
 * Vector2::Rotate -- Rotate vector                                       * 
 *                                                                        * 
 * INPUT:                                                                 *
 * CustomFloat theta - angle to rotate                                          * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE void Vector2::Rotate(CustomFloat theta)
{
	Rotate(WWMath::Sin(theta), WWMath::Cos(theta));
}

/************************************************************************** 
 * Vector2::Rotate -- Rotate vector                                       * 
 *                                                                        * 
 * INPUT:                                                                 * 
 * s - sine of the angle                                                  * 
 * c - cosine of the angle                                                * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE void Vector2::Rotate(CustomFloat s, CustomFloat c)
{
	CustomFloat new_x = X * c + Y * -s;
	CustomFloat new_y = X * s + Y * c;
	X = new_x;
	Y = new_y;
}

/************************************************************************** 
 * Vector2::Rotate -- Rotate towards given vector (stop on reaching it)   * 
 *                                                                        * 
 * INPUT:                                                                 *
 * CustomFloat theta - angle to rotate                                          * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 * bool - true if we have reached the desired vector                      * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 * This function assumes both vectors are normalized!                     * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool Vector2::Rotate_Towards_Vector(Vector2 &target, CustomFloat max_theta, bool & positive_turn)
{
	return Rotate_Towards_Vector(target, WWMath::Sin(max_theta), WWMath::Cos(max_theta), positive_turn);
}

/************************************************************************** 
 * Vector2::Rotate -- Rotate towards given vector (stop on reaching it)   * 
 *                                                                        * 
 * INPUT:                                                                 *
 * s - sine of the angle                                                  * 
 * c - cosine of the angle                                                * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 * bool - true if we have reached the desired vector                      * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 * This function assumes both vectors are normalized!                     * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool Vector2::Rotate_Towards_Vector(Vector2 &target, CustomFloat max_s, CustomFloat max_c, bool & positive_turn)
{
	bool return_value = false;
	
	positive_turn = Vector2::Perp_Dot_Product(target, *this) > 0.0f;

	if (Vector2::Dot_Product(*this, target) >= max_c) {
		Set(target);
		return_value = true;
	} else {
		// Determine turn direction and rotate accordingly.
		if (positive_turn) {
			Rotate(max_s, max_c);
		} else {
			Rotate(-max_s, max_c);
		}
	}

	return return_value;
}

/*********************************************************************************************** 
 * Swap -- swap two Vector2's                                                                  * 
 *                                                                                             * 
 * INPUT:                                                                                      * 
 *                                                                                             * 
 * OUTPUT:                                                                                     * 
 *                                                                                             * 
 * WARNINGS:                                                                                   * 
 *                                                                                             * 
 * HISTORY:                                                                                    * 
 *   08/11/1997 GH  : Created.                                                                 * 
 *=============================================================================================*/
WWINLINE void Swap(Vector2 & a,Vector2 & b)
{
	Vector2 tmp(a);
	a = b;
	b = tmp;
}


/***********************************************************************************************
 * Vector2::Is_Valid -- Verifies that all components are valid CustomFloats                          *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/18/99   gth : Created.                                                                 *
 *=============================================================================================*/
WWINLINE bool Vector2::Is_Valid(void) const
{
	return (WWMath::Is_Valid_Float(X) && WWMath::Is_Valid_Float(Y));
}


/***********************************************************************************************
 * Vector2::Update_Min -- Set each component of the vector to the min of this and a.			  *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/12/00   IML : Created.                                                                 *
 *=============================================================================================*/
WWINLINE void Vector2::Update_Min (const Vector2 & a)
{	
	if (a.X < X) X = a.X;
	if (a.Y < Y) Y = a.Y;
}


/***********************************************************************************************
 * Vector2::Update_Max -- Set each component of the vector to the max of this and a.			  *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/12/00   IML : Created.                                                                 *
 *=============================================================================================*/
WWINLINE void Vector2::Update_Max (const Vector2 & a)
{	
	if (a.X > X) X = a.X;
	if (a.Y > Y) Y = a.Y;
}


/***********************************************************************************************
 * Vector2::Scale -- multiply components of a vector by independant scaling factors.			  *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/19/2000  IML : Created.                                                                 *
 *=============================================================================================*/
WWINLINE void Vector2::Scale (CustomFloat a, CustomFloat b)
{
	X *= a;
	Y *= b;
}


/***********************************************************************************************
 * Vector2::Scale -- multiply components of a vector by independant scaling factors.			  *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/19/2000  IML : Created.                                                                 *
 *=============================================================================================*/
WWINLINE void Vector2::Scale (const Vector2 & a)
{
	X *= a.X;
	Y *= a.Y;
}


/***********************************************************************************************
 * Quick_Distance -- Fast but inaccurate 2D distance calculation.                              *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/1999MLL: Created.                                                                   *
 *=============================================================================================*/
WWINLINE CustomFloat Quick_Distance(CustomFloat x1, CustomFloat y1, CustomFloat x2, CustomFloat y2)
{
	// From Graphic Gems I.
	CustomFloat x_diff = x1 - x2;
	CustomFloat y_diff = y1 - y2;

	WWMath::Fabs(x_diff);
	WWMath::Fabs(y_diff);

	if (x_diff > y_diff)
	{
		return ((y_diff / 2) + x_diff);
	} 
	else
	{
		return ((x_diff / 2) + y_diff);
	}
}

WWINLINE CustomFloat Vector2::Quick_Distance(const Vector2 &a, const Vector2 &b)
{
	return ::Quick_Distance(a.X, a.Y, b.X, b.Y);	
}


/***********************************************************************************************
 * Distance -- Accurate distance 2D calculation.                                               *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/1999MLL: Created.                                                                   *
 *=============================================================================================*/
WWINLINE CustomFloat Vector2::Distance(const Vector2 &a, const Vector2 &b)
{
	Vector2 temp;
	temp = a - b;

	return (temp.Length());
}

WWINLINE CustomFloat Distance(CustomFloat x1, CustomFloat y1, CustomFloat x2, CustomFloat y2)
{
	CustomFloat x_diff = x1 - x2;
	CustomFloat y_diff = y1 - y2;
	
	return (WWMath::Sqrt((x_diff * x_diff) + (y_diff * y_diff)));
}


/***********************************************************************************************
 * Vector2::Lerp -- linearly interpolates two Vector2's                                        *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   4/14/2000  gth : Created.                                                                 *
 *=============================================================================================*/
WWINLINE void Vector2::Lerp(const Vector2 & a,const Vector2 & b,CustomFloat t,Vector2 * set_result)
{
	assert(set_result != NULL);
	set_result->X = (a.X + (b.X - a.X)*t);
   set_result->Y = (a.Y + (b.Y - a.Y)*t);
}


#endif /* VECTOR2_H */

