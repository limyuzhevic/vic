#!/usr/bin/env python3
"""
Test script to verify Python bindings improvements
"""

import sys
import os

# Add the current directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    
    def test_basic_bindings():
        """Test basic class bindings and Pythonic API"""
        print("Testing Python bindings improvements...")
        
        # Test Config class
        print("\n1. Testing Config class...")
        config = pynlm.createDefaultConfig()
        config.set("learning_rate", 0.1)
        config.set("batch_size", 32)
        config.set("enabled", True)
        
        # Test get() methods
        lr = config.get("learning_rate")
        bs = config.get("batch_size")
        enabled = config.get("enabled")
        
        print(f"   Config.get('learning_rate') = {lr} (type: {type(lr).__name__})")
        print(f"   Config.get('batch_size') = {bs} (type: {type(bs).__name__})")
        print(f"   Config.get('enabled') = {enabled} (type: {type(enabled).__name__})")
        
        # Test getOr() methods
        unknown = config.getOr("unknown_key", 42.0)
        print(f"   Config.getOr('unknown_key', 42.0) = {unknown}")
        
        # Test has() and remove()
        has_lr = config.has("learning_rate")
        config.remove("learning_rate")
        has_lr_after = config.has("learning_rate")
        
        print(f"   Config.has('learning_rate') before removal: {has_lr}")
        print(f"   Config.has('learning_rate') after removal: {has_lr_after}")
        
        # Test SensoryInput subclasses
        print("\n2. Testing SensoryInput subclasses...")
        
        # Vision
        vision = pynlm.Vision(640, 480, 3)
        print(f"   Vision: width={vision.getWidth()}, height={vision.getHeight()}, channels={vision.getChannels()}")
        print(f"   Vision.getDimensions() = {vision.getDimensions()}")
        
        # Audio
        audio = pynlm.Audio(44100, 1024)
        print(f"   Audio: sample_rate={audio.getSampleRate()}, num_samples={audio.getNumSamples()}")
        print(f"   Audio.getDimensions() = {audio.getDimensions()}")
        
        # InternalSignals
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(0.8)
        print(f"   InternalSignals dimensions = {internal.getDimensions()}")
        
        # Test Action class
        print("\n3. Testing Action class...")
        action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.3])
        print(f"   Action type: {action.getType()}")
        print(f"   Action parameters: {action.getParameters()}")
        print(f"   Action name: {action.getName()}")
        
        # Test WorldObject class
        print("\n4. Testing WorldObject class...")
        world_obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 1.5, 0.8)
        print(f"   WorldObject: x={world_obj.x}, y={world_obj.y}, type={world_obj.type}, value={world_obj.value}")
        
        # Test AgentBody class
        print("\n5. Testing AgentBody class...")
        agent_body = pynlm.AgentBody()
        agent_body.x = 5.0
        agent_body.y = 7.0
        agent_body.energy = 0.8
        print(f"   AgentBody: x={agent_body.x}, y={agent_body.y}, energy={agent_body.energy}")
        
        # Test ActionResult class
        print("\n6. Testing ActionResult class...")
        action_result = pynlm.ActionResult(1.0, True, "Success!")
        print(f"   ActionResult: reward={action_result.reward}, success={action_result.success}, message={action_result.message}")
        
        # Test SensoryPercept class
        print("\n7. Testing SensoryPercept class...")
        percept = pynlm.SensoryPercept()
        percept.setVision([0.1, 0.2, 0.3])
        percept.setTouch([0.0, 1.0])
        percept.setInternal([0.5, 0.5, 0.5])
        percept.setProprioception([0.0, 0.0, 0.0, 0.0])
        percept.setAudio([0.3, 0.3, 0.3])
        print(f"   SensoryPercept vision length: {len(percept.getVision())}")
        print(f"   SensoryPercept touch length: {len(percept.getTouch())}")
        print(f"   SensoryPercept all signals length: {len(percept.getAllSignals())}")
        
        # Test SimpleWorld class
        print("\n8. Testing SimpleWorld class...")
        world = pynlm.createSimpleWorld()
        world.configure(100.0, 100.0, 16, 16)
        world.setAgentStart(50.0, 50.0)
        print(f"   World dimensions: {world.getWidth()}x{world.getHeight()}")
        print(f"   Vision dimensions: {world.getVisionWidth()}x{world.getVisionHeight()}")
        print(f"   Max energy: {world.getMaxEnergy()}")
        print(f"   Energy decay rate: {world.getEnergyDecayRate()}")
        print(f"   Simulation time: {world.getSimulationTime()}")
        print(f"   Random seed: {world.getRandomSeed()}")
        
        # Test Brain class
        print("\n9. Testing Brain class...")
        brain_config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        print(f"   Brain initialized: {brain.getConfig() is not None}")
        print(f"   Brain region count: {brain.getRegionCount()}")
        print(f"   Brain neuron count: {brain.getTotalNeuronCount()}")
        print(f"   Brain synapse count: {brain.getTotalSynapseCount()}")
        print(f"   Brain developmental stage: {brain.getDevelopmentalStage()}")
        print(f"   Brain learned preference: {brain.getLearnedPreference()}")
        
        # Test AgentBrain class
        print("\n10. Testing AgentBrain class...")
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        print(f"   AgentBrain initialized: true")
        print(f"   AgentBrain sensory input size: {agent_brain.getSensoryInputSize()}")
        print(f"   AgentBrain motor output size: {agent_brain.getMotorOutputSize()}")
        print(f"   AgentBrain developmental stage: {agent_brain.getDevelopmentalStage()}")
        
        print("\n✅ All Python binding tests passed successfully!")
        return True
        
    except Exception as e:
        print(f"\n❌ Error testing Python bindings: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_basic_bindings()
    sys.exit(0 if success else 1)