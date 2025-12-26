#include "types.h"

QuadNum::QuadNum() : a(0), b(0), d(1) {}

QuadNum::QuadNum(int64 a) : a(a), b(0), d(1) {}

QuadNum::QuadNum(int64 a, int64 b, int64 d) : a(a), b(b), d(d) {
  normalize();
}

void QuadNum::normalize() {
  int64 g = gcd(gcd(abs(a), abs(b)), abs(d));
  if (g > 1) {
    a /= g;
    b /= g;
    d /= g;
  }
  if (d < 0) {
    a = -a;
    b = -b;
    d = -d;
  }
}

int QuadNum::sign() const {
  int64 lhs = a * a;
  int64 rhs = ROOT * b * b;
  if (lhs > rhs) return (a > 0) ? 1 : -1;
  if (rhs > lhs) return (b > 0) ? 1 : -1;
  return 0;
}

QuadNum& QuadNum::operator+=(const QuadNum& other) {
  *this = *this + other;
  return *this;
}

QuadNum& QuadNum::operator-=(const QuadNum& other) {
  *this = *this - other;
  return *this;
}

QuadNum& QuadNum::operator*=(const QuadNum& other) {
  *this = *this * other;
  return *this;
}

QuadNum& QuadNum::operator/=(const QuadNum& other) {
  *this = *this / other;
  return *this;
}

QuadNum QuadNum::operator-() const {
  return QuadNum(-a, -b, d);
}

QuadNum operator+(const QuadNum& x, const QuadNum& y) {
  int64 a = x.a * y.d + y.a * x.d;
  int64 b = x.b * y.d + y.b * x.d;
  int64 d = x.d * y.d;
  return QuadNum(a, b, d);
}

QuadNum operator-(const QuadNum& x, const QuadNum& y) {
  int64 a = x.a * y.d - y.a * x.d;
  int64 b = x.b * y.d - y.b * x.d;
  int64 d = x.d * y.d;
  return QuadNum(a, b, d);
}

QuadNum operator*(const QuadNum& x, const QuadNum& y) {
  int64 a = x.a * y.a + ROOT * x.b * y.b;
  int64 b = x.a * y.b + x.b * y.a;
  int64 d = x.d * y.d;
  return QuadNum(a, b, d);
}

QuadNum operator/(const QuadNum& x, const QuadNum& y) {
  if (y.sign() == 0) {
    throw std::invalid_argument("Division by zero");
  }
  int64 a = y.d * (x.a * y.a - ROOT * x.b * y.b);
  int64 b = y.d * (x.b * y.a - x.a * y.b);
  int64 d = x.d * (y.a * y.a - ROOT * y.b * y.b);
  return QuadNum(a, b, d);
}

std::ostream& operator<<(std::ostream& os, const QuadNum& x) {
  os << x.a << ", " << x.b << ", " << x.d;
  return os;
}

Point::Point(const QuadNum& x, const QuadNum& y) : x(x), y(y) {}

Line::Line(const Point& p, const Point& q) {
  a = q.y - p.y;
  b = p.x - q.x;
  c = q.x * p.y - p.x * q.y;
  if (a.sign() == 0 && b.sign() == 0) {
    throw std::invalid_argument("Degenerate line");
  }
  normalize();
}

void Line::normalize() {
  if (a.sign() != 0) {
    c /= a;
    b /= a;
    a = QuadNum(1);
  } else {
    c /= b;
    b = QuadNum(1);
  }
  if (a.sign() == -1 || (a.sign() == 0 && b.sign() == -1)) {
    a = -a;
    b = -b;
    c = -c;
  }
}

Circle::Circle(const Point& p, const Point& q) : x(p.x), y(p.y) {
  QuadNum dx = q.x - p.x;
  QuadNum dy = q.y - p.y;
  r2 = dx * dx + dy * dy;
  if (r2.sign() == 0) {
    throw std::invalid_argument("Degenerate circle");
  }
}

std::size_t State::size() const {
  return lines.size() + circles.size();
}

bool operator==(const QuadNum& x, const QuadNum& y) {
  return x.a == y.a && x.b == y.b && x.d == y.d;
};

bool operator==(const Point& p, const Point& q) {
  return p.x == q.x && p.y == q.y;
};

bool operator==(const Line& l1, const Line& l2) {
  return l1.a == l2.a && l1.b == l2.b && l1.c == l2.c;
};

bool operator==(const Circle& c1, const Circle& c2) {
  return c1.x == c2.x && c1.y == c2.y && c1.r2 == c2.r2;
};

bool operator==(const State& s1, const State& s2) {
  return s1.points  == s2.points && s1.lines   == s2.lines && s1.circles == s2.circles;
};

std::size_t QuadNumHash::operator()(const QuadNum& x) const noexcept {
  std::size_t h = std::hash<int>{}(x.a);
  hash_combine(h, std::hash<int>{}(x.b));
  hash_combine(h, std::hash<int>{}(x.d));
  return h;
}

std::size_t PointHash::operator()(const Point& p) const noexcept {
  std::size_t h = QuadNumHash{}(p.x);
  hash_combine(h, QuadNumHash{}(p.y));
  return h;
}

std::size_t LineHash::operator()(const Line& l) const noexcept {
  std::size_t h = QuadNumHash{}(l.a);
  hash_combine(h, QuadNumHash{}(l.b));
  hash_combine(h, QuadNumHash{}(l.c));
  return h;
}

std::size_t CircleHash::operator()(const Circle& c) const noexcept {
  std::size_t h = QuadNumHash{}(c.x);
  hash_combine(h, QuadNumHash{}(c.y));
  hash_combine(h, QuadNumHash{}(c.r2));
  return h;
}

std::size_t StateHash::operator()(const State& s) const noexcept {
  std::size_t h = 0;
  hash_combine(h, hash_unordered_set<Point, PointHash>(s.points));
  hash_combine(h, hash_unordered_set<Line, LineHash>(s.lines));
  hash_combine(h, hash_unordered_set<Circle, CircleHash>(s.circles));
  return h;
}
