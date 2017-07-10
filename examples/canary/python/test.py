import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

count = 0

def animate(i):
	global count
	xar = [1,2,3]
	yar = [1,2,count]
	count += 1
	ax1.clear()
	ax1.plot(xar,yar)

ani = animation.FuncAnimation(fig, animate, interval=100)
plt.show()
