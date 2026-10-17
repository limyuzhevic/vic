# NLM Advanced Features Guide

## Advanced Command Line Interface

### NLM Command Structure

The NLM framework provides several command-line interfaces for advanced usage:

#### `nlm-checkpoint`
Create and manage checkpoints for large simulation runs:

```bash
# Create checkpoint at current step
nlm-checkpoint save --step 1000 --name "research_phase"

# Load previous checkpoint
nlm-checkpoint load --step 500 --name "research_phase"

# List available checkpoints
nlm-checkpoint list

# Prune old checkpoints (keep 10 most recent)
nlm-checkpoint prune --keep 10
```

#### `nlm-explore`
Run exploration simulations with curiosity-driven learning:

```bash
# Run exploration simulation
nlm-explore --duration 3600 --curiosity 0.8 --reward-modulation

# Use specific world configuration
nlm-explore --config "research.yaml" --world-size 50x50

# Export exploration results
nlm-explore --export "results.json" --export_episodes
```

#### `nlm-analyze`
Analyze simulation results and extract insights:

```bash
# Analyze memory usage patterns
nlm-analyze memory --brain "simulation.nmlcp" --output "memory_analysis.txt"

# Extract prediction patterns
nlm-analyze predictions --brain "simulation.nmlcp" --episodes 1000 --confidence 0.7

# Find optimal action sequences
nlm-analyze actions --brain "simulation.nmlcp" --min-confidence 0.8 --max-actions 5
```

### Advanced Configuration Options

#### Configuration File Format

```yaml
# nlm_config.yaml
brain:
  neuron_count: 5000
  region_count: 3
  connection_probability: 0.15
  
memory:
  working_memory_capacity: 500
  episodic_memory_max_episodes: 2000
  consolidation_interval: 500

prediction:
  prediction_horizon: 10
  error_threshold: 0.1
  confidence_decay: 0.95

 neuromodulation:
  dopamine_sensitivity: 1.0
  curiosity_weight: 0.7
  novelty_threshold: 0.3

cognition:
  planner_depth: 8
  concept_threshold: 0.6
  attention_strength: 1.2

experimental:
  enable_structural_plasticity: true
  enable_development: true
  enable_checkpointing: true
  checkpoint_interval: 5000
```

## Experimental Features

### 1. Multi-Agent Coordination

Implement coordinated groups of agents sharing experiences:

```python
import pynlm

# Create multiple agents
agent1_brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent2_brain = pynlm.createBrain(pynlm.createDefaultConfig())

agent1 = pynlm.createAgentBrain(agent1_brain)
agent2 = pynlm.createAgentBrain(agent2_brain)

# Share experiences through associative memory
agent1_brain.getAssociativeMemory().connectTo(agent2_brain.getAssociativeMemory())

# Run coordinated simulation
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100)

for step in range(1000):
    # All agents share sensory input
    percept = world.getSensoryPercept()
    agent1.processSensoryInput(percept)
    agent2.processSensoryInput(percept)
    
    # Agents learn from shared experiences
    agent1.applySharedLearning(agent2.getBrain())
    agent2.applySharedLearning(agent1.getBrain())
    
    # Each agent acts independently
    action1 = agent1.decodeMotorCommand()
    action2 = agent2.decodeMotorCommand()
    
    # World updates with both actions
    world.applyMotorCommand(action1, world.getSimulationTime())
    world.applyMotorCommand(action2, world.getSimulationTime())
```

### 2. Development Stages Analysis

Track and analyze brain development over time:

