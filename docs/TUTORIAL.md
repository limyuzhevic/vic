# NLM Python API Tutorials

## Overview

This document provides step-by-step tutorials for using the NLM Python API, designed for different skill levels from beginner to advanced.

## Table of Contents

- [Beginner Tutorial: Creating Your First Brain Simulation](#beginner-tutorial-creating-your-first-brain-simulation)
  - [Prerequisites](#prerequisites)
  - [Setup and Configuration](#setup-and-configuration)
  - [Running the Simulation](#running-the-simulation)
  - [Understanding Results](#understanding-results)
  - [Next Steps](#next-steps)
- [Intermediate Tutorial: Agent-Based Simulation with World Interaction](#intermediate-tutorial-agent-based-simulation-with-world-interaction)
  - [Building the Agent Class](#building-the-agent-class)
  - [Implementing Decision Making](#implementing-decision-making)
  - [Agent Learning and Memory](#agent-learning-and-memory)
  - [Running Multiple Agents](#running-multiple-agents)
  - [Agent Competition and Cooperation](#agent-competition-and-cooperation)
- [Advanced Tutorial: Performance Optimization and Expert Features](#advanced-tutorial-performance-optimization-and-expert-features)
  - [Performance Profiling and Optimization](#performance-profiling-and-optimization)
  - [Expert APIs Integration](#expert-apis-integration)
  - [Large-Scale Simulation](#large-scale-simulation)
  - [Real-Time Visualization](#real-time-visualization)
  - [Research-Grade Experiments](#research-grade-experiments)

## Beginner Tutorial: Creating Your First Brain Simulation

This tutorial will guide you through creating your first NLM brain simulation. No prior experience with NLM is required.

### Prerequisites

#### System Requirements

- Python 3.8 or higher
- NLM Python bindings installed (`pip install pynlm`)
- Basic understanding of Python programming

#### Installation

```bash
# Install NLM Python bindings
pip install pynlm

# Verify installation
python -c "import nlm; print('NLM version:', nlm.__version__)"
```

### Setup and Configuration

#### Step 1: Import NLM and Create Basic Configuration

```python
"""1. Import NLM and setup basic configuration"""
import nlm

# Create default configuration
config = nlm.create_default_config()

# Create brain with configuration
brain = nlm.create_brain(config)

# Create simple world
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)  # width, height, view_distance, cell_size
world.setAgentStart(10, 10)  # Agent starting position

# Initialize systems
brain.initialize(world)
```

#### Step 2: Create Agent Brain

```python
"""2. Create and configure agent brain"""
# Create agent brain
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(brain, world)

# Create monitoring and control systems
controller = nlm.create_simulation_controller()
monitor = nlm.create_performance_monitor()

# Verify setup
print("Setup complete!")
print(f"World size: {world.width}x{world.height}")
print(f"Agent starting position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
print(f"Initial neurons: {brain.getStatistics()['total_neurons']}")
```

### Running the Simulation

#### Basic Simulation Loop

```python
"""3. Run the basic simulation loop"""
def run_basic_simulation():
    print("Starting basic simulation...")
    
    # Run simulation for 100 steps
    for step in range(100):
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent brain
        agent_brain.processSensoryInput(percept)
        
        # Decode motor command from brain
        action = agent_brain.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, step)
        
        # Update world state
        world.update(0.001)
        
        # Record performance metrics
        neurons_active = brain.getActiveNeuronCount()
        monitor.recordStep(neurons_active, 0, 0.001)
        
        # Log progress every 10 steps
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Position ({body.x}, {body.y}), Energy: {body.energy:.2f}")
    
    # Get performance report
    performance_report = monitor.getPerformanceReport()
    return performance_report

# Run the simulation
results = run_basic_simulation()
```

#### Enhanced Simulation with Error Handling

```python
"""4. Run enhanced simulation with error handling"""
def run_enhanced_simulation():
    print("Starting enhanced simulation with error handling...")
    
    try:
        for step in range(200):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process and decode
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            
            # Apply action with error checking
            result = world.applyMotorCommand(action, step)
            if not result.wasSuccessful():
                print(f"Action failed at step {step}: {result.getError()}")
            
            # Update world
            world.update(0.001)
            
            # Record metrics
            neurons_active = brain.getActiveNeuronCount()
            monitor.recordStep(neurons_active, 0, 0.001)
            
            # Pause at specific steps for analysis
            if step % 50 == 0:
                controller.pause()
                print(f"Paused at step {step} for analysis")
                # ... analysis code here ...
                controller.resume()
        
        # Generate performance report
        report = monitor.getPerformanceReport()
        print(f"\nSimulation completed successfully!")
        print(f"Average steps per second: {report['simulation_speed']:.2f}")
        print(f"Average active neurons: {report['avg_active_neurons']:.1f}")
        print(f"Total simulation time: {report['total_time']:.2f} seconds")
        
        return report
        
    except Exception as e:
        print(f"Simulation error: {e}")
        # Save partial results
        brain.save(f"failed_brain_{step}.json")
        raise

# Run enhanced simulation
enhanced_results = run_enhanced_simulation()
```

### Understanding Results

#### Analyzing Performance Metrics

```python
"""5. Analyze and understand simulation results"""
def analyze_simulation_results(performance_report):
    print("\n=== Simulation Results Analysis ===")
    
    # Performance metrics
    print(f"\nPerformance Metrics:")
    print(f"  - Steps per second: {performance_report['simulation_speed']:.2f}")
    print(f"  - Average iteration time: {performance_report['avg_iteration_time']:.6f} seconds")
    print(f"  - Average active neurons: {performance_report['avg_active_neurons']:.1f}")
    print(f"  - Average spikes per second: {performance_report['avg_spikes_per_second']:.1f}")
    
    # Brain statistics
    stats = brain.getStatistics()
    print(f"\nBrain Statistics:")
    print(f"  - Total neurons: {stats['total_neurons']}")
    print(f"  - Active neurons: {stats['active_neurons']}")
    print(f"  - Synapses formed: {stats['synapses_formed']}")
    print(f"  - Plasticity events: {stats['plasticity_events']}")
    
    # Agent state
    body = world.getAgentBody()
    print(f"\nAgent State:")
    print(f"  - Final position: ({body.x}, {body.y})")
    print(f"  - Final energy: {body.energy:.2f}")
    print(f"  - Current velocity: {body.velocity}")
    
    # Visualization suggestions
    print(f"\nVisualization Suggestions:")
    print(f"  - Neuron activity: Use generate_neuron_activity_plot(brain)")
    print(f"  - Connection density: Use generate_connection_density_plot(brain)")
    
    return {
        'performance': performance_report,
        'brain_stats': stats,
        'agent_state': {
            'position': (body.x, body.y),
            'energy': body.energy,
            'velocity': body.velocity
        }
    }

# Analyze results
analysis = analyze_simulation_results(results)
```

#### Saving and Loading Results

```python
"""6. Save and load simulation results"""
import json
import time
from datetime import datetime

def save_simulation_results(results, analysis):
    """Save simulation results to file"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    # Save performance report
    with open(f"performance_report_{timestamp}.json", 'w') as f:
        json.dump(results, f, indent=2)
    
    # Save analysis
    with open(f"analysis_{timestamp}.json", 'w') as f:
        # Convert non-serializable objects
        analysis_serializable = {
            'performance': results,
            'brain_stats': analysis['brain_stats'],
            'agent_state': analysis['agent_state'],
            'timestamp': timestamp
        }
        json.dump(analysis_serializable, f, indent=2)
    
    # Save brain state
    brain.save(f"brain_state_{timestamp}.json")
    
    print(f"Results saved with timestamp: {timestamp}")

def load_simulation_results(timestamp):
    """Load simulation results from file"""
    # Load performance report
    with open(f"performance_report_{timestamp}.json", 'r') as f:
        results = json.load(f)
    
    # Load analysis
    with open(f"analysis_{timestamp}.json", 'r') as f:
        analysis = json.load(f)
    
    # Load brain state
    brain.load(f"brain_state_{timestamp}.json")
    
    print(f"Results loaded from timestamp: {timestamp}")
    return results, analysis

# Save results
save_simulation_results(results, analysis)

# Example of loading results
# loaded_results, loaded_analysis = load_simulation_results("20231015_143022")
```

### Next Steps

#### Building on the Basics

```python
"""7. Continue learning with more advanced features"""

# Option 1: Learn about expert APIs
print("\n=== Next Learning Steps ===")
print("1. Explore expert APIs:")
print("   - Performance optimization with MemoryPool")
print("   - Advanced simulation with custom plasticity")
print("   - Real-time visualization")
print("   - Large-scale simulation capabilities")

# Option 2: Create custom examples
print("\n2. Create custom examples:")
print("   - Research simulations")
print("   - Production deployments")
print("   - Visualization demos")
print("   - Performance benchmarks")

# Option 3: Study configuration
print("\n3. Study configuration:")
print("   - Configuration presets (default, minimal, high-performance)")
print("   - Dynamic configuration updates")
print("   - Configuration validation")
print("   - Configuration optimization")

# Option 4: Join research
print("\n4. Join research community:")
print("   - Read research papers")
print("   - Contribute to NLM project")
print("   - Share your simulations")
```

### Example: Complete Working Simulation

```python
"""Complete example: Simple neural network simulation with analysis"""
import nlm
import time
import matplotlib.pyplot as plt

def complete_example_simulation():
    """Complete working example simulation"""
    print("=== Complete NLM Example Simulation ===")
    
    # Setup
    config = nlm.create_default_config()
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    monitor = nlm.create_performance_monitor()
    
    # Track metrics for analysis
    neuron_activities = []
    agent_energies = []
    simulation_times = []
    
    start_time = time.time()
    
    # Run simulation
    for step in range(100):
        step_start = time.time()
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Generate and apply action
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Record metrics
        neurons_active = brain.getActiveNeuronCount()
        body = world.getAgentBody()
        
        neuron_activities.append(neurons_active)
        agent_energies.append(body.energy)
        
        monitor.recordStep(neurons_active, 0, 0.001)
        
        step_time = time.time() - step_start
        simulation_times.append(step_time)
        
        if step % 20 == 0:
            print(f"Step {step}: Neurons active: {neurons_active}, Energy: {body.energy:.2f}")
    
    end_time = time.time()
    
    # Generate visualizations
    print("\nGenerating visualizations...")
    
    # Create neuron activity plot
    activity_plot = nlm.generate_neuron_activity_plot(brain)
    print("Neuron activity plot generated")
    
    # Create connection density plot
    density_plot = nlm.generate_connection_density_plot(brain)
    print("Connection density plot generated")
    
    # Create performance report
    performance_report = monitor.getPerformanceReport()
    
    # Analyze results
    print(f"\n=== Simulation Summary ===")
    print(f"Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"Average step time: {performance_report['avg_iteration_time']:.6f} seconds")
    print(f"Steps per second: {performance_report['simulation_speed']:.2f}")
    print(f"Average active neurons: {performance_report['avg_active_neurons']:.1f}")
    print(f"Final brain statistics: {brain.getStatistics()}")
    
    return {
        'neuron_activities': neuron_activities,
        'agent_energies': agent_energies,
        'simulation_times': simulation_times,
        'performance_report': performance_report
    }

# Run complete example
complete_results = complete_example_simulation()
```

## Intermediate Tutorial: Agent-Based Simulation with World Interaction

This tutorial focuses on creating sophisticated agents that can interact with their environment, make decisions, learn from experience, and work with multiple agents.

### Building the Agent Class

#### Core Agent Architecture

```python
"""Intermediate Tutorial: Agent Class Implementation"""
import nlm
from typing import List, Dict, Any, Optional
import math

class AdvancedAgent:
    """Advanced agent with cognitive capabilities and learning"""
    
    def __init__(self, brain: nlm.Brain, world: nlm.SimpleWorld, agent_id: str = "agent_0"):
        """Initialize advanced agent.
        
        Args:
            brain: Neural network for agent
            world: Simulation environment
            agent_id: Unique agent identifier
        """
        self.brain = brain
        self.world = world
        self.agent_id = agent_id
        
        # Agent state
        self.position = world.getAgentBody().x, world.getAgentBody().y
        self.energy = world.getAgentBody().energy
        self.velocity = [0.0, 0.0]
        self.health = 100.0
        self.age = 0.0
        
        # Learning and memory
        self.sensory_history = []
        self.action_history = []
        self.reward_history = []
        self.memory = {}
        
        # Cognitive state
        self.goals = []
        self.current_goal = None
        self.emotions = {'hunger': 0.0, 'curiosity': 0.0, 'fear': 0.0}
        
        # Perception processing
        self.perception_filter = PerceptionFilter()
        self.action_selector = ActionSelector()
        
    def perceive(self) -> nlm.SensoryInput:
        """Get and process sensory input from environment.
        
        Returns:
            Processed sensory input
        """
        # Get raw sensory input
        raw_percept = self.world.getSensoryPercept()
        
        # Apply perception filters
        filtered_percept = self.perception_filter.process(raw_percept, self)
        
        # Store in history
        self.sensory_history.append({
            'step': self.age,
            'raw': raw_percept,
            'filtered': filtered_percept,
            'position': self.position
        })
        
        return filtered_percept
    
    def decide(self, percept: nlm.SensoryInput) -> nlm.Action:
        """Make decision based on sensory input and internal state.
        
        Args:
            percept: Processed sensory input
            
        Returns:
            Selected action
        """
        # Update internal state
        self.updateInternalState()
        
        # Select goal based on priorities
        self.selectGoal()
        
        # Get action preferences from neural network
        self.brain.processSensoryInput(percept)
        action_preferences = self.brain.decodeMotorCommand()
        
        # Apply action selection with consideration of goals and emotions
        action = self.action_selector.select(
            action_preferences,
            self.goals,
            self.emotions,
            self.getAvailableActions()
        )
        
        # Store decision
        self.action_history.append({
            'step': self.age,
            'action': action,
            'preferences': action_preferences,
            'goals': self.goals,
            'emotions': self.emotions
        })
        
        return action
    
    def act(self, action: nlm.Action) -> Dict[str, Any]:
        """Execute action in environment.
        
        Args:
            action: Action to execute
            
        Returns:
            Action result
        """
        # Execute action
        result = self.world.applyMotorCommand(action, int(self.age))
        
        # Update agent state based on action
        self.updateStateFromAction(action, result)
        
        # Apply physiological effects
        self.applyPhysiologicalEffects(action, result)
        
        # Store experience
        self.storeExperience(action, result)
        
        return result
    
    def learn(self, percept: nlm.SensoryInput, action: nlm.Action, result: Dict[str, Any]):
        """Learn from experience.
        
        Args:
            percept: Sensory input
            action: Action taken
            result: Action result
        """
        # Calculate reward
        reward = self.calculateReward(percept, action, result)
        
        # Store reward
        self.reward_history.append({
            'step': self.age,
            'reward': reward,
            'action': action,
            'percept': percept
        })
        
        # Update neural network
        self.updateNeuralNetwork(percept, action, reward)
        
        # Update internal memory
        self.updateMemory(percept, action, reward, result)
        
        # Update goals based on learning
        self.updateGoalsFromExperience()
    
    def updateInternalState(self):
        """Update internal cognitive state."""
        # Update emotions based on physiological state
        self.emotions['hunger'] = max(0.0, self.emotions['hunger'] - 0.001)
        self.emotions['curiosity'] = min(1.0, self.emotions['curiosity'] + 0.0001)
        self.emotions['fear'] = min(1.0, self.emotions['fear'] + 0.0005)
        
        # Age and energy decay
        self.age += 0.001
        self.energy *= 0.999
        self.health *= 0.9995
    
    def selectGoal(self):
        """Select current goal based on priorities and environment."""
        # Find available goals
        available_goals = self.getAvailableGoals()
        
        if not available_goals:
            self.current_goal = None
            return
        
        # Prioritize goals based on urgency and importance
        goal_scores = {}
        for goal in available_goals:
            urgency = self.calculateGoalUrgency(goal)
            importance = self.getGoalImportance(goal)
            goal_scores[goal] = urgency * importance
        
        # Select highest scoring goal
        self.current_goal = max(goal_scores, key=goal_scores.get) if goal_scores else None
    
    def getAvailableGoals(self) -> List[str]:
        """Get available goals based on current state.
        
        Returns:
            List of available goals
        """
        goals = []
        
        # Basic survival goals
        if self.energy < 20:
            goals.append('find_food')
        
        if self.health < 30:
            goals.append('find_shelter')
        
        # Exploration goals
        if self.energy > 50 and self.emotions['curiosity'] > 0.7:
            goals.append('explore')
        
        # Social goals
        nearby_agents = self.getNearbyAgents(10)
        if nearby_agents and self.energy > 60:
            goals.append('interact_socially')
        
        return goals
    
    def getGoalImportance(self, goal: str) -> float:
        """Calculate importance of a goal.
        
        Args:
            goal: Goal name
            
        Returns:
            Goal importance (0.0 to 1.0)
        """
        importance_map = {
            'find_food': 0.9,
            'find_shelter': 0.8,
            'explore': 0.5,
            'interact_socially': 0.7
        }
        return importance_map.get(goal, 0.5)
    
    def calculateGoalUrgency(self, goal: str) -> float:
        """Calculate urgency of a goal.
        
        Args:
            goal: Goal name
            
        Returns:
            Goal urgency (0.0 to 1.0)
        """
        if goal == 'find_food':
            return min(1.0, (30 - self.energy) / 30)
        elif goal == 'find_shelter':
            return min(1.0, (50 - self.health) / 50)
        elif goal == 'interact_socially':
            nearby_agents = len(self.getNearbyAgents(10))
            return min(1.0, nearby_agents / 5)
        
        return 0.0
    
    def getAvailableActions(self) -> List[nlm.Action]:
        """Get list of available actions based on current state.
        
        Returns:
            List of available actions
        """
        available_actions = []
        
        # Basic movement actions
        if self.energy > 10:
            move_actions = [
                nlm.Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0]),
                nlm.Action.createMoveAction('MoveBackward', [-1.0, 0.0, 0.0]),
                nlm.Action.createMoveAction('MoveLeft', [0.0, -1.0, 0.0]),
                nlm.Action.createMoveAction('MoveRight', [0.0, 1.0, 0.0])
            ]
            available_actions.extend(move_actions)
        
        # Look actions
        look_actions = [
            nlm.Action.createLookAction('LookUp'),
            nlm.Action.createLookAction('LookDown'),
            nlm.Action.createLookAction('LookLeft'),
            nlm.Action.createLookAction('LookRight')
        ]
        available_actions.extend(look_actions)
        
        # Rest action
        if self.energy > 30:
            available_actions.append(nlm.Action.createRestAction())
        
        return available_actions
    
    def getNearbyAgents(self, radius: int) -> List[nlm.WorldObject]:
        """Get nearby agents.
        
        Args:
            radius: Search radius
            
        Returns:
            List of nearby agents
        """
        x, y = self.position
        nearby = []
        
        for dx in range(-radius, radius + 1):
            for dy in range(-radius, radius + 1):
                if dx == 0 and dy == 0:
                    continue
                
                nx, ny = x + dx, y + dy
                if 0 <= nx < self.world.width and 0 <= ny < self.world.height:
                    obj = self.world.getObjectAt(nx, ny)
                    if obj and obj.type == 'agent' and obj.agent_id != self.agent_id:
                        nearby.append(obj)
        
        return nearby
    
    def calculateReward(self, percept: nlm.SensoryInput, action: nlm.Action, result: Dict[str, Any]) -> float:
        """Calculate reward for action.
        
        Args:
            percept: Sensory input
            action: Action taken
            result: Action result
            
        Returns:
            Reward value
        """
        reward = 0.0
        
        # Energy reward
        if result.get('energy_change', 0) > 0:
            reward += result['energy_change'] * 10
        
        # Survival reward
        if self.energy > 50:
            reward += 1.0
        
        # Exploration reward
        if percept.get('novelty', 0) > 0.5:
            reward += percept['novelty'] * 5
        
        # Social reward
        if action.get('type') == 'interact':
            nearby_agents = len(self.getNearbyAgents(5))
            reward += min(10, nearby_agents * 2)
        
        return reward
    
    def updateNeuralNetwork(self, percept: nlm.SensoryInput, action: nlm.Action, reward: float):
        """Update neural network based on experience.
        
        Args:
            percept: Sensory input
            action: Action taken
            reward: Received reward
        """
        # Update based on reward prediction error
        # This would typically involve more complex neural network operations
        pass
    
    def updateMemory(self, percept: nlm.SensoryInput, action: nlm.Action, reward: float, result: Dict[str, Any]):
        """Update agent's memory with new experience.
        
        Args:
            percept: Sensory input
            action: Action taken
            reward: Received reward
            result: Action result
        """
        experience = {
            'percept': percept,
            'action': action,
            'reward': reward,
            'result': result,
            'timestamp': self.age
        }
        
        # Store in episodic memory
        self.memory['episodic'] = self.memory.get('episodic', []) + [experience]
        
        # Update associative memory
        self.updateAssociativeMemory(experience)
        
        # Trim memory if too large
        if len(self.memory['episodic']) > 1000:
            self.memory['episodic'] = self.memory['episodic'][-500:]
    
    def updateAssociativeMemory(self, experience: Dict[str, Any]):
        """Update associative memory.
        
        Args:
            experience: Agent experience
        """
        # Implement Hebbian learning
        percept = experience['percept']
        action = experience['action']
        reward = experience['reward']
        
        # Simple associative memory update
        if 'percept_patterns' not in self.memory:
            self.memory['percept_patterns'] = {}
        
        # Store pattern
        pattern_key = self.extractPatternKey(percept)
        if pattern_key not in self.memory['percept_patterns']:
            self.memory['percept_patterns'][pattern_key] = {
                'actions': [],
                'rewards': [],
                'frequencies': {}
            }
        
        pattern = self.memory['percept_patterns'][pattern_key]
        pattern['actions'].append(action)
        pattern['rewards'].append(reward)
        pattern['frequencies'][action] = pattern['frequencies'].get(action, 0) + 1
    
    def extractPatternKey(self, percept: nlm.SensoryInput) -> str:
        """Extract pattern key from percept.
        
        Args:
            percept: Sensory input
            
        Returns:
            Pattern key string
        """
        # Simple pattern extraction
        return f"{percept.get('type', 'unknown')}_{percept.get('intensity', 0):.2f}"
    
    def updateStateFromAction(self, action: nlm.Action, result: Dict[str, Any]):
        """Update agent state based on action result.
        
        Args:
            action: Action taken
            result: Action result
        """
        # Update position based on movement
        if action.get('type') == 'move':
            dx, dy = action.get('parameters', [0, 0])
            self.position = (self.position[0] + dx, self.position[1] + dy)
        
        # Update energy based on action
        if result.get('energy_change'):
            self.energy += result['energy_change']
        
        # Update health based on action
        if result.get('health_change'):
            self.health += result['health_change']
    
    def applyPhysiologicalEffects(self, action: nlm.Action, result: Dict[str, Any]):
        """Apply physiological effects of action.
        
        Args:
            action: Action taken
            result: Action result
        """
        # Update emotions based on action
        if action.get('type') == 'move':
            self.emotions['curiosity'] *= 1.1  # Movement increases curiosity
        
        if result.get('stress_level'):
            self.emotions['fear'] += result['stress_level'] * 0.1
        
        # Energy decay over time
        self.energy *= 0.995
        self.health *= 0.998
    
    def updateGoalsFromExperience(self):
        """Update goals based on experience."""
        # Analyze experience for goal modification
        if self.memory['episodic']:
            recent_experiences = self.memory['episodic'][-10:]
            
            # Count successful outcomes for different goals
            goal_success = {}
            for exp in recent_experiences:
                if exp['reward'] > 0:
                    # Infer goals from actions
                    action_type = exp['action'].get('type', 'unknown')
                    if action_type not in goal_success:
                        goal_success[action_type] = 0
                    goal_success[action_type] += 1
            
            # Update goal priorities based on success
            for goal in self.goals:
                action_type = goal.replace('_', '')
                if action_type in goal_success:
                    success_rate = goal_success[action_type] / len(recent_experiences)
                    if success_rate < 0.3:
                        # Low success rate - deprioritize goal
                        pass
    
    def getState(self) -> Dict[str, Any]:
        """Get complete agent state.
        
        Returns:
            Agent state dictionary
        """
        return {
            'agent_id': self.agent_id,
            'position': self.position,
            'energy': self.energy,
            'health': self.health,
            'age': self.age,
            'velocity': self.velocity,
            'emotions': self.emotions,
            'goals': self.goals,
            'current_goal': self.current_goal,
            'sensory_history_length': len(self.sensory_history),
            'action_history_length': len(self.action_history),
            'reward_history_length': len(self.reward_history)
        }


class PerceptionFilter:
    """Filters and processes sensory input."""
    
    def process(self, percept: nlm.SensoryInput, agent: AdvancedAgent) -> nlm.SensoryInput:
        """Process sensory input.
        
        Args:
            percept: Raw sensory input
            agent: Agent receiving input
            
        Returns:
            Processed sensory input
        """
        # Apply filters based on agent state
        filtered_percept = percept
        
        # Attention filter
        if agent.emotions['fear'] > 0.5:
            # Fearful agents focus on immediate surroundings
            filtered_percept = self.applyAttentionFilter(filtered_percept, radius=5)
        
        # Curiosity filter
        if agent.emotions['curiosity'] > 0.7:
            # Curious agents enhance novel stimuli
            filtered_percept = self.applyNoveltyFilter(filtered_percept)
        
        # Energy filter
        if agent.energy < 20:
            # Hungry agents enhance food-related stimuli
            filtered_percept = self.applyMotivationFilter(filtered_percept, 'hunger')
        
        return filtered_percept
    
    def applyAttentionFilter(self, percept: nlm.SensoryInput, radius: int) -> nlm.SensoryInput:
        """Apply attention filter focusing on immediate surroundings.
        
        Args:
            percept: Input percept
            radius: Attention radius
            
        Returns:
            Filtered percept
        """
        # Enhance features within attention radius
        # Implementation would depend on percept type
        return percept
    
    def applyNoveltyFilter(self, percept: nlm.SensoryInput) -> nlm.SensoryInput:
        """Apply novelty filter enhancing novel stimuli.
        
        Args:
            percept: Input percept
            
        Returns:
            Filtered percept
        """
        # Enhance novelty detection
        percept['novelty'] = min(1.0, percept.get('novelty', 0) * 1.5)
        return percept
    
    def applyMotivationFilter(self, percept: nlm.SensoryInput, motivation: str) -> nlm.SensoryInput:
        """Apply motivation filter enhancing motivation-related stimuli.
        
        Args:
            percept: Input percept
            motivation: Motivation type
            
        Returns:
            Filtered percept
        """
        # Enhance motivation-related features
        if motivation == 'hunger':
            # Enhance food-related stimuli
            if percept.get('type') == 'visual':
                percept['food_intensity'] = percept.get('food_intensity', 0) * 1.8
        
        return percept


class ActionSelector:
    """Selects optimal actions based on preferences and constraints."""
    
    def select(self, preferences: nlm.Action, goals: List[str], emotions: Dict[str, float], available_actions: List[nlm.Action]) -> nlm.Action:
        """Select optimal action.
        
        Args:
            preferences: Action preferences from neural network
            goals: Current goals
            emotions: Current emotional state
            available_actions: Available actions
            
        Returns:
            Selected action
        """
        # Score each available action
        action_scores = []
        
        for action in available_actions:
            score = self.calculateActionScore(action, preferences, goals, emotions)
            action_scores.append((action, score))
        
        # Select highest scoring action
        if action_scores:
            selected_action = max(action_scores, key=lambda x: x[1])[0]
            return selected_action
        
        return available_actions[0] if available_actions else None
    
    def calculateActionScore(self, action: nlm.Action, preferences: nlm.Action, goals: List[str], emotions: Dict[str, float]) -> float:
        """Calculate score for an action.
        
        Args:
            action: Action to score
            preferences: Action preferences
            goals: Current goals
            emotions: Current emotions
            
        Returns:
            Action score
        """
        score = 0.0
        
        # Preference score
        if preferences:
            action_type = action.get('type', 'unknown')
            preference_score = preferences.get(action_type, 0.0)
            score += preference_score * 0.4
        
        # Goal alignment score
        goal_alignment = 0.0
        for goal in goals:
            if self.actionAdvancesGoal(action, goal):
                goal_alignment += 1.0 / len(goals)
        
        score += goal_alignment * 0.3
        
        # Emotion alignment score
        emotion_alignment = self.getEmotionAlignment(action, emotions)
        score += emotion_alignment * 0.2
        
        # Feasibility score
        feasibility = 1.0 if self.isFeasible(action) else 0.0
        score += feasibility * 0.1
        
        return score
    
    def actionAdvancesGoal(self, action: nlm.Action, goal: str) -> bool:
        """Check if action advances a goal.
        
        Args:
            action: Action to check
            goal: Goal to check against
            
        Returns:
            True if action advances goal
        """
        action_type = action.get('type', 'unknown')
        
        goal_action_map = {
            'find_food': ['move', 'interact'],
            'find_shelter': ['move', 'interact'],
            'explore': ['move'],
            'interact_socially': ['interact']
        }
        
        return action_type in goal_action_map.get(goal, [])
    
    def getEmotionAlignment(self, action: nlm.Action, emotions: Dict[str, float]) -> float:
        """Get emotion alignment score for action.
        
        Args:
            action: Action to check
            emotions: Current emotions
            
        Returns:
            Emotion alignment score
        """
        action_type = action.get('type', 'unknown')
        
        # Emotion-action compatibility matrix
        compatibility_matrix = {
            'move': {'curiosity': 0.8, 'fear': 0.3},
            'interact': {'curiosity': 0.7, 'fear': 0.5},
            'rest': {'fear': 0.9, 'hunger': 0.4}
        }
        
        total_compatibility = 0.0
        count = 0
        
        for emotion, value in emotions.items():
            if emotion in compatibility_matrix.get(action_type, {}):
                total_compatibility += compatibility_matrix[action_type][emotion] * value
                count += 1
        
        return total_compatibility / count if count > 0 else 0.0
    
    def isFeasible(self, action: nlm.Action) -> bool:
        """Check if action is feasible.
        
        Args:
            action: Action to check
            
        Returns:
            True if action is feasible
        """
        action_type = action.get('type', 'unknown')
        
        # Check if action type is supported
        feasible_actions = ['move', 'look', 'rest', 'interact']
        return action_type in feasible_actions


class MultiAgentEnvironment:
    """Environment supporting multiple agents."""
    
    def __init__(self, width: int = 50, height: int = 50):
        """Initialize multi-agent environment.
        
        Args:
            width: Environment width
            height: Environment height
        """
        self.width = width
        self.height = height
        self.agents = []
        self.world_objects = []
        
        # Add resources
        self.addResourceNodes()
    
    def addAgent(self, agent: AdvancedAgent) -> None:
        """Add agent to environment.
        
        Args:
            agent: Agent to add
        """
        self.agents.append(agent)
        
        # Place agent at random position
        x = random.randint(0, self.width - 1)
        y = random.randint(0, self.height - 1)
        agent.position = (x, y)
    
    def addResourceNode(self, position: tuple, resource_type: str) -> None:
        """Add resource node to environment.
        
        Args:
            position: Resource position
            resource_type: Resource type
        """
        resource = {
            'position': position,
            'type': resource_type,
            'amount': 100,
            'regeneration_rate': 0.1
        }
        self.world_objects.append(resource)
    
    def addResourceNodes(self) -> None:
        """Add resource nodes to environment."""
        # Add food sources
        for _ in range(10):
            x = random.randint(0, self.width - 1)
            y = random.randint(0, self.height - 1)
            self.addResourceNode((x, y), 'food')
        
        # Add shelter nodes
        for _ in range(5):
            x = random.randint(0, self.width - 1)
            y = random.randint(0, self.height - 1)
            self.addResourceNode((x, y), 'shelter')
    
    def step(self, time_step: float) -> Dict[str, Any]:
        """Advance environment by time step.
        
        Args:
            time_step: Time step
            
        Returns:
            Environment step results
        """
        results = {
            'agent_steps': [],
            'resource_changes': [],
            'agent_interactions': []
        }
        
        # Update each agent
        for agent in self.agents:
            # Get sensory input
            percept = self.getSensoryInput(agent.position)
            
            # Make decision
            action = agent.decide(percept)
            
            # Execute action
            action_result = agent.act(action)
            
            # Learn from experience
            agent.learn(percept, action, action_result)
            
            # Record agent step
            results['agent_steps'].append({
                'agent_id': agent.agent_id,
                'position': agent.position,
                'action': action,
                'result': action_result,
                'energy': agent.energy,
                'health': agent.health
            })
        
        # Update resources
        for resource in self.world_objects:
            resource['amount'] *= (1 + resource['regeneration_rate'] * time_step)
            results['resource_changes'].append(resource)
        
        # Update environment state
        self.updateEnvironment(time_step)
        
        return results
    
    def getSensoryInput(self, position: tuple) -> nlm.SensoryInput:
        """Get sensory input for position.
        
        Args:
            position: Agent position
            
        Returns:
            Sensory input
        """
        x, y = position
        percept = nlm.SensoryInput('visual')
        
        # Collect environmental features
        features = {
            'position': position,
            'nearby_resources': [],
            'nearby_agents': [],
            'terrain_type': 'normal',
            'visibility': self.calculateVisibility(position)
        }
        
        # Add nearby resources
        for resource in self.world_objects:
            rx, ry = resource['position']
            distance = math.sqrt((x - rx)**2 + (y - ry)**2)
            
            if distance <= 10:  # Perception radius
                features['nearby_resources'].append({
                    'type': resource['type'],
                    'amount': resource['amount'],
                    'distance': distance
                })
        
        # Add nearby agents
        for agent in self.agents:
            ax, ay = agent.position
            distance = math.sqrt((x - ax)**2 + (y - ay)**2)
            
            if distance <= 10 and (ax != x or ay != y):
                features['nearby_agents'].append({
                    'agent_id': agent.agent_id,
                    'distance': distance,
                    'energy': agent.energy,
                    'health': agent.health
                })
        
        percept.setData(features)
        return percept
    
    def calculateVisibility(self, position: tuple) -> float:
        """Calculate visibility from position.
        
        Args:
            position: Agent position
            
        Returns:
            Visibility (0.0 to 1.0)
        """
        x, y = position
        
        # Check for obstacles
        obstacles = 0
        total_rays = 0
        
        for angle in range(0, 360, 15):  # Cast rays in different directions
            rad = math.radians(angle)
            
            for distance in range(1, 20):
                dx = int(x + distance * math.cos(rad))
                dy = int(y + distance * math.sin(rad))
                
                if not (0 <= dx < self.width and 0 <= dy < self.height):
                    break
                
                # Check if obstacle
                if self.isObstacle(dx, dy):
                    obstacles += 1
                    break
                
                total_rays += 1
        
        # Calculate visibility
        visibility = 1.0 - (obstacles / total_rays) if total_rays > 0 else 1.0
        return max(0.0, min(1.0, visibility))
    
    def isObstacle(self, x: int, y: int) -> bool:
        """Check if position is obstacle.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if obstacle
        """
        # Simple obstacle detection
        for agent in self.agents:
            if agent.position == (x, y):
                return True
        
        # Check for obstacles in world objects
        for resource in self.world_objects:
            if resource['position'] == (x, y) and resource['type'] == 'shelter':
                return True
        
        return False
    
    def updateEnvironment(self, time_step: float) -> None:
        """Update environment state.
        
        Args:
            time_step: Time step
        """
        # Update environmental factors
        for agent in self.agents:
            # Environmental decay
            agent.energy *= (1 - time_step * 0.01)
            agent.health *= (1 - time_step * 0.005)
            
            # Environmental influence
            self.applyEnvironmentalInfluence(agent, time_step)
    
    def applyEnvironmentalInfluence(self, agent: AdvancedAgent, time_step: float) -> None:
        """Apply environmental influence to agent.
        
        Args:
            agent: Agent to influence
            time_step: Time step
        """
        # Weather effects
        temperature = self.getTemperature(agent.position)
        humidity = self.getHumidity(agent.position)
        
        # Temperature effects
        if temperature > 35:
            agent.health *= (1 - time_step * 0.02)
            agent.emotions['fear'] += time_step * 0.1
        
        # Humidity effects
        if humidity > 0.8:
            agent.energy *= (1 - time_step * 0.01)
        
        # Light effects
        light_level = self.getLightLevel(agent.position)
        if light_level < 0.2:
            agent.emotions['fear'] += time_step * 0.05


# Multi-Agent Simulation Examples

def run_multi_agent_simulation():
    """Example multi-agent simulation."""
    print("=== Multi-Agent Simulation Example ===")
    
    # Create environment
    environment = MultiAgentEnvironment(width=100, height=100)
    
    # Create agents
    agents = []
    for i in range(5):
        # Create brain for agent
        config = nlm.create_minimal_config()
        brain = nlm.create_brain(config)
        
        # Create agent
        agent = AdvancedAgent(brain, environment, f"agent_{i}")
        agents.append(agent)
        
        # Add to environment
        environment.addAgent(agent)
    
    # Run simulation
    for step in range(200):
        # Advance environment
        results = environment.step(0.001)
        
        # Log results
        if step % 50 == 0:
            print(f"Step {step}:")
            for agent_result in results['agent_steps']:
                print(f"  {agent_result['agent_id']}: "
                      f"Pos {agent_result['position']}, "
                      f"Energy {agent_result['energy']:.2f}, "
                      f"Health {agent_result['health']:.2f}")
    
    # Print final statistics
    print(f"\n=== Simulation Results ===")
    print(f"Total agents: {len(agents)}")
    print(f"Final positions:")
    for agent in agents:
        print(f"  {agent.agent_id}: {agent.position}")


def run_agent_competition_simulation():
    """Example agent competition simulation."""
    print("=== Agent Competition Simulation Example ===")
    
    # Create environment
    environment = MultiAgentEnvironment(width=80, height=80)
    
    # Create competing agents with different strategies
    agents = []
    
    # Hungry agent strategy
    hungry_config = nlm.create_high_performance_config()
    hungry_brain = nlm.create_brain(hungry_config)
    hungry_agent = AdvancedAgent(hungry_brain, environment, "hungry_agent")
    hungry_agent.emotions['hunger'] = 0.9
    agents.append(hungry_agent)
    environment.addAgent(hungry_agent)
    
    # Curious agent strategy  
    curious_config = nlm.create_high_performance_config()
    curious_brain = nlm.create_brain(curious_config)
    curious_agent = AdvancedAgent(curious_brain, environment, "curious_agent")
    curious_agent.emotions['curiosity'] = 0.9
    agents.append(curious_agent)
    environment.addAgent(curious_agent)
    
    # Fearful agent strategy
    fearful_config = nlm.create_high_performance_config()
    fearful_brain = nlm.create_brain(fearful_config)
    fearful_agent = AdvancedAgent(fearful_brain, environment, "fearful_agent")
    fearful_agent.emotions['fear'] = 0.9
    agents.append(fearful_agent)
    environment.addAgent(fearful_agent)
    
    # Run competition
    for step in range(300):
        results = environment.step(0.001)
        
        if step % 50 == 0:
            print(f"Step {step}:")
            for agent_result in results['agent_steps']:
                agent = next(a for a in agents if a.agent_id == agent_result['agent_id'])
                strategy = getAgentStrategy(agent)
                print(f"  {agent.agent_id} ({strategy}): "
                      f"Pos {agent_result['position']}, "
                      f"Energy {agent_result['energy']:.2f}")
    
    # Analyze competition results
    print(f"\n=== Competition Analysis ===")
    survivors = [agent for agent in agents if agent.energy > 10]
    print(f"Survivors: {len(survivors)}/{len(agents)}")
    
    if survivors:
        most_successful = max(survivors, key=lambda a: a.energy)
        print(f"Most successful agent: {most_successful.agent_id} "
              f"(Energy: {most_successful.energy:.2f})")


def getAgentStrategy(agent: AdvancedAgent) -> str:
    """Get agent strategy based on dominant emotions.
    
    Args:
        agent: Agent to analyze
        
    Returns:
        Agent strategy
    """
    emotions = agent.emotions
    
    if emotions['hunger'] > 0.7:
        return "hungry_forager"
    elif emotions['curiosity'] > 0.7:
        return "explorer"
    elif emotions['fear'] > 0.7:
        return "avoidant_survivor"
    else:
        return "balanced"


def run_agent_cooperation_simulation():
    """Example agent cooperation simulation."""
    print("=== Agent Cooperation Simulation Example ===")
    
    # Create environment with cooperative resources
    environment = MultiAgentEnvironment(width=60, height=60)
    
    # Add cooperative resources
    for _ in range(5):
        x = random.randint(10, 50)
        y = random.randint(10, 50)
        environment.addResourceNode((x, y), 'cooperative_resource')
    
    # Create cooperative agents
    agents = []
    for i in range(3):
        config = nlm.create_minimal_config()
        brain = nlm.create_brain(config)
        agent = AdvancedAgent(brain, environment, f"cooperative_agent_{i}")
        
        # Set cooperative goals
        agent.goals = ['interact_socially', 'share_resources', 'build_shelter']
        agent.emotions['curiosity'] = 0.5
        
        agents.append(agent)
        environment.addAgent(agent)
    
    # Run cooperation simulation
    for step in range(200):
        results = environment.step(0.001)
        
        # Track cooperation metrics
        if step % 25 == 0:
            print(f"Step {step}:")
            
            # Calculate cooperation metrics
            resource_access = analyzeResourceAccess(results['agent_steps'])
            social_interactions = countSocialInteractions(results['agent_steps'])
            
            print(f"  Resource access rate: {resource_access:.2%}")
            print(f"  Social interactions: {social_interactions}")
            
            # Show agent states
            for agent_result in results['agent_steps']:
                print(f"    {agent_result['agent_id']}: "
                      f"Energy {agent_result['energy']:.2f}")
    
    # Analyze cooperation effectiveness
    print(f"\n=== Cooperation Analysis ===")
    analyzeCooperationMetrics(results['agent_steps'])


def analyzeResourceAccess(agent_steps: List[Dict[str, Any]]) -> float:
    """Analyze resource access patterns.
    
    Args:
        agent_steps: Agent step results
        
    Returns:
        Resource access rate
    """
    resource_interactions = 0
    total_interactions = 0
    
    for step in agent_steps:
        # Check for resource interaction actions
        action = step['action']
        if action.get('type') == 'interact':
            total_interactions += 1
            # Determine if resource-related
            if 'resource' in action.get('parameters', {}).get('target', ''):
                resource_interactions += 1
    
    return resource_interactions / max(total_interactions, 1)


def countSocialInteractions(agent_steps: List[Dict[str, Any]]) -> int:
    """Count social interactions.
    
    Args:
        agent_steps: Agent step results
        
    Returns:
        Number of social interactions
    """
    social_count = 0
    
    for step in agent_steps:
        action = step['action']
        if action.get('type') == 'interact':
            # Check if targeting another agent
            parameters = action.get('parameters', {})
            if parameters.get('target_type') == 'agent':
                social_count += 1
    
    return social_count


def analyzeCooperationMetrics(agent_steps: List[Dict[str, Any]]) -> None:
    """Analyze cooperation metrics.
    
    Args:
        agent_steps: Agent step results
    """
    # Calculate cooperation indices
    survival_rates = []
    cooperation_levels = []
    resource_sharing = []
    
    # Analyze final states
    final_states = {}
    for step in agent_steps:
        agent_id = step['agent_id']
        if agent_id not in final_states:
            final_states[agent_id] = step
    
    for agent_id, state in final_states.items():
        energy = state['energy']
        survival_rates.append(1 if energy > 20 else 0)
        
        # Simple cooperation metric based on energy sharing
        if energy > 50:
            resource_sharing.append(1)
        else:
            resource_sharing.append(0)
    
    # Calculate averages
    avg_survival_rate = sum(survival_rates) / len(survival_rates)
    avg_resource_sharing = sum(resource_sharing) / len(resource_sharing)
    
    print(f"  Survival rate: {avg_survival_rate:.2%}")
    print(f"  Resource sharing rate: {avg_resource_sharing:.2%}")
    
    # Determine cooperation level
    if avg_survival_rate > 0.8 and avg_resource_sharing > 0.6:
        print("  Cooperation level: HIGH - Agents working together effectively")
    elif avg_survival_rate > 0.6:
        print("  Cooperation level: MODERATE - Some cooperation observed")
    else:
        print("  Cooperation level: LOW - Limited cooperation")


# Run example simulations (when script is executed directly)
if __name__ == "__main__":
    import random
    
    print("Running Intermediate Tutorial Examples\n")
    
    # Run multi-agent simulation
    run_multi_agent_simulation()
    
    print("\n" + "="*60 + "\n")
    
    # Run competition simulation
    run_agent_competition_simulation()
    
    print("\n" + "="*60 + "\n")
    
    # Run cooperation simulation
    run_agent_cooperation_simulation()

# Note: This tutorial requires the AdvancedAgent class to be properly integrated
# with the actual NLM brain and world implementations.
```