#!/usr/bin/env python
import sys
import numpy as np
import matplotlib.pyplot as plt
filename = sys.argv[1]
data = np.loadtxt(filename)
nm_per_pixel = sys.argv[2]

nm_per_pixel = float(nm_per_pixel);
dx = nm_per_pixel;
dy = nm_per_pixel;
Nx = 6144
Ny = 6144
dfx = 1.0/(Nx*dx);
dfy = 1.0/(Ny*dy);
df = sqrt(dfx*dfx + dfy*dfy);

q = data[:,0] * df
I = data[:,1]
plt.plot(q,I)
plt.ylabel('I')
plt.xlabel('q')
plt.show()
