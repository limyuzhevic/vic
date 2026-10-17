#!/usr/bin/env python3
"""
Example: Using NLM BrainBuilder
===============================

This example demonstrates how to use the new BrainBuilder API to create
and configure NLM brains with different configurations for various use cases.
"""

import sys
sys.path.insert(0, 'python')

import pynlm

def example_basic_brain():
    """Example: Create a basic brain using BrainBuilder."""
    print("Example 1: Basic BrainBuilder Usage")
    print("=" * 40)
    
    # Create a standard research brain
    brain = pynlm.createStandardBrain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain type: Standard Research")
    
    # Run a few simulation steps
    for i in range(10):
        brain.step(i)
    
    print(f"After 10 steps: {brain.getTotalSpikeCount()} spikes")
    print()

def example_learning_brain():
    """Example: Create a learning-optimized brain."""
    print("Example 2: Learning-Optimized Brain")
    print("=" * 40)
    
    # Use BrainBuilder directly for more control
    brain = pynlm.BrainBuilder() \
        .configure_for_learning() \
        .add_region("Sensory", 200) \
        .add_region("Association", 300) \
        .add_region("Motor", 200) \
        .enable_memory(True) \
        .enable_cognition(True) \
        .build_research()
    
    brain.initialize()
    
    print(f"Created learning brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain type: Learning Optimized")
    print(f"Regions: {brain.getRegionCount()}")
    print()
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    print(f"After 50 steps: {brain.getTotalSpikeCount()} spikes")
    print()

def example_configuration_comparison():
    """Example: Compare different brain configurations."""
    print("Example 3: Configuration Comparison")
    print("=" * 40)
    
    brain_types = ['minimal', 'standard', 'learning', 'performance']
    
    for brain_type in brain_types:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Configure based on type
        if brain_type == 'minimal':
            brain = pynlm.BrainBuilder().configure_for_minimal().build_research()
        elif brain_type == 'performance':
            brain = pynlm.BrainBuilder().configure_for_performance().build_research()
        else:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
        
        # Run simulation
        steps = 100 if brain_type != 'performance' else 10
        for i in range(steps):
            brain.step(i)
        
        stats = brain.getStatistics()
        print(f"{brain_type:12s}: Neurons={stats['total_neurons']:4d}, "
              f"Spikes={stats['total_spikes']:5d}, "
              f"Firing={stats['avg_firing_rate']:6.2f} Hz")
    
    print()

def example_factory_functions():
    """Example: Using convenience factory functions."""
    print("Example 4: Factory Functions")
    print("=" * 40)
    
    # Create different brain types using factory functions
    brains = {
        'minimal': pynlm.createMinimalBrain(),
        'standard': pynlm.createStandardBrain(),
        'learning': pynlm.createLearningBrain(),
        'cognition': pynlm.createCognitionBrain(),
        'development': pynlm.createDevelopmentBrain(),
        'performance': pynlm.createPerformanceBrain(),
        'research': pynlm.createResearchBrain(),
    }
    
    for name, brain in brains.items():
        brain.initialize()
        
        # Quick benchmark
        for i in range(20):
            brain.step(i)
        
        stats = brain.getStatistics()
        print(f"{name:12s}: {stats['total_neurons']:4d} neurons, "
              f"{stats['total_spikes']:5d} spikes")
        
        # Clean up
        del brain
    
    print()

def example_world_interaction():
    """Example: Agent-world interaction."""
    print("Example 5: Agent-World Interaction")
    print("=" * 40)
    
    # Create brain, world, and agent
    brain = pynlm.createLearningBrain()
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Created agent-world system")
    print(f"World size: {world.getWidth()} x {world.getHeight()}")
    print(f"Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run interaction
    for step in range(30):
        world.update(0.1)
        
        # Get sensor input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step:2d}: {brain.getTotalSpikeCount():4d} spikes, "
                  f"Action: {action.getType()}, "
                  f"Success: {result.success}")
    
    # Print final status
    stats = brain.getStatistics()
    print(f"\nFinal statistics:")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
    print(f"  Learning enabled: {agent.isRewardModulationEnabled()}")
    
    print()

def example_brain_builder_fluent_interface():
    """Example: Fluent interface with BrainBuilder."""
    print("Example 6: Fluent Interface with BrainBuilder")
    print("=" * 40)
    
    # Create complex brain using fluent interface
    brain = (pynlm.BrainBuilder()
             .configure_for_learning()
             .add_region("Sensory", 150)
             .add_region("Association", 400)
             .add_region("Motor", 150)
             .add_region("Cognitive", 200)
             .enable_stdp(True)
             .enable_hebbian(True)
             .enable_memory(True)
             .enable_prediction(True)
             .enable_cognition(True)
             .enable_neuromodulation(True)
             .enable_development(True)
             .set_stdp_parameters(0.025f, 0.02f, 15.0f)
             .build_research())
    
    brain.initialize()
    
    print("Created complex brain using fluent interface:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print(f"  STDP enabled: {brain.getSTDP() is not None}")
    print(f"  Hebbian enabled: {brain.getHebbian() is not None}")
    print(f"  Memory enabled: {brain.getWorkingMemory() is not None}")
    print(f"  Prediction enabled: {brain.getPredictionSystem() is not None}")
    print(f"  Cognition enabled: {brain.getPlanner() is not None}")
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    stats = brain.getStatistics()
    print(f"\nAfter 100 steps:")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
    print(f"  Development stage: {stats['development_stage']}")
    
    print()

def main():
    """Run all examples."""
    print("NLM BrainBuilder Examples")
    print("=" * 50)
    print()
    
    example_basic_brain()
    example_learning_brain()
    example_configuration_comparison()
    example_factory_functions()
    example_world_interaction()
    example_brain_builder_fluent_interface()
    
    print("All examples completed successfully!")
    print("\nKey Features Demonstrated:")
    print("  • Basic brain creation with factory functions")
    print("  • BrainBuilder with fluent interface")
    print("  • Configuration comparison")
    print("  • Convenience factory functions")
    print("  • Agent-world interaction")
    print("  • Advanced BrainBuilder configuration")
    print("\nFor more information, see:")
    print("  • nlm_cli.py - Command line interface")
    print("  • docs/ARCHITECTURE.md - Architecture documentation")
    print("  • python/bindings.cpp - Python API documentation")
if __name__ == "__main__":
    main()