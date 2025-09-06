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

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : G                                                            *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/wwmath/v3_rnd.cpp                            $*
 *                                                                                             *
 *                      $Author:: Greg_h                                                      $*
 *                                                                                             *
 *                     $Modtime:: 7/09/99 9:49a                                               $*
 *                                                                                             *
 *                    $Revision:: 4                                                           $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "v3_rnd.h"
#include "vector2.h"

const CustomFloat Vector3Randomizer::OOIntMax = (CustomFloat)1.0f / (CustomFloat)INT_MAX;
const CustomFloat Vector3Randomizer::OOUIntMax = (CustomFloat)1.0f / (CustomFloat)UINT_MAX;
Random3Class Vector3Randomizer::Randomizer;

Vector3SolidBoxRandomizer::Vector3SolidBoxRandomizer(const Vector3 & extents)
{
	Extents.X = MAX(extents.X, CustomFloat(0.0f));
	Extents.Y = MAX(extents.Y, CustomFloat(0.0f));
	Extents.Z = MAX(extents.Z, CustomFloat(0.0f));
}

void Vector3SolidBoxRandomizer::Get_Vector(Vector3 &vector)
{
	vector.X = Get_Random_Float_Minus1_To_1() * Extents.X;
	vector.Y = Get_Random_Float_Minus1_To_1() * Extents.Y;
	vector.Z = Get_Random_Float_Minus1_To_1() * Extents.Z;
}

CustomFloat Vector3SolidBoxRandomizer::Get_Maximum_Extent(void)
{
	CustomFloat max = MAX(Extents.X, Extents.Y);
	max = MAX(max, Extents.Z);
	return max;
}

void Vector3SolidBoxRandomizer::Scale(CustomFloat scale)
{
	scale = MAX(scale, CustomFloat(0.0f));
	Extents.X *= scale;
	Extents.Y *= scale;
	Extents.Z *= scale;
}


Vector3SolidSphereRandomizer::Vector3SolidSphereRandomizer(CustomFloat radius)
{
	Radius = MAX(radius, 0.0f);
}

void Vector3SolidSphereRandomizer::Get_Vector(Vector3 &vector)
{
	// Generate vectors in a cube and discard the ones not in a sphere
	CustomFloat rad_squared = Radius * Radius;
	for (;;) {
		vector.X = Get_Random_Float_Minus1_To_1() * Radius;
		vector.Y = Get_Random_Float_Minus1_To_1() * Radius;
		vector.Z = Get_Random_Float_Minus1_To_1() * Radius;
		if (vector.Length2() <= rad_squared) break;
	}
}

CustomFloat Vector3SolidSphereRandomizer::Get_Maximum_Extent(void)
{
	return Radius;
}

void Vector3SolidSphereRandomizer::Scale(CustomFloat scale)
{
	scale = MAX(scale, CustomFloat(0.0f));
	Radius *= scale;
}


Vector3HollowSphereRandomizer::Vector3HollowSphereRandomizer(CustomFloat radius)
{
	Radius = MAX(radius, CustomFloat(0.0f));
}

void Vector3HollowSphereRandomizer::Get_Vector(Vector3 &vector)
{
	// Generate vectors in a 2x2x2 origin-centered cube, discard the ones not in a unit-radius
	// sphere and scale the result to Radius.
	CustomFloat v_l2;
	for (;;) {
		vector.X = Get_Random_Float_Minus1_To_1();
		vector.Y = Get_Random_Float_Minus1_To_1();
		vector.Z = Get_Random_Float_Minus1_To_1();
		v_l2 = vector.Length2();
		if (v_l2 <= 1.0f && v_l2 > 0.0f) break;
	}

	CustomFloat scale = Radius * WWMath::Inv_Sqrt(v_l2);

	vector.X *= scale;
	vector.Y *= scale;
	vector.Z *= scale;
}

CustomFloat Vector3HollowSphereRandomizer::Get_Maximum_Extent(void)
{
	return Radius;
}

void Vector3HollowSphereRandomizer::Scale(CustomFloat scale)
{
	scale = MAX(scale, CustomFloat(0.0f));
	Radius *= scale;
}


Vector3SolidCylinderRandomizer::Vector3SolidCylinderRandomizer(CustomFloat extent, CustomFloat radius)
{
	Extent = MAX(extent, CustomFloat(0.0f));
	Radius = MAX(radius, CustomFloat(0.0f));
}

void Vector3SolidCylinderRandomizer::Get_Vector(Vector3 &vector)
{
	vector.X = Get_Random_Float_Minus1_To_1() * Extent;

	// Generate 2D vectors in a square and discard the ones not in a circle
	Vector2 vec2;
	CustomFloat rad_squared = Radius * Radius;
	for (;;) {
		vec2.X = Get_Random_Float_Minus1_To_1() * Radius;
		vec2.Y = Get_Random_Float_Minus1_To_1() * Radius;
		if (vec2.Length2() <= rad_squared) break;
	}

	vector.Y = vec2.X;
	vector.Z = vec2.Y;
}

CustomFloat Vector3SolidCylinderRandomizer::Get_Maximum_Extent(void)
{
	return MAX(Extent, Radius);
}

void Vector3SolidCylinderRandomizer::Scale(CustomFloat scale)
{
	scale = MAX(scale, CustomFloat(0.0f));
	Extent *= scale;
	Radius *= scale;
}
