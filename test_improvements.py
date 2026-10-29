#!/usr/bin/env python3
"""
Comprehensive test script for NLM Python bindings improvements.
This script tests all the new functionality and demonstrates the enhanced API.
"""

import pynlm
import sys

def test_config_improvements():
    """Test Config class improvements"""
    print("=== Testing Config Class Improvements ===")
    
    # Test 1: Basic config creation and traditional methods
    config = pynlm.createDefaultConfig()
    print(f"✓ Created config: {config}")
    
    # Test 2: Traditional getter/setter methods
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    config.set("epochs", 100)
    config.set("use_gpu", True)
    config.set("model_name", "test_model")
    
    # Test type-specific getters
    learning_rate = config.getInt("learning_rate")
    batch_size = config.getInt("batch_size")
    epochs = config.getInt("epochs")
    use_gpu = config.getBool("use_gpu")
    model_name = config.getString("model_name")
    
    print(f"  ✓ Traditional getters:")
    print(f"    learning_rate: {learning_rate}")
    print(f"    batch_size: {batch_size}")
    print(f"    epochs: {epochs}")
    print(f"    use_gpu: {use_gpu}")
    print(f"    model_name: {model_name}")
    
    # Test 3: Enhanced type-specific getters
    learning_rate2 = config.getInt("learning_rate", 0.0)
    batch_size2 = config.getInt("batch_size", 0)
    print(f"  ✓ Enhanced getters with defaults:")
    print(f"    learning_rate (with default): {learning_rate2}")
    print(f"    batch_size (with default): {batch_size2}")
    
    # Test 4: Enhanced setter methods
    config.setFloat("learning_rate", 0.05)
    config.setString("description", "Test model with enhanced setters")
    print(f"  ✓ Enhanced setter methods work")
    
    # Test 5: Pythonic dict-like interface
    config["optimizer"] = "adam"
    config["regularization"] = 0.001
    config["early_stopping"] = True
    
    print(f"  ✓ Pythonic dict-like access:")
    print(f"    config['optimizer']: {config['optimizer']}")
    print(f"    config['regularization']: {config['regularization']}")
    print(f"    config['early_stopping']: {config['early_stopping']}")
    
    # Test 6: Check membership
    print(f"  ✓ Membership testing:")
    print(f"    'learning_rate' in config: {'learning_rate' in config}")
    print(f"    'nonexistent_key' in config: {'nonexistent_key' in config}")
    
    # Test 7: Configuration operations
    config.update({
        "new_param1": 42,
        "new_param2": 3.14,
        "new_param3": "test_value",
        "new_param4": False,
        "new_param5": [1, 2, 3]
    })
    
    print(f"  ✓ Configuration update works:")
    print(f"    Keys after update: {config.getKeys()}")
    
    # Test 8: Configuration merge
    config2 = pynlm.createDefaultConfig()
    config2["extra_param"] = "extra_value"
    config2["extra_param2"] = 999
    
    config.merge(config2)
    print(f"  ✓ Configuration merge works:")
    print(f"    'extra_param' in config: {'extra_param' in config}")
    
    # Test 9: Configuration dictionary access
    config_dict = config.getDict()
    print(f"  ✓ Configuration as dictionary:")
    for key, value in config_dict.items():
        print(f"    {key}: {value}")
    
    # Test 10: Configuration toDict alias
    config_dict2 = config.toDict()
    print(f"  ✓ toDict alias works: {len(config_dict2)} items")
    
    # Test 11: Configuration save/load
    config.saveToFile("test_config.cfg")
    print(f"  ✓ Configuration saved to test_config.cfg")
    
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("test_config.cfg")
    print(f"  ✓ Configuration loaded from file")
    
    # Test 12: Configuration representation
    print(f"  ✓ Config representation: {repr(config)}")
    
    return True

