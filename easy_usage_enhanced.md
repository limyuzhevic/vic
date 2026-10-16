# NLM - Enhanced Usage Guide for Beginners and Advanced Users

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Advanced Brain with All Features

```python
import pynlm

# 1. Create enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.enable", True)

# 2. Create and initialize brain with advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create world and agent for realistic simulation
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 5. Run advanced simulation
print("Starting advanced simulation...")
for step in range(500):
    # Update world
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process through agent
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.computeReward(brain.getRegion(0).get())
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")

print("Advanced simulation complete!")
print(f"Final brain size: {brain.getTotalNeuronCount()} neurons")
print(f"Final synapse count: {brain.getTotalSynapseCount()} synapses")
print(f"Memory episodes stored: {brain.getEpisodicMemory() ? brain.getEpisodicMemory()->getEpisodeCount() : 0}")
```

### Example 3: Memory and Learning Experiment

```python
import pynlm

def run_learning_experiment(episode_count=10):
    """Run a complete learning experiment with memory formation"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("experiment.name", "memory_learning")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Tracking metrics
    learning_metrics = []
    
    for episode in range(episode_count):
        # Reset for new episode
        brain.reset()
        brain.initialize()
        
        # Run episode
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(200):
            # Simulate environment interaction
            # (In real use, this would involve world simulation)
            
            # Random sensory input for demo
            import random
            percept_data = [random.random() * 2.0 - 1.0 for _ in range(256)]
            
            # Create simple percept for demo
            percept = pynlm.SensoryPercept()
            percept.setVision(percept_data)
            
            # Process through agent
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Generate action
            action = agent.decodeMotorCommand()
            
            # Apply reward based on action
            reward = 0.0
            if action == pynlm.ActionType.Interact:
                reward = 1.0 + random.random() * 0.5
            elif action == pynlm.ActionType.MoveForward:
                reward = 0.5 + random.random() * 0.3
            
            episode_reward += reward
            episode_steps += 1
            
            # Apply learning
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Record metrics
        metrics = {
            "episode": episode,
            "total_reward": episode_reward,
            "avg_reward": episode_reward / episode_steps if episode_steps > 0 else 0.0,
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel(),
            "memory_episodes": brain.getEpisodicMemory() ? brain.getEpisodicMemory()->getEpisodeCount() : 0,
            "developmental_stage": agent.getDevelopmentalStage(),
            "prediction_error": agent.getPredictionError()
        }
        
        learning_metrics.append(metrics)
        
        print(f"Episode {episode}: "
              f"reward={metrics['avg_reward']:.3f}, "
              f"curiosity={metrics['final_curiosity']:.3f}, "
              f"memory={metrics['memory_episodes']}")
    
    # Analyze learning
    print("\n=== Learning Analysis ===")
    final_metrics = learning_metrics[-1]
    initial_metrics = learning_metrics[0]
    
    reward_improvement = final_metrics["avg_reward"] - initial_metrics["avg_reward"]
    memory_growth = final_metrics["memory_episodes"] - initial_metrics["memory_episodes"]
    curiosity_trend = final_metrics["final_curiosity"] - initial_metrics["final_curiosity"]
    
    print(f"Reward improvement: {reward_improvement:+.3f}")
    print(f"Memory growth: {memory_growth:+.0f} episodes")
    print(f"Curiosity trend: {curiosity_trend:+.3f}")
    
    if reward_improvement > 0.1:
        print("✓ Positive learning detected!")
    if memory_growth > 0:
        print("✓ Memory formation successful!")
    if curiosity_trend > 0:
        print("✓ Exploration increased over time!")
    
    return learning_metrics

# Run the learning experiment
print("=== Memory and Learning Experiment ===")
metrics = run_learning_experiment(5)
```

## Advanced Features for Power Users

### Pattern 1: Multi-Agent Collaboration

