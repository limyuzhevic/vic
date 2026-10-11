#!/usr/bin/env python3
"""
Visualizer Unit Tests for NLM
Comprehensive unit tests for NLM Visualizer functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import time
from pathlib import Path
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.visualization.Visualizer import (
    PlotBuilder,
    PlotStyle,
    create_plot,
    plot_neuron_activity,
    plot_regional_stats,
    create_dashboard
)

class TestVisualizer(unittest.TestCase):
    """Test cases for Visualizer functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.plotter = PlotBuilder()
        self.mock_brain = Mock()
        
        # Configure mock brain
        self.mock_brain.getTotalNeuronCount.return_value = 100
        self.mock_brain.getTotalSynapseCount.return_value = 1000
        self.mock_brain.getRegionCount.return_value = 2
        self.mock_brain.getActiveNeuronCount.return_value = 10
        self.mock_brain.getFiringNeuronCount.return_value = 5
        self.mock_brain.getTotalSpikeCount.return_value = 1000
        self.mock_brain.getAverageFiringRate.return_value = 10.0
        self.mock_brain.getExcitationInhibitionRatio.return_value = 2.0
        
        # Mock regions
        mock_region1 = Mock()
        mock_region1.getId.return_value = Mock(value=1)
        mock_region1.getName.return_value = "Region 1"
        mock_region1.getTotalNeuronCount.return_value = 50
        mock_region1.getSynapseCount.return_value = 500
        mock_region1.getFiringNeuronCount.return_value = 3
        
        mock_region2 = Mock()
        mock_region2.getId.return_value = Mock(value=2)
        mock_region2.getName.return_value = "Region 2"
        mock_region2.getTotalNeuronCount.return_value = 50
        mock_region2.getSynapseCount.return_value = 500
        mock_region2.getFiringNeuronCount.return_value = 2
        
        self.mock_brain.getRegions.return_value = [mock_region1, mock_region2]
        
        # Mock regional methods
        mock_region1.getAverageSynapticWeight.return_value = 0.15
        mock_region2.getAverageSynapticWeight.return_value = 0.18
    
    def tearDown(self):
        """Clean up test fixtures"""
        # Close any open figures
        plt.close('all')
    
    def testPlotStyleCreation(self):
        """Test PlotStyle creation"""
        # Test default style
        default_style = PlotStyle("default")
        self.assertEqual(default_style.style_name, "default")
        self.assertIsNotNone(default_style.colors)
        self.assertIn('brain_activity', default_style.cmaps)
        
        # Test scientific style
        scientific_style = PlotStyle("scientific")
        self.assertEqual(scientific_style.style_name, "scientific")
        
        # Test dark style
        dark_style = PlotStyle("dark")
        self.assertEqual(dark_style.style_name, "dark")
        
        # Test minimal style
        minimal_style = PlotStyle("minimal")
        self.assertEqual(minimal_style.style_name, "minimal")
    
    def testPlotBuilderInitialization(self):
        """Test PlotBuilder initialization with different styles"""
        # Test with default style
        plotter1 = PlotBuilder()
        self.assertIsNotNone(plotter1.style)
        self.assertEqual(plotter1.style.style_name, "default")
        
        # Test with custom style
        custom_style = PlotStyle("scientific")
        plotter2 = PlotBuilder(custom_style)
        self.assertEqual(plotter2.style.style_name, "scientific")
    
    def testPlotNeuronActivityOverTime(self):
        """Test plotting neuron activity over time"""
        # Mock brain for activity data
        self.mock_brain.getTotalSpikeCount.return_value = 500
        
        # Test plotting without save
        fig = self.plotter.plot_neuron_activity_over_time(
            self.mock_brain, save_path=None, show=False
        )
        
        self.assertIsNotNone(fig)
        self.assertIsInstance(fig, plt.Figure)
        
        # Close the figure
        plt.close(fig)
        
        # Test plotting with save
        with tempfile.TemporaryDirectory() as temp_dir:
            save_path = os.path.join(temp_dir, "neuron_activity.png")
            fig = self.plotter.plot_neuron_activity_over_time(
                self.mock_brain, save_path=save_path, show=False
            )
            
            self.assertTrue(os.path.exists(save_path))
            plt.close(fig)
    
    def testPlotRegionalDistribution(self):
        """Test plotting regional distribution"""
        # Test plotting without save
        fig = self.plotter.plot_regional_distribution(
            self.mock_brain, save_path=None, show=False
        )
        
        self.assertIsNotNone(fig)
        self.assertIsInstance(fig, plt.Figure)
        plt.close(fig)
        
        # Test plotting with save
        with tempfile.TemporaryDirectory() as temp_dir:
            save_path = os.path.join(temp_dir, "regional_distribution.png")
            fig = self.plotter.plot_regional_distribution(
                self.mock_brain, save_path=save_path, show=False
            )
            
            self.assertTrue(os.path.exists(save_path))
            plt.close(fig)
    
    def testPlotDevelopmentalTrajectory(self):
        """Test plotting developmental trajectory"""
        # Create mock developmental data
        developmental_stages = []
        for i in range(10):
            stage = {
                'step': i * 10,
                'stage': 0.1 * i,  # Progress from 0 to 0.9
                'firing_rate': 5.0 + i * 0.5,
                'neuron_count': 50 + i * 5
            }
            developmental_stages.append(stage)
        
        # Test plotting without save
        fig = self.plotter.plot_developmental_trajectory(
            developmental_stages, save_path=None, show=False
        )
        
        self.assertIsNotNone(fig)
        self.assertIsInstance(fig, plt.Figure)
        plt.close(fig)
        
        # Test plotting with save
        with tempfile.TemporaryDirectory() as temp_dir:
            save_path = os.path.join(temp_dir, "developmental_trajectory.png")
            fig = self.plotter.plot_developmental_trajectory(
                developmental_stages, save_path=save_path, show=False
            )
            
            self.assertTrue(os.path.exists(save_path))
            plt.close(fig)
    
    def testPlotConnectivityNetwork(self):
        """Test plotting connectivity network"""
        # Create mock connectivity data
        connectivity_data = {
            'regions': [
                {'index': 0, 'x': 0.0, 'y': 0.0, 'size': 10, 'name': 'Region 1'},
                {'index': 1, 'x': 1.0, 'y': 0.0, 'size': 10, 'name': 'Region 2'},
                {'index': 2, 'x': 0.5, 'y': 1.0, 'size': 10, 'name': 'Region 3'}
            ],
            'connections': [
                {'source_x': 0.0, 'source_y': 0.0, 'target_x': 1.0, 'target_y': 0.0},
                {'source_x': 1.0, 'source_y': 0.0, 'target_x': 2.0, 'target_y': 0.0},
                {'source_x': 2.0, 'source_y': 0.0, 'target_x': 0.0, 'target_y': 1.0}
            ],
            'connection_types': ['excitatory', 'inhibitory', 'modulatory']
        }
        
        # Test plotting without save
        fig = self.plotter.plot_connectivity_network(
            connectivity_data, save_path=None, show=False
        )
        
        self.assertIsNotNone(fig)
        self.assertIsInstance(fig, plt.Figure)
        plt.close(fig)
        
        # Test plotting with save
        with tempfile.TemporaryDirectory() as temp_dir:
            save_path = os.path.join(temp_dir, "connectivity_network.png")
            fig = self.plotter.plot_connectivity_network(
                connectivity_data, save_path=save_path, show=False
            )
            
            self.assertTrue(os.path.exists(save_path))
            plt.close(fig)
    
    def testPlotPerformanceMetrics(self):
        """Test plotting performance metrics"""
        # Create mock performance data
        performance_data = []
        for i in range(20):
            data = {
                'step': i,
                'time': time.time() + i,
                'cpu_percent': 20.0 + (i % 10),
                'memory_usage_mb': 100.0 + (i * 2.0),
                'avg_step_time_ms': 10.0 + (i % 5),
                'total_spikes': i * 100,
                'active_neurons': 10 + (i % 20)
            }
            performance_data.append(data)
        
        # Test plotting without save
        fig = self.plotter.plot_performance_metrics(
            performance_data, save_path=None, show=False
        )
        
        self.assertIsNotNone(fig)
        self.assertIsInstance(fig, plt.Figure)
        plt.close(fig)
        
        # Test plotting with save
        with tempfile.TemporaryDirectory() as temp_dir:
            save_path = os.path.join(temp_dir, "performance_metrics.png")
            fig = self.plotter.plot_performance_metrics(
                performance_data, save_path=save_path, show=False
            )
            
            self.assertTrue(os.path.exists(save_path))
            plt.close(fig)
    
    def testCreateComprehensiveDashboard(self):
        """Test creating comprehensive dashboard"""
        # Create mock data sources
        data_sources = {
            'brain': {
                'total_neurons': 100,
                'total_synapses': 1000,
                'region_count': 2,
                'time_steps': 100,
                'simulation_time': 5.0,
                'peak_memory_mb': 150.0
            },
            'performance': [
                {'step': i, 'cpu_percent': 50.0 + (i % 20), 'memory_usage_mb': 100.0 + i * 1.5}
                for i in range(10)
            ],
            'regional': [
                {'region_name': f'Region {i}', 'neuron_count': 50}
                for i in range(3)
            ],
            'time_series': [
                {'step': i, 'average_firing_rate': 10.0 + i * 0.5, 'total_spikes': i * 100}
                for i in range(10)
            ],
            'connectivity': {
                'connection_types': ['excitatory', 'inhibitory', 'modulatory'],
                'connection_counts': [100, 50, 20]
            },
            'development': [
                {'stage': 0.1 * (i // 2), 'firing_rate': 5.0 + i * 0.3, 'neuron_count': 50 + i}
                for i in range(10)
            ],
            'system_info': {
                'total_neurons': 100,
                'total_synapses': 1000,
                'region_count': 2,
                'time_steps': 10,
                'simulation_time': 1.0,
                'peak_memory_mb': 150.0
            }
        }
        
        # Test creating dashboard without save
        fig = self.plotter.create_comprehensive_dashboard(
            data_sources, save_path=None, show=False
        )
        
        self.assertIsNotNone(fig)
        self.assertIsInstance(fig, plt.Figure)
        plt.close(fig)
        
        # Test creating dashboard with save
        with tempfile.TemporaryDirectory() as temp_dir:
            save_path = os.path.join(temp_dir, "dashboard.png")
            fig = self.plotter.create_comprehensive_dashboard(
                data_sources, save_path=save_path, show=False
            )
            
            self.assertTrue(os.path.exists(save_path))
            plt.close(fig)
    
    def testConvenienceFunctions(self):
        """Test convenience functions for easy usage"""
        # Test create_plot function
        plotter = create_plot("scientific")
        self.assertIsInstance(plotter, PlotBuilder)
        self.assertEqual(plotter.style.style_name, "scientific")
        
        # Test plot_neuron_activity function
        fig = plot_neuron_activity(self.mock_brain, save_path=None, show=False)
        self.assertIsNotNone(fig)
        plt.close(fig)
        
        # Test plot_regional_stats function
        fig = plot_regional_stats(self.mock_brain, save_path=None, show=False)
        self.assertIsNotNone(fig)
        plt.close(fig)
        
        # Test create_dashboard function
        fig = create_dashboard(data_sources=None, save_path=None, show=False)
        self.assertIsNotNone(fig)
        plt.close(fig)
    
    def testErrorHandling(self):
        """Test error handling in visualization functions"""
        # Test with invalid brain
        invalid_brain = Mock()
        invalid_brain.getTotalNeuronCount.side_effect = AttributeError("Brain not initialized")
        
        # This should handle the error gracefully (depending on implementation)
        fig = self.plotter.plot_neuron_activity_over_time(
            invalid_brain, save_path=None, show=False
        )
        
        # The function might still create a figure with default data
        self.assertIsNotNone(fig)
        plt.close(fig)
    
    def testStyleConsistency(self):
        """Test that different styles produce consistent results"""
        styles = ['default', 'scientific', 'dark', 'minimal']
        
        for style_name in styles:
            with self.subTest(style=style_name):
                plotter = create_plot(style_name)
                self.assertEqual(plotter.style.style_name, style_name)
                
                # Test that plotting works with the style
                fig = plotter.plot_neuron_activity_over_time(
                    self.mock_brain, save_path=None, show=False
                )
                self.assertIsNotNone(fig)
                plt.close(fig)

if __name__ == '__main__':
    unittest.main()
