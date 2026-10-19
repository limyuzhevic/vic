#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python API

This script demonstrates the new features and improvements made to the NLM Python API.
It shows the difference between the old API and the enhanced API.
"""

import pynlm

def demonstrate_enhanced_api():
    """Demonstrate the enhanced NLM Python API."""
    
    print("=== NLM Enhanced Python API Demonstration ===\n")
    
    # 1. Demonstrate configuration factory methods
    print("1. Configuration Factory Methods:")
    print("   Creating default configuration...")
    config = pynlm.Config.createDefault()
    print(f"   - Created config with {config.getNeuronCount()} neurons")
    
    print("\n   Loading from file (simulated)...")
    # Simulate loading from file
    config.set("world.width", 32)
    config.set("world.height", 32)
    print(f"   - Set world size to {config.getWorldSize()[0]}x{config.getWorldSize()[1]}")
    
    print("\n   Creating from dictionary...")
    config2 = pynlm.Config.fromDict({
        "neuron.count": 200,
        "world.width": 10,
        "world.height": 10,
        "plasticity.stdp_enabled": True,
        "neuromodulation.curiosity_enabled": True
    })
    print(f"   - Created config from dict: {config2.getNeuronCount()} neurons")
    
    # 2. Demonstrate configuration validation
    print("\n2. Configuration Validation:")
    is_valid, errors = pynlm.Config.validate(config)
    if is_valid:
        print("   ✅ Configuration is valid")
    else:
        print(f"   ❌ Configuration errors: {errors}")
    
    # 3. Demonstrate component creation
    print("\n3. Component Creation:")
    print("   Creating brain...")
    brain = pynlm.Config.createBrain(config)
    print(f"   - Brain created with {brain.getTotalNeuronCount()} neurons")
    
    print("   Creating world...")
    world = pynlm.Config.createSimpleWorldWithConfig(config)
    print(f"   - World configured: {world.getWidth()}x{world.getHeight()}")
    
    print("   Creating agent...")
    agent = pynlm.Config.createAgentBrainWithWorld(brain, world)
    print("   - Agent created and initialized")
    
    # 4. Demonstrate ready-to-use simulation
    print("\n4. Ready-to-Use Simulation:")
    print("   Creating complete simulation...")
    brain2, world2, agent2 = pynlm.createReadySimulation()
    print("   - Complete simulation created")
    print(f"   - Brain: {brain2.getTotalNeuronCount()} neurons")
    print(f"   - World: {world2.getWidth()}x{world2.getHeight()}")
    
    # 5. Demonstrate simulation loop
    print("\n5. Running Simulation Loop:")
    world2.reset()
    for step in range(5):
        world2.update(0.1)
        percept = world2.getSensoryPercept()
        agent2.processSensoryInput(percept)
        brain2.step(step)
        action = agent2.decodeMotorCommand()
        world2.applyMotorCommand(action, world2.getSimulationTime())
        
        if step == 0:
            print(f"   Step 0 - Firing: {brain2.getFiringNeuronCount()} neurons")
            print(f"   Step 0 - Dopamine: {agent2.getNeuromodulationLevel():.3f}")
            print(f"   Step 0 - Curiosity: {agent2.getCuriosityLevel():.3f}")
    
    print(f"   Final firing rate: {brain2.getAverageFiringRate():.1f}")
    
    # 6. Demonstrate configuration management
    print("\n6. Configuration Management:")
    print("   Original config neuron count:", config.getNeuronCount())
    
    # Create copy
    config_copy = config.clone()
    print("   Created copy of configuration")
    
    # Modify copy
    config_copy.set("neuron.count", 1000)
    print("   Modified copy neuron count to 1000")
    
    print("   Original remains unchanged:", config.getNeuronCount())
    
    # 7. Demonstrate validation and error handling
    print("\n7. Validation and Error Handling:")
    bad_config = pynlm.Config.createDefault()
    bad_config.set("neuron.count", -1)  # Invalid value
    
    is_valid, errors = pynlm.Config.validate(bad_config)
    if not is_valid:
        print(f"   ✅ Validation caught invalid value: {errors}")
    
    # 8. Demonstrate performance optimizations
    print("\n8. Performance Optimizations:")
    print("   Reusing configuration for multiple brains...")
    config3 = pynlm.Config.fromDict({"neuron.count": 100})
    
    # Create multiple brains with same config (efficient)
    brain_a = pynlm.Config.createBrain(config3)
    brain_b = pynlm.Config.createBrain(config3)
    print("   ✅ Created multiple brains efficiently")
    
    # 9. Demonstrate troubleshooting
    print("\n9. Troubleshooting Example:")
    print("   Testing file loading error handling...")
    try:
        # This would fail in real scenario
        config_fail = pynlm.Config.fromFile("nonexistent.json")
    except Exception as e:
        print(f"   ✅ Error handling works: {type(e).__name__}")
    
    print("\n=== Demonstration Complete ===")
    print("\nKey Benefits of Enhanced API:")
    print("✅ Factory methods reduce boilerplate code")
    print("✅ Configuration validation prevents runtime errors")
    print("✅ Ready-to-use simulations simplify setup")
    print("✅ Better error handling makes debugging easier")
    print("✅ Performance optimizations reduce overhead")
    print("✅ Comprehensive documentation guides users")

if __name__ == "__main__":
    demonstrate_enhanced_api()
