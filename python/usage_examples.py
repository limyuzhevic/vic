#!/usr/bin/env python3
"""
Example usage of the enhanced NLM Python bindings.
This demonstrates all the new features added to bindings.cpp.
"""

import sys
import os

# Add the Python module path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

print("Enhanced NLM Python Bindings Usage Examples")
print("=" * 50)

# Note: This is a demonstration file that shows how the enhanced
# bindings would be used. The actual pynlm module would need to be
# built first using CMake and pybind11.

print("\n1. HIGH-LEVEL FACTORY METHODS AND CONVENIENCE FUNCTIONS")
print("-" * 50)

print("""
# Create a brain with developmental configuration
from pynlm import create_brain, create_developmental_config
config = create_developmental_config()
brain = create_brain(config)

# Create a complete agent system
from pynlm import create_agent_system, create_simple_world
world = create_simple_world()
system = create_agent_system(config, world)

# Create a complete working simulation
from pynlm import create_simple_simulation
config, brain, agent_brain, world = create_simple_simulation()

# Create visualization helper
from pynlm import create_visualization_helper
viz_helper = create_visualization_helper()

# Create event system
from pynlm import create_event_system
event_system = create_event_system()
""")

print("\n2. CONTEXT MANAGERS FOR AUTOMATIC RESOURCE CLEANUP")
print("-" * 50)

print("""
# Using Brain with context manager
from pynlm import create_brain, create_developmental_config
config = create_developmental_config()

with create_brain(config) as brain:
    brain.initialize()
    print(f"Brain initialized: {brain}")
    # Brain is automatically cleaned up when exiting the context

# Using AgentBrain with context manager  
from pynlm import create_agent_brain
with create_agent_brain(brain) as agent_brain:
    agent_brain.initialize(world)
    print(f"Agent brain initialized")
    # Agent brain is automatically cleaned up when exiting the context

# Using SimpleWorld with context manager
from pynlm import create_simple_world
with create_simple_world() as world:
    world.configure(100, 100, 16, 16)
    print(f"World configured: {world}")
    # World is automatically cleaned up when exiting the context
""")

print("\n3. PYTHONIC APIS AND PROPERTY VALIDATION")
print("-" * 50)

print("""
from pynlm import create_simple_simulation

config, brain, agent_brain, world = create_simple_simulation()
brain.initialize()

# Pythonic property access with validation
brain.set_developmental_stage("CriticalPeriod")  # String accepted
brain.set_developmental_stage(brain.DevelopmentalStage.CriticalPeriod)  # Enum accepted

# Safe getters with default values
neuron_count = brain.get_total_neuron_count()
firing_rate = brain.get_average_firing_rate() or 0.0  # Default to 0.0 if None
preference = brain.get_developmental_stage().name  # Get enum name

# Property validation
brain.set_developmental_stage("InvalidStage")  # Will raise RuntimeError with helpful message

# Pythonic setters
brain.set_developmental_stage("Maturation")
brain.set_developmental_stage("Adult")

# Enhanced Action API
action = brain.produce_action()
print(f"Action type: {action.get_type()}")
print(f"Action name: {action.get_name()}")
print(f"Action parameters: {action.get_parameters()}")
""")

print("\n4. EVENT SYSTEM WITH CALLBACKS")
print("-" * 50)

print("""
from pynlm import create_event_system, BrainEventType

event_system = create_event_system()

# Define event callbacks
def on_spike(event):
    print(f"Spike event: neuron {event.get_data()[0]}, step {event.step}")

def on_action(event):
    action = event.get_data()[1]
    print(f"Action event: {action.get_name()}")

def on_reward(event):
    reward = event.get_data()[2]
    print(f"Reward event: {reward}")

# Register callbacks
event_system.register_callback(BrainEventType.Spike, on_spike)
event_system.register_callback(BrainEventType.Action, on_action)
event_system.register_callback(BrainEventType.Reward, on_reward)

# Create event and trigger it
from pynlm import NeuronId
spike_event = BrainEvent(
    BrainEventType.Spike,
    timestamp=1.0,
    step=100,
    description="Neuron spike",
    neuron_id=NeuronId(42)
)
event_system.trigger_event(spike_event)

# Event filtering
events = event_system.get_events()
event_system.filter_events(events, lambda e: e.step > 50)
""")

