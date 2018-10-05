import numpy as np
import matplotlib.pyplot as plt

output = np.loadtxt("output.dat")

stimgrp = 5
numstim = 4

rainbow_color_list = ["#9400D3", "#4B0082", "#00FF00", "#FFFF00", "#FF7F00", "#FF0000"]

fig, axarr = plt.subplots(2,share=True)
ax[0].scatter(data[:,1],data[:,0],s=9,linewidths=1.,marker='o',alpha=0.5)

for i in xrange(numstim):
    stimind = np.where(np.logical_and(data[:0] >= i*stimgrp, data[:,0] < (i+1)*stimgrp))
    ax[1].plot(data[stimind,1],s=9,linewidths=3.,marker='|',alpha=0.5)

fig.show()


