"""
NLM Plugin Architecture Examples

This file demonstrates how to use the NLM plugin system to extend
the neural learning machine with custom neural models, learning rules,
and other components.

The plugin system allows users to:
1. Load custom neural dynamics models (e.g., Hodgkin-Huxley, Izhikevich)
2. Add novel plasticity rules (e.g., Oja learning, homeostatic plasticity)
3. Create custom neuromodulation systems
4. Implement specialized connection rules
5. Extend sensory and motor models
6. Develop custom memory systems
7. Add advanced cognitive modules

These examples show the main patterns for plugin development and usage.
"""

from pynlm import (
    # Core NLM classes
    Brain, Config, createBrain, createDefaultConfig,
    AgentBrain, SimpleWorld, createAgentBrain, createSimpleWorld,
    
    # Plugin system
    NLPLuginManager, Plugin, PluginMetadata, PluginType,
    INeuronDynamics, IPlasticityRule, INeuromodulator, 
    IConnectionRule, ISensoryModel, IMotorModel,
    
    # Exceptions
    NLMError, ConfigurationError, SimulationError, ValidationError,
)

print("NLM Plugin Architecture Examples")
print("=" * 50)

# Example 1: Basic Plugin Usage
print("\n1. Basic Plugin Usage Example")
print("-" * 30)

# Create a simple brain
brain = createBrain(createDefaultConfig())
brain.initialize()

# Create agent and world
world = createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = createAgentBrain(brain)
agent.initialize(world)

print("Created basic NLM simulation with agent and world")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Agent enabled curiosity: {agent.isCuriosityEnabled()}")

# Example 2: Plugin Discovery
print("\n2. Plugin Discovery Example")
print("-" * 30)

# Create plugin manager
plugin_manager = NLPLuginManager()

# Discover available plugins (this would scan for .so, .json files, etc.)
available_plugins = plugin_manager.discover_plugins()

if available_plugins:
    print(f"Found {len(available_plugins)} available plugins:")
    for plugin in available_plugins:
        print(f"  - {plugin.name} ({plugin.version}): {plugin.description}")
else:
    print("No plugins found in default directory. Create a 'plugins' folder with plugin files.")

# Example 3: Plugin Loading
print("\n3. Plugin Loading Example")
print("-" * 30)

# Load plugins (in real usage, you would specify actual plugin files)
try:
    # This would load a custom neuron dynamics plugin
    success = plugin_manager.load_plugins("plugins/")
    
    if success:
        print("Successfully loaded plugins")
        
        # List loaded plugins
        loaded_plugins = plugin_manager.list_plugins()
        print(f"Loaded {len(loaded_plugins)} plugins:")
        for plugin in loaded_plugins:
            print(f"  - {plugin.name} ({plugin.version})")
    else:
        print("No plugins loaded (directory might be empty)")
        
except Exception as e:
    print(f"Plugin loading failed: {e}")

# Example 4: Creating Custom Neural Components
print("\n4. Custom Neural Component Creation Example")
print("-" * 30)

# Enable advanced learning features
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Enabled advanced learning subsystems:")
print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"  - Development: {agent.isDevelopmentEnabled()}")
print(f"  - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")

# Example 5: Plugin Configuration Management
print("\n5. Plugin Configuration Management Example")
print("-" * 30)

# Create custom configuration
custom_config = createDefaultConfig()

# Set custom parameters
custom_config.set("brain.neuron_count", 2000)
custom_config.set("plasticity.stdp.learning_rate", 0.01)
custom_config.set("neuromod.dopamine.scale", 2.0)

print("Created custom configuration:")
print(f"  - Neuron count: {custom_config.get('brain.neuron_count')}")
print(f"  - STDP learning rate: {custom_config.get('plasticity.stdp.learning_rate')}")
print(f"  - Dopamine scale: {custom_config.get('neuromod.dopamine.scale')}")

# Create brain with custom config
custom_brain = createBrain(custom_config)
custom_brain.initialize()
print(f"Custom brain created with {custom_brain.getTotalNeuronCount()} neurons")

# Example 6: Plugin-Based Simulation
print("\n6. Plugin-Based Simulation Example")
print("-" * 30)

# Run simulation with plugin-like capabilities
def run_plugin_simulation(brain, world, agent, num_steps=500):
    """Run simulation with plugin-like features."""
    print(f"Running simulation with {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (plugin-like feature)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (plugin-like feature)
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"{brain.getFiringNeuronCount()} neurons firing, "
                  f"{agent.getCuriosityLevel():.3f} curiosity, "
                  f"{brain.getDevelopmentalStage()} development stage")
    
    print("Simulation complete!")
    return {
        'total_steps': num_steps,
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_prediction_error': agent.getPredictionError(),
        'development_stage': brain.getDevelopmentalStage(),
    }

