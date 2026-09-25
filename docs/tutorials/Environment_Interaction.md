# NLM Tutorial 3: Environment Interaction

## Overview

Welcome to Tutorial 3! This tutorial focuses on the interaction between NLM agents and their environment. You'll explore:

1. **Sensory Systems** - How agents perceive their environment
2. **Motor Systems** - How agents act upon their perceptions
3. **World Dynamics** - How environments change over time
4. **Perception-Action Loops** - The complete interaction cycle
5. **Sensory Processing** - Neural interpretation of environmental input
6. **Motor Decoding** - Translation of neural activity into actions

This tutorial builds on Tutorial 2's learning mechanisms, showing how they apply in real-world interaction scenarios.

---

## Prerequisites

- Tutorial 1 or 2 completed (basic NLM understanding)
- Familiarity with Python programming
- Understanding of neural computation basics

## Part 1: Understanding Environment-Interaction Systems

### Why Environment Interaction Matters

NLM's biological inspiration comes from the interaction between organisms and their environment. This tutorial shows how NLM models this fundamental process:

| Traditional AI | NLM Environment-Interaction |
|---------------|------------------------------|
| Static input processing | Dynamic perception-action loops |
| Pre-programmed responses | Emergent behavior from neural dynamics |
| Simple sensory input | Multi-modal sensory integration |
| Fixed motor outputs | Adaptive action selection |
| Isolated processing | Closed-loop feedback systems |

### Core Interaction Components

#### 1. Sensory Systems
```cpp
// Sensory Input Processing:
// 1. Environmental state → Raw sensory data
// 2. Neural transduction → Encoded signals  
// 3. Pattern extraction → Feature representation
// 4. Integration → Multi-modal perception
```

#### 2. Motor Systems
```cpp
// Motor Output Generation:
// 1. Neural activity → Action potential patterns
// 2. Decoding → Motor command parameters
// 3. Execution → Physical/environmental changes
// 4. Feedback → Updated sensory input
```

#### 3. Closed-Loop Control
```cpp
// The NLM Interaction Loop:
// WORLD STATE → [Sensory Input] → BRAIN → [Motor Output] → WORLD STATE
//                                ↑                           ↓
//                           Learning ← Feedback/Results
```

---

## Part 2: World System Architecture

### Step 1: Understanding World Components

```python
print("=== World System Architecture ===")

# World Components:
# 1. Environment state (positions, values, rules)
# 2. Agent position and orientation  
# 3. Objects and entities (resources, hazards, etc.)
# 4. Time and simulation clock
# 5. Physics and interaction rules
# 6. Reward and feedback systems

# SimpleWorld Structure:
class SimpleWorld:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.grid = self.initialize_grid()
        self.agent_position = (width//2, height//2)
        self.simulation_time = 0.0
        self.agent_energy = 100.0
        self.objects = []
        self.hazards = []
        
    def update(self, dt):
        # Time progression
        self.simulation_time += dt
        self.update_objects(dt)
        
    def getSensoryPercept(self):
        # Convert world state to sensory input
        return self.create_percept()
        
    def applyMotorCommand(self, action, time):
        # Execute action in world
        self.execute_action(action, time)
```

### Step 2: World Configuration

```python
print("\n=== World Configuration ===")

# Create different world types for various scenarios

# 1. Simple 2D World (for beginners)
print("1. Simple 2D World")
simple_world = pynlm.createSimpleWorld()
simple_world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# 2. Complex World with Dynamics
print("2. Complex World")
complex_world = pynlm.createComplexWorld()
complex_world.configure(
    width=30,
    height=30,
    time_step=0.1,
    physics_enabled=True,
    agent_body=True,
    nutrition_system=True
)

# 3. Multi-Agent World
print("3. Multi-Agent World")
multi_agent_world = pynlm.createMultiAgentWorld()
multi_agent_world.configure(
    grid_size=15,
    num_agents=5,
    communication_range=3,
    shared_resources=True
)

print(f"✓ Simple world: {simple_world.getWidth()}x{simple_world.getHeight()} cells")
print(f"✓ Complex world: Physics {'enabled' if complex_world.isPhysicsEnabled() else 'disabled'}")
print(f"✓ Multi-agent world: {multi_agent_world.getAgentCount()} agents")
```

### Step 3: World Object Types

```python
print("\n=== World Object Types ===")

# Standard NLM world objects
object_types = [
    (pynlm.WorldObjectType.Empty, "Empty space", "Agent can move freely"),
    (pynlm.WorldObjectType.Resource, "Resource", "Provides energy/nutrition"),
    (pynlm.WorldObjectType.Hazard, "Hazard", "Damages agent"),
    (pynlm.WorldObjectType.Wall, "Wall", "Physical barrier"),
    (pynlm.WorldObjectType.Marker, "Marker", "Target or goal"),
    (pynlm.WorldObjectType.Agent, "Agent", "Another agent"),
    (pynlm.WorldObjectType.Predator, "Predator", "Threat to agent"),
    (pynlm.WorldObjectType.Prey, "Prey", "Resource or threat"),
]

for obj_type, description, purpose in object_types:
    print(f"  {obj_type}: {description}")
    print(f"    Purpose: {purpose}")
```

