#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings
"""

import pynlm
import sys

def test_basic_functionality():
    """Test basic functionality of improved bindings"""
    print("Testing basic functionality...")
    
    # Test 1: Create default config
    print("1. Testing createDefaultConfig...")
    config = pynlm.createDefaultConfig()
    print(f"   Config created: {config}")
    print(f"   Default keys: {config.getKeys()}")
    
    # Test 2: Create brain with default config
    print("2. Testing createBrain with default config...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized: {brain}")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    # Test 3: Test brain step
    print("3. Testing brain.step...")
    for i in range(10):
        brain.step(i)
    print(f"   After 10 steps - Total spikes: {brain.getTotalSpikeCount()}")
    
    # Test 4: Test world creation
    print("4. Testing createSimpleWorld...")
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    print(f"   World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"   Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Test 5: Test agent creation
    print("5. Testing createAgentBrain...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print(f"   Agent initialized: {agent}")
    print(f"   Sensory input size: {agent.getSensoryInputSize()}")
    print(f"   Motor output size: {agent.getMotorOutputSize()}")
    
    # Test 6: Test convenience factory methods
    print("6. Testing convenience factory methods...")
    quick_brain = pynlm.createQuickBrain()
    print(f"   Quick brain created: {quick_brain.getTotalNeuronCount()} neurons")
    
    quick_world = pynlm.createQuickWorld()
    print(f"   Quick world created: {quick_world.getWidth()}x{quick_world.getHeight()}")
    
    quick_brain, quick_agent, quick_world = pynlm.createQuickAgent()
    print(f"   Quick agent created with {quick_brain.getTotalNeuronCount()} neurons")
    
    # Test 7: Test advanced utilities
    print("7. Testing advanced utilities...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    errors = pynlm.validateConfiguration(config)
    print(f"   Configuration validation errors: {len(errors)} (should be 0)")
    
    # Test invalid config
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 5)  # Too low
    errors2 = pynlm.validateConfiguration(config2)
    print(f"   Invalid config errors: {len(errors2)} (should be > 0)")
    
    # Test version info
    version, phase = pynlm.getVersionInfo()
    print(f"   Version: {version}, Phase: {phase}")
    
    # Test 8: Test constants
    print("8. Testing constants...")
    print(f"   DEFAULT_NEURON_COUNT: {pynlm.DEFAULT_NEURON_COUNT}")
    print(f"   DEFAULT_SYNAPSE_DENSITY: {pynlm.DEFAULT_SYNAPSE_DENSITY}")
    print(f"   DEFAULT_SIMULATION_TIME: {pynlm.DEFAULT_SIMULATION_TIME}")
    
    print("\nAll basic tests passed!")
    return True

def test_advanced_features():
    """Test advanced features for power users"""
    print("\nTesting advanced features...")
    
    # Create a complex configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    
    # Create brain with complex config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test agent with all features enabled
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 16, 16)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   Complex brain created: {brain.getTotalNeuronCount()} neurons")
    print(f"   Development stage: {agent.getDevelopmentalStage()}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Run a simulation loop
    print("   Running simulation loop...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"     Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Advanced features test completed!")
    return True

def test_sensory_input_features():
    """Test advanced sensory input features"""
    print("\nTesting sensory input features...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create a world and get sensory percept
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get initial percept
    percept = world.getSensoryPercept()
    print(f"1. Initial percept acquired")
    
    # Test vision data access
    vision = percept.getVision()
    print(f"2. Vision data: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Test 2D vision access
    vision_2d = vision.getDataAs2D()
    print(f"3. 2D vision access: {len(vision_2d)} rows, {len(vision_2d[0]) if vision_2d else 0} cols")
    
    # Test audio features
    audio = pynlm.Audio(44100, 1024)
    audio_data = [i * 0.01 * (1 if i % 2 == 0 else -1) for i in range(1024)]
    audio.setData(audio_data)
    audio.normalize(0.5)
    print(f"4. Audio features: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
    
    # Test internal signals
    internal = pynlm.InternalSignals()
    for i in range(5):
        internal.addSignal(i * 0.1)
    signals = internal.getAllSignals()
    print(f"5. Internal signals: {len(signals)} signals, average: {sum(signals)/len(signals):.3f}")
    
    # Test action features
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    action.setParameter(0, 1.0)  # Movement speed
    print(f"6. Action type: {action.getType()}, is movement: {action.isMovement()}, "
          f"is interaction: {action.isInteraction()}")
    
    print("Sensory input features test completed!")
    return True

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    try:
        # Run all tests
        test_basic_functionality()
        test_advanced_features()
        test_sensory_input_features()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED SUCCESSFULLY!")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\nERROR: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())