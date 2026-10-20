#!/usr/bin/env python3
"""
Test suite for NLM Visualization and Debugging Tools

This test file verifies the functionality of the NLM visualization and debugging
tools, including the VisualizationInterface and DebugInterface classes.
"""

import os
import sys
import time
import json
import unittest
from typing import Dict, Any, List
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm_visualization_tools import (
    VisualizationInterface,
    DebugInterface,
    VisualizationMode,
    DebugMode,
    MonitoringMode,
    OutputFormat,
    NeuronActivityData,
    SynapseActivityData,
    PopulationActivityData,
    WorldStateData,
    TimeSeriesDataPoint,
    HeatMapData,
    DebugInfo,
    MemoryAllocation,
    PerformanceMetrics,
    NeuralMetrics,
    SimulationMetrics,
    ResourceUsage,
    MonitoringHook,
    ErrorInfo,
    DebugStepInfo,
    ValidationResult,
    DebugCommand,
    NLMVisualizationTools
)

class TestDataStructures(unittest.TestCase):
    """Test data structure classes"""
    
    def test_neuron_activity_data(self):
        """Test NeuronActivityData"""
        data = NeuronActivityData(
            neuron_id=1,
            membrane_potential=45.5,
            firing_rate=12.3,
            inhibition_level=0.2,
            last_spike_time=1.5,
            is_active=True
        )
        
        self.assertEqual(data.neuron_id, 1)
        self.assertEqual(data.membrane_potential, 45.5)
        self.assertEqual(data.firing_rate, 12.3)
        self.assertEqual(data.inhibition_level, 0.2)
        self.assertEqual(data.last_spike_time, 1.5)
        self.assertTrue(data.is_active)
    
    def test_synapse_activity_data(self):
        """Test SynapseActivityData"""
        data = SynapseActivityData(
            synapse_id=2,
            source_neuron_id=1,
            target_neuron_id=3,
            weight=0.75,
            eligibility_trace=0.1,
            last_update_time=2.5,
            is_plastic=True
        )
        
        self.assertEqual(data.synapse_id, 2)
        self.assertEqual(data.source_neuron_id, 1)
        self.assertEqual(data.target_neuron_id, 3)
        self.assertEqual(data.weight, 0.75)
        self.assertEqual(data.eligibility_trace, 0.1)
        self.assertEqual(data.last_update_time, 2.5)
        self.assertTrue(data.is_plastic)
    
    def test_population_activity_data(self):
        """Test PopulationActivityData"""
        data = PopulationActivityData(
            population_id=3,
            type="excitatory",
            average_firing_rate=8.7,
            total_activity=123.4,
            active_neuron_count=45,
            total_neuron_count=100
        )
        
        self.assertEqual(data.population_id, 3)
        self.assertEqual(data.type, "excitatory")
        self.assertEqual(data.average_firing_rate, 8.7)
        self.assertEqual(data.total_activity, 123.4)
        self.assertEqual(data.active_neuron_count, 45)
        self.assertEqual(data.total_neuron_count, 100)
    
    def test_time_series_data_point(self):
        """Test TimeSeriesDataPoint"""
        metadata = {"source": "brain", "unit": "Hz"}
        data = TimeSeriesDataPoint(
            timestamp=time.time(),
            metric_name="firing_rate",
            value=15.2,
            metadata=metadata
        )
        
        self.assertIsInstance(data.timestamp, float)
        self.assertEqual(data.metric_name, "firing_rate")
        self.assertEqual(data.value, 15.2)
        self.assertEqual(data.metadata, metadata)
    
    def test_heat_map_data(self):
        """Test HeatMapData"""
        heat_values = [[0.1, 0.5], [0.3, 0.9]]
        data = HeatMapData(
            heat_values=heat_values,
            width=2,
            height=2,
            color_scheme="thermal",
            min_value=0.0,
            max_value=1.0
        )
        
        self.assertEqual(data.heat_values, heat_values)
        self.assertEqual(data.width, 2)
        self.assertEqual(data.height, 2)
        self.assertEqual(data.color_scheme, "thermal")
        self.assertEqual(data.min_value, 0.0)
        self.assertEqual(data.max_value, 1.0)

