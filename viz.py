import numpy as np
import matplotlib.pyplot as plt

BLACK = '#222222'
GREY = '#aaaaaa'
GOLD = '#f7b215'

def parse(filename):
  data = {
    'start points': [],
    'start lines': [],
    'start circles': [],
    'state points': [],
    'state lines': [],
    'state circles': [],
    'goal points': [],
    'goal lines': [],
    'goal circles': [],
    'goal segments': []
  }
  current = None
  with open(filename, 'r') as file:
    for line in file:
      line = line.strip()
      if line in data:
        current = data[line]
      elif line and current is not None:
        current.append(list(map(float, line.split())))
  return data

def line_endpoints(a, b, c):
  d = np.sqrt(a**2 + b**2)
  D = 1000
  x1 = a * c / d**2 + D * b / d
  y1 = b * c / d**2 - D * a / d
  x2 = a * c / d**2 - D * b / d
  y2 = b * c / d**2 + D * a / d
  return (x1, x2), (y1, y2)

data = parse('temp.txt')

plt.figure()
plt.axis('equal')
plt.axis([-4, 4, -4, 4])
plt.axis('off')
plt.box(False)
plt.tight_layout()

for x, y in data['start points']:
  plt.scatter(x, y, color=BLACK, s=5.1, zorder=1)

for a, b, c in data['start lines']:
  x, y = line_endpoints(a, b, c)
  plt.plot(x, y, color=BLACK, lw=1.1, zorder=1)

for x, y, r2 in data['start circles']:
  circle = plt.Circle((x, y), np.sqrt(r2), color=BLACK, fill=False, lw=1.1, zorder=1)
  plt.gca().add_artist(circle)

for x, y in data['state points']:
  plt.scatter(x, y, color=GREY, s=5, zorder=0)

for a, b, c in data['state lines']:
  x, y = line_endpoints(a, b, c)
  plt.plot(x, y, color=GREY, lw=1, zorder=0)

for x, y, r2 in data['state circles']:
  circle = plt.Circle((x, y), np.sqrt(r2), color=GREY, fill=False, lw=1, zorder=0)
  plt.gca().add_artist(circle)

for x, y in data['goal points']:
  plt.scatter(x, y, color=GOLD, s=5.2, zorder=2)

for a, b, c in data['goal lines']:
  x, y = line_endpoints(a, b, c)
  plt.plot(x, y, color=GOLD, lw=1.2, zorder=2)

for x, y, r2 in data['goal circles']:
  circle = plt.Circle((x, y), np.sqrt(r2), color=GOLD, fill=False, lw=1.2, zorder=2)
  plt.gca().add_artist(circle)

for x1, y1, x2, y2 in data['goal segments']:
  plt.plot([x1, x2], [y1, y2], color=GOLD, lw=1.2, zorder=2)

plt.show()
