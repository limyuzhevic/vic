"""
NLM (Neural Learning Machine) - Python Usage Examples

Comprehensive examples demonstrating the improved NLM Python bindings.

This file shows how to use the enhanced NLM features including:
- Basic functionality
- Factory functions
- Configuration management
- Agent workflows
- Advanced features for power users

For beginners, see: easy_usage.md
For complete documentation, see: docs/ARCHITECTURE.md
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

import pynlm
import time

def example_1_basic_brain():
    """Example 1: Basic brain simulation"""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation for 100 steps
    for i in range(100):
        brain.step(i)
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def example_2_factory_functions():
    """Example 2: Using factory functions"""
    print("=== Example 2: Factory Functions ===")
    
    # Create test configuration (optimized for testing)
    test_config = pynlm.createTestConfig()
    print("Test config created:")
    print(f"  - Neuron count: {test_config.get('brain.neuron_count')}")
    print(f"  - Synapse density: {test_config.get('brain.synapse_density')}")
    print(f"  - STDP enabled: {test_config.get('plasticity.stdp.enable')}")
    print()
    
    # Create simulation configuration
    sim_config = pynlm.createSimulationConfig(neuron_count=500, connection_prob=0.05)
    print("Simulation config created:")
    print(f"  - Neuron count: {sim_config.get('brain.neuron_count')}")
    print(f"  - Connection probability: {sim_config.get('brain.connection_probability')}")
    print()

def example_3_dict_based_config():
    """Example 3: Dictionary-based configuration"""
    print("=== Example 3: Dictionary-Based Configuration ===")
    
    # Create config from Python dictionary
    config_dict = {
        "brain.neuron_count": 200,
        "brain.connection_probability": 0.15,
        "plasticity.learning_rate": 0.02,
        "neuromod.dopamine.scale": 1.2,
        "development.synaptogenesis_rate": 0.001,
        "environment.width": 10,
        "environment.height": 10,
        "visualization.enabled": False
    }
    
    config = pynlm.createConfigFromDict(config_dict)
    print("Configuration created from dictionary:")
    for key in sorted(config.getKeys()):
        value = config.get(key)
        print(f"  {key}: {value}")
    print()
    
    # Create brain from dictionary
    brain = pynlm.createBrainFromDict(config_dict)
    brain.initialize()
    print(f"Brain created from dict with {brain.getTotalNeuronCount()} neurons")
    print()

def example_4_agent_workflow():
    """Example 4: Complete agent workflow"""
    print("=== Example 4: Complete Agent Workflow ===")
    
    # Setup environment
    print("Creating agent with world...")
    brain = pynlm.createTestConfig()
    brain = pynlm.createBrain(brain)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgent(brain, world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running simulation with learning...")
    for step in range(50):
        # Update environment
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks (processes)
        brain.step(step)
        
        # Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # Do the action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation for learning
        agent.applyRewardModulation(1.0, 0.8)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Show final stats
    print(f"\nFinal agent state:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  - Development stage: {agent.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
    print()

def example_5_pythonic_interfaces():
    """Example 5: Pythonic interfaces and properties"""
    print("=== Example 5: Pythonic Interfaces ===")
    
    # Create configuration with property access
    config = pynlm.createDefaultConfig()
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    config.set("epochs", 100)
    config.set("verbose", True)
    
    print("Configuration with property-style access:")
    print(f"  - Keys: {list(config.keys())}")
    print(f"  - Is empty: {config.is_empty}")
    print(f"  - Contains 'learning_rate': {'learning_rate' in config}")
    print(f"  - learning_rate value: {config['learning_rate']}")
    print(f"  - batch_size value: {config.get('batch_size', 0)}")
    print(f"  - Missing key with default: {config.get('missing_key', 'default_value')}")
    
    # Test iteration
    print("\nIterating over config:")
    for key in config:
        print(f"  {key}: {config[key]}")
    
    # Test brain state as dictionary
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("\nBrain state as dictionary:")
    brain_state = brain.to_dict()
    for key, value in brain_state.items():
        print(f"  {key}: {value}")
    print()

def example_6_batch_operations():
    """Example 6: Batch operations for performance"""
    print("=== Example 6: Batch Operations ===")
    
    config = pynlm.createDefaultConfig()
    
    # Create large batch of configuration
    print("Creating batch of 1000 configuration values...")
    batch_data = {}
    for i in range(1000):
        batch_data[f"param_{i}"] = i * 0.1
    
    # Batch set (efficient)
    pynlm.batch_set(config, batch_data)
    print(f"  - Set {len(config.getKeys())} configuration values")
    
    # Sample some values
    sample_keys = ["param_0", "param_500", "param_999", "param_100"]
    print("  Sample values:")
    for key in sample_keys:
        if key in config:
            print(f"    {key}: {config[key]}")
    
    # Batch get for performance
    get_keys = [f"param_{i}" for i in [0, 100, 500, 999]]
    print(f"\nBatch get of {len(get_keys)} keys:")
    batch_result = pynlm.batch_get(config, get_keys)
    for key, value in batch_result.items():
        print(f"  {key}: {value}")
    print()

def example_7_error_handling():
    """Example 7: Error handling and safety features"""
    print("=== Example 7: Error Handling and Safety Features ===")
    
    # Create config and test error handling
    config = pynlm.createDefaultConfig()
    
    # Test safe load with non-existent file
    print("Testing safe config loading...")
    result = pynlm.safe_load_config(config, "nonexistent_file.cfg")
    print(f"  - Load non-existent file: {'Failed (expected)' if not result else 'Unexpected success'}")
    
    # Test safe save
    print("\nTesting safe config saving...")
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        temp_file = f.name
    
    try:
        config.set("test.key", "value")
        result = pynlm.safe_save_config(config, temp_file)
        print(f"  - Save config to file: {'Success' if result else 'Failed'}")
        
        if result:
            print(f"  - File exists: {os.path.exists(temp_file)}")
    finally:
        if os.path.exists(temp_file):
            os.unlink(temp_file)
    
    # Test config validation
    print("\nTesting config validation...")
    valid_config = pynlm.createDefaultConfig()
    valid_config.set("valid_key", 100)
    print(f"  - Valid config: {'Pass' if pynlm.validate_config(valid_config) else 'Fail'}")
    
    # Note: We can't easily test invalid config without accessing internal structures
    print("  - Invalid config testing requires internal access")
    print()

def example_8_advanced_agent_features():
    """Example 8: Advanced agent features"""
    print("=== Example 8: Advanced Agent Features ===")
    
    # Create sophisticated agent setup
    print("Creating advanced agent with multiple features...")
    
    # Advanced configuration
    advanced_config = {
        "brain.neuron_count": 300,
        "brain.connection_probability": 0.2,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "neuromod.dopamine.scale": 2.0,
        "neuromod.curiosity.threshold": 0.5,
        "neuromod.novelty.sensitivity": 0.3,
        "development.synaptogenesis_rate": 0.002,
        "development.pruning_rate": 0.0001,
        "environment.width": 20,
        "environment.height": 20,
        "environment.max_energy": 100.0,
        "environment.energy_decay_rate": 0.1
    }
    
    brain = pynlm.createBrainFromDict(advanced_config)
    brain.initialize()
    
    world = pynlm.createWorldFromConfig(advanced_config)
    world.configure(
        advanced_config.get("environment.width", 10),
        advanced_config.get("environment.height", 10),
        10, 10  # vision dimensions
    )
    
    agent = pynlm.createAgent(brain, world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"  - Agent created with {brain.getTotalNeuronCount()} neurons")
    print(f"  - All learning features enabled:")
    print(f"    * Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"    * Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"    * Development: {agent.isDevelopmentEnabled()}")
    print(f"    * Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run with complex stimuli
    print("\nRunning complex simulation...")
    for step in range(30):
        world.update(0.05)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on performance
        reward = 1.0 if step % 7 == 0 else 0.5
        agent.applyRewardModulation(reward, reward * 0.8)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons active")
    
    print(f"\nAdvanced agent completed:")
    print(f"  - Development stage: {agent.getDevelopmentalStage()}")
    print(f"  - Neuromodulation: {agent.getNeuromodulationLevel():.2f}")
    print(f"  - Curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"  - Novelty: {agent.getNoveltyLevel():.2f}")
    print(f"  - Prediction error: {agent.getPredictionError():.2f}")
    print()

def main():
    """Run all examples"""
    print("=" * 60)
    print("NLM Python Bindings - Usage Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic Brain Simulation", example_1_basic_brain),
        ("Factory Functions", example_2_factory_functions),
        ("Dictionary-Based Configuration", example_3_dict_based_config),
        ("Complete Agent Workflow", example_4_agent_workflow),
        ("Pythonic Interfaces", example_5_pythonic_interfaces),
        ("Batch Operations", example_6_batch_operations),
        ("Error Handling", example_7_error_handling),
        ("Advanced Agent Features", example_8_advanced_agent_features),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print()
    print("For more information:")
    print("- easy_usage.md: Beginner-friendly guide")
    print("- docs/ARCHITECTURE.md: Technical architecture")
    print("- tests/test_python_bindings.py: Comprehensive test suite")
    print("- README.md: Project overview")

if __name__ == "__main__":
    import tempfile
    main()
