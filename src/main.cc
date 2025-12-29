#include "types.h"

vector<State> children(State& state) {
  vector<State> children;
  vector<Point> points(state.points.begin(), state.points.end());
  for (size_t i = 0; i < points.size(); i++) {
    for (size_t j = 0; j < points.size(); j++) {
      if (i == j) continue;
      Circle circle = {points[i], points[j]};
      if (!state.contains_circle(circle)) {
        State child = state;
        child.add_circle(circle);
        children.push_back(child);
      }
      if (i > j) continue;
      Line line = {points[i], points[j]};
      if (!state.contains_line(line)) {
        State child = state;
        child.add_line(line);
        children.push_back(child);
      }
    }
  }
  return children;
}

bool done(State& state, State& goal) {
  for (const Line& line : goal.lines) {
    if (!state.contains_line(line)) return false;
  }
  for (const Circle& circle : goal.circles) {
    if (!state.contains_circle(circle)) return false;
  }
  return true;
}

State bfs(State& start, State& goal) {
  StateSet seen = {start};
  StateQueue pq;
  pq.push({start, start.size()});
  size_t i = 0;
  while (!pq.empty()) {
    State state = pq.top().state;
    pq.pop();
    cout << ++i << ": " << state.size() << endl;
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
