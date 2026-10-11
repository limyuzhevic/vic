"""
NLM Python Bindings - User Guide

This is a comprehensive user guide for the NLM Python bindings.
It covers basic usage, advanced features, and provides examples.
"""

import pynlm as nlm

def main():
    print("=" * 60)
    print("NLM Python Bindings - User Guide")
    print("=" * 60)
    print()
    
    print("## Quick Start")
    print()
    print("The NLM Python bindings provide easy access to the NLM neural simulation")
    print("framework through a Pythonic interface. Here's how to get started:")
    print()
    
    # Basic example
    print("### 1. Basic Brain Usage")
    print()
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    print(f"Created and initialized brain with {brain.total_neurons} neurons")
    print()
    
    for i in range(5):
        brain.step(i)
        print(f"  Step {i}: {brain.firing_neurons} firing neurons")
    print()
    
    print("### 2. Complete Agent Simulation")
    print()
    
    # Create complete agent simulation
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.firing_neurons} firing neurons, action: {action.type}")
    
    print()
    print("Simulation completed!")
    print()
    
    print("## Advanced Features")
    print()
    
    print("### 3. Context Managers")
    print()
    print("Use context managers for automatic resource management:")
    print()
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    with nlm.brain_simulation(brain):
        print("  Brain simulation context entered")
        print(f"  Brain initialized with {brain.total_neurons} neurons")
        brain.step(0)
    print("  Brain simulation context exited")
    print()
    
    print("### 4. Configuration Management")
    print()
    
    config = nlm.createDefaultConfig()
    config["neuron_count"] = 3000
    config["synapse_density"] = 0.2
    config["enable_structural_plasticity"] = True
    
    print("Custom configuration:")
    print(f"  Neurons: {config.get('neuron_count')}")
    print(f"  Synapse density: {config.get('synapse_density')}")
    print(f"  Structural plasticity: {config.get('enable_structural_plasticity')}")
    print()
    
    print("### 5. Performance Monitoring")
    print()
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Run some steps
    for i in range(100):
        brain.step(i)
    
    brain_stats = nlm.get_brain_stats(brain)
    print("Brain performance statistics:")
    for key, value in brain_stats.items():
        print(f"  {key}: {value}")
    print()
    
    print("### 6. Data Analysis")
    print()
    
    # Create brain and run simulation
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(50):
        brain.step(i)
    
    # Analyze data
    active_neurons = nlm.get_active_neuron_ids(brain)
    firing_neurons = nlm.get_firing_neuron_ids(brain)
    region_names = nlm.get_region_names(brain)
    
    print("Data analysis results:")
    print(f"  Total neurons: {brain.total_neurons}")
    print(f"  Active neurons: {len(active_neurons)}")
    print(f"  Firing neurons: {len(firing_neurons)}")
    print(f"  Regions: {region_names}")
    print()
    
    print("### 7. Batch Operations")
    print()
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    print("Running batch operations...")
    spike_count = nlm.batch_steps(brain, 1000)
    print(f"  After 1000 steps: {spike_count} total spikes")
    
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    final_time = nlm.batch_world_updates(world, 500, 0.02)
    print(f"  World simulation time: {final_time:.2f}")
    print()
    
    print("### 8. Serialization")
    print()
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    print("Brain serialized to file 'trained_brain.bin'")
    print("Brain can be deserialized and used later")
    print()
    
    print("## API Reference")
    print()
    
    print("### Core Classes")
    print()
    print("#### Brain")
    print("- `createBrain(config)`: Create a new brain")
    print("- `initialize()`: Initialize the brain")
    print("- `step(step)`: Perform a simulation step")
    print("- `receiveSensoryInput(input)`: Inject sensory input")
    print("- `produceAction()`: Get motor action")
    print("- `reset()`: Reset brain state")
    print("- `save(filepath)`: Save brain state")
    print("- `load(filepath)`: Load brain state")
    print()
    
    print("#### Config")
    print("- `createDefaultConfig()`: Create default configuration")
    print("- `loadFromFile(filepath)`: Load config from file")
    print("- `saveToFile(filepath)`: Save config to file")
    print("- `has(key)`: Check if key exists")
    print("- `set(key, value)`: Set configuration value")
    print("- `get(key)`: Get configuration value")
    print()
    
    print("#### AgentBrain")
    print("- `createAgentBrain(brain)`: Create agent interface")
    print("- `initialize(world)`: Initialize with world")
    print("- `processSensoryInput(percept)`: Process sensory input")
    print("- `decodeMotorCommand()`: Get motor command")
    print("- `applyRewardModulation(reward, predicted)`: Apply reward modulation")
    print()
    
    print("#### SimpleWorld")
    print("- `createSimpleWorld()`: Create simple world")
    print("- `configure(width, height, visionWidth, visionHeight)`: Configure world")
    print("- `update(timestep)`: Update world")
    print("- `reset()`: Reset world")
    print("- `getSensoryPercept()`: Get sensory percept")
    print("- `applyMotorCommand(action, time)`: Apply motor command")
    print()
    
    print("### Enums")
    print()
    print("#### NeuronType")
    print("- `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`")
    print()
    
    print("#### ActionType")
    print("- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`")
    print("- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`")
    print("- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`")
    print()
    
    print("#### DevelopmentalStage")
    print("- `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`")
    print()
    
    print("### Constants")
    print()
    print("#### Invalid IDs")
    print("- `INVALID_NEURON_ID`")
    print("- `INVALID_SYNAPSE_ID`")
    print("- `INVALID_REGION_ID`")
    print("- `INVALID_POPULATION_ID`")
    print()
    
    print("### Utility Functions")
    print()
    print("#### Factory Functions")
    print("- `createDefaultConfig()`: Create default configuration")
    print("- `createBrain(config)`: Create a new brain")
    print("- `createSimpleWorld()`: Create a new world")
    print("- `createAgentBrain(brain)`: Create agent interface")
    print()
    
    print("#### Advanced Functions")
    print("- `brain_simulation(brain)`: Create brain context manager")
    print("- `agent_context(agent)`: Create agent context manager")
    print("- `world_context(world)`: Create world context manager")
    print("- `run_agent_episode(agent, world, steps, options)`: Run agent episode")
    print("- `batch_steps(brain, steps)`: Run batch brain steps")
    print("- `batch_world_updates(world, updates, timestep)`: Run batch world updates")
    print("- `debug_brain_state(brain)`: Get debug information")
    print("- `get_brain_stats(brain)`: Get brain statistics")
    print("- `get_world_stats(world)`: Get world statistics")
    print("- `get_agent_stats(agent)`: Get agent statistics")
    print()
    
    print("## Examples")
    print()
    print("Run the advanced examples:")
    print()
    print("  python examples/advanced_examples.py")
    print()
    print("For more examples, see the examples/ directory.")
    print()
    
    print("## Troubleshooting")
    print()
    print("### Common Issues")
    print()
    print("#### 'My brain isn't doing anything'")
    print("Solution: Make sure you call brain.initialize() before using the brain.")
    print()
    
    print("#### 'The agent isn't moving'")
    print("Solution: Make sure you call world.applyMotorCommand() after getting actions.")
    print()
    
    print("#### 'Everything is 0'")
    print("Solution: Brains need time to 'warm up' - try more steps.")
    print("Also make sure sensory input is connected.")
    print()
    
    print("#### 'ImportError: No module named pynlm'")
    print("Solution: Install NLM Python bindings:")
    print("  pip install pynlm")
    print("Or build from source:")
    print("  python setup.py")
    print()
    
    print("## Further Reading")
    print()
    print("For more information, refer to:")
    print("  - HOW_TO_USE.md: Comprehensive usage guide")
    print("  - easy_usage.md: Beginner-friendly guide")
    print("  - docs/ARCHITECTURE.md: System architecture")
    print("  - docs/SCIENCE.md: Scientific background")
    print("  - examples/: Additional example scripts")
    print()
    
    print("=" * 60)
    print("End of User Guide")
    print("=" * 60)

if __name__ == "__main__":
    main()