#!/usr/bin/env python
import sys
import math
import pandas as pd
from itertools import takewhile
import matplotlib
import matplotlib.pyplot as plt
import argparse
from matplotlib2tikz import save as tikz_save
from cycler import cycler
# import seaborn as sns


def parse_args(args):
    parser = argparse.ArgumentParser(
        description='Plot q vs I data files')
    parser.add_argument(
        '-i', '--input_data',
        required=True,
        help='Input, it could be a csv, or a custom space separated. See --csv')
    parser.add_argument(
        '--csv',
        required=False, default=False,
        help='Select true if data is in csv format: one header, comma separated')
    parser.add_argument(
        '-o', '--output_file',
        required=False, default='',
        help='Output figure, choose format with -f')
    parser.add_argument(
        '-f', '--output_format',
        required=False, default='svg',
        help='Format for savefig')
    parser.add_argument(
        '-nm', '--nm_per_pixel',
        required=False, default=1.0,
        type=float,
        help='q range is scaled to the pixel resolution of the image')
    parser.add_argument(
        '-im', '--intensity_multiplier',
        required=False, default=1.0,
        type=float,
        help='multiply I [a.u] by a factor')
    parser.add_argument(
        '-qc', '--qcutoff',
        required=False, default=0.1,
        type=float,
        help='draw a vertical line at this q value')
    return parser.parse_args(args)


# Parse Header {{{
def parse_header (input_file, is_csv = False):
    # Parse Header {{{
    with open(input_file, 'r') as fobj:
        # takewhile returns an iterator over all the lines
        # that start with the comment string
        headiter = takewhile(lambda s: s.startswith('#'), fobj)
        # you may want to process the headers differently,
        # but here we just convert it to a list
        header = list(headiter)

    headers = []
    for h in header:
        if (is_csv):  # one line with commas
            h_comma = []
            h_comma = h.replace('#', '').replace('\n', '').split(',')
            for h_c in h_comma:
                headers += h_c.strip().split('=')
        else:  # header has multiple lines, space separated
            headers += h.replace('#', '').replace('\n', '').strip().split('=')

    return {
        'name': headers[1],
        'Nx': int(headers[3]),
        'Ny': int(headers[5])
    }
# }}}

# Parse Data {{{
def parse_data(input_file, is_csv = False):
    if(is_csv):
        data = pd.read_csv(input_file, header=0,
                           names=['q', 'I'], usecols=[0, 1])
    else:
        # spaces one liner style
        data = pd.read_csv(input_file, sep=' ', comment='#', header=None,
                           names=['q', 'I'], usecols=[0, 1])
    # }}}
    return data
# }}}

def parse_saxs_data(input_file, nskiprows=1):
    # input_file = "../R/Carragenan_K_1car30KCl10A__merged.dat"
    # input_file = "~/Dropbox/shared-geelong-palmerston\ (copy)/Carrageenan/Carrageenan_K/1car30KCl10A_1259_long.dat"
    # input_file = "~/Dropbox/shared-geelong-palmerston\ (copy)/Carrageenan/Carrageenan_K/1car30KCl10A_1243_short.dat"
    # fileSaxsLong = "~/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_K/1car30KCl10A_1259_long.dat"

    data = pd.read_csv(input_file, sep=' ', skipinitialspace=True, skiprows=nskiprows,
                       names=['q', 'I'], usecols=[0, 1])
    return data

def scale_data(data, nm_per_pixel=1, I_multiplier=1):
    """ scale q range based on nm_per_pixel at size of image """
    # Nx = dicto_header['Nx']
    # Ny = dicto_header['Ny']
    # dx = nm_per_pixel
    # dy = nm_per_pixel
    # dfx = 1.0 / (Nx*dx)
    # dfy = 1.0 / (Ny*dy)
    # q_data *= max(dfx, dfy)
    # Other approach:
    q_size = data['q'].size
    q_size = q_size - 1 if q_size % 2 == 1 else q_size
    # multiply by 2 because q_size = min(Nx,Ny)/2 (from c++ code)
    dq = 1.0 / (q_size * 2) * 1.0 / nm_per_pixel
    out_data = data
    out_data['q'] *= dq
    out_data['I'] *= I_multiplier
    #TODO multiply q for 2xpi here instead of __main__
    return out_data

