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
  return {-a, -b, d};
}

double QuadNum::to_double() const {
  return (static_cast<double>(a) + static_cast<double>(b) * std::sqrt(static_cast<double>(ROOT))) / static_cast<double>(d);
}

bool operator==(const QuadNum& x, const QuadNum& y) {
  return x.a == y.a && x.b == y.b && x.d == y.d;
};

QuadNum operator+(const QuadNum& x, const QuadNum& y) {
  int64 a = x.a * y.d + y.a * x.d;
  int64 b = x.b * y.d + y.b * x.d;
  int64 d = x.d * y.d;
  return {a, b, d};
}

QuadNum operator-(const QuadNum& x, const QuadNum& y) {
  int64 a = x.a * y.d - y.a * x.d;
  int64 b = x.b * y.d - y.b * x.d;
  int64 d = x.d * y.d;
  return {a, b, d};
}

QuadNum operator*(const QuadNum& x, const QuadNum& y) {
  int64 a = x.a * y.a + ROOT * x.b * y.b;
  int64 b = x.a * y.b + x.b * y.a;
  int64 d = x.d * y.d;
  return {a, b, d};
}

QuadNum operator/(const QuadNum& x, const QuadNum& y) {
  if (sign(y) == 0) throw std::invalid_argument("Division by zero");
  int64 a = y.d * (x.a * y.a - ROOT * x.b * y.b);
  int64 b = y.d * (x.b * y.a - x.a * y.b);
  int64 d = x.d * (y.a * y.a - ROOT * y.b * y.b);
  return {a, b, d};
}

int64 isqrt(int64 n) {
  if (n < 0) return -1;
  if (n == 0) return 0;
  int64 x = n;
  while (true) {
    int64 y = (x + n / x) / 2;
    if (y >= x) {
      return x * x == n ? x : -1;
    }
    x = y;
  }
}

QuadNum qsqrt(const QuadNum& x) {
  if (sign(x) < 0) return -1;
  if (sign(x) == 0) return 0;
  int64 a = x.a, b = x.b, d = x.d;
  if (b == 0) {
    int64 num2 = a * d;
    int64 r = isqrt(num2);
    if (r != -1) return {r, 0, d};
    if (num2 % ROOT != 0) return -1;
    int64 s = isqrt(num2 / ROOT);
    if (s == -1) return -1;
    return {0, s, d};
  }
  int64 alpha = a * d;
  int64 beta = b * d;
  int64 disc = isqrt(alpha * alpha - ROOT * beta * beta);
  if (disc == -1) return -1;
  for (int sgn : {1, -1}) {
    int64 num = alpha + sgn * disc;
    if (num % 2 != 0) continue;
    int64 r = isqrt(num / 2);
    if (r == -1) continue;
    if (beta % (2 * r) != 0) continue;
    int64 s = beta / (2 * r);
    QuadNum res = {r, s, d};
    return sign(res) == 1 ? res : -res;
  }
  return -1;
}

int sign(const QuadNum& x) {
  int64 lhs = x.a * x.a;
  int64 rhs = ROOT * x.b * x.b;
  if (lhs > rhs) return (x.a > 0) ? 1 : -1;
  if (rhs > lhs) return (x.b > 0) ? 1 : -1;
  return 0;
}

std::ostream& operator<<(std::ostream& os, const QuadNum& x) {
  os << "{" << x.a << " + " << x.b << "√" << ROOT << "} / " << x.d;
  return os;
}

Point::Point(const QuadNum& x, const QuadNum& y) : x(x), y(y) {}

bool operator==(const Point& p, const Point& q) {
  return p.x == q.x && p.y == q.y;
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
  os << p.x.to_double() << " " << p.y.to_double();
  return os;
}

Segment::Segment(const Point& p1, const Point& p2) : p1(p1), p2(p2) {}

bool operator==(const Segment& s1, const Segment& s2) {
  return s1.p1 == s2.p1 && s1.p2 == s2.p2;
};

std::ostream& operator<<(std::ostream& os, const Segment& s) {
  os << s.p1 << " " << s.p2;
  return os;
}

Line::Line(const Point& p, const Point& q) {
  a = q.y - p.y;
  b = p.x - q.x;
  c = p.x * q.y - q.x * p.y;
  if (sign(a) == 0 && sign(b) == 0) throw std::invalid_argument("Degenerate line");
  normalize();
}

Line::Line(const Segment& s) : Line(s.p1, s.p2) {}

void Line::normalize() {
  if (sign(a) != 0) {
    c /= a;
    b /= a;
    a = 1;
  } else {
    c /= b;
    b = 1;
  }
  if (sign(a) == -1 || (sign(a) == 0 && sign(b) == -1)) {
    a = -a;
    b = -b;
    c = -c;
  }
}

bool operator==(const Line& l1, const Line& l2) {
  return l1.a == l2.a && l1.b == l2.b && l1.c == l2.c;
};

std::ostream& operator<<(std::ostream& os, const Line& l) {
  os << l.a.to_double() << " " << l.b.to_double() << " " << l.c.to_double();
  return os;
}

Circle::Circle(const Point& p, const Point& q) : x0(p.x), y0(p.y) {
  QuadNum dx = q.x - p.x;
  QuadNum dy = q.y - p.y;
  r2 = dx * dx + dy * dy;
  if (sign(r2) == 0) throw std::invalid_argument("Degenerate circle");
}

bool operator==(const Circle& c1, const Circle& c2) {
  return c1.x0 == c2.x0 && c1.y0 == c2.y0 && c1.r2 == c2.r2;
};

