#include "types.h"

double time(TimePoint start_time) {
  TimePoint curr_time = std::chrono::steady_clock::now();
  return 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - start_time).count();
}

State iddfs(const State& start, const Goal& goal) {
  TimePoint start_time = std::chrono::steady_clock::now();
  size_t depth = difference(start, goal).size();
  size_t i = 0;
  while (true) {
    std::stack<State> stack;
    stack.push(start);
    while (!stack.empty()) {
      State state = stack.top();
      stack.pop();
      i += 1;
      if (state.contains_points(goal)) {
        state.merge(difference(state, goal));
        cout << "done! depth: " << depth << ", iter: " << i << ", time: " << time(start_time) << '\n';
        return state;
      }
      for (State& child : state.children()) {
        State diff = difference(child, goal);
        if (child.size() - start.size() + diff.size() <= depth) stack.push(child);
      }
    }
    cout << "depth: " << depth << ", iter: " << i << ", time: " << time(start_time) << '\n';
    depth += 1;
  }
}

void save(const State& start, const State& state, const Goal& goal) {
  std::ofstream file = std::ofstream("temp.txt");
  file << "start points" << '\n';
  for (const Point& p : start.points) file << p << '\n';
  file << '\n' << "start lines" << '\n';
  for (const Line& l : start.lines) file << l << '\n';
  file << '\n' << "start circles" << '\n';
  for (const Circle& c : start.circles) file << c << '\n';
  file << '\n' << "state points" << '\n';
  for (const Point& p : state.points) file << p << '\n';
  file << '\n' << "state lines" << '\n';
  for (const Line& l : state.lines) file << l << '\n';
  file << '\n' << "state circles" << '\n';
  for (const Circle& c : state.circles) file << c << '\n';
  file << '\n' << "goal points" << '\n';
  for (const Point& p : goal.points) file << p << '\n';
  file << '\n' << "goal lines" << '\n';
  for (const Line& l : goal.lines) file << l << '\n';
  file << '\n' << "goal circles" << '\n';
  for (const Circle& c : goal.circles) file << c << '\n';
  file << '\n' << "goal segments" << '\n';
  for (const Segment& s : goal.segments) file << s << '\n';
}

int main() {
  State start = {{
    {0, 0}, {0, 1}  // points
  }, {}, {
    {{0, 0}, {0, 1}}  // circles
  }};
  Goal goal = {{}, {}, {}, {
    {{0, 1}, {1, 0}}, {{1, 0}, {0, -1}}, {{0, -1}, {-1, 0}}, {{-1, 0}, {0, 1}}  // segments
  }};
  cout << "starting search..." << '\n';
  State state = iddfs(start, goal);
  save(start, state, goal);
  std::system("conda run -n base python viz.py");
  return 0;
}