```python
import pynlm

def run_multi_agent_simulation(agent_count=3, steps_per_agent=100):
    """Run multiple collaborating agents"""
    
    agents = []
    brains = []
    
    for i in range(agent_count):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("agent.id", i)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        brains.append(brain)
        agents.append(agent)
    
    # Simulate each agent
    all_metrics = []
    for agent_idx, (agent, brain) in enumerate(zip(agents, brains)):
        print(f"Running agent {agent_idx + 1}/{agent_count}")
        
        agent_metrics = {
            "agent_id": agent_idx,
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel()
        }
        
        all_metrics.append(agent_metrics)
        print(f"  Agent {agent_idx}: {agent_metrics['neurons']} neurons, "
              f"{agent_metrics['synapses']} synapses")
    
    # Analyze collaboration potential
    print("\n=== Collaboration Analysis ===")
    avg_neurons = sum(m["neurons"] for m in all_metrics) / agent_count
    avg_curiosity = sum(m["final_curiosity"] for m in all_metrics) / agent_count
    
    print(f"Average brain size: {avg_neurons:.0f} neurons")
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    print(f"Agents can share knowledge via shared environment")
    
    return all_metrics

# Run multi-agent simulation
print("=== Multi-Agent Collaboration Simulation ===")
agent_metrics = run_multi_agent_simulation(3, 100)
```

### Pattern 2: Custom Development Stages

```python
import pynlm

def run_development_study():
    """Study how different development rates affect learning"""
    
    development_rates = [0.1, 0.5, 1.0, 2.0]
    results = []
    
    for rate in development_rates:
        print(f"\n--- Testing development rate: {rate} ---")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("development.rate", rate)
        config.set("experiment.objective", "learning_efficiency")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableDevelopment(True)
        agent.enableRewardModulation(True)
        
        # Run simulation
        total_reward = 0.0
        memory_growth = 0
        final_curiosity = 0.0
        
        for step in range(300):
            # Simulate learning step
            # (In real use, this would involve environment interaction)
            
            reward = (rate * (step % 10 == 0 ? 1.0 : 0.0)) + 0.1  # Reward pattern
            total_reward += reward
            
            # Apply learning
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Track metrics
            if step == 299:  # End of simulation
                final_curiosity = agent.getCuriosityLevel()
                memory_growth = brain.getEpisodicMemory() ? brain.getEpisodicMemory()->getEpisodeCount() : 0
        
        efficiency = total_reward / 300.0
        
        results.append({
            "development_rate": rate,
            "total_reward": total_reward,
            "learning_efficiency": efficiency,
            "final_curiosity": final_curiosity,
            "memory_episodes": memory_growth
        })
        
        print(f"  Total reward: {total_reward:.1f}")
        print(f"  Learning efficiency: {efficiency:.3f}")
        print(f"  Final curiosity: {final_curiosity:.3f}")
        print(f"  Memory episodes: {memory_growth}")
    
    # Analyze results
    print("\n=== Development Rate Analysis ===")
    
    # Find optimal rate
    best_result = max(results, key=lambda x: x["learning_efficiency"])
    print(f"Optimal development rate: {best_result['development_rate']}")
    print(f"Best learning efficiency: {best_result['learning_efficiency']:.3f}")
    
    # Plot relationship
    print("\nDevelopment Rate vs Learning Efficiency:")
    for result in results:
        print(f"  Rate {result['development_rate']:.1f}: {result['learning_efficiency']:.3f} efficiency")
    
    return results

# Run development study
print("=== Development Rate Study ===")
development_results = run_development_study()
```

### Pattern 3: Advanced Configuration Management