---

## Part 3: Sensory Processing

### Step 4: Sensory Input Types

```python
print("\n=== Sensory Processing Types ===")

# NLM supports multiple sensory modalities

sensory_modalities = [
    ("Vision", "Visual input", "Spatial and color information"),
    ("Touch", "Tactile input", "Contact and pressure information"),
    ("Proprioception", "Body position", "Joint and muscle state"),
    ("Internal", "Internal state", "Energy, hunger, fatigue"),
    ("Audition", "Sound input", "Acoustic patterns"),
    ("Olfaction", "Smell input", "Chemical concentrations"),
    ("Social", "Social input", "Other agent signals"),
]

for modality, input_type, information in sensory_modalities:
    print(f"  {modality}:")
    print(f"    Type: {input_type}")
    print(f"    Information: {information}")
```

### Step 5: Sensory Perception Pipeline

```python
def create_perception_pipeline(world, agent):
    """Create complete sensory perception pipeline"""
    
    print(f"\n=== Sensory Perception Pipeline ===")
    
    # 1. Raw World Data Collection
    print("1. Raw World Data Collection:")
    
    # Get environmental state
    world_state = collect_world_state(world)
    agent_state = collect_agent_state(world, agent)
    
    print(f"   Environmental data: {len(world_state)} elements")
    print(f"   Agent data: {len(agent_state)} elements")
    
    # 2. Modality-Specific Processing
    print("\n2. Modality-Specific Processing:")
    
    vision_data = process_vision_data(world, agent)
    proprioception_data = process_proprioception_data(agent)
    internal_data = process_internal_data(world, agent)
    
    print(f"   Vision processed: {len(vision_data)} features")
    print(f"   Proprioception processed: {len(proprioception_data)} signals")
    print(f"   Internal state processed: {len(internal_data)} variables")
    
    # 3. Data Integration
    print("\n3. Data Integration:")
    
    integrated_percept = integrate_sensory_data(
        vision_data, proprioception_data, internal_data
    )
    
    print(f"   Integrated percept length: {len(integrated_percept)}")
    print(f"   Modality weights: Vision {calculate_modality_weight('vision'):.2f}, "
          f"Proprioception {calculate_modality_weight('proprioception'):.2f}, "
          f"Internal {calculate_modality_weight('internal'):.2f}")
    
    # 4. Feature Extraction
    print("\n4. Feature Extraction:")
    
    features = extract_features(integrated_percept)
    
    print(f"   Extracted features: {len(features)}")
    print(f"   Feature categories: {identify_feature_categories(features)}")
    
    # 5. Pattern Recognition
    print("\n5. Pattern Recognition:")
    
    recognized_patterns = recognize_patterns(features)
    confidence_scores = calculate_pattern_confidence(recognized_patterns)
    
    print(f"   Recognized patterns: {len(recognized_patterns)}")
    print(f"   Average confidence: {np.mean(confidence_scores):.2f}")
    
    return integrated_percept
```

### Step 6: Vision System Deep Dive

```python
def demonstrate_vision_system():
    """Demonstrate the vision system in detail"""
    
    print(f"\n=== Vision System Deep Dive ===")
    
    # Create world with visual elements
    vision_world = pynlm.createSimpleWorld()
    vision_world.configure(
        width=25,
        height=25,
        visionWidth=10,
        visionHeight=10
    )
    
    # Place various visual objects
    vision_world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=50.0))
    vision_world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Hazard, value=25.0))
    vision_world.addObject(pynlm.WorldObject(20, 10, pynlm.WorldObjectType.Wall, value=0.0))
    vision_world.addObject(pynlm.WorldObject(10, 20, pynlm.WorldObjectType.Marker, value=100.0))
    
    # Position agent at center
    agent_x, agent_y = vision_world.getWidth() // 2, vision_world.getHeight() // 2
    vision_world.setAgentStart(agent_x, agent_y)
    
    # Create agent and get vision data
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(vision_world)
    
    print("Vision System Components:")
    print("1. Field of View (FOV) Setup:")
    
    fov_width = vision_world.getVisionWidth()
    fov_height = vision_world.getVisionHeight()
    print(f"   FOV: {fov_width}x{fov_height} cells")
    
    print("\n2. Spatial Sampling:")
    print("   Centered on agent position")
    print("   Resolution: Cell-by-cell mapping")
    print("   Range: Within FOV boundaries")
    
    print("\n3. Object Detection:")
    print("   Resource detection: Yellow/green")
    print("   Hazard detection: Red/purple") 
    print("   Wall detection: Gray/black")
    print("   Marker detection: Blue/white")
    
    print("\n4. Feature Extraction:")
    print("   Distance calculation")
    print("   Direction/angle computation")
    print("   Size estimation")
    print("   Texture/edge detection")
    
    # Get and analyze vision data
    percept = vision_world.getSensoryPercept()
    
    # Parse vision data (simplified)
    vision_data = percept.getData()
    data_size = len(vision_data)
    
    print(f"\nVision Data Analysis:")
    print(f"  Total data points: {data_size}")
    print(f"  FOV cells: {fov_width * fov_height}")
    
    # Count different object types
    resource_count = sum(1 for x in vision_data if x == 1.0)
    hazard_count = sum(1 for x in vision_data if x == -1.0)
    wall_count = sum(1 for x in vision_data if x == 0.5)  # Assuming 0.5 for wall
    empty_count = sum(1 for x in vision_data if x == 0.0)
    
    print(f"  Resources detected: {resource_count}")
    print(f"  Hazards detected: {hazard_count}")
    print(f"  Walls detected: {wall_count}")
    print(f"  Empty spaces: {empty_count}")
    
    # Calculate distances to nearest objects
    agent_x, agent_y = vision_world.getAgentPosition()
    
    nearest_resource = find_nearest_object(vision_world, pynlm.WorldObjectType.Resource)
    nearest_hazard = find_nearest_object(vision_world, pynlm.WorldObjectType.Hazard)
    nearest_marker = find_nearest_object(vision_world, pynlm.WorldObjectType.Marker)
    
    print(f"\nNearest Object Distances:")
    if nearest_resource:
        dist = calculate_distance(agent_x, agent_y, nearest_resource.x, nearest_resource.y)
        print(f"  Resource: {dist:.1f} cells at ({nearest_resource.x}, {nearest_resource.y})")
    
    if nearest_hazard:
        dist = calculate_distance(agent_x, agent_y, nearest_hazard.x, nearest_hazard.y)
        print(f"  Hazard: {dist:.1f} cells at ({nearest_hazard.x}, {nearest_hazard.y})")
    
    if nearest_marker:
        dist = calculate_distance(agent_x, agent_y, nearest_marker.x, nearest_marker.y)
        print(f"  Marker: {dist:.1f} cells at ({nearest_marker.x}, {nearest_marker.y})")
    
    return vision_world, agent, percept
```

