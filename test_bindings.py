#!/usr/bin/env python3

"""
Test script to verify NLM Python bindings work correctly.
This tests the examples from the documentation.
"""

import sys
import os

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"  Version: {pynlm.__version__}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_factory_functions():
    """Test the factory functions mentioned in documentation."""
    print("\n=== Testing Factory Functions ===")
    
    # Test createDefaultConfig
    try:
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        # Test config methods
        config.set("test.key", "test_value")
        config.set("test.int", 42)
        config.set("test.float", 3.14)
        config.set("test.bool", True)
        
        if config.has("test.key"):
            print("✓ config.has() works")
        
        keys = config.getKeys()
        print(f"✓ config.getKeys() works: {len(keys)} keys")
        
        # Test get methods
        val = config.get("test.key")
        print(f"✓ config.get() works: {val}")
        
        val2 = config.getOr("nonexistent", "default")
        print(f"✓ config.getOr() works: {val2}")
        
        # Test file operations
        test_file = "test_config.json"
        if config.saveToFile(test_file):
            print(f"✓ config.saveToFile() works: saved to {test_file}")
            
            # Try to load it back
            new_config = pynlm.createDefaultConfig()
            if new_config.loadFromFile(test_file):
                print("✓ config.loadFromFile() works")
            else:
                print("✗ config.loadFromFile() failed")
        
        os.remove(test_file)  # Clean up
        
    except Exception as e:
        print(f"✗ createDefaultConfig() failed: {e}")
        return False
    
    # Test createBrain
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        print("✓ createBrain() works")
        
        # Test brain methods
        if brain.initialize():
            print("✓ brain.initialize() works")
        else:
            print("✗ brain.initialize() failed")
            return False
        
        brain.step(0)
        print("✓ brain.step() works")
        
        neurons = brain.getTotalNeuronCount()
        print(f"✓ brain.getTotalNeuronCount() works: {neurons} neurons")
        
        synapses = brain.getTotalSynapseCount()
        print(f"✓ brain.getTotalSynapseCount() works: {synapses} synapses")
        
        firing = brain.getFiringNeuronCount()
        print(f"✓ brain.getFiringNeuronCount() works: {firing} firing neurons")
        
        rate = brain.getAverageFiringRate()
        print(f"✓ brain.getAverageFiringRate() works: {rate:.3f}")
        
        stage = brain.getDevelopmentalStage()
        print(f"✓ brain.getDevelopmentalStage() works: {stage}")
        
        # Test reset
        brain.reset()
        print("✓ brain.reset() works")
        
        # Test save/load
        test_file = "test_brain.bin"
        if brain.save(test_file):
            print(f"✓ brain.save() works: saved to {test_file}")
            
            # Create a new brain and try to load
            new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            if new_brain.load(test_file):
                print("✓ brain.load() works")
            else:
                print("✗ brain.load() failed")
            
            os.remove(test_file)  # Clean up
        
    except Exception as e:
        print(f"✗ createBrain() failed: {e}")
        return False
    
    # Test createSimpleWorld
    try:
        world = pynlm.createSimpleWorld()
        print("✓ createSimpleWorld() works")
        
        # Test world methods
        world.configure(20, 20, 8, 8)
        print("✓ world.configure() works")
        
        world.reset()
        print("✓ world.reset() works")
        
        world.setAgentStart(10.0, 10.0)
        print("✓ world.setAgentStart() works")
        
        world.update(0.1)
        print("✓ world.update() works")
        
        width = world.getWidth()
        height = world.getHeight()
        print(f"✓ world.getWidth()/getHeight() works: {width}x{height}")
        
        # Test sensory input
        percept = world.getSensoryPercept()
        print("✓ world.getSensoryPercept() works")
        
        body = world.getAgentBody()
        print("✓ world.getAgentBody() works")
        
        # Test object operations
        world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
        print("✓ world.addObject() works")
        
        if world.isValidPosition(5.0, 5.0):
            print("✓ world.isValidPosition() works")
        
    except Exception as e:
        print(f"✗ createSimpleWorld() failed: {e}")
        return False
    
    # Test createAgentBrain
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ createAgentBrain() works")
        
        # Need a world for agent to work with
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent.initialize(world)
        print("✓ agent.initialize() works")
        
        # Test agent methods
        size = agent.getSensoryInputSize()
        print(f"✓ agent.getSensoryInputSize() works: {size}")
        
        size = agent.getMotorOutputSize()
        print(f"✓ agent.getMotorOutputSize() works: {size}")
        
        agent.processSensoryInput(world.getSensoryPercept())
        print("✓ agent.processSensoryInput() works")
        
        action = agent.decodeMotorCommand()
        print("✓ agent.decodeMotorCommand() works")
        
        # Test enable methods
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        print("✓ agent enable methods work")
        
        # Test getter methods
        curiosity = agent.getCuriosityLevel()
        print(f"✓ agent.getCuriosityLevel() works: {curiosity:.3f}")
        
        novelty = agent.getNoveltyLevel()
        print(f"✓ agent.getNoveltyLevel() works: {novelty:.3f}")
        
        prediction_error = agent.getPredictionError()
        print(f"✓ agent.getPredictionError() works: {prediction_error:.3f}")
        
        neuromod = agent.getNeuromodulationLevel()
        print(f"✓ agent.getNeuromodulationLevel() works: {neuromod:.3f}")
        
        agent.reset()
        print("✓ agent.reset() works")
        
    except Exception as e:
        print(f"✗ createAgentBrain() failed: {e}")
        return False
    
    return True

