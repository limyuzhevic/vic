#!/usr/bin/env python3
"""
Integration tests for cross-phase functionality.

This test file covers:
- Tests that verify integration between convenience classes and advanced features
- Tests that verify integration between CLI tools and Python bindings
- Tests that verify the complete workflow from CLI to advanced operations
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
def test_convenience_to_advanced_integration():
    """Test integration between convenience classes (Phase 2) and advanced features (Phase 3)."""
    print("=" * 60)
    print("PHASE 2 TO PHASE 3 - CONVENIENCE TO ADVANCED INTEGRATION")
    print("=" * 60)
    
    try:
        # Setup test environment
        test_config = pynlm.create_default_config("integration_test", 100, 0.01)
        test_brain = pynlm.createBrain(test_config)
        test_brain.initialize()
        
        test_world = pynlm.createSimpleWorld()
        test_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        test_world.setAgentStart(10.0, 10.0)
        
        test_agent = pynlm.createAgentBrain(test_brain)
        test_agent.initialize(test_world)
        
        # Test 1: Integration between SimpleExperiment and AdvancedBrainManipulator
        print("\nTest 1: SimpleExperiment + AdvancedBrainManipulator")
        
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=50,
            timestep=0.01,
            brain=test_brain,
            world=test_world,
            agent_brain=test_agent
        )
        
        # Use convenience class features
        exp.add_step_callback(lambda step: None)
        exp.enable_data_collection(True)
        
        # Integrate with advanced brain manipulation
        manipulator = pynlm.createAdvancedBrainManipulator(test_brain)
        region_id = test_brain.addRegion("exp_region")
        
        neuron1 = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neuron2 = manipulator.createNeuron(region_id, pynlm.NeuronType.Inhibitory)
        synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
        
        # Use convenience class metrics
        metrics_collector = pynlm.MetricsCollector()
        metrics_collector.record("exp_simulation", exp.max_steps)
        metrics_collector.record("manipulator_neurons", 2)
        metrics_collector.record("manipulator_synapses", 1)
        
        print("✓ SimpleExperiment integrates with AdvancedBrainManipulator")
        
        # Test 2: Integration between BrainTrainer and PerformanceMonitor
        print("\nTest 2: BrainTrainer + PerformanceMonitor")
        
        trainer = pynlm.BrainTrainer()
        trainer.configure(
            brain=test_brain,
            environment=test_world,
            max_episodes=20,
            learning_rate=0.01
        )
        
        # Add reward function
        trainer.add_reward_function(lambda b, s: 1.0)
        
        # Integrate with advanced performance monitoring
        monitor = pynlm.createPerformanceMonitor()
        monitor.start()
        
        for episode in range(5):
            # Use convenience class training
            success = trainer.run_episode()
            
            # Integrate with advanced performance tracking
            monitor.recordStep(10.0, episode * 5, episode * 20)
        
        monitor.stop()
        
        # Use convenience class metrics
        trainer_metrics = pynlm.MetricsCollector()
        trainer_metrics.record("trainer_episodes", trainer.max_episodes)
        trainer_metrics.record("trainer_success_rate", trainer.get_successful_episodes() / trainer.get_current_episode() if trainer.get_current_episode() > 0 else 0)
        
        print("✓ BrainTrainer integrates with PerformanceMonitor")
        
        # Test 3: Integration between WorldBuilder and VisualizationTools
        print("\nTest 3: WorldBuilder + VisualizationTools")
        
        # Use convenience class world building
        builder = pynlm.WorldBuilder()
        builder.create_world(
            width=30,
            height=30,
            terrain_type="forest",
            enable_gravity=True
        )
        
        builder.add_terrain_object("resource", 10.0, 10.0, 0.5, 2.0)
        builder.add_terrain_object("hazard", 20.0, 20.0, 0.3, 1.0)
        builder.add_agent(5.0, 5.0, 0.0, 1.5)
        
        # Build the world
        world = builder.build()
        
        # Integrate with advanced visualization tools
        visualizer = pynlm.createVisualizationTools()
        
        # Visualize the built world
        viz_result = visualizer.visualizeNetwork("world_network.png", test_brain)
        
        # Create monitoring tools
        monitor_result = visualizer.createActivityMonitor("world_monitor.html", test_brain)
        
        # Use convenience class validation
        is_valid = builder.validate_world()
        assert is_valid == True
        
        validation_info = builder.get_validation_errors()
        
        print("✓ WorldBuilder integrates with VisualizationTools")
        
        # Test 4: Integration between MetricsCollector and ConfigManager
        print("\nTest 4: MetricsCollector + ConfigManager")
        
        # Use convenience class metrics collection
        metrics = pynlm.MetricsCollector()
        metrics.record("simulation_steps", 50)
        metrics.record("training_episodes", 20)
        metrics.record("world_objects", builder.agent_count + 2)
        
        # Integrate with advanced configuration management
        config_manager = pynlm.createConfigManager()
        
        # Create template for simulation configuration
        sim_config = pynlm.create_default_config("sim_config", 100, 0.01)
        sim_config.set("brain.neuron_count", 2000)
        sim_config.set("world.width", 30)
        
        config_manager.addTemplate("simulation_template", "Simulation configuration", sim_config)
        
        # Create configuration from template
        sim_template_config = config_manager.createFromTemplate("simulation_template")
        
        # Validate configuration
        is_sim_valid = config_manager.validateConfig(sim_template_config)
        assert is_sim_valid == True
        
        # Analyze sensitivity for metrics
        sensitivity = config_manager.analyzeSensitivity(sim_template_config)
        
        # Store metrics in config
        sim_template_config.set("metrics.steps_recorded", metrics.getMetric("simulation_steps"))
        sim_template_config.set("metrics.episodes_recorded", metrics.getMetric("training_episodes"))
        
        print("✓ MetricsCollector integrates with ConfigManager")
        
        # Test 5: Integration between convenience and serialization utilities
        print("\nTest 5: Convenience classes + SerializationManager")
        
        # Use convenience classes for experiment data
        experiment_data = {
            "experiment_name": "integration_test",
            "steps_completed": 50,
            "max_steps": 100,
            "success_rate": 0.8,
            "metrics": {
                "reward": metrics.get_mean("simulation_steps"),
                "firing_rate": 45.0,
                "efficiency": 0.95
            }
        }
        
        # Integrate with advanced serialization
        serializer = pynlm.createSerializationManager()
        
        # Save to HDF5
        datasets = {
            "neuron_activity": [0.1, 0.2, 0.3, 0.4, 0.5],
            "synaptic_weights": [0.9, 0.8, 0.7, 0.6, 0.5],
            "firing_rates": [10.0, 15.0, 20.0, 25.0, 30.0]
        }
        
        hdf5_result = serializer.saveHDF5("integration.h5", sim_template_config, datasets)
        assert hdf5_result == True
        
        # Create checkpoint
        checkpoint_result = serializer.createCheckpoint(
            "integration_checkpoint",
            sim_template_config,
            test_brain
        )
        assert checkpoint_result == True
        
        # Restore checkpoint
        restore_result = serializer.restoreCheckpoint("integration_checkpoint")
        assert restore_result == True
        
        print("✓ Convenience classes integrate with SerializationManager")
        
        print("\n" + "=" * 60)
        print("PHASE 2 TO PHASE 3 INTEGRATION TESTS PASSED")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ Phase 2 to Phase 3 integration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_cli_to_advanced_integration():
    """Test integration between CLI tools (Phase 4) and advanced operations (Phase 3)."""
    print("\n" + "=" * 60)
    print("CLI TOOLS TO ADVANCED FEATURES - CROSS-PHASE INTEGRATION")
    print("=" * 60)
    
    try:
        # Setup test environment
        test_config = pynlm.create_default_config("cli_integration_test", 200, 0.001)
        test_brain = pynlm.createBrain(test_config)
        test_brain.initialize()
        
        test_world = pynlm.createSimpleWorld()
        test_world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
        test_world.setAgentStart(20.0, 20.0)
        
        test_agent = pynlm.createAgentBrain(test_brain)
        test_agent.initialize(test_world)
        
        # Test 1: CLI tool simulation workflow
        print("\nTest 1: CLI simulation workflow integration")
        
        # Simulate CLI tool usage patterns
        # Step 1: Create experiment using convenience classes
        exp = pynlm.SimpleExperiment()
        exp.configure(
            max_steps=100,
            timestep=0.001,
            brain=test_brain,
            world=test_world,
            agent_brain=test_agent
        )
        
        # Step 2: Use advanced brain manipulation during simulation
        manipulator = pynlm.createAdvancedBrainManipulator(test_brain)
        region_id = test_brain.addRegion("cli_sim_region")
        
        neurons = []
        for i in range(5):
            neurons.append(manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory))
        
        for i in range(len(neurons) - 1):
            manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.5)
        
        # Step 3: Monitor performance with advanced tools
        monitor = pynlm.createPerformanceMonitor()
        monitor.start()
        
        # Step 4: Record metrics with advanced recorder
        recorder = pynlm.createMetricRecorder()
        
        # Step 5: Run simulation loop
        for step in range(75):
            # Basic simulation steps
            test_brain.step(step)
            
            # Advanced monitoring
            monitor.recordStep(
                5.0 + (step * 0.1),  # Variable step time
                step * 3,            # Variable spike count
                step * 10           # Variable active neurons
            )
            
            # Advanced metrics recording
            recorder.record("reward", 1.0 + (step * 0.001))
            recorder.record("firing_rate", 40.0 + (step * 0.5))
            recorder.record("efficiency", 0.95 - (step * 0.001))
        
        monitor.stop()
        
        # Step 6: Analyze results with advanced configuration
        config_manager = pynlm.createConfigManager()
        
        # Create performance analysis configuration
        analysis_config = pynlm.create_default_config("performance_analysis", 100, 0.001)
        analysis_config.set("analysis.metrics", ["reward", "firing_rate", "efficiency"])
        analysis_config.set("analysis.thresholds.reward", 1.5)
        analysis_config.set("analysis.thresholds.firing_rate", 50.0)
        
        config_manager.addTemplate("performance_analysis", "Performance analysis configuration", analysis_config)
        
        analysis_template = config_manager.createFromTemplate("performance_analysis")
        is_analysis_valid = config_manager.validateConfig(analysis_template)
        assert is_analysis_valid == True
        
        print("✓ CLI simulation workflow integrates with advanced features")
        
        # Test 2: CLI tool visualization workflow
        print("\nTest 2: CLI visualization workflow integration")
        
        # Create visualization tools
        visualizer = pynlm.createVisualizationTools()
        
        # Visualize brain state during simulation
        viz_result = visualizer.visualizeNetwork("cli_viz_network.png", test_brain)
        assert viz_result == True
        
        # Create activity monitoring
        monitor_result = visualizer.createActivityMonitor("cli_viz_monitor.html", test_brain)
        assert monitor_result == True
        
        # Create real-time plotting setup
        visualizer.startRealTimePlotting("cli_viz_plot.html", test_brain)
        visualizer.stopRealTimePlotting()
        
        # Visualize performance metrics
        performance_data = {
            "steps_per_second": monitor.getStepsPerSecond(),
            "avg_step_time": monitor.getAvgStepTimeMs(),
            "spikes_per_second": monitor.getSpikesPerSecond(),
            "firing_rate_hz": monitor.getFiringRateHz()
        }
        
        viz_result2 = visualizer.visualizeActivity(
            "cli_viz_performance.png",
            test_brain,
            "firing_rate",
            0.5,
            "png"
        )
        assert viz_result2 == True
        
        print("✓ CLI visualization workflow integrates with advanced features")
        
        # Test 3: CLI tool export workflow
        print("\nTest 3: CLI export workflow integration")
        
        # Create serialization tools
        serializer = pynlm.createSerializationManager()
        
        # Export experiment data
        experiment_results = {
            "experiment_name": "cli_export_test",
            "config": {
                "max_steps": 100,
                "timestep": 0.001,
                "neuron_count": 2000
            },
            "performance": {
                "steps_per_second": monitor.getStepsPerSecond(),
                "total_time": monitor.getTotalTimeSeconds(),
                "avg_step_time": monitor.getAvgStepTimeMs()
            },
            "metrics": {
                "final_reward": recorder.getMetric("reward"),
                "avg_firing_rate": recorder.getMean("firing_rate"),
                "min_efficiency": recorder.getMin("efficiency")
            }
        }
        
        # Export to multiple formats
        datasets = {
            "neuron_activity": [0.1, 0.2, 0.3, 0.4, 0.5],
            "synaptic_weights": [0.9, 0.8, 0.7, 0.6, 0.5],
            "firing_rates": [10.0, 15.0, 20.0, 25.0, 30.0]
        }
        
        # HDF5 export
        hdf5_result = serializer.saveHDF5("cli_export.h5", analysis_template, datasets)
        assert hdf5_result == True
        
        # Pickle export
        pickle_result = serializer.savePickle("cli_export.pkl", analysis_template, test_brain)
        assert pickle_result == True
        
        # Create checkpoint for export
        checkpoint_result = serializer.createCheckpoint("cli_export_checkpoint", analysis_template, test_brain)
        assert checkpoint_result == True
        
        print("✓ CLI export workflow integrates with advanced features")
        
        # Test 4: CLI tool configuration workflow
        print("\nTest 4: CLI configuration workflow integration")
        
        # Create advanced configuration
        full_config_manager = pynlm.createConfigManager()
        
        # Create base configuration
        base_config = pynlm.create_default_config("cli_config", 200, 0.001)
        base_config.set("brain.neuron_count", 3000)
        base_config.set("brain.simulation_steps", 20000)
        
        # Add multiple templates
        full_config_manager.addTemplate("base", "Base configuration", base_config, True)
        
        template1 = pynlm.create_default_config("template1", 100, 0.01)
        template1.set("world.width", 25)
        template1.set("world.height", 25)
        full_config_manager.addTemplate("grid_world", "Grid world configuration", template1)
        
        template2 = pynlm.create_default_config("template2", 150, 0.005)
        template2.set("brain.plasticity.enabled", True)
        full_config_manager.addTemplate("plastic_brain", "Plastic brain configuration", template2)
        
        # Create configurations from templates
        base_conf = full_config_manager.createFromTemplate("base")
        grid_conf = full_config_manager.createFromTemplate("grid_world")
        plastic_conf = full_config_manager.createFromTemplate("plastic_brain")
        
        # Validate all configurations
        base_valid = full_config_manager.validateConfig(base_conf)
        grid_valid = full_config_manager.validateConfig(grid_conf)
        plastic_valid = full_config_manager.validateConfig(plastic_conf)
        
        assert base_valid == True
        assert grid_valid == True
        assert plastic_valid == True
        
        # Create configuration inheritance
        inherited_conf = full_config_manager.createFromInheritance(["base", "grid_world"])
        
        # Analyze sensitivity for configuration
        sensitivity = full_config_manager.analyzeSensitivity(base_conf)
        
        print("✓ CLI configuration workflow integrates with advanced features")
        
        print("\n" + "=" * 60)
        print("CLI TOOLS TO ADVANCED FEATURES INTEGRATION TESTS PASSED")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ CLI tools to advanced features integration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_complete_workflow_integration():
    """Test complete workflow from CLI to advanced operations."""
    print("\n" + "=" * 60)
    print("COMPLETE WORKFLOW - CLI TO ADVANCED OPERATIONS")
    print("=" * 60)
    
    try:
        # Step 1: CLI tool setup with configuration
        print("\nStep 1: CLI tool setup and configuration")
        
        # Create configuration using advanced ConfigManager
        config_manager = pynlm.createConfigManager()
        
        # Create comprehensive configuration
        comprehensive_config = pynlm.create_default_config("complete_workflow", 500, 0.001)
        comprehensive_config.set("brain.neuron_count", 5000)
        comprehensive_config.set("brain.simulation_steps", 50000)
        comprehensive_config.set("world.width", 50)
        comprehensive_config.set("world.height", 50)
        comprehensive_config.set("world.vision_range", 10)
        comprehensive_config.set("plasticity.enabled", True)
        comprehensive_config.set("neuromod.dopamine.scale", 1.0)
        
        # Add configuration template
        config_manager.addTemplate("complete_template", "Complete workflow configuration", comprehensive_config, True)
        
        # Get configuration
        workflow_config = config_manager.createFromTemplate("complete_template")
        is_config_valid = config_manager.validateConfig(workflow_config)
        assert is_config_valid == True
        
        print("✓ Step 1: Configuration setup complete")
        
        # Step 2: Create brain with advanced features
        print("\nStep 2: Brain creation with advanced manipulation")
        
        brain = pynlm.createBrain(workflow_config)
        brain.initialize()
        
        # Use advanced brain manipulation
        manipulator = pynlm.createAdvancedBrainManipulator(brain)
        
        # Create brain regions
        visual_region = brain.addRegion("visual_cortex")
        motor_region = brain.addRegion("motor_cortex")
        sensory_region = brain.addRegion("sensory_cortex")
        
        # Create neurons with different types
        neurons = []
        for region, neuron_type in [(visual_region, pynlm.NeuronType.Excitatory),
                                   (motor_region, pynlm.NeuronType.Motor),
                                   (sensory_region, pynlm.NeuronType.Sensory)]:
            for i in range(10):
                neuron = manipulator.createNeuron(region, neuron_type)
                neurons.append(neuron)
        
        # Create complex synaptic connections
        for i in range(len(neurons) - 1):
            synapse = manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.3)
        
        print("✓ Step 2: Brain creation with advanced manipulation complete")
        
        # Step 3: Create environment with convenience classes
        print("\nStep 3: Environment creation with convenience classes")
        
        # Use WorldBuilder for complex environment setup
        world_builder = pynlm.WorldBuilder()
        
        world_builder.create_world(
            width=50,
            height=50,
            terrain_type="continuous",
            enable_gravity=True,
            enable_wind=True,
            weather="rainy"
        )
        
        # Add environment objects
        world_builder.add_terrain_object("resource", 10.0, 10.0, 0.5, 5.0)
        world_builder.add_terrain_object("hazard", 30.0, 30.0, 0.3, 2.0)
        world_builder.add_terrain_object("landmark", 25.0, 25.0, 0.2, 1.0)
        
        # Add multiple agents
        for i in range(3):
            world_builder.add_agent(
                10.0 + (i * 15.0),
                10.0 + (i * 15.0),
                0.0 + (i * 0.5),
                1.0 + (i * 0.2)
            )
        
        # Set world properties
        world_builder.set_weather("stormy")
        world_builder.set_time_of_day(14)
        world_builder.set_season("summer")
        
        # Build world
        world = world_builder.build()
        world_env = world_builder.build_environment()
        
        is_world_valid = world_builder.validate_world()
        assert is_world_valid == True
        
        print("✓ Step 3: Environment creation with convenience classes complete")
        
        # Step 4: Create agent with advanced configuration
        print("\nStep 4: Agent creation with advanced configuration")
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world_env)
        
        # Enable advanced agent features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Step 4: Agent creation with advanced configuration complete")
        
        # Step 5: Run simulation with comprehensive monitoring
        print("\nStep 5: Simulation with comprehensive monitoring")
        
        # Create advanced monitoring tools
        monitor = pynlm.createPerformanceMonitor()
        profiler = pynlm.createProfiler()
        recorder = pynlm.createMetricRecorder()
        
        # Start monitoring
        monitor.start()
        profiler.startProfile("main_simulation")
        
        # Create metrics collector for convenience
        metrics_collector = pynlm.createMetricsCollector()
        
        # Run main simulation loop
        for step in range(100):
            # Update world
            world_env.update(0.1)
            
            # Get sensory input
            percept = world_env.getSensoryPercept()
            
            # Process input in agent brain
            agent.processSensoryInput(percept)
            
            # Run brain step with advanced manipulation
            brain.step(step)
            
            # Use advanced brain manipulation during simulation
            if step % 10 == 0 and step > 0:
                # Modify neuron properties
                new_state = pynlm.NeuronState()
                new_state.threshold = -50.0 - (step * 0.1)
                new_state.firingRate = 15.0 + (step * 0.5)
                
                manipulator.modifyNeuronProperties(neurons[0], new_state)
            
            # Record performance with advanced tools
            monitor.recordStep(5.0 + (step * 0.1), step * 3, step * 10)
            profiler.recordMemoryUsage("simulation", 1024 * step)
            
            # Record metrics
            recorder.record("reward", 1.0 + (step * 0.01))
            recorder.record("firing_rate", 40.0 + (step * 0.5))
            recorder.record("efficiency", 0.95 - (step * 0.001))
            
            # Collect metrics with convenience class
            metrics_collector.record("step_count", step)
            metrics_collector.record("firing_neuron_count", brain.getFiringNeuronCount())
            metrics_collector.record("brain_efficiency", brain.getAverageFiringRate())
        
        profiler.stopProfile("main_simulation")
        monitor.stop()
        
        # Get performance reports
        performance_report = monitor.getPerformanceReport()
        profile_report = profiler.getProfileReport()
        
        print("✓ Step 5: Simulation with comprehensive monitoring complete")
        
        # Step 6: Analyze results with advanced tools
        print("\nStep 6: Results analysis with advanced tools")
        
        # Use ConfigManager for analysis configuration
        analysis_config = pynlm.create_default_config("analysis_config", 100, 0.01)
        analysis_config.set("analysis.metrics", ["reward", "firing_rate", "efficiency"])
        analysis_config.set("analysis.thresholds.reward", 1.5)
        analysis_config.set("analysis.thresholds.firing_rate", 50.0)
        
        config_manager.addTemplate("analysis", "Analysis configuration", analysis_config)
        analysis_template = config_manager.createFromTemplate("analysis")
        is_analysis_valid = config_manager.validateConfig(analysis_template)
        assert is_analysis_valid == True
        
        # Perform sensitivity analysis
        sensitivity = config_manager.analyzeSensitivity(analysis_template)
        
        # Create results visualization
        visualizer = pynlm.createVisualizationTools()
        
        # Visualize neural network
        viz_network = visualizer.visualizeNetwork("complete_viz_network.png", brain)
        
        # Visualize activity
        viz_activity = visualizer.visualizeActivity("complete_viz_activity.png", brain, "firing_rate", 0.5)
        
        # Create activity monitor
        monitor_html = visualizer.createActivityMonitor("complete_viz_monitor.html", brain)
        
        # Create results dashboard
        dashboard = visualizer.createResultsDashboard(
            "complete_viz_dashboard.html",
            [brain],
            ["Complete Workflow Experiment"]
        )
        
        print("✓ Step 6: Results analysis with advanced tools complete")
        
        # Step 7: Export results with serialization
        print("\nStep 7: Export results with serialization")
        
        # Use SerializationManager for comprehensive export
        serializer = pynlm.createSerializationManager()
        
        # Prepare export data
        export_data = {
            "brain_state": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "avg_firing_rate": brain.getAverageFiringRate()
            },
            "simulation_results": {
                "total_steps": 100,
                "total_time": monitor.getTotalTimeSeconds(),
                "steps_per_second": monitor.getStepsPerSecond(),
                "avg_step_time": monitor.getAvgStepTimeMs()
            },
            "performance_metrics": {
                "final_reward": recorder.getMetric("reward"),
                "avg_firing_rate": recorder.getMean("firing_rate"),
                "avg_efficiency": recorder.getMean("efficiency"),
                "min_efficiency": recorder.getMin("efficiency"),
                "max_efficiency": recorder.getMax("efficiency")
            },
            "agent_metrics": {
                "curiosity_level": agent.getCuriosityLevel(),
                "prediction_error": agent.getPredictionError(),
                "neuromodulation_level": agent.getNeuromodulationLevel()
            }
        }
        
        # Export to multiple formats
        export_datasets = {
            "neuron_activity": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0],
            "synaptic_weights": [0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05],
            "firing_rates": [10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0]
        }
        
        # HDF5 export
        hdf5_result = serializer.saveHDF5("complete_export.h5", analysis_template, export_datasets)
        assert hdf5_result == True
        
        # Pickle export
        pickle_result = serializer.savePickle("complete_export.pkl", analysis_template, brain)
        assert pickle_result == True
        
        # Create checkpoint
        checkpoint_result = serializer.createCheckpoint(
            "complete_workflow_checkpoint",
            analysis_template,
            brain
        )
        assert checkpoint_result == True
        
        # Incremental export
        incremental_result = serializer.saveIncremental(
            "complete_export_step1.pkl",
            analysis_template,
            export_datasets
        )
        assert incremental_result == True
        
        print("✓ Step 7: Export results with serialization complete")
        
        print("\n" + "=" * 60)
        print("COMPLETE WORKFLOW INTEGRATION TESTS PASSED")
        print("✓ CLI tools successfully integrate with advanced features")
        print("✓ Complete workflow from configuration to export works")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"\n✗ Complete workflow integration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def run_all_integration_tests():
    """Run all integration tests."""
    print("=" * 80)
    print("PHASE 5 INTEGRATION TESTS")
    print("=" * 80)
    print("Testing cross-phase functionality and workflow integration")
    
    all_passed = True
    
    # Run all integration tests
    if not test_convenience_to_advanced_integration():
        all_passed = False
    
    if not test_cli_to_advanced_integration():
        all_passed = False
    
    if not test_complete_workflow_integration():
        all_passed = False
    
    print("\n" + "=" * 80)
    if all_passed:
        print("ALL INTEGRATION TESTS PASSED")
        print("✓ Phase 2 and Phase 3 features integrate successfully")
        print("✓ CLI tools (Phase 4) integrate with advanced features (Phase 3)")
        print("✓ Complete workflow from CLI to advanced operations works")
    else:
        print("SOME INTEGRATION TESTS FAILED")
        print("✗ Cross-phase integration has issues")
    
    print("=" * 80)
    return all_passed
if __name__ == "__main__":
    success = run_all_integration_tests()
    sys.exit(0 if success else 1)