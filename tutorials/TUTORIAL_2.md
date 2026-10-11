# Tutorial 2: Sensory-Motor Loop with Real World

This tutorial teaches you how to create a complete agent-world interaction system, where a brain perceives its environment, makes decisions, and acts upon it. This is the fundamental sensory-motor loop that makes NLM a truly embodied cognitive system.

## Learning Objectives

After completing this tutorial, you should be able to:

1. **Create and configure a simulation world** with environmental dynamics
2. **Connect a brain to a world** through an agent interface
3. **Implement the sensory-motor loop** in a complete simulation
4. **Observe perception-action cycles** in real-time
5. **Debug and optimize** agent behavior in dynamic environments
6. **Create simple behaviors** through action selection

## What You'll Learn

- World architecture: what makes up a simulation environment
- Agent interface: how brains communicate with worlds
- Sensory processing: converting environmental data to neural signals
- Motor control: translating neural activity into actions
- Behavior generation: creating simple intelligent behaviors
- Debugging techniques: identifying and fixing agent issues

## Prerequisites

- Understanding of Tutorial 1 (basic brain creation and simulation)
- Basic knowledge of Python programming
- Understanding of neural network concepts

## Step 1: Understanding the World-Agent-Brain Architecture

### The Complete Architecture

The NLM sensory-motor loop creates a closed system where:

```
ENVIRONMENT (World)
  ↓ (Sensory Input)
   AGENT (Interface)
  ↓ (Neural Processing)
   BRAIN (Cognition)
  ↓ (Motor Output)
   AGENT (Interface)
  ↓ (Action Execution)
ENVIRONMENT (World)
```

### World Components

A NLM world contains:

1. **Physical space**: 2D or 3D environment with coordinates
2. **Objects**: Resources, hazards, walls, markers
3. **Agent**: A mobile entity with perception capabilities
4. **Time**: Simulation clock for updating dynamics
5. **Rules**: Physics, collision detection, interaction rules

### Agent Components

The agent acts as a bridge between brain and world:

1. **Sensory Percept**: Processed environmental input
2. **Agent Body**: Physical representation of the agent
3. **Interface Methods**: Functions for input/output processing
4. **Neuromodulation**: Reward and motivation signals

### Brain Components

The brain processes agent sensory input:

1. **Sensory Input**: Raw environmental data
2. **Internal State**: Working memory and attention
3. **Motor Output**: Action selection
4. **Learning Systems**: Plasticity and development

## Step 2: Creating Your First World

### World Configuration

Let's create a simple 2D world for our agent to explore:

```python
import pynlm
import time

def create_simple_world():
    """Create a simple 2D world for agent exploration"""
    
    print("=== Creating Simple World ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    
    # Configure world parameters
    world.configure(
        width=20,          # World width (units)
        height=20,         # World height (units)
        visionWidth=8,     # Agent's visual field width
        visionHeight=8,    # Agent's visual field height
    )
    
    # Set world properties
    world.setRandomSeed(42)  # Reproducible behavior
    world.setMaxEnergy(100.0)  # Maximum agent energy
    
    print(f"   World dimensions: {world.getWidth()} x {world.getHeight()}")
    print(f"   Agent vision: {world.getVisionWidth()} x {world.getVisionHeight()}")
    print(f"   Random seed: {world.getRandomSeed()}")
    
    return world

def populate_world(world):
    """Add objects and features to the world"""
    
    print("Populating world with objects...")
    
    # Add resources (positive value for agent)
    world.addObject(pynlm.WorldObject(2.0, 2.0, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=15.0))
    world.addObject(pynlm.WorldObject(15.0, 3.0, pynlm.WorldObjectType.Resource, value=20.0))
    
    # Add hazards (negative value for agent)
    world.addObject(pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, value=-5.0))
    world.addObject(pynlm.WorldObject(12.0, 12.0, pynlm.WorldObjectType.Hazard, value=-8.0))
    
    # Add walls (impassable obstacles)
    for x in range(0, 20, 3):
        world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, value=0.0))
        world.addObject(pynlm.WorldObject(x, 19, pynlm.WorldObjectType.Wall, value=0.0))
    
    for y in range(0, 20, 3):
        world.addObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall, value=0.0))
        world.addObject(pynlm.WorldObject(19, y, pynlm.WorldObjectType.Wall, value=0.0))
    
    # Add markers (landmarks for navigation)
    world.addObject(pynlm.WorldObject(1.0, 1.0, pynlm.WorldObjectType.Marker, value=1.0))
    world.addObject(pynlm.WorldObject(18.0, 18.0, pynlm.WorldObjectType.Marker, value=1.0))
    
    print(f"   Added resources, hazards, walls, and markers")
    return world

def get_world_info(world):
    """Display current world information"""
    
    print(f"\nWorld Status:")
    print(f"   Simulation time: {world.getSimulationTime():.3f}")
    print(f"   Agent position: ({world.getAgentBody().getX():.1f}, {world.getAgentBody().getY():.1f})")
    print(f"   Agent energy: {world.getAgentBody().getEnergy():.1f}/{world.getMaxEnergy()}")
    print(f"   Agent orientation: {world.getAgentBody().getOrientation():.1f}°")
```

