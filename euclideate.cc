// g++ euclideate.cc -o main -O3 -std=c++17

#include <iostream>
#include <math.h>
#include <vector>
#include <deque>
#include <tuple>
#include <string>
#include <fstream>

using std::vector;
using std::string;
using std::cout;
using std::endl;

constexpr double EPS = 1e-5;

bool equal(double a, double b) {
  return std::abs(a - b) <= EPS;
}

bool less_than(double a, double b) {
  return a < b - EPS;
}

class Point {
  public:
  vector<double> data;
  Point(std::initializer_list<double> init_list) : data(init_list) {}
  
  double& operator[](size_t index) {
    return data[index];
  }

  bool operator==(const Point& other) const {
    for (size_t i = 0; i < data.size(); i++) {
      if (!equal(data[i], other.data[i])) {
        return false;
      }
    }
    return true;
  }

  bool operator<(const Point& other) const {
    for (size_t i = 0; i < data.size(); i++) {
      if (less_than(data[i], other.data[i])) {
        return true;
      } else if (less_than(other.data[i], data[i])) {
        return false;
      }
    }
    return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const Point& point) {
    for (size_t i = 0; i < point.data.size(); i++) {
      if (i > 0) os << " ";
      os << point.data[i];
    }
    return os;
  }
};

class State {
  public:
  vector<Point> circles;
  vector<Point> lines;
  vector<Point> points;
  State() : circles{{0.0, 0.0, 1.0}}, points{{0.0, 0.0}, {1.0, 0.0}} {}

  size_t size() {
    return circles.size() + lines.size();
  }

  bool operator==(const State& other) const {
    return circles == other.circles && lines == other.lines && points == other.points;
  }

  bool operator<(const State& other) const {
    if (circles < other.circles) return true;
    if (circles > other.circles) return false;
    if (lines < other.lines) return true;
    if (lines > other.lines) return false;
    if (points < other.points) return true;
    if (points > other.points) return false;
    return false;
  }
};

Point circle_eq(double x1, double y1, double x2, double y2) {
  double r = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
  return {x1, y1, r};
}

Point line_eq(double x1, double y1, double x2, double y2) {
  double a = y2 - y1;
  double b = x1 - x2;
  double c = x2 * y1 - x1 * y2;
  double d = std::sqrt(a * a + b * b);
  a /= d; b /= d; c /= d;
  if (a < 0 || (a == 0 && b < 0)) {
    a = -a; b = -b; c = -c;
  }
  return {a, b, c};
}

template<typename T>
bool contains(vector<T>& vec, T& value) {
  auto it = std::lower_bound(vec.begin(), vec.end(), value);
  return it != vec.end() && *it == value;
}

template<typename T>
bool insert(vector<T>& vec, T& value) {
  auto it = std::lower_bound(vec.begin(), vec.end(), value);
  bool contained = (it != vec.end() && *it == value);
  if (!contained) {
    vec.insert(it, value);
  }
  return contained;
}

vector<Point> l_l_sol(Point& line1, Point& line2) {
  double a1 = line1[0], b1 = line1[1], c1 = line1[2];
  double a2 = line2[0], b2 = line2[1], c2 = line2[2];
  double D = a1 * b2 - a2 * b1;
  if (equal(D, 0.0)) {
    return {};
  }
  double x = (b1 * c2 - b2 * c1) / D;
  double y = (a1 * c2 - a2 * c1) / D;
  return {{x, y}};
}

vector<Point> l_c_sol(Point& line1, Point& circle1) {
  double a1 = line1[0], b1 = line1[1], c1 = line1[2];
  double x1 = circle1[0], y1 = circle1[1], r1 = circle1[2];
  if (b1 != 0) {
    double t1 = a1 / b1;
    double t2 = c1 / b1 + y1;
    double A = 1 + t1 * t1;
    double B = -2 * x1 + 2 * t1 * t2;
    double C = x1 * x1 + t2 * t2 - r1 * r1;
    double D = B * B - 4 * A * C;
    if (less_than(0.0, D)) {
      double x_res1 = (-B + std::sqrt(D)) / (2 * A);
      double x_res2 = (-B - std::sqrt(D)) / (2 * A);
      double y_res1 = (-a1 * x_res1 - c1) / b1;
      double y_res2 = (-a1 * x_res2 - c1) / b1;
      return {{x_res1, y_res1}, {x_res2, y_res2}};
    } else if (equal(D, 0.0)) {
      double x = -B / (2 * A);
      double y = (-a1 * x - c1) / b1;
      return {{x, y}};
    }
  } else {
    double x = -c1 / a1;
    double D = r1 * r1 - (x - x1) * (x - x1);
    if (less_than(0.0, D)) {
      return {{x, y1 + std::sqrt(D)}, {x, y1 - std::sqrt(D)}};
    } else if (equal(D, 0.0)) {
      return {{x, y1}};
    }
  }
  return {};
}

