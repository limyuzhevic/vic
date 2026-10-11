# Hands-On Examples

This section provides practical, ready-to-run code examples that demonstrate the core capabilities of the NLM framework. These examples are designed to be educational, instructive, and immediately useful for learning and experimentation.

## Overview

The Hands-On Examples section bridges the gap between theoretical tutorials and practical implementation. Each example focuses on a specific aspect of NLM functionality and provides complete, runnable code with clear explanations.

## Example Structure

Each example follows a consistent structure:

1. **Problem Description**: What the example demonstrates
2. **Key Concepts**: Core principles and mechanisms
3. **Complete Code**: Fully functional Python implementation
4. **Analysis**: Results interpretation and insights
5. **Extensions**: Ideas for further exploration

## Example 1: Simple Feed-Forward Neural Network

### Problem Description
Create a basic feed-forward neural network that processes input and produces output without recurrent connections.

### Key Concepts
- Feed-forward architecture
- Static signal processing
- Layered neural organization
- Direct input-output relationships

### Complete Code

```python
import pynlm

def create_feed_forward_network(input_size=10, hidden_size=20, output_size=5):
    """Create a simple feed-forward neural network"""
    
    # Create configuration for feed-forward architecture
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", input_size + hidden_size + output_size)
    config.set("plasticity.stdp.enable", False)  # Static network
    config.set("plasticity.hebbian.enable", False)
    config.set("plasticity.structural.enable", False)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    return brain, agent

def run_feed_forward_simulation(brain, agent, input_data, steps=100):
    """Run feed-forward simulation with given input"""
    
    print(f"Running feed-forward simulation with {len(input_data)} input features")
    print("Step | Output Pattern | Stability")
    print("-----|----------------|----------")
    
    for step in range(steps):
        # Inject input data
        for i, value in enumerate(input_data):
            if i < brain.getTotalNeuronCount() // 3:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, value * 10)
        
        # Run brain step
        brain.step(step)
        
        # Monitor output
        if step % 20 == 0:
            output_pattern = get_output_pattern(brain, output_size=5)
            stability = calculate_pattern_stability(brain)
            
            print(f"{step:4d} | {output_pattern} | {stability:9.3f}")
    
    return brain

def get_output_pattern(brain, output_size=5):
    """Extract output pattern from brain activity"""
    
    # Get firing neurons
    firing_neurons = brain.getFiringNeuronCount()
    total_neurons = brain.getTotalNeuronCount()
    
    # Create simple pattern representation
    if firing_neurons > total_neurons * 0.1:
        return "High Activity"
    elif firing_neurons > total_neurons * 0.05:
        return "Moderate Activity"
    else:
        return "Low Activity"

def calculate_pattern_stability(brain):
    """Calculate pattern stability over time"""
    
    # Simplified stability calculation
    # In a real implementation, track firing over multiple steps
    firing = brain.getFiringNeuronCount()
    total = brain.getTotalNeuronCount()
    
    if total == 0:
        return 0.0
    
    return firing / total

# Example usage
if __name__ == "__main__":
    # Create feed-forward network
    brain, agent = create_feed_forward_network(input_size=8, hidden_size=16, output_size=3)
    
    # Run simulation with sample input
    input_data = [0.5, 0.3, 0.8, 0.2, 0.6, 0.4, 0.7, 0.1]
    run_feed_forward_simulation(brain, agent, input_data, steps=80)
    
    print(f"\nFeed-forward network simulation complete!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Analysis

This example demonstrates:

1. **Static Network Architecture**: Without plasticity, the network maintains stable connections
2. **Input Processing**: Sensory inputs are injected and propagate through the network
3. **Output Generation**: Brain activity produces observable output patterns
4. **Network Stability**: Feed-forward networks maintain consistent behavior without learning

### Extensions

1. **Add plasticity**: Enable STDP or Hebbian learning for dynamic networks
2. **Multi-layer architecture**: Create deeper networks with multiple hidden layers
3. **Dynamic inputs**: Use time-varying input signals
4. **Performance metrics**: Add quantitative output analysis

## Example 2: Recurrent Neural Network with Memory

### Problem Description
Create a recurrent neural network that maintains information over time, demonstrating short-term memory capabilities.

### Key Concepts
- Recurrent connections
- Short-term memory
- Temporal processing
- State maintenance

### Complete Code

```python
import pynlm