```python
import pynlm
import matplotlib.pyplot as plt

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Development metrics
metrics = {
    'step': [],
    'firing_rate': [],
    'connection_count': [],
    'concept_count': [],
    'memory_episodes': [],
    'development_stage': []
}

for step in range(5000):
    # Run simulation
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record development metrics
    metrics['step'].append(step)
    metrics['firing_rate'].append(brain.getAverageFiringRate())
    metrics['connection_count'].append(brain.getTotalSynapseCount())
    metrics['concept_count'].append(brain.getConceptFormation().getConceptCount() if brain.getConceptFormation() else 0)
    metrics['memory_episodes'].append(brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0)
    metrics['development_stage'].append(int(brain.getDevelopmentalStage()))

# Plot development stages
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
plt.plot(metrics['step'], metrics['firing_rate'])
plt.title('Firing Rate vs Step')
plt.xlabel('Step')
plt.ylabel('Firing Rate')

plt.subplot(2, 3, 2)
plt.plot(metrics['step'], metrics['connection_count'])
plt.title('Connection Count vs Step')
plt.xlabel('Step')
plt.ylabel('Connection Count')

plt.subplot(2, 3, 3)
plt.plot(metrics['step'], metrics['concept_count'])
plt.title('Concept Count vs Step')
plt.xlabel('Step')
plt.ylabel('Concept Count')

plt.subplot(2, 3, 4)
plt.plot(metrics['step'], metrics['memory_episodes'])
plt.title('Memory Episodes vs Step')
plt.xlabel('Step')
plt.ylabel('Memory Episodes')

plt.subplot(2, 3, 5)
plt.plot(metrics['step'], metrics['development_stage'])
plt.title('Development Stage vs Step')
plt.xlabel('Step')
plt.ylabel('Stage')

plt.tight_layout()
plt.savefig('development_analysis.png')
plt.show()
```

### 3. Real-Time Visualization

Interactive visualization of brain activity:

```python
import pynlm
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle, Rectangle

class NLMVisualizer:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.fig, self.ax = plt.subplots(figsize=(12, 8))
        self.setup_visualization()
        
    def setup_visualization(self):
        # Setup the plot
        self.ax.set_xlim(0, 1)
        self.ax.set_ylim(0, 1)
        self.ax.set_aspect('equal')
        self.ax.axis('off')
        
        # Create neuron representations
        self.neuron_circles = []
        for i, neuron in enumerate(self.brain.getAllNeurons()):
            circle = Circle((0.5, 0.5), 0.001, alpha=0.6)
            self.ax.add_patch(circle)
            self.neuron_circles.append(circle)
            
        # Create connection lines
        self.connection_lines = []
        
    def update_frame(self, frame):
        # Update neuron sizes based on activity
        for i, neuron in enumerate(self.brain.getAllNeurons()):
            # Size based on membrane potential
            potential = neuron.getState().membranePotential
            normalized_size = (potential + 20) / 60  # Normalize to reasonable range
            self.neuron_circles[i].radius = max(0.001, min(0.02, normalized_size * 0.02))
            
            # Color based on neuron type
            if neuron.getNeuronType() == NeuronType::Sensory:
                self.neuron_circles[i].set_color('lightblue')
            elif neuron.getNeuronType() == NeuronType::Motor:
                self.neuron_circles[i].set_color('lightcoral')
            elif neuron.getNeuronType() == NeuronType::Internal:
                self.neuron_circles[i].set_color('lightgreen')
            else:
                self.neuron_circles[i].set_color('lightgray')
                
        # Update connections based on synapse weights
        self.connection_lines.clear()
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                if synapse.getWeight() > 0.1:  # Only show strong connections
                    source_neuron = synapse.getSourceNeuron()
                    dest_neuron = synapse.getDestinationNeuron()
                    
                    # Convert neuron IDs to positions
                    source_pos = self.neuron_id_to_position(source_neuron)
                    dest_pos = self.neuron_id_to_position(dest_neuron)
                    
                    line = self.ax.plot([source_pos[0], dest_pos[0]], 
                                      [source_pos[1], dest_pos[1]], 
                                      'gray', alpha=0.3, linewidth=0.5)[0]
                    self.connection_lines.append(line)
                    
        return self.neuron_circles + self.connection_lines
        
    def neuron_id_to_position(self, neuron_id):
        # Convert neuron ID to screen position
        # This is a simplified implementation - real implementation would use actual neuron positions
        return (0.5 + (neuron_id % 10 - 5) * 0.1, 0.5 + (neuron_id // 10 - 5) * 0.1)
        
    def run_visualization(self, duration, fps=30):
        ani = animation.FuncAnimation(self.fig, self.update_frame, 
                                    frames=duration * fps, interval=1000/fps,
                                    blit=False)
        plt.show()
        return ani

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

visualizer = NLMVisualizer(brain, world)
visualizer.run_visualization(1000)
```