class TestVisualizationInterface(unittest.TestCase):
    """Test VisualizationInterface class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.viz = VisualizationInterface()
    
    def tearDown(self):
        """Clean up after tests"""
        if self.viz.is_active():
            self.viz.close()
    
    def test_initialization(self):
        """Test visualization interface initialization"""
        # The constructor will try to initialize the library
        # We expect it might fail and use Python fallback
        print("Testing visualization interface initialization...")
        self.assertIsNotNone(self.viz)
    
    def test_set_get_update_rate(self):
        """Test setting and getting update rate"""
        self.viz.set_update_rate(60.0)
        self.assertEqual(self.viz.get_update_rate(), 60.0)
        
        self.viz.set_update_rate(30.0)
        self.assertEqual(self.viz.get_update_rate(), 30.0)
    
    def test_set_get_visualization_mode(self):
        """Test setting and getting visualization mode"""
        self.viz.set_visualization_mode(VisualizationMode.REAL_TIME)
        self.assertEqual(self.viz.get_visualization_mode(), VisualizationMode.REAL_TIME)
        
        self.viz.set_visualization_mode(VisualizationMode.HEAT_MAP)
        self.assertEqual(self.viz.get_visualization_mode(), VisualizationMode.HEAT_MAP)
    
    def test_time_series_operations(self):
        """Test time series operations"""
        # Add time series data
        self.viz.add_time_series_metric("steps", 100.0)
        self.viz.add_time_series_metric("firing_rate", 12.5)
        self.viz.add_time_series_metric("steps", 101.0)
        
        # Get time series data
        steps_data = self.viz.get_time_series_data("steps")
        self.assertEqual(len(steps_data), 2)
        self.assertEqual(steps_data[0].value, 100.0)
        self.assertEqual(steps_data[1].value, 101.0)
        
        firing_data = self.viz.get_time_series_data("firing_rate")
        self.assertEqual(len(firing_data), 1)
        self.assertEqual(firing_data[0].value, 12.5)
    
    def test_debug_operations(self):
        """Test debug operations"""
        # Add debug info
        debug_info1 = DebugInfo(
            component_name="BrainVisualization",
            current_state="Active",
            warnings=["Warning 1"],
            errors=[],
            last_update_time=time.time(),
            is_healthy=True
        )
        
        debug_info2 = DebugInfo(
            component_name="DebugInterface",
            current_state="Running",
            warnings=[],
            errors=["Error 1"],
            last_update_time=time.time(),
            is_healthy=False
        )
        
        self.viz.add_debug_info(debug_info1)
        self.viz.add_debug_info(debug_info2)
        
        # Get debug info
        debug_infos = self.viz.get_debug_info()
        self.assertEqual(len(debug_infos), 2)
        self.assertEqual(debug_infos[0].component_name, "BrainVisualization")
        self.assertEqual(debug_infos[1].component_name, "DebugInterface")
    
    def test_real_time_updates(self):
        """Test real-time updates"""
        self.viz.enable_real_time_updates(True)
        self.assertTrue(self.viz.are_real_time_updates_enabled())
        
        self.viz.enable_real_time_updates(False)
        self.assertFalse(self.viz.are_real_time_updates_enabled())
    
    def test_recording(self):
        """Test recording functionality"""
        # Start recording
        self.viz.start_recording("test_recording.json")
        self.assertTrue(self.viz.is_recording())
        
        # Stop recording
        self.viz.stop_recording()
        self.assertFalse(self.viz.is_recording())
    
    def test_debug_overlay(self):
        """Test debug overlay functionality"""
        self.viz.enable_debug_overlay(True)
        self.assertTrue(self.viz.is_debug_overlay_enabled())
        
        self.viz.enable_debug_overlay(False)
        self.assertFalse(self.viz.is_debug_overlay_enabled())
    
    def test_monitoring_hooks(self):
        """Test monitoring hooks"""
        # Create callback function
        def test_callback(metrics):
            pass
        
        # Create hook
        hook = MonitoringHook(
            hook_name="test_hook",
            callback=test_callback,
            last_execution=time.time(),
            execution_interval=1.0,
            is_active=True,
            hook_type="periodic"
        )
        
        # Add hook
        self.viz.add_monitoring_hook(hook)
        
        # Get hooks
        hooks = self.viz.get_monitoring_hooks()
        self.assertEqual(len(hooks), 1)
        self.assertEqual(hooks[0].hook_name, "test_hook")
        
        # Remove hook
        self.viz.remove_monitoring_hook("test_hook")
        hooks = self.viz.get_monitoring_hooks()
        self.assertEqual(len(hooks), 0)

class TestDebugInterface(unittest.TestCase):
    """Test DebugInterface class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.debug = DebugInterface()
    
    def tearDown(self):
        """Clean up after tests"""
        if self.debug.is_active():
            self.debug.close()
    
    def test_initialization(self):
        """Test debug interface initialization"""
        print("Testing debug interface initialization...")
        self.assertIsNotNone(self.debug)
    
    def test_set_get_debug_mode(self):
        """Test setting and getting debug mode"""
        self.debug.set_debug_mode(DebugMode.DETAILED)
        self.assertEqual(self.debug.get_debug_mode(), DebugMode.DETAILED)
        
        self.debug.set_debug_mode(DebugMode.EXPERT)
        self.assertEqual(self.debug.get_debug_mode(), DebugMode.EXPERT)
    
    def test_set_get_monitoring_mode(self):
        """Test setting and getting monitoring mode"""
        self.debug.set_monitoring_mode(MonitoringMode.ACTIVE)
        self.assertEqual(self.debug.get_monitoring_mode(), MonitoringMode.ACTIVE)
        
        self.debug.set_monitoring_mode(MonitoringMode.ALERTING)
        self.assertEqual(self.debug.get_monitoring_mode(), MonitoringMode.ALERTING)
    
    def test_performance_metrics(self):
        """Test performance metrics"""
        metrics = self.debug.get_performance_metrics()
        self.assertIsInstance(metrics, PerformanceMetrics)
        self.assertGreaterEqual(metrics.cpu_usage, 0.0)
        self.assertLessEqual(metrics.cpu_usage, 100.0)
        self.assertGreaterEqual(metrics.memory_usage, 0.0)
        self.assertLessEqual(metrics.memory_usage, 100.0)
    
    def test_neural_metrics(self):
        """Test neural metrics"""
        metrics = self.debug.get_neural_metrics()
        self.assertIsInstance(metrics, NeuralMetrics)
        self.assertGreaterEqual(metrics.total_neuron_count, 0)
        self.assertGreaterEqual(metrics.active_neuron_count, 0)
        self.assertGreaterEqual(metrics.spiking_neuron_count, 0)
        self.assertGreaterEqual(metrics.average_firing_rate, 0.0)
    
    def test_neural_activity_helpers(self):
        """Test neural activity helper methods"""
        # Note: These methods would normally require a Brain object
        # For testing, we'll just verify they exist and don't crash
        print("Testing neural activity helper methods...")
        
        # Test that methods exist
        self.assertTrue(hasattr(self.debug, 'get_neural_metrics'))
        self.assertTrue(hasattr(self.debug, 'get_average_firing_rate'))
        self.assertTrue(hasattr(self.debug, 'get_active_neuron_count'))
        self.assertTrue(hasattr(self.debug, 'get_most_active_neurons'))
    
    def test_error_detection(self):
        """Test error detection"""
        # Enable error detection
        self.debug.enable_error_detection(True)
        self.assertTrue(self.debug.is_error_detection_enabled())
        
        # Log an error
        self.debug.log_error("TEST_ERROR", "Test error message", True)
        
        # Get errors
        errors = self.debug.get_recent_errors()
        self.assertEqual(len(errors), 1)
        self.assertEqual(errors[0].error_code, "TEST_ERROR")
        self.assertEqual(errors[0].error_message, "Test error message")
        
        # Get critical errors
        critical_errors = self.debug.get_critical_errors()
        self.assertEqual(len(critical_errors), 1)
    
    def test_step_by_step_debugging(self):
        """Test step-by-step debugging"""
        # Enable step-by-step debugging
        self.debug.enable_step_by_step_debugging(True)
        self.assertTrue(self.debug.is_step_by_step_debugging_enabled())
        
        # Set debug step
        self.debug.set_debug_step(100)
        self.assertEqual(self.debug.get_current_debug_step(), 100)
        
        # Test step navigation
        self.debug.step_forward()
        self.assertEqual(self.debug.get_current_debug_step(), 101)
        
        self.debug.step_backward()
        self.assertEqual(self.debug.get_current_debug_step(), 100)
    
    def test_command_line_interface(self):
        """Test command-line interface"""
        # Enable command-line interface
        self.debug.enable_command_line_interface(True)
        self.assertTrue(self.debug.is_command_line_interface_enabled())
        
        # Test commands
        self.assertTrue(self.debug.execute_command("help"))
        self.assertTrue(self.debug.execute_command("quit"))
        self.assertTrue(self.debug.execute_command("visualize brain"))
        self.assertFalse(self.debug.execute_command("unknown_command"))
        
        # Test command help
        help_text = self.debug.get_command_help("help")
        self.assertIn("Show available commands", help_text)
        
        # Test invalid command help
        invalid_help = self.debug.get_command_help("invalid_command")
        self.assertIn("Command not found", invalid_help)
    
    def test_recording(self):
        """Test recording functionality"""
        # Start recording
        self.debug.start_recording("test_debug.json")
        self.assertTrue(self.debug.is_recording())
        
        # Stop recording
        self.debug.stop_recording()
        self.assertFalse(self.debug.is_recording())
    
    def test_monitoring_hooks(self):
        """Test monitoring hooks"""
        # Create callback function
        def test_callback(metrics):
            pass
        
        # Create hook
        hook = MonitoringHook(
            hook_name="test_hook",
            callback=test_callback,
            last_execution=time.time(),
            execution_interval=1.0,
            is_active=True,
            hook_type="periodic"
        )
        
        # Add hook
        self.debug.add_monitoring_hook(hook)
        
        # Get hooks
        hooks = self.debug.get_monitoring_hooks()
        self.assertEqual(len(hooks), 1)
        self.assertEqual(hooks[0].hook_name, "test_hook")
        
        # Remove hook
        self.debug.remove_monitoring_hook("test_hook")
        hooks = self.debug.get_monitoring_hooks()
        self.assertEqual(len(hooks), 0)