def create_recurrent_network(neuron_count=50, memory_strength=0.7):
    """Create a recurrent neural network with memory capabilities"""
    
    # Create configuration with recurrent connections
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.enable", True)  # Enable learning for memory
    config.set("plasticity.hebbian.enable", True)
    config.set("brain.synapse_density", 0.15)  # Higher connectivity for recurrence
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for memory management
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)
    
    return brain, agent

def run_recurrent_memory_simulation(brain, agent, steps=200, pattern_length=10):
    """Run recurrent network simulation with memory tasks"""
    
    print(f"Running recurrent network simulation with {pattern_length}-step patterns")
    print("Step | Memory State | Pattern Accuracy | Stability")
    print("-----|--------------|-----------------|----------")
    
    # Memory task: reproduce input patterns after delay
    memory_performance = {
        'pattern_remembered': [],
        'reproduction_accuracy': [],
        'memory_decay': []
    }
    
    for step in range(steps):
        # Generate and present pattern
        if step < pattern_length:
            pattern = generate_input_pattern(pattern_length)
            present_pattern(brain, pattern, step)
        
        # Memory delay and recall
        elif step < pattern_length * 2:
            delay_steps = step - pattern_length
            recall_accuracy = recall_stored_pattern(brain, pattern, delay_steps)
            memory_performance['pattern_remembered'].append(1 if recall_accuracy > 0.5 else 0)
            memory_performance['reproduction_accuracy'].append(recall_accuracy)
            memory_performance['memory_decay'].append(calculate_memory_decay(brain, delay_steps))
            
            print(f"{step:4d} | Maintaining | {recall_accuracy:15.2f} | {calculate_network_stability(brain):9.3f}")
        
        # Continue normal processing
        else:
            brain.step(step)
    
    return brain, memory_performance

def generate_input_pattern(length):
    """Generate random input pattern"""
    import random
    return [random.random() for _ in range(length)]

def present_pattern(brain, pattern, step):
    """Present pattern to network"""
    
    for i, value in enumerate(pattern):
        if i < brain.getTotalNeuronCount() // 3:
            # Stronger input for pattern encoding
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, value * 15)
    
    brain.step(step)

def recall_stored_pattern(brain, pattern, delay_steps):
    """Recall stored pattern after delay"""
    
    # Simplified recall mechanism
    # In a real implementation, this would access working memory
    
    # Calculate similarity between current state and target pattern
    current_firing = brain.getFiringNeuronCount()
    target_activity = sum(pattern) / len(pattern) * 15
    
    # Similarity metric
    similarity = min(1.0, current_firing / (target_activity + 1))
    
    return similarity

def calculate_memory_decay(brain, delay_steps):
    """Calculate memory decay over time"""
    
    # Memory decay based on network dynamics
    decay_factor = 0.95 ** delay_steps  # Exponential decay
    
    # Modulate by network properties
    firing = brain.getFiringNeuronCount()
    total = brain.getTotalNeuronCount()
    
    if total == 0:
        return 0.0
    
    activity = firing / total
    return activity * decay_factor

def calculate_network_stability(brain):
    """Calculate network stability"""
    
    # Simplified stability metric
    firing = brain.getFiringNeuronCount()
    total = brain.getTotalNeuronCount()
    
    if total == 0:
        return 0.0
    
    return firing / total

# Example usage
if __name__ == "__main__":
    # Create recurrent network
    brain, agent = create_recurrent_network(neuron_count=40, memory_strength=0.8)
    
    # Run memory simulation
    pattern_length = 8
    brain, performance = run_recurrent_memory_simulation(
        brain, agent, steps=pattern_length * 3, pattern_length=pattern_length
    )
    
    # Analyze memory performance
    print(f"\nRecurrent Network Memory Performance:")
    
    if performance['pattern_remembered']:
        accuracy = sum(performance['reproduction_accuracy']) / len(performance['reproduction_accuracy'])
        success_rate = sum(performance['pattern_remembered']) / len(performance['pattern_remembered'])
        
        print(f"   Pattern recall accuracy: {accuracy:.3f}")
        print(f"   Success rate: {success_rate:.1%}")
        print(f"   Average memory decay: {sum(performance['memory_decay']) / len(performance['memory_decay']):.3f}")
        
        if accuracy > 0.7:
            print("   ✓ Strong memory capabilities demonstrated")
        elif accuracy > 0.5:
            print("   = Moderate memory performance")
        else:
            print("   ! Limited memory performance")