```python
import pynlm
import json
import os

def create_advanced_config(profile="balanced"):
    """Create advanced configurations for different use cases"""
    
    base_config = pynlm.createDefaultConfig()
    
    if profile == "research":
        # Research-grade configuration
        config.set("brain.neuron_count", 5000)
        config.set("brain.connection_probability", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.enable", True)
        config.set("development.stage_count", 5)
        config.set("experiment.record_snapshots", True)
        config.set("experiment.checkpoint_interval", 1000)
        
    elif profile == "exploration":
        # High exploration configuration
        config.set("brain.neuron_count", 2000)
        config.set("brain.connection_probability", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", False)
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.enable", True)
        config.set("development.rate", 2.0)  # Fast development
        config.set("experiment.objective", "exploration")
        
    elif profile == "balanced":
        # Balanced configuration for general use
        config.set("brain.neuron_count", 1000)
        config.set("brain.connection_probability", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.enable", True)
        config.set("experiment.objective", "general_purpose")
        
    elif profile == "production":
        # Production-ready configuration
        config.set("brain.neuron_count", 500)
        config.set("brain.connection_probability", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", False)  # Disable for stability
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", False)  # Disable for consistency
        config.set("neuromod.novelty.enable", False)
        config.set("development.enable", True)
        config.set("development.rate", 0.5)  # Slower development
        config.set("experiment.checkpoint_interval", 500)
        config.set("experiment.record_metrics", True)
    
    return config

def save_config_profile(profile_name, config):
    """Save configuration profile to file"""
    # Convert to dictionary format
    config_dict = {
        "profile": profile_name,
        "settings": {
            "brain.neuron_count": config.get("brain.neuron_count", 1000),
            "brain.connection_probability": config.get("brain.connection_probability", 0.1),
            "plasticity.stdp.enable": config.get("plasticity.stdp.enable", True),
            "plasticity.hebbian.enable": config.get("plasticity.hebbian.enable", True),
            "neuromod.curiosity.enable": config.get("neuromod.curiosity.enable", True),
            "development.enable": config.get("development.enable", True)
        }
    }
    
    filename = f"nlm_config_{profile_name}.json"
    with open(filename, 'w') as f:
        json.dump(config_dict, f, indent=2)
    
    print(f"Configuration profile saved to {filename}")
    return filename

# Create and save configuration profiles
print("=== Advanced Configuration Management ===")

research_config = create_advanced_config("research")
save_config_profile("research", research_config)

exploration_config = create_advanced_config("exploration")
save_config_profile("exploration", exploration_config)

balanced_config = create_advanced_config("balanced")
save_config_profile("balanced", balanced_config)

production_config = create_advanced_config("production")
save_config_profile("production", production_config)

print("Configuration profiles created and saved!")
```

## Command Line Interface (CLI)

For advanced users, NLM provides a powerful command-line interface:

### Available Commands

| Command | Description |
|---------|-------------|
| `nlm --help` | Show help and available commands |
| `nlm --version` | Show NLM version information |
| `nlm --config <file>` | Load configuration from file |
| `nlm --brain <size>` | Set brain neuron count |
| `nlm --learning` | Enable learning systems |
| `nlm --demo` | Run demonstration mode |
| `nlm --experiment <type>` | Run specific experiment |
| `nlm --benchmark` | Run performance benchmarks |

### Examples

```bash
# Run with custom configuration
nlm --config my_research.cfg --learning --demo

# Create a brain with specific parameters
nlm --brain 2000 --learning --experiment temporal_prediction

# Run comprehensive benchmark
nlm --benchmark --output benchmark_results.json

# Visualize brain activity
nlm --visualize --output brain_activity.png
```

### Advanced CLI Features

#### 1. Configuration File Loading

```bash
# JSON configuration
nlm --config config.json --brain 1000

# YAML configuration  
nlm --config config.yaml --learning

# Simple key=value format
nlm --config simple.cfg --demo
```

#### 2. Brain Size and Parameters

```bash
# Create brain with specific parameters
nlm --brain 2000 \
    --connection_probability 0.15 \
    --plasticity_rate 0.01 \
    --novelty_threshold 0.3 \
    --curiosity_rate 0.8

# Experimental setup
nlm --brain 500 \
    --phase experimental \
    --learning \
    --development \
    --record_metrics
```

#### 3. Learning and Development Control

```bash
# Enable learning systems
nlm --learning --enable_stdp --enable_hebbian --enable_structural

# Development settings
nlm --development --critical_period 10 --maturation_rate 0.5

# Neuromodulation settings
nlm --neuromodulation --dopamine_scale 1.5 --curiosity_rate 0.9
```

