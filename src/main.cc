#include "types.h"

vector<State> children(const State& state) {
  vector<State> children;
  vector<Point> points(state.points.begin(), state.points.end());
  for (size_t i = 0; i < points.size(); i++) {
    for (size_t j = 0; j < points.size(); j++) {
      if (i == j) continue;
      Circle circle = {points[i], points[j]};
      if (!state.contains(circle)) {
        State child = state;
        child.add(circle);
        children.push_back(child);
      }
      if (i > j) continue;
      Line line = {points[i], points[j]};
      if (!state.contains(line)) {
        State child = state;
        child.add(line);
        children.push_back(child);
      }
    }
  }
  return children;
}

bool done(const State& state, const Goal& goal) {
  for (const Point& p : goal.points) {
    if (!state.contains(p)) return false;
  }
  for (const Line& l : goal.lines) {
    if (!state.contains(l)) return false;
  }
  for (const Circle& c : goal.circles) {
    if (!state.contains(c)) return false;
  }
  for (const Segment& s : goal.segments) {
    if (!state.contains(Line(s)) || !state.contains(s.p1) || !state.contains(s.p2)) return false;
  }
  return true;
}

State bfs(const State& start, const Goal& goal) {
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

void save(const State& start, const State& state, const Goal& goal) {
  std::ofstream file = std::ofstream("temp.txt");
  file << "start points" << endl;
  for (const Point& p : start.points) {
    file << p << endl;
  }
  file << endl << "start lines" << endl;
  for (const Line& l : start.lines) {
    file << l << endl;
  }
  file << endl << "start circles" << endl;
  for (const Circle& c : start.circles) {
    file << c << endl;
  }
  file << endl << "state points" << endl;
  for (const Point& p : state.points) {
    file << p << endl;
  }
  file << endl << "state lines" << endl;
  for (const Line& l : state.lines) {
    file << l << endl;
  }
  file << endl << "state circles" << endl;
  for (const Circle& c : state.circles) {
    file << c << endl;
  }
  file << endl << "goal points" << endl;
  for (const Point& p : goal.points) {
    file << p << endl;
  }
  file << endl << "goal lines" << endl;
  for (const Line& l : goal.lines) {
    file << l << endl;
  }
  file << endl << "goal circles" << endl;
  for (const Circle& c : goal.circles) {
    file << c << endl;
  }
  file << endl << "goal segments" << endl;
  for (const Segment& s : goal.segments) {
    file << s << endl;
  }
}

int main() {
  State start = {{
    {0, 0}, {0, 1}  // points
  }, {}, {
    {{0, 0}, {0, 1}}  // circles
  }};
  Goal goal = {{
    {0, 1}, {{0, -1, 2}, {-1, 0, 2}}, {{0, 1, 2}, {-1, 0, 2}}  // points
  }, {}, {}, {
    {{0, 1}, {{0, -1, 2}, {-1, 0, 2}}}, {{{0, -1, 2}, {-1, 0, 2}}, {{0, 1, 2}, {-1, 0, 2}}}, {{{0, 1, 2}, {-1, 0, 2}}, {0, 1}}  // segments
  }};
  State state = bfs(start, goal);
  save(start, state, goal);
  return 0;
}
