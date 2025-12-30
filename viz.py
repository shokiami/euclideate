import numpy as np
import matplotlib.pyplot as plt

COLOR = {
  'start': '#222222',
  'state': '#888888',
  'goal': '#f7b215'
}

PLOT_SIZE = {
  'start': 15,
  'state': 10,
  'goal': 20
}

LINE_WIDTH = {
  'start': 0.5,
  'state': 1.5,
  'goal': 2
}

ZORDER = {
  'start': 1,
  'state': 0,
  'goal': 2
}

def parse(filename):
  sections = [
    'start points', 'start lines', 'start circles',
    'state points', 'state lines', 'state circles',
    'goal points',  'goal lines',  'goal circles', 'goal segments'
  ]
  data = {key: [] for key in sections}
  current = None
  with open(filename, 'r') as file:
    for line in file:
      line = line.strip()
      if line in data:
        current = data[line]
      elif line and current is not None:
        current.append(list(map(float, line.split())))
  return data

def line_endpoints(a, b, c, D=1000):
  d = np.sqrt(a**2 + b**2)
  x1 = a * c / d**2 + D * b / d
  y1 = b * c / d**2 - D * a / d
  x2 = a * c / d**2 - D * b / d
  y2 = b * c / d**2 + D * a / d
  return (x1, x2), (y1, y2)

def plot_points(points, color, size, zorder):
  for x, y in points:
    plt.scatter(x, y, color=color, s=size, zorder=zorder+len(ZORDER))
    plt.scatter(x, y, color='#ffffff', s=3, zorder=2*len(ZORDER))

def plot_lines(lines, color, lw, zorder):
  for a, b, c in lines:
    x, y = line_endpoints(a, b, c)
    plt.plot(x, y, color=color, lw=lw, zorder=zorder)

def plot_circles(circles, color, lw, zorder):
  for x, y, r2 in circles:
    circle = plt.Circle((x, y), np.sqrt(r2), color=color, fill=False, lw=lw, zorder=zorder)
    plt.gca().add_artist(circle)

def plot_segments(segments, color, lw, zorder):
  for x1, y1, x2, y2 in segments:
    plt.plot([x1, x2], [y1, y2], color=color, lw=lw, zorder=zorder)

def main():
  data = parse('temp.txt')

  plt.figure()
  plt.axis('equal')
  plt.axis([-4, 4, -4, 4])
  plt.axis('off')
  plt.box(False)
  plt.tight_layout()

  for phase in ('start', 'state', 'goal'):
    plot_points(data[f'{phase} points'], COLOR[phase], PLOT_SIZE[phase], zorder=ZORDER[phase])
    plot_lines(data[f'{phase} lines'], COLOR[phase], LINE_WIDTH[phase], zorder=ZORDER[phase])
    plot_circles(data[f'{phase} circles'], COLOR[phase], LINE_WIDTH[phase], zorder=ZORDER[phase])
  plot_segments(data['goal segments'], COLOR['goal'], LINE_WIDTH['goal'], ZORDER['goal'])

  plt.show()

if __name__ == "__main__":
  main()
