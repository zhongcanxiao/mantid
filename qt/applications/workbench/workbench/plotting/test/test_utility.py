# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2017 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid package
import unittest

import matplotlib
matplotlib.use("Agg")  # noqa
import matplotlib.pyplot as plt
from mantid.plots.legend import LegendProperties
from numpy import testing as np_testing

from mantid.plots.utility import zoom, zoom_axis
from testhelpers import assertRaisesNothing


class TestUtility(unittest.TestCase):

    def test_zoom_axis_raises_for_invalid_axis(self):
        self.assertRaises(ValueError, zoom_axis, None, None, 'z', None)

    def test_zoom_sets_correct_axis_limits(self):
        fig, ax = plt.subplots()
        ax.plot([0, 1, 2], [6, 4, 6])

        zoom_point = [2, 6]
        factor = 1 / 1.1  # = 0.909090...
        xlims, ylims = zoom(ax, *zoom_point, factor=factor)

        np_testing.assert_almost_equal([-0.31, 2.11], xlims)
        np_testing.assert_almost_equal([3.69, 6.11], ylims)

    def test_from_legend_correctly_returns_properties_if_title_is_none(self):
        fig, ax = plt.subplots()
        ax.plot([0, 1, 2], [6, 4, 6])
        ax.legend(labels='A', title=None)

        legend_props = LegendProperties.from_legend(ax.legend_)

        self.assertEqual(legend_props['title'], '')

    def test_zoom_out_really_far(self):
        """
        Attempt to zoom out far enough so that the new y-axis limits are beyond the maximum size of a float.
        """
        fig, ax = plt.subplots()
        ax.plot([0, 1, 2], [1e10, -1e10, 1e10])

        zoom_point = [0, 0]
        zoom_factor = 1e-300

        # zoom function will attempt to set the y-axis limits to +/-1e300.
        assertRaisesNothing(self, zoom, ax, *zoom_point, zoom_factor)


if __name__ == '__main__':
    unittest.main()
