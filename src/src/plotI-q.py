#!/usr/bin/env python
import sys
import numpy as np
import matplotlib.pyplot as plt
filename = sys.argv[1]
data = np.loadtxt(filename)
q = data[:,0]
I = data[:,1]

plt.plot(q,I)
plt.ylabel('I')
plt.xlabel('q')
plt.show()
