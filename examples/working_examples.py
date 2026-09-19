#!/usr/bin/env python3
"""
NLM Python Examples - Working demonstrations of pynlm usage

This script contains working examples using the actual NLM Python bindings
as they exist in the codebase. It demonstrates the real API as exposed
by pybind11 in bindings.cpp.
"""

import pynlm
import time
import sys

def example_1_basic_brain():
    """Example 1: Basic brain creation and simulation"""
    print("=== Example 1: Basic Brain Creation ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes fired\n")

def example_2_brain_with_config():
    """Example 2: Brain with custom configuration"""
    print("=== Example 2: Custom Configuration ===")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Customize parameters
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.1)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    config.set("brain.v_thresh", -55.0)  # Threshold
    config.set("brain.v_rest", -70.0)   # Resting potential
    config.set("brain.v_reset", -75.0)  # Reset potential
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Custom brain configuration:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Run longer simulation for statistics
    for step in range(100):
        brain.step(step)
    
    print(f"After 100 steps: {brain.getTotalSpikeCount()} spikes\n")

def example_3_complete_agent_simulation():
    """Example 3: Complete agent with world interaction"""
    print("=== Example 3: Complete Agent Simulation ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Agent initialized:")
    print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  - Agent expects sensory input size: {agent.getSensoryInputSize()}")
    print(f"  - Agent expects motor output size: {agent.getMotorOutputSize()}")
    
    # Run complete simulation
    num_steps = 100
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  - Dev Stage: {brain.getDevelopmentalStage()}")
    
    print(f"Simulation complete! Final status:")
    print(f"  - Total reward: {agent.getNeuromodulationLevel():.3f}")
    print(f"  - Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Final development stage: {brain.getDevelopmentalStage()}\n")

def example_4_plasticity_learning():
    """Example 4: Plasticity and learning demonstration"""
    print("=== Example 4: Plasticity and Learning ===")
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for plasticity experiments
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable plasticity
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Plasticity Learning Experiment:")
    print(f"  - Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"  - Reward modulation enabled: True")
    print(f"  - Structural plasticity enabled: True")
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getFiringNeuronCount()
    
    # Run learning simulation
    for step in range(200):
        # Create learning stimuli
        if step % 10 == 0:
            # Inject stimulating input
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
        
        # Update world and get perception
        world.update(timestep=0.1)
        percept = world.getSensoryPercept()
        
        # Process and learn
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on actions
        reward = 1.0 if action.getName() != "Wait" else 0.1
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print learning progress
        if step % 50 == 0:
            current_spikes = brain.getTotalSpikeCount()
            current_neurons = brain.getFiringNeuronCount()
            print(f"Step {step}:")
            print(f"  - Spikes: {current_spikes - initial_spikes} (change)")
            print(f"  - Firing neurons: {current_neurons}")
            print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Final analysis
    final_spikes = brain.getTotalSpikeCount()
    final_neurons = brain.getFiringNeuronCount()
    
    print(f"\nLearning Experiment Results:")
    print(f"  - Initial spikes: {initial_spikes}")
    print(f"  - Final spikes: {final_spikes}")
    print(f"  - Spike increase: {final_spikes - initial_spikes}")
    print(f"  - Learning efficiency: {(final_spikes - initial_spikes)/200:.1f} spikes/step")
    print(f"  - Final neural activity: {final_neurons} firing neurons")
    print(f"  - Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}\n")

def example_5_multiple_simulations():
    """Example 5: Running multiple simulations for comparison"""
    print("=== Example 5: Multiple Simulation Comparison ===")
    
    configurations = [
        {"name": "Small Network", "neurons": 200, "connections": 0.05},
        {"name": "Medium Network", "neurons": 500, "connections": 0.08},
        {"name": "Large Network", "neurons": 1000, "connections": 0.1}
    ]
    
    results = []
    
    for config_info in configurations:
        print(f"\nRunning {config_info['name']}...")
        
        # Create config
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_info['neurons'])
        config.set("brain.connection_probability", config_info['connections'])
        
        # Create and run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        
        # Collect results
        simulation_time = end_time - start_time
        result = {
            'name': config_info['name'],
            'neurons': config_info['neurons'],
            'connections': config_info['connections'],
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'ei_ratio': brain.getExcitationInhibitionRatio(),
            'simulation_time': simulation_time,
            'steps_per_second': 100 / simulation_time
        }
        
        results.append(result)
        
        print(f"  - Completed in {simulation_time:.2f}s")
        print(f"  - Spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Final firing: {brain.getFiringNeuronCount()} neurons")
        print(f"  - Speed: {100/simulation_time:.1f} steps/second")
    
    # Compare results
    print(f"\n=== Comparison Results ===")
    print(f"{'Network':<15} {'Neurons':<8} {'Conns':<6} {'Spikes':<8} {'Firing':<8} {'Rate':<8} {'Speed':<10}")
    print("-" * 75)
    
    for result in results:
        print(f"{result['name']:<15} {result['neurons']:<8} "
              f"{result['connections']:<6.2f} {result['total_spikes']:<8} "
              f"{result['firing_neurons']:<8} {result['avg_firing_rate']:<8.3f} "
              f"{result['steps_per_second']:<10.1f}")
    
    # Find best performer
    best = max(results, key=lambda x: x['total_spikes'] / x['simulation_time'])
    print(f"\nBest performer: {best['name']} ({best['total_spikes']/best['simulation_time']:.1f} spikes/second)")
    
    # Find most efficient
    most_efficient = max(results, key=lambda x: x['steps_per_second'])
    print(f"Most efficient: {most_efficient['name']} ({most_efficient['steps_per_second']:.1f} steps/second)")
    
    print("\n")

