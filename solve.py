import numpy as np
import matplotlib.pyplot as plt
from collections import deque
from copy import deepcopy
import sympy as sp
import math

EPS = 1e-5

class state:
  def __init__(self):
    self.circles = [(0.0, 0.0, 1.0)]
    self.lines = []
    self.points = [(0.0, 0.0), (1.0, 0.0)]

def goal_state():
  n = 6
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
  d = math.sqrt(a**2 + b**2)
  a /= d
  b /= d
  c /= d
  if a < 0 or (a == 0 and b < 0):
    a = -a
    b = -b
    c = -c
  return a, b, c

def circle_eq(x1, y1, x2, y2):
  return x1, y1, math.sqrt((x2 - x1)**2 + (y2 - y1)**2)

def equal(a, b):
  a = np.asarray(a)
  b = np.asarray(b)
  if a.shape != b.shape:
    return False
  return np.all(np.abs(a - b) <= EPS)

def l_l_sol(line1, line2):
  a1, b1, c1 = line1
  a2, b2, c2 = line2
  x, y = sp.symbols('x y', real=True)
  eq1 = a1 * x + b1 * y + c1
  eq2 = a2 * x + b2 * y + c2
  sol = sp.solve([eq1, eq2], (x, y))
  if len(sol) == 0:
    return []
  return [(sol[x], sol[y])]

def l_c_sol(line1, circle1):
  a1, b1, c1 = line1
  x1, y1, r1 = circle1
  x, y = sp.symbols('x y', real=True)
  eq1 = a1 * x + b1 * y + c1
  eq2 = (x - x1)**2 + (y - y1)**2 - r1**2
  sol = sp.solve([eq1, eq2], (x, y))
  return sol

def c_c_sol(circle1, circle2):
  x1, y1, r1 = circle1
  x2, y2, r2 = circle2
  x, y = sp.symbols('x y', real=True)
  eq1 = (x - x1)**2 + (y - y1)**2 - r1**2
  eq2 = (x - x2)**2 + (y - y2)**2 - r2**2
  sol = sp.solve([eq1, eq2], (x, y))
  return sol

def viz(state):
  plt.figure()
  plt.axis('equal')
  plt.xlim(-1.5, 1.5)
  plt.ylim(-1.5, 1.5)
  plt.axis('off')
  plt.box(False)
  plt.tight_layout()

  for a, b, c in state.lines:
    d = math.sqrt(a**2 + b**2)
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