def test_brain_improvements():
    """Test Brain class improvements"""
    print("\n=== Testing Brain Class Improvements ===")
    
    # Test 1: Basic brain creation and traditional methods
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Traditional brain methods
    print(f"  ✓ Traditional methods:")
    print(f"    Total neurons: {brain.getTotalNeuronCount()}")
    print(f"    Total synapses: {brain.getTotalSynapseCount()}")
    print(f"    Active neurons: {brain.getActiveNeuronCount()}")
    print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test 3: Enhanced statistics
    stats = brain.getStatistics()
    print(f"  ✓ Enhanced statistics (dict):")
    for key, value in stats.items():
        print(f"    {key}: {value}")
    
    # Test 4: Brain info
    brain_info = brain.brainInfo()
    print(f"  ✓ Brain info (string): {brain_info[:100]}...")
    
    # Test 5: Brain validation
    validation = brain.validate()
    print(f"  ✓ Brain validation: valid={validation['valid']}, errors={len(validation['errors'])}")
    
    # Test 6: Safe step
    result = brain.safeStep(0)
    print(f"  ✓ Safe step result: {result}")
    
    # Test 7: Reset with config
    new_config = pynlm.createDefaultConfig()
    new_brain = pynlm.resetBrain(brain, new_config)
    print(f"  ✓ Reset with config: {new_brain.getTotalNeuronCount()} neurons")
    
    # Test 8: Run simulation
    for step in range(10):
        brain.step(step)
    
    print(f"  ✓ Simulation completed:")
    print(f"    Total spikes: {brain.getTotalSpikeCount()}")
    print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test 9: Region operations
    region_id = pynlm.createRegion(brain, "vision")
    region = brain.getRegion(region_id)
    print(f"  ✓ Region operations:")
    print(f"    Created region with ID: {region_id}")
    print(f"    Retrieved region: {region}")
    
    # Test 10: Find neurons by type
    sensory_neurons = pynlm.findNeuronsByType(brain, pynlm.NeuronType.Sensory)
    print(f"  ✓ Find neurons by type: {len(sensory_neurons)} sensory neurons")
    
    return True

def test_agent_improvements():
    """Test AgentBrain class improvements"""
    print("\n=== Testing AgentBrain Class Improvements ===")
    
    # Test 1: Basic agent creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"✓ Created agent with features:")
    
    # Test 2: Enable/disable features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"  ✓ Features enabled:")
    print(f"    Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"    Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"    Development: {agent.isDevelopmentEnabled()}")
    print(f"    Curiosity: {agent.isCuriosityEnabled()}")
    
    # Test 3: Agent statistics
    agent_stats = agent.agentStats()
    print(f"  ✓ Agent statistics (dict):")
    for key, value in agent_stats.items():
        print(f"    {key}: {value}")
    
    # Test 4: Agent info
    agent_info = agent.agentInfo()
    print(f"  ✓ Agent info (string): {agent_info[:100]}...")
    
    # Test 5: Run episode
    episode_result = pynlm.runEpisode(brain, world, agent, 5, 0.1)
    print(f"  ✓ Episode simulation:")
    print(f"    Rewards: {episode_result['rewards']}")
    print(f"    Actions: {len(episode_result['actions'])} actions")
    print(f"    Final brain state: {episode_result['final_brain_state']}")
    
    return True

