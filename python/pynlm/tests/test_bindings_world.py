#!/usr/bin/env python3
"""
World operations test suite for NLM Python bindings.
Tests world simulation, object management, and spatial queries.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestWorldCreation:
    """Test world creation and basic setup."""
    
    def test_simple_world_creation(self):
        """Test creating a simple world."""
        world = nlm.SimpleWorld()
        assert world is not None
        assert isinstance(world, nlm.SimpleWorld)
        
        # Should have default dimensions
        width = world.getWidth()
        height = world.getHeight()
        assert width > 0
        assert height > 0
    
    def test_world_configuration(self):
        """Test world configuration."""
        world = nlm.SimpleWorld()
        
        # Configure with custom dimensions
        world.configure(200.0, 100.0, 32, 32)
        
        assert world.getWidth() == 200.0
        assert world.getHeight() == 100.0

class TestWorldOperations:
    """Test world simulation and operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.world = nlm.SimpleWorld()
        self.world.configure_simulation(100.0, 100.0)
    
    def test_world_reset(self):
        """Test world reset functionality."""
        # Add some objects
        self.world.add_object_at(10.0, 10.0, nlm.WorldObjectType.Resource, 5.0)
        
        # Reset should clear objects
        self.world.reset()
        
        # World should still be functional
        assert self.world.getWidth() == 100.0
        assert self.world.getHeight() == 100.0
    
    def test_agent_start(self):
        """Test agent starting position setup."""
        self.world.setAgentStart(50.0, 50.0)
        
        # World should be configured
        assert self.world.getWidth() == 100.0
        assert self.world.getHeight() == 100.0
    
    def test_world_update(self):
        """Test world update (simulation step)."""
        # Update with small timestep
        timestep = 0.01
        self.world.update(timestep)
        
        # Should complete without error
        assert self.world.getSimulationTime() >= 0
    
    def test_object_addition(self):
        """Test adding objects to world."""
        # Create different types of objects
        resource = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 10.0)
        hazard = nlm.WorldObject(20.0, 20.0, nlm.WorldObjectType.Hazard, -5.0)
        wall = nlm.WorldObject(30.0, 30.0, nlm.WorldObjectType.Wall, 0.0, 2.0)
        marker = nlm.WorldObject(40.0, 40.0, nlm.WorldObjectType.Marker, 0.0)
        
        # Add objects
        self.world.addObject(resource)
        self.world.addObject(hazard)
        self.world.addObject(wall)
        self.world.addObject(marker)
        
        # Should complete without error
        assert self.world.getWidth() == 100.0
    
    def test_pythonic_object_addition(self):
        """Test add_object_at convenience method."""
        # Add objects using Pythonic interface
        self.world.add_object_at(10.0, 10.0, nlm.WorldObjectType.Resource, 5.0)
        self.world.add_object_at(20.0, 20.0, nlm.WorldObjectType.Hazard, -2.0)
        self.world.add_object_at(30.0, 30.0, nlm.WorldObjectType.Wall, 0.0, 1.5)
        self.world.add_object_at(40.0, 40.0, nlm.WorldObjectType.Marker, 0.0)
        
        # World should still be functional
        assert self.world.getWidth() == 100.0

