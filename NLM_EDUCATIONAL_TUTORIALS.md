# NLM Educational Tutorials and Demo Scenarios

## Overview

This section provides educational tutorials and demo scenarios for using NLM (Neural Learning Machine). These tutorials are designed to help users understand different aspects of the system and demonstrate its capabilities.

## Tutorial 1: Getting Started with NLM

### Objectives
- Understand the basic NLM architecture
- Create and run a simple brain simulation
- Explore the brain-world-agent interaction pattern
- Learn how to interpret simulation results

### Prerequisites
- Basic C++ or Python knowledge
- Understanding of neural network concepts (optional)

### Step-by-Step Instructions

#### 1. Setup and Installation

**C++ Installation:**
```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Python Installation:**
```bash
pip install pynlm
```

#### 2. First Brain Simulation (C++)

Create a file `tutorial1.cpp`:

```cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== NLM Tutorial 1: Basic Brain Simulation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Brain created with " << brain->getTotalNeuronCount() 
              << " neurons and " << brain->getTotalSynapseCount() 
              << " synapses." << std::endl;
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons, "
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "\n=== Tutorial 1 Complete ===" << std::endl;
    std::cout << "Your brain simulated " << brain->getTotalSpikeCount() 
              << " spikes over 100 steps." << std::endl;
    
    return 0;
}
```

Compile and run:
```bash
g++ tutorial1.cpp -o tutorial1 $(pkg-config --cflags --libs nlm)
./tutorial1
```

#### 3. First Brain Simulation (Python)

Create a file `tutorial1.py`:

```python
import pynlm

print("=== NLM Tutorial 1: Basic Brain Simulation ===")

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.region_count", 1)
config.set("brain.connection_probability", 0.1)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons and "
      f"{brain.getTotalSynapseCount()} synapses.")

# Run simulation
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getTotalSpikeCount()} total spikes")

print("\n=== Tutorial 1 Complete ===")
print(f"Your brain simulated {brain.getTotalSpikeCount()} spikes over 100 steps.")
```

Run:
```bash
python tutorial1.py
```

### Expected Results
- Brain creates 100 neurons with approximately 10 synapses each
- Spikes are generated when neurons reach firing threshold
- Firing rate increases with simulation steps due to learning
- Final spike count should be > 0 (learning detected)

### Learning Objectives
1. Understand NLM brain creation process
2. Learn basic simulation loop
3. Interpret brain statistics
4. Observe neural activity patterns

### Further Exploration
1. Try different neuron counts (e.g., 500, 1000)
2. Modify connection probabilities
3. Add visualization with `nlm_phase7_demo`
4. Create learning experiments with rewards

## Tutorial 2: Brain-World-Agent Interaction

### Objectives
- Understand the complete agent lifecycle
- Learn sensory input and motor output processing
- Explore environment interaction
- Create goal-directed behavior

### Prerequisites
- Completed Tutorial 1
- Understanding of basic NLM concepts

### Step-by-Step Instructions

#### 1. Environment Setup

Create a file `tutorial2.py`:

```python
import pynlm
import time

print("=== NLM Tutorial 2: Brain-World-Agent Interaction ===")
```

#### 2. Setup Components

```python
# Create brain with learning configuration
config = pynlm.createLearningConfig(neurons=200)
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(15.0, 15.0)  # Center of world

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print(f"Environment setup: {world.getWidth()}x{world.getHeight()} world")
print(f"Vision: {agent.getSensoryInputSize()} inputs, {agent.getMotorOutputSize()} outputs")
print(f"Learning enabled: Reward={agent.isRewardModulationEnabled()}, "
      f"Curiosity={agent.isCuriosityEnabled()}, "
      f"Development={agent.isDevelopmentEnabled()}")
