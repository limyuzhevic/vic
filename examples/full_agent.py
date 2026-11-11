#!/usr/bin/env python3
"""
Full NLM Agent Example with Learning and Development

This script demonstrates a complete agent simulation with all subsystems:
- Brain with spiking neurons
- World simulation
- Sensory processing
- Motor control
- Neuromodulation (reward, curiosity, novelty)
- Development stages
- Plasticity (learning from experience)
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    import pynlm
    print("NLM Full Agent Example")
    print("=" * 60)
    print()
    
    def run_agent_simulation(num_steps=500, enable_all_features=True):
        """Run a complete agent simulation with all features enabled."""
        
        print(f"Starting agent simulation: {num_steps} steps")
        print(f"Learning features: {'Enabled' if enable_all_features else 'Limited'}")
        print()
        
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.region_count", 2)
        config.set("simulation_timestep", 0.01)
        
        # 2. Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # 3. Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(
            width=20,
            height=20,
            visionWidth=8,
            visionHeight=8,
            maxEnergy=100.0
        )
        world.reset()
        
        # Set agent starting position
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent with world
        agent.initialize(world)
        print("✓ Initialized agent with world")
        
        # 5. Enable all learning subsystems if requested
        if enable_all_features:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            print("✓ Enabled all learning subsystems")
        
        # 6. Main simulation loop
        print()
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain step (spiking neural computation)
            brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation based on world outcome
            reward = 0.0
            # Simple reward system: get resources, avoid hazards
            body = world.getAgentBody()
            vision = percept.getVision()
            
            # Check for resources (bright objects)
            for i, v in enumerate(vision):
                if v > 0.5:  # Resource indicator
                    reward += 10.0
                    break
            
            # Check for hazards (dark objects)
            hazard_detected = False
            for v in vision:
                if v < 0.3:  # Hazard indicator
                    hazard_detected = True
                    reward -= 5.0
                    break
            
            # Apply reward with prediction error
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress every 50 steps
            if step % 50 == 0:
                print(f"Step {step:4d}: "
                      f"Firing {brain.getFiringNeuronCount():3d} neurons, "
                      f"Curiosity {agent.getCuriosityLevel():.3f}, "
                      f"Novelty {agent.getNoveltyLevel():.3f}, "
                      f"Reward {reward:+6.1f}")
        
        # 7. Simulation complete - show results
        print()
        print("=" * 60)
        print("SIMULATION COMPLETE")
        print("=" * 60)
        print()
        
        print("Final Statistics:")
        print(f"  • Total simulation steps: {num_steps}")
        print(f"  • Total spikes generated: {brain.getTotalSpikeCount()}")
        print(f"  • Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  • Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  • E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
        print(f"  • Final developmental stage: {agent.getDevelopmentalStage()}")
        
        if enable_all_features:
            print(f"  • Final curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  • Final novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  • Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  • Learned preference: {agent.getLearnedPreference():.3f}")
        
        # Show agent's thought process if available
        thought_trace = agent.getThoughtTrace()
        if thought_trace != "none":
            print(f"  • Agent thought trace: {thought_trace}")
        
        return brain, agent, world
    
    # Run the main example
    if __name__ == "__main__":
        try:
            # Run basic simulation
            brain, agent, world = run_agent_simulation(200, enable_all_features=True)
            
            print()
            print("Example completed successfully!")
            print("The agent learned from experience through:")
            print("  • Reward-modulated plasticity (dopamine system)")
            print("  • Curiosity-driven exploration")
            print("  • Novelty detection and habituation")
            print("  • Developmental changes over time")
            print("  • Structural plasticity (growing new connections)")
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            import traceback
            traceback.print_exc()
            sys.exit(1)
    
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please make sure pynlm is installed:")
    print("  pip install pynlm")
    sys.exit(1)
except Exception as e:
    print(f"Unexpected error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)