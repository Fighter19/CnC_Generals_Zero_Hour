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

/* $Header: /Commando/Code/wwmath/vector3.h 40    5/11/01 7:11p Jani_p $ */
/*********************************************************************************************** 
 ***                  Confidential - Westwood Studios                                        *** 
 *********************************************************************************************** 
 *                                                                                             * 
 *                 Project Name : Westwood 3D                                                  * 
 *                                                                                             * 
 *                    File Name : VECTOR3.H                                                    *  
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
 *   Vector Equality Operator -- Determine if two vectors are identical                        *
 *   Vector Inequality Operator -- Determine if two vectors are identical                      *
 *   Equal_Within_Epsilon -- Determine if two vectors are identical within                     *
 *   Cross_Product -- compute the cross product of two vectors                                 *
 *   Vector3::Normalize -- Normalizes the vector.                                              *
 *   Vector3::Length -- Returns the length of the vector                                       *
 *   Vector3::Length2 -- Returns the square of the length of the vector                        *
 *   Vector3::Quick_Length -- returns a quick approximation of the length                      *
 *   Swap -- swap two Vector3's                                                                *
 *   Lerp -- linearly interpolate two Vector3's by an interpolation factor.                    *
 *   Lerp -- linearly interpolate two Vector3's without return-by-value                        *
 *   Vector3::Add -- Add two vector3's without return-by-value                                 *
 *   Vector3::Subtract -- Subtract two vector3's without return-by-value                       *
 *   Vector3::Update_Min -- sets each component of the vector to the min of this and a         *
 *   Vector3::Update_Max -- Sets each component of the vector to the max of this and a         *
 *   Vector3::Scale -- scale this vector by 3 independent scale factors                        *
 *   Vector3::Rotate_X -- rotates this vector around the X axis                                *
 *   Vector3::Rotate_X -- Rotates this vector around the x axis                                *
 *   Vector3::Rotate_Y -- Rotates this vector around the y axis                                *
 *   Vector3::Rotate_Y -- Rotates this vector around the Y axis                                *
 *   Vector3::Rotate_Z -- Rotates this vector around the Z axis                                *
 *   Vector3::Rotate_Z -- Rotates this vector around the Z axis                                *
 *   Vector3::Is_Valid -- Verifies that each component of this vector is a valid CustomFloat         *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef VECTOR3_H
#define VECTOR3_H

#include "always.h"
#include "wwmath.h"
#include <assert.h>
#ifdef _UNIX
#include "osdep.h"
#endif

#include "CustomFloat.h"


/*
** Vector3 - 3-Dimensional Vectors
*/
class Vector3
{

public:


	CustomFloat	X;
	CustomFloat Y;
	CustomFloat Z;


	// Constructors
	WWINLINE Vector3(void) {};
	WWINLINE Vector3(const Vector3 & v) { X = v.X; Y = v.Y; Z = v.Z; }
	WWINLINE Vector3(CustomFloat x, CustomFloat y, CustomFloat z) { X = x; Y = y; Z = z; }
	WWINLINE Vector3(const CustomFloat vector[3]) { X = vector[0]; Y = vector[1]; Z = vector[2]; }
	
	// Assignment
	WWINLINE Vector3 & operator = (const Vector3 & v) { X = v.X; Y = v.Y; Z = v.Z; return *this; }	
	WWINLINE void	Set(CustomFloat x, CustomFloat y, CustomFloat z) { X = x; Y = y; Z = z; }
	WWINLINE void	Set(const Vector3 & that) { X = that.X; Y = that.Y; Z = that.Z; }

	// Array access
	WWINLINE CustomFloat &	operator [](int i) { return (&X)[i]; }     
	WWINLINE const CustomFloat &  operator [](int i) const { return (&X)[i]; }  

	// normalize, compute length
	void	Normalize(void);
	WWINLINE CustomFloat	Length(void) const;
	WWINLINE CustomFloat	Length2(void) const;
	CustomFloat Quick_Length(void) const;
	void  Scale(const Vector3 & scale);

