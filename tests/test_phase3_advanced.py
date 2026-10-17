#!/usr/bin/env python3
"""
Unit tests for Phase 3 - Advanced features for expert users.

This test file covers:
- AdvancedBrainManipulator functionality
- Performance monitoring tools
- Advanced configuration management
- Advanced serialization and persistence
- Advanced visualization and monitoring
"""

import sys
import os
import tempfile
import shutil
import json
import time
from pathlib import Path
from typing import Dict, List, Any

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import python.pynlm as pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
def test_advanced_features_structure():
    """Test that all advanced features classes are properly structured."""
    # Check that all Phase 3 classes exist
    expected_classes = [
        'AdvancedBrainManipulator', 'PerformanceMonitor', 'MetricRecorder',
        'TimelineRecorder', 'Profiler', 'MemoryMonitor', 'ConfigManager',
        'SerializationManager', 'VisualizationTools'
    ]
    
    for class_name in expected_classes:
        assert hasattr(pynlm, class_name), f"Missing class: {class_name}"
        print(f"✓ Found class: {class_name}")
    
    # Check that all Phase 3 factory functions exist
    expected_factories = [
        'createAdvancedBrainManipulator', 'createPerformanceMonitor',
        'createMetricRecorder', 'createTimelineRecorder', 'createProfiler',
        'createMemoryMonitor', 'createConfigManager', 'createSerializationManager',
        'createVisualizationTools'
    ]
    
    for factory_name in expected_factories:
        assert hasattr(pynlm, factory_name), f"Missing factory function: {factory_name}"
        print(f"✓ Found factory function: {factory_name}")
    
    print("✓ All Phase 3 advanced features are present")
