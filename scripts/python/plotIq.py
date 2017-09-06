#!/usr/bin/env python
import sys
import pandas as pd
from itertools import takewhile
import matplotlib
import matplotlib.pyplot as plt
import argparse


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
        '-s', '--save_output',
        required=False, default=False,
        help='Save a figure, set up output_file as well')
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
    return parser.parse_args(args)


# Parse Header {{{
def parse_header (input_file, is_csv):
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
def parse_data(input_file, is_csv):
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

def scale_q(q_data, nm_per_pixel = 1):
    """ scale q range based on nm_per_pixel at size of image """
    # Nx = dicto_header['Nx']
    # Ny = dicto_header['Ny']
    # dx = nm_per_pixel
    # dy = nm_per_pixel
    # dfx = 1.0 / (Nx*dx)
    # dfy = 1.0 / (Ny*dy)
    # q_data *= max(dfx, dfy)
    # Other approach:
    q_size = q_data.size - 1 if q_data.size % 2 == 1 else q_data.size
    # multiply by 2 because q_size = min(Nx,Ny)/2 (from c++ code)
    dq = 1.0 / (q_size * 2) * 1.0/nm_per_pixel

    return q_data * dq

# Plot Data {{{
def plot_data(data, plot_name='', save=False, output_file='', output_format='svg'):
    # Paper quality options: {{{
    # https://github.com/jbmouret/matplotlib_for_papers#a-publication-quality-figure
    # for a list: matplotlib.rcParams
    params = {
        'xtick.top': True,
        'ytick.right': True,
        'axes.linewidth': 1.2,
        'axes.grid': True,
        'axes.grid.axis': 'both',
        'grid.color': 'gainsboro',
        'font.family': 'sans-serif',
        # 'font.sans-serif': 'Helvetica',
        'font.size': 8,
        'axes.labelsize': 8,
        'legend.fontsize': 10,
        'xtick.labelsize': 10,
        'ytick.labelsize': 10,
        'text.usetex': True,
        'figure.figsize': [4.5, 4.5]
    }
    # }}}
    # plt.rcParams.update(plt.rcParamsDefault)
    matplotlib.style.use('grayscale')
    # matplotlib.style.use('seaborn')
    plt.rcParams.update(params)
    # remove zero q for plotting.
    ax = data[1:].plot.line('q', 'I', legend=False)
    ax.set_title(plot_name)
    ax.set_xscale("log", nonposx='clip')
    ax.set_xlabel('q [$nm^{-1}$]')
    ax.set_yscale("log", nonposy='clip')
    ax.set_ylabel('I')
    ax.yaxis.grid(True, which='major')
    ax.xaxis.grid(True, which='minor')
    ax.xaxis.set_minor_formatter(matplotlib.ticker.FormatStrFormatter(""))

    # Vertical line:
    qcutoff = 5
    plt.axvline(x=qcutoff, linestyle='dotted')
    plt.axvspan(xmin=qcutoff, xmax=max(data['q']), color='whitesmoke')

    if(save and output_file):
        print(out_name)
        plt.savefig(output_file + '.' + output_format, format=output_format)

    plt.show(block=True)
# }}}


if __name__ == "__main__":
    args = parse_args(sys.argv[1:])
    is_csv = args.csv
    input_file = args.input_data
    output_file = args.output_file
    output_format = args.output_format
    save_output = args.save_output
    nm_per_pixel = args.nm_per_pixel

    header_dicto = parse_header(input_file, is_csv)
    data = parse_data(input_file, is_csv)
    data['q'] = scale_q(data['q'], nm_per_pixel)
    plot_data(data,
              plot_name='',
              # plot_name=header_dicto['name'],
              save=save_output, output_file=output_file,
              output_format=output_format)

# vim: foldmethod=marker foldlevel=0