def test_enumerations():
    """Test that enumerations work correctly."""
    print("\n=== Testing Enumerations ===")
    
    try:
        # Neuron types
        print(f"✓ NeuronType.Excitatory = {pynlm.NeuronType.Excitatory}")
        print(f"✓ NeuronType.Inhibitory = {pynlm.NeuronType.Inhibitory}")
        print(f"✓ NeuronType.Sensory = {pynlm.NeuronType.Sensory}")
        print(f"✓ NeuronType.Motor = {pynlm.NeuronType.Motor}")
        print(f"✓ NeuronType.Modulatory = {pynlm.NeuronType.Modulatory}")
        print(f"✓ NeuronType.Internal = {pynlm.NeuronType.Internal}")
        
        # Action types
        print(f"✓ ActionType.MoveForward = {pynlm.ActionType.MoveForward}")
        print(f"✓ ActionType.TurnLeft = {pynlm.ActionType.TurnLeft}")
        print(f"✓ ActionType.Interact = {pynlm.ActionType.Interact}")
        print(f"✓ ActionType.Eat = {pynlm.ActionType.Eat}")
        print(f"✓ ActionType.Rest = {pynlm.ActionType.Rest}")
        print(f"✓ ActionType.Wait = {pynlm.ActionType.Wait}")
        
        # Developmental stages
        print(f"✓ DevelopmentalStage.Initial = {pynlm.DevelopmentalStage.Initial}")
        print(f"✓ DevelopmentalStage.CriticalPeriod = {pynlm.DevelopmentalStage.CriticalPeriod}")
        print(f"✓ DevelopmentalStage.Adult = {pynlm.DevelopmentalStage.Adult}")
        
        # World object types
        print(f"✓ WorldObjectType.Empty = {pynlm.WorldObjectType.Empty}")
        print(f"✓ WorldObjectType.Resource = {pynlm.WorldObjectType.Resource}")
        print(f"✓ WorldObjectType.Hazard = {pynlm.WorldObjectType.Hazard}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enumeration test failed: {e}")
        return False

def test_example_scenarios():
    """Test the example scenarios from documentation."""
    print("\n=== Testing Example Scenarios ===")
    
    # Test "Simplest Brain" example
    try:
        print("Testing 'Simplest Brain' example...")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        spikes = brain.getTotalSpikeCount()
        print(f"✓ Simplest Brain example works: {spikes} spikes")
        
    except Exception as e:
        print(f"✗ Simplest Brain example failed: {e}")
        return False
    
    # Test "Complete Agent" example (simplified)
    try:
        print("Testing 'Complete Agent' example...")
        
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run agent (shortened)
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        firing = brain.getFiringNeuronCount()
        print(f"✓ Complete Agent example works: {firing} neurons firing")
        
    except Exception as e:
        print(f"✗ Complete Agent example failed: {e}")
        return False
    
    return True

def test_constants_and_identifiers():
    """Test constants and identifier types."""
    print("\n=== Testing Constants and Identifiers ===")
    
    try:
        # Test constants
        print(f"✓ INVALID_NEURON_ID = {pynlm.INVALID_NEURON_ID}")
        print(f"✓ INVALID_SYNAPSE_ID = {pynlm.INVALID_SYNAPSE_ID}")
        print(f"✓ INVALID_REGION_ID = {pynlm.INVALID_REGION_ID}")
        print(f"✓ INVALID_POPULATION_ID = {pynlm.INVALID_POPULATION_ID}")
        
        # Test NeuronId methods
        neuron_id = pynlm.NeuronId(42)
        index = neuron_id.index()
        print(f"✓ NeuronId.index() works: {index}")
        
        if neuron_id == pynlm.NEURON_ID:
            print("✓ NeuronId comparison works")
        
        # Test SynapseId methods
        synapse_id = pynlm.SynapseId(100)
        index = synapse_id.index()
        print(f"✓ SynapseId.index() works: {index}")
        
        # Test RegionId methods
        region_id = pynlm.RegionId(1)
        index = region_id.index()
        print(f"✓ RegionId.index() works: {index}")
        
        # Test PopulationId methods
        pop_id = pynlm.PopulationId(5)
        index = pop_id.index()
        print(f"✓ PopulationId.index() works: {index}")
        
        return True
        
    except Exception as e:
        print(f"✗ Constants/identifiers test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("Testing NLM Python Bindings")
    print("=" * 50)
    
    tests = [
        test_factory_functions,
        test_enumerations,
        test_example_scenarios,
        test_constants_and_identifiers,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
                print(f"✓ {test.__name__} PASSED")
            else:
                failed += 1
                print(f"✗ {test.__name__} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} ERROR: {e}")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n🎉 All tests passed! Python bindings are working correctly.")
        return 0
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())