### Running the World Creation Code

```python
if __name__ == "__main__":
    # Create and populate world
    world = create_simple_world()
    world = populate_world(world)
    
    # Display world info
    get_world_info(world)
```

## Step 3: Creating the Agent Interface

### Agent Creation and Initialization

Now let's create an agent that will bridge the brain and world:

```python
import pynlm

def create_agent_brain():
    """Create and configure the agent interface"""
    
    print("=== Creating Agent Interface ===")
    
    # Create brain configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)  # Reasonable size for world interaction
    config.set("plasticity.stdp.enable", True)  # Enable learning
    config.set("plasticity.hebbian.enable", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   Agent interface connected")
    
    return brain, agent

def initialize_agent_with_world(agent, world):
    """Connect agent to world for interaction"""
    
    print("Initializing agent with world...")
    
    # Set agent starting position
    world.setAgentStart(10.0, 10.0)  # Center of world
    
    # Initialize agent with world reference
    agent.initialize(world)
    
    # Enable agent capabilities
    agent.enableRewardModulation(True)   # Learn from rewards
    agent.enableCuriosity(True)         # Explore new areas
    agent.enableDevelopment(True)       # Brain matures over time
    
    print("   ✓ Agent initialized with world")
    print("   ✓ Reward modulation enabled")
    print("   ✓ Curiosity module enabled")
    print("   ✓ Development system enabled")
    
    return agent

def monitor_agent_state(agent, world, step):
    """Monitor and display agent state"""
    
    if step % 50 == 0:
        body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        print(f"   Step {step:4d}: Pos({body.getX():5.1f},{body.getY():5.1f}) "
              f"Energy({body.getEnergy():4.1f}) "
              f"Curiosity({agent.getCuriosityLevel():5.3f}) "
              f"Novelty({agent.getNoveltyLevel():5.3f})")
```

## Step 4: Implementing the Complete Sensory-Motor Loop

### The Core Loop

The complete sensory-motor loop implementation:

