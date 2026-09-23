#!/usr/bin/env python3
"""
NLM Python Bindings - Quick Start Guide

This script provides examples and demonstrations of the NLM (Neural Learning Machine)
Python bindings. It showcases the Pythonic interface and common use cases.
"""

import pynlm as nlm
import sys
import os

def example_1_basic_usage():
    """Example 1: Basic NLM usage with Pythonic interface"""
    print("=== Example 1: Basic NLM Usage ===")
    
    # Create default configuration
    config = nlm.create_config()
    print(f"Created config: {config}")
    
    # Create brain with configuration
    brain = nlm.create_brain(config)
    print(f"Created brain: {brain}")
    
    # Create world
    world = nlm.create_world()
    world.configure(100, 100, 20, 20)
    print(f"Created world: {world}")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized successfully")
    
    # Create agent
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    print("Agent initialized successfully")
    
    # Run one simulation step
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    action = agent.decode_motor_command()
    
    print(f"Agent action: {action.get_name()} (type: {action.get_type()})")
    print(f"Action parameters: {action.get_parameters()}")
    print("=== Basic Usage Complete ===\n")

def example_2_advanced_agent():
    """Example 2: Advanced agent with all features enabled"""
    print("=== Example 2: Advanced Agent Setup ===")
    
    # Create training environment
    world = nlm.create_training_environment(50, 50, 10, 10)
    print(f"Created training environment: {world}")
    
    # Create advanced agent with all systems enabled
    agent = nlm.create_advanced_agent(
        config=nlm.create_config(),
        world=world
    )
    print("Created advanced agent with all systems enabled")
    
    # Enable/disable features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Check status
    print(f"Reward modulation enabled: {agent.is_reward_modulation_enabled()}")
    print(f"Structural plasticity enabled: {agent.is_structural_plasticity_enabled()}")
    print(f"Development enabled: {agent.is_development_enabled()}")
    print(f"Curiosity enabled: {agent.is_curiosity_enabled()}")
    
    # Get agent information
    print(f"Sensory input size: {agent.get_sensory_input_size()}")
    print(f"Motor output size: {agent.get_motor_output_size()}")
    
    print("=== Advanced Agent Setup Complete ===\n")

def example_3_experiment_setup():
    """Example 3: Complete experimental setup"""
    print("=== Example 3: Complete Experimental Setup ===")
    
    # Create experimental setup with all components
    config, world, agent = nlm.create_experiment_setup(80, 80, 15, 15)
    
    print(f"Experiment config: {config}")
    print(f"Experiment world: {world}")
    print(f"Experiment agent: {agent}")
    
    # Add some world objects
    obj1 = nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 100.0)
    obj2 = nlm.WorldObject(30, 30, nlm.WorldObjectType.Hazard, 0.5)
    world.add_object(obj1)
    world.add_object(obj2)
    
    print(f"Added resource at (10, 10) with value {obj1.value}")
    print(f"Added hazard at (30, 30) with radius {obj2.radius}")
    
    # Initialize brain and agent
    agent.get_brain().initialize()
    agent.initialize(world)
    
    # Run multiple simulation steps
    for step in range(3):
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        action = agent.decode_motor_command()
        world.update(step)
        
        print(f"Step {step + 1}: Action = {action.get_name()}")
        
        # Apply action to world (simplified)
        agent.get_brain().produce_action()
    
    print("=== Experimental Setup Complete ===\n")

def example_4_context_managers():
    """Example 4: Using context managers for resource management"""
    print("=== Example 4: Context Manager Usage ===")
    
    # Create config and use context manager for brain
    config = nlm.create_config()
    
    with nlm.BrainContext(config) as brain:
        brain.initialize()
        print(f"Brain in context: {brain}")
        
        # Perform operations
        print(f"Total neurons: {brain.get_total_neuron_count()}")
        print(f"Regions: {brain.get_region_count()}")
        
        # Brain will be automatically reset when exiting context
    
    print("Context manager automatically reset brain")
    print("=== Context Manager Example Complete ===\n")

def example_5_debug_and_monitor():
    """Example 5: Debug and monitoring"""
    print("=== Example 5: Debug and Monitoring ===")
    
    # Create and initialize brain
    brain = nlm.create_brain()
    brain.initialize()
    
    # Debug brain connections
    nlm.debug_brain_connections(brain)
    
    # Analyze brain efficiency
    efficiency = nlm.analyze_brain_efficiency(brain)
    print(f"Brain efficiency: {efficiency * 100:.1f}%")
    
    # Get brain status
    status = nlm.monitor_brain_health(brain)
    print(f"Brain status:\n{status}")
    
    # Print brain summary
    nlm.print_brain_summary(brain)
    
    print("=== Debug and Monitoring Complete ===\n")

