#!/usr/bin/env python3
"""
NLM - Advanced Features Example

This example demonstrates advanced NLM features including:
- Memory systems (working, episodic, associative)
- Prediction system
- Cognition systems (planning, concept formation, attention)
- Neuromodulation (curiosity, novelty, dopamine)
- Development system
- Advanced configuration
"""

import pynlm

def create_advanced_config():
    """Create an advanced configuration for research experiments."""
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.setInt("brain.neuron_count", 5000)
    config.setFloat("brain.synapse_density", 0.15)
    config.setFloat("brain.connection_probability", 0.08)
    config.setFloat("brain.v_thresh", -45.0)
    config.setFloat("brain.v_rest", -65.0)
    config.setFloat("brain.v_reset", -70.0)
    
    # Enhanced plasticity
    config.setBool("plasticity.stdp.enable", True)
    config.setFloat("plasticity.stdp.learning_rate", 0.001)
    config.setFloat("plasticity.stdp.tau_plus", 20.0)
    config.setFloat("plasticity.stdp.tau_minus", 20.0)
    config.setBool("plasticity.hebbian.enable", True)
    config.setBool("plasticity.structural.enable", True)
    config.setFloat("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.setFloat("plasticity.structural.pruning_rate", 0.00001)
    
    # Advanced neuromodulation
    config.setBool("neuromod.dopamine.enable", True)
    config.setFloat("neuromod.dopamine.scale", 1.5)
    config.setBool("neuromod.curiosity.enable", True)
    config.setFloat("neuromod.curiosity.exploration_bonus", 2.0)
    config.setBool("neuromod.novelty.enable", True)
    config.setFloat("neuromod.novelty.threshold", 0.5)
    
    # Memory system configuration
    config.setInt("memory.working.capacity", 500)
    config.setInt("memory.episodic.max_episodes", 5000)
    config.setInt("memory.associative.pattern_count", 1000)
    
    # Prediction system configuration
    config.setBool("prediction.enable", True)
    config.setFloat("prediction.accuracy_threshold", 0.8)
    config.setInt("prediction.error_window", 100)
    
    # Cognitive system configuration
    config.setBool("cognition.planner.enable", True)
    config.setFloat("cognition.planner.planning_horizon", 10.0)
    config.setBool("cognition.concept_formation.enable", True)
    config.setBool("cognition.attention.enable", True)
    
    # Development configuration
    config.setBool("development.enable", True)
    config.setFloat("development.initial_plasticity", 1.0)
    config.setFloat("development.maturation_rate", 0.01)
    config.setFloat("development.critical_period_end", 100.0)
    
    # Simulation parameters
    config.setFloat("simulation.timestep", 0.001)
    config.setInt("simulation.replay_interval", 50)
    config.setInt("simulation.consolidation_interval", 500)
    
    # Random seed for reproducibility
    config.setInt("random.seed", 12345)
    
    return config

def run_advanced_experiment(num_steps=500):
    """Run an advanced experiment with all NLM systems integrated."""
    print("=== NLM Advanced Features Experiment ===")
    print(f"Running {num_steps} simulation steps...")
    
    # Create advanced configuration
    config = create_advanced_config()
    print(f"Configuration loaded ({config.getKeys().size()} settings)")
    
    # Create brain with advanced config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced systems enabled:")
    print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  - Development: {agent.isDevelopmentEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run the experiment
    print("\nRunning experiment...")
    memory_snapshots = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect statistics every 50 steps
        if step % 50 == 0:
            # Get comprehensive brain stats
            brain_stats = {
                "step": step,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "excitement_inhibition_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": brain.getDevelopmentalStage(),
                "developmental_age": brain.getMemoryStats().get("developmental_age", 0.0),
                "working_memory_usage": brain.getMemoryStats().get("working_memory_usage", 0),
                "working_memory_capacity": brain.getMemoryStats().get("working_memory_capacity", 0),
                "episodic_memory_episodes": brain.getMemoryStats().get("episodic_memory_episodes", 0),
                "prediction_accuracy": brain.getMemoryStats().get("prediction_accuracy", 0.0),
                "planner_performance": brain.getMemoryStats().get("planner_performance", 0.0),
                "curiosity_level": agent.getCuriosityLevel(),
                "novelty_level": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError(),
                "neuromodulation_level": agent.getNeuromodulationLevel()
            }
            memory_snapshots.append(brain_stats)
            
            if step % 100 == 0:
                print(f"  Step {step:3d}: "
                      f"Spikes={brain_stats['total_spikes']:6d}, "
                      f"Firing={brain_stats['firing_neurons']:4d}, "
                      f"Rate={brain_stats['average_firing_rate']:6.2f}, "
                      f"Curiosity={agent.getCuriosityLevel():5.3f}, "
                      f"Novelty={agent.getNoveltyLevel():5.3f}")
    
    # Final results
    print("\n=== Experiment Results ===")
    final_stats = brain.getMemoryStats()
    
    print(f"Total simulation steps: {num_steps}")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final developmental age: {final_stats.get('developmental_age', 0.0):.2f}")
    
    print(f"\nMemory System Performance:")
    print(f"  - Working memory: {final_stats.get('working_memory_usage', 0)}/{final_stats.get('working_memory_capacity', 0)} slots used")
    print(f"  - Episodic memory: {final_stats.get('episodic_memory_episodes', 0)} episodes stored")
    print(f"  - Prediction accuracy: {final_stats.get('prediction_accuracy', 0.0):.3f}")
    print(f"  - Planning performance: {final_stats.get('planner_performance', 0.0):.3f}")
    
    print(f"\nNeuromodulation Levels:")
    print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Prediction error: {agent.getPredictionError():.3f}")
    print(f"  - Overall neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    print(f"\nConfiguration validation: {'PASS' if config.validate() else 'FAIL'}")
    if not config.validate():
        print(f"Validation errors: {config.getValidationErrors()}")
    
    return {
        "brain": brain,
        "world": world,
        "agent": agent,
        "config": config,
        "stats": final_stats,
        "memory_snapshots": memory_snapshots
    }

def main():
    """Main function to run the advanced example."""
    print("NLM Advanced Features Example")
    print("=" * 50)
    print("This example demonstrates all advanced NLM systems working together.")
    print("It may take a few seconds to complete.\n")
    
    # Run the advanced experiment
    results = run_advanced_experiment(300)
    
    print("\nAdvanced example complete!")
    print("The experiment demonstrates integration of all NLM systems:")
    print("  ✓ Memory systems (working, episodic, associative)")
    print("  ✓ Prediction system with accuracy tracking")
    print("  ✓ Cognition systems (planning, concept formation, attention)")
    print("  ✓ Neuromodulation (curiosity, novelty, dopamine)")
    print("  ✓ Developmental system with age-based learning")
    print("  ✓ Advanced configuration with JSON/YAML support")
    
    return results

if __name__ == "__main__":
    main()