def create_reciprocal_axis( ax_original ):
    # Create top x axis with space values:
    # https://stackoverflow.com/questions/28112217/matplotlib-second-x-axis-with-transformed-values
    def reciprocal_labels( ticks, angular = True ):
        V = 1.0/ticks
        if angular:
            V *= 2*math.pi
        return ["$%.2f$" % z for z in V]

    axtop = ax_original.twiny()
    axtop.set_xscale(ax_original.get_xscale())
    axtop.set_xticks(ax_original.get_xticks())
    axtop.set_xbound(ax_original.get_xbound())
    axtop.set_xticklabels(reciprocal_labels(ax_original.get_xticks()),
                          fontsize=13)
                          #fontsize = ax_original.xaxis.get_label().get_fontsize())
    axtop.set_xlabel("d [$nm$]",
                     fontsize = 16)
                     #fontsize = ax_original.title.get_fontsize()
    return axtop

def plot_params(dict_more_params={}):
    # Paper quality options: {{{
    # https://github.com/jbmouret/matplotlib_for_papers#a-publication-quality-figure
    # for a list: matplotlib.rcParams
    width = 6.2  # inches
    height = width / 1.618  # golden ratio
    # matplotlib.style.use('grayscale')
    # matplotlib.style.use('seaborn-deep')
    # sns.set_palette(sns.color_palette("cubehelix", 8))
    # sns.set_palette(sns.color_palette("Set1", n_colors=4))
    # cmap = sns.cubehelix_palette(as_cmap=True)
    # 'image.cmap': cmap,
    # 'axes.prop_cycle': cycler(color=map(lambda x: 'C' + x, '031456789')),
    params = {
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
        'figure.figsize': [width, height],
        'figure.facecolor': 'w',
        'figure.frameon': False
    }
    for k, v in dict_more_params.items():
        params[k] = v
    # }}}
    # plt.rcParams.update(plt.rcParamsDefault)
    plt.rcParams.update(params)

# Plot Data {{{
def plot_data(data, axes=None, plot_name='', label=''):
    # remove zero q for plotting.
    ax = data[1:].plot(x='q', y='I', legend=False, ax=axes, label=label)
    ax.set_title(plot_name)
    ax.set_xscale("log", nonposx='clip')
    ax.set_xlabel('q [$nm^{-1}$]')
    ax.set_yscale("log", nonposy='clip')
    ax.set_ylabel('I [A.U.]')
    ax.yaxis.grid(True, which='major')
    ax.xaxis.grid(True, which='major')
    ax.xaxis.set_minor_formatter(matplotlib.ticker.FormatStrFormatter(""))
    return ax

def plot_vline(qcutoff, data, ax):
    # Vertical line:
    # ax.axvline(x=qcutoff, linestyle='dotted', color='gray')
    ax.axvspan(xmin=qcutoff, xmax=ax.get_xlim()[1], color='whitesmoke')
    return ax

def plot_all_and_save(data, qcutoff=0.1, axes=None,
                      plot_name='', output_file='', output_format='svg',
                      show=True):
    ax = plot_data(data, plot_name=plot_name, axes=axes)
    ax = plot_vline(qcutoff, data, ax)
    create_reciprocal_axis(ax)
    if(output_file):
        if(output_format != 'tikz'):
            plt.savefig(output_file + '.' + output_format, format=output_format)
        else:
            tikz_save(output_file + '.' + 'tikz')
    else:
        if(show):
            plt.show(block=True)

    return ax
# }}}


if __name__ == "__main__":
    args = parse_args(sys.argv[1:])
    is_csv = args.csv
    input_file = args.input_data
    output_file = args.output_file
    output_format = args.output_format
    nm_per_pixel = args.nm_per_pixel
    intensity_multiplier = args.intensity_multiplier
    qcutoff = args.qcutoff

    header_dicto = parse_header(input_file, is_csv)
    data = parse_data(input_file, is_csv)
    data = scale_data(data, nm_per_pixel, intensity_multiplier)
    data['q'] *= 2.0 * math.pi
    plot_all_and_save(data,
                      qcutoff=qcutoff,
                      plot_name='',
                      # plot_name=header_dicto['name'],
                      output_file=output_file,
                      output_format=output_format)

# vim: foldmethod=marker foldlevel=0
