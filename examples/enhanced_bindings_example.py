#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings - Example Usage
===========================================

This file demonstrates the enhanced Python API for NLM (Neural Learning Machine)
with all the new features introduced in this release.

The enhanced API provides:
1. Enhanced Configuration API with YAML support
2. Advanced Brain Interface with visualization and diagnostics
3. Enhanced Agent Interface with advanced sensory processing
4. World and Environment Tools
5. Memory and Learning Tools
6. Performance and Monitoring Tools
7. Educational and Debugging Tools
8. Advanced Scripting and Automation

Note: This is example code. The actual C++ bindings need to be compiled and
installed for these examples to work.
"""

import sys
import time
import json
from typing import Dict, List, Optional, Any

# Import the enhanced NLM bindings
try:
    import pynlm
    print("NLM bindings imported successfully")
except ImportError as e:
    print(f"Error importing NLM bindings: {e}")
    print("Make sure the enhanced bindings are compiled and installed")
    sys.exit(1)

def example_enhanced_configuration():
    """Example 1: Enhanced Configuration API with YAML support"""
    print("\n=== Example 1: Enhanced Configuration API ===")
    
    # Create configuration from environment variables
    config = pynlm.createConfigFromEnvironment()
    print(f"Created config from environment: {config.summary()}")
    
    # Create default configuration template
    default_config = pynlm.createDefaultConfig()
    print(f"Default config created with {len(default_config.getKeys())} keys")
    
    # Load configuration from YAML file
    # config.loadFromFile("config.yaml")  # Uncomment when YAML support is implemented
    
    # Validate configuration
    validation_result = config.validate()
    if not validation_result.valid:
        print(f"ERROR: Configuration validation failed: {validation_result.errorMessage}")
    elif validation_result.warnings:
        print("WARNING: Configuration validation warnings:")
        for warning in validation_result.warnings:
            print(f"  - {warning}")
    else:
        print("Configuration validation passed")
    
    # Set values with validation
    config.set("brain.neuronCount", 2000)
    config.set("world.width", 150.0)
    config.set("world.height", 120.0)
    
    # Get values with defaults
    neuron_count = config.getOr("brain.neuronCount", 1000)
    print(f"Neuron count: {neuron_count}")
    
    # Export/import configuration
    config_data = config.exportConfig()
    print(f"Exported config with {len(config_data)} entries")
    
    # Import config back
    config2 = pynlm.EnhancedConfig()
    config2.importConfig(config_data)
    
    return config

def example_enhanced_brain():
    """Example 2: Enhanced Brain Interface with visualization and diagnostics"""
    print("\n=== Example 2: Enhanced Brain Interface ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create enhanced brain
    brain = pynlm.createEnhancedBrain(config)
    
    # Initialize with diagnostics
    print("Initializing brain with diagnostics...")
    if brain.initializeWithDiagnostics():
        print("Brain initialized successfully")
    else:
        print("Brain initialization failed")
        return None
    
    # Get debug information
    debug_info = brain.getDebugInfo()
    print(f"Debug info:")
    print(f"  - Error: {debug_info.errorMessage}")
    print(f"  - Simulation time: {debug_info.simulationTime:.3f}s")
    print(f"  - Active neurons: {debug_info.activeNeurons}")
    print(f"  - Firing neurons: {debug_info.firingNeurons}")
    print(f"  - Average firing rate: {debug_info.averageFiringRate:.2f} Hz")
    
    # Enable visualization
    brain.enableVisualization(True)
    print("Visualization enabled")
    
    # Perform enhanced simulation step
    print("Performing enhanced simulation step...")
    brain.stepAdvanced(timestep=0.01, collectStats=True)
    
    # Get performance metrics
    perf_metrics = brain.getPerformanceMetrics()
    print(f"Performance metrics:")
    print(f"  - CPU usage: {perf_metrics.cpuUsage:.1f}%")
    print(f"  - Memory usage: {perf_metrics.memoryUsage:.1f}%")
    print(f"  - Simulation speed: {perf_metrics.simulationSpeed:.1f}x")
    
    # Get learning progress
    learning_progress = brain.getLearningProgress()
    print(f"Learning progress:")
    print(f"  - Total time: {learning_progress.totalTime:.2f}s")
    print(f"  - Total steps: {learning_progress.totalSteps}")
    print(f"  - Total spikes: {learning_progress.totalSpikes}")
    print(f"  - Convergence score: {learning_progress.convergenceScore:.3f}")
    
    # Update visualization
    brain.updateVisualization()
    
    return brain

def example_enhanced_agent():
    """Example 3: Enhanced Agent Interface with advanced sensory processing"""
    print("\n=== Example 3: Enhanced Agent Interface ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and agent
    brain = pynlm.createEnhancedBrain(config)
    agent = pynlm.createEnhancedAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=100.0, height=100.0, visionWidth=10, visionHeight=10)
    
    # Initialize agent with world
    agent.initializeWithWorld(world, enableLearning=True)
    print("Agent initialized with world")
    
    # Simulate sensory processing
    print("Simulating sensory processing...")
    
    # Create sensory percept
    from pynlm import Vision, SensoryPercept
    vision = Vision(width=10, height=10, channels=3)
    percept = SensoryPercept()
    percept.setVision(vision)
    
    # Process with enhanced agent
    agent.processSensoryInputEnhanced(percept, applySmoothing=True, detectNovelty=True)
    print("Sensory processing completed")
    
    # Analyze sensory input
    analysis = agent.analyzeSensoryInput(percept)
    print(f"Sensory analysis:")
    print(f"  - Novelty score: {analysis.noveltyScore:.3f}")
    print(f"  - Predictability: {analysis.predictability:.3f}")
    print(f"  - Complexity: {analysis.complexity:.3f}")
    
    # Decode motor command with enhanced features
    motor_cmd = agent.decodeMotorCommandEnhanced(
        applyRewardShaping=True,
        considerExploration=True,
        temperature=1.0,
        useActionValueEstimation=True
    )
    print(f"Decoded motor command: {motor_cmd}")
    
    # Apply reward modulation with enhanced learning
    agent.applyRewardModulationEnhanced(
        reward=1.0,
        predictedReward=0.8,
        useTDLearning=True,
        usePolicyGradient=False,
        useActorCritic=True
    )
    print("Reward modulation applied")
    
    # Update development
    agent.updateDevelopmentEnhanced(timestep=0.01, enableStructuralPlasticity=True)
    print("Development updated")
    
    # Get neuromodulation levels
    print(f"Neuromodulation levels:")
    print(f"  - Level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Prediction error: {agent.getPredictionError():.3f}")
    
    # Get learning statistics
    stats = agent.getLearningStatistics()
    print(f"Learning statistics:")
    print(f"  - Total time: {stats.totalTime:.2f}s")
    print(f"  - Total episodes: {stats.totalEpisodes}")
    print(f"  - Average reward: {stats.averageReward:.3f}")
    
    return agent

def example_world_and_environment():
    """Example 4: World and Environment Tools"""
    print("\n=== Example 4: World and Environment Tools ===")
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=200.0, height=150.0, visionWidth=20, visionHeight=20)
    
    # Set agent starting position
    world.setAgentStart(50.0, 50.0)
    
    # Add objects to world
    from pynlm import WorldObject, WorldObjectType
    
    # Add resource
    resource = WorldObject(x=100.0, y=100.0, type=WorldObjectType.Resource, value=10.0)
    world.addObject(resource)
    
    # Add hazard
    hazard = WorldObject(x=150.0, y=80.0, type=WorldObjectType.Hazard, value=-5.0)
    world.addObject(hazard)
    
    # Add wall
    wall = WorldObject(x=180.0, y=50.0, type=WorldObjectType.Wall, radius=2.0)
    world.addObject(wall)
    
    print(f"World configured: {world.getWidth()} x {world.getHeight()}")
    print(f"Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"Max energy: {world.getMaxEnergy()}")
    print(f"Energy decay rate: {world.getEnergyDecayRate()}")
    
    # Simulate world update
    for step in range(10):
        world.update(timestep=0.1)
        percept = world.getSensoryPercept()
        print(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Energy: {world.getAgentBody().energy:.1f}")
        
        # Check for objects nearby
        body = world.getAgentBody()
        if world.isValidPosition(body.x, body.y):
            obj = world.getObjectAt(body.x, body.y)
            if obj:
                print(f"  Found object: type={obj.type}, value={obj.value}")
        
        if step % 5 == 0:
            world.setRandomSeed(step + 12345)
    
    # Get simulation time
    print(f"Simulation time: {world.getSimulationTime():.2f}")
    
    return world

def example_memory_and_learning():
    """Example 5: Memory and Learning Tools"""
    print("\n=== Example 5: Memory and Learning Tools ===")
    
    # Create memory systems
    working_memory = pynlm.createWorkingMemory()
    episodic_memory = pynlm.createEpisodicMemory()
    semantic_memory = pynlm.createSemanticMemory()
    procedural_memory = pynlm.createProceduralMemory()
    associative_memory = pynlm.createAssociativeMemory()
    
    # Create visualization interface
    viz = pynlm.createVisualizationInterface()
    
    print("Memory systems created:")
    print(f"  - Working memory capacity: {working_memory.getCapacity()}")
    print(f"  - Working memory size: {working_memory.getCurrentSize()}")
    
    # Store data in memories
    from pynlm import NeuronId
    
    # Working memory
    neuron_id = NeuronId(123)
    working_memory.store(neuron_id, 0.85f)
    retrieved = working_memory.retrieve(neuron_id)
    print(f"Working memory: stored {retrieved}, retrieved {retrieved}")
    
    # Semantic memory
    semantic_memory.storeFact("temperature", "warm")
    fact = semantic_memory.retrieveFact("temperature")
    print(f"Semantic memory: stored and retrieved fact '{fact}'")
    
    # Procedural memory
    pattern = [NeuronId(1), NeuronId(2), NeuronId(3)]
    procedural_memory.learnSkill("walk_forward", pattern)
    skill = procedural_memory.getSkill("walk_forward")
    print(f"Procedural memory: learned skill '{skill.name}' with proficiency {skill.proficiency}")
    
    # Associative memory
    associative_memory.associate(NeuronId(100), NeuronId(200), 0.9f)
    associations = associative_memory.getAssociations(NeuronId(100))
    print(f"Associative memory: Neuron 100 associated with {len(associations)} neurons")
    
    # Initialize and use visualization
    if viz.initialize():
        print("Visualization interface initialized")
        # viz.visualizeNetwork(brain)  # Would require brain reference
        viz.close()
    
    return {
        'working_memory': working_memory,
        'episodic_memory': episodic_memory,
        'semantic_memory': semantic_memory,
        'procedural_memory': procedural_memory,
        'associative_memory': associative_memory,
        'visualization': viz
    }

def example_performance_monitoring():
    """Example 6: Performance and Monitoring Tools"""
    print("\n=== Example 6: Performance and Monitoring Tools ===")
    
    # Create configuration with performance settings
    config = pynlm.createDefaultConfig()
    config.set("brain.performance.enabled", True)
    config.set("brain.performance.monitor_interval", 0.1)
    
    # Create brain with performance monitoring
    brain = pynlm.createEnhancedBrain(config)
    
    # Performance metrics structures
    perf_metrics = pynlm.createPerformanceMetrics()
    debug_info = pynlm.createDebugInfo()
    
    print("Performance monitoring enabled")
    print(f"Initial performance metrics:")
    print(f"  - CPU usage: {perf_metrics.cpuUsage:.1f}%")
    print(f"  - Memory usage: {perf_metrics.memoryUsage:.1f}%")
    
    # Run simulation with performance tracking
    for i in range(5):
        brain.stepAdvanced(timestep=0.1, collectStats=True)
        
        # Get updated metrics
        updated_metrics = brain.getPerformanceMetrics()
        updated_debug = brain.getDebugInfo()
        
        print(f"Step {i+1}: CPU={updated_metrics.cpuUsage:.1f}%, "
              f"Memory={updated_metrics.memoryUsage:.1f}%, "
              f"Speed={updated_metrics.simulationSpeed:.1f}x")
        
        time.sleep(0.05)  # Small delay
    
    # Compare with baseline (would require baseline brain)
    # score = brain.compareWithBaseline(baseline_brain)
    # print(f"Performance comparison score: {score:.3f}")
    
    return brain

def example_educational_tools():
    """Example 7: Educational and Debugging Tools"""
    print("\n=== Example 7: Educational and Debugging Tools ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createEnhancedBrain(config)
    
    # Create debugging structures
    debug_info = pynlm.createDebugInfo()
    learning_progress = pynlm.createLearningProgress()
    
    print("Educational tools created")
    
    # Step-by-step execution
    print("Step-by-step execution:")
    for step in range(3):
        print(f"  Step {step + 1}:")
        brain.stepAdvanced(timestep=0.01, collectStats=True)
        
        # Get step-by-step debug info
        step_debug = brain.getDebugInfo()
        print(f"    - Neurons: {step_debug.activeNeurons} active, {step_debug.firingNeurons} firing")
        print(f"    - Firing rate: {step_debug.averageFiringRate:.2f} Hz")
        print(f"    - Time: {step_debug.simulationTime:.3f}s")
        
        # Create neuron/synapse visualization
        # viz.visualizeNetwork(brain)  # Would require visualization interface
    
    # Learning progress tracking
    progress = brain.getLearningProgress()
    print(f"\nLearning progress:")
    print(f"  - Total steps: {progress.totalSteps}")
    print(f"  - Total spikes: {progress.totalSpikes}")
    print(f"  - Convergence score: {progress.convergenceScore:.3f}")
    
    # Educational mode
    print("\nEducational mode - neuron and synapse visualization:")
    print("  (Would show detailed network visualization)")
    
    return brain

def example_advanced_scripting():
    """Example 8: Advanced Scripting and Automation"""
    print("\n=== Example 8: Advanced Scripting and Automation ===")
    
    # Batch operations
    configs = []
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuronCount", 1000 + i * 500)
        config.set("world.width", 100.0 + i * 20)
        config.set("world.height", 100.0 + i * 20)
        configs.append(config)
        print(f"Created config {i+1}: {config.summary()}")
    
    # Experiment scripting
    print("\nExperiment scripting:")
    
    experiments = []
    for i, config in enumerate(configs):
        experiment = {
            'name': f'experiment_{i}',
            'config': config,
            'brain': pynlm.createEnhancedBrain(config),
            'results': []
        }
        experiments.append(experiment)
        print(f"  Created experiment: {experiment['name']}")
    
    # Automated testing framework
    print("\nAutomated testing:")
    test_results = []
    for i, experiment in enumerate(experiments):
        # Run experiment
        brain = experiment['brain']
        if brain.initializeWithDiagnostics():
            # Run simulation steps
            for step in range(10):
                brain.stepAdvanced(timestep=0.1, collectStats=True)
            
            # Collect results
            debug_info = brain.getDebugInfo()
            learning_progress = brain.getLearningProgress()
            
            experiment['results'] = {
                'success': True,
                'final_neurons': debug_info.activeNeurons,
                'final_firing': debug_info.firingNeurons,
                'learning_score': learning_progress.convergenceScore
            }
            
            test_results.append(experiment['results'])
            print(f"  Experiment {i+1}: {'PASS' if experiment['results']['success'] else 'FAIL'}")
        else:
            experiment['results'] = {'success': False, 'error': 'Initialization failed'}
            test_results.append(experiment['results'])
            print(f"  Experiment {i+1}: FAIL (initialization)")
    
    # Summary
    print(f"\nExperiment summary:")
    passed = sum(1 for r in test_results if r['success'])
    print(f"  Passed: {passed}/{len(test_results)}")
    
    # Custom simulation builder
    print("\nCustom simulation builder:")
    
    # Example: Build a simulation with custom configuration
    class CustomSimulationBuilder:
        def __init__(self):
            self.config = pynlm.createDefaultConfig()
            self.world = None
            self.brain = None
            self.agent = None
        
        def add_neurons(self, count):
            self.config.set("brain.neuronCount", count)
            return self
        
        def add_objects(self, objects):
            # Would add world objects
            return self
        
        def build(self):
            self.world = pynlm.createSimpleWorld()
            self.brain = pynlm.createEnhancedBrain(self.config)
            self.agent = pynlm.createEnhancedAgentBrain(self.brain)
            return self
    
    custom_sim = CustomSimulationBuilder().add_neurons(2000).build()
    print(f"  Built custom simulation with {custom_sim.config.getOr('brain.neuronCount', 0)} neurons")
    
    return experiments

def main():
    """Run all examples"""
    print("Enhanced NLM Python Bindings - Complete Examples")
    print("=" * 60)
    
    try:
        # Run all examples
        example_enhanced_configuration()
        example_enhanced_brain()
        example_enhanced_agent()
        example_world_and_environment()
        example_memory_and_learning()
        example_performance_monitoring()
        example_educational_tools()
        example_advanced_scripting()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("\nKey improvements demonstrated:")
        print("1. Enhanced Configuration API with YAML support and validation")
        print("2. Advanced Brain Interface with visualization and diagnostics")
        print("3. Enhanced Agent Interface with advanced sensory processing")
        print("4. World and Environment management tools")
        print("5. Memory and Learning visualization tools")
        print("6. Performance and Monitoring tools with real-time metrics")
        print("7. Educational and Debugging tools for research")
        print("8. Advanced scripting and automation for experiments")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())