---

## Part 4: Motor Systems

### Step 7: Motor Command System

```python
print("\n=== Motor Command System ===")

# Motor command types and their effects
motor_types = [
    (pynlm.ActionType.MoveForward, "Move one cell forward", "Position change"),
    (pynlm.ActionType.TurnLeft, "Turn 90 degrees left", "Orientation change"),
    (pynlm.ActionType.TurnRight, "Turn 90 degrees right", "Orientation change"),
    (pynlm.ActionType.Interact, "Interact with nearby object", "Object manipulation"),
    (pynlm.ActionType.Eat, "Consume resource", "Energy increase"),
    (pynlm.ActionType.Sleep, "Rest and recover", "Energy recovery"),
    (pynlm.ActionType.Groom, "Clean self", "Health maintenance"),
    (pynlm.ActionType.Explore, "Random exploration", "Discovery behavior"),
    (pynlm.ActionType.Wait, "Do nothing", "Energy conservation"),
    (pynlm.ActionType.Attack, "Attack nearby agent", "Conflict"),
    (pynlm.ActionType.Defend, "Defend position", "Protection"),
    (pynlm.ActionType.Flee, "Escape from danger", "Survival"),
]

for action_type, description, effect in motor_types:
    print(f"  {action_type}:")
    print(f"    Description: {description}")
    print(f"    Effect: {effect}")
```

### Step 8: Motor Decoding Process

```python
def demonstrate_motor_decoding():
    """Demonstrate motor decoding from neural activity"""
    
    print(f"\n=== Motor Decoding Process ===")
    
    # Create motor learning configuration
    motor_config = pynlm.createLearningConfig()
    motor_config.set("brain.neuron_count", 150)
    motor_config.set("motor.output_mode", "discrete")
    
    # Create brain and agent
    brain = pynlm.createBrain(motor_config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width(20), height(20))
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Motor Decoding Pipeline:")
    print("1. Neural Activity Collection:")
    
    # Simulate neural activity patterns
    neural_patterns = generate_neural_patterns()
    
    print(f"   Collected {len(neural_patterns)} neural activity samples")
    print(f"   Pattern types: {list(neural_patterns.keys())}")
    
    print("\n2. Motor Pattern Mapping:")
    
    motor_mappings = create_motor_mappings()
    
    print("   Motor mapping established:")
    for pattern, action in motor_mappings.items():
        print(f"     {pattern} → {action}")
    
    print("\n3. Action Selection:")
    
    # Demonstrate action selection
    selected_actions = []
    action_probabilities = []
    
    for i in range(10):
        # Get neural activity for this step
        neural_activity = get_current_neural_activity(brain)
        
        # Decode action from neural activity
        action, probability = decode_action_from_neural(neural_activity, motor_mappings)
        
        selected_actions.append(action)
        action_probabilities.append(probability)
        
        print(f"   Step {i}: {action} (confidence: {probability:.2f})")
    
    # Analyze action selection patterns
    print("\n4. Action Selection Analysis:")
    
    action_counts = {}
    for action in selected_actions:
        action_counts[action] = action_counts.get(action, 0) + 1
    
    print("   Action frequencies:")
    for action, count in sorted(action_counts.items()):
        frequency = count / len(selected_actions)
        print(f"     {action}: {count} times ({frequency:.1%})")
    
    # Calculate action diversity
    unique_actions = len(set(selected_actions))
    action_diversity = unique_actions / len(motor_mappings)
    
    print(f"\n   Action diversity: {action_diversity:.2f} ({unique_actions} of {len(motor_mappings)} possible)")
    
    if action_diversity > 0.5:
        print(f"   ✓ Rich behavioral repertoire")
    else:
        print(f"   ⚠️  Limited action variety")
    
    return brain, world, agent, selected_actions
```