```

### Analysis

This example demonstrates:

1. **Recurrent Dynamics**: Connections that feedback create temporal dependencies
2. **Memory Maintenance**: Networks can maintain information over time
3. **Pattern Completion**: Partial inputs can trigger full pattern recall
4. **Memory Decay**: Memories naturally decay without reinforcement

### Extensions

1. **Long short-term memory**: Implement mechanisms for longer memory retention
2. **Multi-pattern tasks**: Test network with multiple sequential patterns
3. **Memory interference**: Study how similar patterns compete
4. **External memory**: Integrate with working memory systems

## Example 3: Multi-Region Brain Architecture

### Problem Description
Create a brain with multiple specialized regions, each responsible for different functions.

### Key Concepts
- Modular architecture
- Specialized regions
- Inter-regional communication
- Functional segregation and integration

### Complete Code

```python
import pynlm

def create_multi_region_brain(num_regions=4, neurons_per_region=30):
    """Create a multi-region brain architecture"""
    
    # Create configuration with multiple regions
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", num_regions * neurons_per_region)
    config.set("brain.region_count", num_regions)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize regions with different specializations
    initialize_region_specializations(brain, num_regions)
    
    return brain, agent

def initialize_region_specializations(brain, num_regions):
    """Initialize regions with different functional specializations"""
    
    specializations = [
        ("Sensory Processing", pynlm.NeuronType.Sensory, 0.8),
        ("Motor Control", pynlm.NeuronType.Motor, 0.8),
        ("Working Memory", pynlm.NeuronType.Internal, 0.6),
        ("Integration", pynlm.NeuronType.Excitatory, 0.5)
    ]
    
    for region_id in range(1, num_regions + 1):
        region = brain.getRegion(region_id)
        if region:
            specialization_name, preferred_type, specialization_strength = specializations[region_id - 1]
            
            # Configure region for specialization
            configure_region_specialization(region, preferred_type, specialization_strength)
            
            print(f"   Region {region_id}: {specialization_name} ({specialization_strength:.1f})")

def configure_region_specialization(region, preferred_type, strength):
    """Configure region for specific functional specialization"""
    
    # This is a simplified implementation
    # In a real system, you would modify region-specific parameters
    
    pass  # Implementation depends on region API