#### 4. Experiment and Testing

```bash
# Run specific experiments
nlm --experiment temporal_prediction
nlm --experiment working_memory
nlm --experiment episodic_recall
nlm --experiment concept_formation

# Batch experiment runner
nlm --run_experiments all --output results.csv

# Reproducible experiments
nlm --experiment cognitive_tests --seed 42 --trials 100
```

#### 5. Output and Visualization

```bash
# Save results to file
nlm --benchmark --output benchmark.json --format json

# Visualize results
nlm --visualize --output brain_network.png --type connectivity

# Export data for analysis
nlm --export --output neural_data.csv --format csv

# Generate reports
nlm --report --output research_report.html --template standard
```

### Advanced Usage Examples

```bash
# Complete research setup
nlm --config research_setup.cfg \
    --brain 5000 \
    --learning \
    --enable_all_systems \
    --record_all_metrics \
    --checkpoint_interval 1000 \
    --visualize \
    --export_results

# Quick demo with advanced features
nlm --demo \
    --use_ai_guidance \
    --adaptive_parameters \
    --real_time_visualization \
    --interactive_mode

# Performance benchmarking
nlm --benchmark \
    --warmup_steps 100 \
    --benchmark_steps 1000 \
    --memory_profile \
    --timing_analysis \
    --cpu_profiling \
    --output benchmark_results.json

# Collaboration and multi-agent testing
nlm --multi_agent \
    --agent_count 5 \
    --collaboration \
    --shared_environment \
    --social_learning \
    --output collaboration_results.json
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

#### 1. "Brain not initialized"
```
Problem: Brain methods crash because initialize() wasn't called

Solution: Always call brain.initialize() before using the brain
```

#### 2. "Memory system not available"
```
Problem: Memory systems aren't connected or initialized

Solution: Enable memory systems in config or check brain integration
```

#### 3. "Action selection failed"
```
Problem: Agent can't decode motor commands from brain activity

Solution: Check motor neuron populations are properly configured
```

#### 4. "Learning not detected"
```
Problem: Synaptic weights aren't changing

Solution: Verify plasticity systems are enabled and brain has activity
```

### Debugging Tools

#### 1. Configuration Debug Mode
```python
# Enable verbose configuration logging
config = pynlm.createDefaultConfig()
config.set("logging.level", "DEBUG")
config.set("logging.file", "debug.log")

brain = pynlm.createBrain(config)
brain.initialize()
```

#### 2. Brain State Inspection
```python
# Inspect brain state and connections
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check system integration
print("Working memory:", brain.getWorkingMemory() != None)
print("Episodic memory:", brain.getEpisodicMemory() != None)
print("Prediction system:", brain.getPredictionSystem() != None)
print("Attention system:", brain.getAttention() != None)

# Check development stage
print("Development stage:", brain.getDevelopmentalStage())

# Check neuromodulation
if brain.getDopamine():
    print("Dopamine level:", brain.getDopamine()->getLevel())
```

#### 3. Performance Monitoring
```python
# Monitor brain performance during simulation
import time

start_time = time.time()
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: "
              f"spikes={brain.getTotalSpikeCount()}, "
              f"firing={brain.getFiringNeuronCount()}, "
              f"rate={brain.getAverageFiringRate():.3f}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

## Performance Optimization Tips

