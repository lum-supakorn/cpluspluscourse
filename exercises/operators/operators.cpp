#include <iomanip>
#include <iostream>
#include <numeric>

class Fraction {
 public:
  Fraction(unsigned int num, unsigned int denom): m_num(num), m_denom(denom) {}
  Fraction(unsigned int num): m_num(num), m_denom(1) {}

  Fraction operator*(unsigned int coeff) {
	  Fraction rr{m_num, m_denom};
	  rr *= coeff;
	  rr.normalize();
	  return rr;
  }

  friend Fraction operator*(unsigned int coeff, Fraction r);

  friend std::ostream& operator<<(std::ostream& os, const Fraction& r);

  friend bool operator==(const Fraction& l, const Fraction& r);
  friend bool operator!=(const Fraction& l, const Fraction& r);
  friend bool operator<(const Fraction& l, const Fraction& r);
  friend bool operator<=(const Fraction& l, const Fraction& r);
  friend bool operator>(const Fraction& l, const Fraction& r);
  friend bool operator>=(const Fraction& l, const Fraction& r);
  
  friend Fraction operator*(const Fraction l, const Fraction r);

  Fraction& operator*=(unsigned int coeff) {
	  m_num *= coeff;
	  this->normalize();
	  return *this;
  }

  Fraction& operator*=(const Fraction& r) {
	  m_num *= r.m_num;
	  m_denom *= r.m_denom;
	  this->normalize();
	  return *this;
  }

 private:
  void normalize() {
    const int gcd = std::gcd(m_num, m_denom);
    m_num /= gcd;
    m_denom /= gcd;
  }

  unsigned int m_num, m_denom;
};

/* Operators */
Fraction operator*(unsigned int coeff, Fraction r) {
	Fraction rr{r.m_num, r.m_denom};
	rr *= coeff;
	rr.normalize();
	return rr;
}

Fraction operator*(const Fraction l, const Fraction r) {
	Fraction rr{l.m_num*r.m_num, l.m_denom*r.m_denom};
	rr.normalize();
	return rr;
}

std::ostream& operator<<(std::ostream& os, const Fraction& r) {
	os << r.m_num << "/" << r.m_denom;
	return os;
}

bool operator==(const Fraction& l, const Fraction& r) {
  Fraction ll = l;
  ll.normalize();
  Fraction rr = r;
  rr.normalize();
  return ((ll.m_num == rr.m_num) && (ll.m_denom == rr.m_denom));
}

bool operator!=(const Fraction& l, const Fraction& r) {
  return !(l == r);
}

bool operator<(const Fraction& l, const Fraction& r) {
  Fraction ll = l;
  ll.normalize();
  Fraction rr = r;
  rr.normalize();
  return ll.m_num*rr.m_denom < rr.m_num*ll.m_denom;
}

bool operator<=(const Fraction& l, const Fraction& r) {
  return l < r || l == r;
}

bool operator>(const Fraction& l, const Fraction& r) {
  Fraction ll = l;
  ll.normalize();
  Fraction rr = r;
  rr.normalize();
  return ll.m_num*rr.m_denom > rr.m_num*ll.m_denom;
}

bool operator>=(const Fraction& l, const Fraction& r) {
  return l > r || l == r;
}

void printAndCheck(
		std::string const & what, Fraction const & result, Fraction const & expected) {
  const bool passed = result == expected;
  std::cout << std::left << std::setw(40) << what << ": "
	  << (passed ? "PASS" : "** FAIL **") << "    " << result << "\n";
}
void printAndCheck(std::string const & what, bool result, bool expected) {
  const bool passed = result == expected;
  std::cout << std::left << std::setw(40) << what << ": "
	  << (passed ? "PASS" : "** FAIL **") << "    " << result << "\n";
}

int main() {
  // create a fraction with values 3 (which is 3/1) and 1/3
  const Fraction three{3};
  const Fraction athird{1, 3};

  // print the fractions
  std::cout << "Three: " << three << '\n';
  std::cout << "One third: " << athird << '\n';

  // multiply fraction with an int
  // the printAndCheck function requires operator<< and operator==:
  printAndCheck("One third times two", athird * 2, Fraction{2, 3});
  // ensure symmetry
  printAndCheck("Two times one third", 2 * athird, Fraction{2, 3});

  // multiply two fractions
  printAndCheck("Three times one third", three * athird, Fraction{1, 1});
  // normalize the fraction after multiplication so the above statement
  // prints 1/1 instead of e.g. 3/3
  printAndCheck("Three times one third", 3 * athird, Fraction{1, 1});

  // multiply in place
  Fraction f = athird;
  f *= 2;
  printAndCheck("One third times two", f, Fraction{2, 3});

  f *= athird;
  printAndCheck("Two third times one third", f, Fraction{2, 9});

  // you might have some redundancy between the implementation of operator* and
  // operator*=. Can you refactor your code and implement operator* in terms of
  // operator*=?

  std::cout << std::boolalpha;  // print bools as 'true' or 'false' from now on

  // more equality comparisons
  printAndCheck("One third == one third", (athird == Fraction{1, 3}), true);
  printAndCheck("One third != one forth", (athird != Fraction{1, 4}), true);
  printAndCheck("One third == two sixth", (athird == Fraction{2, 6}), true);
  printAndCheck("One third != three sixth", (athird != Fraction{3, 6}), true);
  // try to implement operator!= in terms of operator==

  // more comparisons
  const Fraction afourth{1, 4};
  printAndCheck("athird < athird", (athird < athird), false);
  printAndCheck("afourth < athird", (afourth < athird), true);
  printAndCheck("athird  <= athird", (athird <= athird), true);
  printAndCheck("athird  <= afourth", (athird <= afourth), false);
  printAndCheck("athird  > athird", (athird > athird), false);
  printAndCheck("afourth > athird", (afourth > athird), false);
  printAndCheck("athird >= athird", (athird >= athird), true);
  printAndCheck("athird >= afourth", (athird >= afourth), true);
  // the operators <=, >= and > can typically be implemented just in terms of
  // operator<. Can you do this as well? ;)

  // take aways on operators:
  // * we can very often implement an arithemtic operator@ in terms of
  // operator@=
  // * it usually suffices to implement operator< and operator== and derive the
  // other relational operators from them. C++20 will do part of this automatically.
  
  return 0;
}
