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
q_low_begin = carK_saxs_data['q'][0]
q_low_end = carK_saxs_data['q'][20]
q_intermediate_begin = carK_saxs_data['q'][27]
q_intermediate_end = carK_saxs_data['q'][140]
q_high_begin = carK_saxs_data['q'][157]
q_high_end = carK_saxs_data['q'][256]
q_noisy_cutoff = 5 * 10**-1

# carNa
carNa_saxs_file = os.path.join(data_path,"saxs/Carragenan_Na_1car300NaCl10A_merged.dat")
carNa_saxs_data = parse_saxs_data(input_file=carNa_saxs_file)
q_low_begin = carNa_saxs_data['q'][0]
q_low_end = carNa_saxs_data['q'][31]
q_intermediate_begin = carNa_saxs_data['q'][32]
q_intermediate_end = carNa_saxs_data['q'][106]
q_high_begin = carNa_saxs_data['q'][107]
q_high_end = carNa_saxs_data['q'][200]
q_noisy_cutoff = 2.5 * 10**-1

# pectin
pectin_saxs_file = os.path.join(data_path,"saxs/Pectin1Acid_1car200NaCl10A_merged.dat")
pectin_saxs_data = parse_saxs_data(input_file=pectin_saxs_file)
q_low_begin = 10**-3
q_low_end =  7 * 10**-3
q_intermediate_begin = 4 * 10**-2
q_intermediate_end = 1.55 * 10**-1
q_high_begin = 1.9 * 10**-1
q_high_end = 3.7 * 10**-1
q_noisy_cutoff = 4 * 10**-1

# Choose one:
data = pectin_saxs_data
title = 'pectin'
output_filename = "saxs-tem-pectin_fit.pdf"
# Q Thresholds to separate regions (different fits for regions)
# Eyeball fit, change depending on data set
q_low_begin = 10**-3
q_low_end =  7 * 10**-3
q_intermediate_begin = 4 * 10**-2
q_intermediate_end = 1.55 * 10**-1
q_high_begin = 1.9 * 10**-1
q_high_end = 3.7 * 10**-1
q_noisy_cutoff = 4 * 10**-1

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
region_all = data.query('q <= @q_noisy_cutoff')
plt.plot(region_all['q'], shift_data_factor * region_all['I'], figure=fig, label='saxs')
regionL = data.query('@q_low_begin <= q <= @q_low_end')
regionM = data.query('@q_intermediate_begin <= q <= @q_intermediate_end')
regionH = data.query('@q_high_begin <= q <= @q_high_end')
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
perr = regionL_perr
b_value_error = [ popt[1], perr[1] ]
# plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='fit: B=%3.3f (%2.8f)' % tuple(b_value_error))
plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='low-q: B=%3.3f (%1.3f)' % tuple(b_value_error))
#========MEDIUM========#
xdata = regionM['q']
popt = regionM_popt
perr = regionM_perr
b_value_error = [ popt[1], perr[1] ]
plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='int-q: B=%3.3f (%1.3f)' % tuple(b_value_error))
#========HIGH========#
xdata = regionH['q']
popt = regionH_popt
perr = regionH_perr
b_value_error = [ popt[1], perr[1] ]
plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='high-q: B=%3.3f (%1.3f)' % tuple(b_value_error))

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


#Carrageenan K:
# Low q: 1:21
# Coefficients (with 95% confidence bounds):
#        a =    0.008015  (0.005581, 0.01045)
#        b =      -2.088  (-2.137, -2.038)
#
# Intermediate q: 28:141
# Coefficients (with 95% confidence bounds):
#        a =       0.989  (0.9454, 1.033)
#        b =      -1.115  (-1.126, -1.105)
#
# High q: 158:257
# Coefficients (with 95% confidence bounds):
#        a =     0.05384  (0.05292, 0.05477)
#        b =      -2.124  (-2.131, -2.116)
#
# Carrageenan Na:
# low q: 1:32
# Coefficients (with 95% confidence bounds):
#        a =     0.08754  (0.07815, 0.09693)
#        b =      -1.926  (-1.943, -1.908)
#
# Intermediate q: 33:107
# Coefficients (with 95% confidence bounds):
#        a =    0.009776  (0.00927, 0.01028)
#        b =       -2.39  (-2.402, -2.378)
#
# High q: 108:201
# Coefficients (with 95% confidence bounds):
#        a =     0.02592  (0.02457, 0.02726)
#        b =      -2.108  (-2.124, -2.093)
#
# Pectin:
#
# low q: 1:20
# Coefficients (with 95% confidence bounds):
#        a =   2.448e-05  (1.002e-05, 3.894e-05)
#        b =      -2.282  (-2.378, -2.186)
#
# mid q: 100:239
# Coefficients (with 95% confidence bounds):
#        a =    0.002046  (0.002007, 0.002084)
#        b =      -1.271  (-1.277, -1.264)
#
# High q: 260:327
# Coefficients (with 95% confidence bounds):
#        a =   0.0003532  (0.0003287, 0.0003777)
#        b =      -2.278  (-2.324, -2.232)