```python
import pynlm
import time

def run_sensory_motor_loop(world, agent, brain, num_steps=300):
    """Implement the complete sensory-motor loop"""
    
    print(f"\n=== Running Sensory-Motor Loop ===")
    print(f"Simulation steps: {num_steps}")
    print(f"Loop structure: World → Agent → Brain → Agent → World → ...")
    print()
    print("Step | Action      | Perception      | Learning     | State")
    print("-----|-------------|-----------------|--------------|------")
    
    for step in range(num_steps):
        # 1. Update world environment
        world.update(0.1)  # Advance time by 0.1 seconds
        
        # 2. Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 4. Run brain computation
        brain.step(step)
        
        # 5. Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # 6. Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 7. Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # 9. Monitor state every 50 steps
        if step % 50 == 0:
            body = world.getAgentBody()
            print(f"{step:4d} | {str(motor_cmd):11} | "
                  f"FOV({percept.getFOVWidth():2d}x{percept.getFOVHeight():2d}) | "
                  f"Curiosity({agent.getCuriosityLevel():5.3f}) | "
                  f"Energy({body.getEnergy():4.1f})")
    
    print(f"\n=== Loop Complete ===")
    print(f"Total steps simulated: {num_steps}")
    print(f"Final agent position: ({world.getAgentBody().getX():.1f}, {world.getAgentBody().getY():.1f})")
    print(f"Final energy: {world.getAgentBody().getEnergy():.1f}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")

def demonstrate_perception_action_cycle(world, agent, brain, steps_per_cycle=100):
    """Demonstrate a single perception-action cycle in detail"""
    
    print(f"\n=== Detailed Perception-Action Cycle ===")
    print(f"Analyzing one complete cycle (first {steps_per_cycle} steps):\n")
    
    for step in range(steps_per_cycle):
        # World Update
        world.update(0.1)
        
        # Perception
        percept = world.getSensoryPercept()
        fov = percept.getFOV()
        
        # Brain Processing
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Action Selection
        action = agent.decodeMotorCommand()
        
        # Action Execution
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Display cycle details
        if step % 25 == 0:
            body = world.getAgentBody()
            print(f"Step {step:3d}: "
                  f"Percept({len(fov)} vision pixels) → "
                  f"Action({action}) → "
                  f"Pos({body.getX():5.1f},{body.getY():5.1f}) "
                  f"Energy({body.getEnergy():4.1f})")

def interactive_simulation_loop(world, agent, brain, interactive=True):
    """Run simulation with optional interactive control"""
    
    print(f"\n=== Interactive Simulation Loop ===")
    print(f"{'Step':>5} {'Action':>10} {'Energy':>8} {'FOV':>8} {'Learning':>9} {'Status':>10}")
    print(f"{'-----':>5} {'----------':>10} {'-------':>8} {'---':>8} {'--------':>9} {'----------':>10}")
    
    for step in range(200):
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute and learn
        world.applyMotorCommand(action, world.getSimulationTime())
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Display row
        body = world.getAgentBody()
        fov = percept.getFOVWidth() * percept.getFOVHeight()
        learning = "Yes" if step % 50 == 0 else "No"
        status = "OK" if body.getEnergy() > 0 else "DEAD"
        
        print(f"{step:5d} {str(action):>10} {body.getEnergy():8.1f} {fov:8d} {learning:>9} {status:>10}")
        
        if interactive and step % 50 == 0:
            input("Press Enter to continue...")
```

### Complete Simulation Example

```python
import pynlm
import time

def run_complete_sensory_motor_demo():
    """Complete demonstration of sensory-motor loop"""
    
    print("=== NLM Sensory-Motor Loop Tutorial ===")
    print("This tutorial demonstrates how brains interact with environments through perception and action.\n")
    
    # 1. Create world
    print("1. Creating simulation world...")
    world = create_simple_world()
    world = populate_world(world)
    
    # 2. Create agent
    print("\n2. Creating agent interface...")
    brain, agent = create_agent_brain()
    agent = initialize_agent_with_world(agent, world)
    
    # 3. Show initial state
    print("\n3. Initial agent state:")
    monitor_agent_state(agent, world, 0)
    
    # 4. Run main simulation
    print("\n4. Running sensory-motor loop...")
    run_sensory_motor_loop(world, agent, brain, 200)
    
    # 5. Demonstrate detailed cycle
    print("\n5. Detailed analysis of perception-action cycle...")
    demonstrate_perception_action_cycle(world, agent, brain, 100)
    
    # 6. Run interactive loop
    print("\n6. Interactive simulation with detailed monitoring...")
    interactive_simulation_loop(world, agent, brain, interactive=False)
    
    # 7. Save final state
    print("\n7. Saving final brain state...")
    brain.save("sensory_motor_brain.bin")
    print("   ✓ Brain state saved")
    
    print("\n=== Tutorial Complete ===")
    print("Your agent has successfully explored the world through perception and action!")
    print("The sensory-motor loop demonstrates how brains interact with their environment.")

if __name__ == "__main__":
    run_complete_sensory_motor_demo()
```

## Step 5: Advanced Sensory Processing

### Visual Processing Pipeline

Let's explore how sensory input is processed:

