"""
Advanced NLM Examples
=====================

These examples demonstrate advanced usage patterns and features
of the NLM Python bindings.

Save as: examples_advanced.py
"""

import pynlm

def example_advanced_brain_creation():
    """Example of advanced brain configuration and customization."""
    print("=== Advanced Brain Creation ===")
    
    # Create configuration with custom parameters
    config = pynlm.createDefaultConfig()
    
    # Customize neural properties
    config.setFloat("simulation_timestep", 0.0005)
    config.setFloat("v_thresh", -45.0)
    config.setFloat("v_rest", -65.0)
    config.setFloat("v_reset", -60.0)
    config.setFloat("tau_mem", 15.0)
    config.setFloat("tau_ref", 3.0)
    
    # Set plasticity parameters
    config.setFloat("plasticity.stdp.enable", True)
    config.setFloat("plasticity.stdp.learning_rate", 0.005)
    config.setFloat("plasticity.stdp.tau_plus", 15.0)
    config.setFloat("plasticity.stdp.tau_minus", 15.0)
    
    config.setFloat("plasticity.hebbian.enable", True)
    config.setFloat("plasticity.structural.enable", True)
    
    # Set neuromodulation
    config.setFloat("neuromod.dopamine.scale", 1.5)
    config.setFloat("neuromod.curiosity.enable", True)
    config.setFloat("neuromod.novelty.enable", True)
    
    # Set development parameters
    config.setFloat("development.initial_plasticity", 1.0)
    config.setFloat("development.critical_plasticity", 0.7)
    config.setFloat("development.maturation_plasticity", 0.4)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Advanced brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print()
    
    return brain

def example_memory_systems():
    """Example demonstrating integrated memory systems."""
    print("=== Memory Systems Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get memory system pointers
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity() if working_memory else 'N/A'}")
    print(f"Episodic memory max episodes: {episodic_memory.getMaxEpisodes() if episodic_memory else 'N/A'}")
    
    # Simulate memory operations
    if working_memory:
        print("Working memory integrated with brain")
    
    if episodic_memory:
        print("Episodic memory integrated with brain")
    
    if associative_memory:
        print("Associative memory integrated with brain")
    
    print()
    return brain

def example_prediction_system():
    """Example demonstrating prediction system integration."""
    print("=== Prediction System Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    prediction_system = brain.getPredictionSystem()
    
    if prediction_system:
        print("Prediction system integrated with brain")
        # The prediction system would be used by agent for action selection
        # This is typically handled by the AgentBrain class
    else:
        print("Prediction system not available (check C++ implementation)")
    
    print()
    return brain

def example_cognition_systems():
    """Example demonstrating cognition systems."""
    print("=== Cognition Systems Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    if planner:
        print("Neural planner integrated with brain")
        planner.setPlanningDepth(3)
    
    if concept_formation:
        print("Concept formation integrated with brain")
        concept_formation.setPatternThreshold(0.7)
    
    if attention:
        print("Attentional selection integrated with brain")
        attention.setInhibitionStrength(0.6)
        attention.setExcitationStrength(1.8)
    
    print()
    return brain

def example_neuromodulation():
    """Example demonstrating neuromodulation systems."""
    print("=== Neuromodulation Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Create agent to access neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    print("Neuromodulation systems:")
    print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    # Enable different neuromodulators
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Reward modulation and curiosity enabled")
    print()
    return brain, agent

def example_developmental_plasticity():
    """Example demonstrating developmental plasticity."""
    print("=== Developmental Plasticity Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    print(f"Initial developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Initial plasticity modifier: {agent.getBrain().getConfig().getFloat('plasticity_modifier', 1.0)}")
    
    # Simulate development over time
    for timestep in [0.0, 60.0, 120.0, 300.0, 600.0]:
        agent.updateDevelopment(timestep)
        print(f"After {timestep}s: Stage={agent.getDevelopmentalStage()}, "
              f"Plasticity={agent.getBrain().getConfig().getFloat('plasticity_modifier', 1.0):.2f}")
    
    print()
    return brain, agent

def example_checkpoint_system():
    """Example demonstrating checkpoint system."""
    print("=== Checkpoint System Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run a few steps
    for step in range(10):
        brain.step(step)
    
    print(f"Brain state after {brain.getTotalSpikeCount()} spikes")
    
    # Save checkpoint
    checkpoint_file = "nlm_checkpoint.bin"
    try:
        success = brain.save(checkpoint_file)
        if success:
            print(f"Checkpoint saved to {checkpoint_file}")
        else:
            print("Failed to save checkpoint")
    except Exception as e:
        print(f"Error saving checkpoint: {e}")
    
    # Reset and reload
    brain.reset()
    print("Brain reset")
    
    # Try to load checkpoint (will fail since we just reset)
    try:
        success = brain.load(checkpoint_file)
        if success:
            print("Checkpoint loaded successfully")
        else:
            print("Failed to load checkpoint (file may not exist)")
    except Exception as e:
        print(f"Error loading checkpoint: {e}")
    
    print()

def example_visualization_interface():
    """Example of visualization interface (if available)."""
    print("=== Visualization Interface Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    visualization = brain.getVisualizationInterface()
    
    if visualization:
        print("Visualization interface available")
        # Various visualization methods would go here
        # visualization.show_neural_activity()
        # visualization.plot_spike_raster()
        # etc.
    else:
        print("Visualization interface not available (check C++ implementation)")
    
    print()

def example_performance_monitoring():
    """Example of performance monitoring."""
    print("=== Performance Monitoring Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get performance statistics
    import time
    start_time = time.time()
    
    # Run many steps to measure performance
    num_steps = 1000
    for step in range(num_steps):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Performance metrics:")
    print(f"  Steps per second: {num_steps / elapsed:.0f}")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Time per step: {elapsed / num_steps * 1000:.2f} ms")
    
    print()

def main():
    """Run all advanced examples."""
    print("Advanced NLM Python Examples")
    print("==========================")
    print()
    
    example_advanced_brain_creation()
    example_memory_systems()
    example_prediction_system()
    example_cognition_systems()
    example_neuromodulation()
    example_developmental_plasticity()
    example_checkpoint_system()
    example_visualization_interface()
    example_performance_monitoring()
    
    print("All advanced examples completed!")

if __name__ == "__main__":
    main()
