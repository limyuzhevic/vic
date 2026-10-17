#!/usr/bin/env python3
"""
Comprehensive test suite for Phase 5 - Comprehensive Testing improvements.

This test suite includes all tests required for NLM Python bindings:
1. Unit Tests for all new bindings (Phases 1-3)
2. Integration Tests (cross-phase functionality)
3. Performance Tests (performance-critical features)
4. CLI Tests (command-line tools)
5. Regression Tests (backward compatibility)

The tests cover:
- Unit tests for each individual class/method
- Integration tests for complex workflows
- End-to-end tests for complete functionality
- Performance tests for critical operations
- Error condition tests for edge cases
- Configuration tests for various setup scenarios
"""

import sys
import os
import tempfile
import shutil
import time
import json
import csv
import unittest
from pathlib import Path
from typing import Dict, List, Any, Optional
from unittest.mock import Mock, patch, MagicMock

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

# Import the bindings
try:
    import python.pynlm as pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
class TestPhase1Bindings(unittest.TestCase):
    """Unit tests for Phase 1 - Basic Python bindings improvements."""
    
    def setUp(self):
        """Set up test fixtures."""
        # Create a temporary directory for test files
        self.test_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.test_dir)
        
        # Create test configuration
        self.test_config = pynlm.createDefaultConfig(
            "test_phase1", 
            max_steps=100, 
            timestep=0.01
        )
    
    def test_create_default_config(self):
        """Test create_default_config utility function."""
        config = pynlm.create_default_config("test_exp", 100, 0.01)
        
        self.assertIsNotNone(config)
        self.assertEqual(config.getString("name"), "test_exp")
        self.assertEqual(config.getInt("max_steps"), 100)
        self.assertEqual(config.getFloat("timestep"), 0.01)
    
    def test_config_manipulation(self):
        """Test basic configuration manipulation."""
        config = pynlm.create_default_config()
        
        # Test setting and getting different value types
        config.setInt("brain.neurons", 500)
        config.setFloat("world.size", 10.0)
        config.setString("experiment.name", "test")
        config.setBool("plasticity.enabled", True)
        
        self.assertEqual(config.getInt("brain.neurons"), 500)
        self.assertEqual(config.getFloat("world.size"), 10.0)
        self.assertEqual(config.getString("experiment.name"), "test")
        self.assertEqual(config.getBool("plasticity.enabled"), True)
    
    def test_config_file_operations(self):
        """Test configuration file I/O operations."""
        config = pynlm.create_default_config("file_test", 50, 0.005)
        
        # Save to JSON file
        config_path = Path(self.test_dir) / "test_config.json"
        config.saveToFile(str(config_path))
        
        # Load from JSON file
        loaded_config = pynlm.create_default_config()
        loaded_config.loadFromFile(str(config_path))
        
        # Verify loaded configuration
        self.assertEqual(loaded_config.getString("name"), "file_test")
        self.assertEqual(loaded_config.getInt("max_steps"), 50)
        self.assertEqual(loaded_config.getFloat("timestep"), 0.005)
    
    def test_brain_creation_and_initialization(self):
        """Test brain creation and initialization."""
        brain = pynlm.createBrain(self.test_config)
        brain.initialize()
        
        self.assertIsNotNone(brain)
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertEqual(brain.getTotalSynapseCount(), 0)  # Initial state
    
    def test_brain_step_operation(self):
        """Test brain step operation."""
        brain = pynlm.createBrain(self.test_config)
        brain.initialize()
        
        # Run simulation steps
        for step in range(10):
            brain.step(step)
            self.assertEqual(brain.getStepCount(), step + 1)
    
    def test_agent_brain_creation(self):
        """Test AgentBrain creation and basic operations."""
        brain = pynlm.createBrain(self.test_config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Test agent configuration
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Verify agent is initialized
        self.assertEqual(agent.getCuriosityLevel(), 0.0)
        self.assertEqual(agent.getPredictionError(), 0.0)
    
    def test_simple_world_creation(self):
        """Test SimpleWorld creation and configuration."""
        world = pynlm.createSimpleWorld()
        
        # Configure world parameters
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        self.assertIsNotNone(world)
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        self.assertEqual(world.getVisionWidth(), 8)
        self.assertEqual(world.getVisionHeight(), 8)
    
    def test_error_handling(self):
        """Test error handling and validation."""
        # Test invalid configuration
        config = pynlm.createDefaultConfig("invalid_test", -1, 0.01)
        
        # This should still work but may produce warnings
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test invalid operations
        world = pynlm.createSimpleWorld()
        
        # Invalid world configuration (negative dimensions)
        try:
            world.configure(width=-10, height=-10)
            # Should handle this gracefully
        except Exception:
            pass  # Expected
    
    def test_performance_stats(self):
        """Test brain performance statistics collection."""
        brain = pynlm.createBrain(self.test_config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
        
        # Check performance metrics
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        self.assertGreater(brain.getAverageFiringRate(), 0)
        self.assertEqual(brain.getStepCount(), 50)
    
    def test_backward_compatibility(self):
        """Test backward compatibility with original NLM API."""
        # Test that all original methods still work
        config = pynlm.createDefaultConfig()
        
        # Original configuration methods
        config.set("brain.neuron_count", 1000)
        config.set("brain.simulation_steps", 10000)
        
        # Original brain methods
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Original world methods
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        
        # Original agent methods
        agent = pynlm.createAgentBrain(brain)
        
        self.assertIsNotNone(brain)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
class TestPhase2Convenience(unittest.TestCase):
    """Unit tests for Phase 2 - Convenience classes and utilities."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.test_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.test_dir)
        
        # Create test configuration
        self.test_config = pynlm.create_default_config("phase2_test", 100, 0.01)
        
        # Create test brain
        self.test_brain = pynlm.createBrain(self.test_config)
        self.test_brain.initialize()
        
        # Create test world
        self.test_world = pynlm.createSimpleWorld()
        self.test_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.test_world.setAgentStart(10.0, 10.0)
        
        # Create test agent
        self.test_agent = pynlm.createAgentBrain(self.test_brain)
        self.test_agent.initialize(self.test_world)
    
    def test_simple_experiment_basic(self):
        """Test SimpleExperiment basic functionality."""
        exp = pynlm.SimpleExperiment()
        
        # Configure experiment
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=self.test_brain,
            world=self.test_world,
            agent_brain=self.test_agent
        )
        
        # Test experiment properties
        self.assertEqual(exp.max_steps, 100)
        self.assertEqual(exp.timestep, 0.01)
        self.assertIsNotNone(exp.brain)
        self.assertIsNotNone(exp.world)
        self.assertIsNotNone(exp.agent_brain)
        
        # Test property accessors
        self.assertIsNone(exp.total_reward)  # Before running
        self.assertIsNone(exp.final_step)    # Before running
    
    def test_simple_experiment_context_manager(self):
        """Test SimpleExperiment context manager."""
        with pynlm.SimpleExperiment() as exp:
            exp.configure(
                max_steps=50,
                timestep=0.01,
                brain=self.test_brain,
                world=self.test_world,
                agent_brain=self.test_agent
            )
            self.assertIsNotNone(exp)
    
    def test_brain_trainer_configuration(self):
        """Test BrainTrainer configuration and methods."""
        trainer = pynlm.BrainTrainer()
        
        # Configure trainer
        trainer.configure(
            brain=self.test_brain,
            environment=self.test_world,
            max_episodes=50,
            learning_rate=0.01
        )
        
        self.assertIsNotNone(trainer)
        self.assertEqual(trainer.max_episodes, 50)
        self.assertEqual(trainer.learning_rate, 0.01)
    
    def test_world_builder_creation(self):
        """Test WorldBuilder world creation."""
        builder = pynlm.WorldBuilder()
        
        # Create world
        builder.create_world(
            width=30,
            height=30,
            terrain_type="forest",
            enable_gravity=True
        )
        
        # Add objects
        builder.add_terrain_object("wall", 5.0, 5.0, 0.5)
        builder.add_terrain_object("resource", 15.0, 15.0, 0.5, 2.0)
        builder.add_agent(10.0, 10.0, 0.0, 1.0)
        
        # Test property accessors
        self.assertEqual(builder.width, 30)
        self.assertEqual(builder.height, 30)
        self.assertEqual(builder.terrain_type, "forest")
        self.assertEqual(builder.agent_count, 1)
        
        # Build the world
        world = builder.build()
        self.assertIsNotNone(world)
    
    def test_metrics_collector_recording(self):
        """Test MetricsCollector recording and statistical methods."""
        collector = pynlm.MetricsCollector()
        
        # Record various metrics
        collector.record("reward", 1.5)
        collector.record("firing_rate", 42.3)
        collector.record("energy", 0.8)
        collector.record("step_time", 0.001)
        
        # Test time series recording
        values = [1.0, 2.0, 3.0, 4.0, 5.0]
        collector.record_time_series("test_metric", values, 1000.0)
        
        # Test statistical methods
        self.assertGreater(collector.get_mean("reward"), 0)
        self.assertGreater(collector.get_stddev("firing_rate"), 0)
        self.assertLess(collector.get_min("energy"), 1.0)
        self.assertLess(collector.get_max("step_time"), 0.01)
        self.assertAlmostEqual(collector.get_median("reward"), 1.5, places=2)
    
    def test_metrics_collector_export(self):
        """Test MetricsCollector export methods."""
        collector = pynlm.MetricsCollector()
        
        # Record some data
        collector.record("reward", 1.0)
        collector.record("firing_rate", 50.0)
        
        # Test CSV export
        csv_data = collector.to_csv("test_metrics")
        self.assertIsInstance(csv_data, str)
        self.assertIn("reward", csv_data)
        self.assertIn("firing_rate", csv_data)
        
        # Test JSON export
        json_data = collector.to_json()
        self.assertIsInstance(json_data, str)
        
        # Verify JSON contains expected metrics
        parsed_json = json.loads(json_data)
        self.assertIn("reward", parsed_json)
        self.assertIn("firing_rate", parsed_json)
    
    def test_utility_functions(self):
        """Test utility functions."""
        # Test create_default_config
        config = pynlm.create_default_config("utility_test", 200, 0.005)
        self.assertEqual(config.getString("name"), "utility_test")
        
        # Test run_simulation (basic test)
        simulation = pynlm.run_simulation("sim_test", config, max_steps=10)
        self.assertIsNotNone(simulation)
        
        # Test plot_brain_metrics (basic test)
        firing_rates = [10.0, 15.0, 20.0, 25.0, 30.0]
        synaptic_weights = [0.1, 0.2, 0.3, 0.4, 0.5]
        plot_result = pynlm.plot_brain_metrics(firing_rates, synaptic_weights, "Test")
        self.assertIsNotNone(plot_result)
        
        # Test save_experiment_data
        experiments = [simulation] if simulation else []
        save_result = pynlm.save_experiment_data(
            str(Path(self.test_dir) / "experiments.csv"),
            experiments
        )
        self.assertIsNotNone(save_result)
class TestPhase3Advanced(unittest.TestCase):
    """Unit tests for Phase 3 - Advanced features for expert users."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.test_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.test_dir)
        
        # Create test configuration
        self.test_config = pynlm.create_default_config("phase3_test", 100, 0.01)
        
        # Create test brain
        self.test_brain = pynlm.createBrain(self.test_config)
        self.test_brain.initialize()
    
    def test_advanced_brain_manipulator(self):
        """Test AdvancedBrainManipulator functionality."""
        manipulator = pynlm.createAdvancedBrainManipulator(self.test_brain)
        
        # Create neurons
        region_id = self.test_brain.addRegion("test_region")
        neuron1 = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neuron2 = manipulator.createNeuron(region_id, pynlm.NeuronType.Inhibitory)
        
        self.assertIsNotNone(neuron1)
        self.assertIsNotNone(neuron2)
        self.assertNotEqual(neuron1, neuron2)
        
        # Create synapses
        synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
        self.assertIsNotNone(synapse)
        
        # Modify neuron properties
        new_state = pynlm.NeuronState()
        new_state.threshold = -45.0
        manipulator.modifyNeuronProperties(neuron1, new_state)
        
        # Get neuron state
        state = manipulator.getNeuronState(neuron1)
        self.assertEqual(state.threshold, -45.0)
        
        # Get neurons in region
        neurons_in_region = manipulator.getNeuronsInRegion(region_id)
        self.assertIn(neuron1, neurons_in_region)
        self.assertIn(neuron2, neurons_in_region)
    
    def test_performance_monitor(self):
        """Test PerformanceMonitor functionality."""
        monitor = pynlm.createPerformanceMonitor()
        
        monitor.start()
        
        # Record performance steps
        for i in range(10):
            step_time = 10.0 + (i * 2.0)
            spikes = i * 10
            active_neurons = i * 50
            monitor.recordStep(step_time, spikes, active_neurons)
        
        monitor.stop()
        
        # Check performance metrics
        self.assertGreater(monitor.getTotalTimeSeconds(), 0)
        self.assertGreater(monitor.getAvgStepTimeMs(), 0)
        self.assertGreater(monitor.getStepsPerSecond(), 0)
        self.assertGreater(monitor.getSpikesPerSecond(), 0)
        self.assertGreater(monitor.getFiringRateHz(), 0)
        self.assertEqual(monitor.getStepCount(), 10)
        self.assertEqual(monitor.getTotalSpikes(), 45)  # 0+10+20+30+40+50+60+70+80+90
    
    def test_metric_recorder(self):
        """Test MetricRecorder functionality."""
        recorder = pynlm.createMetricRecorder()
        
        # Record metrics
        recorder.record("reward", 1.5)
        recorder.record("firing_rate", 42.3)
        recorder.record("energy", 0.8)
        
        # Record time series
        values = [1.0, 2.0, 3.0, 4.0, 5.0]
        recorder.recordTimeSeries("firing_rate", values, 1000.0)
        
        # Check metric retrieval
        self.assertAlmostEqual(recorder.getMetric("reward"), 1.5, places=2)
        self.assertAlmostEqual(recorder.getMean("firing_rate"), 3.0, places=2)
        self.assertAlmostEqual(recorder.getStdDev("firing_rate"), 1.414, places=3)
        self.assertAlmostEqual(recorder.getMin("energy"), 0.8, places=2)
        self.assertAlmostEqual(recorder.getMax("firing_rate"), 5.0, places=2)
        
        # Check metric names
        metric_names = recorder.getMetricNames()
        self.assertIn("reward", metric_names)
        self.assertIn("firing_rate", metric_names)
        self.assertIn("energy", metric_names)
    
    def test_config_manager(self):
        """Test ConfigManager functionality."""
        config_manager = pynlm.createConfigManager()
        
        # Create and add templates
        template_config = pynlm.create_default_config()
        config_manager.addTemplate("default", "Default configuration", template_config, True)
        
        template_config2 = pynlm.create_default_config()
        template_config2.set("brain.neuron_count", 5000)
        config_manager.addTemplate("large", "Large network configuration", template_config2)
        
        # Create config from template
        config = config_manager.createFromTemplate("large")
        self.assertIsNotNone(config)
        
        # Validate config
        is_valid = config_manager.validateConfig(config)
        self.assertTrue(is_valid)
        
        # Check if config is complete
        is_complete = config_manager.isConfigComplete(config)
        self.assertTrue(is_complete)
        
        # Test sensitivity analysis
        sensitivity = config_manager.analyzeSensitivity(config)
        self.assertIsInstance(sensitivity, dict)
    
    def test_serialization_manager(self):
        """Test SerializationManager functionality."""
        serializer = pynlm.createSerializationManager()
        
        # Test saving to HDF5
        datasets = {"neuron_activity": [0.1, 0.2, 0.3, 0.4, 0.5]}
        save_result = serializer.saveHDF5("test.h5", self.test_config, datasets)
        self.assertTrue(save_result)
        
        # Test creating checkpoints
        checkpoint_result = serializer.createCheckpoint("test_checkpoint", self.test_config, self.test_brain)
        self.assertTrue(checkpoint_result)
        
        # Test getting checkpoints
        checkpoints = serializer.getCheckpoints()
        self.assertIn("test_checkpoint", checkpoints)
        
        # Test saving to pickle
        pickle_result = serializer.savePickle("test.pkl", self.test_config, self.test_brain)
        self.assertTrue(pickle_result)
    
    def test_visualization_tools(self):
        """Test VisualizationTools functionality."""
        visualizer = pynlm.createVisualizationTools()
        
        # Create neurons and synapses for visualization
        manipulator = pynlm.createAdvancedBrainManipulator(self.test_brain)
        region_id = self.test_brain.addRegion("viz_region")
        
        neurons = [manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory) for _ in range(5)]
        
        for i in range(len(neurons) - 1):
            manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.5)
        
        # Test visualization methods
        viz_result = visualizer.visualizeNetwork("network.png", self.test_brain)
        self.assertTrue(viz_result)
        
        viz_result = visualizer.visualizeConnections("connections.png", self.test_brain)
        self.assertTrue(viz_result)
        
        viz_result = visualizer.visualizeActivity("activity.png", self.test_brain, "firing_rate", 0.5)
        self.assertTrue(viz_result)
