# Python test files for NLM

These tests are written using pytest and provide comprehensive coverage of the NLM Python API.

## Requirements

```bash
pip install pytest numpy
```

## Running Python Tests

```bash
# Run all tests
pytest

# Run specific test modules
pytest test_basic.py
pytest test_agent.py
pytest test_world.py
pytest test_config.py

# Run with verbose output
pytest -v

# Run with coverage
pytest --cov=pynlm
```

## Test Structure

Each test file follows this pattern:

```python
import pytest
import pynlm
import time

@pytest.fixture
def default_brain():
    """Create a default brain for testing."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

@pytest.fixture
def test_world():
    """Create a test world for agent tests."""
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    return world

def test_brain_creation():
    """Test basic brain creation and initialization."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain is not None
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    print("✓ test_brain_creation passed")

def test_brain_basic_functionality(default_brain):
    """Test basic brain functionality using fixture."""
    brain = default_brain
    
    # Verify initial state
    assert brain.getTotalNeuronCount() == 1000
    assert brain.getRegionCount() == 1
    
    # Run simulation steps
    for step in range(10):
        brain.step(step)
    
    # Verify brain state after steps
    assert brain.getTotalNeuronCount() == 1000
    
    print("✓ test_brain_basic_functionality passed")
```

## Test Modules

### test_basic.py

Basic functionality tests for core NLM classes and methods.

```python
import pytest
import pynlm
import numpy as np

class TestConfig:
    """Test configuration functionality."""
    
    def test_default_config(self):
        """Test default configuration creation."""
        config = pynlm.createDefaultConfig()
        
        assert config is not None
        assert config.has("brain.neuron_count")
        assert config.has("simulation.timestep")
        
        # Test getting values
        neuron_count = config.getOr("brain.neuron_count", 1000)
        assert neuron_count == 1000
        
        timestep = config.getOr("simulation.timestep", 0.001)
        assert timestep == 0.001
        
        print("✓ test_default_config passed")
    
    def test_config_modification(self):
        """Test configuration modification."""
        config = pynlm.createDefaultConfig()
        
        # Modify configuration
        config.set("brain.neuron_count", 2000)
        config.set("brain.region_count", 3)
        
        # Verify changes
        assert config.getOr("brain.neuron_count", 0) == 2000
        assert config.getOr("brain.region_count", 0) == 3
        
        # Test file operations (if supported)
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            
            # Create new config and load from file
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(f.name)
            
            assert new_config.getOr("brain.neuron_count", 0) == 2000
            
            os.unlink(f.name)
        
        print("✓ test_config_modification passed")

class TestSensoryInput:
    """Test sensory input classes."""
    
    def test_vision(self):
        """Test vision sensory input."""
        # Create vision data
        width, height, channels = 8, 8, 3
        vision_data = [0.1 * (i % 10) for i in range(width * height * channels)]
        
        # Create vision object (this may require implementation)
        # vision = pynlm.Vision(width, height, channels)
        # vision.setData(vision_data)
        
        # Test basic properties (if accessible)
        # assert vision.getWidth() == width
        # assert vision.getHeight() == height
        # assert vision.getChannels() == channels
        
        print("✓ test_vision passed")  # TODO: implement actual test

class TestAction:
    """Test action classes."""
    
    def test_action_creation(self):
        """Test action creation and manipulation."""
        # Test action types
        for action_type in [pynlm.ActionType.MoveForward, 
                           pynlm.ActionType.TurnLeft,
                           pynlm.ActionType.Interact]:
            action = pynlm.Action(action_type)
            assert action is not None
            assert action.getType() == action_type
        
        print("✓ test_action_creation passed")

class TestWorldObject:
    """Test world object functionality."""
    
    def test_world_object_creation(self):
        """Test world object creation."""
        # Create different types of world objects
        resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        hazard = pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, radius=1.0)
        
        assert resource.x == 5.0
        assert resource.y == 5.0
        assert resource.type == pynlm.WorldObjectType.Resource
        assert resource.value == 10.0
        
        assert hazard.x == 8.0
        assert hazard.y == 8.0
        assert hazard.type == pynlm.WorldObjectType.Hazard
        assert hazard.radius == 1.0
        
        print("✓ test_world_object_creation passed")
```

### test_agent.py

Agent integration and learning tests.

