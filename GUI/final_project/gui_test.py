# from PySide6 import QtWidgets
# import pyqtgraph as pg
# import sys

# app = QtWidgets.QApplication([])

# plot = pg.PlotWidget()
# plot.show()

# sys.exit(app.exec())

##########################################################
# app = QtWidgets.QApplication(sys.argv)

# # Create the window / plot
# plot = pg.PlotWidget(title="Robot Field Test")
# plot.setXRange(0, 10)
# plot.setYRange(0, 10)
# plot.setLabel('left', 'Y Axis')
# plot.setLabel('bottom', 'X Axis')

# # Create ONE object (a red dot)
# object_item = pg.ScatterPlotItem()

# # Object position (x, y)
# x = [5]
# y = [7]

# object_item.setData(
#     x=x,
#     y=y,
#     size=15,
#     brush='r'   # red color
# )

# # Add to plot
# plot.addItem(object_item)

# plot.show()

# sys.exit(app.exec())
############################################################
#            MOCK LIVE OBJECT PLACMENT                     #
############################################################
# import sys
# import random
# from PySide6 import QtWidgets, QtCore
# import pyqtgraph as pg


# app = QtWidgets.QApplication(sys.argv)

# plot = pg.PlotWidget(title="Live Object Test")
# plot.setXRange(0, 10)
# plot.setYRange(0, 10)

# object_item = pg.ScatterPlotItem()
# plot.addItem(object_item)

# x, y = 5, 5  # starting position
# size = 15 # starting size


# def update():
#     global x, y, size

#     # simulate movement (replace this with socket data later)
#     x += random.uniform(-0.3, 0.3)
#     y += random.uniform(-0.3, 0.3)
#     size = random.uniform(15,30)

#     object_item.setData(
#         x=[x],
#         y=[y],
#         size=[size],
#         brush='r'
#     )


# # Timer = runs update() repeatedly
# timer = QtCore.QTimer()
# timer.timeout.connect(update)
# timer.start(100)  # update every 100 ms (10 FPS)

# plot.show()
# sys.exit(app.exec())

#################################################################
#                    MOCK ROBOT ORIENTATON                      #
#################################################################
import sys
import math
import random
from PySide6 import QtWidgets, QtCore
import pyqtgraph as pg


app = QtWidgets.QApplication(sys.argv)

plot = pg.PlotWidget(title="Rotating Robot Triangle (Working)")
plot.setXRange(0, 10)
plot.setYRange(0, 10)
plot.showGrid(x=True, y=True)

robot_item = pg.PlotDataItem(pen=pg.mkPen('b', width=2))
plot.addItem(robot_item)

x, y = 5, 5
angle = 0


def make_triangle(x, y, angle):
    size = 0.5

    pts = [
        (0, size),
        (-size / 2, -size / 2),
        (size / 2, -size / 2)
    ]

    rad = math.radians(angle)

    out = []
    for px, py in pts:
        rx = px * math.cos(rad) - py * math.sin(rad)
        ry = px * math.sin(rad) + py * math.cos(rad)

        out.append((rx + x, ry + y))

    # close shape
    out.append(out[0])

    xs = [p[0] for p in out]
    ys = [p[1] for p in out]

    return xs, ys


def update():
    global x, y, angle

    x += random.uniform(-0.05, 0.05)
    y += random.uniform(-0.05, 0.05)
    angle += 5  # rotation works here

    xs, ys = make_triangle(x, y, angle)

    robot_item.setData(xs, ys)


timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(100)

plot.show()
sys.exit(app.exec())