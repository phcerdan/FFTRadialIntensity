#!/usr/bin/env python
import plotIq
import unittest
import numpy as np

class PlotIqTest(unittest.TestCase):
    def setUp(self):
        self.known_dicto = {
              'name': 'disc20.tif',
              'Nx': 20,
              'Ny': 20
        }
        self.known_data = np.array( [
          [0.00000000e+00, 5.57352000e+09],
          [1.00000000e+00, 2.18122000e+08],
          [2.00000000e+00, 4.54179000e+06],
          [3.00000000e+00, 5.57892000e+06],
          [4.00000000e+00, 1.52643000e+06],
          [5.00000000e+00, 7.94623000e+05],
          [6.00000000e+00, 7.66628000e+05],
          [7.00000000e+00, 1.04966000e+05],
          [8.00000000e+00, 2.43107000e+05],
          [9.00000000e+00, 2.06538000e+05],
          [1.00000000e+01, 2.96088000e+04] ])

    def test_csv(self):
        is_csv = True
        input_file = (
               "./fixtures/"
               "test.csv"
              )
        header_dicto = plotIq.parse_header(input_file, is_csv)
        self.assertEqual(header_dicto, self.known_dicto)
        data = plotIq.parse_data(input_file, is_csv)
        np.testing.assert_array_equal(data.values, self.known_data)

    def test_ssv(self):
        is_csv = False
        input_file = (
               "./fixtures/"
               "test.plot"
           )
        header_dicto = plotIq.parse_header(input_file, is_csv)
        self.assertEqual(header_dicto, self.known_dicto)
        data = plotIq.parse_data(input_file, is_csv)
        np.testing.assert_array_equal(data.values, self.known_data)
