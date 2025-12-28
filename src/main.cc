#include "types.h"

PointSet l_l_sol(Line& line1, Line& line2) {
  if (line1 == line2) throw std::invalid_argument("Same line");
  QuadNum a1 = line1.a, b1 = line1.b, c1 = line1.c;
  QuadNum a2 = line2.a, b2 = line2.b, c2 = line2.c;
  QuadNum det = a1 * b2 - a2 * b1;
  if (sign(det) == 0) return {};
  QuadNum x = (b2 * c1 - b1 * c2) / det;
  QuadNum y = (a1 * c2 - a2 * c1) / det;
  return {Point(x, y)};
}

PointSet l_c_sol(Line& line, Circle& circle) {
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

int main() {
  cout << qsqrt(x) << endl;  // 1 1 2
  return 0;
}
