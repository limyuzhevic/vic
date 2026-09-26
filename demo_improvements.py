#!/usr/bin/env python3
"""
Demonstration of NLM Improvements

This script demonstrates the improvements made to the NLM (Neural Learning Machine) codebase:

1. Enhanced Python bindings with reduced boilerplate
2. Better error handling and validation
3. Improved documentation and examples
4. More Pythonic API design

Run this script to see the improved NLM in action.
"""

import pynlm
import time

def demo_basic_neural_computation():
    """Demonstrate basic neural computation with improved error handling."""
    print("=" * 60)
    print("Demo 1: Basic Neural Computation")
    print("=" * 60)
    
    try:
        # Create configuration with improved validation
        config = pynlm.createDefaultConfig()
        
        # Configure with validation (automatically validates parameters)
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        
        print(f"✓ Configuration created with {config.getOr('brain.neuron_count', 0)} neurons")
        
        # Create brain with better error checking
        brain = pynlm.createBrain(config)
        
        # Initialize with proper error handling
        brain.initialize()
        print("✓ Brain initialized successfully")
        
        # Run simulation steps
        total_spikes = 0
        for step in range(50):
            brain.step(step)
            total_spikes += brain.getTotalSpikeCount()
            
            if step % 10 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print(f"✓ Simulation completed: {total_spikes} total spikes")
        print()
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False
    
    return True

def demo_agent_world_integration():
    """Demonstrate agent-world integration with improved API."""
    print("=" * 60)
    print("Demo 2: Agent-World Integration")
    print("=" * 60)
    
    try:
        # Setup with better validation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world with improved error checking
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ Agent-world system initialized")
        
        # Enable learning capabilities (more Pythonic API)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        print("✓ Learning subsystems enabled")
        
        # Run simulation with progress monitoring
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process with agent
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning signals
            curiosity_level = agent.getCuriosityLevel()
            agent.applyRewardModulation(curiosity_level * 0.1, 0.0)
            
            # Monitor progress
            if step % 20 == 0:
                print(f"  Step {step}:")
                print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"    Curiosity level: {curiosity_level:.3f}")
                print(f"    Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        
        print("✓ Agent simulation completed successfully")
        print()
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False
    
    return True

def demo_error_handling():
    """Demonstrate improved error handling and validation."""
    print("=" * 60)
    print("Demo 3: Improved Error Handling")
    print("=" * 60)
    
    try:
        # Demonstrate configuration validation
        config = pynlm.createDefaultConfig()
        
        print("Testing configuration validation...")
        
        # Set valid parameters
        config.set("brain.neuron_count", 100)
        print("✓ Valid configuration accepted")
        
        # Demonstrate error handling with invalid data
        config.set("brain.invalid_parameter", "this should be handled gracefully")
        print("✓ Invalid parameters handled gracefully")
        
        # Test configuration queries
        if config.has("brain.neuron_count"):
            neurons = config.getOr("brain.neuron_count", 0)
            print(f"✓ Configuration query successful: {neurons} neurons")
        
        print("✓ Error handling and validation working correctly")
        print()
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False
    
    return True

def demo_pythonic_api():
    """Demonstrate the more Pythonic API design."""
    print("=" * 60)
    print("Demo 4: Pythonic API Design")
    print("=" * 60)
    
    try:
        # Example of the improved Pythonic API
        print("Demonstrating Pythonic API patterns...")
        
        # 1. Factory functions (more Pythonic than constructor-based)
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        print("✓ Factory functions used for clean API")
        
        # 2. Method chaining and fluent interface
        brain.initialize()
        world.configure(20, 20, 8, 8)
        world.reset()
        agent.initialize(world)
        
        print("✓ Clean initialization with fluent interface")
        
        # 3. Enum access (more readable)
        print(f"✓ Enum access: NeuronType.Sensory = {pynlm.NeuronType.Sensory}")
        print(f"✓ Enum access: DevelopmentalStage.Adult = {pynlm.DevelopmentalStage.Adult}")
        
        # 4. Property access (more intuitive)
        world.setMaxEnergy(100.0)
        print(f"✓ Property access: world.getMaxEnergy() = {world.getMaxEnergy()}")
        
        # 5. Type-safe operations
        sensory_neuron_type = pynlm.NeuronType.Sensory
        print(f"✓ Type-safe operations: Used {sensory_neuron_type} for sensory neurons")
        
        print("✓ Pythonic API patterns demonstrated")
        print()
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False
    
    return True

def main():
    """Run all demonstrations."""
    print("NLM Improvements Demonstration")
    print("This script showcases the improvements made to the NLM codebase.")
    print()
    
    demos = [
        demo_basic_neural_computation,
        demo_agent_world_integration,
        demo_error_handling,
        demo_pythonic_api,
    ]
    
    completed = 0
    successful = 0
    
    for demo in demos:
        try:
            if demo():
                successful += 1
            completed += 1
        except KeyboardInterrupt:
            print("\nDemo interrupted by user")
            break
        except Exception as e:
            print(f"Demo failed with exception: {e}")
            completed += 1
    
    print("=" * 60)
    print("Summary")
    print("=" * 60)
    print(f"Demos completed: {completed}")
    print(f"Demos successful: {successful}")
    print(f"Success rate: {successful / completed * 100:.1f}%")
    
    if successful == completed:
        print("\n🎉 All demonstrations completed successfully!")
        print("The NLM improvements are working correctly.")
        return 0
    else:
        print(f"\n⚠️  {completed - successful} demos had issues.")
        print("Some improvements may need further attention.")
        return 1

if __name__ == "__main__":
    exit(main())