# Run the simulation
simulation_results = run_plugin_simulation(brain, world, agent, num_steps=300)

print(f"\nSimulation Results:")
print(f"  - Total steps: {simulation_results['total_steps']}")
print(f"  - Final curiosity: {simulation_results['final_curiosity']:.3f}")
print(f"  - Final novelty: {simulation_results['final_novelty']:.3f}")
print(f"  - Final prediction error: {simulation_results['final_prediction_error']:.3f}")
print(f"  - Final development stage: {simulation_results['development_stage']}")

# Example 7: Plugin Communication Example
print("\n7. Plugin Communication Example")
print("-" * 30)

# Simulate plugin communication through the agent
print("Plugin communication through agent interface:")
print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  - Prediction error: {agent.getPredictionError():.3f}")
print(f"  - Development stage: {agent.getDevelopmentalStage()}")

# Example 8: Plugin Error Handling
print("\n8. Plugin Error Handling Example")
print("-" * 30)

try:
    # Try to validate simulation setup
    from pynlm.utils import validate_simulation_setup
    errors = validate_simulation_setup(brain, world, agent)
    
    if errors:
        print("Validation errors found:")
        for error in errors:
            print(f"  - {error}")
    else:
        print("Simulation setup is valid")
        
except Exception as e:
    print(f"Error during validation: {e}")

# Example 9: Plugin Performance Monitoring
print("\n9. Plugin Performance Monitoring Example")
print("-" * 30)

try:
    # Get simulation statistics
    from pynlm.utils import get_simulation_stats
    stats = get_simulation_stats(brain, world, agent)
    
    print("Simulation statistics:")
    print(f"  - Neurons: {stats['brain_stats']['neuron_count']}")
    print(f"  - Firing rate: {stats['brain_stats']['firing_rate']:.3f} Hz")
    print(f"  - Total spikes: {stats['brain_stats']['total_spikes']}")
    print(f"  - Development stage: {stats['brain_stats']['development_stage']}")
    
    if 'agent_stats' in stats:
        print(f"  - Curiosity: {stats['agent_stats']['curiosity_level']:.3f}")
        print(f"  - Novelty: {stats['agent_stats']['novelty_level']:.3f}")
        
except Exception as e:
    print(f"Error getting statistics: {e}")

# Example 10: Plugin Checkpoints
print("\n10. Plugin Checkpointing Example")
print("-" * 30)

try:
    # Save checkpoint
    from pynlm.utils import save_checkpoint
    
    checkpoint_data = save_checkpoint(
        brain, 
        "simulation_checkpoint.bin", 
        "Simulation checkpoint after testing plugin architecture"
    )
    
    print("Checkpoint saved successfully!")
    print(f"  - File: simulation_checkpoint.bin")
    print(f"  - Metadata file: simulation_checkpoint.meta.json")
    print(f"  - Description: {checkpoint_data.get('description', 'N/A')}")
    
except Exception as e:
    print(f"Error saving checkpoint: {e}")

# Example 11: Advanced Plugin Features
print("\n11. Advanced Plugin Features Example")
print("-" * 30)

print("Advanced plugin features demonstration:")
print("  1. Progress tracking with run_with_progress()")
print("  2. Performance benchmarking with benchmark_simulation()")
print("  3. Simulation validation with validate_simulation_setup()")
print("  4. Checkpoint management with save_checkpoint()")
print("  5. Configuration validation and schema checking")
print("  6. Error handling and recovery")
print("  7. Plugin discovery and loading")
print("  8. Component creation and dependency management")

# Example 12: Plugin Development Workflow
print("\n12. Plugin Development Workflow")
print("-" * 30)

print("Plugin development workflow:")
print("  1. Design plugin interface (INeuronDynamics, IPlasticityRule, etc.)")
print("  2. Implement concrete plugin class")
print("  3. Create metadata (PluginMetadata)")
print("  4. Package plugin (shared library + config file)")
print("  5. Test plugin with plugin_manager")
print("  6. Integrate into NLM simulation")
print("  7. Distribute plugin to other users")

print("\n" + "=" * 50)
print("Plugin Architecture Examples Complete!")
print("=" * 50)

print("\nKey Takeaways:")
print("1. Plugins enable runtime extension without recompilation")
print("2. Plugin manager handles loading, validation, and lifecycle")
print("3. Extension points allow specific types of plugins")
print("4. Error handling ensures robust plugin management")
print("5. Configuration management supports custom behavior")
print("6. Performance monitoring helps optimize plugin usage")

print("\nNext Steps:")
print("1. Create your first plugin (neuron dynamics, plasticity rule, etc.)")
print("2. Use the plugin manager to discover and load plugins")
print("3. Integrate plugins into your NLM simulations")
print("4. Share your plugins with the community")