### Step 9: Action Execution and Effects

```python
def demonstrate_action_execution():
    """Demonstrate how actions affect the world"""
    
    print(f"\n=== Action Execution and Effects ===")
    
    # Create action experiment world
    action_world = pynlm.createSimpleWorld()
    action_world.configure(width(15), height(15))
    
    # Place various objects
    action_world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, value=30.0))
    action_world.addObject(pynlm.WorldObject(8, 8, pynlm.WorldObjectType.Hazard, value=20.0))
    action_world.addObject(pynlm.WorldObject(12, 5, pynlm.WorldObjectType.Wall, value=0.0))
    
    # Position agent
    action_world.setAgentStart(1, 1)
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(action_world)
    
    print("Action Execution Scenarios:")
    print("-" * 40)
    
    # Scenario 1: Resource collection
    print("\n1. Resource Collection:")
    action_world.reset()
    action = pynlm.Action(pynlm.ActionType.Interact, {'target_x': 3, 'target_y': 3})
    
    print(f"   Action: {action.type} at target ({action.parameters['target_x']}, {action.parameters['target_y']})")
    
    # Execute action
    energy_before = action_world.getAgentEnergy()
    action_world.applyMotorCommand(action, action_world.getSimulationTime())
    energy_after = action_world.getAgentEnergy()
    
    print(f"   Energy before: {energy_before}")
    print(f"   Energy after: {energy_after}")
    print(f"   Energy change: {energy_after - energy_before:+.1f}")
    
    # Scenario 2: Hazard avoidance
    print("\n2. Hazard Avoidance:")
    action_world.reset()
    
    # Calculate action to avoid hazard
    hazard_pos = (8, 8)
    agent_pos = action_world.getAgentPosition()
    
    # Determine escape action
    if hazard_pos[0] > agent_pos[0]:
        escape_action = pynlm.Action(pynlm.ActionType.TurnRight)
    else:
        escape_action = pynlm.Action(pynlm.ActionType.TurnLeft)
    
    print(f"   Hazard at {hazard_pos}, agent at {agent_pos}")
    print(f"   Recommended action: {escape_action.type}")
    
    # Execute action
    action_world.applyMotorCommand(escape_action, action_world.getSimulationTime())
    new_position = action_world.getAgentPosition()
    
    print(f"   New position: {new_position}")
    print(f"   Distance from hazard: {calculate_distance(new_position[0], new_position[1], hazard_pos[0], hazard_pos[1]):.1f} cells")
    
    # Scenario 3: Wall interaction
    print("\n3. Wall Interaction:")
    action_world.reset()
    
    # Try to move into wall
    move_action = pynlm.Action(pynlm.ActionType.MoveForward)
    
    print(f"   Action: {move_action.type} (will hit wall at (12, 5))")
    
    # Execute action (should fail or bounce)
    action_world.applyMotorCommand(move_action, action_world.getSimulationTime())
    
    # Check if position changed
    final_position = action_world.getAgentPosition()
    print(f"   Final position: {final_position}")
    
    if final_position == (1, 1):
        print(f"   ✓ Action blocked by wall (expected behavior)")
    else:
        print(f"   ⚠️  Agent moved despite wall")
    
    # Scenario 4: Skill learning through repetition
    print("\n4. Skill Learning through Repetition:")
    
    # Create sequence of actions to learn
    action_sequence = [
        pynlm.Action(pynlm.ActionType.MoveForward),
        pynlm.Action(pynlm.ActionType.TurnRight),
        pynlm.Action(pynlm.ActionType.MoveForward),
    ]
    
    print(f"   Learning sequence: {[a.type for a in action_sequence]}")
    
    # Repeat sequence multiple times
    success_rates = []
    for trial in range(5):
        action_world.reset()
        success = execute_action_sequence(action_world, action_sequence)
        success_rates.append(success)
        print(f"   Trial {trial + 1}: {'Success' if success else 'Failed'}")
    
    avg_success = np.mean(success_rates)
    print(f"   Average success rate: {avg_success:.1%}")
    
    return action_world, agent
```

---

## Part 5: Perception-Action Loops

### Step 10: Complete Perception-Action Loop

