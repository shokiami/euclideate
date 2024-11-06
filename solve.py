import numpy as np
import matplotlib.pyplot as plt
from collections import deque
from copy import deepcopy

EPS = 1e-5

class state:
  def __init__(self):
    self.circles = [(0.0, 0.0, 1.0)]
    self.lines = []
    self.points = [(0.0, 0.0), (1.0, 0.0)]

def goal_state():
  n = 3
  theta = 2 * np.pi / n
  goal_state = state()
  goal = []
  for i in range(n):
    x1, y1 = np.cos(i * theta), np.sin(i * theta)
    x2, y2 = np.cos((i + 1) * theta), np.sin((i + 1) * theta)
    line = line_eq(x1, y1, x2, y2)
    _, idx = contains(goal_state.lines, line)
    goal_state.lines.insert(idx, line)
    goal.append([x1, y1, x2, y2])
  return goal_state, goal

def bfs(start, goal):
  queue = deque([start])
  seen = [start]
  while queue:
    state = queue.popleft()
    print(len(state.circles) + len(state.lines), len(state.points))
    # viz(state)
    if done(state, goal):
      print('done!')
      return state
    for child in get_children(state, seen):
      queue.append(child)

def done(state, goal):
  for line in goal.lines:
    if not contains(state.lines, line)[0]:
      return False
  for circle in goal.circles:
    if not contains(state.circles, circle)[0]:
      return False
  return True

def get_children(state, seen):
  children = []
  for i in range(len(state.points)):
     for j in range(len(state.points)):
        if i == j:
          continue

        x1, y1 = state.points[i]
        x2, y2 = state.points[j]

        if i < j:
          line1 = line_eq(x1, y1, x2, y2)
          contained, idx = contains(state.lines, line1)
          if not contained:
            child = deepcopy(state)
            child.lines.insert(idx, line1)
            contained, idx = visited(seen, child)
            if not contained:
              for line2 in state.lines:
                child.points = union(child.points, l_l_sol(line1, line2))
              for circle1 in state.circles:
                child.points = union(child.points, l_c_sol(line1, circle1))
              children.append(child)
              seen.insert(idx, child)

        circle1 = circle_eq(x1, y1, x2, y2)
        contained, idx = contains(state.circles, circle1)
        if not contained:
          child = deepcopy(state)
          child.circles.insert(idx, circle1)
          contained, idx = visited(seen, child)
          if not contained:
            for line1 in state.lines:
              child.points = union(child.points, l_c_sol(line1, circle1))
            for circle2 in state.circles:
              child.points = union(child.points, c_c_sol(circle1, circle2))
            children.append(child)
            seen.insert(idx, child)
  return children

def less_than(a, b):
  a = np.asarray(a)
  b = np.asarray(b)
  for i in range(len(a)):
    if a[i] < b[i] - EPS:
      return True
    elif a[i] > b[i] + EPS:
      return False
  return False

def greater_than(a, b):
  a = np.asarray(a)
  b = np.asarray(b)
  return less_than(-a, -b)

def visited(arr, x):
  def eq_func(a, b):
    return equal(a.lines, b.lines) and equal(a.circles, b.circles)
  def lt_func(a, b):
    if len(a.lines) < len(b.lines):
      return True
    elif len(a.lines) > len(b.lines):
      return False
    else:
      if len(a.circles) < len(b.circles):
        return True
      elif len(a.circles) > len(b.circles):
        return False
      else:
        a_arr = a.lines + a.circles
        b_arr = b.lines + b.circles
        for i in range(len(a_arr)):
          if less_than(a_arr[i], b_arr[i]):
            return True
          elif greater_than(a_arr[i], b_arr[i]):
            return False
    return False
  return binary_search(arr, x, eq_func, lt_func)

def contains(arr, x):
  return binary_search(arr, x, equal, less_than)

def binary_search(arr, x, eq_func, lt_func):
  lo = 0
  hi = len(arr) - 1
  while lo <= hi:
    mid = (lo + hi) // 2
    if eq_func(arr[mid], x):
      return True, mid
    elif lt_func(arr[mid], x):
      lo = mid + 1
    else:
      hi = mid - 1
  return False, lo

def union(s1, s2):
  res = s1
  for x in s2:
    contained, idx = contains(res, x)
    if not contained:
      res.insert(idx, x)
  return res

def line_eq(x1, y1, x2, y2):
  a, b, c = y2 - y1, x1 - x2, x2 * y1 - x1 * y2
  d = np.sqrt(a**2 + b**2)
  a /= d
  b /= d
  c /= d
  if a < 0 or (a == 0 and b < 0):
    a = -a
    b = -b
    c = -c
  return a, b, c

