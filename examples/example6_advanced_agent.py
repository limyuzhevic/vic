#!/usr/bin/env python3
"""
Example 6: Advanced Agent Behavior

This example demonstrates advanced agent capabilities including hierarchical
planning, memory-guided action selection, and meta-cognitive behaviors.
"""

import pynlm
import numpy as np

def advanced_agent_behavior():
    """Demonstrate advanced agent behaviors."""
    
    print("=== Example 6: Advanced Agent Behavior ===")
    
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Create goal-based planning agent
    class GoalBasedAgent:
        def __init__(self, brain, world, agent):
            self.brain = brain
            self.world = world
            self.agent = agent
            self.goals = []
            self.current_goal = None
            self.planning_history = []
        
        def add_goal(self, goal, priority=1.0):
            """Add a goal to the agent."""
            self.goals.append({'id': len(self.goals), 'goal': goal, 
                             'priority': priority, 'achieved': False})
            print(f"Added goal: {goal} (priority: {priority})")
        
        def set_current_goal(self, goal_id):
            """Set current goal based on priority."""
            if 0 <= goal_id < len(self.goals):
                self.current_goal = self.goals[goal_id]
                print(f"Set current goal: {self.current_goal['goal']}")
            else:
                print(f"Goal ID {goal_id} not found")
        
        def plan_action_sequence(self, goal):
            """Plan action sequence for given goal."""
            # Simplified planning: generate sequence based on goal type
            if 'explore' in goal.lower():
                actions = ['move_forward', 'turn_left', 'turn_right', 'move_forward']
            elif 'find' in goal.lower():
                actions = ['move_forward', 'turn_left', 'move_forward', 
                          'turn_right', 'move_forward', 'interact']
            elif 'collect' in goal.lower():
                actions = ['move_forward', 'move_forward', 'interact',
                          'move_backward', 'interact']
            else:
                actions = ['move_forward', 'move_forward', 'move_forward']
            
            print(f"Planned action sequence for '{goal}': {actions}")
            return actions
        
        def execute_action_sequence(self, action_sequence):
            """Execute planned action sequence."""
            print(f"Executing action sequence: {action_sequence}")
            for i, action in enumerate(action_sequence):
                # In a real implementation, this would apply the action
                print(f"  Step {i+1}: {action}")
    
    # Create goal-based agent
    goal_agent = GoalBasedAgent(brain, world, agent)
    
    # Add goals
    goal_agent.add_goal("explore new areas", priority=0.8)
    goal_agent.add_goal("find resources", priority=0.9)
    goal_agent.add_goal("collect items", priority=0.7)
    
    # Set current goal
    goal_agent.set_current_goal(1)  # Find resources
    
    # Run simulation with goal-based planning
    print("\nRunning goal-based simulation...")
    
    for step in range(150):
        # Update world
        world.update(0.1)
        
        # Get current state for planning
        agent_pos = world.getAgentPosition()
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain (includes development updates)
        brain.step(step)
        
        # Get planned action based on current goal
        if goal_agent.current_goal:
            planned_actions = goal_agent.plan_action_sequence(
                goal_agent.current_goal['goal']
            )
            current_action = planned_actions[0] if planned_actions else 'move_forward'
            
            # Map action names to actual actions
            action_map = {
                'move_forward': 0,
                'turn_left': 1,
                'turn_right': 2,
                'interact': 3,
                'move_backward': 4
            }
            action_code = action_map.get(current_action, 0)
            
            # Execute action
            result = world.applyMotorCommand(action_code, world.getSimulationTime())
            
            # Get reward
            reward = result.reward if hasattr(result, 'reward') else 0.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
        else:
            # Fallback action selection
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            reward = result.reward if hasattr(result, 'reward') else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update goal achievement (simplified)
        if step % 50 == 0:
            progress = (step / 150) * goal_agent.current_goal['priority'] if goal_agent.current_goal else 0
            print(f"  Step {step}: Position ({agent_pos.x:.1f}, {agent_pos.y:.1f}), "
                  f"Goal progress: {progress:.2f}")
    
    # Test memory-guided behavior
    print("\n--- Memory-Guided Behavior ---")
    
    # Store experiences in memory systems
    working_memory = brain.getWorkingMemory()
    if working_memory:
        # Store agent positions and actions as memory patterns
        positions = [world.getAgentPosition() for _ in range(10)]
        for i, pos in enumerate(positions):
            memory_pattern = np.array([pos.x, pos.y, i])
            working_memory.store(memory_pattern, strength=1.0)
        
        # Retrieve similar patterns
        current_pos = world.getAgentPosition()
        memory_pattern = np.array([current_pos.x, current_pos.y, 0])
        
        retrieved = working_memory.retrieve()
        print(f"Retrieved {len(retrieved) if retrieved else 0} memory patterns")
        
        # Use memory to guide action
        if retrieved:
            print("Memory-guided action selection: Use learned patterns")
        
        # Get memory activity statistics
        active_traces = working_memory.getActiveTraces()
        print(f"Active memory traces: {active_traces}")
    
    # Meta-cognitive monitoring
    print("\n--- Meta-Cognitive Monitoring ---")
    
    # Monitor agent performance
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    development_stage = brain.getDevelopmentalStage()
    
    print(f"Agent meta-cognition metrics:")
    print(f"  Curiosity level: {curiosity:.3f}")
    print(f"  Novelty level: {novelty:.3f}")
    print(f"  Developmental stage: {development_stage}")
    print(f"  Stage progress: {brain.getStageProgress():.3f}")
    
    # Assess confidence in planning
    planning_confidence = curiosity * 0.5 + (1.0 - brain.getStageProgress()) * 0.5
    print(f"Planning confidence: {planning_confidence:.3f}")
    
    print("\nAdvanced agent behavior demonstration complete!")
    return brain, world, agent, goal_agent

if __name__ == "__main__":
    advanced_agent_behavior()