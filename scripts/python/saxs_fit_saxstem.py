import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import scipy.optimize as optim
import os

def parse_saxs_data(input_file, nskiprows=1):
    data = pd.read_csv(input_file, sep=' ', skipinitialspace=True, skiprows=nskiprows,
                       names=['q', 'I'], usecols=[0, 1])
    return data

data_path  = "/home/phc/repository_local/FFT-from-image-compute-radial-intensity/src/resultsPaper/"
#output:
output_path = "/home/phc/repository_local/saxstem_compute/"

# carK
carK_saxs_file = os.path.join(data_path, "saxs/Carragenan_K_1car30KCl10A_merged.dat")
carK_saxs_data = parse_saxs_data(input_file=carK_saxs_file)
q_L_discard = 10**-4
q_L_M = 7.0 * 10**-3
q_L_M_transition_region = 1.0*10**-3
q_M_H = 5.0 * 10**-2
q_M_H_transition_region = 2.0*10**-2
q_H_discard = 3 * 10**-1

# carNa
carNa_saxs_file = os.path.join(data_path,"saxs/Carragenan_Na_1car300NaCl10A_merged.dat")
carNa_saxs_data = parse_saxs_data(input_file=carNa_saxs_file)
q_L_discard = 10**-4
q_L_M = 5.0 * 10**-3
q_L_M_transition_region = 1.0*10**-3
q_M_H = 2.0 * 10**-2
q_M_H_transition_region = 1.0*10**-2
q_H_discard = 2.5 * 10**-1

# pectin
pectin_saxs_file = os.path.join(data_path,"saxs/Pectin1Acid_1car200NaCl10A_merged.dat")
pectin_saxs_data = parse_saxs_data(input_file=pectin_saxs_file)
q_L_discard = 10**-4
q_L_M = 1.0 * 10**-2
q_L_M_transition_region = 0.5*10**-3
q_M_H = 2.0 * 10**-1
q_M_H_transition_region = 0.5*10**-2
q_H_discard = 2.5 * 10**-0

# Choose one:
data = carK_saxs_data
title = 'carK'
output_filename = "saxs-tem-carK_fit.pdf"
# Q Thresholds to separate regions (different fits for regions)
# Eyeball fit, change depending on data set
q_L_discard = 10**-4
q_L_M = 7.0 * 10**-3
q_L_M_transition_region = 1.0*10**-3
q_M_H = 5.0 * 10**-2
q_M_H_transition_region = 2.0*10**-2
q_H_discard = 3 * 10**-1

def power_func(x,A,B):
    return A * x**B

## Pretty plotting {{{
params = {
    'legend.loc': 'upper right',
    'xtick.top': True,
    'ytick.right': True,
    'axes.linewidth': 1.2,
    'axes.grid': True,
    'axes.grid.axis': 'both',
    'grid.color': 'gainsboro',
    'font.family': 'serif',
    # 'font.family': 'sans-serif',
    # 'font.sans-serif': 'Helvetica',
    'font.size': 12,
    'axes.labelsize': 16,
    'legend.fontsize': 12,
    'xtick.labelsize': 14,
    'ytick.labelsize': 14,
    'text.usetex': True,
    # 'figure.figsize': [width, height],
    'figure.facecolor': 'w',
    'figure.frameon': False
}
plt.rcParams.update(params)

fig, ax = plt.subplots()
ax.set_xscale("log", nonposx='clip')
ax.set_xlabel('q [$A^{-1}$]')
ax.set_yscale("log", nonposy='clip')
ax.set_ylabel('I [A.U.]')
ax.set_title(title)
ax.yaxis.grid(True, which='major')
ax.xaxis.grid(True, which='major')
ax.xaxis.set_minor_formatter(matplotlib.ticker.FormatStrFormatter(""))
# End pretty plotting }}}

# shift_data_factor = 1.0
shift_data_factor = 10**-0.1
plt.plot(data['q'], shift_data_factor * data['I'], figure=fig, label='original')
regionL = data.query('@q_L_discard <= q <= @q_L_M')
regionM = data.query('@q_L_M + @q_L_M_transition_region <= q <= @q_M_H')
regionH = data.query('@q_M_H + @q_M_H_transition_region <= q <= @q_H_discard')
print("Sizes:\n", "L", regionL.size, "M", regionM.size, "H", regionH.size)

regionL_popt, regionL_pcov = optim.curve_fit(power_func, regionL['q'], regionL['I'])
regionL_perr = np.sqrt(np.diag(regionL_pcov))
print("=======LOW======")
print("FitResult A * x**B:\n","Fit: A,B:", regionL_popt, "\nCov:", regionL_pcov)
print("One std-dev error for A, B: ", regionL_perr)
regionM_popt, regionM_pcov = optim.curve_fit(power_func, regionM['q'], regionM['I'])
regionM_perr = np.sqrt(np.diag(regionM_pcov))
print("=======MEDIUM======")
print("FitResult A * x**B:\n","Fit: A,B:", regionM_popt, "\nCov:", regionM_pcov)
print("One std-dev error for A, B: ", regionM_perr)
regionH_popt, regionH_pcov = optim.curve_fit(power_func, regionH['q'], regionH['I'])
regionH_perr = np.sqrt(np.diag(regionH_pcov))
print("=======HIGH======")
print("FitResult A * x**B:\n","Fit: A,B:", regionH_popt, "\nCov:", regionH_pcov)
print("One std-dev error for A, B: ", regionH_perr)

#========LOW========#
xdata = regionL['q']
popt = regionL_popt
plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='fit:A=%5.3f, B=%5.3f' % tuple(popt))
#========MEDIUM========#
xdata = regionM['q']
popt = regionM_popt
plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='fit:A=%5.3f, B=%5.3f' % tuple(popt))
#========HIGH========#
xdata = regionH['q']
popt = regionH_popt
plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='fit:A=%5.3f, B=%5.3f' % tuple(popt))

# Give some space to the limits (pretty plotting)
ylim_new = (ax.get_ylim()[0] - 0.9 * ax.get_ylim()[0],
            ax.get_ylim()[1] + 10 * ax.get_ylim()[1])
ax.set_ylim(ylim_new)
xlim_new = (ax.get_xlim()[0] - 0.1 * ax.get_xlim()[0],
            ax.get_xlim()[1] + 0.1 * ax.get_xlim()[1])
ax.set_xlim(xlim_new)
ax.autoscale_view(scalex=True, scaley=True)
ax.legend()
# Save and show
fig.subplots_adjust(bottom=0.155, top=0.86)
fig.savefig(os.path.join(output_path, output_filename))
plt.show()
