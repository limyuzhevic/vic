#!/usr/bin/env python3
"""
Advanced Agent Simulation Example
================================

This example demonstrates comprehensive agent-world integration using all
newly added NLM Python API features. It showcases advanced neuromodulation,
custom curiosity and exploration methods, development stage management,
performance monitoring, and checkpointing.

Key Features Demonstrated:
- Advanced agent configuration with validation
- Full agent-world integration with all new features
- Advanced neuromodulation strategies
- Custom curiosity and exploration methods
- Development stage management
- Performance monitoring and statistics
- Checkpointing and state management

File: examples/advanced_agent_simulation.py
"""

import time
import sys
import os
from datetime import datetime

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: pynlm module not available: {e}")
    print("Please build the Python bindings first")
    sys.exit(1)

def log_advanced_stats(agent, brain, step, world):
    """
    Log comprehensive statistics from the advanced simulation.

    Args:
        agent: AgentBrain instance
        brain: Brain instance
        step: Current simulation step
        world: SimpleWorld instance

    Returns:
        dict: Statistics dictionary
    """
    try:
        # Collect statistics with error handling
        stats = {
            "step": step,
            "timestamp": world.getSimulationTime(),
            "neurons_firing": brain.getFiringNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "neuromodulation_level": agent.getNeuromodulationLevel(),
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "developmental_stage": agent.getDevelopmentalStage().name,
            "agent_energy": world.getAgentBody().energy,
            "agent_health": world.getAgentBody().health,
            "simulation_time": world.getSimulationTime()
        }

        print(f"[STEP {step:04d}] "
              f"Firing: {stats['neurons_firing']}/{stats['total_neurons']}, "
              f"Neuromod: {stats['neuromodulation_level']:.3f}, "
              f"Curiosity: {stats['curiosity_level']:.3f}, "
              f"Novelty: {stats['novelty_level']:.3f}, "
              f"DevStage: {stats['developmental_stage']}, "
              f"Energy: {stats['agent_energy']:.1f}")

        return stats
    except Exception as e:
        print(f"[STEP {step:04d}] Error collecting statistics: {e}")
        return {
            "step": step,
            "error": str(e),
            "timestamp": time.time()
        }
def run_advanced_simulation(num_steps=1000, checkpoint_interval=100):
    """
    Run an advanced agent simulation with comprehensive features.

    Args:
        num_steps: Number of simulation steps to run
        checkpoint_interval: How often to log statistics
    """
    print("=== Advanced Agent Simulation ===")
    print(f"Starting at {datetime.now()}")
    print(f"Total steps: {num_steps}")
    print()

    # 1. Advanced agent configuration with validation
    print("1. Creating and configuring agent...")
    config = nlm.createDefaultConfig()
    
    # Set advanced configuration parameters
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.stdp.time_constant", 20.0)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.serotonin.modulation", 0.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.prediction_error_weight", 0.8)
    config.set("development.enable", True)
    config.set("development.critical_period_start", 100.0)
    config.set("development.critical_period_end", 500.0)
    
    print(f"   Configuration: {len(config.getKeys())} parameters set")
    print(f"   Neuron count: {config.get('brain.neuron_count')}")
    print(f"   Synapse probability: {config.get('brain.connection_probability')}")

    # 2. Create brain with validation
    print("\n2. Creating and initializing brain...")
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"   Ready to process sensory input and produce actions")

    # 3. Create advanced agent with all features
    print("\n3. Creating advanced agent with all features...")
    agent = nlm.createAgentBrain(brain)
    
    # Initialize with a simple world
    world = nlm.createSimpleWorld()
    world.configure(200.0, 200.0, 32, 32)  # width, height, vision_width, vision_height
    world.reset()
    
    agent.initialize(world)
    
    # Enable all advanced features
    print("   Enabling all advanced features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Set advanced exploration mode and curiosity strategy
    print("   Configuring exploration and curiosity...")
    agent.setExplorationMode("epsilon_greedy")
    agent.setCuriosityStrategy("prediction_error")
    
    # Configure world physics and weather for realism
    print("\n4. Configuring world environment...")
    physics_params = {
        "gravity": 9.8,
        "friction": 0.3,
        "elasticity": 0.8,
        "damping": 0.95,
        "maxSpeed": 5.0,
        "acceleration": 2.0
    }
    world.setPhysicsParameters(physics_params)
    world.enableWeatherSimulation(True)
    world.setWeatherParameters(3.0, 0.1)  # windSpeed, precipitation

    # Add some resources and hazards to the world
    print("   Adding environmental objects...")
    from random import uniform
    for i in range(20):
        x, y = uniform(10.0, 190.0), uniform(10.0, 190.0)
        if world.isValidPosition(x, y):
            # Add resource (50% chance) or hazard (25% chance)
            if uniform(0, 1) < 0.5:
                obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, 1.0, 0.3)
                world.addObject(obj)
            elif uniform(0, 1) < 0.5:
                obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, 2.0, 0.5)
                world.addObject(obj)

    # 5. Run advanced simulation with monitoring
    print(f"\n5. Starting simulation ({num_steps} steps)...")
    start_time = time.time()
    
    for step in range(num_steps):
        # World update with physics integration
        timestep = 0.1
        world.update(timestep)
        
        # Advanced sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Advanced brain processing with timing
        brain.step(step, world.getSimulationTime())
        
        # Advanced action selection with exploration
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Advanced reward modulation with prediction
        reward = action_result.reward
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Advanced development update
        agent.updateDevelopment(timestep)
        
        # Log comprehensive statistics at checkpoints
        if step % checkpoint_interval == 0:
            log_advanced_stats(agent, brain, step, world)
            
            # Save checkpoint state for potential recovery
            if step % (checkpoint_interval * 10) == 0:
                checkpoint_file = f"agent_checkpoint_step_{step}.nlm"
                try:
                    brain.save(checkpoint_file)
                    print(f"   Checkpoint saved: {checkpoint_file}")
                except Exception as e:
                    print(f"   Warning: Failed to save checkpoint: {e}")
    
    # 6. Final statistics and summary
    end_time = time.time()
    final_stats = log_advanced_stats(agent, brain, num_steps, world)
    
    print(f"\n=== Simulation Complete ===")
    print(f"Duration: {end_time - start_time:.2f} seconds")
    print(f"Average time per step: {(end_time - start_time) / num_steps * 1000:.2f} ms")
    print(f"Final state:")
    print(f"  - Neuromodulation level: {final_stats['neuromodulation_level']:.3f}")
    print(f"  - Curiosity level: {final_stats['curiosity_level']:.3f}")
    print(f"  - Novelty level: {final_stats['novelty_level']:.3f}")
    print(f"  - Developmental stage: {final_stats['developmental_stage']}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Environment objects: {final_stats['object_count']}")
    
    # Clean up checkpoints
    import glob
    checkpoint_files = glob.glob("agent_checkpoint_*.nlm")
    if checkpoint_files:
        for f in checkpoint_files:
            try:
                os.remove(f)
                print(f"  - Cleaned up checkpoint: {f}")
            except:
                pass

if __name__ == "__main__":
    # Run the advanced simulation
    run_advanced_simulation(num_steps=1000, checkpoint_interval=100)
