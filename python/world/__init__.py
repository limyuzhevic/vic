"""NLM World module - Environment and simulation world"""

import sys
import os
from typing import List, Optional

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

import pynlm

__all__ = [
    'SimpleWorld',
    'WorldObject',
    'AgentBody',
    'SensoryPercept',
    'ActionResult',
    'create_simple_world',
    'WorldUtils',
    'EnvironmentFactory',
]

# Re-export classes
SimpleWorld = pynlm.SimpleWorld
WorldObject = pynlm.WorldObject
AgentBody = pynlm.AgentBody
SensoryPercept = pynlm.SensoryPercept
ActionResult = pynlm.ActionResult

# Factory function
create_simple_world = pynlm.createSimpleWorld

class WorldUtils:
    """Utility methods for working with worlds"""
    
    @staticmethod
    def get_visible_objects(world, agent_x, agent_y, vision_range):
        """Get objects within agent's vision range"""
        visible = []
        for obj in world.getObjects():
            distance = ((obj.x - agent_x) ** 2 + (obj.y - agent_y) ** 2) ** 0.5
            if distance <= vision_range:
                visible.append({
                    'object': obj,
                    'distance': distance,
                    'angle': 0.0  # Simplified - would calculate actual angle
                })
        return visible
    
    @staticmethod
    def print_world_map(world, agent_x, agent_y, scale=1.0):
        """Print a simple text representation of the world"""
        width = int(world.getWidth() * scale)
        height = int(world.getHeight() * scale)
        
        for y in range(height - 1, -1, -1):
            row = ""
            for x in range(width):
                world_x = x / scale
                world_y = y / scale
                
                # Check for agent
                if abs(world_x - agent_x) < scale/2 and abs(world_y - agent_y) < scale/2:
                    row += "@"
                # Check for objects
                elif world.isValidPosition(world_x, world_y):
                    row += " "
                else:
                    row += "#"
            print(row)
    
    @staticmethod
    def get_world_statistics(world):
        """Get statistics about the world"""
        stats = {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy(),
            'current_energy': world.getAgentBody().energy,
            'health': world.getAgentBody().health,
            'age': world.getAgentBody().age,
            'is_moving': world.getAgentBody().isMoving,
            'is_turning': world.getAgentBody().isTurning,
        }
        return stats
    
    @staticmethod
    def reset_world(world, reset_to_start=True):
        """Reset world to initial state"""
        if reset_to_start:
            agent_body = world.getAgentBody()
            world.setAgentStart(agent_body.x, agent_body.y)
        world.reset()
        return world

class EnvironmentFactory:
    """Factory for creating different types of environments"""
    
    @staticmethod
    def create_training_environment():
        """Create a simple training environment"""
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.setAgentStart(5.0, 5.0)
        
        # Add some resources
        for x in range(10, 25, 4):
            for y in range(10, 25, 4):
                resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
                world.addObject(resource)
        
        # Add some walls
        for x in range(0, 30, 3):
            wall = pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall)
            world.addObject(wall)
            wall = pynlm.WorldObject(x, 29, pynlm.WorldObjectType.Wall)
            world.addObject(wall)
        
        for y in range(0, 30, 3):
            wall = pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall)
            world.addObject(wall)
            wall = pynlm.WorldObject(29, y, pynlm.WorldObjectType.Wall)
            world.addObject(wall)
        
        return world
    
    @staticmethod
    def create_challenge_environment():
        """Create a challenging environment with hazards"""
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.setAgentStart(5.0, 5.0)
        world.setMaxEnergy(200.0)
        
        # Add resources (good)
        for x in range(10, 35, 5):
            for y in range(10, 35, 5):
                resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=2.0)
                world.addObject(resource)
        
        # Add hazards (bad)
        hazard_positions = [(8, 8), (15, 15), (22, 8), (8, 22), (22, 22)]
        for x, y in hazard_positions:
            hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=5.0)
            world.addObject(hazard)
        
        # Add walls
        for x in range(0, 40, 4):
            for y in [0, 39]:
                wall = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall)
                world.addObject(wall)
        
        for y in range(0, 40, 4):
            for x in [0, 39]:
                wall = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall)
                world.addObject(wall)
        
        return world
    
    @staticmethod
    def create_open_environment():
        """Create an open, obstacle-free environment"""
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        world.setAgentStart(25.0, 25.0)
        
        # Add scattered resources
        for x in range(10, 45, 6):
            for y in range(10, 45, 6):
                if (x + y) % 12 == 0:  # Create a pattern
                    resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
                    world.addObject(resource)
        
        return world