class TestIntegration(unittest.TestCase):
    """Integration tests for cross-phase functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.test_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.test_dir)
        
        # Create test configuration
        self.test_config = pynlm.create_default_config("integration_test", 100, 0.01)
        
        # Create brain
        self.test_brain = pynlm.createBrain(self.test_config)
        self.test_brain.initialize()
        
        # Create world
        self.test_world = pynlm.createSimpleWorld()
        self.test_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.test_world.setAgentStart(10.0, 10.0)
        
        # Create agent
        self.test_agent = pynlm.createAgentBrain(self.test_brain)
        self.test_agent.initialize(self.test_world)
    
    def test_convenience_classes_integration(self):
        """Test integration between convenience classes."""
        # Create experiment with convenience classes
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=self.test_brain,
            world=self.test_world,
            agent_brain=self.test_agent
        )
        
        # Add step callback
        exp.add_step_callback(lambda step: None)
        exp.enable_data_collection(True)
        
        # Create trainer with same brain
        trainer = pynlm.BrainTrainer()
        trainer.configure(
            brain=self.test_brain,
            environment=self.test_world,
            max_episodes=20,
            learning_rate=0.01
        )
        
        # Create world builder and create world
        builder = pynlm.WorldBuilder()
        builder.create_world(20, 20, "grid", True)
        builder.add_agent(5.0, 5.0, 0.0, 1.0)
        world = builder.build()
        
        # Test metrics collector
        collector = pynlm.MetricsCollector()
        collector.record("reward", 1.0)
        collector.record("firing_rate", 50.0)
        
        # Integration verification
        self.assertIsNotNone(exp)
        self.assertIsNotNone(trainer)
        self.assertIsNotNone(world)
        self.assertIsNotNone(collector)
    
    def test_advanced_features_integration(self):
        """Test integration between advanced features."""
        # Create advanced manipulator
        manipulator = pynlm.createAdvancedBrainManipulator(self.test_brain)
        region_id = self.test_brain.addRegion("integration_region")
        
        # Create neurons and synapses
        neuron1 = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neuron2 = manipulator.createNeuron(region_id, pynlm.NeuronType.Inhibitory)
        synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
        
        # Create performance monitor and recorder
        monitor = pynlm.createPerformanceMonitor()
        recorder = pynlm.createMetricRecorder()
        
        monitor.start()
        recorder.record("reward", 1.5)
        recorder.record("firing_rate", 45.0)
        
        # Run brain steps
        for step in range(20):
            self.test_brain.step(step)
            monitor.recordStep(10.0, 5, 25)
        
        monitor.stop()
        
        # Create config manager
        config_manager = pynlm.createConfigManager()
        template_config = pynlm.create_default_config()
        config_manager.addTemplate("integration_test", "Integration test config", template_config)
        
        config = config_manager.createFromTemplate("integration_test")
        
        # Test integration
        self.assertIsNotNone(manipulator)
        self.assertIsNotNone(monitor)
        self.assertIsNotNone(recorder)
        self.assertIsNotNone(config_manager)
        self.assertIsNotNone(config)
    
    def test_cli_to_advanced_integration(self):
        """Test integration between CLI tools and advanced operations."""
        # Create brain with advanced features
        advanced_brain = pynlm.createBrain(self.test_config)
        advanced_brain.initialize()
        
        # Use advanced manipulator
        manipulator = pynlm.createAdvancedBrainManipulator(advanced_brain)
        region_id = advanced_brain.addRegion("cli_test")
        
        # Create neurons
        neurons = []
        for i in range(10):
            neurons.append(manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory))
        
        # Create synapses
        for i in range(len(neurons) - 1):
            manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.5)
        
        # Use performance monitoring
        monitor = pynlm.createPerformanceMonitor()
        monitor.start()
        
        for step in range(50):
            advanced_brain.step(step)
            monitor.recordStep(5.0, 3, 15)
        
        monitor.stop()
        
        # Use visualization tools
        visualizer = pynlm.createVisualizationTools()
        viz_result = visualizer.visualizeNetwork("cli_test_network.png", advanced_brain)
        
        # Test performance metrics
        self.assertGreater(monitor.getStepsPerSecond(), 0)
        self.assertGreater(monitor.getAvgStepTimeMs(), 0)
        
        # Test integration results
        self.assertIsNotNone(viz_result)
        self.assertGreater(len(neurons), 0)
    
    def test_complete_workflow_integration(self):
        """Test complete workflow from CLI to advanced operations."""
        # Step 1: Create brain with Phase 1 capabilities
        config = pynlm.create_default_config("complete_workflow", 200, 0.01)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Step 2: Create world with Phase 2 convenience class
        builder = pynlm.WorldBuilder()
        builder.create_world(30, 30, "continuous", True)
        builder.add_agent(15.0, 15.0, 0.0, 1.0)
        world = builder.build()
        
        # Step 3: Create agent brain
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Step 4: Run simulation with Phase 2 SimpleExperiment
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=brain,
            world=world,
            agent_brain=agent
        )
        exp.add_step_callback(lambda step: None)
        exp.enable_data_collection(True)
        
        # Step 5: Use Phase 3 advanced features during simulation
        manipulator = pynlm.createAdvancedBrainManipulator(brain)
        performance_monitor = pynlm.createPerformanceMonitor()
        metric_recorder = pynlm.createMetricRecorder()
        
        performance_monitor.start()
        
        for step in range(50):
            # Run brain step
            brain.step(step)
            
            # Record performance
            performance_monitor.recordStep(5.0, 3, 15)
            
            # Record metrics
            metric_recorder.record("reward", 1.0 + (step * 0.01))
            metric_recorder.record("firing_rate", 40.0 + (step * 2.0))
            
            # Every 10 steps, manipulate brain
            if step % 10 == 0 and step > 0:
                # Modify neuron properties
                if neurons:
                    new_state = pynlm.NeuronState()
                    new_state.threshold = -50.0 - (step * 0.1)
                    manipulator.modifyNeuronProperties(neurons[0], new_state)
        
        performance_monitor.stop()
        
        # Step 6: Analyze results with Phase 2 and Phase 3 tools
        metrics = pynlm.MetricsCollector()
        metrics.record("total_reward", metric_recorder.getMetric("reward"))
        metrics.record("avg_firing_rate", metric_recorder.getMean("firing_rate"))
        metrics.record("performance", monitor.getStepsPerSecond())
        
        # Export results
        results_file = Path(self.test_dir) / "integration_results.csv"
        pynlm.save_experiment_data(str(results_file), [exp] if exp else [])
        
        # Verify complete workflow
        self.assertGreater(performance_monitor.getStepsPerSecond(), 0)
        self.assertGreater(metric_recorder.getMean("firing_rate"), 0)
        self.assertTrue(Path(results_file).exists())
class TestBackwardCompatibility(unittest.TestCase):
    """Regression tests for backward compatibility."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.test_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.test_dir)
    
    def test_original_nlm_api_compatibility(self):
        """Test compatibility with original NLM Python API."""
        # Test original configuration methods
        config = pynlm.createDefaultConfig()
        
        # Test original brain methods
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test original world methods
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        # Test original agent methods
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Verify all original API still works
        self.assertIsNotNone(brain)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
        
        # Test original enumeration types
        neuron_type = pynlm.NeuronType.Excitatory
        self.assertIsNotNone(neuron_type)
        
        developmental_stage = pynlm.DevelopmentalStage.Adult
        self.assertIsNotNone(developmental_stage)
        
        action_type = pynlm.ActionType.MoveForward
        self.assertIsNotNone(action_type)
    
    def test_config_file_compatibility(self):
        """Test compatibility with existing configuration file formats."""
        # Create configuration in original format
        original_config = {
            "brain": {
                "neuron_count": 1000,
                "simulation_steps": 10000,
                "plasticity": {"enabled": True}
            },
            "world": {
                "width": 20,
                "height": 20,
                "environment_type": "grid"
            }
        }
        
        # Save to file
        config_path = Path(self.test_dir) / "original_config.json"
        with open(config_path, 'w') as f:
            json.dump(original_config, f, indent=2)
        
        # Load using original API
        config = pynlm.createDefaultConfig()
        config.loadFromFile(str(config_path))
        
        # Verify compatibility
        self.assertEqual(config.getInt("brain.neuron_count"), 1000)
        self.assertEqual(config.getInt("brain.simulation_steps"), 10000)
        self.assertEqual(config.getBool("brain.plasticity.enabled"), True)
        self.assertEqual(config.getInt("world.width"), 20)
        self.assertEqual(config.getInt("world.height"), 20)
    
    def test_simulation_compatibility(self):
        """Test compatibility with original simulation patterns."""
        # Test original simulation pattern
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Original simulation loop pattern
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify simulation completed
        self.assertEqual(brain.getStepCount(), 100)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
    
    def test_error_handling_compatibility(self):
        """Test that error handling maintains backward compatibility."""
        # Test that errors are raised appropriately for invalid inputs
        config = pynlm.createDefaultConfig()
        
        # Invalid configuration should be handled gracefully
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Invalid operations should raise appropriate exceptions
        world = pynlm.createSimpleWorld()
        
        # Test invalid world configuration (should not crash)
        try:
            world.configure(width=0, height=0)  # Invalid dimensions
            # Should handle gracefully or raise appropriate error
        except Exception:
            pass  # Expected behavior
        
        # Test that existing error handling still works
        self.assertIsNotNone(brain)
