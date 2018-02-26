import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import scipy.optimize as optim
import os
import matplotlib.ticker as plticker

def parse_saxs_data(input_file, nskiprows=1):
    data = pd.read_csv(input_file, sep=' ', skipinitialspace=True, skiprows=nskiprows,
                       names=['q', 'I'], usecols=[0, 1])
    return data

data_path   = "/home/phc/repository_local/FFT-from-image-compute-radial-intensity/src/resultsPaper/saxs/"
#output:
output_path = "/home/phc/repository_local/FFT-from-image-compute-radial-intensity/src/resultsPaper/saxs/fit/"

# pectin
pectin_saxs_file = os.path.join(data_path,"Pectin1Acid_1car200NaCl10A_merged.dat")
pectin_saxs_data = parse_saxs_data(input_file=pectin_saxs_file)
q_pectin_low_begin = 10**-3
q_pectin_low_end =  7 * 10**-3
q_pectin_intermediate_begin = 4 * 10**-2
q_pectin_intermediate_end = 1.55 * 10**-1
q_pectin_high_begin = 1.9 * 10**-1
q_pectin_high_end = 3.7 * 10**-1
# q_pectin_noisy_cutoff = 4 * 10**-1
q_pectin_noisy_cutoff = pectin_saxs_data['q'].iloc[-1]

# carK
carK_saxs_file = os.path.join(data_path, "Carragenan_K_1car30KCl10A_merged.dat")
carK_saxs_data = parse_saxs_data(input_file=carK_saxs_file)
q_carK_low_begin = carK_saxs_data['q'][0]
q_carK_low_end = carK_saxs_data['q'][20]
q_carK_intermediate_begin = carK_saxs_data['q'][27]
q_carK_intermediate_end = carK_saxs_data['q'][140]
q_carK_high_begin = carK_saxs_data['q'][157]
q_carK_high_end = carK_saxs_data['q'][256]
q_carK_noisy_cutoff = q_pectin_noisy_cutoff
# q_carK_noisy_cutoff = 4 * 10**-1

# carNa
carNa_saxs_file = os.path.join(data_path,"Carragenan_Na_1car300NaCl10A_merged.dat")
carNa_saxs_data = parse_saxs_data(input_file=carNa_saxs_file)
q_carNa_low_begin = carNa_saxs_data['q'][0]
q_carNa_low_end = carNa_saxs_data['q'][30]
q_carNa_intermediate_begin = carNa_saxs_data['q'][34]
q_carNa_intermediate_end = carNa_saxs_data['q'][102]
q_carNa_high_begin = carNa_saxs_data['q'][114]
q_carNa_high_end = carNa_saxs_data['q'][200]
q_carNa_noisy_cutoff = q_pectin_noisy_cutoff
# q_carNa_noisy_cutoff = 4 * 10**-1

# Intensity multiplier for pretty plotting
I_pectin_multiplier = 10**3.0
I_carK_multiplier = 1.0
I_carNa_multiplier = 10**-1.3
# I_multiplier_list = [I_pectin_multiplier, I_carK_multiplier, I_carNa_multiplier]
pectin_saxs_data['I'] = pectin_saxs_data['I'] * I_pectin_multiplier
carK_saxs_data['I'] = carK_saxs_data['I'] * I_carK_multiplier
carNa_saxs_data['I'] = carNa_saxs_data['I'] * I_carNa_multiplier

data_list = [pectin_saxs_data, carK_saxs_data, carNa_saxs_data]
legend_labels = ['Pectin', 'Carrageenan K', 'Carrageenan Na']
# data = pectin_saxs_data
title = ''
output_filename = "saxs_fit.pdf"
# Q Thresholds to separate regions (different fits for regions)
# Eyeball fit, change depending on data set
q_low_begin_list = [q_pectin_low_begin, q_carK_low_begin, q_carNa_low_begin]
q_low_end_list = [q_pectin_low_end, q_carK_low_end, q_carNa_low_end]
q_intermediate_begin_list = [q_pectin_intermediate_begin, q_carK_intermediate_begin, q_carNa_intermediate_begin]
q_intermediate_end_list = [q_pectin_intermediate_end, q_carK_intermediate_end, q_carNa_intermediate_end]
q_high_begin_list = [q_pectin_high_begin, q_carK_high_begin, q_carNa_high_begin]
q_high_end_list = [q_pectin_high_end, q_carK_high_end, q_carNa_high_end]
q_noisy_cutoff_list = [q_pectin_noisy_cutoff, q_carK_noisy_cutoff, q_carNa_noisy_cutoff]

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
    # 'font.serif': 'Palatino',
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

fig_size = (8,6)
fig, ax = plt.subplots(figsize=fig_size)
ax.set_xscale("log", nonposx='clip')
ax.set_xlabel('q [$\mathring{A}^{-1}$]')
ax.set_yscale("log", nonposy='clip')
ax.set_ylabel('I [A.U.]')
ax.set_title(title)
ax.yaxis.grid(True, which='major')
ax.xaxis.grid(True, which='major')
ax.xaxis.set_minor_formatter(matplotlib.ticker.FormatStrFormatter(""))
loc = plticker.LogLocator(base=10.0, numticks=12)
ax.yaxis.set_major_locator(loc)
# End pretty plotting }}}