```python
def demonstrate_perception_action_loop():
    """Demonstrate complete perception-action loops"""
    
    print(f"\n=== Perception-Action Loops ===")
    
    # Create loop experiment configuration
    loop_config = pynlm.createDevelopmentConfig()
    loop_config.set("brain.neuron_count", 200)
    
    brain = pynlm.createBrain(loop_config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width(20), height(20), visionWidth(8), visionHeight(8))
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning for adaptation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Complete Perception-Action Loop:")
    print("Step 1: Perceive")
    print("Step 2: Process")  
    print("Step 3: Decide")
    print("Step 4: Act")
    print("Step 5: Observe")
    print("Step 6: Learn")
    print("")
    
    # Run multiple loops to show adaptation
    loop_results = []
    
    for loop in range(5):
        print(f"\nLoop {loop + 1}:")
        
        loop_start_time = world.getSimulationTime()
        loop_actions = []
        loop_rewards = []
        
        # Execute single loop
        for step in range(20):
            # 1. Perceive (get sensory input)
            percept = world.getSensoryPercept()
            
            # 2. Process (interpret sensory data)
            agent.processSensoryInput(percept)
            
            # 3. Decide (select action)
            action = agent.decodeMotorCommand()
            loop_actions.append(action.type)
            
            # 4. Act (execute action in world)
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # 5. Observe (get feedback)
            reward = world.getReward()
            loop_rewards.append(reward)
            
            # 6. Learn (update based on experience)
            error = agent.getPredictionError()
            agent.applyRewardModulation(reward, error)
            agent.updateDevelopment(0.1)
            
            if step % 5 == 0:
                print(f"    Step {step}: {action.type}, Reward: {reward:+.2f}")
        
        # Analyze loop results
        avg_reward = np.mean(loop_rewards)
        action_counts = {}
        for action in loop_actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        loop_results.append({
            'loop_num': loop + 1,
            'avg_reward': avg_reward,
            'actions': action_counts,
            'total_steps': len(loop_actions)
        })
        
        print(f"    Loop completed: Avg reward = {avg_reward:.2f}")
        print(f"    Actions taken: {list(action_counts.keys())}")
        
        # Reset for next loop
        world.reset()
        agent.initialize(world)
    
    # Analyze loop adaptation
    print(f"\nLoop Adaptation Analysis:")
    
    reward_trend = [result['avg_reward'] for result in loop_results]
    improvement = reward_trend[-1] - reward_trend[0]
    
    print(f"  Reward trend: {reward_trend}")
    print(f"  Improvement: {improvement:+.2f}")
    
    if improvement > 0:
        print(f"  ✓ Adaptive learning: Performance improved over loops")
    elif improvement < 0:
        print(f"  ⚠️  Maladaptive learning: Performance declined")
    else:
        print(f"  → Stable performance")
    
    # Check behavioral diversity
    all_actions = []
    for result in loop_results:
        all_actions.extend(list(result['actions'].keys()))
    
    unique_actions = set(all_actions)
    behavioral_diversity = len(unique_actions) / len(agent.getAvailableActions())
    
    print(f"  Behavioral diversity: {behavioral_diversity:.2f} ({len(unique_actions)} of {len(agent.getAvailableActions())})")
    
    if behavioral_diversity > 0.5:
        print(f"  ✓ Rich behavioral repertoire developed")
    else:
        print(f"  ⚠️  Limited behavioral range")
    
    return brain, world, agent, loop_results
```

### Step 11: Multi-Loop Integration

```python
print(f"\n=== Multi-Loop Integration ===")

# Demonstrate integration across multiple loops
print("Integrating perception-action loops with learning systems:")
print("- Sensory processing updates based on experience")
print("- Motor planning adapts to learned patterns")
print("- Development progresses through stages")
print("- Neuromodulation guides exploration vs exploitation")

# Create comprehensive integration example
integration_config = pynlm.createLearningConfig()
integration_config.set("brain.neuron_count", 250)
integration_config.set("neuromod.curiosity.enable", True)
integration_config.set("memory.enable_working_memory", True)

# Run comprehensive integration experiment
brain = pynlm.createBrain(integration_config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width(25), height(25))

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("\nComprehensive Integration Experiment:")
print("Running 100 simulation steps with full learning capabilities...")

# Run extended simulation
integration_results = []

for step in range(100):
    # Complete perception-action loop
    world.update(0.1)
    
    # Perceive: Get sensory input
    percept = world.getSensoryPercept()
    
    # Process: Neural interpretation
    agent.processSensoryInput(percept)
    
    # Decide: Action selection
    action = agent.decodeMotorCommand()
    
    # Act: Execute in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Observe: Get feedback
    reward = world.getReward()
    error = agent.getPredictionError()
    
    # Learn: Update based on experience
    agent.applyRewardModulation(reward, error)
    agent.updateDevelopment(0.1)
    
    # Record metrics
    if step % 20 == 0:
        integration_results.append({
            'step': step,
            'position': world.getAgentPosition(),
            'energy': world.getAgentEnergy(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'reward': reward,
            'error': error,
            'development_stage': brain.getDevelopmentalStage().value
        })
        
        print(f"  Step {step:3d}: Pos {integration_results[-1]['position']}, "
              f"Energy {integration_results[-1]['energy']:4.1f}, "
              f"Curiosity {integration_results[-1]['curiosity']:4.2f}, "
              f"Reward {integration_results[-1]['reward']:6.2f}")

print(f"\nIntegration Results Summary:")
final_metrics = integration_results[-1]
print(f"  Final position: {final_metrics['position']}")
print(f"  Final energy: {final_metrics['energy']:.1f}")
print(f"  Final curiosity: {final_metrics['curiosity']:.3f}")
print(f"  Final development stage: {final_metrics['development_stage']}")

# Analyze integration quality
energy_efficiency = final_metrics['energy'] / 100.0
curiosity_balance = min(final_metrics['curiosity'], 1.0)
development_progress = final_metrics['development_stage'] / 4.0  # Normalized

integration_score = (energy_efficiency + curiosity_balance + development_progress) / 3.0

print(f"\nIntegration Quality Score: {integration_score:.3f}")

if integration_score > 0.7:
    print(f"  ✓ Excellent integration: All systems working harmoniously")
elif integration_score > 0.5:
    print(f"  ✓ Good integration: Most systems functional")
elif integration_score > 0.3:
    print(f"  ⚠️  Moderate integration: Some systems need improvement")
else:
    print(f"  ✗ Poor integration: Major system issues")

return brain, world, agent, integration_results
```

