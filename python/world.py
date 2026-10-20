"""
World NLM functionality.

This module contains world-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import SimpleWorld, createSimpleWorld as create_simple_world_native

def create_enhanced_world(width: int = 20, height: int = 20,
                          vision_width: int = 8, vision_height: int = 8,
                          config: Optional[Dict[str, Any]] = None) -> SimpleWorld:
    """
    Create an enhanced world with additional features.
    
    Args:
        width: World width in cells.
        height: World height in cells.
        vision_width: Agent's vision width.
        vision_height: Agent's vision height.
        config: Optional configuration dictionary.
        
    Returns:
        Enhanced SimpleWorld instance.
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    
    if config:
        # Apply world-specific configuration
        if 'max_energy' in config:
            world.setMaxEnergy(config['max_energy'])
        
        if 'energy_decay_rate' in config:
            world.setEnergyDecayRate(config['energy_decay_rate'])
        
        if 'random_seed' in config:
            world.setRandomSeed(config['random_seed'])
    
    return world

def create_resource_world(width: int = 20, height: int = 20,
                          resource_density: float = 0.3,
                          config: Optional[Dict[str, Any]] = None) -> SimpleWorld:
    """
    Create a world with resources.
    
    Args:
        width: World width in cells.
        height: World height in cells.
        resource_density: Density of resources (0.0 to 1.0).
        config: Optional configuration dictionary.
        
    Returns:
        Resource world instance.
    """
    world = create_enhanced_world(width, height, 8, 8, config)
    
    # Add resources
    import random
    for x in range(width):
        for y in range(height):
            if random.random() < resource_density:
                # Add a resource object
                obj = WorldObject(x, y, WorldObjectType.Resource, value=1.0)
                world.addObject(obj)
    
    return world

def create_hazard_world(width: int = 20, height: int = 20,
                       hazard_density: float = 0.1,
                       config: Optional[Dict[str, Any]] = None) -> SimpleWorld:
    """
    Create a world with hazards.
    
    Args:
        width: World width in cells.
        height: World height in cells.
        hazard_density: Density of hazards (0.0 to 1.0).
        config: Optional configuration dictionary.
        
    Returns:
        Hazard world instance.
    """
    world = create_enhanced_world(width, height, 8, 8, config)
    
    # Add hazards
    import random
    for x in range(width):
        for y in range(height):
            if random.random() < hazard_density:
                # Add a hazard object
                obj = WorldObject(x, y, WorldObjectType.Hazard, value=0.5)
                world.addObject(obj)
    
    return world

def simulate_environment(world: SimpleWorld, steps: int,
                         agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
    """
    Run a simulation environment for a specified number of steps.
    
    Args:
        world: World instance to simulate.
        steps: Number of steps to simulate.
        agent: Optional agent brain to interact with the world.
        
    Returns:
        Dictionary containing simulation results.
    """
    results = {
        'steps': 0,
        'agent_actions': [],
        'world_objects': [],
        'success': True,
        'error': None,
    }
    
    try:
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get world state
            results['world_objects'].append({
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'agent_body': {
                    'x': world.getAgentBody().x,
                    'y': world.getAgentBody().y,
                    'orientation': world.getAgentBody().orientation,
                    'energy': world.getAgentBody().energy,
                }
            })
            
            # If agent is provided, let it interact with the world
            if agent:
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                results['agent_actions'].append(action)
        
        results['steps'] = steps
        
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def get_world_statistics(world: SimpleWorld) -> Dict[str, Any]:
    """
    Get statistics about a world.
    
    Args:
        world: World instance to analyze.
        
    Returns:
        Dictionary containing world statistics.
    """
    return {
        'width': world.getWidth(),
        'height': world.getHeight(),
        'simulation_time': world.getSimulationTime(),
        'max_energy': world.getMaxEnergy(),
        'energy_decay_rate': world.getEnergyDecayRate(),
        'random_seed': world.getRandomSeed(),
    }

def create_test_world(config: Optional[Dict[str, Any]] = None) -> SimpleWorld:
    """
    Create a test world with obstacles and resources for testing.
    
    Args:
        config: Optional configuration dictionary.
        
    Returns:
        Test world instance.
    """
    world = create_enhanced_world(15, 15, 6, 6, config)
    
    # Add walls as boundaries
    for x in range(15):
        obj = WorldObject(x, 0, WorldObjectType.Wall)
        world.addObject(obj)
        obj = WorldObject(x, 14, WorldObjectType.Wall)
        world.addObject(obj)
    
    for y in range(1, 14):
        obj = WorldObject(0, y, WorldObjectType.Wall)
        world.addObject(obj)
        obj = WorldObject(14, y, WorldObjectType.Wall)
        world.addObject(obj)
    
    # Add some resources
    import random
    for _ in range(20):
        x = random.randint(2, 12)
        y = random.randint(2, 12)
        obj = WorldObject(x, y, WorldObjectType.Resource, value=1.0)
        world.addObject(obj)
    
    return world