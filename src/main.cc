#include "types.h"

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

vector<State> children(State& state) {
  vector<State> children;
  vector<Point> points(state.points.begin(), state.points.end());
  for (size_t i = 0; i < points.size(); i++) {
    for (size_t j = 0; j < points.size(); j++) {
      if (i == j) continue;
      Circle circle = {points[i], points[j]};
      if (!state.circles.contains(circle)) {
        State child = state;
        child.circles.insert(circle);
        for (const Line& l : state.lines) {
          for (const Point& p : lc_sol(l, circle)) {
            if (!child.points.contains(p)) child.points.insert(p);
          }
        }
        for (const Circle& c : state.circles) {
          for (const Point& p : cc_sol(c, circle)) {
            if (!child.points.contains(p)) child.points.insert(p);
          }
        }
        children.push_back(child);
      }
      if (i > j) continue;
      Line line = {points[i], points[j]};
      if (!state.lines.contains(line)) {
        State child = state;
        child.lines.insert(line);
        for (const Line& l : state.lines) {
          for (const Point& p : ll_sol(l, line)) {
            if (!child.points.contains(p)) child.points.insert(p);
          }
        }
        for (const Circle& c : state.circles) {
          for (const Point& p : lc_sol(line, c)) {
            if (!child.points.contains(p)) child.points.insert(p);
          }
        }
        children.push_back(child);
      }
    }
  }
  return children;
}

bool done(State& state, State& goal) {
  for (const Line& line : goal.lines) {
    if (!state.lines.contains(line)) return false;
  }
  for (const Circle& circle : goal.circles) {
    if (!state.circles.contains(circle)) return false;
  }
  return true;
}

State bfs(State& start, State& goal) {
  StateSet seen = {start};
  StateQueue pq;
  pq.push({start, start.size()});
  while (!pq.empty()) {
    State state = pq.top().state;
    pq.pop();
    cout << state.size() << endl;
    if (done(state, goal)) {
      return state;
    }
    for (State& child : children(state)) {
      if (!seen.contains(child)) {
        seen.insert(child);
        pq.push({child, child.size()});
      }
    }
  }
  return start;
}

void save(State& state) {
  std::ofstream file = std::ofstream("temp.txt");
  file << "Points:" << endl;
  for (const Point& point : state.points) {
    file << point << endl;
  }
  file << "Lines:" << endl;
  for (const Line& line : state.lines) {
    file << line << endl;
  }
  file << "Circles:" << endl;
  for (const Circle& circle : state.circles) {
    file << circle << endl;
  }
}

int main() {
  State start = {{
    {0, 0}, {0, 1}  // points
  }, {}, {
    {{0, 0}, {0, 1}}  // circles
  }};
  State goal = {{
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}  //points
  }, {
    {{0, 1}, {1, 0}}, {{1, 0}, {0, -1}}, {{0, -1}, {-1, 0}}, {{-1, 0}, {0, 1}}  // lines
  }, {}};
  State result = bfs(start, goal);
  save(result);
  return 0;
}
