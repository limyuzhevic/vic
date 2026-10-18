#!/usr/bin/env python3
"""
NLM Visualization Demo Script

This script demonstrates the enhanced visualization capabilities of NLM.
It shows real-time monitoring, statistical analysis, and various visualization modes.
"""

import sys
import time
import threading
from typing import Optional

# Try to import NLM
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Error: Could not import pynlm: {e}")
    print("Please install the NLM Python bindings using:")
    print("  pip install pynlm")
    sys.exit(1)


def create_sample_brain():
    """Create a sample brain for demonstration."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1f)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain


def demonstrate_basic_visualization(brain: pynlm.Brain):
    """Demonstrate basic visualization capabilities."""
    print("=== Basic Visualization Demo ===")
    print("This demonstrates the enhanced visualization interface.")
    print()
    
    # Show basic brain information
    print("Brain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print()
    
    # Simulate some activity
    print("Running simulation for 100 steps...")
    for step in range(100):
        brain.step(step)
        if step % 25 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"  Step {step:3d}: {firing:3d} firing neurons, {rate:6.2f} avg rate")
    
    print()
    print("Basic visualization complete!")


def demonstrate_agent_simulation_with_monitoring():
    """Demonstrate agent simulation with enhanced monitoring."""
    print("=== Agent Simulation with Enhanced Monitoring ===")
    print("This demo shows real-time monitoring and statistics collection.")
    print()
    
    # Create simulation components
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("region_count", 1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all monitoring capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Simulation Components:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  World: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  Agent: Enabled reward, curiosity, development, structural plasticity")
    print()
    
    # Run simulation with monitoring
    print("Running simulation with enhanced monitoring...")
    metrics_history = []
    
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on position and activity
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        
        position_reward = 1.0 if (0 <= x < width and 0 <= y < height) else -1.0
        activity_reward = brain.getFiringNeuronCount() * 0.01
        curiosity_reward = agent.getCuriosityLevel() * 0.1
        
        total_reward = position_reward + activity_reward + curiosity_reward
        agent.applyRewardModulation(total_reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record metrics for monitoring
        metrics = {
            'step': step,
            'total_reward': total_reward,
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'development_stage': brain.getDevelopmentalStage(),
            'brain_age': body.age if body else 0
        }
        metrics_history.append(metrics)
        
        # Print periodic updates
        if step % 50 == 0:
            print(f"  Step {step:3d}: "
                  f"reward={total_reward:6.2f}, "
                  f"curiosity={agent.getCuriosityLevel():.2f}, "
                  f"firing={brain.getFiringNeuronCount()}, "
                  f"dev_stage={brain.getDevelopmentalStage()}")
    
    # Analyze simulation results
    print()
    print("=== Simulation Analysis ===")
    
    final_step = metrics_history[-1]
    print(f"Final Statistics:")
    print(f"  Total reward: {sum(m['total_reward'] for m in metrics_history) / len(metrics_history):.2f}")
    print(f"  Average firing rate: {sum(m['avg_firing_rate'] for m in metrics_history) / len(metrics_history):.2f}")
    print(f"  Final curiosity level: {final_step['curiosity']:.2f}")
    print(f"  Final novelty level: {final_step['novelty']:.2f}")
    print(f"  Final neuromodulation level: {final_step['neuromodulation']:.2f}")
    print(f"  Final developmental stage: {final_step['development_stage']}")
    print(f"  Total neurons used: {brain.getTotalNeuronCount()}")
    print(f"  Active synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Calculate statistics
    avg_firing = sum(m['avg_firing_rate'] for m in metrics_history) / len(metrics_history)
    max_firing = max(m['avg_firing_rate'] for m in metrics_history)
    min_firing = min(m['avg_firing_rate'] for m in metrics_history)
    
    print()
    print("=== Performance Metrics ===")
    print(f"Firing Rate Statistics:")
    print(f"  Average: {avg_firing:.2f}")
    print(f"  Maximum: {max_firing:.2f}")
    print(f"  Minimum: {min_firing:.2f}")
    print(f"  Range: {max_firing - min_firing:.2f}")
    
    print()
    print("Enhanced monitoring simulation complete!")


def demonstrate_data_export():
    """Demonstrate data export functionality."""
    print("=== Data Export Demonstration ===")
    print("This demonstrates the enhanced data export capabilities.")
    print()
    
    # Create a sample brain
    brain = create_sample_brain()
    
    # Run simulation to generate data
    print("Running simulation to generate data for export...")
    for step in range(100):
        brain.step(step)
    
    print(f"Simulation complete. Generated data for {brain.getTotalNeuronCount()} neurons.")
    print()
    
    # Show available export formats
    export_formats = [
        ("CSV", "Comma-separated values"),
        ("JSON", "JavaScript Object Notation"),
        ("TEXT", "Plain text format"),
        ("MATLAB", "MATLAB data format"),
        ("PYTHON", "Python data structures"),
        ("DAT", "Raw binary data"),
        ("SVG", "Vector graphics"),
        ("PNG", "Image format")
    ]
    
    print("Available export formats:")
    for fmt, desc in export_formats:
        print(f"  {fmt:6s} - {desc}")
    
    print()
    print("Data export demonstration complete!")
    print("Note: Actual export functionality requires implementation of:")
    print("  - File writing and format handling")
    print("  - Compression algorithms")
    print("  - Validation and error handling")
    print("  - Progress tracking for large datasets")


def demonstrate_configuration_management():
    """Demonstrate configuration management capabilities."""
    print("=== Configuration Management Demo ===")
    print("This demonstrates the enhanced configuration system.")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set various configuration options
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15f)
    config.set("stdp_ltp_weight", 0.005f)
    config.set("stdp_ltd_weight", 0.006f)
    config.set("random_seed", 12345)
    config.set("simulation_timestep", 0.001)
    config.set("checkpoint_enabled", True)
    
    # Show configuration
    print("Current Configuration:")
    keys = config.getKeys()
    for key in sorted(keys):
        value = config.get(key)
        source = "unknown"
        print(f"  {key:30s}: {value} (source: {source})")
    
    print()
    print("Configuration management complete!")
    print("Note: Full implementation includes:")
    print("  - JSON/YAML file support")
    print("  - Configuration validation")
    print("  - Environment variable overrides")
    print("  - Schema validation")
    print("  - Configuration inheritance")


def main():
    """Main demo entry point."""
    print("=" * 60)
    print("NLM Enhanced Visualization and Monitoring Demo")
    print("=" * 60)
    print()
    
    if not PYNLM_AVAILABLE:
        sys.exit(1)
    
    demos = [
        ("Basic Visualization", demonstrate_basic_visualization),
        ("Agent Simulation with Monitoring", demonstrate_agent_simulation_with_monitoring),
        ("Data Export", demonstrate_data_export),
        ("Configuration Management", demonstrate_configuration_management),
    ]
    
    print("Available Demos:")
    for i, (name, _) in enumerate(demos, 1):
        print(f"  {i}. {name}")
    print()
    print("To run a specific demo, modify the script to call:")
    print("  demonstrate_basic_visualization(brain)")
    print("  demonstrate_agent_simulation_with_monitoring()")
    print("  demonstrate_data_export()")
    print("  demonstrate_configuration_management()")
    print()
    print("This script demonstrates the enhanced visualization capabilities")
    print("of the NLM framework, including real-time monitoring,")
    print("statistical analysis, and data export functionality.")
    
    # Run all demos
    for name, demo_func in demos:
        print()
        print("=" * 60)
        if demo_func.__name__ == "demonstrate_basic_visualization":
            brain = create_sample_brain()
            demo_func(brain)
        else:
            demo_func()


if __name__ == '__main__':
    main()
