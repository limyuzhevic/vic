"""Python tests for NLM bindings."""

import pynlm
import pytest
import tempfile
import os


def test_basic_bindings():
    """Test that basic bindings are available and functional."""
    # Test config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test config operations
    config.set("brain.neuron_count", 100)
    config.set("plasticity.stdp.enable", True)
    assert config.has("brain.neuron_count")
    assert config.get("brain.neuron_count") == 100
    
    keys = config.getKeys()
    assert "brain.neuron_count" in keys
    
    # Test brain creation
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test brain initialization
    brain.initialize()
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() > 0


def test_simple_world():
    """Test SimpleWorld functionality."""
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Test configuration
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    
    # Test agent start
    world.setAgentStart(10.0, 10.0)
    
    # Test reset and update
    world.reset()
    world.update(0.1)
    assert world.getSimulationTime() > 0
    
    # Test agent body
    body = world.getAgentBody()
    assert body.x == 10.0
    assert body.y == 10.0


def test_agent_brain():
    """Test AgentBrain functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    # Test initialization
    agent.initialize(world)
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor decoding
    action = agent.decodeMotorCommand()
    assert action is not None
    
    # Test reward modulation
    agent.applyRewardModulation(1.0, 0.5)
    
    # Test development
    agent.updateDevelopment(0.1)
    
    # Test subsystem enabling
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Test status queries
    assert agent.isRewardModulationEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isCuriosityEnabled() == True
    
    # Test statistics
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction = agent.getPredictionError()
    neuromod = agent.getNeuromodulationLevel()
    
    assert isinstance(curiosity, float)
    assert isinstance(novelty, float)
    assert isinstance(prediction, float)
    assert isinstance(neuromod, float)


def test_action_and_objects():
    """Test Action and WorldObject functionality."""
    # Test action creation
    action = pynlm.Action()
    assert action is not None
    
    # Test action from type
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward
    
    # Test action parameters
    params = [0.1, 0.2, 0.3]
    action = pynlm.Action(pynlm.ActionType.Custom, params)
    assert action.getParameters() == params
    
    # Test action name
    assert action.getName() is not None
    assert "MoveForward" in action.getName()
    
    # Test cloning
    action_clone = action.clone()
    assert action_clone.getType() == action.getType()
    assert action_clone.getParameters() == action.getParameters()
    
    # Test world object
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
    assert obj.x == 5.0
    assert obj.y == 5.0
    assert obj.type == pynlm.WorldObjectType.Resource
    assert obj.value == 1.0


def test_sensory_percept():
    """Test SensoryPercept functionality."""
    percept = pynlm.SensoryPercept()
    assert percept is not None
    
    # Test vision
    vision = percept.getVision()
    assert vision is not None
    
    # Test audio
    audio = percept.getAudio()
    assert audio is not None
    
    # Test internal signals
    internal = percept.getInternal()
    assert internal is not None
    
    # Test proprioception
    proprioception = percept.getProprioception()
    assert proprioception is not None
    
    # Test timestamps
    percept.setTimestamp(123.456)
    assert percept.getTimestamp() == 123.456


def test_agent_body():
    """Test AgentBody functionality."""
    body = pynlm.AgentBody()
    assert body is not None
    
    # Test position
    body.x = 10.0
    body.y = 20.0
    assert body.x == 10.0
    assert body.y == 20.0
    
    # Test orientation
    body.orientation = 0.785  # ~45 degrees
    assert body.orientation == 0.785
    
    # Test movement
    body.isMoving = True
    body.isTurning = False
    assert body.isMoving == True
    assert body.isTurning == False
    
    # Test reset
    body.reset()
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.isMoving == False


def test_enums():
    """Test enumerations."""
    # Test neuron types
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    assert pynlm.NeuronType.Sensory is not None
    assert pynlm.NeuronType.Motor is not None
    
    # Test action types
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.TurnLeft is not None
    assert pynlm.ActionType.Interact is not None
    assert pynlm.ActionType.Eat is not None
    
    # Test world object types
    assert pynlm.WorldObjectType.Resource is not None
    assert pynlm.WorldObjectType.Hazard is not None
    assert pynlm.WorldObjectType.Wall is not None


def test_simulation_steps():
    """Test running a simple simulation."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world percept
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Check that brain is working
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getTotalSpikeCount() >= 0


def test_checkpoints():
    """Test brain save/load functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate some steps
    for i in range(50):
        brain.step(i)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as f:
        filepath = f.name
    
    try:
        # Save brain state
        result = brain.save(filepath)
        assert result == True
        
        # Create new brain and load
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        # Load the state
        result = new_brain.load(filepath)
        assert result == True
        
        # Check that some properties are preserved
        assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
        
    finally:
        # Clean up
        if os.path.exists(filepath):
            os.unlink(filepath)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])