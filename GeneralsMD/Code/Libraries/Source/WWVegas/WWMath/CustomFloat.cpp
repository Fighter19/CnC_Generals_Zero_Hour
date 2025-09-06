#include "CustomFloat.h"
#include <cmath>

CustomFloat::CustomFloat(float f)
{
  value = f;
}

CustomFloat CustomFloat::operator+(const CustomFloat& other) const
{
  CustomFloat result;
  result.value = this->value + other.value;
  return result;
}

CustomFloat CustomFloat::operator-(const CustomFloat& other) const
{
  CustomFloat result;
  result.value = this->value - other.value;
  return result;
}

CustomFloat CustomFloat::operator*(const CustomFloat& other) const
{
  CustomFloat result;
  result.value = this->value * other.value;
  return result;
}

CustomFloat CustomFloat::operator/(const CustomFloat& other) const
{
  CustomFloat result;
  result.value = this->value / other.value;
  return result;
}

// compount assignments
CustomFloat &CustomFloat::operator+=(const CustomFloat &other)
{
  *this = *this + other;
  return *this;
}

CustomFloat &CustomFloat::operator-=(const CustomFloat &other)
{
  *this = *this - other;
  return *this;
}

CustomFloat &CustomFloat::operator*=(const CustomFloat &other)
{
  *this = *this * other;
  return *this;
}

CustomFloat &CustomFloat::operator/=(const CustomFloat &other)
{
  *this = *this / other;
  return *this;
}

CustomFloat CustomFloat::operator-() const
{
  return CustomFloat(-value);
}

CustomFloat CustomFloat::operator+() const
{
  return CustomFloat(+value);
}

// comparison
bool CustomFloat::operator==(const CustomFloat& other) const
{
  return value == other.value;
}

bool CustomFloat::operator!=(const CustomFloat& other) const
{
  return value != other.value;
}

bool CustomFloat::operator<(const CustomFloat& other) const
{
  return value < other.value;
}

bool CustomFloat::operator<=(const CustomFloat& other) const
{
  return value <= other.value;
}

bool CustomFloat::operator>(const CustomFloat& other) const
{
  return value > other.value;
}

bool CustomFloat::operator>=(const CustomFloat& other) const
{
  return value >= other.value;
}

CustomFloat CustomFloat::sqrt() const
{
  CustomFloat result;
  result.value = sqrtf(this->value);
  return result;
}

CustomFloat CustomFloat::pow(const CustomFloat &exponent) const
{
  CustomFloat result;
  result.value = ::pow((float)this->value, (float)exponent.value);
  return result;
}

bool CustomFloat::isnan() const
{
  return ::isnan(value);
}

CustomFloat::operator Int() const
{
  return static_cast<Int>(value);
}

CustomFloat::operator long() const
{
  return static_cast<long>(value);
}