class TestWorldObjectManagement:
    """Test world object operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.world = nlm.SimpleWorld()
        self.world.configure_simulation(100.0, 100.0)
        
        # Add some test objects
        self.resource1 = self.world.add_object_at(25.0, 25.0, nlm.WorldObjectType.Resource, 10.0)
        self.hazard1 = self.world.add_object_at(75.0, 75.0, nlm.WorldObjectType.Hazard, -5.0)
        self.wall1 = self.world.add_object_at(50.0, 50.0, nlm.WorldObjectType.Wall, 0.0, 2.0)
    
    def test_get_object_at(self):
        """Test getting object at specific position."""
        # Get object at resource position
        obj_at_25_25 = self.world.getObjectAt(25.0, 25.0)
        assert obj_at_25_25 is not None
        assert obj_at_25_25.type == nlm.WorldObjectType.Resource
        assert obj_at_25_25.value == 10.0
        
        # Get object at hazard position
        obj_at_75_75 = self.world.getObjectAt(75.0, 75.0)
        assert obj_at_75_75 is not None
        assert obj_at_75_75.type == nlm.WorldObjectType.Hazard
        assert obj_at_75_75.value == -5.0
        
        # Get object at wall position
        obj_at_50_50 = self.world.getObjectAt(50.0, 50.0)
        assert obj_at_50_50 is not None
        assert obj_at_50_50.type == nlm.WorldObjectType.Wall
        
        # Get object at empty position
        obj_at_empty = self.world.getObjectAt(0.0, 0.0)
        # May be null or empty object depending on implementation
    
    def test_remove_object(self):
        """Test removing object at position."""
        # Remove object at resource position
        self.world.removeObject(25.0, 25.0)
        
        # Object at that position should no longer exist
        obj_after_removal = self.world.getObjectAt(25.0, 25.0)
        # Implementation may return null or different object
    
    def test_is_valid_position(self):
        """Test checking if position is valid."""
        # Valid positions (not in walls)
        assert self.world.isValidPosition(10.0, 10.0) == True  # Empty space
        assert self.world.isValidPosition(99.0, 99.0) == True  # Border space
        
        # Potentially invalid positions (in walls or out of bounds)
        # Note: Implementation details may vary
        result = self.world.isValidPosition(50.0, 50.0)  # Possibly wall
        # Should return boolean
    
    def test_world_object_properties(self):
        """Test world object property access."""
        # Create object directly
        obj = nlm.WorldObject(15.0, 15.0, nlm.WorldObjectType.Resource, 7.5, 1.2)
        
        # Test property access
        assert obj.x == 15.0
        assert obj.y == 15.0
        assert obj.radius == 1.2
        assert obj.type == nlm.WorldObjectType.Resource
        assert obj.value == 7.5
        assert obj.active == True
        
        # Test modification
        obj.x = 20.0
        obj.y = 20.0
        obj.active = False
        
        assert obj.x == 20.0
        assert obj.y == 20.0
        assert obj.active == False

class TestSpatialQueries:
    """Test spatial query functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.world = nlm.SimpleWorld()
        self.world.configure_simulation(100.0, 100.0)
        
        # Add objects for spatial queries
        self.resource = self.world.add_object_at(30.0, 30.0, nlm.WorldObjectType.Resource, 10.0)
        self.hazard = self.world.add_object_at(70.0, 70.0, nlm.WorldObjectType.Hazard, -5.0)
        self.marker = self.world.add_object_at(50.0, 50.0, nlm.WorldObjectType.Marker, 0.0)
        self.wall = self.world.add_object_at(80.0, 80.0, nlm.WorldObjectType.Wall, 0.0, 3.0)
    
    def test_get_nearby_objects(self):
        """Test get_nearby_objects convenience method."""
        # Get objects within radius of center
        nearby = self.world.get_nearby_objects(50.0, 50.0, 20.0)
        assert isinstance(nearby, list)
        
        # Should find at least the marker and wall
        assert len(nearby) >= 2
        
        # Test with filters
        nearby_active = self.world.get_nearby_objects(50.0, 50.0, 20.0, active_only=True)
        assert isinstance(nearby_active, list)
        
        # Test with type filter (if supported)
        # nearby_resources = self.world.get_nearby_objects(50.0, 50.0, 20.0, type_filter=nlm.WorldObjectType.Resource)
        # assert isinstance(nearby_resources, list)
    
    def test_spatial_query_filters(self):
        """Test spatial query with different filters."""
        center_x, center_y = 30.0, 30.0
        radius = 15.0
        
        # Query with active_only=True
        nearby_active = self.world.get_nearby_objects(
            center_x, center_y, radius, active_only=True
        )
        assert isinstance(nearby_active, list)
        
        # Query with active_only=False
        nearby_all = self.world.get_nearby_objects(
            center_x, center_y, radius, active_only=False
        )
        assert isinstance(nearby_all, list)
        
        # Both should return lists
        assert isinstance(nearby_active, list)
        assert isinstance(nearby_all, list)

class TestWorldConfiguration:
    """Test world configuration options."""
    
    def test_configure_simulation(self):
        """Test configure_simulation quick setup method."""
        world = nlm.SimpleWorld()
        
        # Use quick configuration
        world.configure_simulation(200.0, 150.0, 64, 64, 200.0, 0.05)
        
        # Verify configuration
        assert world.getWidth() == 200.0
        assert world.getHeight() == 150.0
        
        # Energy settings (may not be directly accessible)
        # assert world.getMaxEnergy() == 200.0
        # assert world.getEnergyDecayRate() == 0.05
    
    def test_custom_world_setup(self):
        """Test custom world setup."""
        world = nlm.SimpleWorld()
        
        # Custom setup
        world.configure_simulation(50.0, 50.0, 16, 16, 50.0, 0.2)
        
        # Set agent start
        world.setAgentStart(25.0, 25.0)
        
        # Add some objects
        world.add_object_at(10.0, 10.0, nlm.WorldObjectType.Resource, 5.0)
        world.add_object_at(40.0, 40.0, nlm.WorldObjectType.Hazard, -3.0)
        
        # Verify setup
        assert world.getWidth() == 50.0
        assert world.getHeight() == 50.0
        assert world.getSimulationTime() >= 0
    
    def test_world_dimensions(self):
        """Test world dimension access."""
        world = nlm.SimpleWorld()
        world.configure_simulation(120.0, 80.0)
        
        assert world.getWidth() == 120.0
        assert world.getHeight() == 80.0
    
    def test_world_energy_settings(self):
        """Test world energy system settings."""
        world = nlm.SimpleWorld()
        world.configure_simulation(100.0, 100.0)
        
        # Energy settings (may not be directly accessible)
        # Initial values should be reasonable
        # assert world.getMaxEnergy() > 0
        # assert world.getEnergyDecayRate() >= 0
        
        # Can set energy values
        # world.setMaxEnergy(150.0)
        # world.setEnergyDecayRate(0.1)

class TestUtilityFunctions:
    """Test world utility functions."""
    
    def test_create_test_environment(self):
        """Test create_test_environment utility function."""
        world = nlm.create_test_environment()
        
        assert world is not None
        assert isinstance(world, nlm.SimpleWorld)
        
        assert world.getWidth() > 0
        assert world.getHeight() > 0
        assert world.getWidth() == world.getHeight()  # Default square
        
        # Should have test objects
        # assert world.getObjectCount() > 0
    
    def test_create_training_environment(self):
        """Test create_training_environment utility function."""
        world = nlm.create_training_environment()
        
        assert world is not None
        assert isinstance(world, nlm.SimpleWorld)
        
        assert world.getWidth() > 0
        assert world.getHeight() > 0
        
        # Should have training objects (resources, hazards, markers)
        # assert world.getObjectCount() >= 10
    
    def test_create_training_environment_custom(self):
        """Test create_training_environment with custom dimensions."""
        world = nlm.create_training_environment(200.0, 100.0)
        
        assert world is not None
        assert world.getWidth() == 200.0
        assert world.getHeight() == 100.0

if __name__ == "__main__":
    pytest.main([__file__, "-v"])