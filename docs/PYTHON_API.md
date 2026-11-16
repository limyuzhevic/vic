"""
NLM Python API Documentation
=============================

This document provides comprehensive documentation for the NLM Python API,
showing how to use the improved bindings and new convenience methods.
"""

import pynlm

print("NLM Python API Documentation")
print("=" * 50)
print()

print("Package Information:")
print(f"  Version: {pynlm.__version__}")
print(f"  Author: {pynlm.__author__}")
print(f"  Description: {pynlm.__description__}")
print()

print("Core Classes:")
print(f"  - Config: Configuration management")
print(f"  - Brain: Neural network simulation engine")
print(f"  - AgentBrain: Interface between brain and world")
print(f"  - SimpleWorld: Simulated environment")
print(f"  - Vision, Audio, InternalSignals: Sensory input types")
print(f"  - Action, ActionResult: Motor output types")
print()

print("Key Enumerations:")
print(f"  - NeuronType: Excitatory, Inhibitory, Sensory, Motor, Internal, Modulatory")
print(f"  - SynapseType: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction")
print(f"  - DevelopmentalStage: Initial, CriticalPeriod, Maturation, Adult, Aging")
print(f"  - ActionType: Various movement and interaction actions")
print(f"  - MotorCommand: Low-level motor commands")
print(f"  - WorldObjectType: Empty, Resource, Hazard, Wall, Marker")
print()

print("Basic Usage Examples:")
print()

print("Example 1: Simple Brain Simulation")
print("-" * 40)
example1_code = '''
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
'''
print(example1_code)

print("Example 2: Complete Agent Simulation")
print("-" * 40)
example2_code = '''
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    agent.applyRewardModulation(0.1, 0.05)
    agent.updateDevelopment(0.1)
'''
print(example2_code)

print("Example 3: Configuration Management")
print("-" * 40)
example3_code = '''
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("brain.initial_weight_mean", 0.6f)
config.set("brain.v_thresh", -55.0f)

# Validate configuration
if pynlm.validate_config(config):
    print("Configuration validated successfully")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
'''
print(example3_code)

print("Example 4: Statistics and Monitoring")
print("-" * 40)
example4_code = '''
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive statistics
summary = pynlm.get_brain_summary(brain)
print(f"Brain Summary: {summary}")

# Get detailed metrics
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'total_spikes': brain.getTotalSpikeCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'avg_firing_rate': brain.getAverageFiringRate(),
    'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
}

print("\nDetailed Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
'''
print(example4_code)

print("Example 5: Error Handling")
print("-" * 40)
example5_code = '''
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration - throws exception if invalid
try:
    pynlm.validate_config(config)
    print("Configuration validation passed")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Create brain with error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain created successfully")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
'''
print(example5_code)

print("Convenience Functions:")
print("-" * 40)
print("  - pynlm.create_demo_config(): Create demo configuration with sensible defaults")
print("  - pynlm.create_brain(): Alias for pynlm.createBrain()")
print("  - pynlm.run_simulation(): Complete simulation wrapper")
print("  - pynlm.validate_config(): Validate configuration")
print("  - pynlm.get_brain_summary(): Get brain statistics summary")
print()

print("Error Classes:")
print("-" * 40)
print("  - pynlm.NLMError: Base exception for all NLM errors")
print("  - pynlm.ConfigError: Configuration-related errors")
print("  - pynlm.SimulationError: Simulation-related errors")
print("  - pynlm.RuntimeError: Standard runtime errors")
print("  - pynlm.InvalidArgumentError: Invalid argument errors")
print("  - pynlm.OutOfRangeError: Out of range errors")
print()

print("Type Hints Available:")
print("-" * 40)
print("  - NeuronType")
print("  - SynapseType")
print("  - DevelopmentalStage")
print("  - ActionType")
print("  - MotorCommand")
print("  - WorldObjectType")
print("  - NeuronId, SynapseId, RegionId, PopulationId")
print("  - SensoryInput, Vision, Audio, InternalSignals")
print("  - Action, ActionResult")
print("  - WorldObject, AgentBody, SensoryPercept")
print()

print("Documentation Sources:")
print("-" * 40)
print("  - NLM README.md: Main documentation")
print("  - easy_usage.md: Quick start guide")
print("  - HOW_TO_USE.md: Comprehensive usage instructions")
print("  - docs/ARCHITECTURE.md: System architecture")
print("  - Phase6FinalAudit.md: Phase 6 integration audit")
print("  - src/brain/Brain.hpp: C++ Brain class documentation")
print()

print("Installation and Setup:")
print("-" * 40)
print("  pip install pynlm")
print("  # Or install from source:")
print("  pip install -e .")
print("  # After building with:")
print("  python setup.py build_ext --inplace")
print()

print("Performance Considerations:")
print("-" * 40)
print("  - Use compile-time configuration for production")
print("  - Monitor neuron count and synapse density")
print("  - Consider using multiple regions for large networks")
print("  - Enable checkpointing for long simulations")
print("  - Use batch processing for repeated simulations")
print()

print("Troubleshooting:")
print("-" * 40)
print("  - 'ImportError: No module named pynlm'")
print("    -> Install the package: pip install pynlm")
print("  - 'RuntimeError: Brain not initialized'")
print("    -> Call brain.initialize() before brain.step()")
print("  - 'RuntimeError: Configuration validation failed'")
print("    -> Use pynlm.create_demo_config() for default settings")
print("  - 'RuntimeError: Memory error'")
print("    -> Reduce neuron count or increase system memory")
print("  - 'RuntimeError: Invalid action'")
print("    -> Check world boundaries and agent capabilities")
print()

print("For More Information:")
print("-" * 40)
print("  - NLM project documentation")
print("  - Phase 6 integration guides")
print("  - Python pybind11 tutorials")
print("  - C++ NLM source code (src/ directory)")
print()

print("The NLM Python API provides a comprehensive interface to the")
print("powerful neural simulation framework, with special attention")
print("to ease of use, error handling, and comprehensive documentation.")
