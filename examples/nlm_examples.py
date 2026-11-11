#!/usr/bin/env python3

"""
NLM Examples and Utilities

This file demonstrates various ways to use the NLM Python API and provides
utility functions to make working with NLM brains easier and more intuitive.
"""

import pynlm
import time
import json
from typing import Optional, List, Dict, Any


def create_standard_brain(neuron_count: int = 500, region_count: int = 1) -> pynlm.Brain:
    """Create a standard brain with common Phase 6 settings."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain


def run_training_simulation(brain: pynlm.Brain, steps: int, verbose: bool = False) -> Dict[str, Any]:
    """Run a standard training simulation with basic plasticity."""
    print(f"Starting training simulation for {steps} steps...")
    
    # Enable all plasticity mechanisms
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        for syn in region.getSynapses():
            syn.enablePlasticity(True, True, False)  # Hebbian and STDP
    
    start_time = time.time()
    initial_firing = brain.getFiringNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    
    for step in range(steps):
        # Simple pattern: stimulate sensory neurons
        if step % 10 == 0 and brain.getTotalNeuronCount() > 0:
            brain.injectCurrent(NeuronId(0), 30.0)
        
        brain.step(step)
        
        if verbose and step % 100 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, {brain.getTotalSpikeCount() - initial_spikes} new spikes")
    
    end_time = time.time()
    
    stats = {
        'steps_completed': steps,
        'total_spikes': brain.getTotalSpikeCount() - initial_spikes,
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'ei_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'duration_seconds': end_time - start_time,
        'neurons_per_second': steps / (end_time - start_time) if end_time > start_time else 0
    }
    
    print(f"Training completed in {stats['duration_seconds']:.2f} seconds")
    print(f"  Spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
    
    return stats


def create_agent_world(width: int = 20, height: int = 20, vision_width: int = 8, vision_height: int = 8):
    """Create a standard agent setup with world."""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    
    return world


def create_agent_system(brain: pynlm.Brain, world: pynlm.SimpleWorld) -> pynlm.AgentBrain:
    """Create an agent system connecting brain to world."""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    return agent


def run_complete_agent_episode(agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                             steps: int, enable_learning: bool = True) -> Dict[str, Any]:
    """Run a complete agent episode."""
    print(f"Starting agent episode for {steps} steps...")
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    brain = agent.getBrain()
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward (simple curiosity-based reward)
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 50 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}")
    
    stats = {
        'steps_completed': steps,
        'final_firing': brain.getFiringNeuronCount(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_prediction_error': agent.getPredictionError(),
        'final_development_stage': brain.getDevelopmentalStage(),
        'world_time': world.getSimulationTime()
    }
    
    print(f"Episode completed. Development stage: {stats['final_development_stage']}")
    
    return stats


def save_brain_with_metadata(brain: pynlm.Brain, filepath: str, metadata: Optional[Dict[str, Any]] = None):
    """Save brain state with additional metadata."""
    brain.save(filepath)
    
    if metadata:
        # Save metadata separately
        metadata_file = filepath.replace('.bin', '_metadata.json')
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        print(f"Brain saved with metadata to: {metadata_file}")
    
    print(f"Brain saved to: {filepath}")


def load_brain_with_metadata(brain: pynlm.Brain, filepath: str) -> Optional[Dict[str, Any]]:
    """Load brain state and return metadata if available."""
    if brain.load(filepath):
        # Try to load metadata
        metadata_file = filepath.replace('.bin', '_metadata.json')
        try:
            with open(metadata_file, 'r') as f:
                metadata = json.load(f)
            print(f"Brain loaded with metadata from: {metadata_file}")
            return metadata
        except FileNotFoundError:
            print(f"Brain loaded (no metadata file: {metadata_file})")
            return None
    else:
        print(f"Failed to load brain from: {filepath}")
        return None


def benchmark_configuration(config: pynlm.Config, steps: int = 1000) -> Dict[str, Any]:
    """Benchmark a brain configuration."""
    print(f"Benchmarking configuration with {steps} steps...")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    duration = end_time - start_time
    spikes_per_step = brain.getTotalSpikeCount() / steps
    neurons_per_second = brain.getTotalNeuronCount() / duration if duration > 0 else 0
    
    stats = {
        'steps': steps,
        'duration_seconds': duration,
        'neurons_per_second': neurons_per_second,
        'total_spikes': brain.getTotalSpikeCount(),
        'spikes_per_step': spikes_per_step,
        'final_firing': brain.getFiringNeuronCount(),
        'ei_ratio': brain.getExcitationInhibitionRatio()
    }
    
    print(f"Benchmark completed in {duration:.2f} seconds")
    print(f"  Neurons per second: {neurons_per_second:.0f}")
    print(f"  Spikes per step: {spikes_per_step:.2f}")
    
    return stats


def generate_test_config(name: str = "test_config") -> pynlm.Config:
    """Generate a test configuration with common settings."""
    config = pynlm.createDefaultConfig()
    
    # Basic brain parameters
    config.set(f"{name}.neuron_count", 500)
    config.set(f"{name}.region_count", 1)
    config.set(f"{name}.connection_probability", 0.1)
    
    # Plasticity settings
    config.set(f"{name}.plasticity_learning_rate", 0.01)
    config.set(f"{name}.plasticity_stdp_enable", True)
    config.set(f"{name}.plasticity_hebbian_enable", True)
    
    # Neuromodulation
    config.set(f"{name}.neuromod.dopamine_baseline", 0.1)
    config.set(f"{name}.neuromod.curiosity_enable", True)
    config.set(f"{name}.neuromod.novelty_enable", True)
    
    # Simulation parameters
    config.set(f"{name}.simulation_timestep", 0.001)
    config.set(f"{name}.max_simulation_steps", 10000)
    
    return config


def validate_brain_state(brain: pynlm.Brain) -> List[str]:
    """Validate brain state and return list of warnings/errors."""
    issues = []
    
    if brain.getTotalNeuronCount() == 0:
        issues.append("WARNING: Brain has no neurons")
    
    if brain.getTotalSynapseCount() == 0:
        issues.append("WARNING: Brain has no synapses")
    
    firing_rate = brain.getAverageFiringRate()
    if firing_rate > 100:  # Too high firing rate
        issues.append(f"WARNING: Very high firing rate: {firing_rate:.2f} Hz")
    elif firing_rate < 0.1:  # Too low firing rate
        issues.append(f"WARNING: Very low firing rate: {firing_rate:.2f} Hz")
    
    ei_ratio = brain.getExcitationInhibitionRatio()
    if ei_ratio > 10 or ei_ratio < 0.1:  # Unbalanced E/I ratio
        issues.append(f"WARNING: Unbalanced E/I ratio: {ei_ratio:.2f}")
    
    dev_stage = brain.getDevelopmentalStage()
    if dev_stage == pynlm.DevelopmentalStage.Aging:
        issues.append("INFO: Brain is in aging stage - plasticity may be reduced")
    
    if not issues:
        issues.append("SUCCESS: Brain state appears valid")
    
    for issue in issues:
        print(issue)
    
    return issues


def create_reproduction_script() -> str:
    """Generate a complete reproduction script for a brain configuration."""
    script = '''
import pynlm

# Configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed with {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
'''
    return script


def run_example_1():
    """Example 1: Simple brain simulation."""
    print("=== Example 1: Simple Brain Simulation ===")
    brain = create_standard_brain(neuron_count=100, region_count=1)
    
    for i in range(50):
        brain.step(i)
        if i % 10 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print()


def run_example_2():
    """Example 2: Complete agent system."""
    print("=== Example 2: Complete Agent System ===")
    
    # Create components
    brain = create_standard_brain(neuron_count=200)
    world = create_agent_world(15, 15, 6, 6)
    agent = create_agent_system(brain, world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run episode
    stats = run_complete_agent_episode(agent, world, 100, enable_learning=True)
    
    print(f"Episode completed. Final curiosity: {stats['final_curiosity']:.3f}")
    print()


def run_example_3():
    """Example 3: Configuration benchmarking."""
    print("=== Example 3: Configuration Benchmarking ===")
    
    config = generate_test_config()
    
    # Benchmark different configurations
    configs = [
        ("Small brain", generate_test_config("small")),
        ("Medium brain", config),
        ("Large brain", generate_test_config("large")),
    ]
    
    results = {}
    
    for name, test_config in configs:
        print(f"\nBenchmarking {name}:")
        results[name] = benchmark_configuration(test_config, steps=200)
    
    # Find best configuration
    best_config = max(results.items(), key=lambda x: x[1]['neurons_per_second'])
    print(f"\nBest configuration: {best_config[0]} ({best_config[1]['neurons_per_second']:.0f} neurons/second)")
    print()


if __name__ == "__main__":
    print("NLM Examples and Utilities")
    print("=" * 50)
    print()
    
    # Run all examples
    run_example_1()
    run_example_2()
    run_example_3()
    
    print("All examples completed successfully!")