std::ostream& operator<<(std::ostream& os, const Circle& c) {
  os << c.x0.to_double() << " " << c.y0.to_double() << " " << c.r2.to_double();
  return os;
}

vector<Point> ll_sol(const Line& line1, const Line& line2) {
  if (line1 == line2) throw std::invalid_argument("Same line");
  QuadNum a1 = line1.a, b1 = line1.b, c1 = line1.c;
  QuadNum a2 = line2.a, b2 = line2.b, c2 = line2.c;
  QuadNum det = a1 * b2 - a2 * b1;
  if (det == 0) return {};
  QuadNum x = (b2 * c1 - b1 * c2) / det;
  QuadNum y = (a1 * c2 - a2 * c1) / det;
  return {{x, y}};
}

vector<Point> lc_sol(const Line& line, const Circle& circle) {
  QuadNum a = line.a, b = line.b, c = line.c;
  QuadNum x0 = circle.x0, y0 = circle.y0, r2 = circle.r2;
  QuadNum dot = a * x0 + b * y0 - c;
  QuadNum norm2 = a * a + b * b;
  QuadNum d2 = dot * dot / norm2;
  QuadNum h2 = r2 - d2;
  if (h2 == -1) return {};
  QuadNum x_proj = x0 - a * dot / norm2;
  QuadNum y_proj = y0 - b * dot / norm2;
  if (h2 == 0) return {{x_proj, y_proj}};
  QuadNum h_norm = qsqrt(h2 / norm2);
  if (h_norm == -1) return {};
  Point p1 = {x_proj + b * h_norm, y_proj - a * h_norm};
  Point p2 = {x_proj - b * h_norm, y_proj + a * h_norm};
  return {p1, p2};
}

vector<Point> cc_sol(const Circle& circle1, const Circle& circle2) {
  if (circle1 == circle2) throw std::invalid_argument("Same circle");
  QuadNum x0_1 = circle1.x0, y0_1 = circle1.y0, r2_1 = circle1.r2;
  QuadNum x0_2 = circle2.x0, y0_2 = circle2.y0, r2_2 = circle2.r2;
  if (x0_1 == x0_2 && y0_1 == y0_2 && r2_1 != r2_2) return {};
  QuadNum dx = x0_2 - x0_1;
  QuadNum dy = y0_2 - y0_1;
  QuadNum d2 = dx * dx + dy * dy;
  QuadNum t = (r2_1 - r2_2 + d2) / (2 * d2);
  QuadNum h2 = r2_1 - t * t * d2;
  if (h2 == -1) return {};
  QuadNum x_mid = x0_1 + t * dx;
  QuadNum y_mid = y0_1 + t * dy;
  if (h2 == 0) return {{x_mid, y_mid}};
  QuadNum h_d = qsqrt(h2 / d2);
  if (h_d == -1) return {};
  Point p1 = {x_mid + dy * h_d, y_mid - dx * h_d};
  Point p2 = {x_mid - dy * h_d, y_mid + dx * h_d};
  return {p1, p2};
}

State::State(const vector<Point>& points, const vector<Line>& lines, const vector<Circle>& circles) :
  points(points), lines(lines), circles(circles) {}

vector<State> State::children() const {
  vector<State> children;
  vector<Point> pts(points.begin(), points.end());
  for (size_t i = 0; i < pts.size(); i++) {
    for (size_t j = 0; j < pts.size(); j++) {
      if (i == j) continue;
      Circle circle = {pts[i], pts[j]};
      if (!contains(circle)) {
        State child = *this;
        child.add(circle);
        children.push_back(child);
      }
      if (i > j) continue;
      Line line = {pts[i], pts[j]};
      if (!contains(line)) {
        State child = *this;
        child.add(line);
        children.push_back(child);
      }
    }
  }
  return children;
}

bool State::contains(const Point& point) const {
  return std::find(points.begin(), points.end(), point) != points.end();
}

bool State::contains(const Line& line) const {
  return std::find(lines.begin(), lines.end(), line) != lines.end();
}

bool State::contains(const Circle& circle) const {
  return std::find(circles.begin(), circles.end(), circle) != circles.end();
}

void State::add(const Point& point) {
  if (!contains(point)) points.push_back(point);
}

void State::add(const Line& line) {
  if (!contains(line)) {
    for (const Line& l : lines) {
      if (l == line) continue;
      for (const Point& p : ll_sol(line, l)) add(p);
    }
    for (const Circle& c : circles) {
      for (const Point& p : lc_sol(line, c)) add(p);
    }
    lines.push_back(line);
  }
}

void State::add(const Circle& circle) {
  if (!contains(circle)) {
    for (const Line& l : lines) {
      for (const Point& p : lc_sol(l, circle)) add(p);
    }
    for (const Circle& c : circles) {
      if (c == circle) continue;
      for (const Point& p : cc_sol(circle, c)) add(p);
    }
    circles.push_back(circle);
  }
}

std::size_t State::size() const {
  return lines.size() + circles.size();
}

Goal::Goal(const vector<Point>& points, const vector<Line>& lines, const vector<Circle>& circles, const vector<Segment>& segments) :
  State(points, lines, circles), segments(segments) {}

bool Goal::contained_in(const State& state) const {
  for (const Point& p : points) {
    if (!state.contains(p)) return false;
  }
  for (const Line& l : lines) {
    if (!state.contains(l)) return false;
  }
  for (const Circle& c : circles) {
    if (!state.contains(c)) return false;
  }
  for (const Segment& s : segments) {
    if (!state.contains(Line(s)) || !state.contains(s.p1) || !state.contains(s.p2)) return false;
  }
  return true;
}