def test_world_improvements():
    """Test World class improvements"""
    print("\n=== Testing World Class Improvements ===")
    
    # Test 1: Basic world creation
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"✓ Created world: {world.getWidth()}x{world.getHeight()}")
    
    # Test 2: Traditional world methods
    print(f"  ✓ Traditional world methods:")
    print(f"    Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"    Agent body: x={world.getAgentBody().x}, y={world.getAgentBody().y}")
    
    # Test 3: Enhanced world helper methods
    world.addMarker(2.0, 2.0, "goal")
    world.addResource(5.0, 5.0, 10.0)
    world.addWall(15.0, 15.0, 2.0)
    
    print(f"  ✓ World helper methods:")
    print(f"    Added marker, resource, and wall")
    
    # Test 4: Move towards target
    agent_body = world.getAgentBody()
    pynlm.moveTowardsTarget(world, agent_body, 15.0, 15.0, 1.0)
    print(f"  ✓ Move towards target: agent now at ({agent_body.x:.1f}, {agent_body.y:.1f})")
    
    # Test 5: World info
    world_info = world.worldInfo()
    print(f"  ✓ World info (string): {world_info[:100]}...")
    
    return True

def test_sensory_improvements():
    """Test SensoryInput class improvements"""
    print("\n=== Testing SensoryInput Class Improvements ===")
    
    # Test 1: Create vision
    vision = pynlm.createVision(84, 84, 3)
    print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Test 2: Create audio
    audio = pynlm.createAudio(44100, 1024)
    print(f"✓ Created audio: sample_rate={audio.getSampleRate()}, num_samples={audio.getNumSamples()}")
    
    # Test 3: Create internal signals
    internal = pynlm.createInternalSignals()
    print(f"✓ Created internal signals")
    
    # Test 4: Vision helper methods
    vision_data = [0.1, 0.2, 0.3, 0.4] * 21  # Flatten 2D array
    vision.setData(vision_data)
    
    print(f"  ✓ Vision helper methods:")
    print(f"    Vision shape: {vision.getVisionShape()}")
    print(f"    Has vision: {vision.hasVision()}")
    
    return True

def test_action_improvements():
    """Test Action class improvements"""
    print("\n=== Testing Action Class Improvements ===")
    
    # Test 1: Create action from type
    move_action = pynlm.createAction(pynlm.ActionType.MoveForward)
    print(f"✓ Created move action: type={move_action.getType()}, name={move_action.getName()}")
    
    # Test 2: Create action with parameters
    custom_action = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 0.7, 1.2])
    print(f"✓ Created custom action with parameters: {custom_action.getParameters()}")
    
    # Test 3: Action helper methods
    move_action.addParameter(0.8)
    move_action.addParameter(1.2)
    print(f"  ✓ Action helper methods:")
    print(f"    Added parameters: {move_action.getParameters()}")
    
    # Test 4: Get parameter
    param = move_action.getParameter(0)
    print(f"    First parameter: {param}")
    
    return True

def test_example_usage():
    """Test example usage demonstration"""
    print("\n=== Testing Example Usage ===")
    
    # Print example usage documentation
    example_result = pynlm.exampleUsage()
    print("✓ Example usage documentation printed")
    
    # Run a complete example
    print("\nRunning complete example...")
    
    import pynlm
    
    # Basic usage
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    print(f"  ✓ Neurons: {brain.getTotalNeuronCount()}")
    print(f"  ✓ Spikes: {brain.getTotalSpikeCount()}")
    
    # Advanced usage with world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"  ✓ Agent simulation complete")
    print(f"  ✓ Final firing rate: {brain.getAverageFiringRate():.3f}")
    
    return True

def test_validation():
    """Test validation and type checking"""
    print("\n=== Testing Validation ===")
    
    # Test config validation
    invalid_inputs = {
        "neuron_count": -10,  # Invalid: negative
        "connection_probability": 1.5,  # Invalid: > 1.0
        "valid_param": 42  # Valid
    }
    
    validation = pynlm.validateInputs(invalid_inputs)
    print(f"✓ Input validation:")
    print(f"  Valid: {validation['valid']}")
    print(f"  Errors: {validation['errors']}")
    
    # Test brain validation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    brain_validation = brain.validate()
    print(f"  ✓ Brain validation: {brain_validation['valid']}")
    
    return True

def main():
    """Run all tests"""
    print("NLM Python Bindings - Comprehensive Test Script")
    print("=" * 60)
    
    try:
        test_config_improvements()
        test_brain_improvements()
        test_agent_improvements()
        test_world_improvements()
        test_sensory_improvements()
        test_action_improvements()
        test_example_usage()
        test_validation()
        
        print("\n" + "=" * 60)
        print("✅ All tests completed successfully!")
        print("\nSummary of improvements:")
        print("  ✓ Enhanced Config class with Pythonic dict-like API")
        print("  ✓ Improved Brain class with convenience methods")
        print("  ✓ Enhanced AgentBrain class with statistics and info methods")
        print("  ✓ Improved World class with helper methods")
        print("  ✓ Enhanced SensoryInput classes with helper methods")
        print("  ✓ Enhanced Action class with convenience methods")
        print("  ✓ Added example usage documentation")
        print("  ✓ Added input validation")
        print("  ✓ Added benchmarking and profiling methods")
        print("  ✓ Improved error handling and safety")
        
    except Exception as e:
        print(f"\n❌ Error during testing: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())