## Advanced Documentation Examples

### Example: Building a Learning Agent

```python
import pynlm

class LearningAgent:
    def __init__(self, config):
        self.brain = pynlm.createBrain(config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world for agent
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        self.agent.initialize(self.world)
        
        # Enable advanced learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Enable episodic memory
        self.brain.getEpisodicMemory().setMaxEpisodes(1000)
        
    def train(self, episodes):
        """Train the agent using episodic memory and prediction"""
        for episode in range(episodes):
            self.world.reset()
            self.brain.reset()
            
            for step in range(100):  # Max steps per episode
                # Get current observation
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Brain thinks
                self.brain.step(step)
                
                # Get action
                action = self.agent.decodeMotorCommand()
                
                # Apply action to world
                reward = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Check if episode ended
                if self.world.isEpisodeComplete():
                    break
                    
            # Record episode in episodic memory
            if episode % 10 == 0:  # Record every 10 episodes
                self.brain.getEpisodicMemory().recordEpisode(episode)
                
            # Periodic development
            if episode % 100 == 0:
                self.agent.updateDevelopment(0.1)
                
    def act(self, percept):
        """Agent acting based on current perception"""
        self.agent.processSensoryInput(percept)
        return self.agent.decodeMotorCommand()
    
    def get_brain_state(self):
        """Get current brain state for analysis"""
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'working_memory': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodic_memory': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
            'concepts': self.brain.getConceptFormation().getConceptCount() if self.brain.getConceptFormation() else 0,
            'development_stage': int(self.brain.getDevelopmentalStage()),
            'dopamine_level': self.brain.getDopamine().getLevel() if self.brain.getDopamine() else 0.0,
            'curiosity_level': self.brain.getCuriosity().getLevel() if self.brain.getCuriosity() else 0.0
        }

# Create and train agent
config = pynlm.createDefaultConfig()
agent = LearningAgent(config)
agent.train(1000)

# Test the agent
test_episodes = 5
for episode in range(test_episodes):
    agent.world.reset()
    agent.brain.reset()
    
    while not agent.world.isEpisodeComplete():
        percept = agent.world.getSensoryPercept()
        action = agent.act(percept)
        agent.world.applyMotorCommand(action, agent.world.getSimulationTime())
        
    # Get brain state
    state = agent.get_brain_state()
    print(f"Episode {episode}: {state}")
```

### Example: Checkpoint and Resume Training

```python
import pynlm
import time

def save_checkpoint(agent, checkpoint_path):
    """Save agent brain state to checkpoint"""
    timestamp = int(time.time())
    checkpoint_name = f"checkpoint_{timestamp}"
    
    # Save brain state
    success = agent.brain.save(f"{checkpoint_path}/{checkpoint_name}.nmlcp")
    if success:
        print(f"Checkpoint saved: {checkpoint_name}")
        return checkpoint_name
    return None

def load_checkpoint(agent, checkpoint_path, checkpoint_name):
    """Load agent brain state from checkpoint"""
    checkpoint_file = f"{checkpoint_path}/{checkpoint_name}.nmlcp"
    
    # Create new brain
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    
    # Load checkpoint
    success = new_brain.load(checkpoint_file)
    if success:
        print(f"Checkpoint loaded: {checkpoint_name}")
        return new_brain
    return None

# Training with checkpoints
agent = LearningAgent(pynlm.createDefaultConfig())

# Start training
for episode in range(1000):
    # Train for 10 episodes
    agent.train(10)
    
    # Save checkpoint every 50 episodes
    if episode % 50 == 0 and episode > 0:
        checkpoint_name = save_checkpoint(agent, "./checkpoints")
        
        # Optional: pause training to load latest checkpoint
        if checkpoint_name:
            print("Training paused. Press Enter to continue...")
            input()
            
            # Load the latest checkpoint
            latest_checkpoint = save_checkpoint(agent, "./checkpoints")
            if latest_checkpoint:
                # Create new agent with loaded brain
                new_agent = LearningAgent(pynlm.createDefaultConfig())
                loaded_brain = load_checkpoint(new_agent, "./checkpoints", latest_checkpoint)
                if loaded_brain:
                    new_agent.brain = loaded_brain
                    agent = new_agent
                    print("Training resumed with latest checkpoint")
                    continue

print("Training completed!")
```