def demonstrate_region_communication(brain, agent, world, steps=200):
    """Demonstrate communication between different brain regions"""
    
    print(f"\n=== Multi-Region Communication ===")
    print(f"Testing inter-regional communication over {steps} steps:\n")
    
    # Track regional activity
    regional_activity = {
        'region_1': [],  # Sensory
        'region_2': [],  # Motor
        'region_3': [],  # Memory
        'region_4': []   # Integration
    }
    
    # Track inter-regional connections
    connection_metrics = {
        'sensory_to_motor': [],
        'sensory_to_memory': [],
        'motor_to_integration': [],
        'memory_to_integration': []
    }
    
    for step in range(steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain (activates multiple regions)
        brain.step(step)
        
        # Record regional activity
        for region_id in range(1, 5):
            region = brain.getRegion(region_id)
            if region:
                activity = region.getNeuronCount() / brain.getTotalNeuronCount()
                regional_activity[f'region_{region_id}'].append(activity)
        
        # Record inter-regional connections
        if step % 20 == 0:
            sensory_motor = measure_region_connection(brain, 1, 2)
            sensory_memory = measure_region_connection(brain, 1, 3)
            motor_integration = measure_region_connection(brain, 2, 4)
            memory_integration = measure_region_connection(brain, 3, 4)
            
            connection_metrics['sensory_to_motor'].append(sensory_motor)
            connection_metrics['sensory_to_memory'].append(sensory_memory)
            connection_metrics['motor_to_integration'].append(motor_integration)
            connection_metrics['memory_to_integration'].append(memory_integration)
            
            print(f"   Step {step:3d}: "
                  f"Sensory→Motor={sensory_motor:5.3f} "
                  f"Sensory→Memory={sensory_memory:5.3f} "
                  f"Motor→Integration={motor_integration:5.3f} "
                  f"Memory→Integration={memory_integration:5.3f}")
    
    # Analyze regional communication
    print(f"\nMulti-Region Communication Analysis:")
    
    print(f"   Regional activity averages:")
    for region, activities in regional_activity.items():
        avg_activity = sum(activities) / len(activities)
        print(f"     {region}: {avg_activity:.3f}")
    
    print(f"   Inter-regional connection strengths:")
    for connection, values in connection_metrics.items():
        avg_connection = sum(values) / len(values)
        print(f"     {connection}: {avg_connection:.3f}")
    
    # Check for balanced communication
    avg_connections = sum(sum(v) for v in connection_metrics.values()) / sum(len(v) for v in connection_metrics.values())
    
    if avg_connections > 0.3:
        print("   ✓ Strong inter-regional communication")
    elif avg_connections > 0.15:
        print("   = Moderate inter-regional communication")
    else:
        print("   ! Weak inter-regional communication")
    
    return regional_activity, connection_metrics

def measure_region_connection(brain, source_region, target_region):
    """Measure connection strength between regions"""
    
    # Simplified connection measurement
    # In a real system, this would analyze actual synaptic connections
    
    source = brain.getRegion(source_region)
    target = brain.getRegion(target_region)
    
    if not source or not target:
        return 0.0
    
    # Estimate based on regional sizes and development
    source_activity = source.getNeuronCount() / brain.getTotalNeuronCount()
    target_activity = target.getNeuronCount() / brain.getTotalNeuronCount()
    
    return (source_activity + target_activity) / 2

# Example usage
if __name__ == "__main__":
    # Create multi-region brain
    brain, agent = create_multi_region_brain(num_regions=4, neurons_per_region=25)
    
    # Create simple world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Run communication demonstration
    regional_activity, connection_metrics = demonstrate_region_communication(
        world, agent, brain, steps=150
    )
    
    print(f"\nMulti-region architecture demonstration complete!")
    print(f"Total regions: {brain.getRegionCount()}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Analysis

This example demonstrates:

1. **Modular Design**: Different brain regions specialized for different functions
2. **Functional Segregation**: Each region handles specific types of processing
3. **Integration**: Multiple regions work together for complex tasks
4. **Communication**: Information flows between specialized regions

### Extensions

1. **Cross-region learning**: Enable learning across region boundaries
2. **Region damage**: Test effects of region damage or removal
3. **Region development**: Study how regions develop and specialize
4. **Region competition**: Study how regions compete for resources

## Example 4: Sensory Processing Pipeline

### Problem Description
Implement a complete sensory processing pipeline that transforms raw sensory input into meaningful representations.

### Key Concepts
- Sensory transduction
- Feature extraction
- Pattern recognition
- Sensory filtering

### Complete Code

```python
import pynlm

def create_sensory_processing_pipeline(brain_width=20, brain_height=20):
    """Create sensory processing pipeline with world environment"""
    
    # Create world with sensory capabilities
    world = pynlm.createSimpleWorld()
    world.configure(
        width=brain_width,
        height=brain_height,
        visionWidth=10,
        visionHeight=10
    )
    
    # Configure world with various sensory features
    configure_sensory_features(world)
    
    return world

def configure_sensory_features(world):
    """Configure world with various sensory features"""
    
    # Add resources with different sensory properties
    world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, value=15.0))
    world.addObject(pynlm.WorldObject(7, 7, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(12, 12, pynlm.WorldObjectType.Resource, value=20.0))
    
    # Add hazards with negative sensory properties
    world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Hazard, value=-8.0))
    world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Hazard, value=-12.0))
    
    # Add visual landmarks
    world.addObject(pynlm.WorldObject(2, 8, pynlm.WorldObjectType.Marker, value=1.0))
    world.addObject(pynlm.WorldObject(8, 2, pynlm.WorldObjectType.Marker, value=1.0))
    world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Marker, value=1.0))
    
    # Set agent starting position
    world.setAgentStart(10.0, 10.0)

def run_sensory_processing_demo(world, agent, brain, steps=200):
    """Run complete sensory processing demonstration"""
    
    print(f"\n=== Sensory Processing Pipeline ===")
    print(f"Processing sensory input over {steps} steps:\n")
    
    # Track sensory processing metrics
    sensory_metrics = {
        'raw_input': [],
        'feature_extraction': [],
        'pattern_recognition': [],
        'decision_making': []
    }
    
    # Track agent actions and outcomes
    action_metrics = {
        'actions_taken': [],
        'success_rate': [],
        'learning_progress': []
    }
    
    for step in range(steps):
        world.update(0.1)
        
        # 1. Raw sensory input
        raw_input = get_raw_sensory_input(world)
        sensory_metrics['raw_input'].append(raw_input)
        
        # 2. Sensory agent processing
        agent.processSensoryInput(raw_input)
        
        # 3. Brain processing (feature extraction)
        brain.step(step)
        
        # 4. Action decoding
        action = agent.decodeMotorCommand()
        action_metrics['actions_taken'].append(action)
        
        # 5. Action execution
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Reward calculation (learning signal)
        reward = calculate_sensory_reward(world, action)
        agent.applyRewardModulation(reward, 0.0)
        
        # 7. Extract higher-level features
        features = extract_sensory_features(raw_input, action, reward)
        sensory_metrics['feature_extraction'].append(features)
        
        # 8. Pattern recognition
        patterns = recognize_sensory_patterns(sensory_metrics['feature_extraction'][-5:])
        sensory_metrics['pattern_recognition'].append(patterns)
        
        # 9. Decision making
        decisions = make_sensory_decisions(patterns, action, reward)
        sensory_metrics['decision_making'].append(decisions)
        
        # Monitor performance
        if step % 30 == 0:
            body = world.getAgentBody()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            
            print(f"   Step {step:3d}: Pos({body.getX():5.1f},{body.getY():5.1f}) "
                  f"Energy({body.getEnergy():4.1f}) "
                  f"Actions={len(set(action_metrics['actions_taken'][-5:]))} "
                  f"Curiosity={curiosity:5.3f} Novelty={novelty:5.3f}")
    
    # Analyze sensory processing performance
    print(f"\nSensory Processing Analysis:")
    
    # Raw input analysis
    avg_raw_input = sum(sensory_metrics['raw_input']) / len(sensory_metrics['raw_input'])
    print(f"   Average sensory input: {avg_raw_input:.3f}")
    
    # Feature extraction quality
    if sensory_metrics['feature_extraction']:
        avg_features = sum(len(f) for f in sensory_metrics['feature_extraction']) / len(sensory_metrics['feature_extraction'])
        print(f"   Average features extracted: {avg_features:.1f}")
    
    # Pattern recognition accuracy
    if sensory_metrics['pattern_recognition']:
        pattern_counts = {}
        for patterns in sensory_metrics['pattern_recognition']:
            for pattern in patterns:
                pattern_counts[pattern] = pattern_counts.get(pattern, 0) + 1
        
        most_common_pattern = max(pattern_counts.items(), key=lambda x: x[1])
        print(f"   Most recognized pattern: {most_common_pattern[0]} ({most_common_pattern[1]} times)")
    
    # Action success rate
    if action_metrics['actions_taken']:
        successful_actions = sum(1 for action in action_metrics['actions_taken'] 
                               if action in [pynlm.ActionType.Interact, pynlm.ActionType.Eat])
        success_rate = successful_actions / len(action_metrics['actions_taken'])
        print(f"   Action success rate: {success_rate:.1%}")
    
    # Learning progress
    if len(action_metrics['learning_progress']) > 1:
        learning_trend = action_metrics['learning_progress'][-1] - action_metrics['learning_progress'][0]
        print(f"   Learning progress: {learning_trend:+.3f}")
    
    return sensory_metrics, action_metrics

def get_raw_sensory_input(world):
    """Get raw sensory input from world"""
    
    # Simplified raw sensory input
    # In a real system, this would read actual sensor data
    
    body = world.getAgentBody()
    percept = world.getSensoryPercept()
    
    # Extract basic features
    raw_input = [
        body.getX(),
        body.getY(),
        body.getOrientation(),
        body.getEnergy(),
        percept.getFOVWidth(),
        percept.getFOVHeight()
    ]
    
    return raw_input

def extract_sensory_features(raw_input, action, reward):
    """Extract features from sensory input"""
    
    # Simplified feature extraction
    features = []
    
    # Position features
    features.append(('position_x', raw_input[0]))
    features.append(('position_y', raw_input[1]))
    
    # Movement features
    features.append(('orientation', raw_input[2]))
    
    # Energy features
    features.append(('energy', raw_input[3]))
    
    # Vision features
    features.append(('vision_width', raw_input[4]))
    features.append(('vision_height', raw_input[5]))
    
    # Action features
    if action == pynlm.ActionType.MoveForward:
        features.append(('action', 'move_forward'))
    elif action == pynlm.ActionType.TurnLeft:
        features.append(('action', 'turn_left'))
    elif action == pynlm.ActionType.TurnRight:
        features.append(('action', 'turn_right'))
    elif action == pynlm.ActionType.Interact:
        features.append(('action', 'interact'))
    elif action == pynlm.ActionType.Eat:
        features.append(('action', 'eat'))
    
    # Reward features
    features.append(('reward', reward))
    
    return features

def recognize_sensory_patterns(features_history):
    """Recognize patterns in sensory features"""
    
    # Simplified pattern recognition
    patterns = []
    
    # Detect movement patterns
    if any('position_x' in f and abs(f[1]) > 5 for f in features_history[-3:]):
        patterns.append('moving')
    
    # Detect exploration patterns
    if any('action' in f and f[1] == 'interact' for f in features_history):
        patterns.append('exploring')
    
    # Detect feeding patterns
    if any('action' in f and f[1] == 'eat' for f in features_history):
        patterns.append('feeding')
    
    # Detect energy patterns
    energy_values = [f[1] for f in features_history if f[0] == 'energy']
    if energy_values and max(energy_values) > 60:
        patterns.append('high_energy')
    
    return patterns

def make_sensory_decisions(patterns, action, reward):
    """Make decisions based on sensory patterns"""
    
    # Simplified decision making
    decisions = []
    
    # Decide based on patterns
    if 'moving' in patterns and 'high_energy' in patterns:
        decisions.append('continue_moving')
    
    if 'exploring' in patterns:
        decisions.append('seek_new_objects')
    
    if 'feeding' in patterns:
        decisions.append('search_for_resources')
    
    # Decide based on reward
    if reward > 0.5:
        decisions.append('pursue_reward')
    elif reward < -0.5:
        decisions.append('avoid_penalty')
    
    return decisions

def calculate_sensory_reward(world, action):
    """Calculate reward based on sensory processing"""
    
    reward = 0.0
    
    # Energy-based reward
    body = world.getAgentBody()
    if body.getEnergy() > 50:
        reward += 0.2
    
    # Action-based reward
    if action == pynlm.ActionType.Eat:
        reward += 1.0
    elif action == pynlm.ActionType.Interact:
        reward += 0.5
    
    # Environmental reward
    percept = world.getSensoryPercept()
    for obj in percept.getVisibleObjects():
        if obj.getType() == pynlm.WorldObjectType.Resource:
            reward += obj.getValue() * 0.3
        elif obj.getType() == pynlm.WorldObjectType.Hazard:
            reward -= abs(obj.getValue()) * 0.2
    
    return reward

# Example usage
if __name__ == "__main__":
    # Create sensory processing environment
    world = create_sensory_processing_pipeline(brain_width=20, brain_height=20)
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run sensory processing demonstration
    sensory_metrics, action_metrics = run_sensory_processing_demo(
        world, agent, brain, steps=200
    )
    
    print(f"\nSensory Processing Pipeline demonstration complete!")
    print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
    print(f"Agent vision: {world.getVisionWidth()} x {world.getVisionHeight()}")
```

### Analysis

This example demonstrates:

1. **Sensory Transduction**: Raw environmental data is converted to neural signals
2. **Feature Extraction**: High-level features are extracted from sensory input
3. **Pattern Recognition**: Patterns in sensory data are identified
4. **Decision Making**: Actions are chosen based on sensory analysis
5. **Closed-loop Processing**: Perception and action continuously interact

### Extensions

1. **Multi-sensory integration**: Add auditory, tactile, or other senses
2. **Sensory adaptation**: Model sensory adaptation to stimuli
3. **Attention mechanisms**: Implement selective attention
4. **Memory integration**: Connect sensory processing with memory systems

## Summary

The Hands-On Examples section provides practical, ready-to-run demonstrations of NLM capabilities:

1. **Feed-forward networks**: Static input-output processing without learning
2. **Recurrent networks**: Memory and temporal processing capabilities
3. **Multi-region architectures**: Specialized functional modules
4. **Sensory processing pipelines**: Complete perception-action cycles

Each example:

- **Is self-contained** with complete, runnable code
- **Includes clear explanations** of key concepts
- **Provides analysis** of results and insights
- **Suggests extensions** for further exploration
- **Demonstrates core NLM capabilities** in action

These examples serve as:

- **Entry points** for learning NLM
- **Templates** for custom implementations
- **Test cases** for validating NLM functionality
- **Educational tools** for understanding neural systems
- **Reference implementations** for building more complex systems

The examples bridge theory and practice, helping users understand both how NLM works and how to use it effectively.