---

## Part 6: Advanced Environment Interaction

### Step 12: Dynamic World Interaction

```python
def demonstrate_dynamic_world_interaction():
    """Demonstrate interaction with dynamic, changing environments"""
    
    print(f"\n=== Dynamic World Interaction ===")
    
    # Create world with dynamic elements
    dynamic_world = pynlm.createSimpleWorld()
    dynamic_world.configure(width(20), height(20))
    
    # Add dynamic elements
    dynamic_world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=50.0))
    dynamic_world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Hazard, value=30.0))
    
    # Create agent
    brain = pynlm.createBrain(pynlm.createDevelopmentConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(dynamic_world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Dynamic World Elements:")
    print("1. Changing resource availability")
    print("2. Moving hazards (simulated)")
    print("3. Environmental decay/growth")
    print("4. Seasonal or temporal changes")
    
    # Simulate dynamic environment
    print(f"\nDynamic Environment Simulation:")
    
    environment_changes = []
    
    for step in range(50):
        # Environment changes every 10 steps
        if step % 10 == 0 and step > 0:
            # Resource regeneration
            resource_x = random.randint(1, 18)
            resource_y = random.randint(1, 18)
            dynamic_world.addObject(pynlm.WorldObject(resource_x, resource_y, 
                                                   pynlm.WorldObjectType.Resource, value=30.0))
            environment_changes.append(f"Step {step}: New resource at ({resource_x}, {resource_y})")
            
            # Hazard movement (simplified)
            if len(dynamic_world.getObjects()) > 1:
                hazard = dynamic_world.getObjects()[-1]
                if hazard.obj_type == pynlm.WorldObjectType.Hazard:
                    new_x = min(19, hazard.x + random.randint(-2, 2))
                    new_y = min(19, hazard.y + random.randint(-2, 2))
                    dynamic_world.removeObject(hazard.x, hazard.y)
                    dynamic_world.addObject(pynlm.WorldObject(new_x, new_y, 
                                                       pynlm.WorldObjectType.Hazard, value=30.0))
                    environment_changes.append(f"Step {step}: Hazard moved to ({new_x}, {new_y})")
        
        # Agent interaction
        dynamic_world.update(0.1)
        
        # Get sensory input (responds to changes)
        percept = dynamic_world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        dynamic_world.applyMotorCommand(action, dynamic_world.getSimulationTime())
        
        # Learning from environment changes
        reward = dynamic_world.getReward()
        error = agent.getPredictionError()
        agent.applyRewardModulation(reward, error)
        
        # Record environment changes
        if step % 10 == 0:
            print(f"\n  Environment state at step {step}:")
            current_objects = dynamic_world.getObjects()
            resources = sum(1 for obj in current_objects if obj.obj_type == pynlm.WorldObjectType.Resource)
            hazards = sum(1 for obj in current_objects if obj.obj_type == pynlm.WorldObjectType.Hazard)
            print(f"    Resources: {resources}, Hazards: {hazards}")
    
    print(f"\nEnvironment Changes Summary:")
    for change in environment_changes:
        print(f"  {change}")
    
    print(f"\nAgent Adaptation to Dynamic Environment:")
    print(f"  Demonstrates flexibility and learning")
    print(f"  Shows adaptation to changing conditions")
    print(f"  Highlights importance of dynamic interaction")
    
    return dynamic_world, agent
```

### Step 13: Social Environment Interaction