## Performance Optimization

### Optimized Configuration for Large-Scale Simulations

```python
# Large-scale simulation configuration
def create_optimized_config(neuron_count=10000, region_count=5):
    config = pynlm.createDefaultConfig()
    
    # Optimize for large neuron counts
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", 0.05)  # Lower for large networks
    
    # Optimize for performance
    config.set("simulation_timestep", 0.0001)  # Smaller timestep for accuracy
    config.set("max_steps", 1000000)  # Large number of steps
    
    # Enable efficient checkpointing
    config.set("checkpoint_dir", "./checkpoints")
    config.set("checkpoint_interval", 10000)
    config.set("max_checkpoints", 5)
    
    # Optimize memory usage
    config.set("working_memory_capacity", neuron_count // 20)
    config.set("episodic_memory_max_episodes", 5000)
    
    # Enable advanced learning
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    config.set("enable_reward_modulation", True)
    
    return config

# Create optimized simulation
optimized_config = create_optimized_config(20000, 10)
brain = pynlm.createBrain(optimized_config)
brain.initialize()

print("Optimized brain created:")
print(f"  - {brain.getTotalNeuronCount()} neurons")
print(f"  - {brain.getRegionCount()} regions")
print(f"  - {brain.getTotalSynapseCount()} synapses")
print(f"  - Estimated memory usage: {brain.getTotalNeuronCount() * 4 / 1024 / 1024:.1f} MB (neurons)")
print(f"  - Estimated synaptic memory: {brain.getTotalSynapseCount() * 4 / 1024 / 1024:.1f} MB (synapses)")
```

### Parallel Processing for Multiple Agents

```python
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, as_completed

class ParallelAgentTraining:
    def __init__(self, num_agents=4):
        self.num_agents = num_agents
        self.agents = []
        
    def create_agent(self, agent_id):
        """Create a single agent with unique configuration"""
        config = pynlm.createDefaultConfig()
        
        # Add agent-specific variation
        config.set("neuron_count", 2000 + agent_id * 500)
        config.set("connection_probability", 0.1 + agent_id * 0.02)
        
        agent = LearningAgent(config)
        return agent
        
    def train_agent(self, agent_id):
        """Train a single agent"""
        agent = self.create_agent(agent_id)
        
        # Train the agent
        for episode in range(100):
            agent.train(5)
            
            # Save progress periodically
            if episode % 25 == 0:
                checkpoint_name = save_checkpoint(agent, "./checkpoints")
                
        return agent_id, agent
        
    def train_parallel(self, max_workers=None):
        """Train multiple agents in parallel"""
        with ProcessPoolExecutor(max_workers=max_workers) as executor:
            # Submit training tasks
            futures = [executor.submit(self.train_agent, i) for i in range(self.num_agents)]
            
            # Collect results
            agents = {}
            for future in as_completed(futures):
                agent_id, agent = future.result()
                agents[agent_id] = agent
                print(f"Agent {agent_id} training completed")
                
        self.agents = agents
        return agents

# Use parallel training
parallel_trainer = ParallelAgentTraining(num_agents=4)
print("Starting parallel agent training...")
agents = parallel_trainer.train_parallel()

print(f"Completed training for {len(agents)} agents")
```

## Advanced Usage Tips

### 1. Fine-Tuning Model Parameters