```python
import pynlm

def analyze_sensory_processing(world, agent, brain, steps=100):
    """Analyze how sensory information is processed"""
    
    print(f"\n=== Sensory Processing Analysis ===")
    print(f"Analyzing sensory processing over {steps} steps:\n")
    
    sensory_data = []
    action_patterns = []
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process in agent and brain
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Collect data
        sensory_data.append({
            'step': step,
            'fov_width': percept.getFOVWidth(),
            'fov_height': percept.getFOVHeight(),
            'internal_signal': percept.getInternal(),
            'visible_objects': percept.getVisibleObjects(),
        })
        
        action_patterns.append({
            'step': step,
            'action': action,
            'energy_change': world.getAgentBody().getEnergy() - (world.getAgentBody().getEnergy() if step > 0 else 0),
        })
        
        if step % 50 == 0:
            print(f"Step {step:3d}: FOV={percept.getFOVWidth()}x{percept.getFOVHeight()} "
                  f"Objects={len(percept.getVisibleObjects())} "
                  f"Action={action}")
    
    # Analyze patterns
    print(f"\nSensory Processing Analysis:")
    print(f"   Average FOV size: {sum(d['fov_width']*d['fov_height'] for d in sensory_data)/len(sensory_data):.1f} pixels")
    print(f"   Max visible objects: {max(d['visible_objects'] for d in sensory_data)}")
    print(f"   Action variety: {len(set(a['action'] for a in action_patterns))} different actions")
    
    # Identify environmental features
    interesting_positions = [d for d in sensory_data if d['visible_objects'] > 3]
    print(f"   Interesting scenes (many objects): {len(interesting_positions)}/{len(sensory_data)} steps")
    
    return sensory_data, action_patterns

def demonstrate_perception_learning(world, agent, brain, steps=150):
    """Show how perception affects learning"""
    
    print(f"\n=== Perception-Learning Integration ===")
    print(f"Demonstrating how sensory experience shapes learning:\n")
    
    for step in range(steps):
        world.update(0.1)
        
        # Get fresh percept each step
        percept = world.getSensoryPercept()
        
        # Process and learn
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply reward based on what's seen
        reward = 0.0
        visible_objects = percept.getVisibleObjects()
        
        # Reward for seeing resources
        for obj in visible_objects:
            if obj.getType() == pynlm.WorldObjectType.Resource:
                reward += obj.getValue() * 0.1
        
        # Negative reward for hazards
        for obj in visible_objects:
            if obj.getType() == pynlm.WorldObjectType.Hazard:
                reward -= abs(obj.getValue()) * 0.1
        
        # Learn from this experience
        agent.applyRewardModulation(reward, 0.0)
        
        # Show how perception affects behavior
        if step % 30 == 0:
            body = world.getAgentBody()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            
            print(f"Step {step:3d}: "
                  f"Seen={len(visible_objects)} objects "
                  f"Curiosity={curiosity:5.3f} "
                  f"Novelty={novelty:5.3f} "
                  f"Energy={body.getEnergy():5.1f} "
                  f"Reward={reward:+5.2f}")
```

## Step 6: Behavior Generation and Control

### Simple Behavioral Strategies

Let's implement simple behavioral rules:

