#include "types.h"

PointSet ll_sol(Line& line1, Line& line2) {
  if (line1 == line2) throw std::invalid_argument("Same line");
  QuadNum a1 = line1.a, b1 = line1.b, c1 = line1.c;
  QuadNum a2 = line2.a, b2 = line2.b, c2 = line2.c;
  QuadNum det = a1 * b2 - a2 * b1;
  if (sign(det) == 0) return {};
  QuadNum x = (b2 * c1 - b1 * c2) / det;
  QuadNum y = (a1 * c2 - a2 * c1) / det;
  return {Point(x, y)};
}

PointSet lc_sol(Line& line, Circle& circle) {
  QuadNum a = line.a, b = line.b, c = line.c;
  QuadNum x0 = circle.x0, y0 = circle.y0, r2 = circle.r2;
  QuadNum dot = a * x0 + b * y0 - c;
  QuadNum norm2 = a * a + b * b;
  QuadNum d2 = dot * dot / norm2;
  QuadNum h2 = r2 - d2;
  if (sign(h2) == -1) return {};
  QuadNum x_proj = x0 - a * dot / norm2;
  QuadNum y_proj = y0 - b * dot / norm2;
  if (sign(h2) == 0) return {Point(x_proj, y_proj)};
  QuadNum h_norm = qsqrt(h2 / norm2);
  if (sign(h_norm) == -1) return {};
  Point p1 = Point(x_proj + b * h_norm, y_proj - a * h_norm);
  Point p2 = Point(x_proj - b * h_norm, y_proj + a * h_norm);
  return {p1, p2};
}

PointSet cc_sol(Circle& circle1, Circle& circle2) {
  if (circle1 == circle2) throw std::invalid_argument("Same circle");
  QuadNum x0_1 = circle1.x0, y0_1 = circle1.y0, r2_1 = circle1.r2;
  QuadNum x0_2 = circle2.x0, y0_2 = circle2.y0, r2_2 = circle2.r2;
  QuadNum dx = x0_2 - x0_1;
  QuadNum dy = y0_2 - y0_1;
  QuadNum d2 = dx * dx + dy * dy;
  QuadNum t = (r2_1 - r2_2 + d2) / (2 * d2);
  QuadNum h2 = r2_1 - t * t * d2;
  if (sign(h2) == -1) return {};
  QuadNum x_mid = x0_1 + t * dx;
  QuadNum y_mid = y0_1 + t * dy;
  if (sign(h2) == 0) return {Point(x_mid, y_mid)};
  QuadNum h_d = qsqrt(h2 / d2);
  if (sign(h_d) == -1) return {};
  Point p1 = Point(x_mid + dy * h_d, y_mid - dx * h_d);
  Point p2 = Point(x_mid - dy * h_d, y_mid + dx * h_d);
  return {p1, p2};
}

int main() {
  Line l1 = Line(Point(QuadNum(0), QuadNum(0)), Point(QuadNum(5), QuadNum(1)));
  Circle c1 = Circle(Point(QuadNum(2), QuadNum(1)), Point(QuadNum(-2), QuadNum(2)));
  PointSet points = lc_sol(l1, c1);
  for (const Point& p : points) {
    cout << p << endl;
  }
  return 0;
}