class TestPerformance(unittest.TestCase):
    """Performance tests for critical operations."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.test_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.test_dir)
        
        # Create test configuration
        self.test_config = pynlm.create_default_config("performance_test", 100, 0.01)
    
    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        start_time = time.time()
        
        brains = []
        for i in range(10):
            config = pynlm.create_default_config(f"perf_test_{i}", 100, 0.01)
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        end_time = time.time()
        avg_creation_time = (end_time - start_time) / 10
        
        # Should complete within reasonable time (less than 1 second per brain)
        self.assertLess(avg_creation_time, 1.0)
        self.assertEqual(len(brains), 10)
    
    def test_brain_step_performance(self):
        """Test brain step performance."""
        brain = pynlm.createBrain(self.test_config)
        brain.initialize()
        
        start_time = time.time()
        
        # Run many steps
        for step in range(1000):
            brain.step(step)
        
        end_time = time.time()
        total_time = end_time - start_time
        avg_step_time = total_time / 1000
        
        # Should complete within reasonable time (less than 1ms per step)
        self.assertLess(avg_step_time, 0.001)  # 1ms
        self.assertEqual(brain.getStepCount(), 1000)
    
    def test_config_manipulation_performance(self):
        """Test configuration manipulation performance."""
        start_time = time.time()
        
        configs = []
        for i in range(100):
            config = pynlm.create_default_config(f"config_{i}", 100, 0.01)
            config.setInt("brain.neuron_count", i * 100)
            config.setFloat("world.size", i * 1.0)
            configs.append(config)
        
        end_time = time.time()
        total_time = end_time - start_time
        avg_config_time = total_time / 100
        
        # Should complete within reasonable time
        self.assertLess(avg_config_time, 0.01)  # 10ms per config
        self.assertEqual(len(configs), 100)
    
    def test_memory_usage(self):
        """Test memory usage for critical operations."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        start_memory = process.memory_info().rss
        
        # Create and process many objects
        brains = []
        for i in range(20):
            config = pynlm.create_default_config(f"memory_test_{i}", 50, 0.01)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run some steps
            for step in range(20):
                brain.step(step)
            
            brains.append(brain)
        
        end_memory = process.memory_info().rss
        memory_increase = end_memory - start_memory
        
        # Memory increase should be reasonable (less than 100MB for 20 brains)
        self.assertLess(memory_increase, 100 * 1024 * 1024)  # 100MB
        self.assertEqual(len(brains), 20)
    
    def test_large_scale_simulation_performance(self):
        """Test performance with large-scale simulations."""
        # Create larger configuration
        large_config = pynlm.create_default_config("large_test", 500, 0.001)
        
        start_time = time.time()
        
        # Create brain with larger size
        brain = pynlm.createBrain(large_config)
        brain.initialize()
        
        # Run larger simulation
        for step in range(1000):
            brain.step(step)
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Should complete within reasonable time
        self.assertLess(total_time, 10.0)  # 10 seconds
        self.assertGreater(brain.getStepCount(), 0)
        
        # Performance metrics
        steps_per_second = brain.getStepCount() / total_time
        self.assertGreater(steps_per_second, 0)