```python
import pynlm

def create_behavioral_strategies():
    """Create different behavioral strategies for the agent"""
    
    print("=== Behavioral Strategy Examples ===\n")
    
    strategies = {
        'exploration': 'Move randomly to discover new areas',
        'exploitation': 'Stay in known profitable areas',
        'avoidance': 'Avoid hazardous areas at all costs',
        'curiosity': 'Move toward novel/interesting stimuli',
        'balance': 'Mix of exploration and exploitation',
    }
    
    for strategy, description in strategies.items():
        print(f"{strategy.upper():12}: {description}")
    
    return strategies

def simulate_behavioral_strategy(world, agent, brain, strategy='balance', steps=200):
    """Simulate different behavioral strategies"""
    
    print(f"\n=== Simulating '{strategy}' Strategy ===")
    
    # Reset for clean simulation
    world.reset()
    brain.reset()
    brain.initialize()
    agent.initialize(world)
    
    strategy_stats = {
        'movements': 0,
        'interactions': 0,
        'energy_changes': [],
        'novelty_encounters': 0,
    }
    
    for step in range(steps):
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Strategy-specific behavior
        if strategy == 'exploration':
            # Random movement
            action = random_movement(world)
        elif strategy == 'exploitation':
            # Move toward known good areas
            action = move_to_resource(world)
        elif strategy == 'avoidance':
            # Avoid hazards
            action = avoid_hazards(world)
        elif strategy == 'curiosity':
            # Move toward novel stimuli
            action = move_to_novelty(world, agent)
        else:  # balance
            # Mix of exploration and exploitation
            action = balanced_behavior(world, agent)
        
        # Execute action
        agent.processSensoryInput(percept)
        brain.step(step)
        agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track statistics
        strategy_stats['movements'] += 1
        body = world.getAgentBody()
        strategy_stats['energy_changes'].append(body.getEnergy())
        
        if agent.getNoveltyLevel() > 0.5:
            strategy_stats['novelty_encounters'] += 1
        
        # Print progress
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            print(f"Step {step:3d}: {action} | Energy: {body.getEnergy():5.1f} | "
                  f"Curiosity: {curiosity:5.3f} | Novelty: {agent.getNoveltyLevel():5.3f}")
    
    # Analyze strategy performance
    print(f"\nStrategy Analysis:")
    avg_energy = sum(strategy_stats['energy_changes']) / len(strategy_stats['energy_changes'])
    energy_trend = "Increasing" if strategy_stats['energy_changes'][-1] > strategy_stats['energy_changes'][0] else "Decreasing"
    
    print(f"   Average energy: {avg_energy:6.1f}")
    print(f"   Energy trend: {energy_trend}")
    print(f"   Novelty encounters: {strategy_stats['novelty_encounters']}")
    
    return strategy_stats

def random_movement(world):
    """Generate random movement (exploration strategy)"""
    
    actions = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Interact,
    ]
    
    return random.choice(actions)

def move_to_resource(world):
    """Move toward visible resources (exploitation strategy)"""
    
    percept = world.getSensoryPercept()
    visible_objects = percept.getVisibleObjects()
    
    # Find closest resource
    closest_resource = None
    min_distance = float('inf')
    
    for obj in visible_objects:
        if obj.getType() == pynlm.WorldObjectType.Resource:
            body = world.getAgentBody()
            distance = ((obj.getX() - body.getX())**2 + (obj.getY() - body.getY())**2)**0.5
            if distance < min_distance:
                min_distance = distance
                closest_resource = obj
    
    if closest_resource:
        # Simple navigation logic
        body = world.getAgentBody()
        if closest_resource.getX() > body.getX():
            return pynlm.ActionType.MoveForward
        else:
            return pynlm.ActionType.TurnRight
    
    return pynlm.ActionType.MoveForward  # Default action

def avoid_hazards(world):
    """Avoid visible hazards (avoidance strategy)"""
    
    percept = world.getSensoryPercept()
    visible_objects = percept.getVisibleObjects()
    
    # Check for nearby hazards
    for obj in visible_objects:
        if obj.getType() == pynlm.WorldObjectType.Hazard:
            body = world.getAgentBody()
            distance = ((obj.getX() - body.getX())**2 + (obj.getY() - body.getY())**2)**0.5
            
            if distance < 3.0:  # Hazard is close
                # Try to move away
                if body.getX() > obj.getX():
                    return pynlm.ActionType.TurnLeft
                else:
                    return pynlm.ActionType.TurnRight
    
    return pynlm.ActionType.MoveForward  # Continue moving if no hazard nearby

def move_to_novelty(world, agent):
    """Move toward novel stimuli (curiosity strategy)"""
    
    novelty = agent.getNoveltyLevel()
    
    if novelty > 0.7:
        # High novelty - explore more
        return pynlm.ActionType.MoveForward
    elif novelty > 0.3:
        # Medium novelty - balanced
        return pynlm.ActionType.Interact
    else:
        # Low novelty - return to familiar areas
        body = world.getAgentBody()
        # Simple return to starting area
        if body.getX() > 10:
            return pynlm.ActionType.TurnLeft
        else:
            return pynlm.ActionType.MoveForward

def balanced_behavior(world, agent):
    """Balance exploration and exploitation"""
    
    curiosity = agent.getCuriosityLevel()
    
    if curiosity > 0.6:
        # High curiosity - explore
        return pynlm.ActionType.MoveForward
    elif curiosity < 0.2:
        # Low curiosity - return to safe area
        body = world.getAgentBody()
        if body.getEnergy() < 50:  # Low energy, need resources
            return pynlm.ActionType.Interact
        else:
            return pynlm.ActionType.MoveForward
    else:
        # Balanced - normal behavior
        return pynlm.ActionType.MoveForward
```

