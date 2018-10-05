import numpy as np
import matplotlib.pyplot as plt

output = np.loadtxt("output.dat")

stimgrp = 5
numstim = 4


color_list = ["#00FF00", "#0000FF", "#FF0000", "#01FFFE", "#FFA6FE",
              "#FFDB66", "#006401", "#010067", "#95003A", "#007DB5", "#FF00F6", "#FFEEE8", "#774D00",
              "#90FB92", "#0076FF", "#D5FF00", "#FF937E", "#6A826C", "#FF029D", "#FE8900", "#7A4782",
              "#7E2DD2", "#85A900", "#FF0056", "#A42400", "#00AE7E", "#683D3B", "#BDC6FF", "#263400",
              "#BDD393", "#00B917", "#9E008E", "#001544", "#C28C9F", "#FF74A3", "#01D0FF", "#004754",
              "#E56FFE", "#788231", "#0E4CA1", "#91D0CB", "#BE9970", "#968AE8", "#BB8800", "#43002C",
              "#DEFF74", "#00FFC6", "#FFE502", "#620E00", "#008F9C", "#98FF52", "#7544B1", "#B500FF",
              "#00FF78", "#FF6E41", "#005F39", "#6B6882", "#5FAD4E", "#A75740", "#A5FFD2", "#FFB167", 
              "#009BFF", "#E85EBE"]


fig = plt.figure()
scp = plt.scatter(output[:,1],output[:,0],s=9,linewidths=1.,marker='o',alpha=0.5)
plt.xlabel('Time (ms)', fontsize=14)
plt.ylabel('Cell Index', fontsize=14)

legend_labels = []
mlst = []

for i in xrange(numstim):
    label = 'Stimulus %i' % i
    color = color_list[i%len(color_list)]
    stimind = np.where(np.logical_and(output[:,0] >= (i*stimgrp), output[:,0] < ((i+1)*stimgrp)))[0]
    legend_labels.append(label)
    for x in output[stimind,1]:
        line = plt.axvline(x,marker='|',alpha=0.5,c = color, label=label, ymin=0.0, ymax=0.03)
    mlst.append(line)
        
plt.legend(mlst, legend_labels, loc='upper left', bbox_to_anchor=(-0.02, 1.12), ncol=4)

        
fig.show()