def example_6_error_handling():
    """Example 6: Proper error handling"""
    print("=== Example 6: Error Handling ===")
    
    try:
        # Create brain and initialize properly
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Brain created and initialized successfully")
        
        # Try to use brain without initialization (should fail gracefully)
        brain2 = pynlm.createBrain(config)
        # brain2.initialize()  # Intentionally skipped for error handling demo
        
        # This would cause issues, but we handle it
        try:
            brain2.step(0)
            print("ERROR: Should have failed without initialization!")
        except Exception as e:
            print(f"Correctly caught error without initialization: {type(e).__name__}")
        
        # Proper usage
        brain.initialize()
        for step in range(10):
            brain.step(step)
        
        print("Proper initialization worked correctly")
        
    except RuntimeError as e:
        print(f"Runtime error: {e}")
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
    
    print("Error handling example complete\n")

def example_7_api_overview():
    """Example 7: Complete API overview"""
    print("=== Example 7: Complete API Overview ===")
    
    print("NLM Python API Key Components:")
    print("1. Core Classes:")
    print("   - Brain: Neural network simulation")
    print("   - Config: Configuration management")
    print("   - AgentBrain: Brain-world interface")
    print("   - SimpleWorld: Simulation environment")
    print("   - SensoryInput: Sensory data container")
    print("   - Action: Motor commands")
    
    print("\n2. Enumerations:")
    print("   - NeuronType: Excitatory, Inhibitory, Sensory, etc.")
    print("   - SynapseType: Excitatory, Inhibitory, GapJunction")
    print("   - DevelopmentalStage: Initial, CriticalPeriod, Maturation, etc.")
    print("   - ActionType: MoveForward, TurnLeft, Interact, etc.")
    print("   - WorldObjectType: Resource, Hazard, Wall, etc.")
    
    print("\n3. Key Methods (Brain class):")
    print("   - brain.initialize() - Initialize simulation")
    print("   - brain.step(step, time) - Run simulation step")
    print("   - brain.reset() - Reset brain state")
    print("   - brain.getTotalNeuronCount() - Get neuron count")
    print("   - brain.getTotalSynapseCount() - Get synapse count")
    print("   - brain.getFiringNeuronCount() - Get firing neurons")
    print("   - brain.getAverageFiringRate() - Get average firing rate")
    print("   - brain.getExcitationInhibitionRatio() - Get E/I balance")
    print("   - brain.getTotalSpikeCount() - Get total spikes")
    print("   - brain.save(path) - Save state")
    print("   - brain.load(path) - Load state")
    
    print("\n4. Key Methods (AgentBrain class):")
    print("   - agent.initialize(world) - Initialize with world")
    print("   - agent.processSensoryInput(percept) - Process sensory data")
    print("   - agent.decodeMotorCommand() - Get motor command")
    print("   - agent.applyRewardModulation(reward, predicted) - Learn from rewards")
    print("   - agent.updateDevelopment(timestep) - Update development")
    
    print("\n5. Key Methods (SimpleWorld class):")
    print("   - world.configure(width, height, visionW, visionH) - Configure world")
    print("   - world.reset() - Reset world")
    print("   - world.update(timestep) - Update world")
    print("   - world.applyMotorCommand(cmd, time) - Apply action")
    print("   - world.getSensoryPercept() - Get sensory input")
    print("   - world.getAgentBody() - Get agent state")
    
    print("\n6. Configuration Parameters:")
    print("   - brain.neuron_count: Total number of neurons")
    print("   - brain.connection_probability: Synapse connectivity")
    print("   - brain.initial_weight_mean: Initial synaptic weights")
    print("   - brain.v_thresh: Neuron threshold potential")
    print("   - brain.v_rest: Resting potential")
    print("   - brain.v_reset: Reset potential after spike")
    print("   - plasticity.stdp.enable: Enable STDP learning")
    print("   - plasticity.hebbian.enable: Enable Hebbian learning")
    
    print("\nAPI Overview complete!\n")

def main():
    """Run all examples"""
    print("NLM Python Examples - Working Demonstrations")
    print("=" * 60)
    print("These examples demonstrate the actual NLM Python API")
    print("as exposed by the pybind11 bindings.\n")
    
    try:
        example_1_basic_brain()
        example_2_brain_with_config()
        example_3_complete_agent_simulation()
        example_4_plasticity_learning()
        example_5_multiple_simulations()
        example_6_error_handling()
        example_7_api_overview()
        
        print("=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        print("\nKey takeaways:")
        print("1. Always call brain.initialize() before using the brain")
        print("2. Use brain.step() for simulation steps")
        print("3. AgentBrain connects brain to world environment")
        print("4. SimpleWorld provides the simulation environment")
        print("5. Configuration allows customization of network properties")
        print("6. Proper error handling is essential for robust code")
        
    except Exception as e:
        print(f"\nError during execution: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
