#pragma once
#include <type_traits>

typedef int								Int;							// 4 bytes 

class CustomFloat
{
	public:
		CustomFloat() = default;
		CustomFloat(float f);
		// CustomFloat is not allowed to have a destructor, because it is used in unions and must be trivially destructible.
		// This rules out the usage of memory allocations in this class.

		// Use default copy constructor and assignment operator to keep the type trivially copyable.
		
		// arithmetic operations
		CustomFloat operator+(const CustomFloat& other) const;
		CustomFloat operator-(const CustomFloat& other) const;
		CustomFloat operator*(const CustomFloat& other) const;
		CustomFloat operator/(const CustomFloat& other) const;

		// arithmetic operations with Int
		CustomFloat operator+(Int other) const
		{
			CustomFloat temp((float)other);
			return *this + temp;
		}
		CustomFloat operator-(Int other) const
		{
			CustomFloat temp((float)other);
			return *this - temp;
		}
		CustomFloat operator*(Int other) const
		{
			CustomFloat temp((float)other);
			return *this * temp;
		}
		CustomFloat operator/(Int other) const
		{
			CustomFloat temp((float)other);
			return *this / temp;
		}

		// arithmetic operations with float
		CustomFloat operator+(float other) const
		{
			CustomFloat temp(other);
			return *this + temp;
		}
		CustomFloat operator-(float other) const
		{
			CustomFloat temp(other);
			return *this - temp;
		}
		CustomFloat operator*(float other) const
		{
			CustomFloat temp(other);
			return *this * temp;
		}
		CustomFloat operator/(float other) const
		{
			CustomFloat temp(other);
			return *this / temp;
		}

		// arithmetic operations with double
		CustomFloat operator+(double other) const
		{
			CustomFloat temp((float)other);
			return *this + temp;
		}
		CustomFloat operator-(double other) const
		{
			CustomFloat temp((float)other);
			return *this - temp;
		}
		CustomFloat operator*(double other) const
		{
			CustomFloat temp((float)other);
			return *this * temp;
		}
		CustomFloat operator/(double other) const
		{
			CustomFloat temp((float)other);
			return *this / temp;
		}

		// compound assignment
		CustomFloat& operator+=(const CustomFloat& other);
		CustomFloat& operator-=(const CustomFloat& other);
		CustomFloat& operator*=(const CustomFloat& other);
		CustomFloat& operator/=(const CustomFloat& other);

		// unary operations
		CustomFloat operator-() const;
		CustomFloat operator+() const;

		// comparisons
		bool operator==(const CustomFloat& other) const;
		bool operator!=(const CustomFloat& other) const;
		bool operator<(const CustomFloat& other) const;
		bool operator<=(const CustomFloat& other) const;
		bool operator>(const CustomFloat& other) const;
		bool operator>=(const CustomFloat& other) const;

		// Integer comparisons
    bool operator==(float other) const
    {
      CustomFloat temp(other);
      return *this == temp;
    }
    bool operator!=(float other) const
    {
      CustomFloat temp(other);
      return *this != temp;
    }
    bool operator<(float other) const
    {
      CustomFloat temp(other);
      return *this < temp;
    }
    bool operator<=(float other) const
    {
      CustomFloat temp(other);
      return *this <= temp;
    }
    bool operator>(float other) const
    {
      CustomFloat temp(other);
      return *this > temp;
    }
    bool operator>=(float other) const
    {
      CustomFloat temp(other);
      return *this >= temp;
    }

		CustomFloat sqrt() const;
		CustomFloat pow(const CustomFloat& exponent) const;
		bool isnan() const;

		// Int cast
		explicit operator Int() const;
    explicit operator long() const;
    explicit operator bool() const;
    explicit operator float() const;

    static CustomFloat atan(const CustomFloat& value);
		static CustomFloat atan2(const CustomFloat& y, const CustomFloat& x);
		static CustomFloat cos(const CustomFloat& value);
    static CustomFloat acos(const CustomFloat& value);
		static CustomFloat sin(const CustomFloat& value);
    static CustomFloat asin(const CustomFloat& value);
		static CustomFloat tan(const CustomFloat& value);
		static CustomFloat ceil(const CustomFloat& value);
    static CustomFloat floor(const CustomFloat& value);

	public:
		// A 32-bit float placeholder
		float value;
};

static_assert(std::is_trivial<CustomFloat>::value, "CustomFloat must be trivial");

CustomFloat sqrt(const CustomFloat& f)
{
	return f.sqrt();
}

CustomFloat fabs(const CustomFloat& f)
{
	if (f < CustomFloat(0.0f))
		return -f;
	else
		return f;
}

CustomFloat pow(const CustomFloat& base, const CustomFloat& exponent)
{
	return base.pow(exponent);
}

bool isnan(const CustomFloat& f)
{
	return f.isnan();
}

CustomFloat atan(const CustomFloat& value)
{
  return CustomFloat::atan(value);
}

CustomFloat atan2(const CustomFloat& y, const CustomFloat& x)
{
	return CustomFloat::atan2(y, x);
}

CustomFloat cos(const CustomFloat& value)
{
	return CustomFloat::cos(value);
}

CustomFloat acos(const CustomFloat& value)
{
  return CustomFloat::acos(value);
}

CustomFloat sin(const CustomFloat& value)
{
	return CustomFloat::sin(value);
}

CustomFloat asin(const CustomFloat& value)
{
  return CustomFloat::asin(value);
}

CustomFloat tan(const CustomFloat& value)
{
	return CustomFloat::tan(value);
}

CustomFloat ceil(const CustomFloat& value)
{
	return CustomFloat::ceil(value);
}

CustomFloat floor(const CustomFloat& value)
{
  return CustomFloat::floor(value);
}