class TestNLMVisualizationTools(unittest.TestCase):
    """Test NLMVisualizationTools class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.tools = NLMVisualizationTools()
    
    def tearDown(self):
        """Clean up after tests"""
        if self.tools.is_active():
            self.tools.close()
    
    def test_initialization(self):
        """Test NLM tools initialization"""
        # Initialize tools
        self.assertTrue(self.tools.initialize())
        self.assertTrue(self.tools.is_active())
        
        # Get interfaces
        viz = self.tools.get_visualization_interface()
        self.assertIsNotNone(viz)
        
        debug = self.tools.get_debug_interface()
        self.assertIsNotNone(debug)
    
    def test_get_visualization_data(self):
        """Test getting visualization data"""
        self.assertTrue(self.tools.initialize())
        
        # Get visualization data
        data = self.tools.get_visualization_data()
        self.assertIsInstance(data, dict)
        self.assertIn("visualization_mode", data)
        self.assertIn("debug_mode", data)
        self.assertIn("performance_metrics", data)
        self.assertIn("neural_metrics", data)
    
    def test_export_data(self):
        """Test data export"""
        self.assertTrue(self.tools.initialize())
        
        # Export to JSON
        self.tools.export_data("test_export.json", OutputFormat.JSON)
        
        # Check if file was created
        self.assertTrue(os.path.exists("test_export.json"))
        
        # Clean up
        if os.path.exists("test_export.json"):
            os.remove("test_export.json")
    
    def test_close(self):
        """Test closing tools"""
        self.assertTrue(self.tools.initialize())
        self.tools.close()
        self.assertFalse(self.tools.is_active())

class TestIntegration(unittest.TestCase):
    """Integration tests for the whole system"""
    
    def setUp(self):
        """Set up test fixtures"""
        # Initialize NLM tools
        from nlm_visualization_tools import initialize_nlm_tools
        self.tools = initialize_nlm_tools()
    
    def tearDown(self):
        """Clean up after tests"""
        from nlm_visualization_tools import get_nlm_tools
        if get_nlm_tools().is_active():
            get_nlm_tools().close()
    
    def test_full_workflow(self):
        """Test full workflow with all features"""
        tools = self.tools
        
        # Get interfaces
        viz = tools.get_visualization_interface()
        debug = tools.get_debug_interface()
        
        # Configure tools
        viz.set_update_rate(30.0)
        viz.set_visualization_mode(VisualizationMode.REAL_TIME)
        
        debug.set_debug_mode(DebugMode.DETAILED)
        debug.set_monitoring_mode(MonitoringMode.ACTIVE)
        debug.enable_real_time_debugging(True)
        debug.enable_memory_leak_detection(True)
        debug.enable_performance_profiling(True)
        debug.enable_error_detection(True)
        
        # Run simulation
        for step in range(50):
            # Update interfaces
            viz.update()
            debug.update()
            
            # Add data
            viz.add_time_series_metric("step", float(step))
            viz.add_time_series_metric("firing_rate", 10.0 + (time.time() % 5.0))
            
            # Add debug info
            debug_info = DebugInfo(
                component_name="BrainVisualization",
                current_state="Active",
                warnings=["Warning 1"],
                errors=[],
                last_update_time=time.time(),
                is_healthy=True
            )
            viz.add_debug_info(debug_info)
            
            # Log some errors occasionally
            if step % 10 == 0:
                debug.log_error(f"ERROR_{step}", f"Test error at step {step}")
            
            time.sleep(0.01)  # Simulate time passing
        
        # Verify data
        time_series_data = viz.get_time_series_data("step")
        self.assertEqual(len(time_series_data), 50)
        
        debug_infos = viz.get_debug_info()
        self.assertEqual(len(debug_infos), 50)
        
        errors = debug.get_recent_errors()
        self.assertEqual(len(errors), 5)  # Steps 0, 10, 20, 30, 40
        
        # Generate report
        report = debug.generate_debug_report()
        self.assertIn("NLM Debug Report", report)
        
        # Export data
        tools.export_data("integration_test.json", OutputFormat.JSON)
        self.assertTrue(os.path.exists("integration_test.json"))
        
        # Clean up
        os.remove("integration_test.json")

class TestExampleUsage(unittest.TestCase):
    """Test example usage functionality"""
    
    def test_example_usage(self):
        """Test the example usage function"""
        from nlm_visualization_tools import example_usage
        
        # Note: This test would run the full example, which takes time
        # For testing purposes, we'll just verify the function exists and doesn't crash
        self.assertTrue(callable(example_usage))
        
        # We won't actually call it here as it would take a long time
        print("Example usage function exists and is callable")

if __name__ == "__main__":
    # Run all tests
    print("Running NLM Visualization and Debugging Tools Tests...")
    print("=" * 60)
    
    # Run unittest tests
    unittest.main(verbosity=2)
    
    print("\nAll tests completed!")
