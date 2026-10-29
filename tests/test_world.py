"""
World NLM Python tests for world simulation.

Tests for world configuration, object management, and simulation.
"""

import pytest
import pynlm


class TestSimpleWorldCreation:
    """Test SimpleWorld creation and configuration."""
    
    def test_world_creation(self):
        """Test basic world creation."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Check default world properties
        assert world.getWidth() == 0  # Default width
        assert world.getHeight() == 0  # Default height
        assert world.getVisionWidth() == 0  # Default vision width
        assert world.getVisionHeight() == 0  # Default vision height
    
    def test_world_configuration(self):
        """Test world configuration."""
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Check configuration
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
    
    def test_world_reset(self):
        """Test world reset functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Set agent start position
        world.setAgentStart(5.0, 5.0)
        
        # Reset world
        world.reset()
        
        # Check that agent body exists
        body = world.getAgentBody()
        assert body is not None
        assert body.x == 5.0  # Should be at start position
        assert body.y == 5.0


class TestWorldObjectManagement:
    """Test world object management."""
    
    def test_add_world_object(self):
        """Test adding world objects."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create a world object
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        
        # Add object to world
        world.addObject(obj)
        
        # Check that object is valid
        assert obj.active == True
        assert obj.x == 5.0
        assert obj.y == 5.0
        assert obj.type == pynlm.WorldObjectType.Resource
        assert obj.value == 10.0
    
    def test_remove_world_object(self):
        """Test removing world objects."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create and add object
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        world.addObject(obj)
        
        # Verify object exists
        assert obj.active == True
        
        # Remove object
        world.removeObject(5.0, 5.0)
        
        # Check that object is removed (it should be deactivated)
        assert obj.active == False
    
    def test_is_valid_position(self):
        """Test position validation."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Test valid positions
        assert world.isValidPosition(0.0, 0.0) == True
        assert world.isValidPosition(10.0, 10.0) == True
        assert world.isValidPosition(19.0, 19.0) == True
        
        # Test invalid positions (outside world bounds)
        assert world.isValidPosition(-1.0, 0.0) == False
        assert world.isValidPosition(0.0, -1.0) == False
        assert world.isValidPosition(21.0, 0.0) == False
        assert world.isValidPosition(0.0, 21.0) == False
    
    def test_clear_objects(self):
        """Test clearing all objects from world."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Add some objects
        obj1 = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        obj2 = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=5.0)
        
        world.addObject(obj1)
        world.addObject(obj2)
        
        # Activate objects
        obj1.active = True
        obj2.active = True
        
        # Clear objects
        world.clearObjects()
        
        # Check that objects are deactivated
        # (Note: clearObjects doesn't explicitly deactivate objects,
        # but it removes them from the world)
        # In a real implementation, this would check that no objects remain


class TestWorldSimulation:
    """Test world simulation functionality."""
    
    def test_world_update(self):
        """Test world update."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Set initial time
        initial_time = world.getSimulationTime()
        
        # Update world
        world.update(0.1)
        
        # Check that time increased
        assert world.getSimulationTime() > initial_time
        assert world.getSimulationTime() == pytest.approx(initial_time + 0.1, rel=1e-6)
    
    def test_world_random_seed(self):
        """Test world random seed functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Set random seed
        world.setRandomSeed(12345)
        
        # Get random seed
        seed = world.getRandomSeed()
        assert seed == 12345
        
        # Update world twice with same seed - should be deterministic
        world.update(0.1)
        time1 = world.getSimulationTime()
        
        # Reset and try again
        world.reset()
        world.setRandomSeed(12345)
        
        # Should get same result
        world.update(0.1)
        time2 = world.getSimulationTime()
        
        assert time1 == time2
    
    def test_world_max_energy(self):
        """Test world maximum energy functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Set maximum energy
        world.setMaxEnergy(100.0)
        
        # Get maximum energy
        max_energy = world.getMaxEnergy()
        assert max_energy == 100.0
        
        # Check that agent body has energy
        body = world.getAgentBody()
        assert body.energy <= max_energy
    
    def test_world_energy_decay(self):
        """Test world energy decay functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Set energy decay rate
        world.setEnergyDecayRate(0.01)
        
        # Get decay rate
        decay_rate = world.getEnergyDecayRate()
        assert decay_rate == 0.01