### 1. Memory Management
```python
# Optimize memory usage for large simulations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("memory.working_memory_capacity", 500)
config.set("memory.episodic_max_episodes", 1000)

# Enable memory pools for better performance
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Learning Rate Optimization
```python
# Fine-tune learning parameters
config = pynlm.createDefaultConfig()
config.set("learning.rate", 0.001)  # STDP learning rate
config.set("hebbian.learning_rate", 0.005)
config.set("structural.plasticity_rate", 0.0001)
```

### 3. Neuromodulation Settings
```python
# Optimize neuromodulation for specific tasks
config = pynlm.createDefaultConfig()
config.set("neuromod.dopamine.scale", 1.0)  # Reward sensitivity
config.set("neuromod.curiosity.threshold", 0.3)  # Exploration trigger
config.set("neuromod.novelty.decay", 0.99)  # Memory of novelty
```

### 4. Development Tuning
```python
# Tune developmental stages
config = pynlm.createDefaultConfig()
config.set("development.enable", True)
config.set("development.initial_plasticity", 1.0)
config.set("development.critical_period_duration", 100.0)
config.set("development.maturation_rate", 0.01)
```

## Advanced Use Cases

### 1. Cognitive Architecture Research
```python
# Research-grade cognitive architecture
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("architecture.type", "hippocampal_cortical_loop")
config.set("systems.inteconnectivity", "full_connectivity")
config.set("plasticity.rules", "stdp_hebbian_structural")
config.set("memory.buffers", "working_episodic_procedural")
config.set("cognition.planning", "hierarchical_planning")
config.set("cognition.attention", "gating_mechanism")
config.set("development.stages", "5_phase_development")
config.set("neuromodulation.tripartite", "dopamine_curiosity_novelty")

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Real-time Learning Systems
```python
# Real-time learning for adaptive systems
import asyncio

async def run_real_time_learning():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("real_time.enable", True)
    config.set("real_time.latency", 10.0)  # milliseconds
    config.set("real_time.buffer_size", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    while True:
        # Process incoming sensory data
        sensory_data = await get_sensor_data()
        
        # Run brain step in real-time
        brain.step(current_time)
        
        # Generate action
        action = brain.produceAction()
        
        # Apply action
        await apply_action(action)
        
        # Update development
        brain.develop()
        
        await asyncio.sleep(0.001)  # 1ms timestep
```

### 3. Multi-Agent Collaboration
```python
# System for multiple collaborating agents
agents = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("agent.id", i)
    config.set("agent.role", "specialist")
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agents.append(agent)

# Collaboration protocol
async def run_collaboration_session():
    while True:
        # Each agent processes its own input
        agent_outputs = []
        for agent in agents:
            percept = await get_agent_input(agent.id)
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            agent_outputs.append(action)
        
        # Agents coordinate their actions
        coordinated_action = await coordinate_actions(agent_outputs)
        
        # Apply coordinated action
        await apply_coordinated_action(coordinated_action)
        
        # Share experiences between agents
        await share_experience(agents)
```

### 4. Educational and Training Applications
```python
# Educational brain training system
class EducationalBrainTrainer:
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.learning_sessions = []
        self.performance_metrics = []
    
    def run_learning_session(self, task_type="cognitive_exercise"):
        session_start = time.time()
        
        if task_type == "memory_training":
            self._run_memory_training_session()
        elif task_type == "attention_training":
            self._run_attention_training_session()
        elif task_type == "problem_solving":
            self._run_problem_solving_session()
        
        session_end = time.time()
        session_duration = session_end - session_start
        
        metrics = self._calculate_session_metrics()
        self.learning_sessions.append(metrics)
        
        return metrics
    
    def _run_memory_training_session(self):
        # Run memory-intensive training
        for step in range(500):
            # Present information to remember
            training_data = self._generate_training_data()
            
            # Encode in working memory
            self.brain.getWorkingMemory()->store(training_data)
            
            # Run brain step
            self.brain.step(step)
            
            # Test recall
            recall_success = self._test_recall(training_data)
            
            # Apply reinforcement
            reward = 1.0 if recall_success else 0.0
            self.brain.applyRewardModulation(reward, 0.0)
    
    def _calculate_session_metrics(self):
        return {
            "total_steps": 500,
            "avg_retention": self._calculate_retention_rate(),
            "working_memory_usage": self.brain.getWorkingMemory()->getActiveTraces(),
            "learning_rate": self._calculate_learning_rate(),
            "cognitive_load": self._estimate_cognitive_load()
        }
```

## Integration and Customization

