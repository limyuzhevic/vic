# NLM Tutorial

**Step-by-step tutorials for learning and using the NLM framework**

## Table of Contents

1. [Beginner Tutorial: Neural Network Creation](#1-beginner-tutorial-neural-network-creation)
2. [Intermediate Tutorial: Agent with World Interaction](#2-intermediate-tutorial-agent-with-world-interaction)
3. [Advanced Tutorial: Custom Experiments](#3-advanced-tutorial-custom-experiments)

## 1. Beginner Tutorial: Neural Network Creation

### Introduction

This tutorial introduces you to creating and working with artificial neural networks in NLM. We'll start with the simplest possible brain and gradually build up complexity.

### What You'll Learn

- How to create a basic neural network
- How neurons fire and communicate
- How to monitor brain activity
- How to save and load brain states

### Step 1: Setting Up Your Environment

First, let's make sure we have NLM installed and can import it:

```python
import pynlm
print("NLM version:", pynlm.__version__)
```

### Step 2: Create Your First Brain

Now, let's create the simplest possible brain with just a few neurons:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create the brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain (this sets up all the neurons and connections)
brain.initialize()

print(f"✅ Created a brain with {brain.getTotalNeuronCount()} neurons!")
print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
print(f"   - Current firing neurons: {brain.getFiringNeuronCount()}")
```

**What just happened?**

1. We created a configuration object - this holds all the settings for our brain
2. We used the configuration to create a brain object
3. We initialized the brain, which actually created all the neurons and connections

### Step 3: Run Your Brain

Now let's make the brain "think" by running it for a few steps:

```python
# Run the brain for 10 steps
print("\n🧠 Running brain simulation...")
for step in range(10):
    brain.step(step)
    print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"\n📊 Simulation Results:")
print(f"   - Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**What you're seeing:**

- **Neurons firing**: Some neurons are activated during the simulation
- **Total spikes**: The brain has generated action potentials
- **Firing rate**: How often neurons are firing on average

### Step 4: Explore Brain Statistics

Let's look at more detailed information about what's happening in our brain:

```python
# Get comprehensive statistics
stats = brain.getStatistics()
print(f"\n📈 Brain Statistics:")
for key, value in stats.items():
    print(f"   - {key}: {value}")
```

### Step 5: Save and Load Your Brain

Brains can be saved and loaded to preserve state:

```python
# Save the brain state
brain.save("my_first_brain.bin")
print(f"\n💾 Brain saved to 'my_first_brain.bin'")

# Create a new brain and load the saved state
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.load("my_first_brain.bin")

print(f"📂 Loaded brain:")
print(f"   - Neurons: {brain2.getTotalNeuronCount()}")
print(f"   - Synapses: {brain2.getTotalSynapseCount()}")
print(f"   - Total spikes: {brain2.getTotalSpikeCount()}")
```

### Step 6: Try Different Configurations

Let's experiment with different brain sizes:

```python
# Test with different neuron counts
sizes = [10, 50, 100]

for size in sizes:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", str(size))
    
    test_brain = pynlm.createBrain(config)
    test_brain.initialize()
    
    print(f"\n🧪 Testing {size} neurons:")
    print(f"   - Configuration: {config.summary()}")
    
    for step in range(5):
        test_brain.step(step)
    
    print(f"   - After 5 steps: {test_brain.getTotalSpikeCount()} spikes")
```

## 2. Intermediate Tutorial: Agent with World Interaction

### Introduction

Now we'll create a more complete system with an agent that interacts with an environment. This introduces the concept of perception and action loops.

### What You'll Learn

- How to create an environment/world
- How agents process sensory input
- How agents produce motor output
- How to implement complete agent-world loops

### Step 1: Create the Basic Components

```python
import pynlm

# 1. Create the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Create the world (environment)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 3. Create the agent (connects brain to world)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("✅ Created brain-world-agent system!")
print(f"   - World size: {world.getWidth()}x{world.getHeight()} cells")
print(f"   - Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")
```

### Step 2: Set Up the World

Let's populate our world with objects:

```python
# Add some objects to the world
# Create agent start position
world.setAgentStart(5, 5)

# Add resources (good things to collect)
for i in range(3):
    x = 2 + i * 3
    y = 2
    resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
    world.addObject(resource)

# Add hazards (bad things to avoid)
for i in range(2):
    x = 15 - i * 3
    y = 15
    hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-1.0)
    world.addObject(hazard)

print(f"\n🌍 World setup complete:")
print(f"   - Agent start: (5, 5)")
print(f"   - Resources: {world.getObjectCount(pynlm.WorldObjectType.Resource)}")
print(f"   - Hazards: {world.getObjectCount(pynlm.WorldObjectType.Hazard)}")
```

### Step 3: Create the Complete Simulation Loop

Here's the core agent-world loop:

```python
def run_complete_simulation(world, agent, brain, num_steps):
    """Run a complete agent-world simulation"""
    for step in range(num_steps):
        # 1. Update the world (time passes)
        world.update(0.1)  # 100ms timestep
        
        # 2. Get what the agent perceives (sensory input)
        percept = world.getSensoryPercept()
        
        # 3. Process what the agent perceives
        agent.processSensoryInput(percept)
        
        # 4. The brain thinks/decides
        brain.step(step)
        
        # 5. The agent produces an action
        action = agent.decodeMotorCommand()
        
        # 6. The action affects the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Monitor progress every 10 steps
        if step % 10 == 0:
            agent_body = world.getAgentBody()
            percept = world.getSensoryPercept()
            
            print(f"\n📍 Step {step}:")
            print(f"   - Position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
            print(f"   - Action: {action.getType()}")
            print(f"   - Energy: {agent_body.energy:.2f}")
            print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"   - Curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return world, agent, brain

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("\n🚀 Starting complete simulation...")
world, agent, brain = run_complete_simulation(world, agent, brain, 50)
```

### Step 4: Monitor Agent Performance

Let's create a more sophisticated monitoring system:

```python
def monitor_agent_performance(world, agent, brain, num_steps):
    """Run simulation with comprehensive performance monitoring"""
    performance_data = {
        'steps': [],
        'positions': [],
        'actions': [],
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'brain_activity': []
    }
    
    for step in range(num_steps):
        # Complete simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect performance data
        agent_body = world.getAgentBody()
        
        performance_data['steps'].append(step)
        performance_data['positions'].append((agent_body.x, agent_body.y))
        performance_data['actions'].append(action.getType())
        performance_data['rewards'].append(result.reward)
        performance_data['curiosity'].append(agent.getCuriosityLevel())
        performance_data['novelty'].append(agent.getNoveltyLevel())
        performance_data['brain_activity'].append({
            'firing': brain.getFiringNeuronCount(),
            'spikes': brain.getTotalSpikeCount(),
            'rate': brain.getAverageFiringRate()
        })
        
        # Print progress
        if step % 10 == 0:
            print(f"Step {step}: Pos({agent_body.x:.1f}, {agent_body.y:.1f}) "
                  f"Action({action.getType()}) "
                  f"Curiosity({agent.getCuriosityLevel():.3f})")
    
    return performance_data

# Run monitored simulation
print("\n📊 Starting performance monitoring...")
perf_data = monitor_agent_performance(world, agent, brain, 100)

# Print summary
print(f"\n📈 Performance Summary:")
print(f"   - Total steps: {len(perf_data['steps'])}")
print(f"   - Final position: {perf_data['positions'][-1]}")
print(f"   - Average curiosity: {sum(perf_data['curiosity'])/len(perf_data['curiosity']):.3f}")
print(f"   - Average novelty: {sum(p['novelty'] for p in perf_data['novelty'])/len(perf_data['novelty']):.3f}")
```

### Step 5: Implement Custom Behavior

Let's make our agent learn to avoid hazards and collect resources:

```python
def run_custom_behavior_simulation(world, agent, brain, num_steps):
    """Simulation with custom agent behavior"""
    hazard_avoidance_count = 0
    resource_collection_count = 0
    
    for step in range(num_steps):
        # Get current state
        agent_body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        # Simple decision logic (in a real agent, this would be learned)
        action = pynlm.ActionType.Wait  # Default action
        
        # Check for nearby hazards
        hazards_nearby = False
        resources_nearby = False
        
        # Simple proximity check (in real implementation, use vision data)
        for i in range(3):  # Check nearby cells
            distance = 1 + i
            if world.isHazardPosition(agent_body.x + distance, agent_body.y + distance):
                hazards_nearby = True
                hazard_avoidance_count += 1
                action = pynlm.ActionType.TurnLeft  # Simple avoidance
                break
            elif world.isResourcePosition(agent_body.x + distance, agent_body.y + distance):
                resources_nearby = True
                resource_collection_count += 1
                action = pynlm.ActionType.Interact  # Collect resource
                break
        
        # If no hazards/resources nearby, move forward
        if not hazards_nearby and not resources_nearby:
            action = pynlm.ActionType.MoveForward
        
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(percept)
        brain.step(step)
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
        if step % 10 == 0:
            # Use curiosity to encourage exploration
            curiosity = agent.getCuriosityLevel()
            agent.applyRewardModulation(curiosity, 0.0)
    
    print(f"\n🎯 Custom Behavior Results:")
    print(f"   - Hazard avoidance actions: {hazard_avoidance_count}")
    print(f"   - Resource collection actions: {resource_collection_count}")
    print(f"   - Total steps: {num_steps}")
    
    return hazard_avoidance_count, resource_collection_count

# Run custom behavior simulation
hazards_avoided, resources_collected = run_custom_behavior_simulation(
    world, agent, brain, 50
)
```

## 3. Advanced Tutorial: Custom Experiments

### Introduction

This advanced tutorial shows you how to create custom experiments with NLM. You'll learn how to:

- Create complex experimental setups
- Design specific research questions
- Collect and analyze experimental data
- Implement multi-agent systems
- Create benchmarking experiments

### Step 1: Design an Experiment: Learning in Changing Environments

Let's create an experiment where an agent must learn to adapt to a changing environment:

```python
import pynlm
import time
import statistics

def create_environment_change_experiment():
    """Create an experiment with changing environmental conditions"""
    # Initialize components
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", "2000")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, world, agent

# Define environmental phases
ENVIRONMENT_PHASES = [
    {
        'name': 'Safe Phase',
        'hazard_density': 0.0,
        'resource_density': 0.3,
        'duration': 50
    },
    {
        'name': 'Hazard Phase', 
        'hazard_density': 0.2,
        'resource_density': 0.1,
        'duration': 50
    },
    {
        'name': 'Resource Rich Phase',
        'hazard_density': 0.1,
        'resource_density': 0.5,
        'duration': 50
    },
    {
        'name': 'Mixed Phase',
        'hazard_density': 0.15,
        'resource_density': 0.3,
        'duration': 50
    }
]
```

### Step 2: Implement the Environment Dynamics

```python
def update_environment_dynamics(world, phase_index, step_in_phase):
    """Update world based on current experimental phase"""
    phase = ENVIRONMENT_PHASES[phase_index]
    
    # Clear existing objects (simplified - in real implementation, maintain state)
    # For demo, we'll just show the concept
    
    # Add hazards based on phase
    hazard_count = int(phase['hazard_density'] * 100)
    for i in range(hazard_count):
        x = (i * 23) % world.getWidth()
        y = (i * 17) % world.getHeight()
        if not world.isValidPosition(x, y):
            continue
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-0.5)
        world.addObject(hazard)
    
    # Add resources based on phase  
    resource_count = int(phase['resource_density'] * 100)
    for i in range(resource_count):
        x = (i * 29) % world.getWidth()
        y = (i * 13 + 5) % world.getHeight()
        if not world.isValidPosition(x, y):
            continue
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(resource)
    
    return phase
```

### Step 3: Run the Complete Experiment

```python
def run_environment_change_experiment():
    """Run the complete environment change experiment"""
    print("🧪 Running Environment Change Experiment")
    print("=" * 50)
    
    # Initialize experiment
    brain, world, agent = create_environment_change_experiment()
    
    # Track experiment metrics
    experiment_data = {
        'phases': [],
        'adaptation_metrics': [],
        'performance_trajectory': [],
        'learning_indicators': []
    }
    
    current_phase = 0
    steps_in_phase = 0
    phase_start_time = time.time()
    
    # Main experiment loop
    for total_step in range(400):  # 400 total steps
        # Update environment phase if needed
        if steps_in_phase >= ENVIRONMENT_PHASES[current_phase]['duration']:
            current_phase = (current_phase + 1) % len(ENVIRONMENT_PHASES)
            steps_in_phase = 0
            print(f"\n🔄 Phase Changed to: {ENVIRONMENT_PHASES[current_phase]['name']}")
        
        # Update environment (simplified for demo)
        update_environment_dynamics(world, current_phase, steps_in_phase)
        
        # Run agent in environment
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(total_step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect data
        agent_body = world.getAgentBody()
        experiment_data['phases'].append(current_phase)
        experiment_data['performance_trajectory'].append({
            'step': total_step,
            'phase': current_phase,
            'position': (agent_body.x, agent_body.y),
            'energy': agent_body.energy,
            'action': action.getType(),
            'reward': result.reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'brain_activity': {
                'firing': brain.getFiringNeuronCount(),
                'rate': brain.getAverageFiringRate()
            }
        })
        
        # Apply learning
        if total_step % 5 == 0:
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Update development
        if total_step % 20 == 0:
            agent.updateDevelopment(0.1)
        
        steps_in_phase += 1
        
        # Print progress
        if total_step % 50 == 0:
            phase_name = ENVIRONMENT_PHASES[current_phase]['name']
            print(f"Step {total_step} ({phase_name}): "
                  f"Pos({agent_body.x:.1f}, {agent_body.y:.1f}) "
                  f"Curiosity({agent.getCuriosityLevel():.3f}) "
                  f"Action({action.getType()})")
    
    # Analyze experiment results
    analyze_experiment_results(experiment_data)
    
    return experiment_data

# Run the experiment
experiment_results = run_environment_change_experiment()
```

### Step 4: Analyze Experimental Results

```python
def analyze_experiment_results(data):
    """Analyze and display experiment results"""
    print(f"\n📊 Experiment Analysis")
    print("=" * 30)
    
    phases = data['phases']
    trajectory = data['performance_trajectory']
    
    # Analyze by phase
    for phase_idx in range(len(ENVIRONMENT_PHASES)):
        phase_name = ENVIRONMENT_PHASES[phase_idx]['name']
        phase_data = [d for d in trajectory if d['phase'] == phase_idx]
        
        if not phase_data:
            continue
            
        # Calculate metrics for this phase
        positions = [d['position'] for d in phase_data]
        energies = [d['energy'] for d in phase_data]
        curiosities = [d['curiosity'] for d in phase_data]
        action_counts = {}
        
        for d in phase_data:
            action = d['action']
            action_counts[action] = action_counts.get(action, 0) + 1
        
        # Display phase results
        print(f"\n📍 Phase: {phase_name}")
        print(f"   - Duration: {len(phase_data)} steps")
        print(f"   - Energy range: {min(energies):.2f} - {max(energies):.2f}")
        print(f"   - Average curiosity: {statistics.mean(curiosities):.3f}")
        print(f"   - Actions: {action_counts}")
        
        # Calculate final position relative to start
        if len(positions) >= 2:
            start_x, start_y = positions[0]
            final_x, final_y = positions[-1]
            displacement = ((final_x - start_x)**2 + (final_y - start_y)**2)**0.5
            print(f"   - Final displacement from start: {displacement:.2f} cells")
    
    # Overall experiment summary
    print(f"\n🎯 Overall Experiment Summary")
    print(f"   - Total steps: {len(trajectory)}")
    print(f"   - Final energy: {trajectory[-1]['energy']:.2f}")
    print(f"   - Average curiosity: {statistics.mean(d['curiosity'] for d in trajectory):.3f}")
    print(f"   - Average novelty: {statistics.mean(d['novelty'] for d in trajectory):.3f}")
    
    # Adaptation indicator (change in curiosity over time)
    if len(trajectory) >= 10:
        first_curiosity = statistics.mean(d['curiosity'] for d in trajectory[:10])
        last_curiosity = statistics.mean(d['curiosity'] for d in trajectory[-10:])
        curiosity_change = abs(last_curiosity - first_curiosity)
        print(f"   - Curiosity change (adaptation indicator): {curiosity_change:.3f}")
```

### Step 5: Create Multi-Agent Experiment

```python
def create_multi_agent_experiment():
    """Create an experiment with multiple interacting agents"""
    print("\n👥 Creating Multi-Agent Experiment")
    print("=" * 40)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(3):
        # Create agent with different configurations
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", "1000")
        
        brain = pynlm.createBrain(config)
        brains.append(brain)
        
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
        
        # Place agents at different starting positions
        start_x = 5 + i * 15
        start_y = 5 + i * 10
        world.setAgentStart(start_x, start_y)
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    # Add resources for agents to compete for
    for i in range(10):
        x = (i * 7) % 45 + 2
        y = (i * 11 + 3) % 45 + 2
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(resource)
    
    # Add obstacles
    for x in range(20, 30):
        for y in [20, 21, 22]:
            wall = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, value=0.0)
            world.addObject(wall)
    
    return brains, agents, world

# Run multi-agent experiment
brains, agents, world = create_multi_agent_experiment()
print(f"Created {len(agents)} agents in shared world")

# Run simulation with all agents
for step in range(100):
    world.update(0.1)
    
    # Each agent perceives and acts (simplified - in reality, they'd perceive different things)
    for i, agent in enumerate(agents):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Simple agent-specific behavior
        if step % 10 == i:  # Different behavior patterns
            action = pynlm.ActionType.TurnLeft
        elif step % 15 == i:  # Another pattern
            action = pynlm.ActionType.MoveForward
        else:
            action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning for each agent
    for i, agent in enumerate(agents):
        if step % 20 == 0:
            agent.applyRewardModulation(0.1, 0.0)  # Small reward for exploration
        if step % 25 == 0:
            agent.updateDevelopment(0.1)

print("\n✅ Multi-agent simulation completed!")
```

## Summary and Next Steps

### What You've Learned

1. **Beginner Level**: Created and ran simple neural networks, saved/loaded brain states, explored configuration options
2. **Intermediate Level**: Built complete agent-world systems, implemented simulation loops, monitored performance, created custom behavior
3. **Advanced Level**: Designed and ran complex experiments, analyzed experimental data, created multi-agent systems

### Next Steps

1. **Read the API Reference** ([API_REFERENCE.md](API_REFERENCE.md)) for complete technical documentation
2. **Explore EXAMPLES.md** for complete working examples
3. **Study ARCHITECTURE.md** for deep technical understanding
4. **Try the existing experiments** in the NLM repository
5. **Create your own research questions** and design experiments

### Practice Exercises

1. **Exercise 1**: Modify the beginner tutorial to create a brain with 10,000 neurons
2. **Exercise 2**: Create a new agent behavior (e.g., avoid walls, collect resources efficiently)
3. **Exercise 3**: Design an experiment testing learning speed with different brain sizes
4. **Exercise 4**: Implement multi-agent competition or cooperation
5. **Exercise 5**: Create a visualization of agent activity over time

### Key Concepts to Remember

- **Configuration**: Brain behavior is controlled through configuration files
- **Simulation Loop**: World → Agent → Brain → Action → World
- **Learning**: Neuromodulation (reward, curiosity, novelty) drives learning
- **Development**: Brain changes and matures over time
- **Memory**: Episodic, working, and associative memory systems
- **Attention**: Competitive selection of neural activity
- **Prediction**: Temporal sequence learning and consequence prediction

This concludes the NLM Tutorial. You're now ready to:

1. ✅ **Understand** the NLM framework architecture and components
2. ✅ **Create** brains, agents, and worlds
3. ✅ **Run** simulations and monitor performance
4. ✅ **Design** and run experiments
5. ✅ **Analyze** experimental results

Welcome to the world of artificial developmental brains! 🎉