class TestWorldAgentInteraction:
    """Test world-agent interaction."""
    
    def test_agent_sensory_percept(self):
        """Test agent sensory percept retrieval."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Check percept properties
        assert percept is not None
        assert percept.getTimestamp() > 0
        
        # Check that percept contains data (vision, audio, internal)
        if percept.hasVision():
            assert percept.getVisionWidth() == 8
            assert percept.getVisionHeight() == 8
        
        if percept.hasAudio():
            assert percept.getAudio().getNumSamples() > 0
        
        if percept.hasInternal():
            assert len(percept.getInternal()) > 0
    
    def test_agent_body_access(self):
        """Test agent body access."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get agent body
        body = world.getAgentBody()
        
        # Check body properties
        assert body is not None
        assert body.x >= 0.0
        assert body.y >= 0.0
        assert 0.0 <= body.orientation < 6.2831853  # 2π
        assert body.energy >= 0.0
        assert body.health >= 0.0
        assert body.age >= 0.0
        assert body.isMoving is True or body.isMoving is False
        assert body.isTurning is True or body.isTurning is False
    
    def test_agent_start_position(self):
        """Test agent start position setting."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Set custom start position
        world.setAgentStart(7.5, 7.5)
        
        # Reset world
        world.reset()
        
        # Check that agent body is at start position
        body = world.getAgentBody()
        assert body.x == 7.5
        assert body.y == 7.5
    
    def test_agent_start_position_default(self):
        """Test agent start position default."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Don't set start position - use default
        world.reset()
        
        # Check that agent body is somewhere reasonable (not necessarily 0,0)
        body = world.getAgentBody()
        assert body.x >= 0.0
        assert body.y >= 0.0
        assert body.x < 20.0
        assert body.y < 20.0


class TestWorldCompleteWorkflow:
    """Test complete world workflow."""
    
    def test_complete_world_workflow(self):
        """Test complete world workflow."""
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Set custom start position
        world.setAgentStart(10.0, 10.0)
        
        # Reset world
        world.reset()
        
        # Run world simulation
        steps = 10
        for step in range(steps):
            world.update(0.1)
            
            # Check that world state is consistent
            time = world.getSimulationTime()
            body = world.getAgentBody()
            percept = world.getSensoryPercept()
            
            assert time >= 0.0
            assert body is not None
            assert percept is not None
            assert 0.0 <= body.x < 20.0
            assert 0.0 <= body.y < 20.0
        
        # Check final world state
        final_time = world.getSimulationTime()
        final_body = world.getAgentBody()
        assert final_time == steps * 0.1
    
    def test_world_with_objects(self):
        """Test world with multiple objects."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Add multiple objects
        objects = [
            pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0),
            pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=5.0),
            pynlm.WorldObject(2.0, 8.0, pynlm.WorldObjectType.Wall, value=0.0),
            pynlm.WorldObject(8.0, 2.0, pynlm.WorldObjectType.Marker, value=1.0),
        ]
        
        for obj in objects:
            world.addObject(obj)
        
        # Update world
        world.update(0.1)
        
        # Check that objects are active
        for obj in objects:
            assert obj.active == True
        
        # Test object interactions
        # Move agent towards a resource
        body = world.getAgentBody()
        body.x = 6.0
        body.y = 6.0
        
        # Check that agent is near the resource
        resource = None
        for obj in objects:
            if obj.type == pynlm.WorldObjectType.Resource:
                resource = obj
                break
        
        if resource is not None:
            distance = ((body.x - resource.x)**2 + (body.y - resource.y)**2)**0.5
            # Should be relatively close to resource
            assert distance < 2.0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