	// rotation, (warning, modifies this vector!)
	WWINLINE void	Rotate_X(CustomFloat angle);
	WWINLINE void	Rotate_X(CustomFloat s_angle,CustomFloat c_angle);
	WWINLINE void	Rotate_Y(CustomFloat angle);
	WWINLINE void	Rotate_Y(CustomFloat s_angle,CustomFloat c_angle);
	WWINLINE void	Rotate_Z(CustomFloat angle);
	WWINLINE void	Rotate_Z(CustomFloat s_angle,CustomFloat c_angle);

	// unary operators
	WWINLINE Vector3 operator-() const { return(Vector3(-X,-Y,-Z)); } 
	WWINLINE Vector3 operator+() const { return *this; } 

	WWINLINE Vector3 & operator += (const Vector3 & v) { X += v.X; Y += v.Y; Z += v.Z; return *this; }	
	WWINLINE Vector3 & operator -= (const Vector3 & v) { X -= v.X; Y -= v.Y; Z -= v.Z; return *this; }		
	WWINLINE Vector3 & operator *= (CustomFloat k) { X = X*k; Y=Y*k; Z=Z*k; return *this; }
	WWINLINE Vector3 & operator /= (CustomFloat k) { CustomFloat ook=CustomFloat(1.0f)/k; X=X*ook; Y=Y*ook; Z=Z*ook; return *this; }

	// scalar multiplication, division
	WWINLINE friend Vector3 operator * (const Vector3 &a,CustomFloat k);
	WWINLINE friend Vector3 operator * (CustomFloat k,const Vector3 &a);
	WWINLINE friend Vector3 operator / (const Vector3 &a,CustomFloat k);

	// vector addition,subtraction
	WWINLINE friend Vector3 operator + (const Vector3 &a,const Vector3 &b);
	WWINLINE friend Vector3 operator - (const Vector3 &a,const Vector3 &b);

	// Equality operators
	friend bool operator == (const Vector3 &a,const Vector3 &b);
	friend bool operator != (const Vector3 &a,const Vector3 &b);
   WWINLINE friend bool Equal_Within_Epsilon(const Vector3 &a,const Vector3 &b,CustomFloat epsilon);

	// dot product / inner product
	//WWINLINE friend CustomFloat operator * (const Vector3 &a,const Vector3 &b);
	static WWINLINE CustomFloat Dot_Product(const Vector3 &a,const Vector3 &b);
	
	// cross product / outer product
#ifdef ALLOW_TEMPORARIES
	static WWINLINE Vector3 Cross_Product(const Vector3 &a,const Vector3 &b);
#endif
	static WWINLINE void Cross_Product(const Vector3 &a,const Vector3 &b,Vector3 * result);
	static WWINLINE void Normalized_Cross_Product(const Vector3 &a, const Vector3 &b, Vector3 * result);
	static WWINLINE CustomFloat Cross_Product_X(const Vector3 &a,const Vector3 &b);
	static WWINLINE CustomFloat Cross_Product_Y(const Vector3 &a,const Vector3 &b);
	static WWINLINE CustomFloat Cross_Product_Z(const Vector3 &a,const Vector3 &b);

	// add and subtract without return by value
	static WWINLINE void Add(const Vector3 & a,const Vector3 & b,Vector3 * c);
	static WWINLINE void Subtract(const Vector3 & a,const Vector3 & b,Vector3 * c);
	
	// Line intersection functions.
	static WWINLINE CustomFloat Find_X_At_Y(CustomFloat y, const Vector3 &p1, const Vector3 &p2);
	static WWINLINE CustomFloat Find_X_At_Z(CustomFloat z, const Vector3 &p1, const Vector3 &p2);
	static WWINLINE CustomFloat Find_Y_At_X(CustomFloat x, const Vector3 &p1, const Vector3 &p2);
	static WWINLINE CustomFloat Find_Y_At_Z(CustomFloat z, const Vector3 &p1, const Vector3 &p2);
	static WWINLINE CustomFloat Find_Z_At_X(CustomFloat x, const Vector3 &p1, const Vector3 &p2);
	static WWINLINE CustomFloat Find_Z_At_Y(CustomFloat z, const Vector3 &p1, const Vector3 &p2);

