#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings - Advanced Features Examples

This script demonstrates advanced functionality of the enhanced NLM Python bindings.
"""

import pynlm
import time

def advanced_neuromodulation_example():
    """Example of advanced neuromodulation control"""
    print("=== Advanced Neuromodulation Control ===")
    
    config = pynlm.createDefaultConfig()
    # Apply research preset manually
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.2f)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0f)
    config.set("development.mature_plasticity", 0.2f)
    config.set("simulation.timestep", 0.001f)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    agent.initialize(world)
    
    # Fine-tune neuromodulation parameters
    print("Initial neuromodulation levels:")
    print(f"  Dopamine: {brain.getDopamine().getLevel():.3f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    # Run simulation with enhanced control
    for step in range(30):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply adaptive reward modulation
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development with variable timestep
        agent.updateDevelopment(0.05 + (step % 5) * 0.01)
        
        if step % 10 == 0:
            print(f"Step {step}: Dev stage {brain.getDevelopmentalStage()}")
    
    print("Advanced neuromodulation example completed!\n")

def brain_state_analysis_example():
    """Example of detailed brain state analysis"""
    print("=== Brain State Analysis ===")
    
    config = pynlm.createDefaultConfig()
    # Apply research preset manually
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.2f)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0f)
    config.set("development.mature_plasticity", 0.2f)
    config.set("simulation.timestep", 0.001f)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run for a while to generate activity
    for step in range(100):
        brain.step(step)
    
    # Comprehensive brain analysis
    print("Brain State Analysis:")
    
    # 1. Neuron analysis
    neuron_stats = brain.getNeuronStats()
    print(f"  Total neurons: {neuron_stats['total']}")
    print(f"  Active neurons: {neuron_stats['active']} ({neuron_stats['active']/neuron_stats['total']*100:.1f}%)")
    print(f"  Firing neurons (current): {neuron_stats['firing']}")
    print(f"  Average firing rate: {neuron_stats['avg_firing_rate']:.3f}")
    print(f"  Total spikes: {neuron_stats['spike_count']}")
    print(f"  E/I ratio: {neuron_stats['ei_ratio']:.3f}")
    print(f"  Pending spike events: {neuron_stats['pending_spikes']}")
    
    # 2. Regional analysis
    regions = brain.getRegionStats()
    print(f"\n  Regional Analysis:")
    for region in regions:
        activity = region['neuron_count'] > 0
        print(f"    Region {region['id']} ({region['name']}): {region['neuron_count']} neurons")
        print(f"      Activity: {'Active' if activity else 'Inactive'}")
    
    # 3. Memory system analysis
    memory_stats = brain.getMemoryStats()
    print(f"\n  Memory System Status:")
    for key, value in memory_stats.items():
        print(f"    {key}: {value}")
    
    # 4. Neuromodulation analysis
    neuromod_stats = brain.getNeuromodulationStats()
    print(f"\n  Neuromodulation Analysis:")
    for key, value in neuromod_stats.items():
        print(f"    {key}: {value:.3f}")
    
    # 5. Developmental analysis
    print(f"\n  Developmental Analysis:")
    print(f"    Current stage: {brain.getDevelopmentalStage()}")
    
    print("Brain state analysis example completed!\n")

def config_workflow_example():
    """Example of advanced configuration workflows"""
    print("=== Configuration Workflows ===")
    
    # 1. Create minimal configuration manually
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.region_count", 1)
    config.set("brain.connection_probability", 0.05f)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 0.5f)
    print("Created minimal config")
    
    # 2. Create research configuration manually
    research_config = pynlm.createDefaultConfig()
    research_config.set("brain.neuron_count", 5000)
    research_config.set("brain.region_count", 3)
    research_config.set("brain.connection_probability", 0.2f)
    research_config.set("plasticity.stdp.enable", True)
    research_config.set("plasticity.hebbian.enable", True)
    research_config.set("plasticity.structural.enable", True)
    research_config.set("neuromod.dopamine.scale", 1.5f)
    research_config.set("neuromod.curiosity.enable", True)
    research_config.set("neuromod.novelty.enable", True)
    research_config.set("neuromod.prediction_error.enable", True)
    research_config.set("development.enable", True)
    
    # 3. Create custom configuration
    custom_config = pynlm.createDefaultConfig()
    custom_config.set("brain.neuron_count", 2500)
    custom_config.set("brain.region_count", 5)
    custom_config.set("brain.connection_probability", 0.15)
    custom_config.set("plasticity.stdp.enable", True)
    custom_config.set("plasticity.hebbian.enable", True)
    custom_config.set("plasticity.structural.enable", True)
    custom_config.set("neuromod.dopamine.scale", 1.2)
    custom_config.set("neuromod.curiosity.enable", True)
    custom_config.set("neuromod.novelty.enable", True)
    custom_config.set("neuromod.prediction_error.enable", True)
    custom_config.set("development.enable", True)
    
    print(f"\nCreated custom configuration:")
    print(f"  Neurons: {custom_config.get('brain.neuron_count')}")
    print(f"  Regions: {custom_config.get('brain.region_count')}")
    print(f"  Connection probability: {custom_config.get('brain.connection_probability')}")
    print(f"  STDP enabled: {custom_config.get('plasticity.stdp.enable')}")
    print(f"  Development enabled: {custom_config.get('development.enable')}")
    
    # Test config validation
    schema = {
        "required": ["brain.neuron_count", "brain.region_count"],
        "type": {
            "brain.neuron_count": "int",
            "brain.region_count": "int"
        }
    }
    
    print(f"\nConfig validation test:")
    is_valid, errors = custom_config.validateSchema(schema)
    print(f"  Valid: {is_valid}")
    if not is_valid:
        print(f"  Errors: {errors}")
    
    print("Configuration workflow example completed!\n")

def performance_benchmarking_example():
    """Example of performance benchmarking"""
    print("=== Performance Benchmarking ===")
    
    # Create configs for benchmarking
    minimal_config = pynlm.createDefaultConfig()
    minimal_config.set("brain.neuron_count", 100)
    minimal_config.set("brain.region_count", 1)
    minimal_config.set("brain.connection_probability", 0.05f)
    minimal_config.set("plasticity.stdp.enable", True)
    minimal_config.set("plasticity.hebbian.enable", True)
    minimal_config.set("neuromod.dopamine.scale", 0.5f)
    
    standard_config = pynlm.createDefaultConfig()
    standard_config.set("brain.neuron_count", 1000)
    standard_config.set("brain.region_count", 1)
    standard_config.set("brain.connection_probability", 0.1f)
    standard_config.set("plasticity.stdp.enable", True)
    standard_config.set("plasticity.hebbian.enable", True)
    standard_config.set("plasticity.structural.enable", False)
    standard_config.set("neuromod.dopamine.scale", 1.0f)
    standard_config.set("neuromod.curiosity.enable", True)
    standard_config.set("neuromod.novelty.enable", True)
    standard_config.set("development.enable", True)
    
    research_config = pynlm.createDefaultConfig()
    research_config.set("brain.neuron_count", 5000)
    research_config.set("brain.region_count", 3)
    research_config.set("brain.connection_probability", 0.2f)
    research_config.set("plasticity.stdp.enable", True)
    research_config.set("plasticity.hebbian.enable", True)
    research_config.set("plasticity.structural.enable", True)
    research_config.set("neuromod.dopamine.scale", 1.5f)
    research_config.set("neuromod.curiosity.enable", True)
    research_config.set("neuromod.novelty.enable", True)
    research_config.set("neuromod.prediction_error.enable", True)
    research_config.set("development.enable", True)
    research_config.set("development.initial_plasticity", 1.0f)
    research_config.set("development.mature_plasticity", 0.2f)
    research_config.set("simulation.timestep", 0.001f)
    
    configs_to_test = [
        ("minimal", minimal_config),
        ("standard", standard_config),
        ("research", research_config)
    ]
    
    for config_name, config_obj in configs_to_test:
        print(f"\nBenchmarking {config_name} configuration:")
        
        brain = pynlm.createBrain(config_obj)
        brain.initialize()
        
        # Benchmark single steps
        num_steps = 100
        start_time = time.time()
        
        for step in range(num_steps):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"  Single step average: {duration/num_steps*1000:.2f} ms")
        print(f"  Steps per second: {num_steps/duration:.0f}")
        print(f"  Neurons processed: {brain.getTotalNeuronCount() * num_steps}")
    
    # Benchmark batch operations
    print(f"\nBenchmarking batch operations:")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    brain.stepMultiple(0, 1000, 0.001)
    end_time = time.time()
    
    duration = end_time - start_time
    print(f"  1000 steps (batch): {duration:.3f} seconds")
    print(f"  Average per step: {duration/1000*1000:.2f} ms")
    
    print("Performance benchmarking example completed!\n")

def development_cycle_example():
    """Example of development cycle and maturation"""
    print("=== Development Cycle Simulation ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize and run through developmental stages
    brain.initialize()
    
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Initial plasticity factor: {brain.getDevelopmentalStage().plasticity_factor():.3f}")
    
    # Simulate development over time
    for stage_step in range(0, 600, 100):
        # Run a few steps
        for step in range(10):
            brain.step(stage_step + step)
        
        # Check developmental stage
        stage = brain.getDevelopmentalStage()
        print(f"\nAfter {stage_step + 10} steps:")
        print(f"  Developmental stage: {stage}")
        print(f"  Plasticity factor: {stage.plasticity_factor():.3f}")
        print(f"  Neuronal activity: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    print("\nDevelopment cycle example completed!\n")

def main():
    """Run all advanced examples"""
    print("Enhanced NLM Python Bindings - Advanced Features Examples")
    print("=" * 60)
    
    try:
        advanced_neuromodulation_example()
        brain_state_analysis_example()
        config_workflow_example()
        performance_benchmarking_example()
        development_cycle_example()
        
        print("=" * 60)
        print("All advanced examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
