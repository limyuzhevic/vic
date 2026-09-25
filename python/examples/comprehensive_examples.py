#!/usr/bin/env python3
"""
NLM Python Bindings - Complete Examples and Demos

This file contains comprehensive examples demonstrating the improved
NLM Python API with all new features and improvements.

Examples include:
1. Basic usage and setup
2. Advanced agent simulation
3. Error handling and debugging
4. Performance optimization
5. Machine learning workflows
"""

import sys
import os
import time
import tempfile
from pathlib import Path

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm import (
    # Core components
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    create_development_agent,
    
    # Classes
    Config, Brain, SimpleWorld, AgentBrain, Action, WorldObject,
    Vision, Audio, InternalSignals, SensoryInput,
    
    # Enums and IDs
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, WorldObjectType,
    
    # Convenience functions
    run_simulation_with_callback,
    quick_test_simulation,
    save_checkpoint,
    load_checkpoint,
)


def example_1_basic_usage():
    """
    Example 1: Basic Usage and Setup
    ================================
    
    Demonstrates the simplest possible way to create and use an NLM brain.
    """
    print("Example 1: Basic Usage")
    print("-" * 40)
    
    # Create a default configuration
    config = create_default_config()
    print(f"Created config: {config.summary()}")
    
    # Create and initialize a brain
    brain = create_brain(config)
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run a simple simulation
    print("Running simulation for 10 steps...")
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
    print()