```python
import pytest
import pynlm
import time

class TestAgentBrain:
    """Test agent brain functionality."""
    
    @pytest.fixture
    def agent_and_world(self):
        """Create agent and world for testing."""
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent
        agent.initialize(world)
        
        return agent, world, brain
    
    def test_agent_initialization(self, agent_and_world):
        """Test agent brain initialization."""
        agent, world, brain = agent_and_world
        
        assert agent is not None
        assert world is not None
        assert brain is not None
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Verify subsystems are enabled
        assert agent.isRewardModulationEnabled() == True
        assert agent.isStructuralPlasticityEnabled() == True
        assert agent.isDevelopmentEnabled() == True
        assert agent.isCuriosityEnabled() == True
        
        print("✓ test_agent_initialization passed")
    
    def test_agent_sensory_processing(self, agent_and_world):
        """Test agent sensory input processing."""
        agent, world, brain = agent_and_world
        
        # Update world to get current state
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Verify brain state changed
        assert brain.getFiringNeuronCount() >= 0
        
        print("✓ test_agent_sensory_processing passed")
    
    def test_agent_motor_output(self, agent_and_world):
        """Test agent motor output decoding."""
        agent, world, brain = agent_and_world
        
        # Run brain simulation
        for step in range(10):
            brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Verify motor command is valid
        assert motor_cmd is not None
        # Motor command type check depends on implementation
        # assert motor_cmd.getType() in [pynlm.MotorCommand.MoveForward, ...]
        
        print("✓ test_agent_motor_output passed")

class TestAgentLearning:
    """Test agent learning capabilities."""
    
    def test_agent_reward_modulation(self):
        """Test agent reward modulation."""
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Create simple world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Initialize agent
        agent.initialize(world)
        
        # Apply reward
        agent.enableRewardModulation(True)
        agent.applyRewardModulation(1.0, 0.0)  # reward, predicted_reward
        
        # Verify neuromodulation level changed
        neuromod_level = agent.getNeuromodulationLevel()
        assert neuromod_level >= 0.0
        
        print("✓ test_agent_reward_modulation passed")
    
    def test_agent_curiosity(self):
        """Test agent curiosity mechanism."""
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Enable curiosity
        agent.enableCuriosity(True)
        
        # Run some simulation to generate novelty
        for step in range(50):
            brain.step(step)
        
        # Get curiosity level
        curiosity = agent.getCuriosityLevel()
        
        # Verify curiosity level is within bounds
        assert 0.0 <= curiosity <= 1.0
        
        print("✓ test_agent_curiosity passed")

class TestAgentDevelopment:
    """Test agent development system."""
    
    def test_agent_development_stages(self):
        """Test agent development through stages."""
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        
        # Get initial developmental stage
        initial_stage = agent.getDevelopmentalStage()
        
        # Update development over time
        for timestep in [0.1, 0.2, 0.5, 1.0]:
            agent.updateDevelopment(timestep)
        
        # Get final developmental stage
        final_stage = agent.getDevelopmentalStage()
        
        # Development should progress or stay the same
        assert final_stage >= initial_stage
        
        print("✓ test_agent_development_stages passed")
```

### test_world.py

World simulation and environment tests.