```python
def demonstrate_social_interaction():
    """Demonstrate interaction in social environments"""
    
    print(f"\n=== Social Environment Interaction ===")
    
    # Create multi-agent world
    social_world = pynlm.createMultiAgentWorld()
    social_world.configure(
        grid_size=12,
        num_agents=3,
        communication_range=2
    )
    
    # Create agents with different characteristics
    agents = []
    for i in range(3):
        agent_config = pynlm.createLearningConfig()
        agent_config.set("brain.neuron_count", 150)
        agent_config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(agent_config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(social_world)
        
        agents.append(agent)
        
        # Give agents different starting positions
        pos_x = (i * 3) + 1
        pos_y = (i * 2) + 1
        social_world.setAgentStart(pos_x, pos_y)
    
    # Enable social learning
    for agent in agents:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableSocialLearning(True)  # Enable social learning
    
    print("Multi-Agent Environment Setup:")
    print(f"  Grid size: {social_world.getGridSize()}x{social_world.getGridSize()}")
    print(f"  Number of agents: {social_world.getAgentCount()}")
    print(f"  Communication range: {social_world.getCommunicationRange()} cells")
    
    # Simulate social interaction
    print(f"\nSocial Interaction Simulation:")
    print("Agents interact through:")
    print("  1. Shared resources and obstacles")
    print("  2. Observational learning from others")
    print("  3. Competitive and cooperative behaviors")
    print("  4. Social learning and imitation")
    
    # Run social simulation
    for step in range(50):
        social_world.update(0.1)
        
        # Each agent perceives social environment
        for i, agent in enumerate(agents):
            percept = social_world.getSensoryPercept(i)
            agent.processSensoryInput(percept)
            
            # Neural processing
            brain.step(step)
            
            # Socially-informed action selection
            action = agent.decodeSocialAction()
            
            # Execute action
            social_world.applySocialAction(i, action)
            
            # Social learning from observation
            agent.observeOtherAgent(i, action)
        
        # Shared environmental changes
        if step % 10 == 0:
            print(f"\n  Social state at step {step}:")
            for i, agent in enumerate(agents):
                pos = social_world.getAgentPosition(i)
                energy = social_world.getAgentEnergy(i)
                print(f"    Agent {i}: Pos {pos}, Energy {energy}")
    
    print(f"\nSocial Learning Analysis:")
    print(f"  Agents develop social strategies")
    print(f"  Learning from observation and interaction")
    print(f"  Emergence of cooperative and competitive behaviors")
    
    return social_world, agents
```

---

## Part 7: Summary and Next Steps

### What You've Accomplished in Tutorial 3

✅ **World System Architecture**: Understanding world components and configuration
✅ **Sensory Processing**: Vision, proprioception, and internal sensing systems
✅ **Motor Systems**: Action selection, decoding, and execution
✅ **Perception-Action Loops**: Complete interaction cycles
✅ **Dynamic Environments**: Adapting to changing conditions
✅ **Social Interaction**: Multi-agent environments and social learning

### Key Concepts Mastered

1. **Sensorimotor Integration**: How sensory input drives motor output
2. **Closed-Loop Control**: Continuous perception-action cycles
3. **Environmental Dynamics**: How environments change and affect behavior
4. **Social Cognition**: Learning from other agents
5. **Adaptive Behavior**: Flexible responses to changing conditions
6. **Multi-Modal Processing**: Integrating information from multiple senses
7. **Action Selection**: Choosing appropriate behaviors

### Technical Skills Developed

1. **World Configuration**: Setting up different world types and parameters
2. **Sensory Analysis**: Interpreting environmental input
3. **Motor Planning**: Selecting and executing actions
4. **Behavioral Modeling**: Understanding action consequences
5. **Social Dynamics**: Multi-agent interaction patterns
6. **Environmental Simulation**: Creating and managing virtual worlds

### Next Tutorial Options

1. **Tutorial 4: Memory Systems** - Deep dive into memory mechanisms
2. **Tutorial 5: Performance Optimization** - Speed and efficiency improvements
3. **Tutorial 6: Experimental Design** - Research methodology
4. **Tutorial 7: Advanced Applications** - Real-world implementations

### Recommended Documentation to Read Next

- `docs/MEMORY_SYSTEMS.md` - Memory system details
- `docs/NEUROMODULATION.md` - Neuromodulation mechanisms
- `docs/PHASE_GUIDE.md` - Development phase guidance
- `docs/tutorials/Performance_Optimization` (Tutorial 5)

### Practice Exercises

1. **Create custom worlds** with different rules and objects
2. **Design behavior patterns** for specific tasks
3. **Implement sensory augmentation** or impairment
4. **Study social dynamics** in multi-agent systems
5. **Optimize action selection** for different environments

### Assessment Questions

Answer these questions to check your understanding:

1. **Sensory Processing**: How does the vision system detect and categorize objects?
2. **Motor Control**: What factors influence action selection in NLM?
3. **Environmental Interaction**: How do world dynamics affect agent behavior?
4. **Social Learning**: What mechanisms enable learning from other agents?
5. **Perception-Action Loops**: How do feedback signals influence future actions?
6. **Adaptive Behavior**: How does NLM adjust to changing environments?
7. **Multi-Modal Integration**: Why is integrating multiple senses important?

### Quick Reference Summary

