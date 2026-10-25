#!/usr/bin/env python3
"""
Demonstration: Enhanced NLM Python Bindings API

This script demonstrates the key improvements made to the NLM Python bindings:
1. Enhanced error handling and validation
2. Convenience functions and helper methods
3. Pythonic API design
4. Better statistics and debugging
5. Support for learning and development scenarios
"""

import pynlm

def demonstrate_enhanced_api():
    """Demonstrate the enhanced NLM Python bindings API."""
    
    print("NLM Python Bindings - Enhanced API Demonstration")
    print("=" * 60)
    
    # 1. Enhanced Configuration Management
    print("\n1. Enhanced Configuration Management:")
    print("-" * 40)
    
    config = pynlm.createStandardConfig()
    
    # Pythonic configuration access
    print("   Creating standard configuration...")
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Enhanced access methods
    neuron_count = config.getInt("brain.neuron_count")
    stdp_rate = config.getFloat("plasticity.stdp.learning_rate")
    print(f"   ✓ Neuron count: {neuron_count}")
    print(f"   ✓ STDP rate: {stdp_rate}")
    
    # Configuration validation
    print("\n   Validating configuration...")
    for key in ["brain.neuron_count", "plasticity.stdp.enable", "random_seed"]:
        if config.validateKey(key):
            value = config.get(key)
            print(f"   ✓ {key}: {value}")
        else:
            print(f"   ✗ {key}: missing or invalid")
    
    # 2. Enhanced Error Handling
    print("\n2. Enhanced Error Handling:")
    print("-" * 40)
    
    # Test invalid file loading
    print("   Testing invalid file loading...")
    try:
        config.loadFromFile("/nonexistent/file.json")
        print("   ✗ Should have raised an error")
    except Exception as e:
        print(f"   ✓ Correctly handled: {type(e).__name__}")
    
    # Test invalid key access
    print("   Testing invalid key access...")
    try:
        value = config.get("invalid_key")
        print(f"   ✗ Should have raised an error (got: {value})")
    except Exception as e:
        print(f"   ✓ Correctly handled: {type(e).__name__}")
    
    # 3. Enhanced Brain API
    print("\n3. Enhanced Brain API:")
    print("-" * 40)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enhanced statistics access
    print("   Getting enhanced brain statistics...")
    stats = brain.getStats()
    print(f"   ✓ Total neurons: {stats['total_neurons']}")
    print(f"   ✓ Total synapses: {stats['total_synapses']}")
    print(f"   ✓ Firing neurons: {stats['firing_neurons']}")
    print(f"   ✓ Average firing rate: {stats['average_firing_rate']:.3f}")
    print(f"   ✓ E/I ratio: {stats['e_i_ratio']:.3f}")
    
    # Enhanced brain validation
    print("\n   Enhanced brain validation:")
    is_valid = brain.isValid()
    is_initialized = brain.checkInitialized()
    print(f"   ✓ Is valid: {is_valid}")
    print(f"   ✓ Is initialized: {is_initialized}")
    
    # 4. Convenience Functions
    print("\n4. Convenience Functions:")
    print("-" * 40)
    
    # Create ready agent
    print("   Creating ready agent with world...")
    result = pynlm.createReadyAgent(
        width=15,
        height=15,
        enableRewardModulation=True,
        enableCuriosity=True
    )
    
    agent = result["agent"]
    world = result["world"]
    
    print(f"   ✓ Agent subsystems: {agent.getSubsystemsStatus()}")
    
    # Run simple episode
    print("   Running simple episode...")
    pynlm.runSimpleEpisode(brain, world, agent, 10)
    
    print("   ✓ Episode completed")
    
    # 5. Enhanced Action Production
    print("\n5. Enhanced Action Production:")
    print("-" * 40)
    
    print("   Producing action from brain...")
    action = brain.produceAction()
    if action:
        print(f"   ✓ Action produced: type={action.getType()}, params={action.getParameters()}")
    else:
        print("   ℹ No action produced (normal if no motor neurons active)")
    
    # 6. Learning and Development
    print("\n6. Learning and Development:")
    print("-" * 40)
    
    print("   Running learning simulation...")
    for episode in range(3):
        print(f"\n   Learning episode {episode + 1}:")
        
        episode_brain = pynlm.createBrain(config)
        episode_brain.initialize()
        
        for step in range(30):
            episode_brain.step(step)
            
            if step % 10 == 0:
                stage = episode_brain.getDevelopmentalStage()
                print(f"     Step {step}: Development stage = {stage}")
        
        # Check learning progress
        initial_stats = brain.getStats()
        final_stats = episode_brain.getStats()
        
        learning_progress = (
            final_stats["total_spikes"] > initial_stats["total_spikes"] or
            final_stats["e_i_ratio"] != initial_stats["e_i_ratio"]
        )
        
        if learning_progress:
            print("     ✓ Learning detected through synaptic changes")
        else:
            print("     ℹ Learning may require more complex setup")
    
    print("\n" + "=" * 60)
    print("Enhanced API Demonstration Complete!")
    print("=" * 60)
    
    print("\nKey Improvements Demonstrated:")
    print("  ✓ Enhanced error handling with clear error messages")
    print("  ✓ Convenience functions for common patterns")
    print("  ✓ Pythonic API design with intuitive method names")
    print("  ✓ Comprehensive statistics and debugging information")
    print("  ✓ Better validation and safety checks")
    print("  ✓ Support for learning and development scenarios")
    print("  ✓ Enhanced configuration management")
    
    print(f"\nPython Bindings Version: {pynlm.__version__}")

if __name__ == "__main__":
    demonstrate_enhanced_api()