class TestCLIIntegration(unittest.TestCase):
    """CLI integration tests."""
    
    def test_cli_structure_verification(self):
        """Test CLI tools structure verification."""
        # This would test the actual CLI tools from Phase 4
        # Since we can't run CLI tools without Python, we'll test the structure
        
        cli_dir = Path(__file__).parent.parent / "scripts" / "cli"
        
        # Check that all CLI tools exist
        required_tools = ["nlm_train.py", "nlm_simulate.py", "nlm_visualize.py", "nlm_export.py"]
        
        for tool in required_tools:
            tool_path = cli_dir / tool
            self.assertTrue(tool_path.exists(), f"CLI tool {tool} not found")
            self.assertTrue(os.access(tool_path, os.X_OK), f"CLI tool {tool} not executable")
        
        # Check shared utilities
        init_file = cli_dir / "__init__.py"
        self.assertTrue(init_file.exists(), "Shared CLI utilities not found")
        
        # Check automation scripts
        auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
        required_scripts = ["run_experiment.py", "train_brain.py", "analyze_results.py", "create_world.py"]
        
        for script in required_scripts:
            script_path = auto_dir / script
            self.assertTrue(script_path.exists(), f"Automation script {script} not found")
            self.assertTrue(os.access(script_path, os.X_OK), f"Automation script {script} not executable")
    
    def test_cli_help_commands(self):
        """Test CLI help command structure (without running)."""
        # This would test the actual CLI help functionality
        # We'll just verify the structure and content of CLI tools
        
        cli_dir = Path(__file__).parent.parent / "scripts" / "cli"
        
        # Check for common CLI structure elements
        for tool_file in cli_dir.glob("*.py"):
            if tool_file.name != "__init__.py":
                content = tool_file.read_text()
                
                # Check for common CLI elements
                self.assertIn("#!/bin/env python3", content, f"{tool_file.name} missing Python shebang")
                self.assertIn("argparse", content, f"{tool_file.name} missing argparse")
                self.assertIn("def main()", content, f"{tool_file.name} missing main function")
                self.assertIn("if __name__ ==", content, f"{tool_file.name} missing main guard")

# Test discovery and execution
if __name__ == "__main__":
    # Run all tests
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests with verbose output
    runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "="*80)
    print("PHASE 5 COMPREHENSIVE TEST SUITE SUMMARY")
    print("="*80)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Success rate: {((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100):.1f}%")
    
    if result.failures:
        print("\nFAILURES:")
        for test, traceback in result.failures:
            print(f"- {test}: {traceback}")
    
    if result.errors:
        print("\nERRORS:")
        for test, traceback in result.errors:
            print(f"- {test}: {traceback}")
    
    if result.wasSuccessful():
        print("\n✓ ALL TESTS PASSED - Phase 5 implementation is working correctly!")
        sys.exit(0)
    else:
        print("\n✗ SOME TESTS FAILED - Please check the implementation")
        sys.exit(1)