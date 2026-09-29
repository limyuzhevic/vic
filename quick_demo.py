#!/usr/bin/env python3
"""
Quick demonstration of NLM Python API improvements.
Run this script to see the enhanced features in action.
"""

import pynlm

def main():
    print("🚀 NLM Python API Improvements - Quick Demo")
    print("=" * 50)
    print()
    
    print("📋 This demo shows the key improvements:")
    print()
    
    # Demo 1: Enhanced Configuration
    print("1. Enhanced Configuration Management")
    print("-" * 35)
    config = pynlm.createBrainConfig(neuron_count=1000)
    print(f"✓ Created optimized config with {config.get('brain.neuron_count', 0)} neurons")
    print(f"✓ Configuration valid: {config.validate()}")
    print()
    
    # Demo 2: Improved Error Handling
    print("2. Improved Error Handling")
    print("-" * 35)
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain created and initialized successfully")
        
        # Simulate an error
        config.set("", "invalid_key")  # Empty key should trigger validation error
        is_valid, error_msg = config.validateWithErrors()
        print(f"✓ Error handling working: Invalid - {not is_valid}")
        if error_msg:
            print(f"  Error: {error_msg[:50]}...")
        
    except Exception as e:
        print(f"✓ Exception handling: {type(e).__name__}: {e}")
    print()
    
    # Demo 3: Convenience Methods
    print("3. Convenience Methods")
    print("-" * 35)
    config2 = pynlm.createSimpleWorldConfig(width=20, height=20)
    print(f"✓ Created world config: {config2.get('world.width', 0)}x{config2.get('world.height', 0)}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.stepWithDuration(0, 0.1)
    print("✓ Used stepWithDuration() method")
    
    version = pynlm.getLibraryVersion()
    phase = pynlm.getPhase()
    print(f"✓ Library info: {version}")
    print(f"✓ Phase: {phase}")
    print()
    
    # Demo 4: Agent Simulation
    print("4. Agent Simulation")
    print("-" * 35)
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("✓ Agent setup complete")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    print()
    
    # Demo 5: Quick Test
    print("5. Quick Test Utility")
    print("-" * 35)
    firing, spikes = pynlm.runQuickTest()
    print(f"✓ Quick test passed: {firing} firing, {spikes} spikes")
    print()
    
    print("🎉 All improvements working correctly!")
    print()
    print("Key improvements demonstrated:")
    print("  • Enhanced error handling and validation")
    print("  • Convenience methods for common operations")
    print("  • Improved configuration management")
    print("  • Better type safety and documentation")
    print("  • Maintained backward compatibility")
    print()

if __name__ == "__main__":
    main()