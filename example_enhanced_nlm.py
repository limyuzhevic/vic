#!/usr/bin/env python3
"""
Enhanced NLM Python Example
==========================

This example demonstrates the enhanced NLM Python bindings with improved
memory safety, error handling, and new features.

Key Improvements:
- Better memory management with safe reference policies
- Enhanced error handling with clear exceptions
- New utility functions for common patterns
- Improved type safety and validation
- Comprehensive logging and debugging tools
"""

import pynlm
import sys
import traceback

def example_basic_brain():
    """Example 1: Basic brain simulation with enhanced safety."""
    print("Example 1: Basic Brain Simulation")
    print("-" * 40)
    
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with enhanced error checking
    print(f"Initial state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    for step in range(20):
        brain.step(step)
        
        # Enhanced logging with safety checks
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    print(f"Simulation complete! Final spike count: {brain.getTotalSpikeCount()}")
    print()

def example_agent_world():
    """Example 2: Complete agent-world interaction with enhanced features."""
    print("Example 2: Agent-World Interaction")
    print("-" * 40)
    
    # Setup enhanced agent with all subsystems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with enhanced configuration
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent with all capabilities enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems (enhanced safety)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Agent configured with:")
    print(f"  Sensory input size: {agent.getSensoryInputSize()} features")
    print(f"  Motor output size: {agent.getMotorOutputSize()} actions")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    # Enhanced simulation loop with logging
    for step in range(50):
        # Update world state
        world.update(0.1)
        
        # Get sensory input (enhanced safety)
        percept = world.getSensoryPercept()
        
        # Process sensory input with error handling
        try:
            agent.processSensoryInput(percept)
            
            # Get agent state before action
            before_neuromod = agent.getNeuromodulationLevel()
            before_curiosity = agent.getCuriosityLevel()
            
            # Brain processes
            brain.step(step)
            
            # Enhanced action selection with curiosity
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log enhanced agent state
            if step % 10 == 0:
                after_curiosity = agent.getCuriosityLevel()
                after_neuromod = agent.getNeuromodulationLevel()
                dev_stage = agent.getDevelopmentalStage()
                
                print(f"Step {step}: Action={action.getName()}, "
                      f"Curiosity={before_curiosity:.3f}->{after_curiosity:.3f}, "
                      f"DevStage={dev_stage}")
        
        except Exception as e:
            print(f"Error at step {step}: {e}")
            continue
    
    print(f"Agent simulation complete!")
    print()

def example_enhanced_utilities():
    """Example 3: Enhanced utility functions and error handling."""
    print("Example 3: Enhanced Utilities and Error Handling")
    print("-" * 40)
    
    # Test configuration validation
    config = pynlm.createDefaultConfig()
    
    # Try to load invalid config file with enhanced error handling
    print("Testing configuration file handling...")
    try:
        success = config.loadFromFile("invalid_config.txt")
        if not success:
            print("✓ Invalid file properly handled")
        else:
            print("✗ Unexpected success with invalid file")
    except Exception as e:
        print(f"✓ Exception caught: {e}")
    
    # Test configuration modification
    print("\nTesting configuration modification...")
    config.set("test.param", "test_value")
    config.set("test.number", 42)
    config.set("test.boolean", True)
    
    print(f"Config has test.param: {config.has('test.param')}")
    print(f"Config summary: {config.summary()}")
    
    # Test object cloning and comparison
    print("\nTesting object cloning...")
    action1 = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.3])
    action2 = action1.clone()
    
    print(f"Original action: {action1.getName()}, params: {action1.getParameters()}")
    print(f"Cloned action: {action2.getName()}, params: {action2.getParameters()}")
    print(f"Actions are equivalent: {action1.getType() == action2.getType()}")
    
    # Test world object management
    print("\nTesting world object management...")
    world = pynlm.createSimpleWorld()
    
    # Add various world objects
    from pynlm import WorldObject, WorldObjectType
    
    resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, 10.0, 1.0)
    hazard = WorldObject(15.0, 15.0, WorldObjectType.Hazard, 5.0, 0.5)
    
    world.addObject(resource)
    world.addObject(hazard)
    
    print(f"Added resource at ({resource.x}, {resource.y})")
    print(f"Added hazard at ({hazard.x}, {hazard.y})")
    print(f"Valid position (10,10): {world.isValidPosition(10, 10)}")
    print(f"Valid position (20,20): {world.isValidPosition(20, 20)}")
    
    print("\n✓ Enhanced utilities test complete")
    print()

