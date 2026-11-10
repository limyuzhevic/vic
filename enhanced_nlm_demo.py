#!/usr/bin/env python3
"""
Enhanced NLM Python API - Basic Demonstration

This script demonstrates the enhanced NLM Python API capabilities,
showing how to use the new neuroscience classes and systems.

Author: NLM Development Team
Version: 2.0.0
"""

import pynlm
import time

def print_header(title):
    """Print a formatted header for section output."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def print_subheader(title):
    """Print a formatted subheader."""
    print(f"\n{'-'*60}")
    print(f"{title}")
    print(f"{'-'*60}")

def basic_brain_simulation():
    """Demonstrate basic brain simulation with enhanced API."""
    print_subheader("Basic Brain Simulation")

    # Create enhanced configuration
    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 500,
        'brain.synapse_density': 0.15,
        'plasticity.stdp.enable': True,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True
    })

    print(f"Configuration summary: {config.summary()}")

    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()

    print(f"Brain initialized:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")

    # Run simulation
    print(f"\nRunning simulation (100 steps)...")
    for step in range(100):
        brain.step(step)

        if step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")

    print(f"\nSimulation complete!")
    return brain

def memory_system_demonstration():
    """Demonstrate memory system capabilities."""
    print_subheader("Memory System Demonstration")

    # Create brain with memory support
    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 200,
        'neuromod.curiosity.enable': True
    })

    brain = pynlm.createBrain(config)
    brain.initialize()

    # Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()

    print(f"Working Memory:")
    print(f"  - Capacity: {working_memory.getActiveTraces()} traces")

    # Store some activation traces
    print(f"\nStoring activation traces...")
    working_memory.store_to_neuron(1, 0.8)
    working_memory.store_to_neuron(5, 0.6)
    working_memory.store_to_neuron(10, 0.9)

    working_memory.update(0.1)
    print(f"  - After update: {working_memory.getActiveTraces()} traces")
    print(f"  - Neuron 1 activation: {working_memory.getActivation(1):.3f}")

    # Create an episode
    print(f"\nCreating episodic memory episode...")
    episode = pynlm.EpisodicMemoryItem()
    episode.set_position(3.0, 4.0)
    episode.set_action(pynlm.ActionType.MoveForward)
    episode.set_reward(0.8)
    episode.set_energy(0.9)
    episode.set_novelty(0.3)

    episodic_memory.store_episode(episode)
    print(f"  - Episode stored: position ({episode.get_position()[0]:.1f}, {episode.get_position()[1]:.1f})")
    print(f"  - Action: {episode.get_action().name}")
    print(f"  - Reward: {episode.get_reward():.3f}")

    # Replay episodes
    episodes = episodic_memory.get_episodes_for_replay(5)
    print(f"\nReplaying {len(episodes)} episodes...")
    for i, ep in enumerate(episodes):
        episodic_memory.replay_episode(ep)
        print(f"  Episode {i+1}: reward {ep.get_reward():.3f}")

    print(f"Total episodes: {episodic_memory.getEpisodeCount()}")
    return brain

def neuromodulation_demonstration():
    """Demonstrate neuromodulation capabilities."""
    print_subheader("Neuromodulation Demonstration")

    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 300,
        'neuromod.dopamine.scale': 1.5,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True
    })

    brain = pynlm.createBrain(config)
    brain.initialize()

    # Access neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNoveltyDetector()

    print(f"Neuromodulation Systems:")
    print(f"  - Dopamine enabled: {config.has('neuromod.dopamine.scale')}")
    print(f"  - Curiosity enabled: {config.has('neuromod.curiosity.enable')}")
    print(f"  - Novelty enabled: {config.has('neuromod.novelty.enable')}")

    # Simulate some activity to update neuromodulators
    print(f"\nSimulating activity and updating neuromodulators...")
    for step in range(50):
        brain.step(step)

        # Update neuromodulators (simplified simulation)
        novelty.update([0.5, 0.3, 0.7, 0.2, 0.8], 0.1)  # Example sensory input
        curiosity.update(novelty.getLevel(), 0.0, 0.1)
        dopamine.update(0.1)

        if step % 10 == 0:
            print(f"  Step {step}: curiosity={curiosity.getLevel():.3f}, "
                  f"novelty={novelty.getLevel():.3f}, dopamine={dopamine.getLevel():.3f}")

    # Check neuromodulation levels
    print(f"\nFinal neuromodulation levels:")
    print(f"  - Curiosity level: {curiosity.getLevel():.3f}")
    print(f"  - Novelty level: {novelty.getLevel():.3f}")
    print(f"  - Dopamine level: {dopamine.getLevel():.3f}")
    print(f"  - Dopamine plasticity factor: {dopamine.getPlasticityFactor():.3f}")

    # Demonstrate curiosity-driven behavior
    if curiosity.getLevel() > 0.5:
        print(f"\nHigh curiosity detected - promoting exploration!")
    elif novelty.getLevel() > 0.7:
        print(f"\nHigh novelty detected - encouraging investigation!")

    return brain

def agent_world_simulation():
    """Demonstrate agent-world interaction with enhanced features."""
    print_subheader("Agent-World Interaction")

    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)

    # Create brain and agent with advanced features
    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 400,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'neuromod.dopamine.scale': 1.2
    })

    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)

    print(f"World setup:")
    print(f"  - Dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"  - Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  - Agent start: (10.0, 10.0)")

    # Initialize agent with world
    agent.initialize(world)

    # Run episode with enhanced simulation controller
    sim = pynlm.createSimulationController(brain, world, agent)

    print(f"\nRunning enhanced simulation (80 steps)...")
    start_time = time.time()
    sim.run_curiosity_driven(80.0, curiosity_threshold=0.3)
    end_time = time.time()

    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Final agent status:")
    print(f"  - Developmental stage: {agent.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  - Prediction error: {agent.getPredictionError():.3f}")

    return brain

def development_demonstration():
    """Demonstrate developmental dynamics."""
    print_subheader("Development Dynamics")

    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 500,
        'development.stage': pynlm.DevelopmentalStage.CriticalPeriod
    })

    brain = pynlm.createBrain(config)
    brain.initialize()

    print(f"Development simulation:")
    print(f"  - Initial developmental stage: {brain.getDevelopmentalStage()}")

    # Run development simulation
    for stage in [pynlm.DevelopmentalStage.CriticalPeriod,
                  pynlm.DevelopmentalStage.Maturation,
                  pynlm.DevelopmentalStage.Adult]:
        brain.setDevelopmentalStage(stage)
        print(f"\nDevelopmental stage: {stage}")

        # Run some simulation steps at each stage
        for step in range(20):
            brain.step(step)
            time.sleep(0.01)  # Small delay for demonstration

        # Check development system
        dev_system = brain.getDevelopmentSystem()
        print(f"  - Stage effects on plasticity")

    print(f"\nDevelopment simulation complete!")
    return brain

def data_export_demonstration():
    """Demonstrate data export capabilities."""
    print_subheader("Data Export Demonstration")

    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 100,
        'simulation_timestep': 0.01
    })

    brain = pynlm.createBrain(config)
    brain.initialize()

    print(f"Running simulation for data export...")
    for step in range(50):
        brain.step(step)

    # Export neural data
    print(f"\nExporting neural activity data...")
    pynlm.export_neural_data(brain, "demo_neural_activity.csv", window=50)
    print(f"  - Exported to demo_neural_activity.csv")

    # Export synaptic connections
    pynlm.export_synapse_weights(brain, "demo_synaptic_connections.csv")
    print_f"  - Exported to demo_synaptic_connections.csv")

    # Visualize firing rates
    pynlm.plot_firing_rates(brain, "demo_firing_rates.png")
    print(f"  - Created demo_firing_rates.png")

    print(f"\nData export demonstration complete!")
    return brain

def main():
    """Run all demonstration examples."""
    print_header("Enhanced NLM Python API - Basic Demonstration")
    print(f"\nThis script demonstrates the enhanced NLM Python API capabilities.")
    print(f"Version: 2.0.0")
    print(f"Enhanced features include memory systems, neuromodulation, development, and more.")

    try:
        # Run all demonstrations
        brain1 = basic_brain_simulation()
        brain2 = memory_system_demonstration()
        brain3 = neuromodulation_demonstration()
        brain4 = agent_world_simulation()
        brain5 = development_demonstration()
        brain6 = data_export_demonstration()

        print_header("Demonstration Complete")
        print(f"\nAll enhanced NLM Python API demonstrations completed successfully!")
        print(f"\nKey features demonstrated:")
        print(f"  1. Basic brain simulation with advanced configuration")
        print(f"  2. Working memory and episodic memory systems")
        print(f"  3. Neuromodulation (Dopamine, Curiosity, Novelty)")
        print(f"  4. Agent-world interaction with curiosity-driven behavior")
        print(f"  5. Developmental dynamics across life stages")
        print(f"  6. Data export and visualization capabilities")

        print(f"\nFor more information and examples, see:")
        print(f"  - enhanced_python_api_documentation.md")
        print_f"  - IMPLEMENTATION_SUMMARY.md")
        print_f"  - NLM_PYTHON_IMPROVEMENT_PLAN.md")

    except Exception as e:
        print(f"\nError during demonstration: {e}")
        print_f"Please check the documentation for troubleshooting information.")
        return 1

    return 0

if __name__ == "__main__":
    exit(main())