color_list = ['C9', 'C1', 'C2']
shift_data_factor = 1.0
# shift_data_factor = 10**-0.1
for index, data in enumerate(data_list):
    q_noisy_cutoff = q_noisy_cutoff_list[index]
    region_all = data.query('q <= @q_noisy_cutoff')
    plt.plot(region_all['q'], shift_data_factor * region_all['I'], linewidth=4, figure=fig,
            color=color_list[index], label=legend_labels[index])

for index, data in enumerate(data_list):
    q_low_begin = q_low_begin_list[index]
    q_low_end = q_low_end_list[index]
    q_intermediate_begin = q_intermediate_begin_list[index]
    q_intermediate_end = q_intermediate_end_list[index]
    q_high_begin = q_high_begin_list[index]
    q_high_end = q_high_end_list[index]
    q_noisy_cutoff = q_noisy_cutoff_list[index]
    regionL = data.query('@q_low_begin <= q <= @q_low_end')
    regionM = data.query('@q_intermediate_begin <= q <= @q_intermediate_end')
    regionH = data.query('@q_high_begin <= q <= @q_high_end')
    print("Sizes:\n", "L", regionL.size, "M", regionM.size, "H", regionH.size)

    regionL_popt, regionL_pcov = optim.curve_fit(power_func, regionL['q'], regionL['I'])
    regionL_perr = np.sqrt(np.diag(regionL_pcov))
    print("======", legend_labels[index] ,"========")
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
    # plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig, label='low-q: B=%3.3f (%1.3f)' % tuple(b_value_error))
    plt.plot(xdata, power_func(xdata, *popt), figure=fig, label='Power-Law Fit', color='black')
    #========MEDIUM========#
    xdata = regionM['q']
    popt = regionM_popt
    perr = regionM_perr
    b_value_error = [ popt[1], perr[1] ]
    plt.plot(xdata, power_func(xdata, *popt), figure=fig, label='Power-Law Fit', color='black')
    #========HIGH========#
    xdata = regionH['q']
    popt = regionH_popt
    perr = regionH_perr
    b_value_error = [ popt[1], perr[1] ]
    plt.plot(xdata, power_func(xdata, *popt), figure=fig, label='Power-Law Fit', color='black')

# Give some space to the limits (pretty plotting)
ylim_new = (ax.get_ylim()[0] - 0.1 * ax.get_ylim()[0],
            ax.get_ylim()[1] + 0.1 * ax.get_ylim()[1])
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


## RESULTS for manuscript (biomacromolecules)
# Sizes:
#  L 40 M 284 H 130
# ====== Pectin ========
# =======LOW======
# FitResult A * x**B:
#  Fit: A,B: [ 0.02447957 -2.28204189]
# Cov: [[4.74046373e-05 3.14107323e-04]
#  [3.14107323e-04 2.08486309e-03]]
# One std-dev error for A, B:  [0.0068851 0.0456603]
# =======MEDIUM======
# FitResult A * x**B:
#  Fit: A,B: [ 2.04707501 -1.27041707]
# Cov: [[3.68720361e-04 6.20939075e-05]
#  [6.20939075e-05 1.05636581e-05]]
# One std-dev error for A, B:  [0.01920209 0.00325018]
# =======HIGH======
# FitResult A * x**B:
#  Fit: A,B: [ 0.33411954 -2.31844905]
# Cov: [[0.00013014 0.00026021]
#  [0.00026021 0.00052584]]
# One std-dev error for A, B:  [0.01140793 0.02293121]
# Sizes:
#  L 42 M 228 H 200
# ====== Carrageenan K ========
# =======LOW======
# FitResult A * x**B:
#  Fit: A,B: [ 0.00801483 -2.08780703]
# Cov: [[1.35203253e-06 2.74813792e-05]
#  [2.74813792e-05 5.59739254e-04]]
# One std-dev error for A, B:  [0.00116277 0.02365881]
# =======MEDIUM======
# FitResult A * x**B:
#  Fit: A,B: [ 0.98903671 -1.11536744]
# Cov: [[4.84965054e-04 1.15676288e-04]
#  [1.15676288e-04 2.78390679e-05]]
# One std-dev error for A, B:  [0.02202192 0.00527627]
# =======HIGH======
# FitResult A * x**B:
#  Fit: A,B: [ 0.05384463 -2.12361247]
# Cov: [[2.16948325e-07 1.83008727e-06]
#  [1.83008727e-06 1.56817125e-05]]
# One std-dev error for A, B:  [0.00046578 0.00396001]
# Sizes:
#  L 62 M 138 H 174
# ====== Carrageenan Na ========
# =======LOW======
# FitResult A * x**B:
#  Fit: A,B: [ 0.00439922 -1.9252723 ]
# Cov: [[5.51271478e-08 2.05269255e-06]
#  [2.05269255e-06 7.66436794e-05]]
# One std-dev error for A, B:  [0.00023479 0.00875464]
# =======MEDIUM======
# FitResult A * x**B:
#  Fit: A,B: [ 4.63404537e-04 -2.40387883e+00]
# Cov: [[1.43039262e-10 7.16295506e-08]
#  [7.16295506e-08 3.59587509e-05]]
# One std-dev error for A, B:  [1.19599023e-05 5.99656159e-03]
# =======HIGH======
# FitResult A * x**B:
#  Fit: A,B: [ 1.24293981e-03 -2.12212128e+00]
# Cov: [[1.13888008e-09 2.75987510e-07]
#  [2.75987510e-07 6.70415060e-05]]
# One std-dev error for A, B:  [3.37472974e-05 8.18788776e-03]
