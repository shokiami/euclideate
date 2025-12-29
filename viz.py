import numpy as np
import matplotlib.pyplot as plt

def parse(filename):
  data = ([], [], [])
  with open(filename, 'r') as file:
    for line in file:
      line = line.strip()
      if line == "Points:":
        i = 0
      elif line == "Lines:":
        i = 1
      elif line == "Circles:":
        i = 2
      elif line:
        data[i].append(list(map(float, line.split())))
  return data

points, lines, circles = parse('temp.txt')

plt.figure()
plt.axis('equal')
plt.axis([-4, 4, -4, 4])
plt.axis('off')
plt.box(False)
plt.tight_layout()

for a, b, c in lines:
  d = np.sqrt(a**2 + b**2)
  D = 1000
  x1 = a * c / d**2 + D * b / d
  y1 = b * c / d**2 - D * a / d
  x2 = a * c / d**2 - D * b / d
  y2 = b * c / d**2 + D * a / d
  plt.plot([x1, x2], [y1, y2], color='#000000', lw=1)

for x, y, r2 in circles:
  circle = plt.Circle((x, y), np.sqrt(r2), color='#000000', fill=False, lw=1)
  plt.gca().add_artist(circle)

for x, y in points:
  plt.scatter(x, y, color='#000000', s=5)

# for x1, y1, x2, y2 in goal:
#   plt.plot([x1, x2], [y1, y2], color='#ffcc00', zorder=2, lw=1.5)
#   plt.scatter(x1, y1, color='#ffcc00', zorder=2, s=5.5)
#   plt.scatter(x2, y2, color='#ffcc00', zorder=2, s=5.5)

plt.show()
