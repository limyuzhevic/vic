# NLM Python Advanced Usage Guide

## Overview

This guide provides advanced usage patterns and techniques for the NLM Python API. It covers sophisticated simulation strategies, optimization techniques, and complex use cases for experienced users and developers.

## Table of Contents

- [Advanced Simulation Techniques](#advanced-simulation-techniques)
- [Memory Management](#memory-management)
- [Parallel and Distributed Computing](#parallel-and-distributed-computing)
- [Custom Development](#custom-development)
- [Performance Optimization](#performance-optimization)
- [Advanced Configuration](#advanced-configuration)
- [Troubleshooting Advanced Issues](#troubleshooting-advanced-issues)

## Advanced Simulation Techniques

### Event-Driven Simulation

```python
import pynlm
import time

class EventDrivenSimulation:
    """Advanced event-driven simulation with precise timing"""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("simulation.use_event_driven", True)
        self.config.set("simulation.event_threshold", 0.9)
        self.config.set("simulation.max_events_per_step", 5000)
        self.config.set("simulation.min_event_interval", 0.001)
        
        # Event queues
        self.spike_events = []
        self.plastic_events = []
        self.development_events = []
        
    def run_event_driven_simulation(self, steps, event_callback=None):
        """Run event-driven simulation with precise timing control"""
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        for step in range(steps):
            step_start = time.time()
            
            # Process scheduled events
            self.process_events(step, event_callback)
            
            # Schedule new events based on simulation state
            self.schedule_events(step)
            
            # Update simulation state
            brain.step(step)
            
            # Record timing
            step_time = time.time() - step_start
            print(f"Step {step}: {step_time:.4f}s, Events: {len(self.spike_events)}")
            
    def process_events(self, step, event_callback=None):
        """Process events in chronological order"""
        
        # Sort events by time
        all_events = []
        all_events.extend(self.spike_events)
        all_events.extend(self.plastic_events)
        all_events.extend(self.development_events)
        
        all_events.sort(key=lambda e: e['time'])
        
        # Process events
        for event in all_events:
            if event['time'] <= step:
                if event['type'] == 'spike':
                    self.handle_spike_event(event)
                elif event['type'] == 'plastic':
                    self.handle_plastic_event(event)
                elif event['type'] == 'development':
                    self.handle_development_event(event)
                    
                # Call user callback if provided
                if event_callback:
                    event_callback(event)
                    
        # Clear processed events
        self.spike_events.clear()
        self.plastic_events.clear()
        self.development_events.clear()
        
    def schedule_events(self, step):
        """Schedule new events based on simulation state"""
        
        # Schedule spike events
        firing_neurons = brain.getFiringNeuronCount()
        if firing_neurons > 0:
            spike_count = int(firing_neurons * 1.5)  # Scale factor
            for i in range(spike_count):
                event = {
                    'type': 'spike',
                    'time': step + 0.1 + (i * 0.01),  # Staggered timing
                    'neuron_id': i,
                    'weight': np.random.random()
                }
                self.spike_events.append(event)
        
        # Schedule plasticity events
        if self.config.getBool("learning.enable_stdp"):
            event = {
                'type': 'plastic',
                'time': step + 0.05,
                'plasticity_type': 'stdp',
                'strength': self.config.getFloat("learning.stdp_strength")
            }
            self.plastic_events.append(event)
            
        # Schedule development events
        if self.config.getBool("neuromodulation.enable_development"):
            current_stage = brain.getDevelopmentalStage()
            if current_stage != pynlm.DevelopmentalStage.Aging:
                event = {
                    'type': 'development',
                    'time': step + 0.02,
                    'stage': current_stage,
                    'age': self.get_development_age()
                }
                self.development_events.append(event)
                
    def handle_spike_event(self, event):
        """Handle spike event"""
        # Implement custom spike handling
        pass
        
    def handle_plastic_event(self, event):
        """Handle plasticity event"""
        # Implement custom plasticity handling
        pass
        
    def handle_development_event(self, event):
        """Handle development event"""
        # Implement custom development handling
        pass
```

### Custom Development Environments

```python
import pynlm
import numpy as np

class CustomWorld:
    """Custom world implementation for advanced simulations"""
    
    def __init__(self):
        self.width = 50
        self.height = 50
        self.time_step = 0.05
        self.obstacle_map = np.zeros((50, 50), dtype=bool)
        self.resource_map = np.zeros((50, 50), dtype=float)
        self.agent_position = (25, 25)
        self.agent_energy = 100.0
        self.max_energy = 100.0
        
        # Initialize world
        self._initialize_world()
        
    def _initialize_world(self):
        """Initialize world with custom terrain and resources"""
        
        # Create custom terrain
        for i in range(self.width):
            for j in range(self.height):
                # Create interesting terrain patterns
                distance_from_center = np.sqrt((i - 25)**2 + (j - 25)**2)
                if distance_from_center > 20 and distance_from_center < 30:
                    self.obstacle_map[i, j] = True  # Create circular obstacle
                    
        # Create resource distribution
        for i in range(self.width):
            for j in range(self.height):
                # Higher resources in center, lower on edges
                distance_from_center = np.sqrt((i - 25)**2 + (j - 25)**2)
                self.resource_map[i, j] = np.exp(-distance_from_center / 15)
                
    def update(self, time_step):
        """Update world state"""
        
        # Update agent
        self._update_agent(time_step)
        
        # Update resources
        self._update_resources(time_step)
        
        # Update obstacles (optional)
        self._update_obstacles(time_step)
        
        # Update environment
        self._update_environment(time_step)
        
    def _update_agent(self, time_step):
        """Update agent state"""
        
        # Move agent based on internal decision
        action = self._decide_action()
        new_position = self._apply_action(self.agent_position, action)
        
        if self._is_valid_position(new_position):
            self.agent_position = new_position
            self.agent_energy -= action['energy_cost']
            
        # Regenerate resources at agent position
        if time_step % 100 == 0:
            x, y = self.agent_position
            self.resource_map[x, y] = min(1.0, self.resource_map[x, y] + 0.1)
            
    def _decide_action(self):
        """Decisive action based on complex logic"""
        
        # Consider multiple factors
        energy_factor = self.agent_energy / self.max_energy
        location_factor = self._get_location_factor()
        risk_factor = self._calculate_risk_factor()
        
        # Choose action based on factors
        if energy_factor < 0.3:  # Low energy
            action = {'type': 'move_to_resource', 'energy_cost': 10.0}
        elif risk_factor > 0.8:  # High risk
            action = {'type': 'stay_away', 'energy_cost': 2.0}
        elif location_factor > 0.7:  # Good location
            action = {'type': 'explore', 'energy_cost': 5.0}
        else:
            action = {'type': 'random_walk', 'energy_cost': 3.0}
            
        return action
        
    def _get_location_factor(self):
        """Get location preference factor"""
        x, y = self.agent_position
        return self.resource_map[x, y] / 1.0
        
    def _calculate_risk_factor(self):
        """Calculate risk factor based on proximity to obstacles"""
        x, y = self.agent_position
        
        # Check nearby positions for obstacles
        nearby_risk = 0.0
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                nx, ny = x + dx, y + dy
                if self._is_valid_position((nx, ny)):
                    if self.obstacle_map[nx, ny]:
                        nearby_risk += 0.2
                        
        return min(nearby_risk, 1.0)
        
    def _is_valid_position(self, position):
        """Check if position is valid"""
        x, y = position
        return (0 <= x < self.width and 
                0 <= y < self.height and 
                not self.obstacle_map[x, y])
                
    def getSensoryPercept(self):
        """Get sensory perception from world"""
        
        x, y = self.agent_position
        percept = {
            'vision': self._get_vision_data(x, y),
            'energy': self.agent_energy,
            'time': self._get_simulation_time(),
            'resources_nearby': self._get_nearby_resources(x, y),
            'obstacles_nearby': self._get_nearby_obstacles(x, y)
        }
        
        return percept
        
    def _get_vision_data(self, x, y):
        """Get vision data for agent"""
        
        vision_data = []
        vision_range = 5
        
        for dx in range(-vision_range, vision_range + 1):
            for dy in range(-vision_range, vision_range + 1):
                nx, ny = x + dx, y + dy
                
                if self._is_valid_position((nx, ny)):
                    distance = np.sqrt(dx**2 + dy**2)
                    if distance <= vision_range:
                        vision_data.append({
                            'position': (nx, ny),
                            'resource_value': self.resource_map[nx, ny],
                            'is_obstacle': self.obstacle_map[nx, ny],
                            'distance': distance,
                            'angle': np.arctan2(dy, dx)
                        })
                        
        return vision_data
        
    def _get_nearby_resources(self, x, y):
        """Get resources in nearby cells"""
        
        resources = []
        for dx in range(-3, 4):
            for dy in range(-3, 4):
                nx, ny = x + dx, y + dy
                if self._is_valid_position((nx, ny)):
                    resources.append(self.resource_map[nx, ny])
                    
        return resources
        
    def _get_nearby_obstacles(self, x, y):
        """Get obstacles in nearby cells"""
        
        obstacles = []
        for dx in range(-3, 4):
            for dy in range(-3, 4):
                nx, ny = x + dx, y + dy
                if self._is_valid_position((nx, ny)):
                    obstacles.append(self.obstacle_map[nx, ny])
                    
        return obstacles
        
    def getAgentBody(self):
        """Get agent body information"""
        return {
            'position': self.agent_position,
            'energy': self.agent_energy,
            'max_energy': self.max_energy,
            'velocity': (0.0, 0.0),  # Simplified for this example
        }
        
    def getSimulationTime(self):
        """Get simulation time"""
        # Could be tracked externally
        return 0.0
        
    def applyMotorCommand(self, action, time):
        """Apply motor command to agent"""
        
        if action['type'] == 'move_to_resource':
            self._move_to_nearest_resource()
        elif action['type'] == 'stay_away':
            self._stay_away_from_obstacles()
        elif action['type'] == 'explore':
            self._move_randomly()
        elif action['type'] == 'random_walk':
            self._move_randomly()
            
    def _move_to_nearest_resource(self):
        """Move to nearest resource"""
        
        min_distance = float('inf')
        nearest_resource = None
        
        for i in range(self.width):
            for j in range(self.height):
                if self.resource_map[i, j] > 0.1:
                    distance = np.sqrt((i - self.agent_position[0])**2 + 
                                     (j - self.agent_position[1])**2)
                    if distance < min_distance:
                        min_distance = distance
                        nearest_resource = (i, j)
                        
        if nearest_resource:
            self.agent_position = nearest_resource
            
    def _stay_away_from_obstacles(self):
        """Move away from obstacles"""
        
        # Simple implementation: move randomly but avoid obstacles
        self._move_randomly()
        
    def _move_randomly(self):
        """Move randomly"""
        
        # Try random moves
        for attempt in range(10):
            dx = np.random.randint(-1, 2)
            dy = np.random.randint(-1, 2)
            
            new_position = (self.agent_position[0] + dx, 
                          self.agent_position[1] + dy)
            
            if self._is_valid_position(new_position):
                self.agent_position = new_position
                break
                
    def _update_resources(self, time_step):
        """Update resource levels over time"""
        
        # Natural resource regeneration
        regeneration_rate = 0.001
        for i in range(self.width):
            for j in range(self.height):
                if np.random.random() < regeneration_rate:
                    self.resource_map[i, j] = min(1.0, 
                                                 self.resource_map[i, j] + 0.01)
        
        # Resource depletion by agent
        x, y = self.agent_position
        depletion_rate = 0.01
        if np.random.random() < depletion_rate:
            self.resource_map[x, y] = max(0.0, 
                                         self.resource_map[x, y] - 0.1)

# Use custom world in advanced simulation
def advanced_simulation_with_custom_world():
    """Run simulation with custom world"""
    
    # Create custom world
    world = CustomWorld()
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("learning.rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Energy={world.agent_energy:.1f}, "
                  f"Resources={np.sum(world.resource_map):.1f}")
    
    return brain, world, agent

# Run advanced simulation
advanced_simulation_with_custom_world()
```

### Multi-Agent Coordination

```python
import pynlm
import numpy as np
import time

class MultiAgentCoordinator:
    """Coordinate multiple agents in shared environment"""
    
    def __init__(self, num_agents=3):
        self.num_agents = num_agents
        self.agents = []
        self.shared_world = pynlm.createSimpleWorld()
        self.shared_world.configure(width=100, height=100, 
                                   visionWidth=10, visionHeight=10)
        self.shared_world.reset()
        
        # Create agents
        self._create_agents()
        
    def _create_agents(self):
        """Create multiple agents"""
        
        # Create shared brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("learning.rate", 0.01)
        
        shared_brain = pynlm.createBrain(config)
        shared_brain.initialize()
        
        # Create different agents with different behaviors
        for i in range(self.num_agents):
            agent = pynlm.createAgentBrain(shared_brain)
            agent.initialize(self.shared_world)
            
            # Configure each agent differently
            if i == 0:
                # Agent 1: Resource seeker
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
            elif i == 1:
                # Agent 2: Avoider
                agent.enableCuriosity(True)
                agent.enableStructuralPlasticity(True)
            elif i == 2:
                # Agent 3: Balanced
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
                
            self.agents.append(agent)
            
    def run_coordinated_simulation(self, steps):
        """Run simulation with agent coordination"""
        
        # Initialize agents at different positions
        for i, agent in enumerate(self.agents):
            x = (i + 1) * 25
            y = 50 - (i + 1) * 10
            self.shared_world.setAgentStart(x, y)
            
        for step in range(steps):
            # Update world
            self.shared_world.update(0.1)
            
            # Get shared perception (same for all agents)
            shared_percept = self.shared_world.getSensoryPercept()
            
            # Each agent processes perception differently
            for i, agent in enumerate(self.agents):
                # Agent-specific processing
                modified_percept = self._modify_perception(
                    shared_percept, i, step)
                    
                # Process sensory input
                agent.processSensoryInput(modified_percept)
                
                # Brain computation
                agent.decodeMotorCommand()  # Each agent has same brain
                
            # Apply all agents' actions
            for agent in self.agents:
                action = agent.decodeMotorCommand()
                self.shared_world.applyMotorCommand(action, 
                                                 self.shared_world.getSimulationTime())
                
            # Print coordination status
            if step % 100 == 0:
                self._print_coordination_status(step)
                
    def _modify_perception(self, shared_percept, agent_id, step):
        """Modify perception for individual agent"""
        
        # Create modified perception based on agent ID and step
        modified_percept = shared_percept.copy()
        
        # Add agent-specific biases
        if agent_id == 0:  # Resource seeker
            # Emphasize resource information
            if 'vision' in modified_percept:
                for cell in modified_percept['vision']:
                    if cell['is_obstacle']:
                        cell['resource_value'] *= 0.1  # Ignore obstacles
                        
        elif agent_id == 1:  # Avoider
            # Emphasize obstacle information
            if 'vision' in modified_percept:
                for cell in modified_percept['vision']:
                    if not cell['is_obstacle']:
                        cell['resource_value'] *= 0.1  # Ignore resources
                        
        elif agent_id == 2:  # Balanced
            # Maintain normal perception
            pass
            
        # Add temporal component
        if 'time' in modified_percept:
            modified_percept['time'] += step * 0.001
            
        return modified_percept
        
    def _print_coordination_status(self, step):
        """Print coordination status"""
        
        # Get world state
        percept = self.shared_world.getSensoryPercept()
        
        print(f"Step {step}:")
        print(f"  World time: {self.shared_world.getSimulationTime():.2f}")
        
        if 'vision' in percept:
            total_resources = sum(cell['resource_value'] 
                                for cell in percept['vision'])
            total_obstacles = sum(1 for cell in percept['vision'] 
                                if cell['is_obstacle'])
            print(f"  Resources: {total_resources:.2f}, Obstacles: {total_obstacles}")
            
        # Print agent statistics (if available)
        for i, agent in enumerate(self.agents):
            print(f"  Agent {i}: ", end="")
            # Agent statistics would go here
            print("N/A")

# Use multi-agent coordinator
multi_agent_coord = MultiAgentCoordinator(3)
multi_agent_coord.run_coordinated_simulation(1000)
```

### Hierarchical Control

```python
import pynlm
import numpy as np

class HierarchicalController:
    """Hierarchical control system with multiple levels"""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("learning.rate", 0.01)
        
        # Create hierarchical brain structure
        self._create_hierarchy()
        
    def _create_hierarchy(self):
        """Create hierarchical brain structure"""
        
        # Create controller brain
        controller_config = pynlm.createDefaultConfig()
        controller_config.set("brain.neuron_count", 500)
        self.controller_brain = pynlm.createBrain(controller_config)
        self.controller_brain.initialize()
        
        # Create executive brain
        executive_config = pynlm.createDefaultConfig()
        executive_config.set("brain.neuron_count", 800)
        self.executive_brain = pynlm.createBrain(executive_config)
        self.executive_brain.initialize()
        
        # Create lower-level brains
        self.lower_brains = []
        for i in range(3):
            lower_config = pynlm.createDefaultConfig()
            lower_config.set("brain.neuron_count", 300)
            lower_brain = pynlm.createBrain(lower_config)
            lower_brain.initialize()
            self.lower_brains.append(lower_brain)
            
        # Create world and agents
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, 
                           visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Create controllers
        self.controller_agent = pynlm.createAgentBrain(self.controller_brain)
        self.executive_agent = pynlm.createAgentBrain(self.executive_brain)
        self.lower_agents = []
        
        for lower_brain in self.lower_brains:
            lower_agent = pynlm.createAgentBrain(lower_brain)
            self.lower_agents.append(lower_agent)
            
        # Initialize all agents
        self.controller_agent.initialize(self.world)
        self.executive_agent.initialize(self.world)
        
        for i, lower_agent in enumerate(self.lower_agents):
            lower_agent.initialize(self.world)
            # Set different starting positions
            self.world.setAgentStart((i + 1) * 10, (i + 1) * 10)
            
    def run_hierarchical_simulation(self, steps):
        """Run hierarchical simulation"""
        
        for step in range(steps):
            # Level 1: Executive functions (planning, strategy)
            executive_plan = self._executive_planning(step)
            
            # Level 2: Controller functions (goal management)
            controller_commands = self._controller_control(step, executive_plan)
            
            # Level 3: Lower-level control (action execution)
            self._lower_level_control(step, controller_commands)
            
            # Update world
            self.world.update(0.1)
            
            # Print hierarchical status
            if step % 100 == 0:
                self._print_hierarchical_status(step)
                
    def _executive_planning(self, step):
        """Executive planning level"""
        
        # Long-term planning
        plan = {
            'goal': 'explore_environment',
            'strategy': 'systematic_coverage',
            'resources_target': 50.0,
            'safety_margin': 0.2,
            'estimated_time': step + 1000
        }
        
        # Execute planning using executive agent
        executive_percept = self.world.getSensoryPercept()
        self.executive_agent.processSensoryInput(executive_percept)
        self.executive_brain.step(step)
        
        # Extract plan from executive agent
        # This would typically involve reading executive agent's output
        
        return plan
        
    def _controller_control(self, step, executive_plan):
        """Controller control level"""
        
        # Create controller percept with executive context
        controller_percept = {
            **self.world.getSensoryPercept(),
            'executive_plan': executive_plan,
            'current_resources': self._get_current_resources(),
            'agent_positions': self._get_agent_positions(),
            'simulation_step': step
        }
        
        # Process using controller agent
        self.controller_agent.processSensoryInput(controller_percept)
        self.controller_brain.step(step)
        
        # Get controller commands
        commands = {
            'action_priority': self._get_action_priority(controller_percept),
            'resource_allocation': self._allocate_resources(controller_percept),
            'safety_checks': self._perform_safety_checks(controller_percept)
        }
        
        return commands
        
    def _lower_level_control(self, step, controller_commands):
        """Lower-level control level"""
        
        # Create lower-level percept with controller commands
        for i, lower_agent in enumerate(self.lower_agents):
            lower_percept = {
                **self.world.getSensoryPercept(),
                'controller_commands': controller_commands,
                'agent_id': i,
                'local_resources': self._get_local_resources(i),
                'neighbor_agents': self._get_neighbor_agents(i)
            }
            
            # Process using lower agent
            lower_agent.processSensoryInput(lower_percept)
            self.lower_brains[i].step(step)
            
            # Get action
            action = lower_agent.decodeMotorCommand()
            
            # Apply action (all agents share same world)
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
    def _get_action_priority(self, percept):
        """Get action priority from controller"""
        
        # Simple priority logic
        resources = percept.get('resources', 0)
        obstacles = percept.get('obstacles', 0)
        
        if resources > 30:
            return 'explore_resources'
        elif obstacles > 10:
            return 'avoid_obstacles'
        else:
            return 'general_exploration'
            
    def _allocate_resources(self, percept):
        """Allocate resources among agents"""
        
        # Simple resource allocation
        total_resources = percept.get('total_resources', 100)
        num_agents = len(self.lower_agents)
        
        allocation = {}
        for i in range(num_agents):
            allocation[i] = total_resources / (num_agents * (i + 1))  # More for lower IDs
            
        return allocation
        
    def _perform_safety_checks(self, percept):
        """Perform safety checks"""
        
        # Check for obstacles
        obstacles = percept.get('obstacles', [])
        
        # Check energy levels
        energy_levels = percept.get('energy_levels', [100] * len(self.lower_agents))
        
        # Check proximity
        positions = percept.get('positions', [])
        
        safety_checks = {
            'obstacle_collision': len([o for o in obstacles if o > 0.5]) < 5,
            'energy_sufficient': all(e > 20 for e in energy_levels),
            'spacing_adequate': self._check_spacing(position for position in positions)
        }
        
        return safety_checks
        
    def _check_spacing(self, positions):
        """Check spacing between agents"""
        
        # Simple spacing check
        for i, pos1 in enumerate(positions):
            for j, pos2 in enumerate(positions):
                if i != j:
                    distance = np.sqrt((pos1[0] - pos2[0])**2 + 
                                     (pos1[1] - pos2[1])**2)
                    if distance < 5:  # Too close
                        return False
                        
        return True
        
    def _get_current_resources(self):
        """Get current resource levels"""
        
        # This would be tracked by the world
        return 100.0  # Placeholder
        
    def _get_agent_positions(self):
        """Get positions of all agents"""
        
        positions = []
        for i in range(self.num_agents):
            # This would get actual agent positions from world
            positions.append((i * 10, i * 10))  # Placeholder
            
        return positions
        
    def _get_local_resources(self, agent_id):
        """Get resources in local area"""
        
        # Get agent's current position
        position = self._get_agent_positions()[agent_id]
        
        # Extract local resources
        local_resources = []
        for dx in range(-3, 4):
            for dy in range(-3, 4):
                x, y = position[0] + dx, position[1] + dy
                # Get resource value from world
                local_resources.append(self._get_resource_at(x, y))
                
        return local_resources
        
    def _get_neighbor_agents(self, agent_id):
        """Get nearby agents"""
        
        agent_position = self._get_agent_positions()[agent_id]
        neighbor_agents = []
        
        for i, position in enumerate(self._get_agent_positions()):
            if i != agent_id:
                distance = np.sqrt((agent_position[0] - position[0])**2 + 
                                 (agent_position[1] - position[1])**2)
                if distance < 20:  # Within neighborhood
                    neighbor_agents.append(i)
                    
        return neighbor_agents
        
    def _get_resource_at(self, x, y):
        """Get resource at position"""
        
        # This would get actual resource value from world
        return 0.5  # Placeholder
        
    def _print_hierarchical_status(self, step):
        """Print hierarchical status"""
        
        print(f"\nHierarchical Control Status - Step {step}:")
        print(f"  Executive Level: Planning and strategy")
        print(f"  Controller Level: Goal management and coordination")
        print(f"  Lower Level: Individual agent control")
        
        # Print level-specific statistics
        print(f"\n  Brain Statistics:")
        print(f"    Controller Brain: {self.controller_brain.getTotalNeuronCount()} neurons")
        print(f"    Executive Brain: {self.executive_brain.getTotalNeuronCount()} neurons")
        print(f"    Lower Brains: {sum(b.getTotalNeuronCount() for b in self.lower_brains)} neurons")
        
        # Print agent statistics
        print(f"\n  Agent Statistics:")
        print(f"    Controller Agent: {self.controller_agent.getCuriosityLevel():.2f} curiosity")
        print(f"    Executive Agent: {self.executive_agent.getCuriosityLevel():.2f} curiosity")
        
        for i, agent in enumerate(self.lower_agents):
            print(f"    Lower Agent {i}: {agent.getCuriosityLevel():.2f} curiosity")

# Use hierarchical controller
hierarchical_controller = HierarchicalController()
hierarchical_controller.run_hierarchical_simulation(1000)
```

### Evolutionary Algorithms

```python
import pynlm
import numpy as np
import random

class EvolutionarySimulation:
    """Evolutionary simulation with genetic algorithms"""
    
    def __init__(self, population_size=10, generations=100):
        self.population_size = population_size
        self.generations = generations
        self.population = []
        self.generation = 0
        
        # Initialize population
        self._initialize_population()
        
    def _initialize_population(self):
        """Initialize population with random genomes"""
        
        for i in range(self.population_size):
            # Create random genome
            genome = {
                'brain_config': self._create_random_brain_config(),
                'agent_traits': self._create_random_agent_traits(),
                'world_config': self._create_random_world_config(),
                'fitness': 0.0,
                'age': 0
            }
            
            self.population.append(genome)
            
    def _create_random_brain_config(self):
        """Create random brain configuration"""
        
        config = pynlm.createDefaultConfig()
        
        # Randomize configuration parameters
        config.set("brain.neuron_count", random.randint(500, 2000))
        config.set("learning.rate", random.uniform(0.001, 0.05))
        config.set("brain.excitability", random.uniform(0.8, 1.5))
        config.set("memory.working_memory_size", random.randint(50, 500))
        config.set("memory.episodic_capacity", random.randint(100, 1000))
        
        # Randomly enable/disable features
        if random.random() < 0.7:
            config.set("learning.enable_stdp", True)
        if random.random() < 0.6:
            config.set("learning.enable_hebbian", True)
        if random.random() < 0.5:
            config.set("neuromodulation.enable_reward_modulation", True)
        if random.random() < 0.5:
            config.set("neuromodulation.enable_curiosity", True)
        if random.random() < 0.4:
            config.set("neuromodulation.enable_structural_plasticity", True)
            
        return config
        
    def _create_random_agent_traits(self):
        """Create random agent traits"""
        
        traits = {
            'curiosity_threshold': random.uniform(0.1, 0.5),
            'exploration_drive': random.uniform(0.2, 1.0),
            'risk_tolerance': random.uniform(0.1, 0.9),
            'learning_rate': random.uniform(0.01, 0.1),
            'memory_capacity': random.randint(50, 200),
            'attention_span': random.uniform(0.5, 2.0),
            'social tendency': random.uniform(0.0, 1.0)
        }
        
        return traits
        
    def _create_random_world_config(self):
        """Create random world configuration"""
        
        config = pynlm.createDefaultConfig()
        
        # Randomize world parameters
        config.set("world.width", random.randint(20, 50))
        config.set("world.height", random.randint(20, 50))
        config.set("world.vision_range", random.randint(5, 15))
        config.set("world.max_energy", random.uniform(50, 200))
        config.set("world.time_step", random.uniform(0.05, 0.2))
        
        return config
        
    def run_evolution(self):
        """Run evolutionary simulation"""
        
        print(f"Starting evolutionary simulation:")
        print(f"  Population size: {self.population_size}")
        print(f"  Generations: {self.generations}")
        print(f"  Total simulations: {self.population_size * self.generations}")
        
        for generation in range(self.generations):
            print(f"\nGeneration {generation + 1}/{self.generations}")
            
            # Evaluate fitness
            self._evaluate_fitness(generation)
            
            # Select parents
            parents = self._select_parents()
            
            # Create offspring
            offspring = self._create_offspring(parents)
            
            # Replace population
            self.population = offspring
            self.generation = generation + 1
            
            # Print statistics
            self._print_generation_stats()
            
    def _evaluate_fitness(self, generation):
        """Evaluate fitness of each genome"""
        
        # Run simulation for each genome
        for i, genome in enumerate(self.population):
            fitness = self._simulate_genome(genome, generation)
            genome['fitness'] = fitness
            genome['age'] += 1
            
        print(f"  Fitness evaluation complete")
        print(f"  Best fitness: {max(g['fitness'] for g in self.population):.4f}")
        
    def _simulate_genome(self, genome, generation):
        """Simulate genome and return fitness"""
        
        try:
            # Create brain with genome configuration
            brain = pynlm.createBrain(genome['brain_config'])
            brain.initialize()
            
            # Create world with genome configuration
            world = pynlm.createSimpleWorld()
            world.configure(
                width=genome['world_config'].getInt("world.width"),
                height=genome['world_config'].getInt("world.height"),
                visionWidth=genome['world_config'].getInt("world.vision_range"),
                visionHeight=genome['world_config'].getInt("world.vision_range"),
                maxEnergy=genome['world_config'].getFloat("world.max_energy"),
                timeStep=genome['world_config'].getFloat("world.time_step")
            )
            world.reset()
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Apply genome traits
            self._apply_genome_traits(agent, genome['agent_traits'])
            
            # Run simulation
            steps_completed = 0
            total_spikes = 0
            total_resources = 0
            max_energy_reached = 0
            
            for step in range(500):  # Fixed simulation length
                # Update world
                world.update(world.getSimulationTime() + 
                           genome['world_config'].getFloat("world.time_step"))
                
                # Get perception
                percept = world.getSensoryPercept()
                
                # Process perception
                agent.processSensoryInput(percept)
                
                # Brain computation
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Collect statistics
                steps_completed += 1
                total_spikes += brain.getFiringNeuronCount()
                
                # Simple fitness calculation
                energy = agent.getEnergy() if hasattr(agent, 'getEnergy') else 100.0
                max_energy_reached = max(max_energy_reached, energy)
                
            # Calculate fitness
            final_fitness = (
                0.3 * (steps_completed / 500) +                    # Completion
                0.2 * (max_energy_reached / 100) +               # Energy efficiency
                0.2 * (total_spikes / (steps_completed * 100)) + # Activity
                0.15 * (total_resources / 1000) +               # Resource collection
                0.15 * genome['agent_traits']['exploration_drive']  # Exploration
            )
            
            return max(0.0, min(1.0, final_fitness))
            
        except Exception as e:
            print(f"  Simulation failed: {e}")
            return 0.0
            
    def _apply_genome_traits(self, agent, traits):
        """Apply genome traits to agent"""
        
        # Apply traits based on genome
        if traits['exploration_drive'] > 0.7:
            agent.enableCuriosity(True)
            
        if traits['risk_tolerance'] > 0.7:
            agent.enableStructuralPlasticity(True)
            
        if traits['learning_rate'] > 0.05:
            agent.enableRewardModulation(True)
            
        if traits['social_tendency'] > 0.5:
            agent.enableDevelopment(True)
            
    def _select_parents(self):
        """Select parents for next generation"""
        
        # Sort by fitness (descending)
        self.population.sort(key=lambda g: g['fitness'], reverse=True)
        
        # Select top 20% as parents
        num_parents = max(2, int(self.population_size * 0.2))
        parents = self.population[:num_parents]
        
        return parents
        
    def _create_offspring(self, parents):
        """Create offspring through genetic operations"""
        
        offspring = []
        
        while len(offspring) < self.population_size:
            # Select two parents
            parent1, parent2 = random.sample(parents, 2)
            
            # Crossover
            child = self._crossover(parent1, parent2)
            
            # Mutate
            child = self._mutate(child)
            
            offspring.append(child)
            
        return offspring
        
    def _crossover(self, parent1, parent2):
        """Crossover between two parents"""
        
        # Create child genome
        child = {
            'brain_config': self._crossover_configs(parent1['brain_config'], 
                                                   parent2['brain_config']),
            'agent_traits': self._crossover_traits(parent1['agent_traits'],
                                                 parent2['agent_traits']),
            'world_config': self._crossover_configs(parent1['world_config'],
                                                  parent2['world_config']),
            'fitness': 0.0,
            'age': 0
        }
        
        return child
        
    def _crossover_configs(self, config1, config2):
        """Crossover between two configurations"""
        
        # Create new config
        new_config = pynlm.createDefaultConfig()
        
        # For each parameter, randomly choose from one parent
        params1 = config1.getKeys()
        params2 = config2.getKeys()
        all_params = list(set(params1) | set(params2))
        
        for param in all_params:
            if param in params1 and param in params2:
                # Choose randomly
                if random.random() < 0.5:
                    self._copy_config_value(new_config, param, config1)
                else:
                    self._copy_config_value(new_config, param, config2)
            elif param in params1:
                self._copy_config_value(new_config, param, config1)
            else:
                self._copy_config_value(new_config, param, config2)
                
        return new_config
        
    def _crossover_traits(self, traits1, traits2):
        """Crossover between two trait sets"""
        
        child_traits = {}
        
        # For each trait, randomly choose from one parent
        all_traits = set(traits1.keys()) | set(traits2.keys())
        
        for trait in all_traits:
            if trait in traits1 and trait in traits2:
                # Choose randomly
                if random.random() < 0.5:
                    child_traits[trait] = traits1[trait]
                else:
                    child_traits[trait] = traits2[trait]
            elif trait in traits1:
                child_traits[trait] = traits1[trait]
            else:
                child_traits[trait] = traits2[trait]
                
        return child_traits
        
    def _mutate(self, genome):
        """Apply mutations to genome"""
        
        # Deep copy genome
        mutated_genome = {
            'brain_config': self._mutate_config(genome['brain_config']),
            'agent_traits': self._mutate_traits(genome['agent_traits']),
            'world_config': self._mutate_config(genome['world_config']),
            'fitness': genome['fitness'],
            'age': genome['age']
        }
        
        return mutated_genome
        
    def _mutate_config(self, config):
        """Mutate configuration"""
        
        # Create mutated config
        mutated_config = pynlm.createDefaultConfig()
        
        # Copy existing values
        for key in config.getKeys():
            value = config.get(key)
            if isinstance(value, bool):
                mutated_config.set(key, value)
            elif isinstance(value, int):
                mutated_config.setInt(key, value)
            elif isinstance(value, float):
                mutated_config.setFloat(key, value)
            else:
                mutated_config.setString(key, value)
        
        # Apply mutations
        for key in mutated_config.getKeys():
            if random.random() < 0.1:  # 10% mutation rate
                if key.startswith("brain."):
                    if key.endswith("neuron_count"):
                        mutated_config.setInt(key, 
                                            max(100, mutated_config.getInt(key) + 
                                               random.randint(-100, 100)))
                    elif key.endswith("rate"):
                        mutated_config.setFloat(key,
                                              max(0.001, min(0.1,
                                                           mutated_config.getFloat(key) +
                                                           random.uniform(-0.01, 0.01))))
                    elif key.endswith("excitability"):
                        mutated_config.setFloat(key,
                                              max(0.5, min(2.0,
                                                           mutated_config.getFloat(key) +
                                                           random.uniform(-0.1, 0.1))))
                        
                elif key.startswith("learning."):
                    if random.random() < 0.3:  # Enable/disable
                        mutated_config.set(key, not mutated_config.getBool(key))
                        
                elif key.startswith("memory."):
                    if key.endswith("size") or key.endswith("capacity"):
                        mutated_config.setInt(key,
                                            max(10, mutated_config.getInt(key) +
                                               random.randint(-20, 20)))
                        
                elif key.startswith("neuromodulation."):
                    if random.random() < 0.3:  # Enable/disable
                        mutated_config.set(key, not mutated_config.getBool(key))
        
        return mutated_config
        
    def _mutate_traits(self, traits):
        """Mutate traits"""
        
        mutated_traits = traits.copy()
        
        # Apply mutations to each trait
        for trait in mutated_traits:
            if random.random() < 0.2:  # 20% mutation rate
                if trait.endswith("_threshold"):
                    mutated_traits[trait] = max(0.01, min(1.0, 
                                                        mutated_traits[trait] +
                                                        random.uniform(-0.1, 0.1)))
                elif trait.endswith("_drive") or trait.endswith("_rate"):
                    mutated_traits[trait] = max(0.01, min(1.0,
                                                        mutated_traits[trait] +
                                                        random.uniform(-0.1, 0.1)))
                elif trait.endswith("_capacity") or trait.endswith("_span"):
                    mutated_traits[trait] = max(10, min(500,
                                                        mutated_traits[trait] +
                                                        random.randint(-20, 20)))
                elif trait.endswith("_tendency"):
                    mutated_traits[trait] = max(0.0, min(1.0,
                                                        mutated_traits[trait] +
                                                        random.uniform(-0.1, 0.1)))
        
        return mutated_traits
        
    def _copy_config_value(self, target_config, key, source_config):
        """Copy configuration value"""
        
        if source_config.has(key):
            value = source_config.get(key)
            if isinstance(value, bool):
                target_config.set(key, value)
            elif isinstance(value, int):
                target_config.setInt(key, value)
            elif isinstance(value, float):
                target_config.setFloat(key, value)
            else:
                target_config.setString(key, value)
        
    def _print_generation_stats(self):
        """Print generation statistics"""
        
        # Calculate statistics
        avg_fitness = sum(g['fitness'] for g in self.population) / len(self.population)
        max_fitness = max(g['fitness'] for g in self.population)
        min_fitness = min(g['fitness'] for g in self.population)
        
        print(f"  Average fitness: {avg_fitness:.4f}")
        print(f"  Maximum fitness: {max_fitness:.4f}")
        print(f"  Minimum fitness: {min_fitness:.4f}")
        print(f"  Extinction risk: {(len([g for g in self.population if g['fitness'] < 0.1]) / len(self.population) * 100):.1f}%")

# Use evolutionary simulation
evolutionary_sim = EvolutionarySimulation(population_size=20, generations=50)
evolutionary_sim.run_evolution()
```

## Advanced Configuration

### Configuration Templates

```python
import pynlm

class ConfigurationTemplates:
    """Predefined configuration templates"""
    
    TEMPLATES = {
        "beginner": {
            "brain.neuron_count": 500,
            "learning.rate": 0.01,
            "brain.excitability": 1.0,
            "memory.working_memory_size": 100,
            "simulation.time_step": 0.1,
            "neuromodulation.enable_reward_modulation": True,
            "neuromodulation.enable_curiosity": False,
            "neuromodulation.enable_structural_plasticity": False,
        },
        "intermediate": {
            "brain.neuron_count": 1000,
            "learning.rate": 0.02,
            "brain.excitability": 1.2,
            "memory.working_memory_size": 200,
            "simulation.time_step": 0.05,
            "neuromodulation.enable_reward_modulation": True,
            "neuromodulation.enable_curiosity": True,
            "neuromodulation.enable_structural_plasticity": False,
        },
        "advanced": {
            "brain.neuron_count": 2000,
            "learning.rate": 0.03,
            "brain.excitability": 1.5,
            "memory.working_memory_size": 500,
            "simulation.time_step": 0.02,
            "neuromodulation.enable_reward_modulation": True,
            "neuromodulation.enable_curiosity": True,
            "neuromodulation.enable_structural_plasticity": True,
            "neuromodulation.enable_development": True,
            "memory.enable_replay": True,
        },
        "expert": {
            "brain.neuron_count": 5000,
            "learning.rate": 0.05,
            "brain.excitability": 2.0,
            "memory.working_memory_size": 1000,
            "simulation.time_step": 0.01,
            "neuromodulation.enable_reward_modulation": True,
            "neuromodulation.enable_curiosity": True,
            "neuromodulation.enable_structural_plasticity": True,
            "neuromodulation.enable_development": True,
            "memory.enable_replay": True,
            "simulation.use_simd": True,
            "simulation.use_event_driven": True,
            "simulation.use_parallel_processing": True,
        }
    }
    
    @staticmethod
    def get_template(template_name):
        """Get configuration template"""
        if template_name in ConfigurationTemplates.TEMPLATES:
            return ConfigurationTemplates.TEMPLATES[template_name]
        else:
            raise ValueError(f"Unknown template: {template_name}")
    
    @staticmethod
    def apply_template(brain, template_name):
        """Apply configuration template to brain"""
        template = ConfigurationTemplates.get_template(template_name)
        
        config = brain.getConfig()
        for key, value in template.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.setInt(key, value)
            elif isinstance(value, float):
                config.setFloat(key, value)
            else:
                config.setString(key, value)

# Use configuration templates
ConfigurationTemplates.apply_template(brain, "intermediate")
```

### Configuration Validation

```python
import pynlm

class ConfigurationValidator:
    """Validate configuration parameters"""
    
    VALIDATION_RULES = {
        "brain.neuron_count": {
            "type": "int",
            "min": 100,
            "max": 10000,
            "default": 1000
        },
        "learning.rate": {
            "type": "float",
            "min": 0.0001,
            "max": 0.1,
            "default": 0.01
        },
        "brain.excitability": {
            "type": "float",
            "min": 0.5,
            "max": 2.0,
            "default": 1.0
        },
        "memory.working_memory_size": {
            "type": "int",
            "min": 10,
            "max": 1000,
            "default": 100
        },
        "simulation.time_step": {
            "type": "float",
            "min": 0.001,
            "max": 1.0,
            "default": 0.1
        },
        "world.width": {
            "type": "int",
            "min": 10,
            "max": 200,
            "default": 50
        },
        "world.height": {
            "type": "int",
            "min": 10,
            "max": 200,
            "default": 50
        }
    }
    
    @staticmethod
    def validate_config(config):
        """Validate configuration"""
        
        errors = []
        warnings = []
        
        # Check all parameters
        for key, rules in ConfigurationValidator.VALIDATION_RULES.items():
            if config.has(key):
                value = config.get(key)
                
                # Check type
                if rules["type"] == "int" and not isinstance(value, int):
                    errors.append(f"Parameter {key}: Expected integer, got {type(value)}")
                elif rules["type"] == "float" and not isinstance(value, (int, float)):
                    errors.append(f"Parameter {key}: Expected float, got {type(value)}")
                
                # Check range
                if isinstance(value, (int, float)):
                    if value < rules["min"]:
                        warnings.append(f"Parameter {key}: Value {value} below minimum {rules['min']}")
                    elif value > rules["max"]:
                        errors.append(f"Parameter {key}: Value {value} exceeds maximum {rules['max']}")
                        
            else:
                # Use default
                if "default" in rules:
                    config.set(key, rules["default"])
                else:
                    warnings.append(f"Parameter {key}: Using default value")
                    
        # Check consistency
        neuron_count = config.getInt("brain.neuron_count")
        memory_size = config.getInt("memory.working_memory_size")
        
        if memory_size > neuron_count:
            warnings.append(f"Memory size ({memory_size}) exceeds neuron count ({neuron_count})")
            
        time_step = config.getFloat("simulation.time_step")
        if time_step < 0.01:
            warnings.append(f"Time step ({time_step}) is very small, may cause performance issues")
            
        return errors, warnings
        
    @staticmethod
    def suggest_improvements(config):
        """Suggest configuration improvements"""
        
        suggestions = []
        
        # Suggest based on neuron count
        neuron_count = config.getInt("brain.neuron_count")
        if neuron_count > 2000:
            suggestions.append("Consider reducing neuron count for better performance")
            
        # Suggest based on memory usage
        memory_size = config.getInt("memory.working_memory_size")
        if memory_size > 500:
            suggestions.append("Consider reducing working memory size for memory efficiency")
            
        # Suggest based on learning rate
        learning_rate = config.getFloat("learning.rate")
        if learning_rate < 0.001:
            suggestions.append("Learning rate is very low, may require more training")
        elif learning_rate > 0.05:
            suggestions.append("Learning rate is high, may cause instability")
            
        # Suggest based on excitability
        excitability = config.getFloat("brain.excitability")
        if excitability < 0.8:
            suggestions.append("Low excitability may result in reduced activity")
        elif excitability > 1.8:
            suggestions.append("High excitability may cause excessive spiking")
            
        # Check for enabled features
        enabled_features = []
        for key in config.getKeys():
            if key.startswith("learning.") or key.startswith("neuromodulation."):
                if config.getBool(key):
                    enabled_features.append(key.split('.')[-1])
                    
        if enabled_features:
            suggestions.append(f"Enabled features: {', '.join(enabled_features)}")
            
        return suggestions

# Use configuration validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Exceeds recommendation
config.set("learning.rate", 0.06)      # Exceeds maximum

errors, warnings = ConfigurationValidator.validate_config(config)
print("Validation errors:", errors)
print("Validation warnings:", warnings)

suggestions = ConfigurationValidator.suggest_improvements(config)
print("Suggestions:", suggestions)
```

### Configuration Management

```python
import pynlm
import json
import os

class ConfigurationManager:
    """Manage configuration files and settings"""
    
    def __init__(self, config_dir="./configs"):
        self.config_dir = config_dir
        os.makedirs(config_dir, exist_ok=True)
        
    def save_config(self, config, filename):
        """Save configuration to file"""
        
        # Create config dict
        config_dict = {}
        for key in config.getKeys():
            value = config.get(key)
            config_dict[key] = self._convert_value_for_json(value)
            
        # Save to file
        filepath = os.path.join(self.config_dir, f"{filename}.json")
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
            
        print(f"Configuration saved to {filepath}")
        
    def load_config(self, filename):
        """Load configuration from file"""
        
        filepath = os.path.join(self.config_dir, f"{filename}.json")
        
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Configuration file not found: {filepath}")
            
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
            
        # Create config
        config = pynlm.createDefaultConfig()
        
        # Load values
        for key, value in config_dict.items():
            self._apply_config_value(config, key, value)
            
        print(f"Configuration loaded from {filepath}")
        return config
        
    def _convert_value_for_json(self, value):
        """Convert value for JSON serialization"""
        
        if isinstance(value, (int, float, str, bool)):
            return value
        elif isinstance(value, list):
            return [self._convert_value_for_json(v) for v in value]
        elif isinstance(value, dict):
            return {k: self._convert_value_for_json(v) for k, v in value.items()}
        else:
            return str(value)
            
    def _apply_config_value(self, config, key, value):
        """Apply configuration value"""
        
        if isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, int):
            config.setInt(key, value)
        elif isinstance(value, float):
            config.setFloat(key, value)
        elif isinstance(value, str):
            config.setString(key, value)
        else:
            # Try to determine type
            if isinstance(value, dict):
                for sub_key, sub_value in value.items():
                    self._apply_config_value(config, f"{key}.{sub_key}", sub_value)
            elif isinstance(value, list):
                for i, item in enumerate(value):
                    self._apply_config_value(config, f"{key}[{i}]", item)
            else:
                config.set(key, value)
                
    def list_configs(self):
        """List available configurations"""
        
        configs = []
        for filename in os.listdir(self.config_dir):
            if filename.endswith('.json'):
                configs.append(filename[:-5])  # Remove .json extension
                
        return configs
        
    def export_config_template(self, config, template_name):
        """Export configuration as template"""
        
        template = {
            "name": template_name,
            "description": "Configuration template",
            "parameters": {}
        }
        
        for key in config.getKeys():
            value = config.get(key)
            template["parameters"][key] = {
                "value": value,
                "type": self._get_parameter_type(value)
            }
            
        filepath = os.path.join(self.config_dir, f"{template_name}_template.json")
        with open(filepath, 'w') as f:
            json.dump(template, f, indent=2)
            
        print(f"Configuration template saved to {filepath}")
        
    def _get_parameter_type(self, value):
        """Get parameter type"""
        
        if isinstance(value, bool):
            return "boolean"
        elif isinstance(value, int):
            return "integer"
        elif isinstance(value, float):
            return "float"
        elif isinstance(value, str):
            return "string"
        elif isinstance(value, list):
            return "array"
        elif isinstance(value, dict):
            return "object"
        else:
            return "unknown"

# Use configuration manager
config_manager = ConfigurationManager()

# Create a configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.02)

# Save configuration
config_manager.save_config(config, "my_config")

# Load configuration
loaded_config = config_manager.load_config("my_config")

# Export as template
config_manager.export_config_template(loaded_config, "my_template")

# List configurations
configs = config_manager.list_configs()
print("Available configurations:", configs)
```

## Troubleshooting Advanced Issues

### Performance Debugging

```python
import pynlm
import cProfile
import pstats
import io
import time

class AdvancedPerformanceDebugger:
    """Advanced performance debugging for NLM simulations"""
    
    def __init__(self):
        self.performance_data = []
        self.memory_samples = []
        self.step_times = []
        
    def profile_simulation(self, brain, steps, verbose=False):
        """Profile simulation with advanced metrics"""
        
        print(f"Starting advanced profiling for {steps} steps...")
        
        # Start detailed profiling
        profiler = cProfile.Profile()
        profiler.enable()
        
        # Initialize performance monitoring
        start_time = time.time()
        start_memory = self._get_memory_usage()
        
        # Run simulation with detailed monitoring
        for step in range(steps):
            step_start = time.time()
            
            # Record pre-step metrics
            pre_memory = self._get_memory_usage()
            
            # Run step
            brain.step(step)
            
            # Record post-step metrics
            step_time = time.time() - step_start
            post_memory = self._get_memory_usage()
            
            # Store performance data
            self.performance_data.append({
                'step': step,
                'time': step_time,
                'memory_before': pre_memory,
                'memory_after': post_memory,
                'neuron_count': brain.getTotalNeuronCount(),
                'firing_count': brain.getFiringNeuronCount(),
                'spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            })
            
            # Store timing and memory samples
            self.step_times.append(step_time)
            self.memory_samples.append(post_memory)
            
            # Print progress
            if verbose and step % 100 == 0:
                print(f"Step {step}: {step_time:.4f}s, "
                      f"Memory: {post_memory:.2f} MB")
        
        # Stop profiling
        profiler.disable()
        
        # Calculate final metrics
        total_time = time.time() - start_time
        end_memory = self._get_memory_usage()
        memory_growth = end_memory - start_memory
        
        # Generate performance report
        self._generate_performance_report(total_time, memory_growth, steps)
        
        # Print profile statistics
        self._print_profile_statistics(profiler)
        
        return self.performance_data
        
    def _get_memory_usage(self):
        """Get current memory usage in MB"""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024
        
    def _generate_performance_report(self, total_time, memory_growth, steps):
        """Generate performance report"""
        
        print(f"\nPerformance Report:")
        print(f"  Total simulation time: {total_time:.2f}s")
        print(f"  Average step time: {total_time/steps:.6f}s")
        print(f"  Memory growth: {memory_growth:.2f} MB")
        print(f"  Steps per second: {steps/total_time:.1f}")
        
        if self.step_times:
            avg_step_time = sum(self.step_times) / len(self.step_times)
            min_step_time = min(self.step_times)
            max_step_time = max(self.step_times)
            
            print(f"  Step time statistics:")
            print(f"    Average: {avg_step_time:.6f}s")
            print(f"    Minimum: {min_step_time:.6f}s")
            print(f"    Maximum: {max_step_time:.6f}s")
            print(f"    Standard deviation: {np.std(self.step_times):.6f}s")
            
        if self.memory_samples:
            avg_memory = sum(self.memory_samples) / len(self.memory_samples)
            max_memory = max(self.memory_samples)
            
            print(f"  Memory statistics:")
            print(f"    Average memory: {avg_memory:.2f} MB")
            print(f"    Peak memory: {max_memory:.2f} MB")
            
        # Detect performance issues
        self._detect_performance_issues()
        
    def _detect_performance_issues(self):
        """Detect performance issues"""
        
        issues = []
        
        # Check for slow steps
        slow_steps = [d for d in self.performance_data if d['time'] > 0.1]
        if len(slow_steps) / len(self.performance_data) > 0.1:
            issues.append(f"High percentage of slow steps: {len(slow_steps)/len(self.performance_data)*100:.1f}%")
            
        # Check for memory growth
        if len(self.memory_samples) > 10:
            recent_memory = self.memory_samples[-5:]
            earlier_memory = self.memory_samples[:-5]
            
            if sum(recent_memory) > sum(earlier_memory) * 1.1:
                issues.append("Significant memory growth detected")
                
        # Check for performance degradation
        if len(self.step_times) > 20:
            first_half = self.step_times[:len(self.step_times)//2]
            second_half = self.step_times[len(self.step_times)//2:]
            
            if sum(second_half) > sum(first_half) * 1.2:
                issues.append("Performance degradation detected")
                
        # Check for memory issues
        for data in self.performance_data:
            if data['memory_after'] - data['memory_before'] > 50:  # > 50MB growth
                issues.append(f"Large memory spike at step {data['step']}")
                break
                
        if issues:
            print(f"\nPerformance Issues Detected:")
            for issue in issues:
                print(f"  - {issue}")
        else:
            print(f"\nNo significant performance issues detected.")
            
    def _print_profile_statistics(self, profiler):
        """Print profile statistics"""
        
        # Get profile statistics
        stream = io.StringIO()
        stats = pstats.Stats(profiler, stream=stream)
        stats.strip_dirs()
        stats.sort_stats('cumulative')
        
        # Print top functions by cumulative time
        print(f"\nTop Functions by Cumulative Time:")
        print("-" * 60)
        print(f"{'Function':<40} {'Time':<10} {'Calls':<10}")
        print("-" * 60)
        
        for func in stats.stats:
            cc, nc, tt, ct, callers = stats.stats[func]
            if ct > 0.001:  # Only show functions taking > 1ms
                func_name = f"{func[2]}:{func[0]}"
                print(f"{func_name:<40} {ct:<10.6f} {nc:<10}")
                
        # Print profile to file
        with open("profile_results.txt", 'w') as f:
            f.write(stream.getvalue())
            
    def analyze_step_performance(self, step):
        """Analyze performance of specific step"""
        
        data = [d for d in self.performance_data if d['step'] == step]
        if not data:
            print(f"No performance data for step {step}")
            return
            
        data = data[0]
        
        print(f"\nPerformance Analysis - Step {step}:")
        print(f"  Step time: {data['time']:.6f}s")
        print(f"  Memory before: {data['memory_before']:.2f} MB")
        print(f"  Memory after: {data['memory_after']:.2f} MB")
        print(f"  Memory change: {data['memory_after'] - data['memory_before']:.2f} MB")
        print(f"  Neurons firing: {data['firing_count']}")
        print(f"  Total spikes: {data['spike_count']}")
        print(f"  Average firing rate: {data['average_firing_rate']:.4f}")
        
    def find_performance_bottlenecks(self):
        """Find performance bottlenecks"""
        
        print(f"\nPerformance Bottleneck Analysis:")
        
        # Analyze step times
        if len(self.step_times) > 10:
            # Find slowest steps
            sorted_steps = sorted(self.performance_data, 
                                key=lambda x: x['time'], reverse=True)
            
            print(f"  Top 5 slowest steps:")
            for i, data in enumerate(sorted_steps[:5]):
                print(f"    Step {data['step']}: {data['time']:.6f}s")
                
        # Analyze memory usage
        if len(self.memory_samples) > 10:
            sorted_memory = sorted(self.performance_data,
                                 key=lambda x: x['memory_after'] - x['memory_before'],
                                 reverse=True)
            
            print(f"  Top 5 memory spikes:")
            for i, data in enumerate(sorted_memory[:5]):
                memory_change = data['memory_after'] - data['memory_before']
                print(f"    Step {data['step']}: {memory_change:.2f} MB")
                
        # Analyze firing rate patterns
        if len(self.performance_data) > 10:
            avg_firing = sum(d['average_firing_rate'] for d in self.performance_data) / len(self.performance_data)
            low_firing = sum(1 for d in self.performance_data if d['average_firing_rate'] < avg_firing * 0.5)
            
            print(f"  Low firing rate steps: {low_firing}/{len(self.performance_data)} "
                  f"({low_firing/len(self.performance_data)*100:.1f}%)")

# Use advanced performance debugger
debugger = AdvancedPerformanceDebugger()

# Profile simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

performance_data = debugger.profile_simulation(brain, 1000, verbose=True)

# Analyze specific steps
debugger.analyze_step_performance(100)
debugger.analyze_step_performance(500)

# Find bottlenecks
debugger.find_performance_bottlenecks()
```

### Error Recovery

```python
import pynlm

class ErrorRecoverySystem:
    """Advanced error recovery system for NLM simulations"""
    
    def __init__(self):
        self.error_handlers = []
        self.recovery_points = {}
        self.error_log = []
        
    def add_error_handler(self, handler):
        """Add error handler"""
        self.error_handlers.append(handler)
        
    def add_recovery_point(self, name, state):
        """Add recovery point"""
        self.recovery_points[name] = state
        
    def recover_from_error(self, error, recovery_point_name=None):
        """Recover from error"""
        
        print(f"Error occurred: {error}")
        
        # Log error
        self.error_log.append({
            'error': str(error),
            'timestamp': time.time(),
            'recovery_point': recovery_point_name
        })
        
        # Try recovery points
        if recovery_point_name and recovery_point_name in self.recovery_points:
            print(f"Attempting recovery from {recovery_point_name}...")
            recovery_state = self.recovery_points[recovery_point_name]
            return self._restore_state(recovery_state)
            
        # Try error handlers
        for handler in self.error_handlers:
            try:
                result = handler(error)
                if result:
                    print(f"Error handled successfully by {handler.__name__}")
                    return True
            except Exception as handler_error:
                print(f"Error handler {handler.__name__} failed: {handler_error}")
                continue
                
        print(f"Failed to recover from error")
        return False
        
    def _restore_state(self, state):
        """Restore simulation state"""
        
        # Restore brain state
        if 'brain' in state:
            # Restore brain
            pass
            
        # Restore world state
        if 'world' in state:
            # Restore world
            pass
            
        # Restore agent state
        if 'agent' in state:
            # Restore agent
            pass
            
        return True
        
    def run_simulation_with_recovery(self, brain, world, agent, steps):
        """Run simulation with error recovery"""
        
        # Set up recovery points
        self.add_recovery_point('initialization', {
            'brain': brain,
            'world': world,
            'agent': agent
        })
        
        # Set up error handlers
        self.add_error_handler(self._handle_runtime_error)
        self.add_error_handler(self._handle_memory_error)
        self.add_error_handler(self._handle_simulation_error)
        
        for step in range(steps):
            try:
                # Update world
                world.update(0.1)
                
                # Get perception
                percept = world.getSensoryPercept()
                
                # Process perception
                agent.processSensoryInput(percept)
                
                # Brain computation
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Save recovery point
                if step % 500 == 0:
                    self.add_recovery_point(f'checkpoint_{step}', {
                        'brain': brain,
                        'world': world,
                        'agent': agent
                    })
                    
            except Exception as e:
                # Try to recover
                recovery_success = self.recover_from_error(e, f'checkpoint_{step - 100}')
                
                if not recovery_success:
                    print(f"Failed to recover from error at step {step}")
                    print(f"Error log: {self.error_log}")
                    raise
                    
    def _handle_runtime_error(self, error):
        """Handle runtime error"""
        
        if isinstance(error, pynlm.RuntimeError):
            print(f"Runtime error occurred: {error}")
            
            # Try to continue simulation
            return True
            
        return False
        
    def _handle_memory_error(self, error):
        """Handle memory error"""
        
        if "memory" in str(error).lower():
            print(f"Memory error occurred: {error}")
            
            # Try to free memory
            import gc
            gc.collect()
            
            # Try to continue with reduced memory
            return True
            
        return False
        
    def _handle_simulation_error(self, error):
        """Handle simulation error"""
        
        if "simulation" in str(error).lower():
            print(f"Simulation error occurred: {error}")
            
            # Try to continue with simplified simulation
            return True
            
        return False
        
    def get_error_summary(self):
        """Get error summary"""
        
        if not self.error_log:
            return "No errors occurred"
            
        error_counts = {}
        for log_entry in self.error_log:
            error_type = log_entry['error'].split(':')[0] if ':' in log_entry['error'] else log_entry['error']
            error_counts[error_type] = error_counts.get(error_type, 0) + 1
            
        summary = f"Total errors: {len(self.error_log)}\n"
        for error_type, count in error_counts.items():
            summary += f"  {error_type}: {count}\n"
            
        return summary

# Use error recovery system
recovery_system = ErrorRecoverySystem()

# Create simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with recovery
recovery_system.run_simulation_with_recovery(brain, world, agent, 1000)

# Get error summary
print(f"\nError Recovery Summary:")
print(recovery_system.get_error_summary())
```

## Conclusion

This advanced usage guide provides comprehensive coverage of NLM Python advanced features, including:

- **Event-driven simulation** for precise timing control
- **Custom development environments** for specialized simulations
- **Multi-agent coordination** for complex social simulations
- **Hierarchical control** for multi-level decision making
- **Evolutionary algorithms** for optimization and adaptation
- **Advanced configuration** for fine-tuned control
- **Configuration management** for reproducibility
- **Performance debugging** for optimization
- **Error recovery** for robust simulations

These advanced techniques enable developers to build sophisticated neural simulations that push the boundaries of computational neuroscience and artificial intelligence. Use these techniques to create cutting-edge applications that leverage the full power of the NLM Python API.

## See Also

- [Python API Reference](../python_api.md)
- [Performance Tuning Guide](../python_performance_tuning.md)
- [Troubleshooting Guide](../python_troubleshooting.md)
- [Architecture Documentation](../docs/ARCHITECTURE.md)
- [Phase 6 Final Audit](../docs/PHASE6_FINAL_AUDIT.md)
