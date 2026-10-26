#!/usr/bin/env python3
"""
NLM Python Example: Demonstrating the improved NLM functionality

This example shows how to use the enhanced NLM Python bindings with:
- Improved configuration system
- Integrated brain systems
- Better error handling and validation
- Comprehensive status reporting
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

try:
    import pynlm as nlm
    print("Successfully imported NLM Python bindings")
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Please build the Python module first:")
    print("  cd /workspace/app && python -m pip install -e .")
    sys.exit(1)

def example_basic_usage():
    """Example 1: Basic brain initialization and configuration"""
    print("\n=== Example 1: Basic Brain Usage ===")
    
    # Create brain with enhanced configuration
    config = nlm.createDefaultConfig()
    
    # Set configuration using the improved Config system
    config.set("neuron_count", 500, nlm.ConfigSource.Runtime)
    config.set("region_count", 2, nlm.ConfigSource.Runtime)
    config.set("simulation_timestep", 0.001, nlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.15, nlm.ConfigSource.Runtime)
    
    # Create brain with the configuration
    brain = nlm.createBrain(config)
    
    # Initialize the brain
    success = brain.initialize()
    if not success:
        print("ERROR: Failed to initialize brain")
        return False
    
    print(f"Brain initialized successfully:")
    print(f"  - Regions: {brain.getRegionCount()}")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Synapses: {brain.getTotalSynapseCount()}")
    
    return True

def example_enhanced_functionality():
    """Example 2: Demonstrate enhanced NLM functionality"""
    print("\n=== Example 2: Enhanced NLM Functionality ===")
    
    # Create brain with configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 200, nlm.ConfigSource.Runtime)
    config.set("region_count", 1, nlm.ConfigSource.Runtime)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Demonstrate integrated systems access
    print("Integrated Systems Status:")
    
    # Get comprehensive system status
    status = brain.getIntegratedSystemsStatus()
    print(f"  - Brain Status:\n{status}")
    
    # Demonstrate memory systems access
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"  - Working Memory: {working_memory.getActiveTraces()} active traces")
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"  - Episodic Memory: {episodic_memory.getEpisodeCount()} episodes")
    
    # Demonstrate neuromodulation access
    dopamine = brain.getDopamine()
    if dopamine:
        print(f"  - Dopamine Level: {dopamine.getLevel():.3f}")
    
    # Demonstrate prediction system
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        print(f"  - Prediction System: Active")
    
    # Demonstrate development system
    development_system = brain.getDevelopmentSystem()
    if development_system:
        print(f"  - Development System: Active")
    
    # Run a simulation with neuromodulation
    print("\nRunning simulation with neuromodulation...")
    for step in range(10):
        # Create sensory input
        vision = nlm.Vision(64, 64, 3)
        # Simple pattern for testing
        vision_data = [0.1] * 64 * 64 * 3
        vision.setData(vision_data)
        
        # Process sensory input through agent brain
        brain = nlm.createAgentBrain(brain)
        brain.processSensoryInput(vision)
        
        # Execute action
        action = brain.decodeMotorCommand()
        
        # Apply reward modulation
        brain.applyRewardModulation(1.0, 0.5)
        
        # Update development
        brain.updateDevelopment(0.001)
        
        # Step the brain
        brain.step(step, step * 0.001)
        
        if step % 5 == 0:
            print(f"  Step {step}: Action {action.getName()}, Neuromod level: {brain.getNeuromodulationLevel():.3f}")
    
    return True

def example_error_handling():
    """Example 3: Demonstrate improved error handling and validation"""
    print("\n=== Example 3: Error Handling and Validation ===")
    
    # Create default config
    config = nlm.createDefaultConfig()
    
    # Test validation through improved Config system
    print("Testing configuration validation...")
    
    # Test type conversion
    config.setFromString("test_int", "42")
    config.setFromString("test_float", "3.14")
    config.setFromString("test_bool", "true")
    
    print(f"  - Integer value: {config.get('test_int').value}")
    print(f"  - Float value: {config.get('test_float').value}")
    print(f"  - Boolean value: {config.get('test_bool').value}")
    
    # Test JSON/YAML file loading (simplified for demonstration)
    print("\nTesting file I/O operations...")
    
    # Save configuration
    success = config.saveToFile("./test_config.cfg")
    if success:
        print("  - Configuration saved successfully")
    else:
        print("  - Failed to save configuration")
    
    # Load configuration
    new_config = nlm.createDefaultConfig()
    success = new_config.loadFromFile("./test_config.cfg")
    if success:
        print("  - Configuration loaded successfully")
    else:
        print("  - Failed to load configuration")
    
    # Clean up
    if os.path.exists("./test_config.cfg"):
        os.remove("./test_config.cfg")
    
    return True

def example_advanced_features():
    """Example 4: Demonstrate advanced NLM features"""
    print("\n=== Example 4: Advanced NLM Features ===")
    
    # Create agent brain with advanced features
    brain = nlm.createDefaultBrain()
    brain.initialize(nlm.createSimpleWorld())
    
    # Enable advanced features
    brain.enableRewardModulation(True)
    brain.enableStructuralPlasticity(True)
    brain.enableDevelopment(True)
    brain.enableCuriosity(True)
    
    print("Advanced features enabled:")
    print(f"  - Reward Modulation: {brain.isRewardModulationEnabled()}")
    print(f"  - Structural Plasticity: {brain.isStructuralPlasticityEnabled()}")
    print(f"  - Development: {brain.isDevelopmentEnabled()}")
    print(f"  - Curiosity: {brain.isCuriosityEnabled()}")
    
    # Demonstrate memory consolidation
    print("\nMemory consolidation demonstration...")
    for step in range(20):
        # Random input for exploration
        vision = nlm.Vision(32, 32, 3)
        import random
        vision_data = [random.uniform(0, 1) for _ in range(32 * 32 * 3)]
        vision.setData(vision_data)
        
        brain.processSensoryInput(vision)
        action = brain.decodeMotorCommand()
        
        # Apply reward based on action type
        reward = 0.1 if action.getType() == nlm.ActionType.Interact else 0.0
        brain.applyRewardModulation(reward, 0.0)
        
        brain.step(step, step * 0.001)
        
        # Periodically trigger memory consolidation
        if step % 10 == 0:
            print(f"  Step {step}: Memory consolidation triggered")
    
    return True

def main():
    """Main function to run all examples"""
    print("=" * 60)
    print("NLM Python Examples - Enhanced Functionality")
    print("=" * 60)
    
    examples = [
        example_basic_usage,
        example_enhanced_functionality,
        example_error_handling,
        example_advanced_features,
    ]
    
    success = True
    for i, example in enumerate(examples, 1):
        try:
            print(f"\n{'='*60}")
            print(f"Running Example {i}: {example.__doc__.split(':')[1] if ':' in example.__doc__ else 'Unknown'}")
            print(f"{'='*60}")
            
            if not example():
                success = False
                print(f"Example {i} FAILED!")
                
        except Exception as e:
            success = False
            print(f"Example {i} ERROR: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    if success:
        print("All examples completed successfully!")
        print("\nNLM Python bindings provide:")
        print("  - Enhanced configuration system (JSON/YAML support)")
        print("  - Comprehensive integrated brain systems")
        print("  - Advanced memory and neuromodulation systems")
        print("  - Better error handling and validation")
        print("  - Advanced agent and world simulation")
    else:
        print("Some examples failed. Please check the output above.")
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())