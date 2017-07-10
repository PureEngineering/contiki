import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig, ax = plt.subplots()

x = [1, 2, 3]
y = [1, 1, 1]
line, = ax.plot(x, y, lw = 2)
ax.set_ylim(0,4)

def animate():
    y[0] += 1
    if(y[0] > (ax.get_ylim()[1] - 5)):
	ax.set_ylim(ax.get_ylim()[1], ax.get_ylim()[1]*2)
    line.set_ydata(y)  # update the datai
    return line,

# Init only required for blitting to give a clean slate.

#ani = animation.FuncAnimation(fig, animate, interval=1000, blit=True)
plt.show()

for i in range(100):
	animate()
