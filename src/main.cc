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
  std::queue<State> queue;
  queue.push(start);
  size_t i = 0;
  TimePoint start_time = std::chrono::steady_clock::now();
  double prev_time = 0;
  cout << "starting bfs..." << '\n';
  while (!queue.empty()) {
    State state = queue.front();
    queue.pop();
    i += 1;
    if (done(state, goal)) {
      TimePoint curr_time = std::chrono::steady_clock::now();
      double time = 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - start_time).count();
      cout << "solved! time: " << time << ", iter: " << i << ", steps: " << state.size() << '\n';
      cout << "cleaning up..." << '\n';
      return state;
    }
    for (State& child : children(state)) {
      if (!seen.contains(child)) {
        seen.insert(child);
        queue.push(child);
      }
    }
    TimePoint curr_time = std::chrono::steady_clock::now();
    double time = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
    if (time != prev_time) {
      cout << "time: " << time << ", iter: " << i << ", steps: " << state.size() << '\n';
      prev_time = time;
    }
  }
  return start;
}

void save(const State& start, const State& state, const Goal& goal) {
  std::ofstream file = std::ofstream("temp.txt");
  file << "start points" << '\n';
  for (const Point& p : start.points) {
    file << p << '\n';
  }
  file << '\n' << "start lines" << '\n';
  for (const Line& l : start.lines) {
    file << l << '\n';
  }
  file << '\n' << "start circles" << '\n';
  for (const Circle& c : start.circles) {
    file << c << '\n';
  }
  file << '\n' << "state points" << '\n';
  for (const Point& p : state.points) {
    file << p << '\n';
  }
  file << '\n' << "state lines" << '\n';
  for (const Line& l : state.lines) {
    file << l << '\n';
  }
  file << '\n' << "state circles" << '\n';
  for (const Circle& c : state.circles) {
    file << c << '\n';
  }
  file << '\n' << "goal points" << '\n';
  for (const Point& p : goal.points) {
    file << p << '\n';
  }
  file << '\n' << "goal lines" << '\n';
  for (const Line& l : goal.lines) {
    file << l << '\n';
  }
  file << '\n' << "goal circles" << '\n';
  for (const Circle& c : goal.circles) {
    file << c << '\n';
  }
  file << '\n' << "goal segments" << '\n';
  for (const Segment& s : goal.segments) {
    file << s << '\n';
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
  // Goal goal = {{
  //   {0, 1}, {1, 0}, {0, -1}, {-1, 0}  // points
  // }, {}, {}, {
  //   {{0, 1}, {1, 0}}, {{1, 0}, {0, -1}}, {{0, -1}, {-1, 0}}, {{-1, 0}, {0, 1}}  // segments
  // }};
  State state = bfs(start, goal);
  save(start, state, goal);
  cout << "done!" << '\n';
  std::system("conda run -n base python viz.py");
  return 0;
}
