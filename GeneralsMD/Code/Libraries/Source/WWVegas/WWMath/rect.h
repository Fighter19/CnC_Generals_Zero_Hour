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
 *                 Project Name : WWMath                                                       *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/wwmath/rect.h                              $*
 *                                                                                             *
 *                       Author:: Byon Garrabrant                                              *
 *                                                                                             *
 *                     $Modtime:: 4/16/01 10:01a                                              $*
 *                                                                                             *
 *                    $Revision:: 9                                                           $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RECT_H
#define RECT_H

#include "vector2.h"

class RectClass
{
public:
	CustomFloat	Left;
	CustomFloat	Top;
	CustomFloat	Right;
	CustomFloat	Bottom;

	// Constructors
	RectClass( void )		{};
	RectClass( const RectClass & r )	{ Left = r.Left; Top = r.Top; Right = r.Right; Bottom = r.Bottom; }
	RectClass( CustomFloat left, CustomFloat top, CustomFloat right, CustomFloat bottom )	{ Left = left; Top = top; Right = right; Bottom = bottom; }
	RectClass( const Vector2 & top_left, const Vector2 & bottom_right )	{ Left = top_left.X; Top = top_left.Y; Right = bottom_right.X; Bottom = bottom_right.Y; }

	// Assignment
	RectClass & operator = (const RectClass & r) { Left = r.Left; Top = r.Top; Right = r.Right; Bottom = r.Bottom; return *this; }	
	void	Set(CustomFloat left, CustomFloat top, CustomFloat right, CustomFloat bottom) { Left = left; Top = top; Right = right; Bottom = bottom; }
	void	Set( const Vector2 & top_left, const Vector2 & bottom_right )	{ Left = top_left.X; Top = top_left.Y; Right = bottom_right.X; Bottom = bottom_right.Y; }
	void	Set(const RectClass & r) { Left = r.Left; Top = r.Top; Right = r.Right; Bottom = r.Bottom; }

	// Access
	CustomFloat	Width(void) const		{ return Right - Left; }
	CustomFloat	Height(void) const	{ return Bottom - Top; }
	Vector2 Center( void ) const { return Vector2( (Left + Right)/2, (Top + Bottom)/2 ); }
	Vector2 Extent( void ) const { return Vector2( (Right - Left)/2, (Bottom - Top)/2 ); }
	Vector2 Upper_Left( void ) const { return Vector2( Left, Top ); }
	Vector2 Lower_Right( void ) const { return Vector2( Right, Bottom ); }
	Vector2 Upper_Right( void ) const { return Vector2( Right, Top ); }
	Vector2 Lower_Left( void ) const { return Vector2( Left, Bottom ); }

	// Scaling
	RectClass & operator *= (CustomFloat k) { return Scale( k ); }
	RectClass & operator /= (CustomFloat k) { return Scale( (CustomFloat)1/k ); }
	RectClass & Scale_Relative_Center( CustomFloat k )	{ Vector2 center = Center(); *this-=center; Left*=k; Top*=k; Right*=k; Bottom*=k; *this+=center; return *this; }
	RectClass & Scale( CustomFloat k )				{ Left*=k; Top*=k; Right*=k; Bottom*=k; return *this; }
	RectClass & Scale( const Vector2 &k )				{ Left*=k.X; Top*=k.Y; Right*=k.X; Bottom*=k.Y; return *this; }
	RectClass & Inverse_Scale( const Vector2 &k )	{ Left/=k.X; Top/=k.Y; Right/=k.X; Bottom/=k.Y; return *this; }

	// Offset
	RectClass & operator += ( const Vector2 & o ) { Left+=o.X; Top+=o.Y; Right+=o.X; Bottom+=o.Y; return *this; }
	RectClass & operator -= ( const Vector2 & o ) { Left-=o.X; Top-=o.Y; Right-=o.X; Bottom-=o.Y; return *this; }

	// Inflate
	void	Inflate( const Vector2 & o ) { Left-=o.X; Top-=o.Y; Right+=o.X; Bottom+=o.Y; }

	// Union
	RectClass & operator += ( const RectClass & r ) { Left=MIN(Left,r.Left); Top=MIN(Top,r.Top); Right=MAX(Right,r.Right); Bottom=MAX(Bottom,r.Bottom); return *this; }

	// Equality
	bool operator == ( const RectClass &rval ) const { return (rval.Left == Left) && (rval.Right == Right) && (rval.Top == Top) && (rval.Bottom == Bottom); }
	bool operator != ( const RectClass &rval ) const { return (rval.Left != Left) || (rval.Right != Right) || (rval.Top != Top) || (rval.Bottom != Bottom); }

	// Containment
	bool	Contains ( const Vector2 &pos ) const { return (pos.X >= Left) && (pos.X <= Right) && (pos.Y >= Top) && (pos.Y <= Bottom); }

	// Misc
	void	Snap_To_Units( const Vector2 & u ) { Left = floor(Left / u.X + (CustomFloat)0.5f) * u.X; Right = floor(Right / u.X + (CustomFloat)0.5f) * u.X; Top = floor(Top / u.Y + (CustomFloat)0.5f) * u.Y; Bottom = floor(Bottom / u.Y + (CustomFloat)0.5f) * u.Y; }

};

#endif

