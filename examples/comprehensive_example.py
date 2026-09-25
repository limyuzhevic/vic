#!/usr/bin/env python3
"""
NLM Python Package - Comprehensive Examples
===========================================

This file demonstrates the enhanced capabilities of the NLM (Neural Learning Machine)
Python package. It shows how to use the various features and utilities available.

Examples include:
1. Basic usage with simple simulation
2. Configuration management
3. Visualization tools
4. Training utilities
5. Advanced simulation techniques
"""

# Example 1: Basic Usage
# ========================

print("=== Example 1: Basic NLM Usage ===")

from pynlm import (
    createConfig, createBrain, createSimpleWorld, createAgentBrain,
    create_simulation, quick_simulation, EnhancedConfig
)

# Create and configure a brain
config = EnhancedConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("random_seed", 42)
config.set("enable_reward_modulation", True)
config.set("enable_structural_plasticity", True)

brain = createBrain(config)
brain.initialize()
print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")

# Create world
world = createSimpleWorld()
world.configure(30.0, 30.0, 24, 24)
print(f"✓ World configured: {world.getWidth()}x{world.getHeight()}")

# Create agent interface
agent = createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print(f"✓ Agent interface created")
print(f"  - Sensory input size: {agent.getSensoryInputSize()}")
print(f"  - Motor output size: {agent.getMotorOutputSize()}")

# Quick simulation
print("\n--- Running quick simulation ---")
results = quick_simulation(neuron_count=500, steps=200)
print(f"✓ Simulation completed in {results['total_spikes']} total spikes")
print(f"  - Total reward: {results['total_reward']:.2f}")
print(f"  - Actions: {results['actions']}")

# Example 2: Advanced Simulation with Metrics
# ============================================

print("\n=== Example 2: Advanced Simulation with Metrics ===")

from pynlm import SimulationProfiler, SimulationDebugger

# Create profiler and debugger
profiler = SimulationProfiler()
debugger = SimulationDebugger()

# Run enhanced simulation
brain2 = createBrain()
brain2.initialize()
world2 = createSimpleWorld()
world2.configure(20, 20, 16, 16)
agent2 = createAgentBrain(brain2)
agent2.initialize(world2)

for step in range(100):
    # Get percept and act
    percept = world2.getSensoryPercept()
    agent2.processSensoryInput(percept)
    
    # Run neural simulation
    for neural_step in range(10):
        brain2.step(neural_step)
    
    cmd = agent2.decodeMotorCommand()
    result = world2.applyMotorCommand(cmd, world2.getSimulationTime())
    
    agent2.applyRewardModulation(result.reward, 0.0)
    
    # Update world and development
    world2.update(0.01)
    agent2.updateDevelopment(0.01)
    
    # Record metrics
    import time
    step_time = 0.01  # Simulated time
    profiler.record_step(brain2, cmd, result.reward, step_time)
    
    # Debug brain state every 10 steps
    if step % 10 == 0:
        debugger.log(f"Step {step}: reward={result.reward:.3f}, action={cmd}")
        debugger.debug_brain_state(brain2, step)

# Get statistics
stats = profiler.get_statistics()
print(f"✓ Advanced simulation completed:")
print(f"  - Steps: {stats['total_steps']}")
print(f"  - Average step time: {stats['avg_step_time']*1000:.2f}ms")
print(f"  - Total reward: {stats['total_reward']:.2f}")
print(f"  - Average firing rate: {stats['avg_spike_count']:.1f}")
print(f"  - Action counts: {stats['action_counts']}")

# Example 3: Configuration Management
# ====================================

print("\n=== Example 3: Configuration Management ===")

from pynlm import EnhancedConfig

# Create enhanced configuration
config = EnhancedConfig()

# Set various configuration options
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)
config.set("random_seed", 123)
config.set("simulation_timestep", 0.001)
config.set("stdp_ltp_weight", 0.015)
config.set("stdp_ltd_weight", 0.012)
config.set("stdp_tau", 25.0)
config.set("environment_width", 50)
config.set("environment_height", 50)
config.set("vision_width", 20)
config.set("vision_height", 20)

# Validate configuration
errors = config.validate()
if errors:
    print(f"⚠ Configuration validation errors: {errors}")
else:
    print("✓ Configuration validated successfully")

# Show configuration summary
print(f"Configuration: {config.summary()}")

# Example 4: Visualization Tools
# ================================

print("\n=== Example 4: Visualization Tools ===")

from pynlm import NLMVisualizer

# Create a brain for visualization
brain3 = createBrain()
brain3.initialize()

print("Available visualization tools:")
print("1. Connectivity plot")
print("2. Activity over time")
print("3. Spike raster plot")

