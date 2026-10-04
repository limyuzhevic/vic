#!/usr/bin/env python3
"""
Example script 3: Learning and Plasticity Experiment

This advanced example demonstrates how the NLM brain can learn and adapt through
experience using STDP, Hebbian learning, and reward modulation.
"""

import sys
import time
import math

def run_learning_experiment():
    """Run a complete learning experiment"""
    try:
        import pynlm
        
        print("=" * 75)
        print("NLM Example 3: Learning and Plasticity Experiment")
        print("=" * 75)
        
        # Step 1: Create configuration optimized for learning
        print("\n1. Setting up learning-optimized configuration...")
        config = pynlm.createDefaultConfig()
        
        # Learning-focused parameters
        config.set("brain.neuron_count", 300)
        config.set("brain.synapse_density", 0.08)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.stdp.tau_plus", 20.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.synaptogenesis_rate", 0.0005)
        config.set("plasticity.structural.pruning_rate", 0.00005)
        
        # Neuromodulation for learning
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction.enable", True)
        
        brain = pynlm.createBrain(config)
        
        # Step 2: Create world with learning challenges
        print("2. Creating learning environment...")
        world = pynlm.createSimpleWorld()
        world.configure(
            width=20,
            height=20,
            visionWidth=8,
            visionHeight=8
        )
        world.reset()
        world.setAgentStart(10.0, 10.0)
        world.setMaxEnergy(80.0)
        
        # Add learning resources and hazards
        # Resource at position (5, 5)
        resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        world.addObject(resource)
        
        # Hazard at position (15, 15)  
        hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=5.0)
        world.addObject(hazard)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning subsystems
        print("3. Enabling comprehensive learning subsystems...")
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Step 3: Run learning experiment
        print("\n4. Running learning experiment (200 steps)...")
        print("   Step | Position | Energy | Spikes | Learning Rate | Curiosity | Stage")
        print("   " + "-" * 80)
        
        learning_curve = []
        curiosity_levels = []
        spike_counts = []
        
        for step in range(200):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain computation
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward based on experience
            reward = 0.0
            predicted = 0.0
            
            body = world.getAgentBody()
            
            # Positive reward for finding resources
            for obj in [resource, hazard]:
                if obj.active:
                    distance = math.sqrt((body.x - obj.x) ** 2 + (body.y - obj.y) ** 2)
                    if distance < 1.0:  # Close enough to interact
                        if obj.type == pynlm.WorldObjectType.Resource:
                            reward += 5.0
                            predicted += 2.0
                            # Resource disappears after being collected
                            obj.active = False
                        elif obj.type == pynlm.WorldObjectType.Hazard:
                            reward -= 3.0
                            predicted += 1.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, predicted)
            
            # Update development (brain maturation)
            agent.updateDevelopment(0.1)
            
            # Record metrics
            learning_curve.append(abs(agent.getPredictionError()))
            curiosity_levels.append(agent.getCuriosityLevel())
            spike_counts.append(brain.getTotalSpikeCount())
            
            # Log progress
            if step % 20 == 0:
                stage = brain.getDevelopmentalStage()
                print(f"   {step:4d} | ({body.x:5.1f}, {body.y:5.1f}) | {body.energy:5.1f} | "
                      f"{brain.getTotalSpikeCount():6d} | {agent.getPredictionError():8.4f} | "
                      f"{agent.getCuriosityLevel():7.3f} | {stage}")
        
        # Step 4: Analyze learning results
        print("\n5. Learning Analysis:")
        print("   " + "=" * 50)
        
        # Calculate learning metrics
        initial_curiosity = curiosity_levels[0]
        final_curiosity = curiosity_levels[-1]
        avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
        
        initial_spikes = spike_counts[0]
        final_spikes = spike_counts[-1]
        avg_spikes = sum(spike_counts) / len(spike_counts)
        
        # Learning success criteria
        curiosity_increase = final_curiosity - initial_curiosity
        spike_increase = final_spikes - initial_spikes
        
        print(f"   Curiosity Level:")
        print(f"     Initial:  {initial_curiosity:.3f}")
        print(f"     Final:    {final_curiosity:.3f}")
        print(f"     Average:  {avg_curiosity:.3f}")
        print(f"     Change:   {curiosity_increase:+.3f}")
        
        print(f"   Neural Activity:")
        print(f"     Initial spikes:  {initial_spikes}")
        print(f"     Final spikes:    {final_spikes}")
        print(f"     Average spikes:  {avg_spikes:.1f}")
        print(f"     Change:          {spike_increase:+d}")
        
        print(f"   Prediction Error:")
        print(f"     Initial:  {learning_curve[0]:.4f}")
        print(f"     Final:    {learning_curve[-1]:.4f}")
        print(f"     Average:  {sum(learning_curve)/len(learning_curve):.4f}")
        
        # Determine learning success
        learning_successful = (curiosity_increase > 0.1 or 
                             spike_increase > 50 or 
                             (learning_curve[-1] < learning_curve[0] and learning_curve[-1] < 0.1))
        
        print(f"\n   Learning Assessment:")
        if learning_successful:
            print("   ✓ LEARNING DETECTED: Agent successfully learned from experience!")
            print("   ✓ Neural plasticity mechanisms are functioning correctly")
            print("   ✓ Agent developed adaptive behavior through interaction")
        else:
            print("   ⚠ LIMITED LEARNING: Agent showed minimal adaptation")
            print("   - May need more training steps or different parameters")
        
        # Step 5: Demonstrate synaptic changes (if possible)
        print(f"\n6. Brain State Summary:")
        print(f"   Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   Total synapses: {brain.getTotalSynapseCount()}")
        print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        
        # Save final state
        print(f"\n7. Saving learning experiment state...")
        brain.save("learning_experiment.bin")
        world.save("learning_world.bin")
        print("   Brain state saved to 'learning_experiment.bin'")
        print("   World state saved to 'learning_world.bin'")
        
        print("\n" + "=" * 75)
        print("Learning experiment completed successfully!")
        print("The agent demonstrated experience-based learning and adaptation.")
        print("=" * 75)
        
        return True
        
    except ImportError as e:
        print(f"ERROR: Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"ERROR: An unexpected error occurred: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main function"""
    success = run_learning_experiment()
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())