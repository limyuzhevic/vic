"""NLM Agent module - Agent brain interface for world interaction"""

import sys
import os
from typing import List, Optional

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

import pynlm

__all__ = [
    'AgentBrain',
    'create_agent_brain',
    'AgentBrainUtils',
    'SensoryPercept',
    'WorldObject',
    'AgentBody',
    'ActionResult',
]

# Re-export main classes
AgentBrain = pynlm.AgentBrain
create_agent_brain = pynlm.createAgentBrain
SensoryPercept = pynlm.SensoryPercept
WorldObject = pynlm.WorldObject
AgentBody = pynlm.AgentBody
ActionResult = pynlm.ActionResult

class AgentBrainUtils:
    """Utility methods for working with agent brains"""
    
    @staticmethod
    def enable_all_subsystems(agent_brain, enabled=True):
        """Enable/disable all agent subsystems"""
        agent_brain.enableRewardModulation(enabled)
        agent_brain.enableStructuralPlasticity(enabled)
        agent_brain.enableDevelopment(enabled)
        agent_brain.enableCuriosity(enabled)
        return agent_brain
    
    @staticmethod
    def get_subsystem_status(agent_brain):
        """Get the status of all agent subsystems"""
        return {
            'reward_modulation': agent_brain.isRewardModulationEnabled(),
            'structural_plasticity': agent_brain.isStructuralPlasticityEnabled(),
            'development': agent_brain.isDevelopmentEnabled(),
            'curiosity': agent_brain.isCuriosityEnabled(),
        }
    
    @staticmethod
    def simulate_episode(agent_brain, world, steps, timestep=0.1):
        """Simulate a complete episode"""
        for step in range(steps):
            world.update(timestep)
            agent_brain.processSensoryInput(world.getSensoryPercept())
            # Note: This would need access to the underlying brain
            # For a complete simulation, you'd need to call brain.step()
            
            motor_cmd = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward (simplified - would get from world)
            agent_brain.applyRewardModulation(0.0, 0.0)
            
            # Update development
            agent_brain.updateDevelopment(timestep)
        
        return agent_brain
    
    @staticmethod
    def create_learning_scenario(agent_brain, world, scenario='navigation'):
        """Create different learning scenarios"""
        if scenario == 'navigation':
            # Simple navigation scenario
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.setAgentStart(5.0, 5.0)
        elif scenario == 'resource_collection':
            # Resource collection scenario
            world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
            world.setAgentStart(10.0, 10.0)
        elif scenario == 'avoidance':
            # Obstacle avoidance scenario
            world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
            world.setAgentStart(2.0, 2.0)
        
        agent_brain.initialize(world)
        return agent_brain, world

# Scenario generators
class ScenarioFactory:
    """Create different simulation scenarios"""
    
    @staticmethod
    def create_simple_environment():
        """Create a simple test environment"""
        return pynlm.createSimpleWorld()
    
    @staticmethod
    def create_grid_environment(width, height, vision_width, vision_height):
        """Create a grid-based environment"""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        return world
    
    @staticmethod
    def create_resource_environment():
        """Create an environment with resources"""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Add some resources
        for x in range(5, 15):
            for y in range(5, 15):
                if x % 4 == 0 and y % 4 == 0:
                    resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
                    world.addObject(resource)
        
        return world
    
    @staticmethod
    def create_hazard_environment():
        """Create an environment with hazards"""
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        
        # Add some hazards
        for x in range(3, 22, 6):
            for y in range(3, 22, 6):
                hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=2.0)
                world.addObject(hazard)
        
        return world