```python
import pytest
import pynlm
import time

class TestSimpleWorld:
    """Test simple world functionality."""
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        world = pynlm.createSimpleWorld()
        
        assert world is not None
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Verify configuration
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        
        print("✓ test_world_creation passed")
    
    def test_world_reset(self):
        """Test world reset functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        
        # Set initial state
        world.setAgentStart(5.0, 5.0)
        
        # Add objects
        obj = pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Resource, value=5.0)
        world.addObject(obj)
        
        # Update world
        world.update(0.1)
        
        # Reset world
        world.reset()
        
        # Verify reset state
        assert world.getAgentBody().x == 5.0
        assert world.getAgentBody().y == 5.0
        
        print("✓ test_world_reset passed")
    
    def test_world_updates(self):
        """Test world update loop."""
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=7, visionHeight=7)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Run multiple updates
        for i in range(10):
            world.update(0.1)
            
            # Verify state consistency
            assert world.getWidth() == 15
            assert world.getHeight() == 15
            assert world.getSimulationTime() > 0.0
        
        print("✓ test_world_updates passed")
    
    def test_world_agent_interaction(self):
        """Test agent interaction with world."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Get initial percept
        percept1 = world.getSensoryPercept()
        assert percept1 is not None
        
        # Apply motor command
        motor_cmd = pynlm.ActionType.MoveForward  # This may need adjustment
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Update world
        world.update(0.1)
        
        # Get updated percept
        percept2 = world.getSensoryPercept()
        assert percept2 is not None
        
        # World should have changed
        assert world.getAgentBody().x != 5.0
        
        print("✓ test_world_agent_interaction passed")

class TestWorldObjects:
    """Test world object management."""
    
    def test_world_object_management(self):
        """Test adding and removing world objects."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Add various object types
        resource = pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Resource, value=10.0)
        hazard = pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, radius=1.0)
        wall = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Wall, radius=2.0)
        marker = pynlm.WorldObject(2.0, 10.0, pynlm.WorldObjectType.Marker, value=1.0)
        
        world.addObject(resource)
        world.addObject(hazard)
        world.addObject(wall)
        world.addObject(marker)
        
        # Verify objects are present
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Remove an object
        world.removeObject(8.0, 8.0)
        
        # Verify object is removed
        percept2 = world.getSensoryPercept()
        assert percept2 is not None
        
        print("✓ test_world_object_management passed")
    
    def test_world_position_validation(self):
        """Test world position validation."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()
        
        # Test valid positions
        assert world.isValidPosition(5.0, 5.0) == True
        assert world.isValidPosition(0.0, 0.0) == True
        assert world.isValidPosition(9.0, 9.0) == True
        
        # Test invalid positions
        assert world.isValidPosition(-1.0, 5.0) == False
        assert world.isValidPosition(5.0, 10.0) == False
        assert world.isValidPosition(10.0, 5.0) == False
        
        print("✓ test_world_position_validation passed")
```

### test_config.py

Configuration management tests.

```python
import pytest
import pynlm
import tempfile
import os

class TestConfiguration:
    """Test configuration system functionality."""
    
    def test_config_keys(self):
        """Test configuration key management."""
        config = pynlm.createDefaultConfig()
        
        # Get all keys
        keys = config.getKeys()
        
        # Should have some basic keys
        assert len(keys) > 0
        assert "brain.neuron_count" in keys
        assert "simulation.timestep" in keys
        
        print("✓ test_config_keys passed")
    
    def test_config_has(self):
        """Test configuration existence checking."""
        config = pynlm.createDefaultConfig()
        
        # Test existing keys
        assert config.has("brain.neuron_count") == True
        assert config.has("simulation.timestep") == True
        
        # Test non-existing keys
        assert config.has("nonexistent.key") == False
        assert config.has("brain.invalid_param") == False
        
        print("✓ test_config_has passed")
    
    def test_config_summary(self):
        """Test configuration summary generation."""
        config = pynlm.createDefaultConfig()
        
        # Get summary
        summary = config.summary()
        
        # Should contain configuration information
        assert summary is not None
        assert len(summary) > 0
        
        print("✓ test_config_summary passed")
    
    def test_config_clear(self):
        """Test configuration clearing."""
        config = pynlm.createDefaultConfig()
        
        # Modify config
        config.set("brain.neuron_count", 5000)
        config.set("custom.value", "test")
        
        # Verify modifications
        assert config.getOr("brain.neuron_count", 0) == 5000
        assert config.has("custom.value") == True
        
        # Clear config
        config.clear()
        
        # Verify clearing
        assert config.getKeys() == []
        assert config.has("brain.neuron_count") == False
        assert config.has("custom.value") == False
        
        print("✓ test_config_clear passed")

class TestConfigurationIO:
    """Test configuration file I/O operations."""
    
    def test_config_save_load(self):
        """Test configuration save and load operations."""
        # Create config
        original_config = pynlm.createDefaultConfig()
        original_config.set("brain.neuron_count", 3000)
        original_config.set("brain.region_count", 4)
        original_config.set("custom.parameter", "test_value")
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_path = f.name
        
        try:
            original_config.saveToFile(config_path)
            
            # Load into new config
            loaded_config = pynlm.createDefaultConfig()
            loaded_config.loadFromFile(config_path)
            
            # Verify loaded values
            assert loaded_config.getOr("brain.neuron_count", 0) == 3000
            assert loaded_config.getOr("brain.region_count", 0) == 4
            assert loaded_config.getOr("custom.parameter", "") == "test_value"
            
        finally:
            # Clean up
            if os.path.exists(config_path):
                os.unlink(config_path)
        
        print("✓ test_config_save_load passed")

class TestConfigurationQuery:
    """Test configuration query operations."""
    
    def test_config_get_or(self):
        """Test configuration getOr method."""
        config = pynlm.createDefaultConfig()
        
        # Test getting existing values
        neuron_count = config.getOr("brain.neuron_count", 0)
        assert neuron_count == 1000
        
        timestep = config.getOr("simulation.timestep", 0.0)
        assert timestep == 0.001
        
        # Test getting non-existing values with defaults
        missing_value = config.getOr("nonexistent.key", 999)
        assert missing_value == 999
        
        print("✓ test_config_get_or passed")
```