class TestPhase3Advanced:
    """Comprehensive test suite for Phase 3 advanced features."""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp()
        self.setup_test_components()
    
    def __del__(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def setup_test_components(self):
        """Set up test components for advanced feature testing."""
        # Create test configuration
        self.test_config = pynlm.create_default_config("phase3_test", 100, 0.01)
        
        # Create test brain
        self.test_brain = pynlm.createBrain(self.test_config)
        self.test_brain.initialize()
    
    def test_advanced_brain_manipulator_comprehensive(self):
        """Test AdvancedBrainManipulator comprehensive functionality."""
        # Test factory function
        manipulator = pynlm.createAdvancedBrainManipulator(self.test_brain)
        assert manipulator is not None
        
        # Create regions
        region1 = self.test_brain.addRegion("visual_cortex")
        region2 = self.test_brain.addRegion("motor_cortex")
        
        # Create neurons
        neuron1 = manipulator.createNeuron(region1, pynlm.NeuronType.Excitatory)
        neuron2 = manipulator.createNeuron(region1, pynlm.NeuronType.Inhibitory)
        neuron3 = manipulator.createNeuron(region2, pynlm.NeuronType.Sensory)
        
        assert neuron1 is not None
        assert neuron2 is not None
        assert neuron3 is not None
        
        # Create synapses
        synapse1 = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
        synapse2 = manipulator.createSynapse(neuron2, neuron3, weight=0.3)
        
        assert synapse1 is not None
        assert synapse2 is not None
        
        # Test neuron manipulation
        new_state = pynlm.NeuronState()
        new_state.threshold = -45.0
        new_state.firingRate = 10.0
        
        result = manipulator.modifyNeuronProperties(neuron1, new_state)
        assert result == True
        
        # Get neuron state
        state = manipulator.getNeuronState(neuron1)
        assert state.threshold == -45.0
        assert state.firingRate == 10.0
        
        # Test synapse manipulation
        new_synapse = pynlm.Synapse()
        new_synapse.weight = 0.7
        new_synapse.delay = 2
        
        result = manipulator.modifySynapseProperties(synapse1, new_synapse)
        assert result == True
        
        # Get synapse properties
        props = manipulator.getSynapseProperties(synapse1)
        assert props.weight == 0.7
        assert props.delay == 2
        
        # Test connectivity queries
        neurons_in_region1 = manipulator.getNeuronsInRegion(region1)
        neurons_in_region2 = manipulator.getNeuronsInRegion(region2)
        
        assert neuron1 in neurons_in_region1
        assert neuron2 in neurons_in_region1
        assert neuron3 in neurons_in_region2
        
        # Test synaptic connections
        synapses_from_neuron1 = manipulator.getSynapsesFromNeuron(neuron1)
        synapses_to_neuron2 = manipulator.getSynapsesToNeuron(neuron2)
        
        assert synapse1 in synapses_from_neuron1
        assert synapse1 in synapses_to_neuron2
        
        # Test plasticity configuration
        manipulator.setHebbianLearningRate(0.01)
        manipulator.setSTDPParameters(10.0, 20.0, 0.01)
        
        # Enable plasticity for synapse
        manipulator.enablePlasticityForSynapse(synapse1, True, False, False)
        
        # Test development stage control
        manipulator.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        stage = manipulator.getDevelopmentalStage()
        assert stage == pynlm.DevelopmentalStage.CriticalPeriod
        
        print("✓ AdvancedBrainManipulator comprehensive test passed")
    
    def test_performance_monitor_comprehensive(self):
        """Test PerformanceMonitor comprehensive functionality."""
        # Test factory function
        monitor = pynlm.createPerformanceMonitor()
        assert monitor is not None
        
        # Start monitoring
        monitor.start()
        assert monitor.is_monitoring == True
        
        # Record performance steps
        test_data = [
            (10.0, 5, 25),
            (12.0, 8, 30),
            (15.0, 12, 35),
            (18.0, 15, 40),
            (20.0, 20, 45)
        ]
        
        for step_time, spikes, active_neurons in test_data:
            monitor.recordStep(step_time, spikes, active_neurons)
        
        # Stop monitoring
        monitor.stop()
        assert monitor.is_monitoring == False
        
        # Test performance metrics
        assert monitor.getTotalTimeSeconds() > 0
        assert monitor.getAvgStepTimeMs() > 0
        assert monitor.getStepsPerSecond() > 0
        assert monitor.getSpikesPerSecond() > 0
        assert monitor.getFiringRateHz() > 0
        assert monitor.getStepCount() == 5
        assert monitor.getTotalSpikes() == 5 + 8 + 12 + 15 + 20  # 60
        assert monitor.getCurrentActiveNeurons() == 45
        
        # Test stability check
        is_stable = monitor.isStable(windowSize=3, threshold=0.5)
        assert isinstance(is_stable, bool)
        
        # Test performance report
        report = monitor.getPerformanceReport()
        assert isinstance(report, str)
        assert "Performance Report" in report
        assert "Steps per Second" in report
        
        print("✓ PerformanceMonitor comprehensive test passed")
    
    def test_metric_recorder_comprehensive(self):
        """Test MetricRecorder comprehensive functionality."""
        # Test factory function
        recorder = pynlm.createMetricRecorder()
        assert recorder is not None
        
        # Test various metric types
        recorder.record("int_metric", 42)
        recorder.record("float_metric", 3.14159)
        recorder.record("bool_metric", True)
        
        # Test time series recording
        int_series = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        float_series = [1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.0]
        
        recorder.recordTimeSeries("int_series", int_series, 1000.0)
        recorder.recordTimeSeries("float_series", float_series, 2000.0)
        
        # Test metric retrieval
        assert recorder.getMetric("int_metric") == 42
        assert recorder.getMetric("float_metric") == 3.14159
        assert recorder.getMetric("bool_metric") == True
        
        # Test statistical methods
        assert recorder.getMean("int_metric") == 42.0
        assert recorder.getMean("float_metric") == 3.14159
        assert recorder.getStdDev("int_metric") == 0.0
        assert recorder.getMin("int_metric") == 42
        assert recorder.getMax("int_metric") == 42
        
        # Test time series statistics
        assert recorder.getMean("int_series") == 5.5
        assert recorder.getStdDev("float_series") > 0
        assert recorder.getMin("float_series") == 1.1
        assert recorder.getMax("float_series") == 10.0
        
        # Test metric names
        metric_names = recorder.getMetricNames()
        assert "int_metric" in metric_names
        assert "float_metric" in metric_names
        assert "bool_metric" in metric_names
        assert "int_series" in metric_names
        assert "float_series" in metric_names
        
        print("✓ MetricRecorder comprehensive test passed")
    
    def test_config_manager_comprehensive(self):
        """Test ConfigManager comprehensive functionality."""
        # Test factory function
        config_manager = pynlm.createConfigManager()
        assert config_manager is not None
        
        # Create test templates
        template1 = pynlm.create_default_config()
        template1.set("brain.neuron_count", 1000)
        template1.set("brain.simulation_steps", 10000)
        template1.set("plasticity.enabled", True)
        
        template2 = pynlm.create_default_config()
        template2.set("brain.neuron_count", 5000)
        template2.set("brain.simulation_steps", 50000)
        template2.set("plasticity.enabled", False)
        
        # Add templates
        config_manager.addTemplate("small", "Small network configuration", template1, True)
        config_manager.addTemplate("large", "Large network configuration", template2)
        
        # Test template management
        templates = config_manager.getTemplateNames()
        assert "small" in templates
        assert "large" in templates
        
        # Create config from template
        config = config_manager.createFromTemplate("large")
        assert config is not None
        assert config.getInt("brain.neuron_count") == 5000
        assert config.getBool("plasticity.enabled") == False
        
        # Test config validation
        is_valid = config_manager.validateConfig(config)
        assert is_valid == True
        
        # Test config validation with details
        errors = config_manager.validateConfigWithDetails(config)
        assert isinstance(errors, list)
        assert len(errors) == 0
        
        # Test config completeness
        is_complete = config_manager.isConfigComplete(config)
        assert is_complete == True
        
        # Test config inheritance and merging
        merged_config = config_manager.mergeConfigs([template1, template2])
        assert merged_config is not None
        
        # Test sensitivity analysis
        sensitivity = config_manager.analyzeSensitivity(config)
        assert isinstance(sensitivity, dict)
        assert "brain.neuron_count" in sensitivity
        
        # Test sensitivity report
        sensitivity_report = config_manager.getSensitivityReport(config)
        assert isinstance(sensitivity_report, str)
        assert "Sensitivity Analysis Report" in sensitivity_report
        
        print("✓ ConfigManager comprehensive test passed")
    
    def test_serialization_manager_comprehensive(self):
        """Test SerializationManager comprehensive functionality."""
        # Test factory function
        serializer = pynlm.createSerializationManager()
        assert serializer is not None
        
        # Test HDF5 serialization
        datasets = {
            "neuron_activity": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0],
            "synaptic_weights": [0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05],
            "firing_rates": [10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0]
        }
        
        hdf5_result = serializer.saveHDF5("test.h5", self.test_config, datasets)
        assert hdf5_result == True
        
        # Test pickle serialization
        pickle_result = serializer.savePickle("test.pkl", self.test_config, self.test_brain)
        assert pickle_result == True
        
        # Test incremental serialization
        incremental_result = serializer.saveIncremental(
            "test_incremental.pkl", self.test_config, datasets
        )
        assert incremental_result == True
        
        # Test compressed serialization
        compressed_result = serializer.saveCompressed(
            "test_compressed.pkl.gz", self.test_config, self.test_brain
        )
        assert compressed_result == True
        
        # Test checkpoint management
        checkpoint1_result = serializer.createCheckpoint("checkpoint1", self.test_config, self.test_brain)
        assert checkpoint1_result == True
        
        checkpoint2_result = serializer.createCheckpoint("checkpoint2", self.test_config, self.test_brain)
        assert checkpoint2_result == True
        
        # Test checkpoint restoration
        restore_result = serializer.restoreCheckpoint("checkpoint1")
        assert restore_result == True
        
        # Test getting checkpoints
        checkpoints = serializer.getCheckpoints()
        assert "checkpoint1" in checkpoints
        assert "checkpoint2" in checkpoints
        
        # Test loading from different formats
        loaded_config, loaded_brain = serializer.loadPickle("test.pkl")
        assert loaded_config is not None
        assert loaded_brain is not None
        
        loaded_config2, loaded_data = serializer.loadIncremental("test_incremental.pkl")
        assert loaded_config2 is not None
        assert loaded_data is not None
        
        loaded_config3, loaded_brain3 = serializer.loadCompressed("test_compressed.pkl.gz")
        assert loaded_config3 is not None
        assert loaded_brain3 is not None
        
        print("✓ SerializationManager comprehensive test passed")
    
    def test_visualization_tools_comprehensive(self):
        """Test VisualizationTools comprehensive functionality."""
        # Test factory function
        visualizer = pynlm.createVisualizationTools()
        assert visualizer is not None
        
        # Create neurons and synapses for visualization
        manipulator = pynlm.createAdvancedBrainManipulator(self.test_brain)
        region_id = self.test_brain.addRegion("viz_test")
        
        neurons = []
        for i in range(10):
            neurons.append(manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory))
        
        for i in range(len(neurons) - 1):
            manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.5)
        
        # Test visualization methods
        network_result = visualizer.visualizeNetwork("network.png", self.test_brain, "png")
        assert network_result == True
        
        connections_result = visualizer.visualizeConnections("connections.png", self.test_brain, "png")
        assert connections_result == True
        
        activity_result = visualizer.visualizeActivity("activity.png", self.test_brain, "firing_rate", 0.5, "png")
        assert activity_result == True
        
        # Test monitoring tools
        monitor_result = visualizer.createActivityMonitor("monitor.html", self.test_brain)
        assert monitor_result == True
        
        realtime_result = visualizer.createRealTimeMonitor("realtime.html", self.test_brain, 1000)
        assert realtime_result == True
        
        # Test real-time plotting
        visualizer.startRealTimePlotting("plot.html", self.test_brain)
        visualizer.stopRealTimePlotting()
        
        # Test dashboard creation
        dashboard_result = visualizer.createResultsDashboard(
            "dashboard.html", [self.test_brain], ["Test Experiment"]
        )
        assert dashboard_result == True
        
        # Test comparison plots
        comparison_result = visualizer.createComparisonPlot(
            "comparison.png", [self.test_brain], "firing_rate"
        )
        assert comparison_result == True
        
        # Test training progress visualization
        training_data = [(0, 0.0), (100, 0.5), (200, 0.8), (300, 0.9), (400, 0.95)]
        training_result = visualizer.createTrainingProgress(
            "training_progress.png", training_data, "Training Progress"
        )
        assert training_result == True
        
        print("✓ VisualizationTools comprehensive test passed")
    
    def test_integration_advanced_features(self):
        """Test integration between all advanced features."""
        # Create all advanced feature components
        manipulator = pynlm.createAdvancedBrainManipulator(self.test_brain)
        monitor = pynlm.createPerformanceMonitor()
        recorder = pynlm.createMetricRecorder()
        profiler = pynlm.createProfiler()
        memory_monitor = pynlm.createMemoryMonitor()
        config_manager = pynlm.createConfigManager()
        serializer = pynlm.createSerializationManager()
        visualizer = pynlm.createVisualizationTools()
        
        # Use advanced brain manipulation
        region_id = self.test_brain.addRegion("integration_test")
        neuron1 = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neuron2 = manipulator.createNeuron(region_id, pynlm.NeuronType.Inhibitory)
        synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
        
        # Use performance monitoring
        monitor.start()
        profiler.startProfile("brain_simulation")
        
        # Run simulation with performance tracking
        for step in range(50):
            self.test_brain.step(step)
            monitor.recordStep(10.0, 3, 15)
            recorder.record("reward", 1.0 + (step * 0.01))
            profiler.recordMemoryUsage("simulation", 1024 * step)
        
        profiler.stopProfile("brain_simulation")
        monitor.stop()
        
        # Use advanced configuration
        template_config = pynlm.create_default_config()
        config_manager.addTemplate("integration", "Integration test", template_config)
        config = config_manager.createFromTemplate("integration")
        
        # Use serialization
        datasets = {"test": [1.0, 2.0, 3.0]}
        serializer.saveHDF5("integration_test.h5", config, datasets)
        serializer.createCheckpoint("integration_checkpoint", config, self.test_brain)
        
        # Use visualization
        viz_result = visualizer.visualizeNetwork("integration_network.png", self.test_brain)
        
        # Verify integration
        assert manipulator is not None
        assert monitor is not None
        assert recorder is not None
        assert profiler is not None
        assert config_manager is not None
        assert serializer is not None
        assert visualizer is not None
        assert viz_result == True
        
        # Test performance metrics
        assert monitor.getStepsPerSecond() > 0
        assert recorder.getMean("reward") > 0
        assert profiler.getProfileReport() is not None
        
        print("✓ Advanced features integration test passed")
    
    def run_all_tests(self):
        """Run all Phase 3 advanced feature tests."""
        print("=" * 60)
        print("PHASE 3 ADVANCED FEATURES - UNIT TESTS")
        print("=" * 60)
        
        try:
            test_advanced_features_structure()
            
            # Run comprehensive tests
            tester = TestPhase3Advanced()
            tester.test_advanced_brain_manipulator_comprehensive()
            tester.test_performance_monitor_comprehensive()
            tester.test_metric_recorder_comprehensive()
            tester.test_config_manager_comprehensive()
            tester.test_serialization_manager_comprehensive()
            tester.test_visualization_tools_comprehensive()
            tester.test_integration_advanced_features()
            
            print("\n" + "=" * 60)
            print("PHASE 3 TESTS PASSED")
            print("=" * 60)
            return True
            
        except Exception as e:
            print(f"\n✗ Phase 3 test failed: {e}")
            import traceback
            traceback.print_exc()
            return False
if __name__ == "__main__":
    tester = TestPhase3Advanced()
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)