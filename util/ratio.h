#ifndef UTIL_RATIO_H_
#define UTIL_RATIO_H_

#include <cstdint>
#include <numeric>
#include <optional>
#include <utility>

namespace aoc {

class Ratio {
 public:
  Ratio(int64_t intval) : numerator_(intval) {}

  bool operator==(const Ratio& other) const {
    return numerator_ == other.numerator_ && denominator_ == other.denominator_;
  }

  bool operator!=(const Ratio& other) const {
    return !(numerator_ == other);
  }

  int64_t IntegralRounded() const {
    return numerator_ / denominator_;
  }

  std::optional<int64_t> IntegralPrecise() const {
    if (denominator_ != 1) return std::nullopt;
    return numerator_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const Ratio& r) {
    return H::combine(std::move(h), r.numerator_, r.denominator_);
  }

  Ratio operator-() const {
    Ratio result(*this);
    result.numerator_ = -numerator_;
    return result;
  }

  Ratio& operator+=(const Ratio& other) {
    numerator_ =
        numerator_ * other.denominator_ + other.numerator_ * denominator_;
    denominator_ = denominator_ * other.denominator_;
    Reduce();
    return *this;
  }

  Ratio operator+(const Ratio& other) const {
    Ratio result(*this);
    result += other;
    return result;
  }

  Ratio& operator-=(const Ratio& other) {
    *this += -other;
    return *this;
  }

  Ratio operator-(const Ratio& other) const {
    Ratio result(*this);
    result -= other;
    return result;
  }

  Ratio& operator*=(const Ratio& other) {
    numerator_ *= other.numerator_;
    denominator_ *= other.denominator_;
    Reduce();
    return *this;
  }

  Ratio operator*(const Ratio& other) const {
    Ratio result(*this);
    result *= other;
    return result;
  }

  Ratio& operator/=(const Ratio& other) {
    numerator_ *= other.denominator_;
    denominator_ *= other.numerator_;
    Reduce();
    return *this;
  }

  Ratio operator/(const Ratio& other) const {
    Ratio result(*this);
    result /= other;
    return result;
  }

 private:
  void Reduce() {
    if (numerator_ == 0) {
      denominator_ = 1;
      return;
    }
    if (denominator_ < 0) {
      numerator_ = -numerator_;
      denominator_ = -denominator_;
    }
    const int64_t gcd = std::gcd(numerator_, denominator_);
    numerator_ /= gcd;
    denominator_ /= gcd;
  }

  int64_t numerator_ = 0;
  int64_t denominator_ = 1;
};

}  // namespace aoc

#endif  // UTIL_RATIO_H_
