#!/usr/bin/env python3
"""
example_world_interaction.py

Complex World Interaction Example
================================

This example demonstrates advanced world interaction using the NLM Python API,
showcasing how agents can manipulate their environment, adapt to changing conditions,
and learn from environmental interactions.

Key Concepts Covered:
- Environmental manipulation and resource management
- Hazard detection and avoidance strategies
- Spatial navigation and path planning
- Dynamic environment adaptation
- Interactive learning through environmental feedback

Learning Objectives:
1. Implement environmental manipulation behaviors
2. Develop hazard detection and avoidance strategies
3. Create spatial navigation capabilities
4. Design adaptive environmental interaction
5. Implement interactive learning from environmental feedback

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
import numpy as np
from typing import List, Dict, Tuple, Optional

def create_complex_environment():
    """
    Create a complex environment with various interactive elements.
    
    Returns:
        Complex interactive environment
    """
    print("   Creating complex interactive environment...")
    
    # Create arena environment with interactive elements
    world = nlm.createArenaWorld(width=200.0, height=200.0, withWalls=True)
    
    # Setup agent starting position
    nlm.setupAgentStartPosition(world, x=10.0, y=10.0, orientation=45.0)
    
    # Create diverse resources for learning
    nlm.spawnResource(world, x=30.0, y=30.0, value=10.0)  # Small resource
    nlm.spawnResource(world, x=80.0, y=30.0, value=25.0)  # Medium resource
    nlm.spawnResource(world, x=130.0, y=30.0, value=40.0)  # Large resource
    nlm.spawnResource(world, x=180.0, y=30.0, value=15.0)  # Small resource
    
    # Create scattered resources
    for i in range(5, 20, 5):
        for j in range(5, 20, 5):
            value = np.random.uniform(5.0, 20.0)
            nlm.spawnResource(world, x=i * 10.0, y=j * 10.0, value=value)
    
    # Create hazards at strategic locations
    nlm.createHazard(world, x=60.0, y=60.0, damage=8.0)   # Medium hazard
    nlm.createHazard(world, x=120.0, y=60.0, damage=12.0)  # High hazard
    nlm.createHazard(world, x=180.0, y=60.0, damage=5.0)   # Low hazard
    
    # Create dynamic obstacles (will change position over time)
    world.addDynamicObstacle(100.0, 100.0, 10.0, 10.0)  # Moving obstacle
    
    return world

def create_resource_collection_behavior():
    """
    Create behavior for resource collection and management.
    
    Returns:
        Resource collection strategy configuration
    """
    print("   Creating resource collection behavior...")
    
    # Resource collection strategy
    strategy = {
        'max_capacity': 100.0,  # Maximum resource carrying capacity
        'priority_threshold': 20.0,  # Minimum resource value to collect
        'search_radius': 50.0,  # Distance to search for resources
        'return_destination': (10.0, 10.0),  # Home/base location
        'optimal_pathfinding': True,
        'avoid_hazards': True,
        'resource_value_threshold': 15.0
    }
    
    return strategy

def create_hazard_avoidance_behavior():
    """
    Create behavior for hazard detection and avoidance.
    
    Returns:
        Hazard avoidance strategy configuration
    """
    print("   Creating hazard avoidance behavior...")
    
    # Hazard avoidance strategy
    strategy = {
        'danger_threshold': 0.7,  # Distance below which hazard is dangerous
        'safe_distance': 20.0,  # Minimum safe distance from hazard
        'avoidance_force': 0.5,  # Strength of avoidance behavior
        'risk_calculation': 'probabilistic',  # Risk assessment method
        'learning_rate': 0.3,  # How quickly to learn hazard locations
        'memory_retention': 0.8  # How long to remember hazard locations
    }
    
    return strategy

def create_navigation_strategy():
    """
    Create navigation and pathfinding strategy.
    
    Returns:
        Navigation strategy configuration
    """
    print("   Creating navigation strategy...")
    
    # Navigation strategy
    strategy = {
        'vision_range': 30.0,  # Sensory perception range
        'turning_speed': 45.0,  # Degrees per second
        'max_speed': 15.0,  # Maximum movement speed
        'path_smoothing': True,  # Smooth path interpolation
        'obstacle_avoidance': True,  # Automatic obstacle avoidance
        'goal_attraction': True,  # Attraction to goals
        'heuristic_weight': 0.7,  # Weight of heuristic vs. exact path
        'dynamic_replanning': True  # Real-time path replanning
    }
    
    return strategy

def resource_collection_simulation(world, brain, agent, strategy):
    """
    Simulate resource collection behavior.
    
    Args:
        world: The environment
        brain: The agent's brain
        agent: The agent
        strategy: Resource collection strategy
        
    Returns:
        Resource collection statistics
    """
    print("   Running resource collection simulation...")
    
    stats = {
        'resources_found': 0,
        'resources_collected': 0,
        'total_resource_value': 0.0,
        'collection_efficiency': 0.0,
        'path_length': 0.0,
        'time_spent_collecting': 0.0,
        'near_miss_count': 0
    }
    
    max_steps = 100
    collected_resources = []
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Process through brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for nearby resources
        agent_pos = world.getAgentPosition()
        nearby_resources = world.getNearbyResources(agent_pos[0], agent_pos[1], strategy['search_radius'])
        
        # Find best resource to collect
        best_resource = None
        best_value = float('-inf')
        
        for resource in nearby_resources:
            if resource.getValue() > best_value and resource.getValue() > strategy['priority_threshold']:
                # Check if path to resource is clear of hazards
                path_clear = True
                if strategy['avoid_hazards']:
                    hazard_positions = world.getHazardPositions()
                    for hazard in hazard_positions:
                        distance_to_resource = np.sqrt(
                            (hazard[0] - resource.getPosition()[0])**2 + 
                            (hazard[1] - resource.getPosition()[1])**2
                        )
                        if distance_to_resource < strategy['safe_distance']:
                            path_clear = False
                            break
                
                if path_clear:
                    best_resource = resource
                    best_value = resource.getValue()
        
        # Attempt to collect resource if found
        if best_resource and strategy['priority_threshold'] > 0:
            stats['resources_found'] += 1
            
            # Check if collection attempt successful
            collection_success = agent.attemptResourceCollection(
                best_resource.getPosition()[0], 
                best_resource.getPosition()[1]
            )
            
            if collection_success:
                stats['resources_collected'] += 1
                stats['total_resource_value'] += best_resource.getValue()
                collected_resources.append(best_resource)
                
                # Remove collected resource from world
                world.removeResource(best_resource)
        
        # Check for near misses (resources too close to hazards)
        for resource in nearby_resources:
            hazard_positions = world.getHazardPositions()
            for hazard in hazard_positions:
                distance = np.sqrt(
                    (hazard[0] - resource.getPosition()[0])**2 + 
                    (hazard[1] - resource.getPosition()[1])**2
                )
                if distance < strategy['safe_distance']:
                    stats['near_miss_count'] += 1
        
        # Check episode termination
        if step % 50 == 0 and step > 0:
            # Return to base periodically
            home_pos = strategy['return_destination']
            distance_to_home = np.sqrt(
                (agent_pos[0] - home_pos[0])**2 + 
                (agent_pos[1] - home_pos[1])**2
            )
            
            if distance_to_home < 5.0:
                # Store resources at base
                agent.storeResources()
                break
        
        if world.isEpisodeDone():
            break
    
    # Calculate efficiency metrics
    if stats['resources_found'] > 0:
        stats['collection_efficiency'] = stats['resources_collected'] / stats['resources_found']
    
    return stats, collected_resources

def hazard_avoidance_simulation(world, brain, agent, strategy):
    """
    Simulate hazard avoidance behavior.
    
    Args:
        world: The environment
        brain: The agent's brain
        agent: The agent
        strategy: Hazard avoidance strategy
        
    Returns:
        Hazard avoidance statistics
    """
    print("   Running hazard avoidance simulation...")
    
    stats = {
        'hazard_encounters': 0,
        'successful_avoidances': 0,
        'near_misses': 0,
        'hazard_distance_trend': [],
        'risk_assessment_accuracy': 0.0
    }
    
    max_steps = 150
    hazard_positions_history = []
    
    for step in range(max_steps):
        # Update world (dynamic hazards may move)
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Process through brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get current agent position
        agent_pos = world.getAgentPosition()
        
        # Get hazard positions
        hazard_positions = world.getHazardPositions()
        hazard_positions_history.append(hazard_positions)
        
        # Calculate distances to hazards
        distances = []
        for hazard in hazard_positions:
            distance = np.sqrt(
                (hazard[0] - agent_pos[0])**2 + 
                (hazard[1] - agent_pos[1])**2
            )
            distances.append(distance)
            
            # Count hazard encounters
            if distance < strategy['danger_threshold']:
                stats['hazard_encounters'] += 1
                
                # Check if avoidance was successful
                if distance >= strategy['safe_distance']:
                    stats['successful_avoidances'] += 1
                else:
                    stats['near_misses'] += 1
        
        # Record distance trend
        if distances:
            avg_distance = np.mean(distances)
            stats['hazard_distance_trend'].append(avg_distance)
        
        # Update hazard memory if learning is enabled
        if agent.isHazardLearningEnabled():
            for hazard in hazard_positions:
                agent.learnHazardLocation(hazard[0], hazard[1], strategy['learning_rate'])
        
        # Check episode termination
        if world.isEpisodeDone():
            break
    
    # Calculate risk assessment accuracy
    if len(hazard_positions_history) > 1:
        # Simple heuristic: check if agent consistently maintains safe distances
        recent_distances = []
        for history_step in hazard_positions_history[-10:]:
            for hazard in history_step:
                distance = np.sqrt(
                    (hazard[0] - agent_pos[0])**2 + 
                    (hazard[1] - agent_pos[1])**2
                )
                recent_distances.append(distance)
        
        if recent_distances:
            safe_ratio = sum(1 for d in recent_distances if d >= strategy['safe_distance']) / len(recent_distances)
            stats['risk_assessment_accuracy'] = safe_ratio
    
    return stats

def pathfinding_simulation(world, brain, agent, strategy):
    """
    Simulate advanced pathfinding and navigation.
    
    Args:
        world: The environment
        brain: The agent's brain
        agent: The agent
        strategy: Navigation strategy
        
    Returns:
        Pathfinding statistics
    """
    print("   Running pathfinding simulation...")
    
    stats = {
        'navigation_attempts': 0,
        'successful_paths': 0,
        'path_efficiency': 0.0,
        'replanning_count': 0,
        'obstacle_encounters': 0,
        'total_distance_traveled': 0.0
    }
    
    max_steps = 200
    target_positions = [(50.0, 50.0), (150.0, 150.0), (180.0, 80.0), (80.0, 180.0)]
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Process through brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get current position
        agent_pos = world.getAgentPosition()
        stats['total_distance_traveled'] += np.linalg.norm(action.getParameters())
        
        # Check if reached target
        if step % 40 == 0 and step > 0:
            stats['navigation_attempts'] += 1
            
            # Check if any target is reached
            target_reached = False
            for target in target_positions:
                distance = np.sqrt((agent_pos[0] - target[0])**2 + (agent_pos[1] - target[1])**2)
                if distance < 5.0:  # Within target radius
                    target_reached = True
                    break
            
            if target_reached:
                stats['successful_paths'] += 1
        
        # Check for obstacles
        obstacles = world.getDynamicObstacles()
        for obstacle in obstacles:
            if obstacle.isActive():
                obstacle_pos = obstacle.getPosition()
                distance = np.sqrt(
                    (obstacle_pos[0] - agent_pos[0])**2 + 
                    (obstacle_pos[1] - agent_pos[1])**2
                )
                if distance < 15.0:  # Obstacle detection range
                    stats['obstacle_encounters'] += 1
                    
                    # Trigger replanning if strategy enabled
                    if strategy['dynamic_replanning'] and np.random.random() < 0.3:
                        stats['replanning_count'] += 1
        
        # Check episode termination
        if world.isEpisodeDone():
            break
    
    # Calculate path efficiency
    if stats['navigation_attempts'] > 0:
        stats['path_efficiency'] = stats['successful_paths'] / stats['navigation_attempts']
    
    return stats

def main():
    """Main function demonstrating complex world interaction."""
    print("=" * 70)
    print("NLM Complex World Interaction Example")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: ENVIRONMENT AND BEHAVIOR SETUP
    # ---------------------------------------------------------------------
    print("\n1. Setting up complex interaction environment...")
    
    # Create complex environment
    world = create_complex_environment()
    print(f"   ✓ Complex environment created ({world.getWidth()}x{world.getHeight()} units)")
    
    # Create behavior strategies
    resource_strategy = create_resource_collection_behavior()
    hazard_strategy = create_hazard_avoidance_behavior()
    navigation_strategy = create_navigation_strategy()
    
    print(f"   ✓ Resource collection strategy configured")
    print(f"   ✓ Hazard avoidance strategy configured")
    print(f"   ✓ Navigation strategy configured")
    
    # Create brain and agent
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createStandardAgent(brain)
    agent.initialize(world)
    
    # Enable relevant capabilities
    agent.enableResourceCollection(True)
    agent.enableHazardAvoidance(True)
    agent.enablePathfinding(True)
    
    print(f"   ✓ Brain initialized ({brain.getTotalNeuronCount()} neurons)")
    print(f"   ✓ Agent configured with interactive capabilities")
    
    # ---------------------------------------------------------------------
    # SECTION 2: BEHAVIORAL SIMULATIONS
    # ---------------------------------------------------------------------
    print("\n2. Running behavioral simulations...")
    
    # Run resource collection simulation
    print("\n  Resource Collection Simulation:")
    resource_stats, collected_resources = resource_collection_simulation(
        world, brain, agent, resource_strategy
    )
    
    print(f"    Resources found: {resource_stats['resources_found']}")
    print(f"    Resources collected: {resource_stats['resources_collected']}")
    print(f"    Collection efficiency: {resource_stats['collection_efficiency']:.1%}")
    print(f"    Total resource value: {resource_stats['total_resource_value']:.1f}")
    
    # Reset world for hazard simulation
    world = create_complex_environment()
    agent.initialize(world)
    
    # Run hazard avoidance simulation
    print("\n  Hazard Avoidance Simulation:")
    hazard_stats = hazard_avoidance_simulation(world, brain, agent, hazard_strategy)
    
    print(f"    Hazard encounters: {hazard_stats['hazard_encounters']}")
    print(f"    Successful avoidances: {hazard_stats['successful_avoidances']}")
    print(f"    Near misses: {hazard_stats['near_misses']}")
    print(f"    Risk assessment accuracy: {hazard_stats['risk_assessment_accuracy']:.1%}")
    
    # Reset world for pathfinding simulation
    world = create_complex_environment()
    agent.initialize(world)
    
    # Run pathfinding simulation
    print("\n  Pathfinding Simulation:")
    pathfinding_stats = pathfinding_simulation(world, brain, agent, navigation_strategy)
    
    print(f"    Navigation attempts: {pathfinding_stats['navigation_attempts']}")
    print(f"    Successful paths: {pathfinding_stats['successful_paths']}")
    print(f"    Path efficiency: {pathfinding_stats['path_efficiency']:.1%}")
    print(f"    Obstacle encounters: {pathfinding_stats['obstacle_encounters']}")
    print(f"    Total distance traveled: {pathfinding_stats['total_distance_traveled']:.1f}")
    
    # ---------------------------------------------------------------------
    # SECTION 3: ENVIRONMENTAL ADAPTATION ANALYSIS
    # ---------------------------------------------------------------------
    print("\n3. Analyzing environmental adaptation...")
    
    # Analyze resource collection adaptation
    resource_efficiency = resource_stats['collection_efficiency']
    resources_per_step = resource_stats['resources_collected'] / resource_stats.get('steps', 1)
    
    # Analyze hazard avoidance adaptation
    hazard_success_rate = (hazard_stats['successful_avoidances'] / 
                          max(1, hazard_stats['hazard_encounters']))
    hazard_avoidance_learned = hazard_stats['risk_assessment_accuracy'] > 0.7
    
    # Analyze pathfinding adaptation
    pathfinding_success = pathfinding_stats['successful_paths'] > 0
    obstacle_adaptation = pathfinding_stats['obstacle_encounters'] > 5
    
    print(f"   ✓ Resource collection adaptation: {'✓' if resource_efficiency > 0.5 else '⚠'} "
          f"(efficiency: {resource_efficiency:.1%})")
    print(f"   ✓ Hazard avoidance adaptation: {'✓' if hazard_avoidance_learned else '⚠'} "
          f"(success rate: {hazard_success_rate:.1%})")
    print(f"   ✓ Pathfinding adaptation: {'✓' if pathfinding_success else '⚠'} "
          f"(obstacle handling: {'✓' if obstacle_adaptation else '⚠'})")
    
    # ---------------------------------------------------------------------
    # SECTION 4: ENVIRONMENTAL FEEDBACK LOOP
    # ---------------------------------------------------------------------
    print("\n4. Environmental feedback analysis...")
    
    # Analyze environmental interaction patterns
    environmental_interaction_score = (
        resource_efficiency * 0.4 + 
        hazard_success_rate * 0.3 + 
        (pathfinding_stats['path_efficiency'] if pathfinding_stats['navigation_attempts'] > 0 else 0) * 0.3
    )
    
    environmental_adaptation_level = "Basic"
    if environmental_interaction_score > 0.8:
        environmental_adaptation_level = "Advanced"
    elif environmental_interaction_score > 0.6:
        environmental_adaptation_level = "Competent"
    elif environmental_interaction_score > 0.4:
        environmental_adaptation_level = "Developing"
    
    print(f"   ✓ Overall environmental interaction score: {environmental_interaction_score:.3f}")
    print(f"   ✓ Environmental adaptation level: {environmental_adaptation_level}")
    print(f"   ✓ Resources collected: {len(collected_resources)}")
    
    # ---------------------------------------------------------------------
    # SECTION 5: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n5. Final validation...")
    
    # Validate brain state
    brain_issues = nlm.validateBrainDynamics(brain, duration=100)
    if brain_issues:
        print(f"   ⚠ Brain dynamics issues ({len(brain_issues)}):")
        for issue in brain_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ Brain dynamics are stable")
    
    # Validate world state
    world_issues = nlm.validateWorldState(world)
    if world_issues:
        print(f"   ⚠ World state issues ({len(world_issues)}):")
        for issue in world_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ World state is valid")
    
    # ---------------------------------------------------------------------
    # SECTION 6: PERFORMANCE BENCHMARKING
    # ---------------------------------------------------------------------
    print("\n6. Performance benchmarking...")
    
    # Benchmark brain performance
    brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=50)
    print(f"   ✓ Brain stepping: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    # Benchmark simulation performance
    simulation_benchmark = nlm.benchmarkSimulation(world, brain, numSteps=30)
    print(f"   ✓ Simulation performance: {simulation_benchmark.getSimulationStepsPerSecond():.1f} steps/sec")
    
    # ---------------------------------------------------------------------
    # SECTION 7: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("EXAMPLE COMPLETE - Complex World Interaction Summary")
    print("=" * 70)
    
    print(f"\nEnvironmental Interaction Performance:")
    print(f"  - Resource collection: {resource_stats['resources_collected']}/{resource_stats['resources_found']} "
          f"({resource_efficiency:.1%} efficiency)")
    print(f"  - Hazard avoidance: {hazard_stats['successful_avoidances']}/{hazard_stats['hazard_encounters']} "
          f"({hazard_success_rate:.1%} success rate)")
    print(f"  - Pathfinding: {pathfinding_stats['successful_paths']}/{pathfinding_stats['navigation_attempts']} "
          f"({pathfinding_stats.get('path_efficiency', 0):.1%} efficiency)")
    
    print(f"\nEnvironmental Adaptation:")
    print(f"  - Adaptation level: {environmental_adaptation_level}")
    print(f"  - Interaction score: {environmental_interaction_score:.3f}")
    print(f"  - Resources gathered: {len(collected_resources)}")
    print(f"  - Hazards avoided: {hazard_stats['successful_avoidances']}")
    print(f"  - Paths completed: {pathfinding_stats['successful_paths']}")
    
    print(f"\nHealth Status:")
    print(f"  - Brain dynamics: {'✓ Stable' if not brain_issues else '⚠ Issues'}")
    print(f"  - World state: {'✓ Valid' if not world_issues else '⚠ Issues'}")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_neural_analysis.py for advanced analysis")
    print(f"  2. Try example_performance_benchmarking.py for performance optimization")
    print(f"  3. Try tutorial_step_by_step.py for comprehensive tutorial")
    print(f"  4. Try recipe_agent_training.py for training recipes")

if __name__ == "__main__":
    main()