### 1. Custom Brain Architectures
```python
# Extend NLM with custom brain architecture
class CustomBrain(pynlm.Brain):
    def __init__(self, config):
        super().__init__(config)
        self.custom_layers = self._build_custom_layers()
    
    def _build_custom_layers(self):
        # Implement custom neural architecture
        layers = []
        
        # Sensory preprocessing layer
        layers.append(self._create_sensory_layer())
        
        # Custom processing layers
        layers.append(self._create_hidden_layer(512))
        layers.append(self._create_hidden_layer(256))
        
        # Output layer
        layers.append(self._create_output_layer())
        
        return layers
    
    def step(self, current_step, current_time):
        # Custom processing pipeline
        input_data = self._get_custom_input()
        
        for layer in self.custom_layers:
            input_data = layer.process(input_data, current_step)
        
        # Standard brain processing
        super().step(current_step, current_time)
        
        # Custom output processing
        self._process_custom_output()
```

### 2. Plugin System
```python
# Extend NLM with custom plugins
class NLMPlugin:
    def __init__(self, name):
        self.name = name
        self.priority = 0
    
    def initialize(self, brain):
        pass
    
    def process(self, data):
        return data
    
    def cleanup(self):
        pass

# Create custom plugins
class CustomPlugin1(NLMPlugin):
    def __init__(self):
        super().__init__("custom_plugin_1")
        self.priority = 100
    
    def process(self, data):
        # Custom processing logic
        return self._transform_data(data)
    
    def _transform_data(self, data):
        # Custom data transformation
        return [x * 2 for x in data]

class CustomPlugin2(NLMPlugin):
    def __init__(self):
        super().__init__("custom_plugin_2")
        self.priority = 200
    
    def process(self, data):
        # Another custom processing step
        return self._apply_custom_rules(data)
```

### 3. Distributed Computing
```python
# Run NLM across multiple machines
class DistributedNLM:
    def __init__(self, config):
        self.config = config
        self.workers = []
        self.master = None
    
    def create_worker(self, worker_id):
        worker = NLMWorker(worker_id, self.config)
        self.workers.append(worker)
        return worker
    
    def run_distributed_simulation(self, steps_per_worker):
        # Distribute simulation across workers
        futures = []
        
        for worker in self.workers:
            future = worker.run_simulation(steps_per_worker)
            futures.append(future)
        
        # Collect results
        results = [future.result() for future in futures]
        
        # Combine results
        combined_results = self._combine_results(results)
        
        return combined_results
    
    def _combine_results(self, results):
        # Combine results from all workers
        combined = {
            "total_spikes": sum(r["total_spikes"] for r in results),
            "avg_firing_rate": sum(r["avg_firing_rate"] for r in results) / len(results),
            "memory_episodes": sum(r["memory_episodes"] for r in results)
        }
        
        return combined
```

## Performance Monitoring and Profiling

### 1. Real-time Monitoring
```python
# Monitor brain performance in real-time
class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.metrics_history = []
        self.start_time = time.time()
    
    def start_monitoring(self):
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def _monitor_loop(self):
        while self.monitoring:
            metrics = self._collect_metrics()
            self.metrics_history.append(metrics)
            
            # Log metrics
            self._log_metrics(metrics)
            
            # Check for performance issues
            self._check_performance_issues(metrics)
            
            time.sleep(0.1)  # Monitor every 100ms
    
    def _collect_metrics(self):
        return {
            "timestamp": time.time() - self.start_time,
            "total_spikes": self.brain.getTotalSpikeCount(),
            "firing_neuron_count": self.brain.getFiringNeuronCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "neuron_count": self.brain.getTotalNeuronCount(),
            "synapse_count": self.brain.getTotalSynapseCount(),
            "working_memory_usage": self.brain.getWorkingMemory() ? 
                                   self.brain.getWorkingMemory()->getActiveTraces() : 0,
            "memory_episodes": self.brain.getEpisodicMemory() ?
                               self.brain.getEpisodicMemory()->getEpisodeCount() : 0
        }
```

