#!/usr/bin/env python3
"""
Test script to verify backward compatibility of the refactored NLM Python bindings.
This script tests that all the original API calls still work.
"""

import sys
import traceback

def test_backward_compatibility():
    """Test that all the original API calls still work."""
    print("Testing backward compatibility of NLM Python bindings...")
    
    try:
        # Test imports
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test creating config
        config = pynlm.createDefaultConfig()
        print("✓ Successfully created config")
        
        # Test config methods
        print(f"  - Config has summary: {config.summary()}")
        print(f"  - Config keys: {config.getKeys()}")
        
        # Test creating brain
        brain = pynlm.createBrain(config)
        print("✓ Successfully created brain")
        
        # Test brain methods
        brain.initialize()
        print("✓ Successfully initialized brain")
        print(f"  - Brain has {brain.getTotalNeuronCount()} neurons")
        print(f"  - Brain has {brain.getTotalSynapseCount()} synapses")
        
        # Test brain step
        brain.step(0)
        print("✓ Successfully executed brain step")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        
        # Test world creation
        world = pynlm.createSimpleWorld()
        print("✓ Successfully created world")
        
        # Test world methods
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        print("✓ Successfully configured world")
        
        world.reset()
        print("✓ Successfully reset world")
        
        # Test agent creation
        agent = pynlm.createAgentBrain(brain)
        print("✓ Successfully created agent")
        
        # Test agent methods
        agent.initialize(world)
        print("✓ Successfully initialized agent")
        print(f"  - Agent sensory input size: {agent.getSensoryInputSize()}")
        print(f"  - Agent motor output size: {agent.getMotorOutputSize()}")
        
        # Test all enumerations
        print("\nTesting enumerations:")
        print(f"  - NeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
        print(f"  - SynapseType.Excitatory: {pynlm.SynapseType.Excitatory}")
        print(f"  - ActionType.MoveForward: {pynlm.ActionType.MoveForward}")
        print(f"  - WorldObjectType.Resource: {pynlm.WorldObjectType.Resource}")
        print(f"  - DevelopmentalStage.Initial: {pynlm.DevelopmentalStage.Initial}")
        
        # Test all ID types
        print("\nTesting ID types:")
        neuron_id = pynlm.NeuronId(123)
        synapse_id = pynlm.SynapseId(456)
        region_id = pynlm.RegionId(789)
        population_id = pynlm.PopulationId(999)
        
        print(f"  - NeuronId: {neuron_id}")
        print(f"  - SynapseId: {synapse_id}")
        print(f"  - RegionId: {region_id}")
        print(f"  - PopulationId: {population_id}")
        
        # Test constants
        print("\nTesting global constants:")
        print(f"  - INVALID_NEURON_ID: {pynlm.INVALID_NEURON_ID}")
        print(f"  - INVALID_SYNAPSE_ID: {pynlm.INVALID_SYNAPSE_ID}")
        print(f"  - INVALID_REGION_ID: {pynlm.INVALID_REGION_ID}")
        print(f"  - INVALID_POPULATION_ID: {pynlm.INVALID_POPULATION_ID}")
        
        # Test all classes from motor.py
        print("\nTesting motor.py classes:")
        action = pynlm.Action()
        action.setType(pynlm.ActionType.MoveForward)
        print(f"  - Action: type={action.getType()}, name={action.getName()}")
        
        world_obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, value=10.0, radius=1.0)
        print(f"  - WorldObject: x={world_obj.x}, y={world_obj.y}, type={world_obj.type}, value={world_obj.value}")
        
        agent_body = pynlm.AgentBody()
        agent_body.x = 5.0
        agent_body.y = 5.0
        print(f"  - AgentBody: x={agent_body.x}, y={agent_body.y}")
        
        action_result = pynlm.ActionResult(reward=1.5, success=True, message="Test")
        print(f"  - ActionResult: reward={action_result.reward}, success={action_result.success}, message='{action_result.message}'")
        
        # Test sensory classes
        print("\nTesting sensory.py classes:")
        vision = pynlm.Vision()
        vision.setData([0.1, 0.2, 0.3])
        print(f"  - Vision: width={vision.getWidth()}, height={vision.getHeight()}, channels={vision.getChannels()}")
        
        audio = pynlm.Audio()
        audio.setSampleRate(44100)
        audio.setNumSamples(1024)
        print(f"  - Audio: sample_rate={audio.getSampleRate()}, num_samples={audio.getNumSamples()}")
        
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(1.0)
        print(f"  - InternalSignals: signals={len(internal.getData())}")
        
        # Test world functionality
        print("\nTesting world functionality:")
        world.setAgentStart(0.0, 0.0)
        print("  - Set agent start position")
        
        # Test brain functionality
        print("\nTesting brain functionality:")
        brain.addRegion("test_region")
        print("  - Added region")
        
        # Test agent functionality
        print("\nTesting agent functionality:")
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        print("  - Enabled subsystems")
        
        # Run a complete simulation loop
        print("\nRunning complete simulation loop...")
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  - Completed 10 simulation steps")
        print(f"  - Final firing neurons: {brain.getFiringNeuronCount()}")
        
        # Test save/load functionality
        print("\nTesting save/load functionality:")
        brain.save("test_brain.bin")
        print("  - Saved brain to test_brain.bin")
        
        brain.reset()
        brain.load("test_brain.bin")
        print("  - Loaded brain from test_brain.bin")
        
        print("\n" + "="*60)
        print("ALL TESTS PASSED! Backward compatibility verified.")
        print("="*60)
        
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {str(e)}")
        print("\nFull traceback:")
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_backward_compatibility()
    sys.exit(0 if success else 1)