print("\n5. VISUALIZATION AND SIMULATION UTILITY FUNCTIONS")
print("-" * 50)

print("""
from pynlm import create_simple_simulation, create_visualization_helper

config, brain, agent_brain, world = create_simple_simulation()
brain.initialize()

# Create visualization helper
viz_helper = create_visualization_helper()

# Generate comprehensive simulation statistics
stats = viz_helper.generate_simulation_stats(brain, world)
print("Simulation Statistics:")
print(stats)

# Generate brain activity summary
summary = viz_helper.generate_brain_activity_summary(brain, steps=100)
print("Brain Activity Summary:")
print(summary)

# Compare multiple simulations
config2, brain2, agent_brain2, world2 = create_simple_simulation()
brain2.initialize()
comparison = viz_helper.generate_comparison_report(
    [brain, brain2],
    ["Simulation 1", "Simulation 2"]
)
print("Simulation Comparison:")
print(comparison)
""")

print("\n6. ENHANCED ENUM SUPPORT WITH STRING REPRESENTATIONS")
print("-" * 50)

print("""
from pynlm import NeuronType, SynapseType, DevelopmentalStage, ActionType

# String to enum conversion
neuron_type = NeuronType.from_string("Excitatory")
print(f"NeuronType from 'Excitatory': {neuron_type}")

# Enum to string conversion
print(f"NeuronType to string: {NeuronType.to_string(neuron_type)}")

# Different enum types
dev_stage = DevelopmentalStage.from_string("CriticalPeriod")
action_type = ActionType.from_string("MoveForward")
synapse_type = SynapseType.from_string("Excitatory")

print(f"Development stage: {dev_stage} -> {DevelopmentalStage.to_string(dev_stage)}")
print(f"Action type: {action_type} -> {ActionType.to_string(action_type)}")
print(f"Synapse type: {synapse_type} -> {SynapseType.to_string(synapse_type)}")

# Enum validation
try:
    invalid = NeuronType.from_string("InvalidType")
except RuntimeError as e:
    print(f"Enum validation caught error: {e}")
""")

print("\n7. LOGGING AND PROGRESS TRACKING UTILITIES")
print("-" * 50)

print("""
from pynlm import get_simulation_logger, PythonSimulationLogger

logger = get_simulation_logger()
logger.set_log_level(PythonSimulationLogger.LogLevel.INFO)

# Progress tracking
for step in range(100):
    logger.info(f"Processing step {step}/100", step)
    # Your simulation code here
    if step % 10 == 0:
        logger.warning(f"Milestone reached: step {step}", step)

# Get log summary
summary = logger.get_log_summary()
print("Log Summary:")
print(summary)

# Log filtering
logs = logger.get_logs()
recent_logs = [log for log in logs if log.step > 50]
print(f"Recent logs (step > 50): {len(recent_logs)}")
""")

print("\n8. CONFIGURATION PRESETS AND ADVANCED OPTIONS")
print("-" * 50)

print("""
from pynlm import create_developmental_config, create_adult_config, create_learning_config

# Developmental preset (youth-like learning phase)
devel_config = create_developmental_config()
print("Developmental config keys:", devel_config.get_keys())

# Adult preset (mature system)
adult_config = create_adult_config()
print("Adult config keys:", adult_config.get_keys())

# Learning preset (balanced)
learn_config = create_learning_config()
print("Learning config keys:", learn_config.get_keys())

# Custom configuration
custom_settings = {
    "brain.neuron_count": 800,
    "brain.inhibitory_ratio": 0.3,
    "world.max_energy": 75.0,
    "brain.reward_modulation_enabled": True
}
from pynlm import create_custom_config
custom_config = create_custom_config(custom_settings)
print("Custom config validation:", custom_config.get_keys())

# Configuration validation
from pynlm import validate_config
errors = validate_config(custom_config)
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid!")
""")