# Note: Uncomment to see visualizations (requires matplotlib)
try:
    import matplotlib.pyplot as plt
    
    # Example connectivity plot
    # fig = NLMVisualizer.plot_connectivity(brain3, max_neurons=50)
    # plt.show()
    
    # Example activity plot
    # fig = NLMVisualizer.plot_activity_over_time(brain3, window_size=50)
    # plt.show()
    
    # Example raster plot
    # fig = NLMVisualizer.plot_spike_raster(brain3, window_size=50)
    # plt.show()
    
    print("✓ Visualization tools available (requires matplotlib)")
    
except ImportError:
    print("⚠ Matplotlib not installed - visualization requires 'pip install matplotlib'")

# Example 5: Training Utilities
# ==============================

print("\n=== Example 5: Training Utilities ===")

from pynlm import NLMTraining, BatchSimulator

# Create training setup
brain4 = createBrain()
brain4.initialize()
world4 = createSimpleWorld()
world4.configure(20, 20, 16, 16)
agent4 = createAgentBrain(brain4)
agent4.initialize(world4)

# Example navigation training
print("Running navigation training...")
navigation_metrics = NLMTraining.train_navigation_task(world4, agent4, episodes=50)
print(f"✓ Navigation training completed:")
print(f"  - Success rate: {navigation_metrics['success_rate']:.1%}")
print(f"  - Exploration rate: {navigation_metrics['exploration_rate']:.1%}")
print(f"  - Episode rewards: {navigation_metrics['episode_rewards'][:5]}...")

# Example batch simulation
print("\nRunning batch simulation...")
def brain_factory(config=None):
    """Factory function for creating brains."""
    brain = createBrain()
    brain.initialize()
    return brain

simulator = BatchSimulator()
batch_results = simulator.run_simulation(brain_factory, steps=100)

print(f"✓ Batch simulation completed:")
print(f"  - Steps: {batch_results['steps']}")
print(f"  - Average step time: {batch_results['stats']['avg_step_time']*1000:.2f}ms")
print(f"  - Total reward: {batch_results['stats']['total_reward']:.2f}")

# Example 6: Command Line Interface
# =================================

print("\n=== Example 6: Command Line Interface ===")

from pynlm import create_cli

# Create and configure CLI
parser = create_cli()

# Add arguments (example)
parser.add_argument('--neurons', type=int, default=500,
                   help='Number of neurons')
parser.add_argument('--steps', type=int, default=1000,
                   help='Number of simulation steps')

# Note: In real usage, you would parse sys.argv
# args = parser.parse_args()

print("✓ CLI available with commands:")
print("  - --version: Show version")
print("  - --quick: Run quick simulation")
print("  - --batch: Run batch simulations")
print("  - --neurons: Number of neurons")
print("  - --steps: Number of steps")

# Example 7: Complete Integration Example
# ========================================

print("\n=== Example 7: Complete Integration Example ===")

def run_complete_example():
    """Run a complete example integrating all NLM features."""
    print("  Setting up complete NLM simulation...")
    
    # Configure with enhanced settings
    config = EnhancedConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("enable_reward_modulation", True)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    
    # Create simulation components
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(30, 30, 24, 24)
    world.setRandomSeed(42)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run simulation with detailed tracking
    profiler = SimulationProfiler()
    debugger = SimulationDebugger()
    
    print("  Running simulation...")
    for step in range(200):
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run neural computation
        for neural_step in range(10):
            brain.step(neural_step)
        
        # Take action and get result
        cmd = agent.decodeMotorCommand()
        result = world.applyMotorCommand(cmd, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update world and development
        world.update(0.01)
        agent.updateDevelopment(0.01)
        
        # Record metrics
        profiler.record_step(brain, cmd, result.reward, 0.01)
        
        if step % 20 == 0:
            debugger.log(f"Progress: Step {step}/{200}")
    
    # Analyze results
    stats = profiler.get_statistics()
    
    print("  ✓ Complete simulation finished!")
    print(f"    - Total reward: {stats['total_reward']:.2f}")
    print(f"    - Average firing rate: {stats['avg_spike_count']:.1f}")
    print(f"    - Average step time: {stats['avg_step_time']*1000:.2f}ms")
    print(f"    - Final developmental stage: {agent.getDevelopmentalStage()}")
    
    return stats

# Run the complete example
complete_stats = run_complete_example()

print("\n" + "="*60)
print("SUMMARY: All NLM Python Examples Completed Successfully!")
print("="*60)
print("\nThe NLM Python package now provides:")
print("✓ Enhanced configuration management with validation")
print("✓ Advanced simulation profiling and debugging")
print("✓ Visualization utilities for neural networks")
print("✓ Training and machine learning utilities")
print("✓ Batch simulation capabilities")
print("✓ Command-line interface tools")
print("✓ Complete integration examples")
print("\nNext steps:")
print("1. Install required dependencies: pip install matplotlib numpy")
print("2. Run your own NLM simulations using these examples")
print("3. Extend with custom training tasks and visualizations")
print("4. Integrate with your specific application requirements")