```

#### 3. Run Episode

```python
# Run simulation episode
def run_episode(num_steps, verbose=True):
    total_reward = 0.0
    exploration_count = 0
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        total_reward += reward
        if agent.getNoveltyLevel() > 0.1:
            exploration_count += 1
        
        if verbose and step % 50 == 0:
            print(f"  Step {step}: Reward={reward:.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"DevStage={agent.getDevelopmentalStage()}")
    
    return {
        'total_reward': total_reward,
        'avg_reward': total_reward / num_steps,
        'exploration_count': exploration_count,
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'final_dev_stage': agent.getDevelopmentalStage()
    }

# Run episode
print("\n=== Running Agent Episode ===")
results = run_episode(500, verbose=True)

print("\n=== Episode Results ===")
print(f"Total Reward: {results['total_reward']:.3f}")
print(f"Average Reward: {results['avg_reward']:.3f}")
print(f"Exploration Events: {results['exploration_count']}")
print(f"Final Curiosity: {results['curiosity_level']:.3f}")
print(f"Final Novelty: {results['novelty_level']:.3f}")
print(f"Final Development Stage: {results['final_dev_stage']}")
```

#### 4. Analysis and Visualization

```python
# Save brain for later analysis
brain.save("tutorial2_brain.bin")

# Run with visualization
print("\n=== Running with Visualization ===")
print("To run with visualization, use the command:")
print("  nlm_phase7_demo --steps 500 --export tutorial2_results")
print("  This will create real-time visualization and export data.")
```

### Expected Results
- Agent moves around the world
- Receives rewards for successful actions
- Develops curiosity for novel experiences
- Shows learning over time
- Final curiosity and novelty levels indicate exploration behavior

### Learning Objectives
1. Understand agent-world interaction
2. Learn reward-based learning
3. Explore curiosity-driven behavior
4. Analyze agent performance

### Further Exploration
1. Change world size and obstacles
2. Modify agent capabilities
3. Run multiple episodes and compare
4. Create learning curves and analysis
5. Use visualization for detailed study

## Tutorial 3: Learning Experiment

### Objectives
- Design and run controlled learning experiments
- Analyze learning curves and performance
- Compare different learning strategies
- Create quantitative measurements

### Prerequisites
- Completed Tutorial 2
- Basic understanding of experimental design

### Step-by-Step Instructions

#### 1. Experiment Setup

Create a file `tutorial3.py`:

```python
import pynlm
import matplotlib.pyplot as plt  # For analysis

print("=== NLM Tutorial 3: Learning Experiment ===")
```

#### 2. Create Experiment Framework

```python
class LearningExperiment:
    def __init__(self):
        self.results = []
        self.brain_history = []
        self.performance_history = []
    
    def run_experiment(self, config, num_episodes=5):
        """Run a complete learning experiment"""
        
        for episode in range(num_episodes):
            print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
            
            # Create fresh brain for each episode
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create environment
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Run episode
            episode_reward = 0.0
            episode_spikes = 0
            
            for step in range(200):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
                episode_reward += reward
                episode_spikes += brain.getTotalSpikeCount()
            
            # Record results
            result = {
                'episode': episode + 1,
                'total_reward': episode_reward,
                'avg_reward': episode_reward / 200,
                'total_spikes': episode_spikes,
                'final_curiosity': agent.getCuriosityLevel(),
                'final_novelty': agent.getNoveltyLevel(),
                'final_dev_stage': agent.getDevelopmentalStage()
            }
            
            self.results.append(result)
            self.brain_history.append(brain)
            self.performance_history.append({
                'reward': episode_reward / 200,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel()
            })
            
            print(f"Episode {episode + 1}: Avg Reward={result['avg_reward']:.3f}, "
                  f"Curiosity={result['final_curiosity']:.3f}, "
                  f"Novelty={result['final_novelty']:.3f}")
    
    def analyze_results(self):
        """Analyze and display experiment results"""
        
        print("\n=== Experiment Analysis ===")
        
        # Calculate averages
        avg_reward = sum(r['avg_reward'] for r in self.results) / len(self.results)
        avg_curiosity = sum(r['final_curiosity'] for r in self.results) / len(self.results)
        avg_novelty = sum(r['final_novelty'] for r in self.results) / len(self.results)
        
        print(f"Average Performance over {len(self.results)} episodes:")
        print(f"  Average Reward per Step: {avg_reward:.4f}")
        print(f"  Average Final Curiosity: {avg_curiosity:.3f}")
        print(f"  Average Final Novelty: {avg_novelty:.3f}")
        
        # Learning trend
        rewards = [r['avg_reward'] for r in self.results]
        if len(rewards) > 1:
            improvement = rewards[-1] - rewards[0]
            print(f"Learning Trend: {improvement:.4f} improvement from first to last episode")
            
            if improvement > 0.01:
                print("  → Strong learning detected")
            elif improvement > 0.001:
                print("  → Moderate learning detected")
            else:
                print("  → Limited learning detected")
        
        # Curiosity trend
        curiosities = [r['final_curiosity'] for r in self.results]
        if len(curiosities) > 1:
            curiosity_change = curiosities[-1] - curiosities[0]
            print(f"Curiosity Change: {curiosity_change:.3f} from first to last episode")
        
        return {
            'avg_reward': avg_reward,
            'avg_curiosity': avg_curiosity,
            'avg_novelty': avg_novelty,
            'learning_trend': improvement if len(rewards) > 1 else 0.0,
            'curiosity_trend': curiosity_change if len(curiosities) > 1 else 0.0
        }
    
    def plot_results(self):
        """Plot results (requires matplotlib)"""
        try:
            import matplotlib.pyplot as plt
            
            episodes = range(1, len(self.results) + 1)
            rewards = [r['avg_reward'] for r in self.results]
            curiosities = [r['final_curiosity'] for r in self.results]
            novelties = [r['final_novelty'] for r in self.results]
            
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
            
            # Reward plot
            ax1.plot(episodes, rewards, 'bo-', linewidth=2, markersize=6)
            ax1.set_ylabel('Average Reward per Step')
            ax1.set_title('Learning Curve')
            ax1.grid(True, alpha=0.3)
            
            # Metrics plot
            ax2.plot(episodes, curiosities, 'ro-', label='Curiosity', linewidth=2, markersize=6)
            ax2.plot(episodes, novelties, 'go-', label='Novelty', linewidth=2, markersize=6)
            ax2.set_xlabel('Episode')
            ax2.set_ylabel('Metric Value')
            ax2.set_title('Exploration Behavior')
            ax2.legend()
            ax2.grid(True, alpha=0.3)
            
            plt.tight_layout()
            plt.savefig('learning_experiment.png')
            print("\nPlot saved as 'learning_experiment.png'")
            
        except ImportError:
            print("matplotlib not available. Skipping plot generation.")
```

#### 3. Run Experiment

```python
# Configure experiment
print("=== Learning Experiment Setup ===")
print("Testing different configurations...")

# Experiment 1: Basic learning
config1 = pynlm.createLearningConfig(neurons=200, learningRate=0.01)

# Experiment 2: Exploration-focused
config2 = pynlm.createExplorationConfig(neurons=200, curiosity=0.8, novelty=0.6)

# Experiment 3: Conservative
config3 = pynlm.createLearningConfig(neurons=200, learningRate=0.005)

experiments = [
    ("Basic Learning", config1),
    ("Exploration-Focused", config2),
    ("Conservative", config3)
]

# Run experiments
for name, config in experiments:
    print(f"\n--- Running {name} Experiment ---")
    experiment = LearningExperiment()
    experiment.run_experiment(config, num_episodes=3)
    results = experiment.analyze_results()
    
    # Save experiment results
    with open(f'{name.lower().replace(" ", "_")}_results.txt', 'w') as f:
        f.write(f"Experiment: {name}\n")
        f.write(f"Average Reward: {results['avg_reward']:.4f}\n")
        f.write(f"Average Curiosity: {results['avg_curiosity']:.3f}\n")
        f.write(f"Learning Trend: {results['learning_trend']:.4f}\n")
    
    print(f"Results saved to: {name.lower().replace(' ', '_')}_results.txt")
```

### Expected Results
- Learning curves showing improvement over episodes
- Curiosity and novelty levels indicating exploration behavior
- Comparison between different learning strategies
- Quantitative measurements of learning efficiency

### Learning Objectives
1. Design controlled experiments
2. Analyze learning curves and trends
3. Compare different learning strategies
4. Create reproducible experimental procedures
5. Generate quantitative results

### Further Exploration
1. Test more configuration variations
2. Run longer experiments for statistical significance
3. Compare different neural architectures
4. Analyze brain states and development
5. Create multi-agent competition experiments

## Tutorial 4: Advanced NLM Features

### Objectives
- Explore advanced NLM features and capabilities
- Understand development stages and plasticity
- Learn about checkpointing and state management
- Create sophisticated experimental setups

### Prerequisites
- Completed Tutorial 3
- Interest in advanced NLM features

### Step-by-Step Instructions

#### 1. Advanced Experiment Setup

Create a file `tutorial4.py`:

```python
import pynlm
from pathlib import Path

print("=== NLM Tutorial 4: Advanced Features ===")
```

#### 2. Create Advanced Experiment Framework

```python
class AdvancedExperiment:
    def __init__(self):
        self.config = pynlm.createLearningConfig(neurons=500, learningRate=0.02)
        self.config.set("plasticity.structural.enable", True)
        self.config.set("plasticity.structural.plasticity_rate", 0.05)
        self.config.set("development.enable_critical_period", True)
        self.config.set("development.enable_synaptogenesis", True)
        self.config.set("development.enable_pruning", True)
        
        # Enable all advanced features
        self.advanced_features = {
            'structural_plasticity': True,
            'development': True,
            'curiosity': True,
            'novelty': True,
            'checkpointing': True,
            'replay': True
        }
    
    def setup_advanced_brain(self):
        """Create brain with all advanced features enabled"""
        
        print("Setting up advanced brain...")
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Create sophisticated agent
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(20.0, 20.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        for feature, enabled in self.advanced_features.items():
            if hasattr(agent, f'enable{feature.replace("_", " ").title().replace(" ", "")}Rpc'):
                getattr(agent, f'enable{feature.replace("_", " ").title().replace(" ", "")}')(True)
            else:
                print(f"Warning: Could not enable feature {feature}")
        
        return brain, world, agent
    
    def run_advanced_simulation(self, brain, world, agent, num_steps):
        """Run advanced simulation with detailed tracking"""
        
        # Tracking variables
        tracking_data = {
            'step': [],
            'reward': [],
            'curiosity': [],
            'novelty': [],
            'firing_rate': [],
            'spikes': [],
            'spike_count': [],
            'dev_stage': [],
            'synaptic_weight': []
        }
        
        print(f"Running advanced simulation for {num_steps} steps...")
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get state before action
            prev_curiosity = agent.getCuriosityLevel()
            prev_novelty = agent.getNoveltyLevel()
            prev_firing_rate = brain.getAverageFiringRate()
            prev_spikes = brain.getTotalSpikeCount()
            prev_dev_stage = agent.getDevelopmentalStage()
            
            # Process sensory input
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Apply reward and development
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record data
            tracking_data['step'].append(step)
            tracking_data['reward'].append(reward)
            tracking_data['curiosity'].append(agent.getCuriosityLevel())
            tracking_data['novelty'].append(agent.getNoveltyLevel())
            tracking_data['firing_rate'].append(brain.getAverageFiringRate())
            tracking_data['spikes'].append(brain.getTotalSpikeCount())
            tracking_data['spike_count'].append(brain.getTotalSpikeCount() - prev_spikes)
            tracking_data['dev_stage'].append(agent.getDevelopmentalStage())
            tracking_data['synaptic_weight'].append(self.get_average_synaptic_weight(brain))
            
            # Progress reporting
            if step % 100 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Firing={brain.getAverageFiringRate():.3f} Hz")
        
        return tracking_data
    
    def get_average_synaptic_weight(self, brain):
        """Calculate average synaptic weight (simplified)"""
        # This would be more complex in real implementation
        return 0.5  # Placeholder
    
    def analyze_advanced_results(self, tracking_data):
        """Analyze advanced simulation results"""
        
        print("\n=== Advanced Results Analysis ===")
        
        # Basic statistics
        total_reward = sum(tracking_data['reward'])
        avg_reward = total_reward / len(tracking_data['reward'])
        
        # Curiosity analysis
        avg_curiosity = sum(tracking_data['curiosity']) / len(tracking_data['curiosity'])
        max_curiosity = max(tracking_data['curiosity'])
        min_curiosity = min(tracking_data['curiosity'])
        
        # Novelty analysis
        avg_novelty = sum(tracking_data['novelty']) / len(tracking_data['novelty'])
        
        # Neural activity analysis
        avg_firing_rate = sum(tracking_data['firing_rate']) / len(tracking_data['firing_rate'])
        total_spikes = tracking_data['spikes'][-1]
        
        # Developmental analysis
        dev_stage_changes = [tracking_data['dev_stage'][i] - tracking_data['dev_stage'][i-1] 
                           for i in range(1, len(tracking_data['dev_stage']))]
        avg_dev_change = sum(dev_stage_changes) / len(dev_stage_changes) if dev_stage_changes else 0
        
        print(f"Simulation Statistics:")
        print(f"  Total Steps: {len(tracking_data['step'])}")
        print(f"  Total Reward: {total_reward:.3f}")
        print(f"  Average Reward: {avg_reward:.4f}")
        print(f"  Average Curiosity: {avg_curiosity:.3f}")
        print(f"  Curiosity Range: {min_curiosity:.3f} - {max_curiosity:.3f}")
        print(f"  Average Novelty: {avg_novelty:.3f}")
        print(f"  Average Firing Rate: {avg_firing_rate:.3f} Hz")
        print(f"  Total Spikes: {total_spikes}")
        print(f"  Average Developmental Change: {avg_dev_change:.3f}")
        
        # Behavioral analysis
        curiosity_increase = tracking_data['curiosity'][-1] - tracking_data['curiosity'][0]
        novelty_increase = tracking_data['novelty'][-1] - tracking_data['novelty'][0]
        firing_increase = tracking_data['firing_rate'][-1] - tracking_data['firing_rate'][0]
        
        print(f"\nBehavioral Changes:")
        print(f"  Curiosity Change: {curiosity_increase:.3f}")
        print(f"  Novelty Change: {novelty_increase:.3f}")
        print(f"  Firing Rate Change: {firing_increase:.3f} Hz")
        
        # Determine behavior type
        behavior_type = self.determine_behavior_type(avg_curiosity, avg_novelty, avg_firing_rate)
        
        return {
            'total_reward': total_reward,
            'avg_reward': avg_reward,
            'avg_curiosity': avg_curiosity,
            'curiosity_range': (min_curiosity, max_curiosity),
            'avg_novelty': avg_novelty,
            'avg_firing_rate': avg_firing_rate,
            'total_spikes': total_spikes,
            'avg_dev_change': avg_dev_change,
            'behavior_type': behavior_type
        }
    
    def determine_behavior_type(self, avg_curiosity, avg_novelty, avg_firing_rate):
        """Determine behavioral type based on metrics"""
        
        if avg_curiosity > 0.7 and avg_novelty > 0.7:
            return "Explorer"
        elif avg_curiosity > 0.5 and avg_novelty > 0.3:
            return "Balanced Explorer"
        elif avg_curiosity > 0.3 and avg_novelty > 0.1:
            return "Cautious Explorer"
        elif avg_firing_rate > 20:
            return "High Activity"
        elif avg_firing_rate > 10:
            return "Moderate Activity"
        else:
            return "Low Activity"
    
    def export_advanced_data(self, tracking_data, filename_prefix):
        """Export advanced tracking data"""
        
        # Create data directory
        output_dir = Path(f"advanced_experiment_{filename_prefix}")
        output_dir.mkdir(exist_ok=True)
        
        # Export tracking data as CSV
        import csv
        
        csv_file = output_dir / "tracking_data.csv"
        with open(csv_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['Step', 'Reward', 'Curiosity', 'Novelty', 
                           'Firing_Rate', 'Total_Spikes', 'New_Spikes', 
                           'Dev_Stage', 'Avg_Synaptic_Weight'])
            
            for i in range(len(tracking_data['step'])):
                writer.writerow([
                    tracking_data['step'][i],
                    tracking_data['reward'][i],
                    tracking_data['curiosity'][i],
                    tracking_data['novelty'][i],
                    tracking_data['firing_rate'][i],
                    tracking_data['spikes'][i],
                    tracking_data['spike_count'][i],
                    tracking_data['dev_stage'][i],
                    tracking_data['synaptic_weight'][i]
                ])
        
        print(f"Data exported to: {csv_file}")
        
        # Create summary report
        summary_file = output_dir / "summary.md"
        with open(summary_file, 'w') as f:
            f.write(f"# Advanced NLM Experiment Summary\n\n")
            f.write(f"**Date:** {self.get_date()}\n")
            f.write(f"**Configuration:** Learning (neurons=500, learningRate=0.02)\n")
            f.write(f"**Advanced Features:** {', '.join(k for k, v in self.advanced_features.items() if v)}\n\n")
        
        print(f"Summary exported to: {summary_file}")
    
    def get_date(self):
        """Get current date string"""
        import datetime
        return datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
```

#### 3. Run Advanced Experiment

```python
# Run advanced experiment
if __name__ == "__main__":
    print("Starting Advanced NLM Experiment...\n")
    
    # Create and setup experiment
    experiment = AdvancedExperiment()
    
    # Setup advanced brain
    brain, world, agent = experiment.setup_advanced_brain()
    
    # Run simulation
    tracking_data = experiment.run_advanced_simulation(brain, world, agent, 1000)
    
    # Analyze results
    results = experiment.analyze_advanced_results(tracking_data)
    
    # Export data
    experiment.export_advanced_data(tracking_data, "advanced_tutorial4")
    
    print(f"\n=== Advanced Experiment Complete ===")
    print(f"Behavior Type: {results['behavior_type']}")
    print(f"Total Reward: {results['total_reward']:.3f}")
    print(f"Average Performance: {results['avg_reward']:.4f}")
    
    print(f"\nExperiment data saved in 'advanced_experiment_advanced_tutorial4/'")
    print("Files include:")
    print("  - tracking_data.csv: Detailed step-by-step data")
    print("  - summary.md: Experiment summary")
```

### Expected Results
- Complex behavior patterns with curiosity and novelty
- Developmental stage progression
- Detailed neural activity tracking
- Synaptic weight evolution
- Exportable data for analysis

### Learning Objectives
1. Explore advanced NLM features
2. Understand development stages
3. Analyze complex behavioral patterns
4. Export and analyze experimental data
5. Create reproducible experimental setups

### Further Exploration
1. Test different developmental trajectories
2. Compare behavior across configurations
3. Analyze checkpointing functionality
4. Study replay system performance
5. Create comparative experiments

## Common Issues and Solutions

### 1. "Brain initialization failed"

**Solution:**
```python
# Ensure configuration is correct
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize!
```

### 2. "Agent initialization failed"

**Solution:**
```python
# Create brain first
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

# Create and initialize agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Agent needs world!
```

### 3. "Visualization not working"

**Solution:**
```bash
# Check if visualization is available
nlm_phase7_demo --help

# Run with visualization
nlm_phase7_demo --steps 500 --width 80 --height 40

# Run with export for analysis
nlm_phase7_demo --steps 1000 --export results --save-frames
```

### 4. "Python bindings not working"

**Solution:**
```bash
# Check Python installation
python -c "import pynlm; print(pynlm.__version__)"

# Reinstall if needed
pip install --force-reinstall pynlm
```

## Summary

These tutorials provide a comprehensive introduction to NLM (Neural Learning Machine) from basic concepts to advanced features. They cover:

1. **Tutorial 1:** Basic brain simulation
2. **Tutorial 2:** Brain-world-agent interaction
3. **Tutorial 3:** Learning experiment design
4. **Tutorial 4:** Advanced features and capabilities

Each tutorial builds on the previous one, allowing users to progressively learn and explore NLM's capabilities. The tutorials include:

- Step-by-step instructions
- Code examples
- Expected results
- Learning objectives
- Further exploration suggestions

These tutorials serve as both educational resources and practical demonstrations of NLM's features, making it accessible to users with different levels of expertise.
