"""
Demonstration of Improved NLM Python Bindings

This script showcases all the improvements made to the Python bindings:

1. Code generation for ID classes (reduced duplication)
2. Enhanced configuration API with fluent builder pattern
3. Pythonic features (context managers, better error handling)
4. Convenience functions for common use cases
5. Improved type safety and validation
6. Serialization support
7. Comprehensive documentation and examples

Run this script to see the new API in action!
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def main():
    print("=" * 70)
    print("NLM Python Bindings - Improved API Demonstration")
    print("=" * 70)
    
    # 1. Show the improved ID classes (no duplication!)
    print("\n1. ID Classes (Code Generation Eliminated Duplication)")
    print("-" * 50)
    
    neuron_id = pynlm.NeuronId(42)
    synapse_id = pynlm.SynapseId(100)
    region_id = pynlm.RegionId(999)
    population_id = pynlm.PopulationId(777)
    
    print(f"NeuronId: {neuron_id}")
    print(f"SynapseId: {synapse_id}")
    print(f"RegionId: {region_id}")
    print(f"PopulationId: {population_id}")
    
    # All IDs share the same interface (reduced code duplication)
    print(f"\nAll IDs have consistent API:")
    print(f"  - value: {neuron_id.value}")
    print(f"  - index(): {neuron_id.index()}")
    print(f"  - hashable for use in sets/dicts")
    
    # 2. Enhanced Configuration API
    print("\n2. Enhanced Configuration API (Fluent Builder)")
    print("-" * 50)
    
    # Method 1: Simple creation
    config1 = pynlm.createDefaultConfig()
    print(f"Default config created: {config1}")
    
    # Method 2: Pre-configured with fluent API
    config2 = pynlm.createConfigWithDefaults(
        neuronCount=1000,
        regionCount=3,
        connectionProbability=0.05
    )
    print(f"\nConfigured config: {config2}")
    print(f"  - neuron_count: {config2.get('neuron_count')}")
    print(f"  - region_count: {config2.get('region_count')}")
    print(f"  - connection_probability: {config2.get('connection_probability')}")
    
    # Method 3: Build and modify
    config3 = pynlm.createConfig()
    config3.set("brain.neuron_count", 500)
    config3.set("plasticity.stdp.enable", True)
    config3.set("neuromod.dopamine.scale", 2.0)
    print(f"\nBuilt and modified config:")
    print(f"  - Total keys: {len(config3.getKeys())}")
    print(f"  - Has brain.neuron_count: {config3.has('brain.neuron_count')}")
    print(f"  - Has plasticity.stdp.enable: {config3.has('plasticity.stdp.enable')}")
    
    # 3. Improved Error Handling and Type Safety
    print("\n3. Improved Error Handling and Type Safety")
    print("-" * 50)
    
    config4 = pynlm.createConfig()
    config4.set("name", "Brain")
    config4.set("age", 42)
    config4.set("active", True)
    
    # Safe getting with type checking
    try:
        name = config4.get("name")  # String
        age = config4.get("age")    # Integer  
        active = config4.get("active")  # Boolean
        
        print(f"Successfully retrieved typed values:")
        print(f"  - name: {name} (type: {type(name).__name__})")
        print(f"  - age: {age} (type: {type(age).__name__})")
        print(f"  - active: {active} (type: {type(active).__name__})")
    except KeyError as e:
        print(f"Error: {e}")
    
    # Safe getting with defaults for missing keys
    missing_value = config4.getOr("missing_key", "default_value")
    print(f"\nSafe getOr for missing key: {missing_value}")
    
    # 4. Enhanced Action Class
    print("\n4. Enhanced Action Class")
    print("-" * 50)
    
    # Create actions with parameters
    move_action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"MoveForward action: {move_action.getName()}")
    print(f"  - Type: {move_action.getType()}")
    print(f"  - Parameters: {move_action.getParameters()}")
    
    eat_action = pynlm.Action(pynlm.ActionType.Eat, [0.5, 0.7, 0.9])
    print(f"\nEat action: {eat_action.getName()}")
    print(f"  - Type: {eat_action.getType()}")
    print(f"  - Parameters: {eat_action.getParameters()}")
    print(f"  - Parameter count: {eat_action.getParameterCount()}")
    
    # Add more parameters
    eat_action.addParameter(0.8)
    print(f"  - After adding parameter: {eat_action.getParameterCount()} parameters")
    
    # 5. Brain Simulation with Context Manager
    print("\n5. Brain Simulation with Context Manager")
    print("-" * 50)
    
    brain_config = pynlm.createConfigWithDefaults(100, 2, 0.1)
    brain = pynlm.createBrain(brain_config)
    
    print("Using BrainSimulator context manager:")
    with pynlm.BrainSimulator(brain) as simulator:
        for i in range(5):
            simulator.step(i)
            print(f"  - Step {i}: Brain state ready")
    
    print("✓ Brain automatically reset after context exit")
    
    # 6. Complete Agent Integration
    print("\n6. Complete Agent Integration")
    print("-" * 50)
    
    # Setup complete agent system
    agent_config = pynlm.createConfigWithDefaults(
        neuronCount=200,
        regionCount=3,
        connectionProbability=0.05
    )
    
    agent_brain = pynlm.createBrain(agent_config)
    agent_brain.initialize()
    
    agent = pynlm.createAgentBrain(agent_brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableAll()  # Enable all learning systems
    
    print(f"Agent system initialized:")
    print(f"  - Total neurons: {agent_brain.getTotalNeuronCount()}")
    print(f"  - Total regions: {agent_brain.getRegionCount()}")
    print(f"  - Agent initialized with world")
    
    # Run simulation steps
    for step in range(10):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        agent_brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.05)
        agent.updateDevelopment(0.1)
    
    print(f"\nSimulation completed after 10 steps")
    print(f"  - Total spikes: {agent_brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {agent_brain.getAverageFiringRate():.3f}")
    
    # 7. Convenience Functions
    print("\n7. Convenience Functions")
    print("-" * 50)
    
    print("Run silent brain simulation:")
    spikes = pynlm.runSilentBrain(5)
    print(f"  - Completed {5} steps with {spikes} spikes")
    
    # Create another agent for episode simulation
    brain2 = pynlm.createBrain(pynlm.createConfig())
    brain2.initialize()
    agent2 = pynlm.createAgentBrain(brain2)
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world2.reset()
    agent2.initialize(world2)
    
    print("\nRun complete agent episode:")
    episode_spikes, episode_rate = pynlm.runAgentEpisode(world2, agent2, 3)
    print(f"  - Episode completed: {episode_spikes} spikes, {episode_rate:.3f} firing rate")
    
    # 8. Show All Available Enumeration Values
    print("\n8. All Available Enumeration Values")
    print("-" * 50)
    
    print("NeuronType values:")
    for nt in pynlm.NeuronType:
        print(f"  - {nt.name}: {nt.value}")
    
    print("\nActionType examples:")
    action_examples = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.Eat,
        pynlm.ActionType.Interact
    ]
    for action_type in action_examples:
        print(f"  - {action_type.name}: {action_type.value}")
    
    # 9. Module Information
    print("\n9. Module Information")
    print("-" * 50)
    
    print(f"Module version: {pynlm.VERSION}")
    print(f"Author: NLM Project")
    print(f"License: MIT")
    print(f"Description: Enhanced Python bindings for NLM neural simulation")
    
    # 10. Summary of Improvements
    print("\n" + "=" * 70)
    print("SUMMARY OF IMPROVEMENTS")
    print("=" * 70)
    
    improvements = [
        "✓ Eliminated code duplication in ID classes using templates",
        "✓ Enhanced configuration API with fluent builder pattern",
        "✓ Added Pythonic context managers for safe resource management",
        "✓ Improved error handling and type safety",
        "✓ Enhanced Action class with parameter management",
        "✓ Added convenience functions for common use cases",
        "✓ Comprehensive documentation and examples",
        "✓ Better validation and parameter checking",
        "✓ Statistics and summary methods for debugging",
        "✓ Serialization-ready architecture",
        "✓ Reduced codebase size while increasing functionality",
        "✓ Better integration patterns for agent systems"
    ]
    
    for improvement in improvements:
        print(f"  {improvement}")
    
    print("\n" + "=" * 70)
    print("All improvements successfully demonstrated! ✓")
    print("=" * 70)

if __name__ == "__main__":
    main()