	// make this vector the min or max of itself and the passed vector
	WWINLINE void Update_Min(const Vector3 & a);
	WWINLINE void Update_Max(const Vector3 & a);
	WWINLINE void Cap_Absolute_To(const Vector3 & a);

	// verify that none of the members of this vector are invalid CustomFloats
	WWINLINE bool Is_Valid(void) const;

	static WWINLINE CustomFloat Quick_Distance(const Vector3 &p1, const Vector3 &p2);
	static WWINLINE CustomFloat Distance(const Vector3 &p1, const Vector3 &p2);

	// Linearly interpolate two Vector3's
	static void Lerp(const Vector3 & a, const Vector3 & b, CustomFloat alpha,Vector3 * set_result);

	// Color Conversion
	WWINLINE unsigned	long	Convert_To_ABGR( void ) const;
	WWINLINE unsigned	long	Convert_To_ARGB( void ) const;
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
WWINLINE Vector3 operator * (const Vector3 &a,CustomFloat k)
{
	return Vector3((a.X * k),(a.Y * k),(a.Z * k));
}

WWINLINE Vector3 operator * (CustomFloat k, const Vector3 &a)
{
	return Vector3((a.X * k),(a.Y * k),(a.Z * k));
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
WWINLINE Vector3 operator / (const Vector3 &a,CustomFloat k)
{
	CustomFloat ook = (CustomFloat)1.0f/k;
	return Vector3((a.X * ook),(a.Y * ook),(a.Z * ook));
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
WWINLINE Vector3 operator + (const Vector3 &a,const Vector3 &b)
{
	return Vector3(
							a.X+b.X,
							a.Y+b.Y,
							a.Z+b.Z
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
WWINLINE Vector3 operator - (const Vector3 &a,const Vector3 &b)
{
	return Vector3(
							a.X-b.X,
							a.Y-b.Y,
							a.Z-b.Z
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
//WWINLINE CustomFloat operator * (const Vector3 &a,const Vector3 &b)
//{
//	return	a.X*b.X + 
//				a.Y*b.Y + 
//				a.Z*b.Z;
//}

WWINLINE CustomFloat Vector3::Dot_Product(const Vector3 &a,const Vector3 &b)
{
	return a.X*b.X + 
				a.Y*b.Y + 
				a.Z*b.Z;
}


/************************************************************************** 
 * Vector Equality Operator -- Determine if two vectors are identical     * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool operator == (const Vector3 &a,const Vector3 &b)
{
	return ( (a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z));
}

/************************************************************************** 
 * Vector Inequality Operator -- Determine if two vectors are identical   * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE bool operator != (const Vector3 &a,const Vector3 &b)
{
	return ( (a.X != b.X) || (a.Y != b.Y) || (a.Z != b.Z));
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
WWINLINE bool Equal_Within_Epsilon(const Vector3 &a,const Vector3 &b,CustomFloat epsilon)
{
   return(	(WWMath::Fabs(a.X - b.X) < epsilon) && 
				(WWMath::Fabs(a.Y - b.Y) < epsilon) && 
				(WWMath::Fabs(a.Z - b.Z) < epsilon)	);
}


/************************************************************************** 
 * Cross_Product -- compute the cross product of two vectors              * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
#ifdef ALLOW_TEMPORARIES
WWINLINE Vector3 Vector3::Cross_Product(const Vector3 &a,const Vector3 &b)
{
	return Vector3(
		(a.Y * b.Z - a.Z * b.Y),
		(a.Z * b.X - a.X * b.Z),
		(a.X * b.Y - a.Y * b.X)
	);
}
#endif

WWINLINE void Vector3::Cross_Product(const Vector3 &a,const Vector3 &b,Vector3 * set_result)
{
	assert(set_result != &a);
	set_result->X = (a.Y * b.Z - a.Z * b.Y);
	set_result->Y = (a.Z * b.X - a.X * b.Z);
	set_result->Z = (a.X * b.Y - a.Y * b.X);
}

WWINLINE void Vector3::Normalized_Cross_Product(const Vector3 &a,const Vector3 &b,Vector3 * set_result)
{
	assert(set_result != &a);
	set_result->X = (a.Y * b.Z - a.Z * b.Y);
	set_result->Y = (a.Z * b.X - a.X * b.Z);
	set_result->Z = (a.X * b.Y - a.Y * b.X);
	set_result->Normalize();
}

WWINLINE CustomFloat Vector3::Cross_Product_X(const Vector3 &a,const Vector3 &b)
{
   return a.Y * b.Z - a.Z * b.Y;
}

WWINLINE CustomFloat Vector3::Cross_Product_Y(const Vector3 &a,const Vector3 &b)
{
   return a.Z * b.X - a.X * b.Z;
}

WWINLINE CustomFloat Vector3::Cross_Product_Z(const Vector3 &a,const Vector3 &b)
{
   return a.X * b.Y - a.Y * b.X;
}

/************************************************************************** 
 * Vector3::Normalize -- Normalizes the vector.                           * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE void Vector3::Normalize()
{
	CustomFloat len2 = Length2();
	if (len2 != 0.0f) 
	{
		CustomFloat oolen = WWMath::Inv_Sqrt(len2);
		X *= oolen;
		Y *= oolen;
		Z *= oolen;
	}
}

#ifdef ALLOW_TEMPORARIES
WWINLINE Vector3 Normalize(const Vector3 & vec)
{
	CustomFloat len2 = vec.Length2();
	if (len2 != 0.0f) 
	{
		CustomFloat oolen = WWMath::Inv_Sqrt(len2);
		return vec * oolen;
	}
	return vec;
}
#endif

/************************************************************************** 
 * Vector3::Length -- Returns the length of the vector                    * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE CustomFloat Vector3::Length() const
{
	return WWMath::Sqrt(Length2());
}

/************************************************************************** 
 * Vector3::Length2 -- Returns the square of the length of the vector     * 
 *                                                                        * 
 * INPUT:                                                                 * 
 *                                                                        * 
 * OUTPUT:                                                                * 
 *                                                                        * 
 * WARNINGS:                                                              * 
 *                                                                        * 
 * HISTORY:                                                               * 
 *========================================================================*/
WWINLINE CustomFloat Vector3::Length2() const
{
	return X*X + Y*Y + Z*Z;
}


/***********************************************************************************************
 * Vector3::Quick_Length -- returns a quick approximation of the length                        *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   7/15/98    GTH : Created.                                                                 *
 *=============================================================================================*/
WWINLINE CustomFloat Vector3::Quick_Length(void) const
{
	// this method of approximating the length comes from Graphics Gems 1 and
	// supposedly gives an error of +/- 8%
	CustomFloat max = WWMath::Fabs(X);
	CustomFloat mid = WWMath::Fabs(Y);
	CustomFloat min = WWMath::Fabs(Z);
	CustomFloat tmp;

	if (max < mid) { tmp = max; max = mid; mid = tmp; }
	if (max < min) { tmp = max; max = min; min = tmp; }
	if (mid < min) { tmp = mid; mid = min; min = mid; }

	return max + ((CustomFloat)11.0f / (CustomFloat)32.0f)*mid + ((CustomFloat)1.0f / (CustomFloat)4.0f)*min;
}


/*********************************************************************************************** 
 * Swap -- swap two Vector3's                                                                  * 
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
WWINLINE void Swap(Vector3 & a,Vector3 & b)
{
	Vector3 tmp(a);
	a = b;
	b = tmp;
}

/***********************************************************************************************
 * Lerp -- linearly interpolate two Vector3's without return-by-value                          *
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
WWINLINE void Vector3::Lerp(const Vector3 & a, const Vector3 & b, CustomFloat alpha,Vector3 * set_result)
{
	assert(set_result != NULL);
	set_result->X = (a.X + (b.X - a.X)*alpha);
   set_result->Y = (a.Y + (b.Y - a.Y)*alpha);
   set_result->Z = (a.Z + (b.Z - a.Z)*alpha);
}

/***********************************************************************************************
 * Vector3::Add -- Add two vector3's without return-by-value                                   *
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
WWINLINE void Vector3::Add(const Vector3 &a,const Vector3 &b,Vector3 * set_result)
{
	assert(set_result != NULL);
	set_result->X = a.X + b.X;
	set_result->Y = a.Y + b.Y;
	set_result->Z = a.Z + b.Z;
}


/***********************************************************************************************
 * Vector3::Subtract -- Subtract two vector3's without return-by-value                         *
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
WWINLINE void Vector3::Subtract(const Vector3 &a,const Vector3 &b,Vector3 * set_result)
{
	assert(set_result != NULL);
	set_result->X = a.X - b.X;
	set_result->Y = a.Y - b.Y;
	set_result->Z = a.Z - b.Z;
}


/***********************************************************************************************
 * Vector3::Update_Min -- sets each component of the vector to the min of this and a           *
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
WWINLINE void Vector3::Update_Min(const Vector3 & a)
{	
	if (a.X < X) X = a.X;
	if (a.Y < Y) Y = a.Y;
	if (a.Z < Z) Z = a.Z;
}


/***********************************************************************************************
 * Vector3::Update_Max -- Sets each component of the vector to the max of this and a           *
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
WWINLINE void Vector3::Update_Max(const Vector3 & a)
{	
	if (a.X > X) X = a.X;
	if (a.Y > Y) Y = a.Y;
	if (a.Z > Z) Z = a.Z;
}

/***********************************************************************************************
 * Vector3::Cap_To_Absolute_Of -- Sets each component of the vector to no larger than the -ve or +ve of*
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/99   wst : Created.                                                                 *
 *=============================================================================================*/
WWINLINE void Vector3::Cap_Absolute_To(const Vector3 & a)
{	
	if (X > 0)
	{
		if (a.X < X) X = a.X;
	}
	else
	{
		if (-a.X > X) X = -a.X;
	}

	if (Y > 0)
	{
		if (a.Y < Y) Y = a.Y;
	}
	else
	{
		if (-a.Y > Y) Y = -a.Y;
	}

	if (Z > 0)
	{
		if (a.Z < Z) Z = a.Z;
	}
	else
	{
		if (-a.Z > Z) Z = -a.Z;
	}

}

/***********************************************************************************************
 * Vector3::Scale -- scale this vector by 3 independent scale factors                          *
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
WWINLINE void Vector3::Scale(const Vector3 & scale)
{
	X *= scale.X;
	Y *= scale.Y;
	Z *= scale.Z;
}


/***********************************************************************************************
 * Vector3::Rotate_X -- rotates this vector around the X axis                                  *
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
WWINLINE void Vector3::Rotate_X(CustomFloat angle)
{
	Rotate_X(sin(angle),cos(angle));
}


/***********************************************************************************************
 * Vector3::Rotate_X -- Rotates this vector around the x axis                                  *
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
WWINLINE void Vector3::Rotate_X(CustomFloat s_angle,CustomFloat c_angle)
{
	CustomFloat tmp_y = Y;
	CustomFloat tmp_z = Z;

	Y = c_angle * tmp_y - s_angle * tmp_z;
	Z = s_angle * tmp_y + c_angle * tmp_z;
}


/***********************************************************************************************
 * Vector3::Rotate_Y -- Rotates this vector around the y axis                                  *
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
WWINLINE void Vector3::Rotate_Y(CustomFloat angle)
{
	Rotate_Y(sin(angle),cos(angle));
}


/***********************************************************************************************
 * Vector3::Rotate_Y -- Rotates this vector around the Y axis                                  *
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
WWINLINE void Vector3::Rotate_Y(CustomFloat s_angle,CustomFloat c_angle)
{
	CustomFloat tmp_x = X;
	CustomFloat tmp_z = Z;

	X = c_angle * tmp_x + s_angle * tmp_z;
	Z = -s_angle * tmp_x + c_angle * tmp_z;
}


/***********************************************************************************************
 * Vector3::Rotate_Z -- Rotates this vector around the Z axis                                  *
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
WWINLINE void Vector3::Rotate_Z(CustomFloat angle)
{
	Rotate_Z(sin(angle),cos(angle));
}


/***********************************************************************************************
 * Vector3::Rotate_Z -- Rotates this vector around the Z axis                                  *
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
WWINLINE void Vector3::Rotate_Z(CustomFloat s_angle,CustomFloat c_angle)
{
	CustomFloat tmp_x = X;
	CustomFloat tmp_y = Y;

	X = c_angle * tmp_x - s_angle * tmp_y;
	Y = s_angle * tmp_x + c_angle * tmp_y;
}


/***********************************************************************************************
 * Vector3::Is_Valid -- Verifies that each component of this vector is a valid CustomFloat           *
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
WWINLINE bool Vector3::Is_Valid(void) const
{
	return (WWMath::Is_Valid_Float(X) && WWMath::Is_Valid_Float(Y) && WWMath::Is_Valid_Float(Z));
}

WWINLINE CustomFloat Vector3::Find_X_At_Y(CustomFloat y, const Vector3 &p1, const Vector3 &p2) 
{
	return(p1.X + ((y - p1.Y) * ((p2.X - p1.X) / (p2.Y - p1.Y))));
}
WWINLINE CustomFloat Vector3::Find_X_At_Z(CustomFloat z, const Vector3 &p1, const Vector3 &p2) 
{
	return(p1.X + ((z - p1.Z) * ((p2.X - p1.X) / (p2.Z - p1.Z))));
}
WWINLINE CustomFloat Vector3::Find_Y_At_X(CustomFloat x, const Vector3 &p1, const Vector3 &p2)  
{
	return(p1.Y + ((x - p1.X) * ((p2.Y - p1.Y) / (p2.X - p1.X))));
}
WWINLINE CustomFloat Vector3::Find_Y_At_Z(CustomFloat z, const Vector3 &p1, const Vector3 &p2)  
{
	return(p1.Y + ((z - p1.Z) * ((p2.Y - p1.Y) / (p2.Z - p1.Z))));
}
WWINLINE CustomFloat Vector3::Find_Z_At_X(CustomFloat x, const Vector3 &p1, const Vector3 &p2)  
{
	return(p1.Z + ((x - p1.X) * ((p2.Z - p1.Z) / (p2.X - p1.X))));
}
WWINLINE CustomFloat Vector3::Find_Z_At_Y(CustomFloat y, const Vector3 &p1, const Vector3 &p2) 
{
	return(p1.Z + ((y - p1.Y) * ((p2.Z - p1.Z) / (p2.Y - p1.Y))));
}

/***********************************************************************************************
 * Vector3::Distance -- Accurate distance calculation.                                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/1999MLL: Created.                                                                   *
 *=============================================================================================*/
WWINLINE CustomFloat Vector3::Distance(const Vector3 &p1, const Vector3 &p2)
{
	Vector3	temp;
	temp = p1 - p2;
	return (temp.Length());
}

/***********************************************************************************************
 * Vector3::Quick_Distance -- Fast but inaccurate distance calculation.                        *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/1999MLL: Created.                                                                   *
 *=============================================================================================*/
WWINLINE CustomFloat Vector3::Quick_Distance(const Vector3 &p1, const Vector3 &p2)
{
	Vector3	temp;
	temp = p1 - p2;
	return (temp.Quick_Length());
}

/***********************************************************************************************
 * Vector3::Convert_To_ABGR -- Converts to SR packed color				.                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/1999MLL: Created.                                                                   *
 *=============================================================================================*/
WWINLINE unsigned long	Vector3::Convert_To_ABGR( void ) const 
{
	return (unsigned(255)<<24) | 
			 ((unsigned)(Int)(Z*255.0f)<<16) | 
			 ((unsigned)(Int)(Y*255.0f)<<8) | 
			 ((unsigned)(Int)(X*255.0f));
}

/***********************************************************************************************
 * Vector3::Convert_To_ARGB -- Converts to packed color				.                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/29/1999MLL: Created.                                                                   *
 *=============================================================================================*/
WWINLINE unsigned long	Vector3::Convert_To_ARGB( void ) const 
{
	return (unsigned(255)<<24) | 
			 ((unsigned)(Int)(X*255.0f)<<16) | 
			 ((unsigned)(Int)(Y*255.0f)<<8) | 
			 ((unsigned)(Int)(Z*255.0f));
}

#endif /* Vector3_H */