vector<Point> c_c_sol(Point& circle1, Point& circle2) {
  double x1 = circle1[0], y1 = circle1[1], r1 = circle1[2];
  double x2 = circle2[0], y2 = circle2[1], r2 = circle2[2];
  double d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
  
  if (less_than(r1 + r2, d) || less_than(d, std::fabs(r1 - r2))) {
    return {};
  } else if (equal(d, 0.0) && equal(r1, r2)) {
    return {};
  } else if (equal(d, r1 + r2) || equal(d, std::fabs(r1 - r2))) {
    double a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
    double x_res = x1 + a * (x2 - x1) / d;
    double y_res = y1 + a * (y2 - y1) / d;
    return {{x_res, y_res}};
  }
  
  double a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
  double h = std::sqrt(r1 * r1 - a * a);
  double x = x1 + a * (x2 - x1) / d;
  double y = y1 + a * (y2 - y1) / d;
  double x_res1 = x + h * (y2 - y1) / d;
  double y_res1 = y - h * (x2 - x1) / d;
  double x_res2 = x - h * (y2 - y1) / d;
  double y_res2 = y + h * (x2 - x1) / d;
  
  return {{x_res1, y_res1}, {x_res2, y_res2}};
}

vector<State> get_children(State& state, vector<State>& seen) {
  vector<State> children;
  for (size_t i = 0; i < state.points.size(); i++) {
    for (size_t j = 0; j < state.points.size(); j++) {
      if (i == j) continue;

      double x1 = state.points[i][0], y1 = state.points[i][1];
      double x2 = state.points[j][0], y2 = state.points[j][1];

      Point circle1 = circle_eq(x1, y1, x2, y2);
      State child1 = state;
      if (!insert(child1.circles, circle1)) {
        if (!insert(seen, child1)) {
          for (Point circle2 : state.circles) {
            for (Point point : c_c_sol(circle1, circle2)) {
              insert(child1.points, point);
            }
          }
          for (Point line1 : state.lines) {
            for (Point point : l_c_sol(line1, circle1)) {
              insert(child1.points, point);
            }
          }
          children.push_back(child1);
        }
      }

      if (i > j) continue;

      Point line1 = line_eq(x1, y1, x2, y2);
      State child2 = state;
      if (!insert(child2.lines, line1)) {
        if (!insert(seen, child2)) {
          for (Point line2 : state.lines) {
            for (Point point : l_l_sol(line1, line2)) {
              insert(child2.points, point);
            }
          }
          for (Point circle1 : state.circles) {
            for (Point point : l_c_sol(line1, circle1)) {
              insert(child2.points, point);
            }
          }
          children.push_back(child2);
        }
      }
    }
  }
  return children;
}

State goal_state() {
  int n = 3;
  double theta = 2 * M_PI / n;
  State goal_state;
  for (int i = 0; i < n; i++) {
    double x1 = std::cos(i * theta), y1 = std::sin(i * theta);
    double x2 = std::cos((i + 1) * theta), y2 = std::sin((i + 1) * theta);
    Point line = line_eq(x1, y1, x2, y2);

    if (!contains(goal_state.lines, line)) {
      goal_state.lines.push_back(line);
    }
  }
  return goal_state;
}

bool done(State& current, State& goal) {
  for (Point& line : goal.lines) {
    if (!contains(current.lines, line)) return false;
  }
  for (Point& circle : goal.circles) {
    if (!contains(current.circles, circle)) return false;
  }
  return true;
}

State bfs(State& start, State& goal) {
  std::deque<State> queue;
  queue.push_back(start);
  vector<State> seen = {start};

  int i = 0;

  while (!queue.empty()) {
    State state = queue.front();
    queue.pop_front();
    cout << state.size() << " " << state.points.size() << endl;

    if (state.size() == 4 && state.points.size() == 8) {
      i++;
    }

    if (i == 9) {
      std::cout << "done!" << std::endl;
      return state;
    }

    if (done(state, goal)) {
      std::cout << "done!" << std::endl;
      return state;
    }

    for (State& child : get_children(state, seen)) {
      queue.push_back(child);
    }
  }
  return start;
}

void save(State& state) {
  std::ofstream file = std::ofstream("temp.txt");
  file << "Circles:" << endl;
  for (Point circle : state.circles) {
    file << circle << endl;
  }
  file << "Lines:" << endl;
  for (Point line : state.lines) {
    file << line << endl;
  }
  file << "Points:" << endl;
  for (Point point : state.points) {
    file << point << endl;
  }
}

int main() {
  State start;
  State goal = goal_state();
  State result = bfs(start, goal);
  save(result);
  return 0;
}
