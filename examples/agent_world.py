#!/usr/bin/env python3
"""
Example script 2: Agent in a World

This example demonstrates how to create an agent that interacts with a simulated
world, processes sensory input, and makes decisions based on neural computation.
"""

import sys
import time

def main():
    """Run agent-world simulation example"""
    try:
        import pynlm
        
        print("=" * 70)
        print("NLM Example 2: Agent in a World Simulation")
        print("=" * 70)
        
        # Step 1: Create brain and configure it
        print("\n1. Setting up the brain...")
        config = pynlm.createDefaultConfig()
        
        # Configure for better agent performance
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        
        # Step 2: Create and initialize world
        print("2. Creating and configuring the world...")
        world = pynlm.createSimpleWorld()
        world.configure(
            width=15,
            height=15,
            visionWidth=6,
            visionHeight=6
        )
        world.reset()
        world.setAgentStart(7.5, 7.5)
        world.setMaxEnergy(100.0)
        world.setEnergyDecayRate(0.001)
        
        # Step 3: Create agent interface
        print("3. Creating agent brain interface...")
        agent = pynlm.createAgentBrain(brain)
        
        # Step 4: Initialize agent with world
        print("4. Initializing agent with world...")
        agent.initialize(world)
        
        # Step 5: Enable learning subsystems
        print("5. Enabling learning subsystems...")
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("   - Reward modulation: ENABLED")
        print("   - Structural plasticity: ENABLED")
        print("   - Development system: ENABLED")
        print("   - Curiosity-driven exploration: ENABLED")
        
        # Step 6: Run simulation
        print("\n6. Running agent simulation (100 steps)...")
        print("   Step | Position | Energy | Firing | Curiosity | Novelty | Stage")
        print("   " + "-" * 65)
        
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain computation
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation based on agent's internal state
            reward = 0.0
            if world.getSensoryPercept().getInternal():
                internal_signals = world.getSensoryPercept().getInternal()
                if not internal_signals.empty():
                    reward = internal_signals[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress every 10 steps
            if step % 10 == 0:
                body = world.getAgentBody()
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                stage = brain.getDevelopmentalStage()
                
                print(f"   {step:4d} | ({body.x:4.1f}, {body.y:4.1f}) | {body.energy:5.1f} | "
                      f"{brain.getFiringNeuronCount():6d} | {curiosity:7.3f} | {novelty:6.3f} | {stage}")
        
        # Step 7: Display final results
        print("\n7. Final agent performance:")
        body = world.getAgentBody()
        brain_stats = {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'spikes': brain.getTotalSpikeCount(),
            'firing': brain.getFiringNeuronCount(),
            'avg_rate': brain.getAverageFiringRate(),
            'ei_ratio': brain.getExcitationInhibitionRatio(),
            'stage': brain.getDevelopmentalStage(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromod': agent.getNeuromodulationLevel()
        }
        
        print(f"   Final position: ({body.x:.1f}, {body.y:.1f})")
        print(f"   Final energy: {body.energy:.1f}")
        print(f"   Final health: {body.health:.1f}")
        print(f"   Final age: {body.age:.1f}")
        print(f"   Simulation time: {world.getSimulationTime():.1f}s")
        print()
        print("   Brain Statistics:")
        for key, value in brain_stats.items():
            if key == 'stage':
                print(f"     {key:12s}: {value}")
            else:
                print(f"     {key:12s}: {value:.3f}")
        
        # Step 8: Save final state
        print("\n8. Saving final agent state...")
        brain.save("agent_simulation.bin")
        world.save("world_simulation.bin")
        print("   Agent state saved to 'agent_simulation.bin'")
        print("   World state saved to 'world_simulation.bin'")
        
        print("\n" + "=" * 70)
        print("Agent-world simulation completed successfully!")
        print("The agent learned and adapted through experience.")
        print("=" * 70)
        
        return 0
        
    except ImportError as e:
        print(f"ERROR: Failed to import pynlm: {e}")
        print("Please ensure NLM is properly installed with Python bindings.")
        return 1
    except Exception as e:
        print(f"ERROR: An unexpected error occurred: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())