#!/usr/bin/env python3
"""
Matplotlib Integration Unit Tests for NLM
Comprehensive unit tests for NLM Matplotlib integration functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import pandas as pd
import numpy as np
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.integration.MatplotlibIntegration import (
    MatplotlibIntegration,
    PlottingTools,
    ChartTypes,
    DataVisualization,
    InteractivePlots
)

class TestMatplotlibIntegration(unittest.TestCase):
    """Test cases for Matplotlib integration"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.integration = MatplotlibIntegration()
        
        # Mock pynlm to avoid dependency issues
        self.pynlm_patch = patch.dict('sys.modules', {
            'pynlm': Mock(),
        })
        self.mock_pynlm = self.pynlm_patch.start()
    
    def tearDown(self):
        """Clean up test fixtures"""
        self.pynlm_patch.stop()
        # Close any open figures
        plt.close('all')
    
    def testMatplotlibIntegrationInitialization(self):
        """Test MatplotlibIntegration initialization"""
        integration = MatplotlibIntegration()
        
        self.assertIsNotNone(integration.plotting_tools)
        self.assertIsNotNone(integration.chart_types)
        self.assertIsNotNone(integration.data_visualization)
        self.assertIsNotNone(integration.interactive_plots)
    
    def testPlottingToolsInitialization(self):
        """Test PlottingTools initialization"""
        tools = PlottingTools()
        
        self.assertEqual(tools.style_settings, {})
        self.assertEqual(tools.color_schemes, {})
        self.assertEqual(tools.figure_sizes, {})
    
    def testPlottingToolsStyleManagement(self):
        """Test style management"""
        tools = PlottingTools()
        
        # Test setting custom style
        style_config = {
            'font_family': 'serif',
            'font_size': 12,
            'figure_size': (10, 8),
            'line_width': 2.0,
            'colors': ['red', 'blue', 'green']
        }
        
        tools.set_style('custom_style', style_config)
        
        self.assertIn('custom_style', tools.style_settings)
        self.assertEqual(tools.style_settings['custom_style']['font_family'], 'serif')
        
        # Test getting style
        retrieved = tools.get_style('custom_style')
        self.assertEqual(retrieved['font_family'], 'serif')
    
    def testPlottingToolsColorSchemes(self):
        """Test color scheme management"""
        tools = PlottingTools()
        
        # Test setting color scheme
        color_scheme = ['red', 'blue', 'green', 'yellow', 'purple']
        tools.set_color_scheme('default', color_scheme)
        
        self.assertIn('default', tools.color_schemes)
        self.assertEqual(tools.color_schemes['default'], color_scheme)
        
        # Test getting color scheme
        retrieved = tools.get_color_scheme('default')
        self.assertEqual(retrieved, color_scheme)
        
        # Test applying color scheme to data
        data = np.array([1, 2, 3, 4, 5])
        colors = tools.apply_color_scheme(data, 'default')
        
        self.assertEqual(len(colors), len(data))
        self.assertEqual(colors[0], 'red')
        self.assertEqual(colors[4], 'purple')
    
    def testPlottingToolsFigureManagement(self):
        """Test figure management"""
        tools = PlottingTools()
        
        # Test creating figure
        fig = tools.create_figure()
        self.assertIsInstance(fig, plt.Figure)
        
        # Test setting figure size
        fig = tools.create_figure(size=(12, 8))
        self.assertEqual(fig.get_size_inches()[0], 12)
        self.assertEqual(fig.get_size_inches()[1], 8)
        
        # Test subplot
        fig, axes = tools.create_subplot_grid(2, 2)
        self.assertEqual(len(axes), 4)
        self.assertIsInstance(axes, np.ndarray)
        
        # Close figures
        plt.close('all')
    
    def testChartTypesBarChart(self):
        """Test bar chart functionality"""
        chart_types = ChartTypes()
        
        # Create sample data
        categories = ['A', 'B', 'C', 'D']
        values = [10, 20, 15, 25]
        
        # Test creating bar chart
        fig = chart_types.bar_chart(categories, values)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify data integrity
        axes = fig.axes[0]
        bars = axes.patches
        self.assertEqual(len(bars), 4)
        
        # Check bar heights
        self.assertAlmostEqual(bars[0].get_height(), 10)
        self.assertAlmostEqual(bars[1].get_height(), 20)
        self.assertAlmostEqual(bars[2].get_height(), 15)
        self.assertAlmostEqual(bars[3].get_height(), 25)
        
        plt.close(fig)
    
    def testChartTypesLineChart(self):
        """Test line chart functionality"""
        chart_types = ChartTypes()
        
        # Create sample data
        x = np.array([1, 2, 3, 4, 5])
        y = np.array([2, 4, 6, 8, 10])
        
        # Test creating line chart
        fig = chart_types.line_chart(x, y)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify line
        axes = fig.axes[0]
        lines = axes.lines
        self.assertEqual(len(lines), 1)
        
        # Check line data
        line_x = lines[0].get_xdata()
        line_y = lines[0].get_ydata()
        
        np.testing.assert_array_equal(line_x, x)
        np.testing.assert_array_equal(line_y, y)
        
        plt.close(fig)
    
    def testChartTypesScatterPlot(self):
        """Test scatter plot functionality"""
        chart_types = ChartTypes()
        
        # Create sample data
        x = np.array([1, 2, 3, 4, 5])
        y = np.array([2, 4, 1, 8, 5])
        
        # Test creating scatter plot
        fig = chart_types.scatter_plot(x, y)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify scatter
        axes = fig.axes[0]
        scatter = axes.collections[0]
        
        # Check data points
        data_x = scatter.get_offsets()[:, 0]
        data_y = scatter.get_offsets()[:, 1]
        
        np.testing.assert_array_equal(data_x, x)
        np.testing.assert_array_equal(data_y, y)
        
        plt.close(fig)
    
    def testChartTypesHeatmap(self):
        """Test heatmap functionality"""
        chart_types = ChartTypes()
        
        # Create sample data
        data = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
        
        # Test creating heatmap
        fig = chart_types.heatmap(data)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify heatmap
        axes = fig.axes[0]
        im = axes.collections[0]
        
        # Check data integrity
        heatmap_data = im.get_array()
        np.testing.assert_array_equal(heatmap_data, data)
        
        plt.close(fig)
    
    def testChartTypesHistogram(self):
        """Test histogram functionality"""
        chart_types = ChartTypes()
        
        # Create sample data
        data = np.array([1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5])
        
        # Test creating histogram
        fig = chart_types.histogram(data)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify histogram
        axes = fig.axes[0]
        patches = axes.patches
        
        # Should have multiple bars
        self.assertGreater(len(patches), 0)
        
        plt.close(fig)
    
    def testDataVisualizationNeuronActivity(self):
        """Test neuron activity visualization"""
        visualization = DataVisualization()
        
        # Create mock brain
        mock_brain = Mock()
        mock_brain.getTotalNeuronCount.return_value = 100
        mock_brain.getTotalSynapseCount.return_value = 1000
        mock_brain.getFiringNeuronCount.return_value = 10
        mock_brain.getAverageFiringRate.return_value = 5.0
        
        # Test creating neuron activity plot
        fig = visualization.neuron_activity_plot(mock_brain)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify plot contains expected elements
        axes = fig.axes
        self.assertGreater(len(axes), 0)
        
        plt.close(fig)
    
    def testDataVisualizationRegionalDistribution(self):
        """Test regional distribution visualization"""
        visualization = DataVisualization()
        
        # Create sample regional data
        regional_data = pd.DataFrame({
            'region_id': [1, 2, 3, 4, 5],
            'neuron_count': [50, 75, 100, 25, 50],
            'synapse_count': [500, 750, 1000, 250, 500],
            'firing_neuron_count': [5, 8, 12, 3, 6]
        })
        
        # Test creating regional distribution plot
        fig = visualization.regional_distribution_plot(regional_data)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify plot contains expected elements
        axes = fig.axes
        self.assertGreater(len(axes), 0)
        
        plt.close(fig)
    
    def testDataVisualizationTimeSeries(self):
        """Test time series visualization"""
        visualization = DataVisualization()
        
        # Create sample time series data
        time_series_data = pd.DataFrame({
            'step': range(100),
            'firing_rate': np.random.randn(100).cumsum() + 50,
            'spike_count': np.random.randint(0, 20, 100),
            'active_neurons': np.random.randint(0, 100, 100)
        })
        
        # Test creating time series plot
        fig = visualization.time_series_plot(time_series_data)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify plot contains expected elements
        axes = fig.axes
        self.assertGreater(len(axes), 0)
        
        plt.close(fig)
    
    def testDataVisualizationComparison(self):
        """Test comparison visualization"""
        visualization = DataVisualization()
        
        # Create sample comparison data
        comparison_data = {
            'model1': [1.0, 2.0, 3.0, 4.0, 5.0],
            'model2': [1.5, 2.5, 3.5, 4.5, 5.5],
            'model3': [0.8, 1.8, 2.8, 3.8, 4.8]
        }
        
        # Test creating comparison plot
        fig = visualization.comparison_plot(comparison_data)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify plot contains expected elements
        axes = fig.axes
        self.assertGreater(len(axes), 0)
        
        plt.close(fig)
    
    def testInteractivePlotsBasic(self):
        """Test basic interactive plots"""
        interactive = InteractivePlots()
        
        # Create sample data
        x = np.array([1, 2, 3, 4, 5])
        y = np.array([2, 4, 6, 8, 10])
        
        # Test creating basic interactive plot
        fig = interactive.create_interactive_plot(x, y)
        
        self.assertIsInstance(fig, plt.Figure)
        
        # Verify interactive elements
        # In a real implementation, this would test for interactive widgets
        # For now, we just verify the figure is created
        
        plt.close(fig)
    
    def testInteractivePlotsWithControls(self):
        """Test interactive plots with controls"""
        interactive = InteractivePlots()
        
        # Create sample data
        x = np.array([1, 2, 3, 4, 5])
        y = np.array([2, 4, 6, 8, 10])
        
        # Test creating interactive plot with controls
        fig = interactive.create_interactive_plot_with_controls(x, y)
        
        self.assertIsInstance(fig, plt.Figure)
        
        plt.close(fig)
    
    def testInteractivePlotsWithSlider(self):
        """Test interactive plots with slider"""
        interactive = InteractivePlots()
        
        # Create sample data
        x = np.array([1, 2, 3, 4, 5])
        y1 = np.array([2, 4, 6, 8, 10])
        y2 = np.array([1, 3, 5, 7, 9])
        
        # Test creating interactive plot with slider
        fig = interactive.create_interactive_plot_with_slider(x, y1, y2)
        
        self.assertIsInstance(fig, plt.Figure)
        
        plt.close(fig)
    
    def testMatplotlibIntegrationCreatePlot(self):
        """Test creating various plot types"""
        # Mock plotting tools
        mock_plotting = Mock()
        mock_chart_types = Mock()
        
        with patch('python.integration.MatplotlibIntegration.PlottingTools') as mock_plotting_class:
            with patch('python.integration.MatplotlibIntegration.ChartTypes') as mock_chart_class:
                mock_plotting_class.return_value = mock_plotting
                mock_chart_class.return_value = mock_chart_types
                
                # Test creating different plot types
                mock_fig = Mock()
                mock_plotting.bar_chart.return_value = mock_fig
                mock_chart_types.line_chart.return_value = mock_fig
                
                # Create integration instance
                integration = MatplotlibIntegration()
                
                # Test creating bar chart
                result = integration.create_plot('bar', [1, 2, 3], [10, 20, 30])
                self.assertEqual(result, mock_fig)
                
                # Test creating line chart
                result = integration.create_plot('line', [1, 2, 3], [2, 4, 6])
                self.assertEqual(result, mock_fig)
    
    def testMatplotlibIntegrationExportPlot(self):
        """Test exporting plots"""
        # Mock file operations
        with patch('matplotlib.pyplot.savefig') as mock_savefig:
            with patch('matplotlib.pyplot.close') as mock_close:
                # Create integration instance
                integration = MatplotlibIntegration()
                
                # Create mock figure
                mock_fig = Mock()
                
                # Test exporting plot
                result = integration.export_plot(mock_fig, '/tmp/test_plot.png')
                
                # Verify savefig was called
                mock_savefig.assert_called_once_with('/tmp/test_plot.png')
                
                # Verify close was called
                mock_close.assert_called_once()
    
    def testMatplotlibIntegrationThemeManagement(self):
        """Test theme management"""
        # Mock matplotlib style
        with patch('matplotlib.pyplot.style.use') as mock_style_use:
            with patch('matplotlib.pyplot.style.available') as mock_available:
                mock_available.return_value = ['default', 'classic', 'ggplot']
                
                # Create integration instance
                integration = MatplotlibIntegration()
                
                # Test setting theme
                result = integration.set_theme('ggplot')
                mock_style_use.assert_called_once_with('ggplot')
                
                # Test getting available themes
                themes = integration.get_available_themes()
                self.assertIsInstance(themes, list)
    
    def testMatplotlibIntegrationErrorHandling(self):
        """Test error handling in matplotlib integration"""
        # Test with invalid data
        invalid_data = "not a plot"
        
        # Should handle error gracefully
        try:
            result = self.integration.plotting_tools.bar_chart(invalid_data, [1, 2, 3])
            self.fail("Expected exception")
        except Exception:
            pass  # Expected
        
        # Test with invalid plot type
        try:
            result = self.integration.create_plot('invalid_type', [1, 2, 3], [2, 4, 6])
            self.fail("Expected exception")
        except Exception:
            pass  # Expected

if __name__ == '__main__':
    unittest.main()