## Step 7: Complete Tutorial Example

```python
import pynlm
import time
import random

def run_complete_tutorial_2():
    """Complete tutorial 2: Sensory-Motor Loop"""
    
    print("=== NLM Tutorial 2: Sensory-Motor Loop ===")
    print("This tutorial demonstrates complete agent-world interaction.\n")
    
    # 1. Setup world and agent
    print("1. Creating world and agent...")
    world = create_simple_world()
    world = populate_world(world)
    
    brain, agent = create_agent_brain()
    agent = initialize_agent_with_world(agent, world)
    
    # 2. Run basic sensory-motor loop
    print("\n2. Running basic sensory-motor loop...")
    run_sensory_motor_loop(world, agent, brain, 150)
    
    # 3. Analyze sensory processing
    print("\n3. Analyzing sensory processing...")
    sensory_data, action_patterns = analyze_sensory_processing(world, agent, brain, 100)
    
    # 4. Demonstrate perception-learning integration
    print("\n4. Demonstrating perception-learning integration...")
    demonstrate_perception_learning(world, agent, brain, 80)
    
    # 5. Show behavioral strategies
    print("\n5. Demonstrating behavioral strategies...")
    strategies = create_behavioral_strategies()
    
    for strategy in ['exploration', 'avoidance', 'curiosity']:
        print(f"\n   Simulating '{strategy}' strategy:")
        simulate_behavioral_strategy(world, agent, brain, strategy, 100)
    
    # 6. Save final state
    print("\n6. Saving final brain state...")
    brain.save("tutorial2_brain.bin")
    print("   ✓ Tutorial brain saved")
    
    print("\n=== Tutorial 2 Complete ===")
    print("Your agent has successfully demonstrated:")
    print("✓ Sensory input processing")
    print("✓ Motor output generation")
    print("✓ Perception-action loops")
    print("✓ Environmental interaction")
    print("✓ Learning from experience")
    print("✓ Behavioral adaptation")

if __name__ == "__main__":
    run_complete_tutorial_2()
```

## Step 8: Advanced Topics and Extensions

### Multi-Agent Systems

```python
import pynlm

def create_multi_agent_system():
    """Create a system with multiple interacting agents"""
    
    print("=== Multi-Agent System Example ===")
    print("Creating a system with multiple agents sharing the same world:\n")
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.setRandomSeed(123)
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(3):
        # Each agent gets its own brain but shares the world
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 80)
        config.set(f"agent.id", i)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agents.append(agent)
        brains.append(brain)
        
        print(f"   Agent {i}: {brain.getTotalNeuronCount()} neurons")
    
    print(f"\n   All agents share the same world")
    print(f"   Each agent has independent sensory-motor loops")
    print(f"   Agents can interact through world resources and hazards")
    
    return world, agents, brains

def run_multi_agent_simulation(world, agents, brains, steps=100):
    """Run a multi-agent simulation"""
    
    print(f"\nRunning multi-agent simulation for {steps} steps...")
    
    for step in range(steps):
        world.update(0.1)
        
        # Each agent perceives and acts independently
        for i, (agent, brain) in enumerate(zip(agents, brains)):
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Action selection
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Display agent positions
        if step % 25 == 0:
            print(f"Step {step:3d}: ", end="")
            for i, agent in enumerate(agents):
                body = world.getAgentBody()
                print(f"Agent{i}({body.getX():4.1f},{body.getY():4.1f}) ", end="")
            print()

# Example usage
if __name__ == "__main__":
    world, agents, brains = create_multi_agent_system()
    run_multi_agent_simulation(world, agents, brains, 75)
```

### Real-Time Visualization

