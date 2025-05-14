#ifndef FIXED_POINT_H
#define FIXED_POINT_H


#include <cstddef>

template<typename T, typename T2, size_t dp>
class fixed
{
public:
  T value = T(0);
  constexpr fixed() = default;
  constexpr fixed(const double d)
  {
    value = T(d * double(1 << dp) + (d >= 0 ? 0.5 : -0.5)); 
  }

  constexpr operator double() const
  {
    return double(value) / double(1 << dp);
  }

private:
  static constexpr fixed form(T v) {fixed k; k.value = v; return k; }

public:

  // Assign
  constexpr fixed& operator = (const fixed& f) = default;

  // Negate
  constexpr fixed operator - () const
  {
    return form(-this->value);
  }

  // Addition
  constexpr fixed operator + (const fixed& f) const
  {
    return form(this->value + f.value);
  }

  constexpr fixed& operator += (const fixed& f)
  {
    this->value += f.value; return *this;
  }

  // Subtraction
  constexpr fixed operator - (const fixed& f) const
  {
    return form(this->value - f.value);
  }

  constexpr fixed& operator -= (const fixed& f)
  {
    this->value -= f.value; return *this;
  }

  // Multiplication
  constexpr fixed operator * (const fixed& f) const
  {
    return form((T2(this->value) * T2(f.value)) >> dp);
  }
  constexpr fixed& operator *= (const fixed& f) 
  {
    this->value = (T2(this->value) * T2(f.value)) >> dp; return *this;
  }

  // Division
  constexpr fixed operator / (const fixed& f) const
  {
    return form(((T2(this->value) << dp) / T2(f.value)));
  }

  constexpr fixed& operator /= (const fixed& f)
  {
    this->value = ((T2(this->value) << dp) / T2(f.value)); return *this;
  }

  // Square Root
  static fixed sqrt(const fixed& x) {
    if (x.value <= 0)
      return fixed(0);

    T2 xn = T2(x.value);                // initial guess in Qdp
    xn = xn < (1 << dp) ? (1 << dp) : xn;  // avoid dividing by too small

    // Convert x to Q2dp to preserve precision during iteration
    T2 x_q2dp = T2(x.value) << dp;      // x * 2^dp → Q2dp

    for (int i = 0; i < 6; ++i) {
      // Newton-Raphson: y_{n+1} = (y_n + x / y_n) / 2
      xn = (xn + x_q2dp / xn) >> 1;
    }

    return form(T(xn)); // still in Qdp scale
  }

};


typedef fixed<int32_t, int64_t, 16> fp16_16;
// typedef fixed<int16_t, int32_t, 8> fp16_16;
typedef int bit_type;
using bitvec = std::vector<bit_type>;
using llrvec = std::vector<int64_t>;
using fltvec = std::vector<float>;
using intvec = std::vector<int>;
using dblvec = std::vector<double>;

#endif