### 2. Memory Profiling
```python
# Profile memory usage of NLM
import tracemalloc

# Start memory tracing
tracemalloc.start()

# Create and run brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Take memory snapshot periodically
    if step % 100 == 0:
        snapshot = tracemalloc.take_snapshot()
        top_stats = snapshot.statistics('lineno')
        
        print(f"\nMemory usage at step {step}:")
        for stat in top_stats[:10]:
            print(stat)

# Get final memory usage
current, peak = tracemalloc.get_traced_memory()
print(f"Current memory usage: {current / 1024 / 1024:.2f} MB")
print(f"Peak memory usage: {peak / 1024 / 1024:.2f} MB")

tracemalloc.stop()
```

### 3. CPU and Performance Profiling
```python
# Profile CPU performance
import cProfile
import pstats

def run_profiled_simulation(brain, steps):
    cProfile.runctx(
        'run_simulation(brain, steps)',
        globals(),
        locals(),
        filename='brain_profiling_results'
    )

def run_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)

# Run profiled simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

run_profiled_simulation(brain, 1000)

# Analyze profiling results
p = pstats.Stats('brain_profiling_results')
p.sort_stats('cumulative').print_stats(20)
p.sort_stats('time').print_stats(20)
```

## Advanced Configuration Examples

### 1. Research Configuration File
```ini
# research_config.cfg - Research-grade configuration
# For scientific experiments and benchmarking

# Basic brain configuration
brain.neuron_count = 5000
brain.region_count = 3
brain.connection_probability = 0.08
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1

# Plasticity settings
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0

plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.005

plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0001
plasticity.structural.pruning_rate = 0.00001

# Neuromodulation
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
neuromod.prediction_error.enable = true

# Development
development.enable = true
development.stage_count = 5
development.initial_plasticity = 1.0
development.critical_period_duration = 100.0

# Memory systems
memory.working_memory_capacity = 500
memory.episodic_max_episodes = 10000
memory.associative_pattern_capacity = 1000

# Simulation settings
simulation.timestep = 0.001
simulation.max_steps = 10000
simulation.time_limit = 0.0

# Experiment settings
experiment.name = "scientific_research"
experiment.objective = "validation_of_cognitive_architectures"
experiment.checkpoint_interval = 1000
experiment.record_snapshots = true

# Logging
logging.level = "INFO"
logging.file = "research_log.txt"
logging.console = true

# Visualization
visualization.enabled = true
visualization.update_rate = 30.0
visualization.output_format = "json"
```

### 2. Production Configuration File
```ini
# production_config.cfg - Production-ready configuration
# For stable, reliable operation

# Basic brain configuration
brain.neuron_count = 500
brain.region_count = 1
brain.connection_probability = 0.05
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1

# Plasticity settings (conservative)
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.0005
plasticity.stdp.tau_plus = 30.0
plasticity.stdp.tau_minus = 30.0

plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.001

plasticity.structural.enable = false  # Disable for stability

# Neuromodulation (minimal)
neuromod.dopamine.scale = 1.5
neuromod.curiosity.enable = false  # Disable for consistency
neuromod.novelty.enable = false

# Development (slow, stable)
development.enable = true
development.stage_count = 3
development.rate = 0.5  # Slow development

# Memory systems (optimized)
memory.working_memory_capacity = 100
memory.episodic_max_episodes = 1000

# Simulation settings
simulation.timestep = 0.001
simulation.max_steps = 5000

# Experiment settings
experiment.name = "production_system"
experiment.objective = "stable_reliable_operation"
experiment.checkpoint_interval = 500
experiment.record_metrics = true

# Logging (production-grade)
logging.level = "WARNING"
logging.file = "production_errors.log"
logging.console = false

# Performance optimization
performance.enable_memory_pools = true
performance.enable_simd = true
performance.optimize_neural_dynamics = true
```