def circle_eq(x1, y1, x2, y2):
  return x1, y1, np.sqrt((x2 - x1)**2 + (y2 - y1)**2)

def equal(a, b):
  a = np.asarray(a)
  b = np.asarray(b)
  if a.shape != b.shape:
    return False
  return np.all(np.abs(a - b) <= EPS)

def l_l_sol(line1, line2):
  a1, b1, c1 = line1
  a2, b2, c2 = line2
  D = a1 * b2 - a2 * b1
  if equal(D, 0.0):
    return []
  x = (b1 * c2 - b2 * c1) / D
  y = (a1 * c2 - a2 * c1) / D
  return [(x, y)]

def l_c_sol(line1, circle1):
  a1, b1, c1 = line1
  x1, y1, r1 = circle1
  if b1 != 0:
    A = 1 + (a1 / b1) ** 2
    B = -2 * x1 + 2 * (a1 / b1) * (c1 / b1 + y1)
    C = x1**2 + (c1 / b1 + y1)**2 - r1**2
    D = B**2 - 4 * A * C
    if D > EPS:
      x_res1 = (-B + np.sqrt(D)) / (2 * A)
      x_res2 = (-B - np.sqrt(D)) / (2 * A)
      y_res1 = (-a1 * x_res1 - c1) / b1
      y_res2 = (-a1 * x_res2 - c1) / b1
      return [(x_res1, y_res1), (x_res2, y_res2)]
    elif equal(D, 0.0):
      x = -B / (2 * A)
      y = (-a1 * x - c1) / b1
      return [(x, y)]
  else:
    x = -c1 / a1
    D = r1**2 - (x - x1)**2
    if D > EPS:
      return [(x, y1 + np.sqrt(D)), (x, y1 - np.sqrt(D))]
    elif equal(D, 0.0):
      return [(x, y1)]
  return []

def c_c_sol(circle1, circle2):
  x1, y1, r1 = circle1
  x2, y2, r2 = circle2
  d = ((x2 - x1)**2 + (y2 - y1)**2)**0.5
  if d > r1 + r2 + EPS or d < np.abs(r1 - r2) - EPS:
    return []
  elif equal(d, 0.0) and equal(r1, r2):
    return []
  elif equal(d, r1 + r2) or equal(d, np.abs(r1 - r2)):
      a = (r1**2 - r2**2 + d**2) / (2 * d)
      x_res = x1 + a * (x2 - x1) / d
      y_res = y1 + a * (y2 - y1) / d
      return [(x_res, y_res)]
  a = (r1**2 - r2**2 + d**2) / (2 * d)
  h = (r1**2 - a**2) ** 0.5
  x = x1 + a * (x2 - x1) / d
  y = y1 + a * (y2 - y1) / d
  x_res1 = x + h * (y2 - y1) / d
  y_res1 = y - h * (x2 - x1) / d
  x_res2 = x - h * (y2 - y1) / d
  y_res2 = y + h * (x2 - x1) / d
  return [(x_res1, y_res1), (x_res2, y_res2)]

def viz(state, goal=[]):
  plt.figure()
  plt.axis('equal')
  plt.xlim(-2.5, 2.5)
  plt.ylim(-3.0, 3.0)
  plt.axis('off')
  plt.box(False)
  plt.tight_layout()

  for a, b, c in state.lines:
    d = np.sqrt(a**2 + b**2)
    x1 = -a * c / d**2 + 1000 * b / d
    y1 = -b * c / d**2 - 1000 * a / d
    x2 = -a * c / d**2 - 1000 * b / d
    y2 = -b * c / d**2 + 1000 * a / d
    plt.plot([x1, x2], [y1, y2], color='#000000', lw=1)

  for x, y, r in state.circles:
    circle = plt.Circle((x, y), r, color='#000000', fill=False, lw=1)
    plt.gca().add_artist(circle)

  for x, y in state.points:
    plt.scatter(x, y, color='#000000', s=5)

  for x1, y1, x2, y2 in goal:
    plt.plot([x1, x2], [y1, y2], color='#ffcc00', zorder=2, lw=1.5)
    plt.scatter(x1, y1, color='#ffcc00', zorder=2, s=5.5)
    plt.scatter(x2, y2, color='#ffcc00', zorder=2, s=5.5)

  plt.show()

if __name__ == '__main__':
  start = state()
  end, goal = goal_state()
  state = bfs(start, end)
  viz(state, goal)
