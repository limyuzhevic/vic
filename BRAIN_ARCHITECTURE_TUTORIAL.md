# NLM Brain Architecture Tutorial

## Welcome to the NLM Brain Architecture Tutorial

This tutorial will guide you through understanding the NLM (Neural Learning Machine) brain architecture, one of the most advanced artificial developmental brain systems ever created. We'll explain how all the different components work together to create a cohesive, learning brain that can adapt, remember, and make decisions.

---

## Module 1: Understanding the Brain as a System

### 1.1 What Makes NLM Different from Traditional AI

Traditional AI systems typically use:

- **Static neural networks** (fixed architectures)
- **Pre-trained models** (learning happens offline)
- **Backpropagation** (gradient-based learning)
- **Centralized planning** (all decisions made in one place)

NLM uses a completely different approach:

- **Developmental architectures** (brains that grow and change)
- **Online learning** (continuous learning during operation)
- **Biologically inspired plasticity** (STDP, Hebbian learning, structural changes)
- **Distributed processing** (multiple specialized subsystems working together)

### 1.2 The NLM Philosophy

NLM is based on the principle that intelligence emerges from the interaction of many specialized subsystems working together, much like a biological brain. Rather than trying to create a single "super intelligent" system, NLM creates a collection of specialized components that interact to produce complex behavior.

### 1.3 Key Architectural Principles

| Principle | Description | Example |
|-----------|-------------|----------|
| **Modular Design** | Separate, specialized subsystems | Working memory, episodic memory, prediction systems |
| **Emergent Behavior** | Complex behaviors arise from simple interactions | Curiosity drives exploration, leading to discovery |
| **Closed-Loop Processing** | Input → Processing → Action → Feedback → Learning | World sensing → Neural processing → Action → Reward → Plasticity |
| **Developmental Trajectory** | Brains change and mature over time | Critical periods, maturation, aging effects |
| **Multiple Memory Systems** | Different types of memory for different purposes | Working memory (active), episodic memory (experiences), associative memory (patterns) |

---

## Module 2: Core Brain Architecture Overview

### 2.1 The Main Brain Loop

The NLM brain operates as a continuous closed-loop system:

```
                ┌───────────────────────┐
                │    WORLD (Environment)   │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │      SENSORY INPUT       │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │      NEURAL PROCESSING    │
                │   (LIF Dynamics, Spikes)  │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │     INTERNAL STATE        │
                │ (Working Memory, Attention)│
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │     MEMORY / PREDICTION    │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │   MOTIVATION / NEUROMODULATION │
                │    (Dopamine, Curiosity)    │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │      ACTION SELECTION     │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │       MOTOR OUTPUT       │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │      WORLD CONSEQUENCE    │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │     REWARD / SURPRISE /   │
                │        ERROR SIGNALS      │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │     PLASTICITY (Learning)  │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │      MEMORY / DEVELOPMENT │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │      CHANGED BRAIN        │
                └─────────────┬─────────────┘
                              │
                ┌─────────────▼─────────────┐
                │    CHANGED FUTURE BEHAVIOR │
                └───────────────────────────┘
```

### 2.2 Major Architectural Components

The NLM brain consists of several major subsystems:

#### A. Core Neural Processing
- **LIF Neurons**: Biologically plausible neuron model with spiking dynamics
- **Synaptic Connections**: Dynamic connections that strengthen/weak
- **Event-Driven Processing**: Efficient spike-based computation

#### B. Memory Systems
- **Working Memory**: Short-term storage with competition
- **Episodic Memory**: Long-term experience storage with replay
- **Associative Memory**: Pattern completion and association

#### C. Neuromodulation
- **Dopamine**: Reward prediction error, reinforcement learning
- **Curiosity**: Novelty detection, intrinsic motivation
- **Novelty Detection**: Surprise signals, exploratory behavior

#### D. Prediction Systems
- **Forward Models**: Predicting future states
- **Prediction Error**: Learning from mismatches
- **Confidence Tracking**: Uncertainty quantification

#### E. Cognitive Systems
- **Neural Planning**: Action sequence evaluation
- **Concept Formation**: Pattern abstraction and categorization
- **Attention**: Competitive selection of information

#### F. Developmental Systems
- **Developmental Stages**: Initial → Critical → Maturation → Adult → Aging
- **Structural Plasticity**: Network growth and pruning
- **Maturation**: Age-dependent changes in brain properties

#### G. World Interface
- **SimpleWorld**: 2D environment with physics simulation
- **Sensory Processing**: Converting world state to neural input
- **Motor Output**: Converting neural activity to actions

---

## Module 3: Step-by-Step Brain Architecture Understanding

### 3.1 Starting with the Neural Foundation

Let's begin by understanding the simplest part of the brain: the neural network itself.

```python
import pynlm

# Step 1: Create the basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Understand what we have
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**What we've learned:** The brain is essentially a network of spiking neurons connected by synapses. These neurons communicate through electrical spikes, and the connections between them can change through learning.

### 3.2 Adding the World Interface

Now let's connect our brain to a world where it can act and learn.

```python
# Step 3: Create the world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Step 4: Create the agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 5: Enable key subsystems
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore novel things
agent.enableDevelopment(True)       # Brain matures over time
```

**What we've learned:** The brain needs a world to interact with and an agent to bridge the two. The agent handles sensory input (seeing the world) and motor output (acting in the world), while the brain does the internal processing.

### 3.3 The Complete Learning Loop

Here's the complete architecture in action:

```python
# Run a complete learning simulation
for step in range(500):
    # 1. Environment update (world physics, time passes)
    world.update(0.1)
    
    # 2. Perception (get what the agent sees)
    percept = world.getSensoryPercept()
    
    # 3. Sensation (send to brain for processing)
    agent.processSensoryInput(percept)
    
    # 4. Neural processing (brain thinks)
    brain.step(step)
    
    # 5. Motor planning (brain decides what to do)
    action = agent.decodeMotorCommand()
    
    # 6. Action execution (do the action in world)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Reward calculation (get feedback)
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    
    # 8. Learning (update connections based on reward)
    agent.applyRewardModulation(reward, 0.0)
    
    # 9. Development (brain matures)
    agent.updateDevelopment(0.1)
    
    # Optional: Log progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")
```

**What we've learned:** The brain doesn't work in isolation. It's part of a complete system where:

- The world provides the context and consequences
- The agent translates between brain and world
- The brain processes information and makes decisions
- The environment provides feedback through rewards

### 3.4 Understanding Developmental Stages

NLM brains develop over time, going through different stages:

```python
# Track developmental progression
development_stages = []
for step in range(300):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    development_stages.append(stage)
    
    if step % 100 == 0:
        print(f"Step {step}: {stage.name} stage")