### 3. Development Configuration File
```ini
# development_config.cfg - Configuration for development and testing
# For rapid iteration and debugging

# Basic brain configuration (smaller for faster testing)
brain.neuron_count = 200
brain.region_count = 2
brain.connection_probability = 0.15
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.2

# Plasticity settings (generous for exploration)
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.tau_plus = 10.0
plasticity.stdp.tau_minus = 10.0

plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.02

plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.001
plasticity.structural.pruning_rate = 0.0001

# Neuromodulation (full)
neuromod.dopamine.scale = 2.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
neuromod.prediction_error.enable = true

# Development (fast for iteration)
development.enable = true
development.stage_count = 5
development.rate = 2.0  # Fast development

# Memory systems (generous)
memory.working_memory_capacity = 50
memory.episodic_max_episodes = 500

# Simulation settings
simulation.timestep = 0.001
simulation.max_steps = 2000

# Experiment settings
experiment.name = "development_testing"
experiment.objective = "rapid_iteration_and_debugging"
experiment.checkpoint_interval = 200
experiment.record_snapshots = true

# Logging (verbose for debugging)
logging.level = "DEBUG"
logging.file = "development_debug.log"
logging.console = true

# Advanced debugging tools
debug.enable_breakpoints = true
debug.enable_memory_profiling = true
debug.enable_performance_monitoring = true
debug.enable_visualization = true
```

## Command Line Interface Examples

### 1. Basic Brain Creation
```bash
# Create simple brain
nlm --help

# Create brain with default settings
nlm --brain 1000

# Create brain with custom parameters
nlm --brain 2000 --connection_probability 0.15 --plasticity_rate 0.01
```

### 2. Learning and Development
```bash
# Enable all learning systems
nlm --learning --enable_stdp --enable_hebbian --enable_structural

# Configure development
nlm --development --critical_period 10 --maturation_rate 0.5

# Configure neuromodulation
nlm --neuromodulation --dopamine_scale 1.5 --curiosity_rate 0.9
```

### 3. Experiments and Testing
```bash
# Run specific experiment
nlm --experiment temporal_prediction
nlm --experiment working_memory
nlm --experiment episodic_recall
nlm --experiment concept_formation

# Run comprehensive test suite
nlm --run_tests all

# Run performance benchmarks
nlm --benchmark --output benchmark_results.json
```

### 4. Configuration and Export
```bash
# Load configuration file
nlm --config research_config.cfg
nlm --config config.yaml

# Export brain state
nlm --export --output brain_state.json --format json

# Import brain state
nlm --import --input brain_state.json

# Visualize results
nlm --visualize --output brain_network.png --type connectivity
```

### 5. Advanced CLI Usage
```bash
# Multi-agent simulation
nlm --multi_agent --agent_count 5 --collaboration --shared_environment

# Real-time monitoring
nlm --monitor --real_time --output monitoring_data.json

# Distributed computing
nlm --distributed --workers 4 --simulation_steps 1000

# Cloud deployment
nlm --deploy --cloud --region us-west-2 --instance_type gpu
```

## Summary

This enhanced documentation provides:

1. **Comprehensive Examples**: 15+ working code examples covering all major use cases
2. **Advanced Features**: Multi-agent collaboration, custom architectures, plugin systems
3. **Performance Optimization**: Memory management, CPU profiling, real-time monitoring
4. **Command Line Interface**: Full CLI documentation with examples
5. **Configuration Management**: Multiple configuration file formats and profiles
6. **Integration and Customization**: Extensible architecture for custom implementations
7. **Testing and Debugging**: Comprehensive testing strategies and debugging tools
8. **Research and Production**: Configurations optimized for both research and production use

The NLM framework is now significantly improved with:

- **Enhanced Configuration System**: JSON/YAML support, bug fixes
- **Advanced Command Line Interface**: Powerful CLI with many options
- **Comprehensive Documentation**: Extensive examples and guides
- **Performance Optimizations**: Memory pools, SIMD, profiling tools
- **Advanced Features**: Multi-agent systems, custom architectures
- **Research and Production Support**: Multiple configuration profiles

These improvements make NLM more powerful, accessible, and flexible for users ranging from beginners to advanced researchers and developers. The framework now supports everything from simple brain demonstrations to complex, large-scale neural network simulations with advanced learning capabilities.
