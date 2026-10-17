#!/usr/bin/env python3
"""
Example 4: Sensorimotor Loop with Learning

This example shows a continuous sensorimotor loop where the agent:
1. Perceives (senses) its environment
2. Processes sensory input
3. Makes decisions (actions)
4. Takes actions in the world
5. Receives feedback/rewards
6. Learns from the experience
"""

import sys
import pynlm

def run_complete_agent_demo(config_file=None, num_steps=200):
    """
    Run a complete agent demonstration.
    
    Args:
        config_file: Optional config file path
        num_steps: Number of simulation steps to run
    
    Returns:
        Dictionary with simulation results
    """
    print(f"Running complete agent demo for {num_steps} steps...")
    
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        if config_file:
            config.loadFromFile(config_file)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create and initialize world
        world = pynlm.createSimpleWorld()
        world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
        world.reset()
        
        # Create and initialize agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        results = {
            'steps': [],
            'total_spikes': 0,
            'max_firing_rate': 0,
            'development_stage': 0,
            'neuromodulation_level': 0,
            'curiosity_level': 0
        }
        
        for step in range(num_steps):
            # 1. Update world
            world.update(0.1)
            
            # 2. Get sensory perception
            percept = world.getSensoryPercept()
            
            # 3. Process sensory input
            agent.processSensoryInput(percept)
            
            # 4. Brain thinking
            brain.step(step)
            
            # 5. Get motor command
            action = agent.decodeMotorCommand()
            
            # 6. Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            firing_count = brain.getFiringNeuronCount()
            avg_firing = brain.getAverageFiringRate()
            results['total_spikes'] += brain.getTotalSpikeCount() - results['total_spikes']
            results['max_firing_rate'] = max(results['max_firing_rate'], avg_firing)
            
            # Store step metrics
            results['steps'].append({
                'step': step,
                'firing_neurons': firing_count,
                'avg_firing_rate': avg_firing,
                'action_type': action,
                'neuromodulation': agent.getNeuromodulationLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'development_stage': brain.getDevelopmentalStage()
            })
        
        # Final metrics
        results['development_stage'] = brain.getDevelopmentalStage()
        results['neuromodulation_level'] = agent.getNeuromodulationLevel()
        results['curiosity_level'] = agent.getCuriosityLevel()
        
        return results
    
    except Exception as e:
        print(f"✗ Error in simulation: {e}")
        raise
def print_results(results):
    """Print formatted simulation results."""
    print("\n" + "=" * 60)
    print("SIMULATION RESULTS")
    print("=" * 60)
    
    print(f"Total simulation steps: {len(results['steps'])}")
    print(f"Total spikes generated: {results['total_spikes']}")
    print(f"Maximum firing rate: {results['max_firing_rate']:.2f} Hz")
    print(f"Final development stage: {results['development_stage']}")
    print(f"Final neuromodulation level: {results['neuromodulation_level']:.2f}")
    print(f"Final curiosity level: {results['curiosity_level']:.2f}")
    
    # Analyze action distribution
    actions = {}
    for step_data in results['steps']:
        action = step_data['action_type']
        actions[action] = actions.get(action, 0) + 1
    
    print("\nAction distribution:")
    for action, count in sorted(actions.items()):
        percentage = (count / len(results['steps'])) * 100
        print(f"  {action}: {count} times ({percentage:.1f}%)")
    
    # Print last few steps
    print(f"\nLast 5 steps:")
    for step_data in results['steps'][-5:]:
        print(f"  Step {step_data['step']}: {step_data['firing_neurons']} "
              f"neurons firing, action: {step_data['action_type']}")
    
    print("=" * 60)
def main():
    print("=" * 50)
    print("Example 4: Sensorimotor Loop with Learning")
    print("=" * 50)
    
    try:
        # Run the simulation
        results = run_complete_agent_demo(num_steps=100)
        
        # Print results
        print_results(results)
        
        print("✓ Complete agent demo finished successfully!")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return 1
    
    print("=" * 50)
    return 0
if __name__ == "__main__":
    sys.exit(main())