```python
# Experiment with different configuration parameters
def hyperparameter_search(baseline_config, param_grid):
    results = []
    
    for learning_rate in param_grid['learning_rate']:
        for curiosity_weight in param_grid['curiosity_weight']:
            for development_speed in param_grid['development_speed']:
                # Create agent with specific hyperparameters
                config = baseline_config.copy()
                config.set("reward_modulation_strength", learning_rate)
                config.set("curiosity_weight", curiosity_weight)
                config.set("development_speed", development_speed)
                
                agent = LearningAgent(config)
                
                # Train and evaluate
                agent.train(500)
                metrics = agent.get_brain_state()
                
                results.append({
                    'learning_rate': learning_rate,
                    'curiosity_weight': curiosity_weight,
                    'development_speed': development_speed,
                    'final_firing_rate': metrics['firing_rate'],
                    'memory_episodes': metrics['memory_episodes'],
                    'concept_count': metrics['concept_count']
                })
                
                print(f"Completed: LR={learning_rate}, CW={curiosity_weight}, DS={development_speed}")
                
    return results

# Define parameter grid
param_grid = {
    'learning_rate': [0.5, 1.0, 2.0],
    'curiosity_weight': [0.3, 0.7, 1.2],
    'development_speed': [0.01, 0.1, 1.0]
}

# Run hyperparameter search
results = hyperparameter_search(pynlm.createDefaultConfig(), param_grid)

# Find best configuration
best_result = max(results, key=lambda x: x['final_firing_rate'])
print(f"Best configuration: {best_result}")
```

### 2. Real-Time Monitoring

```python
import time
import threading
from datetime import datetime

class RealTimeMonitor:
    def __init__(self, brain, world, agent, monitor_interval=1.0):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.monitor_interval = monitor_interval
        self.monitoring = False
        self.monitor_thread = None
        self.metrics_history = []
        
    def collect_metrics(self):
        """Collect current metrics"""
        metrics = {
            'timestamp': datetime.now().isoformat(),
            'step': self.world.getCurrentStep(),
            'simulation_time': self.world.getSimulationTime(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'working_memory': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodic_memory': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
            'concepts': self.brain.getConceptFormation().getConceptCount() if self.brain.getConceptFormation() else 0,
            'development_stage': int(self.brain.getDevelopmentalStage()),
            'dopamine_level': self.brain.getDopamine().getLevel() if self.brain.getDopamine() else 0.0,
            'curiosity_level': self.brain.getCuriosity().getLevel() if self.brain.getCuriosity() else 0.0,
            'novelty_level': self.brain.getNovelty().getNoveltyLevel() if self.brain.getNovelty() else 0.0
        }
        return metrics
        
    def monitor_loop(self):
        """Continuous monitoring loop"""
        while self.monitoring:
            metrics = self.collect_metrics()
            self.metrics_history.append(metrics)
            
            # Print current status
            print(f"\n[{metrics['timestamp']}] Step {metrics['step']}:")
            print(f"  Firing Rate: {metrics['firing_rate']:.2f}")
            print(f"  Memory: WM={metrics['working_memory']}, EM={metrics['episodic_memory']}")
            print(f"  Development: Stage {metrics['development_stage']}, Concepts {metrics['concepts']}")
            print(f"  Neuromodulation: DA={metrics['dopamine_level']:.2f}, C={metrics['curiosity_level']:.2f}, N={metrics['novelty_level']:.2f}")
            
            time.sleep(self.monitor_interval)
            
    def start_monitoring(self):
        """Start real-time monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self.monitor_loop)
        self.monitor_thread.start()
        print("Started real-time monitoring")
        
    def stop_monitoring(self):
        """Stop real-time monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join()
        print("Stopped real-time monitoring")
        
    def export_metrics(self, filename):
        """Export metrics to file"""
        import json
        with open(filename, 'w') as f:
            json.dump(self.metrics_history, f, indent=2)
        print(f"Metrics exported to {filename}")

# Use real-time monitoring
monitor = RealTimeMonitor(brain, world, agent, monitor_interval=0.5)

# Start monitoring
monitor.start_monitoring()

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    action = agent.act(percept)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step} completed")

# Stop monitoring
monitor.stop_monitoring()

# Export metrics
monitor.export_metrics("simulation_metrics.json")
```

## Conclusion

The NLM framework now provides a comprehensive suite of advanced features for researchers and developers:

1. **Sophisticated Command-Line Interface** for simulation management
2. **Experimental Features** including multi-agent coordination, development analysis, and real-time visualization
3. **Advanced Documentation** with practical examples for complex scenarios
4. **Performance Optimization** techniques for large-scale simulations
5. **Real-Time Monitoring** tools for simulation analysis

These features enable researchers to conduct more sophisticated experiments, analyze complex neural behaviors, and develop advanced AI systems using the NLM framework.

The improvements maintain backward compatibility while extending the framework's capabilities for cutting-edge neural research and artificial intelligence development.