# The stages typically progress:
# Initial → Critical Period → Maturation → Adult → Aging
```

**What we've learned:** Brains don't just start working fully formed. They develop through different stages, each with different learning capabilities and behaviors.

### 3.5 Exploring Memory Systems

Let's look at how different memory systems work:

```python
# Create brain with all memory systems
config = pynlm.createDefaultConfig()
config.set("memory.working.capacity", 200)
config.set("memory.episodic.enable", True)
config.set("memory.associative.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Test memory capacity
print(f"Working memory capacity: {brain.getWorkingMemoryCapacity()}")
print(f"Episodic memory episodes: {brain.getEpisodicMemoryCount()}")
print(f"Associative memory patterns: {brain.getAssociativeMemoryCount()}")
```

**What we've learned:** Different types of memory serve different purposes:

- **Working memory**: Active processing, short-term storage
- **Episodic memory**: Long-term storage of experiences
- **Associative memory**: Pattern completion and association

### 3.6 Neuromodulation in Action

Neuromodulators are like the brain's chemical messengers:

```python
# Check neuromodulation levels
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation strength: {agent.getNeuromodulationLevel():.3f}")

# Curiosity drives exploration
if agent.getCuriosityLevel() < 0.3:
    print("Agent needs more exploration!")
    
# Novelty detection
if agent.getNoveltyLevel() > 0.7:
    print("Agent detected something novel and exciting!")
```

**What we've learned:** Neuromodulators like dopamine, curiosity, and novelty detection are crucial for learning and adaptive behavior. They tell the brain when something is important, novel, or rewarding.

---

## Module 4: Advanced Architecture Patterns

### 4.1 Pattern: Multi-Agent Coordination

Sometimes you want multiple brains working together:

```python
# Create multiple agents
agents = []
for i in range(3):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agents.append(agent)

# Coordinated behavior
for step in range(300):
    world.update(0.1)
    
    # Each agent perceives the world
    percepts = [agent.getSensoryPercept() for agent in agents]
    
    # Each agent processes independently
    for i, agent in enumerate(agents):
        agent.processSensoryInput(percepts[i])
    
    # Each brain thinks
    for i, agent in enumerate(agents):
        # Access brain through agent
        agent.decodeMotorCommand()
    
    # World state affects all agents equally
    action = agents[0].decodeMotorCommand()  # All agents see same action
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 4.2 Pattern: Hierarchical Control

Create agents at different levels of abstraction:

```python
class HierarchicalController:
    def __init__(self):
        # Low-level controller (reflexes)
        self.low_level_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.low_level_brain.initialize()
        self.low_level_agent = pynlm.createAgentBrain(self.low_level_brain)
        
        # High-level controller (goals)
        self.high_level_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.high_level_brain.initialize()
        self.high_level_agent = pynlm.createAgentBrain(self.high_level_brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30)
        self.world.reset()
        
        # Initialize both agents
        self.low_level_agent.initialize(self.world)
        self.high_level_agent.initialize(self.world)
    
    def run(self):
        for step in range(500):
            self.world.update(0.1)
            
            # High-level agent sets goals
            high_action = self.high_level_agent.decodeMotorCommand()
            
            # Low-level agent executes with modifications
            low_action = self.modify_for_environment(high_action)
            
            # Both perceive
            self.high_level_agent.processSensoryInput(self.world.getSensoryPercept())
            self.low_level_agent.processSensoryInput(self.world.getSensoryPercept())
            
            # Both think
            self.high_level_brain.step(step)
            self.low_level_brain.step(step)
            
            # World updates based on combined actions
            self.world.applyMotorCommand(low_action, self.world.getSimulationTime())
    
    def modify_for_environment(self, action):
        # Add reflexes, safety checks, etc.
        return action
```

### 4.3 Pattern: Developmental Pipeline

Train agents through different developmental stages:

```python
class DevelopmentalPipeline:
    def __init__(self):
        self.stages = {
            'initial': {'neurons': 500, 'plasticity': 2.0},
            'critical': {'neurons': 1000, 'plasticity': 5.0},
            'maturation': {'neurons': 2000, 'plasticity': 1.5},
            'adult': {'neurons': 3000, 'plasticity': 1.0}
        }
        
        self.current_stage = 'initial'
        self.stage_step = 0
        self.stage_steps = {}
        
        for stage_name, params in self.stages.items():
            self.stage_steps[stage_name] = 100  # Steps per stage
    
    def run_pipeline(self, total_steps=1000):
        for step in range(total_steps):
            # Determine current stage
            current_stage = self.get_current_stage(step)
            
            # Create brain with stage-specific configuration
            config = pynlm.createDefaultConfig()
            stage_params = self.stages[current_stage]
            
            config.set("brain.neuron_count", stage_params['neurons'])
            config.set("brain.development.plasticity_multiplier", stage_params['plasticity'])
            
            # Create agent
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Run stage
            self.run_stage(agent, brain, current_stage, step)
    
    def get_current_stage(self, step):
        # Simple stage progression
        if step < 100:
            return 'initial'
        elif step < 300:
            return 'critical'
        elif step < 700:
            return 'maturation'
        else:
            return 'adult'
```

---

## Module 5: Architecture Debugging and Analysis

### 5.1 Monitoring Brain Health

Track the brain's state over time:

```python
class BrainMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.history = {
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'development_stages': [],
            'memory_usage': []
        }
    
    def record_step(self, step):
        """Record state at current step."""
        self.history['firing_rates'].append(self.brain.getAverageFiringRate())
        self.history['curiosity_levels'].append(self.agent.getCuriosityLevel())
        self.history['novelty_levels'].append(self.agent.getNoveltyLevel())
        self.history['development_stages'].append(self.brain.getDevelopmentalStage().name)
        self.history['memory_usage'].append({
            'working': self.brain.getWorkingMemoryCount(),
            'episodic': self.brain.getEpisodicMemoryCount(),
            'associative': self.brain.getAssociativeMemoryCount()
        })
    
    def analyze_health(self):
        """Analyze brain health and report issues."""
        issues = []
        
        # Check firing rate
        avg_firing = sum(self.history['firing_rates']) / len(self.history['firing_rates'])
        if avg_firing < 0.1:
            issues.append("Very low firing rate - brain may be inactive")
        elif avg_firing > 100:
            issues.append("Very high firing rate - possible instability")
        
        # Check development
        final_stage = self.history['development_stages'][-1]
        if final_stage == 'initial':
            issues.append("Brain stuck in initial development stage")
        
        # Check memory growth
        if self.history['memory_usage'][-1]['working'] < 10:
            issues.append("Working memory may not be developing properly")
        
        return issues
    
    def plot_health(self):
        """Generate health visualization."""
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Firing rate
        axes[0, 0].plot(self.history['firing_rates'])
        axes[0, 0].set_title('Firing Rate Over Time')
        axes[0, 0].set_ylabel('Hz')
        
        # Curiosity
        axes[0, 1].plot(self.history['curiosity_levels'])
        axes[0, 1].set_title('Curiosity Level Over Time')
        axes[0, 1].set_ylabel('Level (0-1)')
        
        # Novelty
        axes[1, 0].plot(self.history['novelty_levels'])
        axes[1, 0].set_title('Novelty Detection Over Time')
        axes[1, 0].set_ylabel('Level (0-1)')
        
        # Memory usage
        working_mem = [m['working'] for m in self.history['memory_usage']]
        episodic_mem = [m['episodic'] for m in self.history['memory_usage']]
        associative_mem = [m['associative'] for m in self.history['memory_usage']]
        
        axes[1, 1].plot(working_mem, label='Working')
        axes[1, 1].plot(episodic_mem, label='Episodic')
        axes[1, 1].plot(associative_mem, label='Associative')
        axes[1, 1].set_title('Memory System Usage')
        axes[1, 1].set_ylabel('Number of Items')
        axes[1, 1].legend()
        
        plt.tight_layout()
        plt.savefig('brain_health_analysis.png')
        plt.show()
```

### 5.2 Architecture Optimization

Optimize brain for specific tasks:

```python
class ArchitectureOptimizer:
    def __init__(self):
        self.objectives = []
        self.constraints = []
    
    def optimize_for_task(self, task_type: str, resource_limit: int):
        """Create optimized architecture for specific task."""
        
        if task_type == "fast_reflexes":
            # Optimize for quick reactions
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 500)  # Small brain, fast
            config.set("brain.v_thresh", -40)   # Lower threshold, easier to fire
            config.set("world.width", 10)       # Small world, fast updates
            config.set("world.height", 10)
            return config
            
        elif task_type == "complex_planning":
            # Optimize for complex decision making
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 3000)  # Large brain, more processing
            config.set("plasticity.stdp.learning_rate", 0.005)  # High learning
            config.set("memory.episodic.capacity", 200)  # More memory
            config.set("world.width", 40)  # Large world for exploration
            config.set("world.height", 40)
            return config
            
        elif task_type == "efficient_learning":
            # Optimize for learning efficiency
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1500)  # Medium size
            config.set("brain.development.critical_period", 300)  # Long critical period
            config.set("neuromod.curiosity.sensitivity", 2.0)  # High curiosity
            config.set("plasticity.structural.enable", True)  # Network growth
            return config
        
        # Apply constraints
        self.apply_constraints(config, resource_limit)
        return config
    
    def apply_constraints(self, config, resource_limit: int):
        """Apply resource constraints to configuration."""
        neurons = config.getInt("brain.neuron_count")
        
        if neurons > resource_limit:
            # Scale down brain size
            scale_factor = resource_limit / neurons
            config.set("brain.neuron_count", resource_limit)
            config.set("brain.synapse_density", 
                      config.getFloat("brain.synapse_density") * scale_factor)
        
        # Ensure minimum resources
        if neurons < 100:
            config.set("brain.neuron_count", 100)
```

---

## Module 6: Advanced Architecture Patterns

### 6.1 Pattern: Evolutionary Architecture

Let architectures evolve over time:

```python
class EvolutionaryArchitecture:
    def __init__(self, population_size=10):
        self.population_size = population_size
        self.population = []
        self.generation = 0
        
        # Initialize population
        for _ in range(population_size):
            self.population.append(self.create_random_architecture())
    
    def create_random_architecture(self):
        """Create a random architecture."""
        config = pynlm.createDefaultConfig()
        
        # Random parameters
        config.set("brain.neuron_count", np.random.randint(500, 3000))
        config.set("brain.v_thresh", np.random.uniform(-60, -40))
        config.set("plasticity.stdp.learning_rate", np.random.uniform(0.0001, 0.01))
        config.set("neuromod.curiosity.sensitivity", np.random.uniform(0.5, 3.0))
        config.set("world.width", np.random.randint(10, 40))
        config.set("world.height", np.random.randint(10, 40))
        
        return config
    
    def evolve_generation(self, fitness_function):
        """Evolve next generation based on fitness."""
        # Evaluate current population
        fitness_scores = []
        for config in self.population:
            fitness_scores.append(fitness_function(config))
        
        # Select parents
        parents = self.select_parents(fitness_scores)
        
        # Create next generation
        next_generation = []
        for _ in range(self.population_size):
            parent1, parent2 = np.random.choice(parents, 2, replace=False)
            child = self.crossover(parent1, parent2)
            child = self.mutate(child)
            next_generation.append(child)
        
        self.population = next_generation
        self.generation += 1
        
        return self.population
    
    def select_parents(self, fitness_scores, num_parents=5):
        """Select parents based on fitness."""
        total_fitness = sum(fitness_scores)
        probabilities = [f / total_fitness for f in fitness_scores]
        return np.random.choice(self.population, num_parents, replace=False, p=probabilities)
    
    def crossover(self, parent1, parent2):
        """Crossover two parent configurations."""
        child = pynlm.createDefaultConfig()
        
        # Crossover brain parameters
        child.set("brain.neuron_count", 
                 np.random.choice([parent1.getInt("brain.neuron_count"), 
                                 parent2.getInt("brain.neuron_count")]))
        
        # Crossover world parameters
        child.set("world.width", 
                 np.random.choice([parent1.getInt("world.width"), 
                                 parent2.getInt("world.width")]))
        child.set("world.height", 
                 np.random.choice([parent1.getInt("world.height"), 
                                 parent2.getInt("world.height")]))
        
        # Inherit other parameters from fitter parent
        fittest_parent = parent1 if parent1.getInt("brain.neuron_count") > parent2.getInt("brain.neuron_count") else parent2
        
        for key in fittest_parent.getKeys():
            if key not in ["brain.neuron_count", "world.width", "world.height"]:
                child.set(key, fittest_parent.getString(key))
        
        return child
    
    def mutate(self, config, mutation_rate=0.1):
        """Apply mutations to configuration."""
        for key in config.getKeys():
            if np.random.random() < mutation_rate:
                value = config.getString(key)
                
                # Apply different mutation strategies
                if "neuron_count" in key:
                    config.set(key, np.random.randint(100, 5000))
                elif "v_thresh" in key:
                    config.set(key, np.random.uniform(-80, -30))
                elif "learning_rate" in key:
                    config.set(key, np.random.uniform(0.0001, 0.01))
                elif "curiosity" in key:
                    config.set(key, np.random.uniform(0.1, 5.0))
                elif "width" in key or "height" in key:
                    config.set(key, np.random.randint(10, 50))
        
        return config
```

### 6.2 Pattern: Transfer Learning Between Architectures

Share knowledge between different architectures:

```python
class ArchitectureTransfer:
    def __init__(self):
        self.architecture_bank = {}
        self.knowledge_base = {}
    
    def train_architecture(self, architecture_id, training_data):
        """Train an architecture and store its knowledge."""
        config = self.architecture_bank[architecture_id]
        
        # Train the architecture
        results = self.train_with_data(config, training_data)
        
        # Extract learned features
        learned_features = self.extract_features(results)
        
        # Store in knowledge base
        self.knowledge_base[architecture_id] = {
            'config': config,
            'features': learned_features,
            'performance': results['performance'],
            'parameters': results['parameters']
        }
        
        return results
    
    def transfer_knowledge(self, source_id, target_id, transfer_strength=0.5):
        """Transfer knowledge from one architecture to another."""
        if source_id not in self.knowledge_base:
            raise ValueError(f"Source architecture {source_id} not found")
        
        if target_id not in self.architecture_bank:
            raise ValueError(f"Target architecture {target_id} not found")
        
        source_knowledge = self.knowledge_base[source_id]
        target_config = self.architecture_bank[target_id]
        
        # Transfer key parameters based on performance
        for param_key, source_value in source_knowledge['parameters'].items():
            if param_key in target_config.getKeys():
                # Blend with existing value
                target_value = target_config.getString(param_key)
                blended_value = self.blend_values(source_value, target_value, transfer_strength)
                target_config.set(param_key, blended_value)
    
    def blend_values(self, source_value, target_value, strength):
        """Blend source and target values."""
        if isinstance(source_value, (int, float)) and isinstance(target_value, (int, float)):
            return source_value * strength + target_value * (1 - strength)
        elif isinstance(source_value, bool) and isinstance(target_value, bool):
            return source_value if np.random.random() < strength else target_value
        else:
            # Default to target for other types
            return target_value
```

---

## Module 7: Architecture Analysis and Visualization

### 7.1 Architecture Visualization

Create visual representations of brain architectures:

```python
class ArchitectureVisualizer:
    def __init__(self, config):
        self.config = config
    
    def create_neural_network_graph(self):
        """Create a visual graph of the neural network."""
        import networkx as nx
        
        # Get brain parameters
        neuron_count = self.config.getInt("brain.neuron_count")
        synapse_density = self.config.getFloat("brain.synapse_density")
        
        # Create graph
        G = nx.erdos_renyi_graph(n=neuron_count, p=synapse_density)
        
        # Add neuron properties
        for node in G.nodes():
            G.nodes[node]['type'] = 'neuron'
            G.nodes[node]['layer'] = self.determine_layer(node, neuron_count)
        
        # Add synapse properties
        for edge in G.edges():
            G.edges[edge]['type'] = 'synapse'
            G.edges[edge]['weight'] = np.random.uniform(0, 1)
        
        return G
    
    def determine_layer(self, neuron_id, total_neurons):
        """Determine which layer a neuron belongs to."""
        if neuron_id < total_neurons * 0.3:
            return 'input'
        elif neuron_id < total_neurons * 0.7:
            return 'hidden'
        else:
            return 'output'
    
    def plot_architecture(self, save_path=None):
        """Create and save architecture visualization."""
        G = self.create_neural_network_graph()
        
        # Create layout
        pos = nx.spring_layout(G, k=1, iterations=50)
        
        # Draw graph
        import matplotlib.pyplot as plt
        
        plt.figure(figsize=(12, 8))
        
        # Draw nodes by layer
        layers = ['input', 'hidden', 'output']
        colors = ['lightblue', 'lightgreen', 'lightcoral']
        
        for layer in layers:
            layer_nodes = [n for n, attr in G.nodes(data=True) 
                          if attr['layer'] == layer]
            layer_pos = [pos[n] for n in layer_nodes]
            
            nx.draw_networkx_nodes(G, pos, nodelist=layer_nodes, 
                                  node_color=colors[layers.index(layer)],
                                  node_size=500, alpha=0.8)
        
        # Draw edges
        nx.draw_networkx_edges(G, pos, alpha=0.3, width=0.5)
        
        # Add labels
        nx.draw_networkx_labels(G, pos, font_size=8)
        
        plt.title("NLM Brain Architecture Visualization")
        plt.axis('off')
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        plt.show()
    
    def create_system_architecture_diagram(self):
        """Create system architecture diagram showing all components."""
        import matplotlib.pyplot as plt
        import matplotlib.patches as mpatches
        
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        fig.suptitle("NLM Brain Architecture System Overview", fontsize=16)
        
        # Component diagrams
        components = [
            ('Neural Processing', 'brain/'),
            ('Memory Systems', 'memory/'),
            ('Neuromodulation', 'neuromod/'),
            ('Prediction', 'prediction/'),
            ('Cognition', 'cognition/'),
            ('Development', 'development/')
        ]
        
        for idx, (name, icon) in enumerate(components):
            row, col = divmod(idx, 3)
            ax = axes[row, col]
            
            # Draw component box
            box = mpatches.Rectangle((0.1, 0.1), 0.8, 0.8, 
                                   linewidth=2, edgecolor='blue', 
                                   facecolor='lightblue', alpha=0.7)
            ax.add_patch(box)
            
            # Add component name
            ax.text(0.5, 0.9, name, ha='center', va='top', fontweight='bold', fontsize=12)
            
            # Add icon (simplified)
            ax.text(0.5, 0.5, icon, ha='center', va='center', fontsize=20)
            
            # Remove axes
            ax.set_xlim(0, 1)
            ax.set_ylim(0, 1)
            ax.axis('off')
        
        plt.tight_layout()
        plt.savefig('system_architecture.png', dpi=300, bbox_inches='tight')
        plt.show()
```

### 7.2 Architecture Metrics and Evaluation

Evaluate architecture performance:

```python
class ArchitectureEvaluator:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.metrics = {}
    
    def run_evaluation(self, tasks, num_trials=10):
        """Evaluate architecture on multiple tasks."""
        results = {}
        
        for task_name, task_config in tasks.items():
            print(f"Evaluating {task_name}...")
            
            task_results = {
                'accuracy': [],
                'efficiency': [],
                'learning_speed': [],
                'stability': []
            }
            
            for trial in range(num_trials):
                # Create fresh configuration for trial
                config = self.create_task_config(task_config)
                
                # Create and run agent
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(self.world)
                
                # Run trial
                trial_results = self.run_task_trial(agent, brain, task_config)
                
                # Collect metrics
                task_results['accuracy'].append(trial_results['accuracy'])
                task_results['efficiency'].append(trial_results['efficiency'])
                task_results['learning_speed'].append(trial_results['learning_speed'])
                task_results['stability'].append(trial_results['stability'])
            
            # Calculate averages
            results[task_name] = {
                'accuracy_mean': np.mean(task_results['accuracy']),
                'accuracy_std': np.std(task_results['accuracy']),
                'efficiency_mean': np.mean(task_results['efficiency']),
                'efficiency_std': np.std(task_results['efficiency']),
                'learning_speed_mean': np.mean(task_results['learning_speed']),
                'learning_speed_std': np.std(task_results['learning_speed']),
                'stability_mean': np.mean(task_results['stability']),
                'stability_std': np.std(task_results['stability'])
            }
        
        self.metrics = results
        return results
    
    def create_task_config(self, task_config):
        """Create configuration for specific task."""
        config = pynlm.createDefaultConfig()
        
        # Apply task-specific parameters
        for param, value in task_config.get('brain', {}).items():
            config.set(f"brain.{param}", value)
        
        for param, value in task_config.get('world', {}).items():
            config.set(f"world.{param}", value)
        
        for param, value in task_config.get('plasticity', {}).items():
            config.set(f"plasticity.{param}", value)
        
        return config
    
    def run_task_trial(self, agent, brain, task_config):
        """Run a single trial for a task."""
        # Reset world
        self.world.reset()
        
        # Run simulation
        results = {
            'accuracy': 0.0,
            'efficiency': 0.0,
            'learning_speed': 0.0,
            'stability': 0.0,
            'step_data': []
        }
        
        for step in range(task_config.get('steps', 500)):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process and act
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect data
            results['step_data'].append({
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'reward': percept.getInternal()[0] if percept.getInternal() else 0.0
            })
        
        # Calculate metrics
        results['accuracy'] = self.calculate_accuracy(results['step_data'])
        results['efficiency'] = self.calculate_efficiency(results['step_data'])
        results['learning_speed'] = self.calculate_learning_speed(results['step_data'])
        results['stability'] = self.calculate_stability(results['step_data'])
        
        return results
    
    def calculate_accuracy(self, step_data):
        """Calculate task accuracy from step data."""
        # Example accuracy calculation
        # This depends on the specific task being evaluated
        final_reward = step_data[-1]['reward']
        avg_reward = np.mean([d['reward'] for d in step_data])
        
        return (final_reward + avg_reward) / 2
    
    def calculate_efficiency(self, step_data):
        """Calculate computational efficiency."""
        # Efficiency = reward per step
        total_reward = sum(d['reward'] for d in step_data)
        return total_reward / len(step_data)
    
    def calculate_learning_speed(self, step_data):
        """Calculate how quickly the agent learns."""
        # Learning speed = how fast reward increases
        rewards = [d['reward'] for d in step_data]
        
        if len(rewards) < 10:
            return 0.0
        
        # Calculate correlation between step number and reward
        steps = list(range(len(rewards)))
        correlation = np.corrcoef(steps, rewards)[0, 1]
        return max(0, correlation)  # Ensure non-negative
    
    def calculate_stability(self, step_data):
        """Calculate learning stability."""
        # Stability = variance of rewards in second half
        rewards = [d['reward'] for d in step_data]
        second_half_start = len(rewards) // 2
        
        if second_half_start >= len(rewards):
            return 0.0
        
        second_half_rewards = rewards[second_half_start:]
        return 1.0 / (1.0 + np.std(second_half_rewards))
    
    def generate_report(self):
        """Generate evaluation report."""
        report = "ARCHITECTURE EVALUATION REPORT\n"
        report += "=" * 50 + "\n\n"
        
        for task_name, metrics in self.metrics.items():
            report += f"Task: {task_name}\n"
            report += f"  Accuracy: {metrics['accuracy_mean']:.3f} (±{metrics['accuracy_std']:.3f})\n"
            report += f"  Efficiency: {metrics['efficiency_mean']:.3f} (±{metrics['efficiency_std']:.3f})\n"
            report += f"  Learning Speed: {metrics['learning_speed_mean']:.3f} (±{metrics['learning_speed_std']:.3f})\n"
            report += f"  Stability: {metrics['stability_mean']:.3f} (±{metrics['stability_std']:.3f})\n\n"
        
        return report
```

---

## Module 8: Best Practices for Architecture Design

### 8.1 Principles for Good Architecture

#### A. Modularity
- Keep components independent and loosely coupled
- Use clear interfaces between components
- Allow easy replacement of individual components

#### B. Scalability
- Design architectures that can grow in complexity
- Ensure computational requirements scale reasonably
- Plan for hardware limitations

#### C. Reproducibility
- Document all configuration parameters
- Use version control for architectures
- Create configuration templates for common use cases

#### D. Testability
- Design components that can be tested independently
- Create comprehensive test suites
- Include benchmarks and performance metrics

### 8.2 Common Pitfalls and How to Avoid Them

#### 1. Overly Complex Architectures
```python
# Anti-pattern: Too many parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.rate", 0.005)
config.set("plasticity.structural.growth_rate", 0.05)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.sensitivity", 3.0)
config.set("neuromod.novelty.threshold", 0.2)
config.set("memory.working.capacity", 500)
config.set("memory.episodic.capacity", 100)

# Better approach: Use template for complex architectures
config = pynlm.createDefaultConfig()
# Set specific parameters for the task
config.set("brain.neuron_count", 2000)  # Appropriate size
config.set("plasticity.stdp.learning_rate", 0.002)  # Reasonable learning
```

#### 2. Inconsistent Configuration
```python
# Anti-pattern: Inconsistent units and formats
config.set("brain.v_thresh", -50)  # Good: float
config.set("brain.tau_mem", "20ms")  # Bad: string with units
config.set("plasticity.stdp.enable", 1)  # Bad: int instead of bool

# Better approach: Use consistent types
config.set("brain.v_thresh", -50.0)
config.set("brain.tau_mem", 20.0)
config.set("plasticity.stdp.enable", True)
```

#### 3. Lack of Validation
```python
# Anti-pattern: No validation
brain = pynlm.createBrain(config)
brain.initialize()
# Run simulation...

# Better approach: Validate configuration
if config.getInt("brain.neuron_count") < 100:
    raise ValueError("Brain too small for effective learning")

if config.getFloat("world.vision_width") > config.getInt("world.width"):
    raise ValueError("Vision extends beyond world boundaries")

# Then create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### 8.3 Architecture Design Checklist

#### A. Neural Configuration
- [ ] Brain has reasonable number of neurons (100-5000)
- [ ] Synaptic density is appropriate for task
- [ ] Neuron parameters are biologically plausible
- [ ] Initial conditions are well-defined

#### B. Learning Configuration
- [ ] Plasticity mechanisms are enabled as needed
- [ ] Learning rates are appropriate for task
- [ ] Stability-plasticity balance is considered
- [ ] Memory systems are configured for expected load

#### C. World Configuration
- [ ] World size is appropriate for task
- [ ] Vision range allows necessary perception
- [ ] Environmental dynamics are challenging but solvable
- [ ] Resources and hazards are balanced

#### D. Subsystem Integration
- [ ] All subsystems are enabled as needed
- [ ] Configuration is consistent across subsystems
- [ ] Performance is monitored and optimized
- [ ] Architectures are documented and reproducible

---

## Module 9: Advanced Architecture Workshop

### 9.1 Workshop: Building a Specialized Agent

Let's build an agent specialized for a specific task: navigating a complex environment while learning new skills.

```python
import pynlm
import numpy as np

class NavigationSpecialist:
    def __init__(self):
        self.config = self.create_navigation_config()
        self.create_agent()
        self.train_agent()
    
    def create_navigation_config(self):
        """Create configuration optimized for navigation."""
        config = pynlm.createDefaultConfig()
        
        # Navigation-specific parameters
        config.set("brain.neuron_count", 2500)  # Enough neurons for complex navigation
        config.set("brain.v_thresh", -45.0)   # Sensitive to input
        config.set("brain.tau_mem", 25.0)    # Good memory time constant
        
        # Enable navigation-specific learning
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.003)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.rate", 0.002)
        
        # Navigation-specific neuromodulation
        config.set("neuromod.dopamine.scale", 1.5)    # Reward learning
        config.set("neuromod.curiosity.sensitivity", 2.0)  # Explore new areas
        config.set("neuromod.novelty.threshold", 0.4)  # Detect new locations
        
        # Memory for navigation
        config.set("memory.working.capacity", 300)  # Remember current path
        config.set("memory.episodic.enable", True)   # Remember visited locations
        config.set("memory.episodic.capacity", 100)  # Store location history
        config.set("memory.associative.enable", True)  # Learn location associations
        
        # World for navigation
        config.set("world.width", 30)
        config.set("world.height", 30)
        config.set("world.vision_width", 10)  # Can see ahead
        config.set("world.vision_height", 10)
        config.set("world.max_energy", 200.0)
        config.set("world.energy_decay", 0.01)
        
        return config
    
    def create_agent(self):
        """Create the navigation specialist agent."""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure world for navigation
        self.world.configure(
            width=self.config.getInt("world.width"),
            height=self.config.getInt("world.height"),
            visionWidth=self.config.getInt("world.vision_width"),
            visionHeight=self.config.getInt("world.vision_height"),
            maxEnergy=self.config.getFloat("world.max_energy"),
            energyDecayRate=self.config.getFloat("world.energy_decay")
        )
        
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)  # Center start
        
        # Initialize agent with navigation specializations
        self.agent.initialize(self.world)
        
        # Enable navigation-specific behaviors
        self.agent.enableRewardModulation(True)   # Learn from rewards
        self.agent.enableCuriosity(True)          # Explore new areas
        self.agent.enableDevelopment(True)        # Continuously improve
        self.agent.enableStructuralPlasticity(True) # Grow new connections
    
    def train_agent(self):
        """Train the navigation specialist."""
        print("Training Navigation Specialist...")
        
        training_stats = []
        
        for episode in range(20):  # 20 training episodes
            print(f"Episode {episode + 1}/20")
            
            self.world.reset()
            episode_stats = {
                'episode': episode + 1,
                'steps': 0,
                'total_reward': 0.0,
                'visited_locations': set(),
                'path_efficiency': 0.0
            }
            
            for step in range(500):  # 500 steps per episode
                # Update world
                self.world.update(0.1)
                
                # Get agent's current position
                agent_body = self.world.getAgentBody()
                position = (agent_body.getX(), agent_body.getY())
                episode_stats['visited_locations'].add(position)
                
                # Perceive environment
                percept = self.world.getSensoryPercept()
                
                # Process and act
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Get reward
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                episode_stats['total_reward'] += reward
                
                # Learn from experience
                self.agent.applyRewardModulation(reward, 0.0)
                self.agent.updateDevelopment(0.1)
                
                episode_stats['steps'] += 1
                
                # Log progress
                if step % 100 == 0:
                    print(f"  Step {step}: Position {position}, Reward {reward:.3f}")
            
            # Calculate episode statistics
            episode_stats['avg_reward'] = episode_stats['total_reward'] / episode_stats['steps']
            episode_stats['location_count'] = len(episode_stats['visited_locations'])
            episode_stats['final_curiosity'] = self.agent.getCuriosityLevel()
            episode_stats['final_novelty'] = self.agent.getNoveltyLevel()
            episode_stats['final_stage'] = self.brain.getDevelopmentalStage().name
            
            training_stats.append(episode_stats)
            
            print(f"  Episode complete: Avg Reward={episode_stats['avg_reward']:.3f}, "
                  f"Locations visited={episode_stats['location_count']}, "
                  f"Final Curiosity={episode_stats['final_curiosity']:.3f}")
        
        self.training_stats = training_stats
        self.analyze_performance()
    
    def analyze_performance(self):
        """Analyze and report training performance."""
        print("\nNavigation Specialist Performance Analysis")
        print("=" * 50)
        
        # Calculate overall statistics
        total_episodes = len(self.training_stats)
        avg_total_reward = sum(s['total_reward'] for s in self.training_stats) / total_episodes
        avg_steps = sum(s['steps'] for s in self.training_stats) / total_episodes
        avg_locations = sum(s['location_count'] for s in self.training_stats) / total_episodes
        
        print(f"Total Episodes: {total_episodes}")
        print(f"Average Total Reward: {avg_total_reward:.3f}")
        print(f"Average Steps per Episode: {avg_steps:.1f}")
        print(f"Average Locations Visited: {avg_locations:.1f}")
        
        # Analyze learning progress
        print("\nLearning Progress:")
        for episode_stats in self.training_stats:
            print(f"  Episode {episode_stats['episode']}: "
                  f"Avg Reward={episode_stats['avg_reward']:.3f}, "
                  f"Locations={episode_stats['location_count']}, "
                  f"Curiosity={episode_stats['final_curiosity']:.3f}")
        
        # Check for specialization
        print("\nSpecialization Analysis:")
        final_curiosity = self.training_stats[-1]['final_curiosity']
        final_novelty = self.training_stats[-1]['final_novelty']
        
        if final_curiosity < 0.3:
            print("✓ Agent has developed efficient exploration (low curiosity)")
        elif final_curiosity > 0.7:
            print("⚠ Agent is still highly exploratory (may be over-exploring)")
        else:
            print("○ Agent has moderate exploration levels")
        
        if final_novelty < 0.2:
            print("✓ Agent has learned environment structure (low novelty detection)")
        elif final_novelty > 0.6:
            print("⚠ Agent is still detecting high novelty (may not have learned)")
        else:
            print("○ Agent has moderate novelty detection")
        
        print(f"\nFinal Development Stage: {self.training_stats[-1]['final_stage']}")
        print("Navigation Specialist training complete!")