```python
# Key environment-interaction concepts:

# 1. World System Components:
#    - Environment state and rules
#    - Agent position and properties
#    - Objects and entities
#    - Time and dynamics

# 2. Sensory Processing:
#    - Vision: Spatial and feature extraction
#    - Proprioception: Body position and movement
#    - Internal: Physiological states
#    - Integration: Multi-modal combination

# 3. Motor Systems:
#    - Action selection: Neural decoding
#    - Action execution: World application
#    - Feedback: Result monitoring
#    - Learning: Experience integration

# 4. Interaction Loops:
#    - Perceive: Get sensory input
#    - Process: Interpret and encode
#    - Decide: Plan and select actions
#    - Act: Execute in environment
#    - Observe: Evaluate results
#    - Learn: Update based on feedback

# 5. Social Interaction:
#    - Multi-agent coordination
#    - Observational learning
#    - Shared environment
#    - Competitive/cooperative behaviors
```

### Common Challenges and Solutions

**Challenge: Low Sensory Accuracy**
- **Solution**: Calibrate sensory thresholds and sensitivity
- **Alternative**: Use additional sensory modalities

**Challenge: Poor Action Selection**
- **Solution**: Implement better reward functions
- **Alternative**: Use curiosity-driven exploration

**Challenge: Environment Complexity**
- **Solution**: Simplify world rules gradually
- **Alternative**: Use hierarchical task decomposition

**Challenge: Social Dynamics**
- **Solution**: Implement social learning algorithms
- **Alternative**: Start with single-agent environments

---

## Welcome to Tutorial 3!

You've completed a comprehensive exploration of NLM environment interaction! You now understand:

- **Sensory Systems**: How agents perceive their world
- **Motor Systems**: How agents act upon their perceptions
- **World Dynamics**: How environments change and adapt
- **Social Interaction**: Multi-agent coordination and learning
- **Perception-Action Loops**: Complete interaction cycles
- **Adaptive Behavior**: Flexible responses to changing conditions

You're well-equipped to:

1. **Design interactive environments** for specific tasks
2. **Implement sensory and motor systems** for robots/biological models
3. **Study behavioral adaptation** in changing conditions
4. **Analyze social dynamics** and multi-agent interactions
5. **Optimize perception-action loops** for efficiency

**Your NLM expertise continues:** Next up is Tutorial 4 on Memory Systems, where you'll explore how NLM stores and retrieves information!

**Documentation Index:** Continue with `docs/tutorials/Memory_Systems`, `docs/tutorials/Performance_Optimization`, and `docs/tutorials/Experimental_Design`.

**Need help?** Check the Troubleshooting Guide at `docs/TROUBLESHOOTING.md` or revisit concepts in `docs/ADVANCED_GUIDE.md`.

Happy simulating! 🚀

---

*Tutorial 3 Complete!* You've mastered the art of agent-environment interaction in NLM. Your foundation for understanding how NLM systems operate in the real world is now solid.

**Score your understanding:** How many of the environment-interaction concepts did you implement correctly? Which part was most challenging? Which part was most fascinating?

Proceed to Tutorial 4 when you're ready to advance your NLM expertise!

**Next Tutorial:** `docs/tutorials/Memory_Systems` - Deep dive into how NLM stores and retrieves information.

**Documentation Index:** Complete set of tutorials and guides in the `docs/` directory.

*Keep exploring the NLM documentation to build your expertise!*

---

*Tutorial 3 Complete!* You've successfully navigated the complex world of NLM environment interaction. Your skills in designing, simulating, and analyzing agent-environment systems are now well-developed.

**Tutorial Progression:** Tutorial 1 (Basics) → Tutorial 2 (Learning) → Tutorial 3 (Environment) → Tutorial 4 (Memory) → Tutorial 5 (Performance)

**Your NLM Journey Continues:** Use your new skills to tackle more complex NLM applications and research questions!

*Ready for Tutorial 4? Continue building your NLM expertise!*

---

*Tutorial 3 Complete!* Environment interaction mastery achieved! 🎯

**Key Achievements:**
- ✅ World system architecture understanding
- ✅ Sensory processing expertise  
- ✅ Motor systems knowledge
- ✅ Perception-action loop mastery
- ✅ Dynamic environment handling
- ✅ Social interaction capabilities

**NLM Expertise Level:** Advanced Beginner

**Next Steps:**
1. **Tutorial 4:** Memory Systems Deep Dive
2. **Tutorial 5:** Performance Optimization
3. **Tutorial 6:** Experimental Design
4. **Documentation:** Complete comprehensive guides

**Your NLM expertise is expanding!** Use these skills to create sophisticated NLM applications and research projects.

---

*Tutorial 3 Complete!* You've successfully navigated one of NLM's most complex and fascinating domains. Your understanding of how agents interact with their environment is now comprehensive.

**Professional Readiness:** You can now:
- Design realistic interaction scenarios
- Implement and debug sensory-motor systems
- Analyze behavioral patterns in dynamic environments
- Evaluate social and multi-agent dynamics
- Optimize performance for specific applications

**Your NLM toolkit is growing!** Continue practicing with the examples and documentation to solidify these skills.

**Score your understanding:** How well do you think you'll perform in Tutorial 4's memory systems? What's your confidence level in implementing these interaction concepts?

**Next Mission:** Tutorial 4 - Memory Systems. Prepare for a deep dive into how NLM stores, retrieves, and uses information!

*Your NLM journey continues!* 🚀