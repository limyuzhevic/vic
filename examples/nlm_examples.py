#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python API Examples
==================================================

This file demonstrates the enhanced Python bindings for NLM, showcasing
the comprehensive functionality exposed through the pynlm module.

The NLM framework provides a brain-inspired computational system with:
- Real spiking neural computation (LIF neurons)
- Multiple memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Prediction and cognition systems
- Development and structural plasticity
- Full integration of all brain systems

Installation:
    pip install pynlm

Basic Usage Examples:
"""

# Example 1: Basic Brain Setup and Simulation
import pynlm

def basic_simulation_example():
    """Create and run a basic NLM simulation"""
    print("=== Basic NLM Simulation Example ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure simulation parameters
    config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
    config.set("region_count", 3, pynlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.15, pynlm.ConfigSource.Runtime)
    config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Runtime)
    
    # Create brain
    brain = pynlm.createBrain(config)
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Number of regions: {brain.getRegionCount()}")
    
    # Initialize brain
    if brain.initialize():
        print("Brain initialized successfully")
    else:
        print("Failed to initialize brain")
        return
    
    # Run simulation steps
    for step in range(10):
        brain.step(step, step * config.getOr("simulation_timestep", 0.001))
    
    # Display statistics
    print(f"Simulation completed:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    brain.reset()
    print("Brain reset completed\n")

# Example 2: Memory Systems Integration
import pynlm

def memory_systems_example():
    """Demonstrate integrated memory systems"""
    print("=== Memory Systems Integration Example ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200, pynlm.ConfigSource.Runtime)
    config.set("region_count", 2, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access different memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Memory Systems Status:")
    print(f"  Working memory: {working_memory if working_memory else 'None'}")
    print(f"  Episodic memory: {episodic_memory if episodic_memory else 'None'}")
    print(f"  Associative memory: {associative_memory if associative_memory else 'None'}")
    
    # Check if memories are properly connected to brain
    if working_memory and episodic_memory:
        print("  ✓ Memory systems are integrated with neural processing")
    
    brain.reset()
    print("Memory systems example completed\n")

# Example 3: Neuromodulation and Learning
import pynlm

def neuromodulation_example():
    """Demonstrate neuromodulation systems"""
    print("=== Neuromodulation and Learning Example ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300, pynlm.ConfigSource.Runtime)
    config.set("region_count", 2, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access neuromodulators
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print("Neuromodulation Systems:")
    print(f"  Dopamine (reward): {dopamine if dopamine else 'None'}")
    print(f"  Curiosity (exploration): {curiosity if curiosity else 'None'}")
    print(f"  Novelty detection: {novelty if novelty else 'None'}")
    print(f"  Prediction error: {prediction_error if prediction_error else 'None'}")
    
    # Create agent brain for simulation
    world = pynlm.createSimpleWorld()
    world.configure(80, 80, 16, 16)  # width, height, vision_width, vision_height
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Get agent state
    print(f"  Agent developmental stage: {agent_brain.getDevelopmentalStage()}")
    print(f"  Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent_brain.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent_brain.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent_brain.getPredictionError():.3f}")
    
    # Apply reward-based learning
    agent_brain.applyRewardModulation(1.0, 0.8)  # reward, predicted_reward
    print(f"  After reward modulation:")
    print(f"    Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"    Prediction error: {agent_brain.getPredictionError():.3f}")
    
    brain.reset()
    print("Neuromodulation example completed\n")

# Example 4: Agent in World Simulation
import pynlm

def agent_world_simulation_example():
    """Demonstrate agent-world interaction"""
    print("=== Agent-World Simulation Example ===")
    
    # Create world and brain
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 32, 32)  # Larger world for better exploration
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 400, pynlm.ConfigSource.Runtime)
    config.set("region_count", 3, pynlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.12, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Set agent starting position
    world.setAgentStart(50, 50)
    
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision dimensions: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    print(f"Agent start position: (50, 50)")
    
    # Get initial agent state
    agent_body = world.getAgentBody()
    print(f"Initial agent state: x={agent_body.x:.1f}, y={agent_body.y:.1f}, health={agent_body.health:.1f}")
    
    # Run simulation loop
    sensory_percept = world.getSensoryPercept()
    
    for episode in range(3):  # 3 episodes
        print(f"\n--- Episode {episode + 1} ---")
        
        # Reset for new episode
        agent_brain.reset()
        brain.reset()
        brain.initialize()
        world.reset()
        world.setAgentStart(50, 50)
        
        # Run simulation steps
        for step in range(100):
            # Get current percept
            world.update(step * 0.001)
            
            # Process sensory input
            agent_brain.processSensoryInput(world.getSensoryPercept())
            
            # Brain step
            brain.step(step, step * 0.001)
            
            # Decode motor command
            motor_command = agent_brain.decodeMotorCommand()
            
            # Apply motor command
            result = world.applyMotorCommand(motor_command, step * 0.001)
            
            # Apply curiosity-based exploration
            if agent_brain.getCuriosityLevel() > 0.3:
                print(f"  Step {step}: Curiously exploring! Command: {motor_command}")
            
            # Log progress every 20 steps
            if step % 20 == 0:
                print(f"  Step {step}: Position ({agent_body.x:.1f}, {agent_body.y:.1f})")
                print(f"    Neuromodulation: {agent_brain.getNeuromodulationLevel():.3f}")
                print(f"    Curiosity: {agent_brain.getCuriosityLevel():.3f}")
                print(f"    Novelty: {agent_brain.getNoveltyLevel():.3f}")
                print(f"    Prediction error: {agent_brain.getPredictionError():.3f}")
            
            # Check if episode ended
            if agent_body.health <= 0 or step >= 99:
                print(f"  Episode ended at step {step}")
                break
    
    print("\nAgent-world simulation completed")
    brain.reset()
    print("Example completed\n")

# Example 5: Configuration Management
import pynlm

def configuration_management_example():
    """Demonstrate advanced configuration management"""
    print("=== Configuration Management Example ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set various configuration values
    config.set("simulation_timestep", 0.0005, pynlm.ConfigSource.Runtime)
    config.set("neuron_count", 1000, pynlm.ConfigSource.Runtime)
    config.set("region_count", 5, pynlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.08, pynlm.ConfigSource.Runtime)
    config.set("max_energy", 100.0, pynlm.ConfigSource.Runtime)
    config.set("energy_decay_rate", 0.01, pynlm.ConfigSource.Runtime)
    
    # Set neuromodulation parameters
    config.set("stdp_ltp_weight", 0.03, pynlm.ConfigSource.Runtime)
    config.set("stdp_ltd_weight", 0.025, pynlm.ConfigSource.Runtime)
    config.set("stdp_tau", 25.0, pynlm.ConfigSource.Runtime)
    
    # Set development parameters
    config.set("synaptogenesis_rate", 0.00005, pynlm.ConfigSource.Runtime)
    config.set("pruning_rate", 0.000005, pynlm.ConfigSource.Runtime)
    
    print("Configuration Summary:")
    print(config.summary())
    
    # Demonstrate type-safe value access
    timestep = config.getOr("simulation_timestep", 0.001)
    neurons = config.getOr("neuron_count", 100)
    regions = config.getOr("region_count", 1)
    
    print(f"Typed configuration values:")
    print(f"  Simulation timestep: {timestep} seconds")
    print(f"  Neuron count: {neurons}")
    print(f"  Region count: {regions}")
    
    # Demonstrate configuration validation
    if config.has("neuron_count"):
        print("✓ neuron_count configuration exists")
    
    # Remove and add configuration
    config.remove("max_energy")
    print("Removed max_energy configuration")
    
    config.set("learning_rate", 0.001, pynlm.ConfigSource.Runtime)
    print("Added learning_rate configuration")
    
    print(f"Current keys: {config.getKeys()}")
    
    # Save configuration to file
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        temp_file = f.name
    
    try:
        if config.saveToFile(temp_file):
            print(f"✓ Configuration saved to {temp_file}")
        else:
            print(f"✗ Failed to save configuration")
        
        # Load configuration from file
        new_config = pynlm.createDefaultConfig()
        if new_config.loadFromFile(temp_file):
            print("✓ Configuration loaded from file")
            print(f"  Loaded timestep: {new_config.getOr('simulation_timestep', 0.001)}")
        else:
            print("✗ Failed to load configuration")
    
    finally:
        # Clean up temporary file
        if os.path.exists(temp_file):
            os.unlink(temp_file)
            print(f"Cleaned up temporary file: {temp_file}")
    
    print("Configuration management example completed\n")

# Example 6: Performance Monitoring
import pynlm

def performance_monitoring_example():
    """Demonstrate performance monitoring capabilities"""
    print("=== Performance Monitoring Example ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 800, pynlm.ConfigSource.Runtime)
    config.set("region_count", 4, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Performance Monitoring:")
    print(f"  Total neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Total synapse count: {brain.getTotalSynapseCount()}")
    print(f"  Total spike count: {brain.getTotalSpikeCount()}")
    print(f"  Pending spike events: {brain.getPendingSpikeEventCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
    
    # Run performance test
    import time
    start_time = time.time()
    
    for step in range(50):
        brain.step(step, step * 0.001)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"\nPerformance Test Results:")
    print(f"  Simulated {50} steps in {elapsed:.3f} seconds")
    print(f"  Average time per step: {elapsed/50*1000:.2f} ms")
    print(f"  Steps per second: {50/elapsed:.1f}")
    
    brain.logStatus()
    brain.reset()
    print("Performance monitoring example completed\n")

# Example 7: Advanced Inter-region Connections
import pynlm

def inter_region_connections_example():
    """Demonstrate inter-region connectivity"""
    print("=== Inter-Region Connections Example ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 600, pynlm.ConfigSource.Runtime)
    config.set("region_count", 4, pynlm.ConfigSource.Runtime)  # More regions for connections
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Initial region setup:")
    print(f"  Number of regions: {brain.getRegionCount()}")
    print(f"  Region IDs: {brain.getRegionIds()}")
    
    # Add inter-region connections
    region_ids = brain.getRegionIds()
    
    if len(region_ids) >= 3:
        print("\nAdding inter-region connections:")
        
        # Connect region 0 to region 1 (feedforward)
        brain.addInterRegionConnection(region_ids[0], region_ids[1], 1.5, 2)
        print(f"  ✓ Connected region {region_ids[0]} → {region_ids[1]} (weight: 1.5, delay: 2)")
        
        # Connect region 2 to region 3 (feedforward)
        brain.addInterRegionConnection(region_ids[2], region_ids[3], 1.2, 1)
        print(f"  ✓ Connected region {region_ids[2]} → {region_ids[3]} (weight: 1.2, delay: 1)")
        
        # Connect region 1 back to region 0 (feedback)
        brain.addInterRegionConnection(region_ids[1], region_ids[0], 0.8, 3)
        print(f"  ✓ Connected region {region_ids[1]} → {region_ids[0]} (weight: 0.8, delay: 3)")
        
        # Show connections (we can access regions to inspect connections)
        print(f"  Total regions: {brain.getRegionCount()}")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        
        # Run simulation with connections
        for step in range(20):
            brain.step(step, step * 0.001)
        
        print(f"\nAfter 20 steps with inter-region connections:")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  Network activity: {'Active' if brain.getTotalSpikeCount() > 0 else 'Inactive'}")
        
        brain.reset()
    
    print("Inter-region connections example completed\n")

# Main execution
def main():
    """Run all examples"""
    print("=" * 60)
    print("NLM Python API Examples")
    print("=" * 60)
    print()
    
    try:
        basic_simulation_example()
        memory_systems_example()
        neuromodulation_example()
        agent_world_simulation_example()
        configuration_management_example()
        performance_monitoring_example()
        inter_region_connections_example()
        
        print("=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"Error during execution: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()