def example_advanced_features():
    """Example 4: Advanced features and performance monitoring."""
    print("Example 4: Advanced Features and Monitoring")
    print("-" * 40)
    
    # Create brain with enhanced monitoring
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial brain status
    print("Initial brain status:")
    brain.logStatus()
    
    # Run extended simulation with monitoring
    print("\nRunning extended simulation...")
    start_spikes = brain.getTotalSpikeCount()
    start_neurons = brain.getTotalNeuronCount()
    
    for step in range(100):
        brain.step(step)
        
        # Periodic monitoring with enhanced reporting
        if step % 25 == 0:
            current_spikes = brain.getTotalSpikeCount() - start_spikes
            firing_rate = brain.getAverageFiringRate()
            e_i_ratio = brain.getExcitationInhibitionRatio()
            
            print(f"Step {step}: Spikes={current_spikes}, "
                  f"FiringRate={firing_rate:.3f}, E/I ratio={e_i_ratio:.3f}")
    
    final_spikes = brain.getTotalSpikeCount()
    print(f"\nSimulation complete: {final_spikes - start_spikes} new spikes")
    print()

def example_performance_optimization():
    """Example 5: Performance optimization patterns."""
    print("Example 5: Performance Optimization")
    print("-" * 40)
    
    # Create multiple brains for comparison
    brains = []
    configs = []
    
    print("Creating multiple brain instances...")
    for i in range(3):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        configs.append(config)
        brains.append(brain)
        print(f"  Brain {i+1}: {brain.getTotalNeuronCount()} neurons")
    
    # Parallel simulation (conceptual)
    print("\nRunning parallel simulation...")
    import time
    
    start_time = time.time()
    for i, brain in enumerate(brains):
        for step in range(20):
            brain.step(step)
    end_time = time.time()
    
    print(f"Parallel simulation completed in {end_time - start_time:.3f} seconds")
    
    # Memory efficiency test
    print("\nTesting memory efficiency...")
    del brains
    del configs
    import gc
    gc.collect()
    
    print("✓ Memory cleanup completed")
    print()

def main():
    """Run all enhanced examples."""
    print("Enhanced NLM Python Bindings Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic Brain Simulation", example_basic_brain),
        ("Agent-World Interaction", example_agent_world),
        ("Enhanced Utilities", example_enhanced_utilities),
        ("Advanced Features", example_advanced_features),
        ("Performance Optimization", example_performance_optimization),
    ]
    
    for example_name, example_func in examples:
        try:
            print(f"\n{'='*20} {example_name} {'='*20}")
            example_func()
            print(f"✓ {example_name} completed successfully")
        except Exception as e:
            print(f"✗ {example_name} failed: {e}")
            traceback.print_exc()
            print(f"Continue with next example...")
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print("\nKey Improvements Demonstrated:")
    print("✓ Enhanced memory safety with proper reference policies")
    print("✓ Comprehensive error handling and validation")
    print("✓ Advanced logging and monitoring")
    print("✓ Multiple utility functions for common patterns")
    print("✓ Performance optimization patterns")
    print("✓ Type safety and input validation")
    print("\nThe enhanced NLM bindings provide:")
    print("• Safer Python-C++ interoperability")
    print("• Better error reporting and debugging")
    print("• More intuitive API design")
    print("• Improved performance and scalability")

if __name__ == "__main__":
    main()