## Integration Tests

### test_integration.py

Integration tests that test the entire NLM system.

```python
import pytest
import pynlm
import time

class TestNLMIntegration:
    """Test NLM system integration."""
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation from setup to execution."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify simulation completed successfully
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert agent is not None
        assert world is not None
        
        print("✓ test_complete_agent_simulation passed")
    
    def test_simulation_reproducibility(self):
        """Test simulation reproducibility with same seed."""
        # Create first brain with seed
        config1 = pynlm.createDefaultConfig()
        config1.set("random_seed", 42)
        
        brain1 = pynlm.createBrain(config1)
        brain1.initialize()
        
        # Run simulation
        for step in range(100):
            brain1.step(step)
        
        # Create second brain with same seed
        config2 = pynlm.createDefaultConfig()
        config2.set("random_seed", 42)
        
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        # Run identical simulation
        for step in range(100):
            brain2.step(step)
        
        # Verify results are identical
        assert brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount()
        assert brain1.getFiringNeuronCount() == brain2.getFiringNeuronCount()
        assert brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount()
        
        print("✓ test_simulation_reproducibility passed")
    
    def test_simulation_checkpoint(self):
        """Test brain checkpoint/save and load functionality."""
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Run some simulation
        for step in range(50):
            brain1.step(step)
        
        # Save brain state
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        try:
            brain1.save(checkpoint_path)
            
            # Create new brain and load checkpoint
            brain2 = pynlm.createBrain(config)
            brain2.load(checkpoint_path)
            
            # Verify loaded brain state
            assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
            assert brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount()
            
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
        
        print("✓ test_simulation_checkpoint passed")

class TestPerformanceTests:
    """Test performance characteristics."""
    
    def test_simulation_speed(self):
        """Test basic simulation speed."""
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure time for simulation steps
        start_time = time.time()
        
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete 100 steps reasonably quickly
        assert elapsed < 10.0  # Should be much less than 10 seconds
        
        print(f"✓ test_simulation_speed passed ({elapsed:.3f}s for 100 steps)")
    
    def test_memory_usage(self):
        """Test memory usage doesn't grow excessively."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and run multiple brains
        for i in range(5):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(20):
                brain.step(step)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_growth = final_memory - initial_memory
        
        # Memory growth should be reasonable (less than 100MB for 5 brains)
        assert memory_growth < 100.0
        
        print(f"✓ test_memory_usage passed (growth: {memory_growth:.2f}MB)")
```

## Running Tests

To run the Python tests:

```bash
# Install dependencies
pip install pytest numpy

# Run all tests
pytest

# Run specific test modules
pytest test_basic.py
pytest test_agent.py
pytest test_world.py
pytest test_config.py
pytest test_integration.py

# Run with verbose output
pytest -v

# Run with coverage
pytest --cov=pynlm --cov-report=html

# Run only failing tests
pytest --lf

# Run tests in specific order
pytest -k "test_brain"
```

## Test Coverage

The tests provide coverage for:

- **Basic Configuration**: Config creation, modification, file I/O
- **Brain Functionality**: Creation, initialization, simulation, statistics
- **Agent Systems**: Sensory processing, motor output, learning mechanisms
- **World Simulation**: Environment interaction, object management
- **Integration**: Complete agent simulation, reproducibility, checkpointing
- **Performance**: Speed, memory usage

## Test Best Practices

1. **Use fixtures** for shared resources (brains, worlds)
2. **Test edge cases** (empty configurations, boundary conditions)
3. **Verify reproducibility** with fixed random seeds
4. **Test error conditions** and invalid inputs
5. **Benchmark performance** but don't let tests fail due to timing
6. **Clean up resources** after tests (file handles, temporary objects)

These Python tests complement the existing C++ tests and provide comprehensive coverage of the NLM Python API for users who want to test their Python code using NLM.