print("\n9. UNIT TESTS FOR NEW FUNCTIONALITY")
print("-" * 50)

print("""
# The enhanced bindings include comprehensive unit tests
# for all new features. Example test structure:

import pytest
from pynlm import (
    create_brain, create_simple_world, create_event_system,
    create_visualization_helper, validate_config
)

def test_factory_methods():
    config = create_developmental_config()
    brain = create_brain(config)
    assert brain is not None
    brain.initialize()
    assert brain.get_total_neuron_count() > 0

def test_context_manager():
    config = create_developmental_config()
    with create_brain(config) as brain:
        brain.initialize()
        assert not brain.get_brain().get_region_count() < 0  # Valid state
    # Brain should be cleaned up
    assert brain.is_cleaned_up()

def test_event_system():
    event_system = create_event_system()
    assert event_system.get_events() == []
    
    # Test event triggering
    from pynlm import BrainEvent, BrainEventType, NeuronId
    event = BrainEvent(
        BrainEventType.Spike,
        timestamp=1.0,
        step=100,
        description="Test spike",
        neuron_id=NeuronId(1)
    )
    event_system.add_event(event)
    assert len(event_system.get_events()) == 1
    
    # Test event filtering
    events = event_system.get_events()
    event_system.filter_events(events, lambda e: e.step > 50)
    assert len(events) == 0  # Should be empty after filtering

def test_pythonic_apis():
    config, brain, agent_brain, world = create_simple_simulation()
    brain.initialize()
    
    # Test Pythonic property access
    brain.set_developmental_stage("CriticalPeriod")
    assert brain.get_developmental_stage().name == "CriticalPeriod"
    
    # Test safe getters
    neuron_count = brain.get_total_neuron_count() or 0
    assert neuron_count >= 0

def test_visualization():
    config, brain, agent_brain, world = create_simple_simulation()
    brain.initialize()
    
    viz_helper = create_visualization_helper()
    stats = viz_helper.generate_simulation_stats(brain, world)
    assert "Simulation Statistics" in stats
    assert "Brain Statistics" in stats
    assert "World Statistics" in stats

def test_config_validation():
    config = create_developamental_config()
    errors = validate_config(config)
    assert errors == []  # Should have no errors

def test_enum_support():
    from pynlm import NeuronType, neuron_type_to_string, neuron_type_from_string
    
    # Test conversion
    neuron_type = neuron_type_from_string("Excitatory")
    assert neuron_type == NeuronType.Excitatory
    
    # Test string representation
    assert neuron_type_to_string(neuron_type) == "Excitatory"
    
    # Test validation
    with pytest.raises(RuntimeError):
        neuron_type_from_string("InvalidType")

if __name__ == "__main__":
    # Run the tests
    test_factory_methods()
    test_context_manager()
    test_event_system()
    test_pythonic_apis()
    test_visualization()
    test_config_validation()
    test_enum_support()
    print("All tests passed!")
""")

print("\n" + "=" * 50)
print("Summary of Enhanced Features:")
print("=" * 50)

print("""
✓ High-level factory methods and convenience functions
✓ Context managers for automatic resource cleanup
✓ Pythonic APIs with property validation
✓ Event system with callbacks and filtering
✓ Visualization and simulation utility functions
✓ Enhanced enum support with string representations
✓ Logging and progress tracking utilities
✓ Configuration presets and validation helpers
✓ Comprehensive unit tests

All features maintain backward compatibility while providing
powerful new Pythonic interfaces for the NLM framework.
""")

print("\nNote: This is a demonstration file. The actual pynlm module")
print("needs to be built using CMake and pybind11 to be used.")