def example_6_quick_prototype():
    """Example 6: Quick prototyping setup"""
    print("=== Example 6: Quick Prototyping ===")
    
    # Use quick prototyping to get all components set up
    agent, world = nlm.quick_prototype()
    
    print(f"Quick prototype created:")
    print(f"  - Agent: {agent}")
    print(f"  - World: {world}")
    print(f"  - World size: {world.get_width()}x{world.get_height()}m")
    
    # Initialize and run one step
    agent.get_brain().initialize()
    agent.initialize(world)
    
    for step in range(5):
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        action = agent.decode_motor_command()
        world.update(step)
        
        print(f"Step {step + 1}: {action.get_name()}")
    
    print("=== Quick Prototyping Complete ===\n")

def example_7_enum_utilities():
    """Example 7: Using enum utilities"""
    print("=== Example 7: Enum Utilities ===")
    
    # Print all enum values for reference
    print("Enum values for reference:")
    nlm.print_enum_values()
    
    # Demonstrate string conversion utilities
    print("\nDemonstrating string conversions:")
    print(f"NeuronType: Excitatory -> {nlm.neuron_type_to_string(nlm.NeuronType.Excitatory)}")
    print(f"SynapseType: Electrical -> {nlm.synapse_type_to_string(nlm.SynapseType.Electrical)}")
    print(f"ActionType: MoveForward -> {nlm.action_type_to_string(nlm.ActionType.MoveForward)}")
    print(f"WorldObjectType: Resource -> {nlm.world_object_type_to_string(nlm.WorldObjectType.Resource)}")
    
    # Demonstrate string to enum conversion
    print("\nDemonstrating string to enum conversion:")
    neuron_type = nlm.string_to_neuron_type("inhibitory")
    print(f"string_to_neuron_type('inhibitory') -> {neuron_type}")
    
    synapse_type = nlm.string_to_synapse_type("gapjunction")
    print(f"string_to_synapse_type('gapjunction') -> {synapse_type}")
    
    action_type = nlm.string_to_action_type("look up")
    print(f"string_to_action_type('look up') -> {action_type}")
    
    world_type = nlm.string_to_world_object_type("hazard")
    print(f"string_to_world_object_type('hazard') -> {world_type}")
    
    print("=== Enum Utilities Complete ===\n")

def example_8_safety_and_error_handling():
    """Example 8: Safety and error handling"""
    print("=== Example 8: Safety and Error Handling ===")
    
    # Create components
    brain = nlm.create_brain()
    world = nlm.create_world()
    world.configure(50, 50, 10, 10)
    
    # Use safe_step with error handling
    result = nlm.safe_step(brain, world, 10)
    print(f"Safe simulation result: {result}")
    
    # Validate brain state
    nlm.validate_brain_state(brain)
    
    print("=== Safety and Error Handling Complete ===\n")

def example_9_development_stages():
    """Example 9: Working with developmental stages"""
    print("=== Example 9: Development Stages ===")
    
    # Create brain for each developmental stage
    stages = [
        nlm.DevelopmentalStage.Initial,
        nlm.DevelopmentalStage.CriticalPeriod,
        nlm.DevelopmentalStage.Maturation,
        nlm.DevelopmentalStage.Adult,
        nlm.DevelopmentalStage.Aging
    ]
    
    for stage in stages:
        brain = nlm.create_development_stage_brain(stage)
        brain.initialize()
        print(f"Created {stage} brain with {brain.get_total_neuron_count()} neurons")
    
    print("=== Development Stages Complete ===\n")

def main():
    """Run all examples"""
    print("NLM Python Bindings - Quick Start Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_advanced_agent,
        example_3_experiment_setup,
        example_4_context_managers,
        example_5_debug_and_monitor,
        example_6_quick_prototype,
        example_7_enum_utilities,
        example_8_safety_and_error_handling,
        example_9_development_stages,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            print(f"Skipping example {example.__name__}\n")
    
    print("=" * 50)
    print("All examples completed!")
    print("\nFor more detailed documentation, refer to:")
    print("- NLM documentation: https://nlm.readthedocs.io")
    print("- Python API reference: See docstrings in the pynlm module")
    print("- Examples directory for more advanced use cases")

if __name__ == "__main__":
    main()