# Run the workshop
if __name__ == "__main__":
    print("NLM Brain Architecture Workshop: Navigation Specialist")
    print("=" * 60)
    
    specialist = NavigationSpecialist()
    
    print("\n" + "=" * 60)
    print("Workshop Complete!")
    print("=" * 60)
    print("Key Architecture Insights:")
    print("1. Navigation specialists need good spatial memory")
    print("2. Curiosity drives exploration of new areas")
    print("3. Development allows continuous improvement")
    print("4. Neuromodulation coordinates multiple learning systems")
    print("=" * 60)
```

---

## Module 10: Architecture Documentation Best Practices

### 10.1 Documentation Standards

#### A. Code Comments
```python
# Good: Clear, specific comment
# Enable dopamine-based reward learning for reinforcement tasks
config.set("neuromod.dopamine.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Better: Even more descriptive
# Configure reward prediction error learning with moderate sensitivity
# This helps the agent learn from both positive and negative outcomes
config.set("neuromod.dopamine.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
```

#### B. Configuration Documentation
```python
# Document configuration parameters
"""
Brain Parameters:
- neuron_count: Total number of neurons in the network
- v_thresh: Membrane potential threshold for firing
- tau_mem: Membrane time constant (decay rate)
- tau_ref: Refractory period after spike

World Parameters:
- width: Map width in cells
- height: Map height in cells
- vision_width: Agent's visual sensing range (width)
- vision_height: Agent's visual sensing range (height)
"""
```

#### C. Architecture Diagrams
Use clear, labeled diagrams showing:
- Component relationships
- Data flow between components
- Configuration parameters for each component
- Performance metrics and expected behavior

### 10.2 Architectural Decision Records

Document major architectural decisions:

```markdown
# ADR-001: Neural Network Architecture

## Problem
Biological brains use spiking neurons with event-driven processing. Traditional artificial neural networks use continuous activations with gradient-based learning.

## Solution
Implement Leaky Integrate-and-Fire (LIF) neurons with STDP learning rules.

## Pros
- Biologically plausible
- Event-driven, computationally efficient
- Supports complex temporal dynamics

## Cons
- More complex to implement
- Less mature tooling
- Requires careful parameter tuning

## Reference
- Gerstner et al. (2014). Neuronal Dynamics
- Song et al. (2000). Competitive Hebbian learning
```

### 10.3 Architecture Validation

Create systematic validation:

```python
def validate_architecture(config, requirements):
    """Validate architecture against requirements."""
    
    # Check basic constraints
    if config.getInt("brain.neuron_count") < requirements['min_neurons']:
        return False, "Brain too small"
    
    if config.getFloat("world.vision_width") > config.getInt("world.width"):
        return False, "Vision extends beyond world"
    
    # Check learning capabilities
    if not config.getBool("plasticity.stdp.enable"):
        return False, "STDP learning not enabled"
    
    if not config.getBool("neuromod.curiosity.enable"):
        return False, "Curiosity-driven exploration not enabled"
    
    # Check developmental potential
    if not config.getBool("brain.development.enable"):
        return False, "Developmental learning not enabled"
    
    return True, "Architecture meets requirements"
```

---

## Module 11: Future Architecture Extensions

### 11.1 Multi-Sensory Extensions

Extend architecture to support multiple sensory modalities:

```python
class MultiSensoryArchitecture:
    def __init__(self):
        self.sensory_systems = {
            'visual': self.create_visual_system(),
            'auditory': self.create_auditory_system(),
            'tactile': self.create_tactile_system(),
            'olfactory': self.create_olfactory_system()
        }
        
        # Integrate with brain
        self.create_integrated_brain()
    
    def create_visual_system(self):
        """Create visual sensory system."""
        config = pynlm.createDefaultConfig()
        config.set("sensory.type", "visual")
        config.set("sensory.resolution", "high")
        config.set("sensory.range", 10)
        return config
    
    def create_auditory_system(self):
        """Create auditory sensory system."""
        config = pynlm.createDefaultConfig()
        config.set("sensory.type", "auditory")
        config.set("sensory.frequency_range", "wide")
        config.set("sensory.spatial_resolution", "medium")
        return config
    
    def integrate_sensory_systems(self):
        """Integrate all sensory systems with brain."""
        # Combine sensory inputs
        combined_sensory = {}
        for name, config in self.sensory_systems.items():
            combined_sensory[name] = self.process_sensory_config(config)
        
        # Create unified brain configuration
        brain_config = self.combine_sensory_configs(combined_sensory)
        
        # Update brain with integrated sensory capabilities
        self.update_brain_sensory(brain_config)
```

### 11.2 Social Learning Extensions

Add multi-agent learning and social behaviors:

```python
class SocialLearningArchitecture:
    def __init__(self):
        self.agent_population = []
        self.social_memory = {}
        self.shared_knowledge = {}
    
    def create_social_agent(self, agent_id):
        """Create agent with social learning capabilities."""
        config = pynlm.createDefaultConfig()
        
        # Enable social learning mechanisms
        config.set("neuromod.dopamine.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.sensitivity", 2.0)
        
        # Add social memory
        config.set("memory.associative.enable", True)
        config.set("memory.associative.pattern_threshold", 0.8)
        
        # Create agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        
        return agent
    
    def enable_social_learning(self, agent, other_agents):
        """Enable social learning between agents."""
        for other_agent in other_agents:
            # Share experiences
            self.share_experiences(agent, other_agent)
            
            # Joint learning
            self.joint_learning(agent, other_agent)
            
            # Social mimicry
            self.mimic_behavior(agent, other_agent)
    
    def share_experiences(self, agent1, agent2):
        """Share experiences between agents."""
        # Transfer successful behaviors
        for memory_type in ['working', 'episodic', 'associative']:
            # Implementation depends on specific memory system
            pass
    
    def joint_learning(self, agent1, agent2):
        """Learn together from shared experiences."""
        # Coordinated learning sessions
        # Shared reward signals
        # Joint prediction error
        pass
    
    def mimic_behavior(self, agent1, agent2):
        """Mimic successful behaviors from other agents."""
        # Copy successful strategies
        # Adapt to local environment
        # Learn from social observation
        pass
```

### 11.3 Transfer Learning Extensions

Add cross-task and cross-domain transfer learning:

```python
class TransferLearningArchitecture:
    def __init__(self):
        self.task_domains = {}
        self.transfer_mechanisms = {}
        self.knowledge_transfer = {}
    
    def train_on_task(self, task_name, training_data, source_config=None):
        """Train on a specific task."""
        
        if source_config:
            # Use pre-trained configuration
            config = source_config
            print(f"Using pre-trained configuration for {task_name}")
        else:
            # Create new configuration
            config = self.create_task_specific_config(task_name)
        
        # Train architecture
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        
        # Transfer knowledge from related tasks
        self.transfer_from_related_tasks(task_name, config)
        
        # Fine-tune on current task
        self.fine_tune(agent, training_data)
        
        # Store learned knowledge
        self.store_task_knowledge(task_name, brain, agent)
        
        return brain, agent
    
    def transfer_from_related_tasks(self, current_task, config):
        """Transfer knowledge from related tasks."""
        related_tasks = self.find_related_tasks(current_task)
        
        for task_name in related_tasks:
            if task_name in self.knowledge_transfer:
                transferred_knowledge = self.knowledge_transfer[task_name]
                
                # Transfer neural parameters
                self.transfer_neural_parameters(config, transferred_knowledge['brain_params'])
                
                # Transfer learning strategies
                self.transfer_learning_strategies(config, transferred_knowledge['learning_params'])
                
                # Transfer memory patterns
                self.transfer_memory_patterns(config, transferred_knowledge['memory_params'])
    
    def create_task_specific_config(self, task_name):
        """Create configuration optimized for specific task."""
        config = pynlm.createDefaultConfig()
        
        task_configurations = {
            'navigation': {
                'brain': {'neuron_count': 2000, 'v_thresh': -45},
                'world': {'width': 30, 'height': 30, 'vision_width': 10},
                'plasticity': {'stdp': {'learning_rate': 0.003}},
                'neuromod': {'curiosity': {'sensitivity': 2.0}}
            },
            'classification': {
                'brain': {'neuron_count': 3000, 'v_thresh': -50},
                'world': {'width': 20, 'height': 20, 'vision_width': 8},
                'plasticity': {'stdp': {'learning_rate': 0.002}},
                'neuromod': {'curiosity': {'sensitivity': 1.5}}
            },
            'prediction': {
                'brain': {'neuron_count': 2500, 'v_thresh': -55},
                'world': {'width': 25, 'height': 25, 'vision_width': 12},
                'plasticity': {'stdp': {'learning_rate': 0.004}},
                'neuromod': {'curiosity': {'sensitivity': 1.0}}
            }
        }
        
        if task_name in task_configurations:
            task_config = task_configurations[task_name]
            
            # Apply brain configuration
            if 'brain' in task_config:
                for param, value in task_config['brain'].items():
                    if param == 'neuron_count':
                        config.set("brain.neuron_count", value)
                    elif param == 'v_thresh':
                        config.set("brain.v_thresh", value)
            
            # Apply world configuration
            if 'world' in task_config:
                for param, value in task_config['world'].items():
                    config.set(f"world.{param}", value)
            
            # Apply plasticity configuration
            if 'plasticity' in task_config:
                for plastic_param, plastic_value in task_config['plasticity'].items():
                    if plastic_param == 'stdp':
                        for stdp_param, stdp_value in plastic_value.items():
                            config.set(f"plasticity.stdp.{stdp_param}", stdp_value)
            
            # Apply neuromodulation configuration
            if 'neuromod' in task_config:
                for neuromod_param, neuromod_value in task_config['neuromod'].items():
                    if neuromod_param == 'curiosity':
                        for curiosity_param, curiosity_value in neuromod_value.items():
                            config.set(f"neuromod.curiosity.{curiosity_param}", curiosity_value)
        
        return config
```

---

## Conclusion

The NLM brain architecture represents a sophisticated integration of multiple learning systems, inspired by biological brains but optimized for computational efficiency. This tutorial has provided:

1. **Foundational understanding** of the brain as a modular, distributed system
2. **Step-by-step guidance** for building and working with NLM architectures
3. **Advanced patterns** for complex agent behaviors
4. **Debugging and analysis** techniques for architecture evaluation
5. **Best practices** for documentation and design
6. **Future extensions** for multi-sensory and social learning

### Key Takeaways

- **Modularity**: Each subsystem (memory, neuromodulation, prediction, cognition) has a specialized role
- **Integration**: The closed-loop architecture connects all components into a coherent learning system
- **Development**: Brains grow and change through developmental stages
- **Emergence**: Complex behaviors arise from simple interactions between components
- **Extensibility**: The architecture can be extended for new tasks and capabilities

The NLM architecture demonstrates that by combining biologically inspired principles with computational optimization, we can create artificial brains that learn, adapt, and solve complex problems in ways that rival biological intelligence.

---

*This tutorial is continuously updated as the NLM architecture evolves. For the most current information, refer to the documentation website and GitHub repository.*