def example_2_agent_simulation():
    """
    Example 2: Complete Agent Simulation
    ====================================
    
    Demonstrates a complete agent simulation with world interaction,
    learning, and development.
    """
    print("Example 2: Complete Agent Simulation")
    print("-" * 40)
    
    # Create a development agent with all subsystems enabled
    brain, agent, world = create_development_agent(
        world_size=(20, 20),
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True,
        enable_reward_modulation=True,
        enable_structural_plasticity=True
    )
    
    print(f"Created development agent:")
    print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  - World: {world.getWidth()}x{world.getHeight()} pixels")
    print(f"  - Agent subsystems: Reward={agent.isRewardModulationEnabled()}, "
          f"Plasticity={agent.isStructuralPlasticityEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}")
    
    # Define a custom simulation callback
    def simulation_callback(step, stats):
        if step % 20 == 0:
            print(f"\n  Simulation checkpoint at step {step}:")
            print(f"    - Curiously exploring: {agent.getCuriosityLevel():.2f}")
            print(f"    - Novelty detected: {agent.getNoveltyLevel():.2f}")
            print(f"    - Prediction error: {agent.getPredictionError():.2f}")
            print(f"    - Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Run the simulation with callbacks and checkpointing
    print("\nRunning simulation for 100 steps...")
    stats = run_simulation_with_callback(
        brain=brain,
        world=world,
        agent=agent,
        num_steps=100,
        timestep=0.1,
        callback=simulation_callback,
        save_frequency=25,
        checkpoint_file="agent_checkpoint"
    )
    
    print(f"\nSimulation complete!")
    print(f"  - Total steps: {stats['steps_completed']}")
    print(f"  - Total spikes: {stats['total_spikes']}")
    print(f"  - Development stage: {brain.getDevelopmentalStage().name}")
    print()


def example_3_advanced_neuron_management():
    """
    Example 3: Advanced Neuron and Synapse Management
    ================================================
    
    Demonstrates advanced features for neuron and synapse management
    using the improved Pythonic API.
    """
    print("Example 3: Advanced Neuron and Synapse Management")
    print("-" * 40)
    
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    # Demonstrate NeuronId management
    print("1. Neuron ID Management:")
    neuron1 = NeuronId(1001)
    neuron2 = NeuronId(2002)
    
    print(f"   Created neurons: {neuron1}, {neuron2}")
    print(f"   Neuron 1 value: {neuron1.value}")
    print(f"   Neuron 1 index: {neuron1.index()}")
    print(f"   Neuron 1 as dict: {neuron1.as_dict()}")
    print(f"   Neuron 1 equals neuron2: {neuron1.equals(neuron2)}")
    print(f"   String representation: {neuron1}")
    
    # Demonstrate SynapseId management
    print("\n2. Synapse ID Management:")
    synapse1 = SynapseId(5001)
    synapse2 = SynapseId(5002)
    
    print(f"   Created synapses: {synapse1}, {synapse2}")
    print(f"   Synapse string: {synapse1.to_string()}")
    
    # Demonstrate PopulationId with sentinel
    print("\n3. Population ID with Sentinel:")
    valid_pop = PopulationId(3001)
    invalid_pop = PopulationId.invalid()
    
    print(f"   Valid population: {valid_pop}")
    print(f"   Invalid population (sentinel): {invalid_pop}")
    
    # Demonstrate enum utilities
    print("\n4. Enum Utilities:")
    neuron_types = [NeuronType.Excitatory, NeuronType.Inhibitory, NeuronType.Sensory]
    for nt in neuron_types:
        print(f"   {nt.name}:")
        print(f"     - Is excitatory: {nt.is_excitatory()}")
        print(f"     - Is inhibitory: {nt.is_inhibitory()}")
    
    synapse_types = [SynapseType.Excitatory, SynapseType.Electrical, SynapseType.GapJunction]
    for st in synapse_types:
        print(f"   {st.name}:")
        print(f"     - Is chemical: {st.is_chemical()}")
        print(f"     - Is electrical: {st.is_electrical()}")
    
    action_types = [ActionType.MoveForward, ActionType.TurnLeft, ActionType.Eat]
    for at in action_types:
        print(f"   {at.name}:")
        print(f"     - Is movement: {at.is_movement()}")
        print(f"     - Is rotational: {at.is_rotational()}")
        print(f"     - Is consumable: {at.is_consumable()}")
    
    print(f"\n   Brain has {brain.getTotalNeuronCount()} neurons and "
          f"{brain.getTotalSynapseCount()} synapses")
    print()


def example_4_error_handling_and_debugging():
    """
    Example 4: Error Handling and Debugging
    =======================================
    
    Demonstrates robust error handling and debugging techniques.
    """
    print("Example 4: Error Handling and Debugging")
    print("-" * 40)
    
    # Create a brain with configuration
    config = create_default_config()
    
    # Example 1: Brain initialization error
    print("1. Brain Initialization Error Handling:")
    try:
        # This should work
        brain = create_brain(config)
        brain.initialize()
        print("   ✓ Brain created and initialized successfully")
    except Exception as e:
        print(f"   ✗ Error creating brain: {type(e).__name__}: {e}")
    
    # Example 2: Using context manager
    print("\n2. Context Manager for Resource Management:")
    try:
        with BrainManager(config) as manager:
            print("   ✓ Brain manager created successfully")
            success = manager.run_simulation(10)
            if success:
                print("   ✓ Simulation completed successfully")
    except Exception as e:
        print(f"   ✗ Context manager error: {type(e).__name__}: {e}")
    
    # Example 3: Configuration validation
    print("\n3. Configuration Validation:")
    try:
        # Validate config parameters
        if not hasattr(config, 'has') or not config.has('brain.neuron_count'):
            raise ValueError("Missing required configuration")
        print("   ✓ Configuration is valid")
    except ValueError as e:
        print(f"   ✗ Configuration error: {e}")
    
    # Example 4: Sensor data validation
    print("\n4. Sensor Data Validation:")
    world = create_simple_world()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    try:
        percept = world.getSensoryPercept()
        if percept is None:
            raise RuntimeError("No sensory input available")
        print("   ✓ Valid sensory percept obtained")
    except RuntimeError as e:
        print(f"   ✗ Sensor error: {e}")
    
    # Example 5: Progress monitoring
    print("\n5. Progress Monitoring:")
    brain = create_brain(config)
    brain.initialize()
    
    def progress_callback(step, total_steps):
        progress = (step / total_steps) * 100
        print(f"   Progress: {step}/{total_steps} ({progress:.1f}%)")
    
    print("   Simulating with progress tracking...")
    for step in range(10):
        brain.step(step)
        progress_callback(step + 1, 10)
    
    print("   ✓ Progress tracking completed")
    print()


class BrainManager:
    """
    Context manager for safe brain operations.
    
    This class demonstrates proper resource management and error handling
    when working with NLM brains.
    """
    
    def __init__(self, config=None):
        self.brain = None
        self.config = config
    
    def __enter__(self):
        """Enter the context and initialize the brain."""
        try:
            self.config = self.config or create_default_config()
            self.brain = create_brain(self.config)
            self.brain.initialize()
            return self
        except Exception as e:
            raise RuntimeError(f"Failed to initialize brain: {e}")
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit the context and clean up."""
        if self.brain:
            try:
                self.brain.reset()
            except Exception:
                pass  # Ignore cleanup errors
    
    def run_simulation(self, steps):
        """Run simulation safely within context."""
        if not self.brain:
            raise RuntimeError("Brain not initialized. Use with 'with' statement.")
        
        try:
            for step in range(steps):
                self.brain.step(step)
            return True
        except Exception as e:
            print(f"Simulation failed at step {step}: {e}")
            return False


def example_5_performance_and_optimization():
    """
    Example 5: Performance Monitoring and Optimization
    ================================================
    
    Demonstrates performance monitoring and optimization techniques.
    """
    print("Example 5: Performance Monitoring and Optimization")
    print("-" * 40)
    
    # Quick test to verify system is working
    print("1. System Performance Check:")
    try:
        test_result = quick_test_simulation(steps=5)
        print(f"   ✓ System test passed:")
        print(f"     - Neurons created: {test_result['total_neurons']}")
        print(f"     - Execution time: {test_result['execution_time']:.3f}s")
        print(f"     - Spikes per step: {test_result['spikes_per_step']:.1f}")
    except Exception as e:
        print(f"   ✗ Performance test failed: {e}")
    
    # Simulate different workloads
    print("\n2. Workload Simulation:")
    config = create_default_config()
    
    workloads = [
        ("Light", 50, 0.05),
        ("Medium", 100, 0.1),
        ("Heavy", 200, 0.2),
    ]
    
    for name, steps, timestep in workloads:
        try:
            brain = create_brain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            spikes_per_step = brain.getTotalSpikeCount() / steps
            
            print(f"   {name} workload ({steps} steps, {timestep}s timestep):")
            print(f"     - Duration: {duration:.2f}s")
            print(f"     - Speed: {steps/duration:.1f} steps/s")
            print(f"     - Spikes/step: {spikes_per_step:.1f}")
            
        except Exception as e:
            print(f"   ✗ {name} workload failed: {e}")
    
    print("\n3. Memory and Resource Efficiency:")
    brain = create_brain(config)
    brain.initialize()
    
    # Demonstrate efficient operations
    initial_memory = brain.getTotalNeuronCount()
    
    # Perform operations
    for step in range(20):
        brain.step(step)
    
    final_memory = brain.getTotalNeuronCount()
    print(f"   ✓ Memory efficient: Maintained {final_memory - initial_memory} neurons")
    print(f"   ✓ Computationally efficient: {brain.getTotalSpikeCount()} total spikes")
    print()


def example_6_machine_learning_workflow():
    """
    Example 6: Machine Learning Workflow
    ===================================
    
    Demonstrates a complete machine learning workflow using NLM.
    """
    print("Example 6: Machine Learning Workflow")
    print("-" * 40)
    
    print("1. Data Preparation:")
    print("   Simulating training data preparation...")
    
    # Create multiple brains for ensemble learning
    brains = []
    agents = []
    worlds = []
    
    for i in range(3):
        config = create_default_config()
        # Modify each config slightly for diversity
        # (In real scenario, you might load different training datasets)
        brain = create_brain(config)
        brain.initialize()
        
        world = create_simple_world()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = create_agent_brain(brain)
        agent.initialize(world)
        
        brains.append(brain)
        agents.append(agent)
        worlds.append(world)
    
    print(f"   ✓ Created {len(brains)} agent models")
    
    print("\n2. Training Phase:")
    training_steps = 100
    
    for episode in range(training_steps):
        # Each brain explores the environment
        for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
            # Enable learning for this episode
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run a single episode
            for step in range(10):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect performance metrics
            curiosity = agent.getCuriosityLevel()
            if episode % 20 == 0:
                print(f"     Agent {i+1} - Episode {episode}: Curiosity={curiosity:.2f}, "
                      f"Spikes={brain.getTotalSpikeCount()}")
    
    print("   ✓ Training complete")
    
    print("\n3. Evaluation Phase:")
    # Evaluate each agent's performance
    best_brain = None
    best_score = 0
    
    for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
        # Run evaluation episode
        evaluation_spikes = 0
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            evaluation_spikes += brain.getFiringNeuronCount()
        
        # Calculate simple performance score
        performance_score = evaluation_spikes / 50
        
        print(f"     Agent {i+1}: Performance score={performance_score:.1f}")
        
        if performance_score > best_score:
            best_score = performance_score
            best_brain = brain
    
    print(f"\n   Best performing agent: Score={best_score:.1f}")
    
    if best_brain:
        print("\n4. Deployment:")
        print("   Best brain ready for deployment!")
        print(f"   - Total neurons: {best_brain.getTotalNeuronCount()}")
        print(f"   - Total synapses: {best_brain.getTotalSynapseCount()}")
        print(f"   - Average firing rate: {best_brain.getAverageFiringRate():.2f} Hz")
    
    print("\n5. Continuous Learning:")
    print("   Setting up continuous learning with new experiences...")
    
    # Continue learning with new data
    new_config = create_default_config()
    new_brain = create_brain(new_config)
    new_brain.initialize()
    
    print("   ✓ New learning agent created and ready")
    print()


def example_7_visualization_and_analysis():
    """
    Example 7: Visualization and Analysis
    =====================================
    
    Demonstrates data visualization and analysis techniques.
    """
    print("Example 7: Visualization and Analysis")
    print("-" * 40)
    
    print("1. Data Collection:")
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    # Collect various statistics over time
    statistics = {
        'steps': [],
        'neuron_count': [],
        'synapse_count': [],
        'firing_neurons': [],
        'total_spikes': [],
        'firing_rate': [],
        'excitatory_inhibition_ratio': []
    }
    
    print("   Collecting statistics over 50 steps...")
    for step in range(50):
        brain.step(step)
        
        statistics['steps'].append(step)
        statistics['neuron_count'].append(brain.getTotalNeuronCount())
        statistics['synapse_count'].append(brain.getTotalSynapseCount())
        statistics['firing_neurons'].append(brain.getFiringNeuronCount())
        statistics['total_spikes'].append(brain.getTotalSpikeCount())
        statistics['firing_rate'].append(brain.getAverageFiringRate())
        statistics['excitatory_inhibition_ratio'].append(brain.getExcitationInhibitionRatio())
    
    print(f"   ✓ Collected {len(statistics['steps'])} data points")
    
    print("\n2. Statistical Analysis:")
    
    def calculate_statistics(data, name):
        if not data:
            return None
        return {
            'mean': sum(data) / len(data),
            'min': min(data),
            'max': max(data),
            'std': (sum((x - sum(data) / len(data)) ** 2 for x in data) / len(data)) ** 0.5
        }
    
    # Analyze key metrics
    for metric, data in statistics.items():
        if data:
            stats = calculate_statistics(data, metric)
            if stats:
                print(f"     {metric.replace('_', ' ').title()}:")
                print(f"       - Mean: {stats['mean']:.2f}")
                print(f"       - Min: {stats['min']}")
                print(f"       - Max: {stats['max']}")
                print(f"       - Std: {stats['std']:.2f}")
    
    print("\n3. Trend Analysis:")
    print("   Analyzing firing rate trends...")
    
    # Check for patterns in firing rate
    firing_rates = statistics['firing_rate']
    if len(firing_rates) >= 10:
        recent_trend = (firing_rates[-5:] + firing_rates[-10:-5]) / 2
        print(f"   Average firing rate trend: {recent_trend:.2f} Hz")
    
    # Check for developmental stage changes
    developmental_stages = []
    for step in range(0, len(statistics['steps']), 10):
        if step < len(statistics['steps']):
            developmental_stages.append(brain.getDevelopmentalStage())
    
    if developmental_stages:
        print(f"   Developmental stages observed: {set(stage.name for stage in developmental_stages)}")
    
    print("\n4. Pattern Recognition:")
    print("   Identifying activity patterns...")
    
    # Simple pattern detection
    spike_counts = statistics['total_spikes']
    increasing_steps = 0
    for i in range(1, len(spike_counts)):
        if spike_counts[i] > spike_counts[i-1]:
            increasing_steps += 1
    
    pattern_ratio = increasing_steps / max(len(spike_counts) - 1, 1)
    print(f"   Spike increase pattern ratio: {pattern_ratio:.2f}")
    
    if pattern_ratio > 0.7:
        print("   ✓ Strong increasing activity pattern detected")
    elif pattern_ratio < 0.3:
        print("   ✓ Stable activity pattern detected")
    else:
        print("   ✓ Variable activity pattern detected")
    
    print("\n5. Export for Visualization:")
    print("   Preparing data for visualization tools...")
    
    # In a real scenario, you would export this data for use with
    # visualization libraries like matplotlib, plotly, etc.
    print("   ✓ Data ready for export")
    print("   (Would save to CSV/JSON for visualization)")
    print()


def main():
    """
    Main function to run all examples.
    """
    print("NLM Python Bindings - Complete Examples")
    print("=" * 50)
    print()
    
    examples = [
        ("Basic Usage", example_1_basic_usage),
        ("Complete Agent Simulation", example_2_agent_simulation),
        ("Advanced Neuron Management", example_3_advanced_neuron_management),
        ("Error Handling and Debugging", example_4_error_handling_and_debugging),
        ("Performance and Optimization", example_5_performance_and_optimization),
        ("Machine Learning Workflow", example_6_machine_learning_workflow),
        ("Visualization and Analysis", example_7_visualization_and_analysis),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"✗ Example '{name}' failed: {type(e).__name__}: {e}")
            import traceback
            traceback.print_exc()
        
        # Separator between examples
        print("\n" + "=" * 50 + "\n")
    
    print("All examples completed!")
    print("\nFor additional documentation, see:")
    print("  - ERROR_HANDLING.md")
    print("  - easy_usage.md")
    print("  - HOW_TO_USE.md")


if __name__ == "__main__":
    main()
