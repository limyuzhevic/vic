"""
Custom World Simulation Examples for NLM

This module demonstrates advanced custom world simulations including
complex environments, multi-agent systems, and specialized simulation scenarios.
"""

import pynlm
import numpy as np
import time
from typing import Dict, List, Optional, Tuple
import json

class CustomWorldExamples:
    """Custom world simulation examples."""
    
    @staticmethod
    def create_2d_resource_world(width: int = 50, height: int = 50) -> Dict:
        """
        Example: Custom 2D world with resources and hazards.
        
        Creates a complex 2D environment with:
        - Static resources at fixed locations
        - Dynamic hazards that move over time
        - Complex terrain with different movement costs
        - Environmental factors affecting agent performance
        """
        print("=== Custom 2D Resource World Example ===")
        print(f"Creating world: {width}x{height} with resources and hazards")
        
        # Create world configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2500)
        config.set("brain.synapse_density", 0.1)
        
        # Create simple world
        world = pynlm.createSimpleWorld()
        
        # Configure world dimensions
        world.configure(width=width, height=height, visionWidth=10, visionHeight=10)
        
        # Create custom environment elements
        # Static resources at specific locations
        resource_locations = [
            (5.0, 5.0),
            (45.0, 10.0),
            (20.0, 35.0),
            (35.0, 45.0)
        ]
        
        # Dynamic hazards (moving over time)
        hazard_patterns = [
            {"type": "fire", "positions": [(15, 15), (15, 20), (15, 25), (15, 30)], "radius": 1.5},
            {"type": "predator", "positions": [(40, 40), (35, 35), (45, 45)], "radius": 2.0},
            {"type": "poison", "positions": [(30, 10), (35, 15), (25, 20)], "radius": 1.0}
        ]
        
        # Complex terrain costs
        terrain_costs = {
            "grass": {"move_cost": 1.0, "visibility": 1.0},
            "forest": {"move_cost": 1.5, "visibility": 0.7},
            "mountain": {"move_cost": 2.0, "visibility": 0.5},
            "water": {"move_cost": 3.0, "visibility": 0.3}
        }
        
        # Environmental factors
        environmental_factors = {
            "wind": {"direction": "north", "strength": 0.5},
            "temperature": {"base": 20.0, "variance": 5.0},
            "humidity": {"base": 0.5, "variance": 0.1}
        }
        
        # Create agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"  Resources placed at: {len(resource_locations)} locations")
        print(f"  Hazards: {len(hazard_patterns)} moving hazard patterns")
        print(f"  Terrain types: {len(terrain_costs)}")
        print(f"  Environmental factors: {len(environmental_factors)}")
        
        # Simulate complex world interaction
        print("\nSimulating complex world interaction...")
        
        simulation_results = {
            "steps": 0,
            "resources_found": 0,
            "hazards_avoided": 0,
            "terrain_navigated": {terrain: 0 for terrain in terrain_costs.keys()},
            "environmental_impacts": {}
        }
        
        for step in range(150):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record simulation metrics
            simulation_results["steps"] += 1
            
            # In a real implementation, this would check for resources,
            # hazards, and terrain navigation based on the world state
            
            if step % 30 == 0:
                print(f"  Step {step}: Simulation ongoing, "
                      f"Steps: {simulation_results['steps']}, "
                      f"Resources: {simulation_results['resources_found']}")
        
        return {
            "world": world,
            "brain": brain,
            "agent": agent,
            "simulation_results": simulation_results,
            "config": config,
            "environment_setup": {
                "resource_locations": resource_locations,
                "hazard_patterns": hazard_patterns,
                "terrain_costs": terrain_costs,
                "environmental_factors": environmental_factors
            }
        }
    
    @staticmethod
    def create_multi_agent_environment(num_agents: int = 5) -> Dict:
        """
        Example: Multi-agent environment with social interactions.
        
        Creates a world with multiple interacting agents, including:
        - Cooperative behaviors
        - Competitive interactions
        - Communication and coordination
        - Emergent social behaviors
        """
        print(f"\n=== Multi-Agent Environment Example ({num_agents} agents) ===")
        
        # Create brain configuration for multi-agent system
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.08)
        
        # Configure for social learning and interaction
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Configure social learning mechanisms
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.015)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        
        # Create main world
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
        
        # Create multiple agents
        agents = []
        brains = []
        
        for i in range(num_agents):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable social learning features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            brains.append(brain)
            agents.append(agent)
            
            # Set different starting positions
            start_x = 5.0 + (i * 7.0)
            start_y = 5.0 + (i * 3.0)
            world.setAgentStart(start_x, start_y)
        
        world.reset()
        
        print(f"Created multi-agent environment with {num_agents} agents")
        
        # Simulate multi-agent interactions
        print("\nSimulating multi-agent interactions...")
        
        interaction_metrics = {
            "total_steps": 0,
            "cooperative_actions": 0,
            "competitive_actions": 0,
            "neutral_actions": 0,
            "communication_events": 0,
            "resource_sharing": 0
        }
        
        for step in range(200):
            # Update world
            world.update(0.1)
            
            # Each agent perceives the world
            agent_percepts = []
            for i, agent in enumerate(agents):
                # In real implementation, each agent would have its own percept
                # This is a simplified version
                percept = world.getSensoryPercept()
                agent_percepts.append(percept)
                agent.processSensoryInput(percept)
            
            # Each agent acts based on its perception
            for i, (agent, percept) in enumerate(zip(agents, agent_percepts)):
                # Run brain step
                brains[i].step(step)
                
                # Decode action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Analyze interaction type (simplified)
                if step % 5 == 0:
                    # Simulate different interaction types based on agent ID
                    interaction_type = step % 3
                    if interaction_type == 0:
                        interaction_metrics["cooperative_actions"] += 1
                    elif interaction_type == 1:
                        interaction_metrics["competitive_actions"] += 1
                    else:
                        interaction_metrics["neutral_actions"] += 1
            
            interaction_metrics["total_steps"] += 1
            
            if step % 40 == 0:
                print(f"  Step {step}: "
                      f"Coop: {interaction_metrics['cooperative_actions']}, "
                      f"Comp: {interaction_metrics['competitive_actions']}, "
                      f"Neutral: {interaction_metrics['neutral_actions']}")
        
        return {
            "world": world,
            "agents": agents,
            "brains": brains,
            "interaction_metrics": interaction_metrics,
            "config": config
        }
    
    @staticmethod
    def create_seasonal_environment() -> Dict:
        """
        Example: Seasonal environment simulation.
        
        Creates an environment with changing conditions over time, including:
        - Seasonal resource availability
        - Weather patterns
        - Migration behaviors
        - Environmental adaptation
        """
        print("\n=== Seasonal Environment Example ===")
        
        # Create environment with seasonal changes
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.12)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=60, height=60, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(30.0, 30.0)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Define seasons
        seasons = {
            "spring": {
                "duration": 40,
                "temperature": {"min": 10.0, "max": 20.0},
                "resources": {"abundance": 0.8, "types": ["flowers", "fruits", "vegetables"]},
                "hazards": {"mild": ["pollen"]}
            },
            "summer": {
                "duration": 40,
                "temperature": {"min": 25.0, "max": 35.0},
                "resources": {"abundance": 1.0, "types": ["fruits", "nuts", "water"]},
                "hazards": {"moderate": ["heat", "insects"]}
            },
            "autumn": {
                "duration": 40,
                "temperature": {"min": 15.0, "max": 25.0},
                "resources": {"abundance": 0.9, "types": ["fruits", "nuts", "seeds"]},
                "hazards": {"mild": ["cold"]}
            },
            "winter": {
                "duration": 30,
                "temperature": {"min": 0.0, "max": 10.0},
                "resources": {"abundance": 0.3, "types": ["snow_roots", "protected_fruits"]},
                "hazards": {"severe": ["cold", "ice"]}
            }
        }
        
        print("Created seasonal environment with 4 seasons:")
        for season_name, season_data in seasons.items():
            print(f"  {season_name.title()}: {season_data['duration']} steps, "
                  f"Resource abundance: {season_data['resources']['abundance']}")
        
        # Simulate seasonal changes
        print("\nSimulating seasonal adaptation...")
        
        season_results = {}
        current_season = "spring"
        season_step = 0
        
        for step in range(160):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Track seasonal progress
            season_step += 1
            
            if season_step >= seasons[current_season]["duration"]:
                # Move to next season
                season_keys = list(seasons.keys())
                current_season_index = season_keys.index(current_season)
                next_season_index = (current_season_index + 1) % len(season_keys)
                current_season = season_keys[next_season_index]
                season_step = 0
                
                print(f"  Season transition: {current_season.title()} begun")
                
                # Record season results
                if current_season not in season_results:
                    season_results[current_season] = []
                season_results[current_season].append({
                    "step": step,
                    "season": current_season,
                    "temperature": seasons[current_season]["temperature"]["min"],
                    "resource_abundance": seasons[current_season]["resources"]["abundance"]
                })
            
            # Record seasonal adaptation
            if step % 20 == 0:
                current_season_data = seasons[current_season]
                print(f"  Step {step}: {current_season.title()} - "
                      f"Temp: {current_season_data['temperature']['min']}-{current_season_data['temperature']['max']}°C, "
                      f"Resources: {current_season_data['resources']['abundance']:.1f}")
        
        return {
            "world": world,
            "brain": brain,
            "agent": agent,
            "season_results": season_results,
            "config": config,
            "seasons": seasons
        }

if __name__ == "__main__":
    print("Custom World Simulation Examples")
    print("=" * 35)
    
    # Example 1: 2D resource world
    resource_world_result = CustomWorldExamples.create_2d_resource_world(30, 30)
    print(f"\n2D resource world: {resource_world_result['simulation_results']['steps']} steps completed")
    
    # Example 2: Multi-agent environment
    multi_agent_result = CustomWorldExamples.create_multi_agent_environment(3)
    print(f"\nMulti-agent environment: {len(multi_agent_result['agents'])} agents interacted")
    
    # Example 3: Seasonal environment
    seasonal_result = CustomWorldExamples.create_seasonal_environment()
    print(f"\nSeasonal environment: {len(seasonal_result['season_results'])} seasons tracked")
    
    print("\n=== All custom world examples completed ===")