```python
import pynlm
import matplotlib.pyplot as plt

def create_realtime_visualization(world, agent, brain, duration_steps=200):
    """Create real-time visualization of agent behavior"""
    
    print("=== Real-Time Visualization ===")
    print("Creating visualization of agent exploration:\n")
    
    # Set up visualization
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('NLM Agent Exploration Visualization', fontsize=16)
    
    # World map
    ax1 = axes[0, 0]
    world_map = world.getWorldMap()
    
    # Agent trajectory
    ax2 = axes[0, 1]
    trajectory = []
    
    # Learning metrics
    ax3 = axes[1, 0]
    curiosity_over_time = []
    novelty_over_time = []
    
    # Neural activity
    ax4 = axes[1, 1]
    firing_activity = []
    
    for step in range(duration_steps):
        world.update(0.1)
        
        # Get current state
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect data
        body = world.getAgentBody()
        trajectory.append((body.getX(), body.getY()))
        curiosity_over_time.append(agent.getCuriosityLevel())
        novelty_over_time.append(agent.getNoveltyLevel())
        firing_activity.append(brain.getFiringNeuronCount())
        
        # Update visualizations (simplified)
        if step % 20 == 0:
            ax1.clear()
            # Draw world elements
            ax1.plot([p[0] for p in trajectory], [p[1] for p in trajectory], 'b-', alpha=0.5, linewidth=1)
            ax1.scatter(body.getX(), body.getY(), c='red', s=100, marker='o')
            ax1.set_xlim(0, world.getWidth())
            ax1.set_ylim(0, world.getHeight())
            ax1.set_title('Agent Trajectory')
            ax1.set_aspect('equal')
            
            # Plot learning metrics
            ax3.clear()
            ax3.plot(range(len(curiosity_over_time)), curiosity_over_time, 'g-', label='Curiosity')
            ax3.plot(range(len(novelty_over_time)), novelty_over_time, 'b-', label='Novelty')
            ax3.set_xlabel('Time Step')
            ax3.set_ylabel('Level')
            ax3.set_title('Learning Metrics')
            ax3.legend()
            ax3.grid(True, alpha=0.3)
            
            # Plot neural activity
            ax4.clear()
            ax4.plot(range(len(firing_activity)), firing_activity, 'r-')
            ax4.set_xlabel('Time Step')
            ax4.set_ylabel('Firing Neurons')
            ax4.set_title('Neural Activity')
            ax4.grid(True, alpha=0.3)
            
            plt.pause(0.1)
    
    plt.savefig('agent_exploration.png', dpi=150, bbox_inches='tight')
    plt.show()
    
    print("Visualization saved to 'agent_exploration.png'")

# Usage
if __name__ == "__main__":
    # Setup world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    create_realtime_visualization(world, agent, brain, 150)
```

## Tutorial Summary

### What You've Accomplished

1. ✅ **Created a complete simulation world** with environmental objects and dynamics
2. ✅ **Set up an agent interface** that bridges brain and world
3. ✅ **Implemented the sensory-motor loop** in a complete simulation
4. ✅ **Observed perception-action cycles** in real-time
5. ✅ **Created and tested different behavioral strategies** (exploration, exploitation, avoidance, curiosity)
6. ✅ **Extended your agent** with multi-agent systems and visualization

### Key Concepts Learned

- **Embodied cognition**: Intelligence emerges from interaction with environment
- **Perception-action cycle**: Continuous loop of sensing, thinking, and acting
- **Situated behavior**: Actions depend on environmental context
- **Adaptive intelligence**: Behaviors change based on experience and learning
- **Multi-agent systems**: Multiple agents can coexist and interact

### Applications

This tutorial provides the foundation for:

- **Robotics**: Creating autonomous agents that explore and learn
- **Virtual environments**: Simulating cognitive systems in virtual worlds
- **Behavior design**: Engineering intelligent behaviors for various tasks
- **Multi-agent research**: Studying interaction between multiple agents

### Next Steps

Now that you understand the sensory-motor loop, you can:

1. **Complete Tutorial 3**: Explore advanced plasticity and learning mechanisms
2. **Explore Tutorial 4**: Study memory systems and episodic storage
3. **Study Tutorial 5**: Understand neuromodulation and reward-based learning
4. **Study Tutorial 6**: Examine developmental processes and aging

### Further Resources

- [Tutorial 3: Advanced Plasticity](./tutorials/TUTORIAL_3.md)
- [Tutorial 4: Memory Systems](./tutorials/TUTORIAL_4.md)
- [Tutorial 5: Neuromodulation](./tutorials/TUTORIAL_5.md)
- [Tutorial 6: Development and Aging](./tutorials/TUTORIAL_6.md)

This tutorial demonstrates the complete sensory-motor loop that makes NLM a truly embodied cognitive system. Your agent can now perceive its environment, make decisions, and act upon it - the fundamental processes of intelligent behavior.
