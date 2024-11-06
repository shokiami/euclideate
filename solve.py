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
  goal = state()
  theta = 2 * np.pi / n
  # for i in range(n):
  #   x1, y1 = np.cos(i * theta), np.sin(i * theta)
  #   x2, y2 = np.cos((i + 1) * theta), np.sin((i + 1) * theta)
  #   line = (y2 - y1, x1 - x2, x2 * y1 - x1 * y2)
  #   goal.lines.add(line)
  x1, y1 = (1, 0)
  x2, y2 = np.cos(theta), np.sin(theta)
  line = line_eq(x1, y1, x2, y2)
  goal.lines.append(line)
  return goal

def bfs(start, goal):
  queue = deque([start])
  visited = [start]
  while queue:
    state = queue.popleft()
    # viz(state)
    print(len(state.circles) + len(state.lines))
    if done(state, goal):
      return state
    for child in get_children(state, visited):
      queue.append(child)

def done(state, goal):
  for line in goal.lines:
    if not contains(state.lines, line):
      return False
  for circle in goal.circles:
    if not contains(state.circles, circle):
      return False
  return True

def get_children(state, visited):
  children = []
  for i in range(len(state.points)):
     for j in range(len(state.points)):
        if i == j:
          continue

        x1, y1 = state.points[i]
        x2, y2 = state.points[j]

        if i < j:
          line1 = line_eq(x1, y1, x2, y2)
          if not contains(state.lines, line1):
            child = deepcopy(state)
            child.lines.append(line1)
            if not state_contains(visited, child):
              for line2 in state.lines:
                child.points = union(child.points, l_l_sol(line1, line2))
              for circle1 in state.circles:
                child.points = union(child.points, l_c_sol(line1, circle1))
              children.append(child)
              visited.append(child)

        circle1 = circle_eq(x1, y1, x2, y2)
        if not contains(state.circles, circle1):
          child = deepcopy(state)
          child.circles.append(circle1)
          if not state_contains(visited, child):
            for line1 in state.lines:
              child.points = union(child.points, l_c_sol(line1, circle1))
            for circle2 in state.circles:
              child.points = union(child.points, c_c_sol(circle1, circle2))
            children.append(child)
            visited.append(child)
  return children

def state_contains(s, x):
  contained = False
  for y in s:
    if equal(x.lines, y.lines) and equal(x.circles, y.circles):
      contained = True
      break
  return contained

def contains(s, x):
  contained = False
  for y in s:
    if equal(x, y):
      contained = True
      break
  return contained

def union(s1, s2):
  res = s1
  for x in s2:
    if not contains(res, x):
      res.append(x)
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
  if equal(a1 * b2 - a2 * b1, 0.0):
    return []
  if b1 != 0.0:
    x = (b1 * c2 - b2 * c1) / (a1 * b2 - a2 * b1)
    y = (c1 - a1 * x) / b1
  else:
    y = (a2 * c1 - a1 * c2) / (b2 * a1)
    x = (c1 - b1 * y) / a1
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
      # Two intersection points
      x1 = (-B + np.sqrt(D)) / (2 * A)
      x2 = (-B - np.sqrt(D)) / (2 * A)
      y1 = (-a1 * x1 - c1) / b1
      y2 = (-a1 * x2 - c1) / b1
      return [(x1, y1), (x2, y2)]
    elif equal(D, 0.0):
      x = -B / (2 * A)
      y = (-a1 * x - c1) / b1
      return [(x, y)]
  else:
    x = -c1 / a1
    delta = r1**2 - (x - x1)**2
    if delta > EPS:
        y1 = y1 + np.sqrt(delta)
        y2 = y1 - np.sqrt(delta)
        return [(x, y1), (x, y2)]
    elif equal(delta, 0.0):
      y = y1
      return [(x, y)]
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
      x = x1 + a * (x2 - x1) / d
      y = y1 + a * (y2 - y1) / d
      return [(x, y)]
  a = (r1**2 - r2**2 + d**2) / (2 * d)
  h = (r1**2 - a**2) ** 0.5
  x = x1 + a * (x2 - x1) / d
  y = y1 + a * (y2 - y1) / d

  # Calculate the intersection points
  x_res1 = x + h * (y2 - y1) / d
  y_res1 = y - h * (x2 - x1) / d
  x_res2 = x - h * (y2 - y1) / d
  y_res2 = y + h * (x2 - x1) / d

  return [(x_res1, y_res1), (x_res2, y_res2)]

def viz(state):
  plt.figure()
  plt.axis('equal')
  plt.xlim(-1.5, 1.5)
  plt.ylim(-1.5, 1.5)
  plt.axis('off')
  plt.box(False)
  plt.tight_layout()

  for a, b, c in state.lines:
    d = np.sqrt(a**2 + b**2)
    x1 = -a * c / d**2 + 1000 * b / d
    y1 = -b * c / d**2 - 1000 * a / d
    x2 = -a * c / d**2 - 1000 * b / d
    y2 = -b * c / d**2 + 1000 * a / d
    plt.plot([x1, x2], [y1, y2], color='black', lw=1)

  for x, y, r in state.circles:
    circle = plt.Circle((x, y), r, color='black', fill=False, lw=1)
    plt.gca().add_artist(circle)

  plt.show()

if __name__ == '__main__':
  start = state()
  end = goal_state()
  state = bfs(start, end)
  viz(state)
