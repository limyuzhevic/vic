# NLM (Neural Learning Machine) - Advanced User Guide

## Overview

This guide covers advanced features, commands, and customization options for experienced users who want to get the most out of NLM's neural simulation capabilities.

---

## Advanced Command-Line Interface

### NLM Command System

NLM supports a comprehensive command-line interface for advanced control and configuration. All commands follow the pattern:

```bash
nlm [command] [options]
```

### Core Commands

#### `nlm run`
Run a simulation with advanced configuration options.

```bash
nlm run [options]
```

**Options:**
- `--config-file <file>`: Path to configuration file
- `--neurons <count>`: Number of neurons (overrides config)
- `--steps <count>`: Number of simulation steps
- `--dt <value>`: Time step duration
- `--checkpoint <dir>`: Checkpoint directory
- `--verbose`: Enable verbose logging
- `--quiet`: Suppress output

#### `nlm develop`
Enable and control brain development phases.

```bash
nlm develop [options]
```

**Options:**
- `--stage <stage>`: Target developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
- `--speed <multiplier>`: Development speed multiplier
- `--plasticity <factor>`: Plasticity factor
- `--continue`: Continue current development

#### `nlm memory`
Advanced memory system control.

```bash
nlm memory [options] <action>
```

**Actions:**
- `inspect <id>`: Inspect specific memory episode
- `search <pattern>`: Search memory for pattern
- `consolidate`: Force memory consolidation
- `replay <id>`: Replay specific episode
- `stats`: Show memory statistics

**Options:**
- `--type <type>`: Memory type (working, episodic, associative)
- `--age <steps>`: Memory age threshold
- `--quality <value>`: Quality threshold

#### `nlm neuromodulate`
Advanced neuromodulation control.

```bash
nlm neuromodulate [options] <neurotransmitter> <level>
```

**Neurotransmitters:**
- `dopamine`: Reward prediction error, motivation
- `serotonin`: Mood regulation, cognitive flexibility
- `norepinephrine`: Arousal, attention
- `acetylcholine`: Learning, memory formation
- `curiosity`: Exploration drive
- `novelty`: Novelty detection

**Options:**
- `--duration <steps>`: Duration of modulation
- `--smooth`: Smooth transition to new level
- `--pattern <name>`: Modulatory pattern (pulse, ramp, sustain)

#### `nlm predict`
Advanced prediction and forecasting.

```bash
nlm predict [options]
```

**Options:**
- `--horizon <steps>`: Prediction horizon
- `--confidence <value>`: Confidence threshold
- `--filter <type>`: Prediction filter (temporal, spatial, pattern)
- `--compare`: Compare predictions with actuals

#### `nlm explore`
Exploration and discovery commands.

```bash
nlm explore [options]
```

**Options:**
- `--curiosity <level>`: Curiosity level (0-1)
- `--novelty <level>`: Novelty sensitivity (0-1)
- `--exploration-rate <rate>`: Random exploration probability
- `--reinforcement-learning`: Enable RL integration

---

## Configuration File System

### Advanced Configuration

NLM supports advanced configuration through JSON files with nested structures:

```json
{
  "brain": {
    "neuron_count": 5000,
    "region_count": 5,
    "connection_probability": 0.15,
    "plasticity": {
      "stdp": {
        "enable": true,
        "learning_rate": 0.002,
        "tau_plus": 25.0,
        "tau_minus": 25.0
      },
      "hebbian": {
        "enable": true,
        "learning_rate": 0.001
      }
    }
  },
  "neuromodulation": {
    "dopamine": {
      "base_level": 0.1,
      "learning_rate": 0.05,
      "effect_strength": 1.0
    },
    "serotonin": {
      "base_level": 0.05,
      "flexibility_factor": 0.8
    },
    "curiosity": {
      "base_rate": 0.3,
      "max_rate": 0.9,
      "decay_rate": 0.99
    }
  },
  "cognition": {
    "planning": {
      "depth": 8,
      "confidence_threshold": 0.7,
      "exploration_bonus": 0.2
    },
    "concept_formation": {
      "stability_threshold": 0.6,
      "similarity_threshold": 0.8,
      "merge_threshold": 0.9
    }
  },
  "world": {
    "width": 30.0,
    "height": 30.0,
    "vision": {
      "width": 16,
      "height": 16
    },
    "resources": [
      {"count": 10, "energy": 50},
      {"count": 5, "energy": 100}
    ],
    "hazards": [
      {"count": 3, "damage": 25}
    ]
  }
}
```

### Configuration Commands

#### `nlm config`
Advanced configuration management.

```bash
nlm config [options] <command>
```

**Commands:**
- `set <key> <value>`: Set configuration value
- `get <key>`: Get configuration value
- `list`: List all configuration keys
- `validate`: Validate configuration
- `merge <file>`: Merge configuration from file
- `export <file>`: Export configuration to file

**Options:**
- `--format <format>`: Output format (json, yaml, ini)
- `--scope <scope>`: Configuration scope (brain, neuromod, cognition, world)

#### `nlm experiment`
Run scientific experiments with advanced control.

```bash
nlm experiment [options] <experiment_name>
```

**Built-in Experiments:**
- `memory_retention`: Test memory retention over time
- `prediction_accuracy`: Measure prediction accuracy
- `learning_curves`: Track learning progress
- `development_stages`: Monitor development progression
- `curiosity_effects`: Analyze curiosity-driven exploration

**Options:**
- `--repeats <count>`: Number of repetitions
- `--parameters <params>`: Override experiment parameters
- `--metrics <metrics>`: Additional metrics to collect
- `--statistical-test`: Perform statistical analysis

---

## Brain State Commands

### Advanced Brain State Management

#### `nlm brain-state`
Advanced brain state inspection and manipulation.

```bash
nlm brain-state [options] <command>
```

**Commands:**
- `dump`: Export complete brain state
- `diff <file>`: Compare with saved state
- `merge <file>`: Merge with saved state
- `statistics`: Show comprehensive statistics
- `connectivity`: Show connectivity matrix
- `plasticity`: Show plasticity state
- `neuromodulation`: Show neuromodulation levels

**Options:**
- `--format <format>`: Output format (binary, json, csv, human)
- `--filter <type>`: Filter type (neurons, synapses, regions)
- `--limit <count>`: Limit number of elements

#### `nlm checkpoint`
Advanced checkpoint management.

```bash
nlm checkpoint [options] <command>
```

**Commands:**
- `list`: List available checkpoints
- `create`: Create new checkpoint
- `restore <id>`: Restore checkpoint
- `cleanup`: Remove old checkpoints
- `verify <id>`: Verify checkpoint integrity
- `benchmark`: Benchmark checkpoint performance

**Options:**
- `--directory <dir>`: Checkpoint directory
- `--max-count <count>`: Maximum number of checkpoints
- `--compression <level>`: Compression level (none, fast, balanced, best)

---

## Script and Automation

### Automation Scripts

#### `nlm-automation`
Automation framework for running complex experiments.

```bash
nlm-automation [options] <script_file>
```

**Script Format:**
```json
{
  "experiment": "memory_retention",
  "parameters": {
    "neuron_count": [1000, 5000],
    "duration": [100, 1000],
    "plasticity": [0.1, 1.0]
  },
  "repeats": 10,
  "output": {
    "format": "json",
    "path": "results/"
  },
  "monitoring": {
    "interval": 100,
    "metrics": ["firing_rate", "memory_recall", "prediction_accuracy"]
  }
}
```

### Automated Experiment Runner

```python
# nlm_runner.py
import pynlm
import json
import time

def run_automated_experiment(experiment_config):
    """Run an automated experiment with NLM."""
    
    # Load configuration
    with open(experiment_config['parameters_file']) as f:
        params = json.load(f)
    
    # Create brain with parameters
    config = pynlm.createDefaultConfig()
    for key, value in params.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(**experiment_config['world_config'])
    agent.initialize(world)
    
    # Enable advanced features based on experiment type
    if experiment_config.get('enable_reward_learning', False):
        agent.enableRewardModulation(True)
    
    if experiment_config.get('enable_curiosity', False):
        agent.enableCuriosity(True)
    
    if experiment_config.get('enable_development', False):
        agent.enableDevelopment(True)
    
    # Run experiment
    start_time = time.time()
    
    for step in range(experiment_config['steps']):
        # Execute experiment-specific actions
        if experiment_config['type'] == 'memory_retention':
            # Store sensory input in memory
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Periodically retrieve and score memory
            if step % 100 == 0:
                # Calculate memory retention score
                memory_score = calculate_memory_retention(brain)
                
                # Store results
                store_experiment_result(step, memory_score)
                
        elif experiment_config['type'] == 'prediction_accuracy':
            # Use prediction system
            if brain.getPredictionSystem():
                prediction = brain.getPredictionSystem().predict()
                # Compare with actual
                actual = get_actual_from_world(world)
                accuracy = calculate_prediction_accuracy(prediction, actual)
                
        # Default agent loop
        else:
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Save results
    save_experiment_results(experiment_config['output_path'], 
                           get_experiment_results())
    
    return {
        'duration': end_time - start_time,
        'steps_completed': experiment_config['steps'],
        'final_metrics': get_final_metrics(brain, agent, world)
    }

def calculate_memory_retention(brain):
    """Calculate memory retention score."""
    if not brain.getEpisodicMemory():
        return 0.0
    
    episodes = brain.getEpisodicMemory().getEpisodeCount()
    if episodes == 0:
        return 0.0
    
    # Simple retention calculation based on episode count and novelty
    retention_score = std::min(1.0, static_cast<float>(episodes) / 100.0)
    
    return retention_score

def save_experiment_results(path, results):
    """Save experiment results to file."""
    import json
    from datetime import datetime
    
    result_data = {
        'timestamp': datetime.now().isoformat(),
        'results': results,
        'config': get_experiment_config()
    }
    
    with open(f"{path}/experiment_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json", 'w') as f:
        json.dump(result_data, f, indent=2)

# Automation control script
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='NLM Automation Runner')
    parser.add_argument('script', help='Automation script file')
    parser.add_argument('--output', help='Output directory', default='experiments/')
    parser.add_argument('--continue-on-error', action='store_true', 
                       help='Continue running even if steps fail')
    
    args = parser.parse_args()
    
    # Load and run automation script
    with open(args.script) as f:
        automation_config = json.load(f)
    
    automation_config['output_path'] = args.output
    
    print(f"Running automation script: {args.script}")
    print(f"Output directory: {args.output}")
    
    # Run automation
    results = run_automated_experiment(automation_config)
    
    print(f"\nAutomation completed!")
    print(f"Duration: {results['duration']:.2f} seconds")
    print(f"Steps completed: {results['steps_completed']}")
    print(f"Final metrics: {results['final_metrics']}")
```

---

## Advanced Python API Extensions

### Extended NLM Python Library

The Python library has been extended with advanced functions for power users:

#### `pynlm.advanced`
Module for advanced operations.

```python
import pynlm
import pynlm.advanced as pynlm_adv

# Advanced configuration management
config = pynlm.createDefaultConfig()
config = pynlm_adv.load_config("advanced_config.yaml")

# Complex brain operations
brain = pynlm_adv.create_brain_with_architecture(
    "hierarchical",
    neuron_count=10000,
    region_count=8,
    plasticity_rules=["stdp", "hebbian", "structural"]
)

# Advanced simulation control
pynlm_adv.run_simulation(
    brain,
    world,
    steps=5000,
    checkpoint_interval=1000,
    advanced_monitoring=True,
    adaptive_timestep=True
)

# Memory analysis
memory_stats = pynlm_adv.analyze_memory_patterns(brain, window_size=100)
concept_network = pynlm_adv.extract_concept_network(brain)

# Performance optimization
optimized_brain = pynlm_adv.optimize_brain(brain, target_performance="high")
profiling_data = pynlm_adv.profile_brain_performance(brain)
```

#### `pynlm.scientific`
Scientific computing utilities.

```python
import pynlm.scientific as pynlm_sci

# Statistical analysis
results = pynlm_sci.statistical_analysis(
    "learning_curve",
    data=[0.1, 0.3, 0.6, 0.8, 0.9],
    model="exponential_growth"
)

# Mathematical modeling
model = pynlm_sci.create_neural_model(
    equations=[
        "dV/dt = (I - gV + noise)/C",
        "dW/dt = A_post * exp(-(t-t_post)/tau) - A_pre * exp(-(t-t_pre)/tau)"
    ],
    parameters={"C": 1.0, "g": 0.1, "A_post": 0.01}
)

# Data visualization
plot_data = pynlm_sci.prepare_plot_data(brain, metrics=[
    "firing_rate", "memory_recall", "prediction_accuracy",
    "novelty_sensitivity", "curiosity_level"
])
pynlm_sci.create_advanced_plots(plot_data, output_format="pdf")
```

### Advanced Configuration Builder

```python
# nlm_config_builder.py
import pynlm
import pynlm.advanced as pynlm_adv

class NLMConfigurationBuilder:
    """Builder class for creating complex NLM configurations."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.metadata = {}
    
    def set_brain_parameters(self, neurons=1000, regions=1, density=0.1):
        """Configure brain parameters."""
        self.config.set("brain.neuron_count", neurons)
        self.config.set("brain.region_count", regions)
        self.config.set("brain.connection_probability", density)
        return self
    
    def set_plasticity_system(self, stdp_enabled=True, hebbian_enabled=True, 
                              structural_enabled=True, learning_rate=0.001):
        """Configure plasticity systems."""
        plasticity_config = {
            "stdp": {"enable": stdp_enabled, "learning_rate": learning_rate},
            "hebbian": {"enable": hebbian_enabled, "learning_rate": learning_rate},
            "structural": {"enable": structural_enabled, "rate": learning_rate}
        }
        self.config.set("plasticity", plasticity_config)
        return self
    
    def set_neuromodulation(self, neuromod_config):
        """Configure neuromodulation systems."""
        self.config.set("neuromodulation", neuromod_config)
        return self
    
    def set_cognitive_systems(self, planning_depth=5, concept_stability=0.7):
        """Configure cognitive systems."""
        cognition_config = {
            "planning": {"depth": planning_depth, "confidence_threshold": 0.7},
            "concept_formation": {"stability_threshold": concept_stability}
        }
        self.config.set("cognition", cognition_config)
        return self
    
    def set_world_parameters(self, width=20, height=20, vision_width=8, vision_height=8):
        """Configure world parameters."""
        world_config = {
            "width": width, "height": height,
            "vision": {"width": vision_width, "height": vision_height}
        }
        self.config.set("world", world_config)
        return self
    
    def add_advanced_features(self, **kwargs):
        """Add advanced features."""
        advanced_features = {}
        
        if kwargs.get('enable_reward_modulation', False):
            advanced_features['reward_modulation'] = {
                "enabled": True,
                "learning_rate": kwargs.get('reward_learning_rate', 0.05),
                "prediction_window": kwargs.get('prediction_window', 10)
            }
        
        if kwargs.get('enable_curiosity', False):
            advanced_features['curiosity'] = {
                "enabled": True,
                "base_rate": kwargs.get('curiosity_base_rate', 0.3),
                "exploration_bonus": kwargs.get('exploration_bonus', 0.2)
            }
        
        if kwargs.get('enable_development', False):
            advanced_features['development'] = {
                "enabled": True,
                "stages": kwargs.get('development_stages', 5),
                "plasticity_decay": kwargs.get('plasticity_decay', 0.8)
            }
        
        self.config.set("advanced_features", advanced_features)
        return self
    
    def set_experiment_parameters(self, experiment_type="standard", 
                                  duration=1000, repetitions=5):
        """Configure experiment parameters."""
        experiment_config = {
            "type": experiment_type,
            "duration": duration,
            "repetitions": repetitions,
            "metrics": ["firing_rate", "memory_recall", "prediction_accuracy"]
        }
        self.config.set("experiment", experiment_config)
        return self
    
    def add_custom_rule(self, rule_name, rule_config):
        """Add custom rule to configuration."""
        if 'custom_rules' not in self.config:
            self.config.set('custom_rules', {})
        
        self.config['custom_rules'][rule_name] = rule_config
        return self
    
    def save_config(self, filepath, format='json'):
        """Save configuration to file."""
        if format == 'json':
            import json
            with open(filepath, 'w') as f:
                json.dump(self.config, f, indent=2)
        elif format == 'yaml':
            import yaml
            with open(filepath, 'w') as f:
                yaml.dump(self.config, f)
        
        print(f"Configuration saved to {filepath}")
        return self
    
    def build(self):
        """Build final configuration."""
        # Add metadata
        self.metadata['built_at'] = time.time()
        self.metadata['nlm_version'] = "2.0"
        self.metadata['builder_version'] = "1.0"
        
        self.config['metadata'] = self.metadata
        
        return self.config

# Usage example
if __name__ == "__main__":
    builder = NLMConfigurationBuilder()
    
    config = (builder
              .set_brain_parameters(neurons=5000, regions=5)
              .set_plasticity_system(learning_rate=0.002)
              .set_neuromodulation({
                  "dopamine": {"base_level": 0.1, "learning_rate": 0.05},
                  "serotonin": {"base_level": 0.05, "flexibility_factor": 0.8}
              })
              .set_cognitive_systems(planning_depth=8, concept_stability=0.6)
              .set_world_parameters(width=30, height=30)
              .add_advanced_features(
                  enable_reward_modulation=True,
                  enable_curiosity=True,
                  enable_development=True,
                  reward_learning_rate=0.03
              )
              .set_experiment_parameters(
                  experiment_type="memory_retention",
                  duration=2000,
                  repetitions=10
              )
              .add_custom_rule("novelty_threshold", {"base": 0.5, "decay": 0.95})
              .build())
    
    # Save built configuration
    builder.save_config("advanced_config.json", "json")
    
    print("Advanced configuration built successfully!")
```

---

## Performance and Scalability

### Performance Optimization

#### `nlm-perf`
Performance analysis and optimization tool.

```bash
nlm-perf [options] <command>
```

**Commands:**
- `analyze`: Analyze performance characteristics
- `profile`: Profile CPU, memory, and I/O usage
- `optimize`: Apply automatic optimizations
- `benchmark`: Benchmark against different configurations
- `tune`: Parameter tuning for performance

**Options:**
- `--target <target>`: Performance target (throughput, latency, memory)
- `--metric <metric>`: Specific metric to optimize
- `--iterations <count>`: Number of benchmark iterations

### Scalability Options

#### Multi-Regional Brain

```python
# nlm_scalability.py
import pynlm
import pynlm.advanced as pynlm_adv

def create_multiregional_brain(num_regions=8, neurons_per_region=2000,
                              inter_region_connections=True):
    """Create a multi-regional brain architecture."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.region_count", num_regions)
    config.set("brain.neuron_count", neurons_per_region * num_regions)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get regions
    regions = []
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            regions.append(region)
    
    # Create inter-regional connections
    if inter_region_connections:
        for i, source_region in enumerate(regions):
            for j, target_region in enumerate(regions):
                if i != j:  # No self-connections
                    # Calculate connection strength based on distance
                    region_distance = abs(i - j)
                    connection_strength = std::pow(2.0, -static_cast<float>(region_distance)) * 0.1
                    
                    # Add inter-regional connection
                    brain.addInterRegionConnection(
                        source_region->getId(),
                        target_region->getId(),
                        connection_strength,
                        1  # Delay
                    )
    
    return brain, regions

def run_scalability_test(brain, num_steps=1000, step_size=100):
    """Run scalability test with varying brain sizes."""
    
    results = []
    
    for step in range(step_size, num_steps + 1, step_size):
        # Simulate
        brain.step(step)
        
        # Collect metrics
        metrics = {
            "step": step,
            "neurons_active": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "ei_ratio": brain.getExcitationInhibitionRatio(),
            "memory_activity": brain.getWorkingMemory().getMemoryActivity() if brain.getWorkingMemory() else 0,
            "novelty_level": brain.getNovelty().getLevel() if brain.getNovelty() else 0
        }
        
        results.append(metrics)
        
        # Log progress
        if step % 1000 == 0:
            print(f"Step {step}: {metrics['neurons_active']} neurons firing")
    
    return results

# Scalability analysis
if __name__ == "__main__":
    # Test different brain sizes
    brain_sizes = [1000, 2000, 5000, 10000]
    
    all_results = {}
    
    for size in brain_sizes:
        print(f"\nTesting brain with {size} neurons...")
        
        # Create brain with specific size
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        config.set("brain.region_count", std::max(1, size // 2000))  # 1 region per 2000 neurons
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run scalability test
        results = run_scalability_test(brain, num_steps=5000, step_size=500)
        all_results[f"{size}_neurons"] = results
        
        print(f"Completed test with {size} neurons")
    
    # Analyze results
    print("\nScalability Analysis Results:")
    for brain_size, results in all_results.items():
        neurons = int(brain_size.split('_')[0])
        final_firing_rate = results[-1]["avg_firing_rate"]
        memory_efficiency = results[-1]["memory_activity"] / neurons
        
        print(f"{brain_size}:")
        print(f"  Final firing rate: {final_firing_rate:.3f}")
        print(f"  Memory efficiency: {memory_efficiency:.6f}")
        print(f"  Total spikes: {results[-1]['total_spikes']}")
    
    # Performance comparison
    print("\nPerformance Comparison:")
    print("Metric                    | Small (1k) | Medium (2k) | Large (5k) | XL (10k)")
    print("--------------------------|-------------|-------------|------------|----------")
    
    metrics = ['avg_firing_rate', 'memory_activity', 'total_spikes']
    for metric in metrics:
        values = []
        for brain_size, results in all_results.items():
            values.append(results[-1][metric])
        
        print(f"{metric:25} | {values[0]:11.3f} | {values[1]:11.3f} | {values[2]:11.3f} | {values[3]:11.3f}")
```

---

## Advanced Agent Control

### Custom Agent Behaviors

#### `nlm-agent-control`
Advanced agent control and customization.

```bash
nlm-agent-control [options] <action>
```

**Actions:**
- `create`: Create custom agent
- `modify`: Modify agent behavior
- `train`: Train agent with custom curriculum
- `evaluate`: Evaluate agent performance
- `export`: Export agent behavior

### Custom Agent Example

```python
# nlm_custom_agent.py
import pynlm
import pynlm.advanced as pynlm_adv
import numpy as np

class CustomAgent:
    """Custom agent with advanced behavior control."""
    
    def __init__(self, brain, world, config=None):
        self.brain = brain
        self.world = world
        self.config = config or self.get_default_config()
        
        # Agent state
        self.step_count = 0
        self.reward_history = []
        self.curiosity_history = []
        self.memory_trace = []
        
        # Behavior parameters
        self.exploration_rate = self.config.get('exploration_rate', 0.1)
        self.exploitation_rate = self.config.get('exploitation_rate', 0.7)
        self.epsilon_greedy = self.config.get('epsilon_greedy', 0.05)
        
        # Neural modulation
        self.dopamine_sensitivity = self.config.get('dopamine_sensitivity', 1.0)
        self.curiosity_threshold = self.config.get('curiosity_threshold', 0.3)
        
        # Cognitive biases
        self.risk_tolerance = self.config.get('risk_tolerance', 0.5)
        self_exploration_decay = self.config.get('exploration_decay', 0.995)
        self.adaptation_rate = self.config.get('adaptation_rate', 0.01)
        
        # Initialize advanced systems
        self.initialize_advanced_systems()
    
    def get_default_config(self):
        """Get default configuration for custom agent."""
        return {
            'exploration_rate': 0.15,
            'exploitation_rate': 0.7,
            'epsilon_greedy': 0.05,
            'dopamine_sensitivity': 1.0,
            'curiosity_threshold': 0.3,
            'risk_tolerance': 0.5,
            'exploration_decay': 0.995,
            'adaptation_rate': 0.01,
            'memory_capacity': 100,
            'planning_depth': 3,
            'concept_update_frequency': 50
        }
    
    def initialize_advanced_systems(self):
        """Initialize advanced agent systems."""
        # Curiosity system
        self.curiosity_system = pynlm_adv.CuriositySystem()
        self.curiosity_system.set_parameters(
            base_rate=self.config['curiosity_threshold'],
            max_rate=1.0,
            decay_rate=self.config['exploration_decay']
        )
        
        # Memory buffer
        self.memory_buffer = pynlm_adv.MemoryBuffer(
            capacity=self.config['memory_capacity'],
            encoding_type='neural_pattern'
        )
        
        # Planning system
        self.planner = pynlm_adv.NeuralPlanner(
            depth=self.config['planning_depth'],
            confidence_threshold=0.7,
            exploration_bonus=0.2
        )
        
        # neuromodulation controller
        self.neuromod_controller = pynlm_adv.NeuromodController()
        self.neuromod_controller.set_sensitivity(
            dopamine=self.config['dopamine_sensitivity']
        )
        
        # Adaptation system
        self.adaptation_system = pynlm_adv.AdaptationSystem(
            learning_rate=self.config['adaptation_rate']
        )
    
    def select_action_advanced(self, state, available_actions):
        """Advanced action selection using multiple strategies."""
        
        # Strategy 1: Epsilon-greedy with curiosity bonus
        base_action = self.select_epsilon_greedy(state, available_actions)
        curiosity_bonus = self.calculate_curiosity_bonus(state)
        
        # Strategy 2: Planning-based selection
        planned_action = self.planner.select_action(state, available_actions)
        
        # Strategy 3: Neuromodulation-based adjustment
        neuromod_action = self.apply_neuromodulation_bias(base_action, state)
        
        # Combine strategies
        final_action = self.combine_strategies(
            base_action, planned_action, neuromod_action,
            curiosity_bonus
        )
        
        # Record action for learning
        self.record_action_selection(state, final_action, curiosity_bonus)
        
        return final_action
    
    def select_epsilon_greedy(self, state, available_actions):
        """Epsilon-greedy action selection with neural biases."""
        
        if np.random.random() < self.config['epsilon_greedy']:
            # Random exploration
            return np.random.choice(available_actions)
        
        # Calculate action values based on neural activity
        action_values = {}
        
        for action in available_actions:
            # Get neural representation of action
            neural_rep = self.get_action_neural_representation(action, state)
            
            # Calculate value based on multiple factors
            value = 0.0
            
            # 1. Direct neural activity
            if self.brain.getMotorNeurons():
                motor_activity = self.calculate_motor_activity(action)
                value += motor_activity * 0.4
            
            # 2. Memory association strength
            memory_strength = self.get_memory_association_strength(action, state)
            value += memory_strength * 0.3
            
            # 3. Habit strength from previous rewards
            habit_strength = self.get_habit_strength(action)
            value += habit_strength * 0.2
            
            # 4. Curiosity bonus
            curiosity_bonus = self.calculate_curiosity_bonus(state, action)
            value += curiosity_bonus * 0.1
            
            action_values[action] = value
        
        # Select action with highest value
        max_value = max(action_values.values())
        best_actions = [act for act, val in action_values.items() if val == max_value]
        
        return np.random.choice(best_actions)
    
    def calculate_curiosity_bonus(self, state, action=None):
        """Calculate curiosity bonus for exploration."""
        
        # Get current curiosity level
        curiosity_level = self.curiosity_system.get_current_level()
        
        # Calculate novelty based on state changes
        if len(self.memory_trace) > 0:
            current_pattern = self.extract_neural_pattern(state)
            novelty_score = self.calculate_pattern_novelty(current_pattern)
        else:
            novelty_score = 0.5  # Default novelty
        
        # Combine curiosity and novelty
        curiosity_bonus = curiosity_level * novelty_score * 0.5
        
        return curiosity_bonus
    
    def apply_neuromodulation_bias(self, action, state):
        """Apply neuromodulation-based action bias."""
        
        # Get current neuromodulation levels
        neuromod_levels = self.neuromod_controller.get_current_levels()
        
        # Dopamine bias: approach behavior
        if neuromod_levels['dopamine'] > 0.5:
            # Bias towards approach actions
            approach_actions = ['move_forward', 'interact', 'eat']
            if action in approach_actions:
                return action
        
        # Serotonin bias: cognitive flexibility
        if neuromod_levels['serotonin'] > 0.3:
            # Increase exploration tendency
            exploration_actions = ['turn_left', 'turn_right', 'look_left', 'look_right']
            return np.random.choice(exploration_actions)
        
        # Norepinephrine bias: focus and attention
        if neuromod_levels['norepinephrine'] > 0.4:
            # Bias towards goal-directed actions
            goal_actions = ['move_forward', 'interact']
            return np.random.choice(goal_actions)
        
        return action
    
    def combine_strategies(self, base_action, planned_action, neuromod_action, curiosity_bonus):
        """Combine different action selection strategies."""
        
        # Strategy weights
        weights = {
            'base': 0.3,
            'planned': 0.4,
            'neuromod': 0.2,
            'curiosity': 0.1
        }
        
        # Calculate weighted scores (simplified for demonstration)
        base_score = self.get_action_score(base_action)
        planned_score = self.get_action_score(planned_action)
        neuromod_score = self.get_action_score(neuromod_action)
        
        combined_score = (
            base_score * weights['base'] +
            planned_score * weights['planned'] +
            neuromod_score * weights['neuromod'] +
            curiosity_bonus * weights['curiosity']
        )
        
        # Select action with highest combined score
        actions_scores = {
            base_action: base_score,
            planned_action: planned_score,
            neuromod_action: neuromod_score
        }
        
        best_action = max(actions_scores, key=actions_scores.get)
        return best_action
    
    def calculate_motor_activity(self, action):
        """Calculate motor neuron activity for specific action."""
        
        # Get motor neuron populations
        motor_populations = self.get_motor_neuron_populations()
        
        # Calculate activity based on action type
        action_to_population = {
            'move_forward': 'forward_population',
            'move_backward': 'backward_population',
            'turn_left': 'turn_left_population',
            'turn_right': 'turn_right_population',
            'interact': 'interact_population',
            'wait': 'wait_population'
        }
        
        population_name = action_to_population.get(action, 'wait_population')
        
        if population_name in motor_populations:
            population = motor_populations[population_name]
            return population.get_average_activity()
        
        return 0.0
    
    def get_memory_association_strength(self, action, state):
        """Get strength of memory association for action."""
        
        # Retrieve relevant memory patterns
        relevant_memories = self.memory_buffer.retrieve_relevant(state)
        
        # Calculate association strength
        total_strength = 0.0
        for memory in relevant_memories:
            if memory.action == action:
                total_strength += memory.strength
        
        return total_strength / std::max(1, relevant_memories.size())
    
    def get_habit_strength(self, action):
        """Get habit strength from previous rewards."""
        
        # Get recent rewards for this action
        action_rewards = []
        for step_info in self.reward_history:
            if step_info['action'] == action:
                action_rewards.append(step_info['reward'])
        
        if action_rewards.empty():
            return 0.0
        
        # Calculate average reward and convert to habit strength
        avg_reward = action_rewards.mean()
        habit_strength = (avg_reward + 1.0) / 2.0  # Normalize to 0-1
        
        return habit_strength
    
    def run_custom_simulation(self, num_steps):
        """Run simulation with custom agent behavior."""
        
        for step in range(num_steps):
            # Get current state
            state = self.get_environment_state()
            
            # Select action using advanced strategies
            available_actions = self.get_available_actions()
            action = self.select_action_advanced(state, available_actions)
            
            # Execute action in environment
            result = self.execute_action(action)
            
            # Store experience
            self.store_experience(state, action, result)
            
            # Update systems
            self.update_systems(state, result)
            
            # Log progress
            if step % 100 == 0:
                self.log_progress(step, state, action, result)
        
        return self.get_final_metrics()
    
    def store_experience(self, state, action, result):
        """Store experience for learning."""
        
        experience = {
            'step': self.step_count,
            'state': state,
            'action': action,
            'reward': result['reward'],
            'novelty': result['novelty'],
            'success': result['success']
        }
        
        # Store in memory buffer
        self.memory_buffer.store(experience)
        
        # Update reward history
        self.reward_history.append({
            'step': self.step_count,
            'action': action,
            'reward': result['reward'],
            'novelty': result['novelty']
        })
        
        # Update curiosity history
        curiosity_level = self.curiosity_system.get_current_level()
        self.curiosity_history.append({
            'step': self.step_count,
            'curiosity': curiosity_level,
            'novelty': result['novelty']
        })
        
        self.step_count += 1
    
    def get_environment_state(self):
        """Get current environment state."""
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Extract features
        state = {
            'sensory': percept,
            'agent_position': self.get_agent_position(),
            'agent_orientation': self.get_agent_orientation(),
            'agent_energy': self.get_agent_energy(),
            'time': self.world.getSimulationTime()
        }
        
        return state
    
    def execute_action(self, action):
        """Execute action and return result."""
        
        # Apply action to world
        result = self.world.applyMotorCommand(
            self.convert_to_motor_command(action),
            self.world.getSimulationTime()
        )
        
        # Get feedback
        feedback = {
            'reward': result.reward,
            'novelty': self.calculate_action_novelty(action),
            'success': result.success,
            'new_objects': self.get_new_objects_created(),
            'novel_situations': self.detect_novel_situations()
        }
        
        return feedback
    
    def update_systems(self, state, result):
        """Update agent systems based on experience."""
        
        # Update curiosity system
        self.curiosity_system.update(state, result['novelty'])
        
        # Update adaptation system
        self.adaptation_system.update(state, result)
        
        # Update neuromodulation
        self.neuromod_controller.update(result)
        
        # Update memory
        self.memory_buffer.update(state, result)
        
        # Update planner
        self.planner.update(state, result)
    
    def log_progress(self, step, state, action, result):
        """Log simulation progress."""
        
        print(f"Step {step}:")
        print(f"  Action: {action}")
        print(f"  Reward: {result['reward']:.3f}")
        print(f"  Novelty: {result['novelty']:.3f}")
        print(f"  Curiosity: {self.curiosity_system.get_current_level():.3f}")
        print(f"  Memory size: {self.memory_buffer.size()}")
        print(f"  Adaptation: {self.adaptation_system.get_adaptation_level():.3f}")
        print()
    
    def get_final_metrics(self):
        """Get final simulation metrics."""
        
        return {
            'total_steps': self.step_count,
            'total_reward': sum(e['reward'] for e in self.reward_history),
            'avg_reward': np.mean([e['reward'] for e in self.reward_history]),
            'max_reward': np.max([e['reward'] for e in self.reward_history]),
            'total_novelty': sum(e['novelty'] for e in self.reward_history),
            'final_curiosity': self.curiosity_system.get_current_level(),
            'memory_efficiency': self.memory_buffer.get_efficiency(),
            'adaptation_level': self.adaptation_system.get_adaptation_level()
        }
    
    def convert_to_motor_command(self, action_name):
        """Convert action name to MotorCommand enum."""
        # This would need to be implemented based on actual MotorCommand enum values
        action_map = {
            'move_forward': pynlm.ActionType.MoveForward,
            'move_backward': pynlm.ActionType.MoveBackward,
            'turn_left': pynlm.ActionType.TurnLeft,
            'turn_right': pynlm.ActionType.TurnRight,
            'interact': pynlm.ActionType.Interact,
            'wait': pynlm.ActionType.Wait
        }
        
        return action_map.get(action_name, pynlm.ActionType.Wait)
    
    def get_agent_position(self):
        """Get agent position from world."""
        body = self.world.getAgentBody()
        return (body.x, body.y)
    
    def get_agent_orientation(self):
        """Get agent orientation from world."""
        body = self.world.getAgentBody()
        return body.orientation
    
    def get_agent_energy(self):
        """Get agent energy from world."""
        body = self.world.getAgentBody()
        return body.energy
    
    def get_new_objects_created(self):
        """Get number of new objects created this step."""
        # Implementation would depend on world interface
        return 0
    
    def detect_novel_situations(self):
        """Detect novel situations in environment."""
        # Implementation would depend on world interface
        return 0
    
    def calculate_action_novelty(self, action):
        """Calculate novelty of action."""
        # Get recent actions
        recent_actions = [e['action'] for e in self.reward_history[-10:]]
        
        # Calculate how often this action has been used recently
        action_frequency = recent_actions.count(action) / len(recent_actions)
        
        # Novelty is inversely related to frequency
        novelty = 1.0 - action_frequency
        
        return novelty
    
    def extract_neural_pattern(self, state):
        """Extract neural pattern from state."""
        
        # This would typically extract patterns from brain activity
        # For demonstration, we'll create a simple feature vector
        pattern = []
        
        # Add sensory features
        if 'sensory' in state:
            sensory = state['sensory']
            pattern.extend(sensory)
        
        # Add positional features
        if 'agent_position' in state:
            pattern.extend(state['agent_position'])
        
        # Add orientation
        if 'agent_orientation' in state:
            pattern.append(state['agent_orientation'])
        
        return pattern
    
    def calculate_pattern_novelty(self, pattern):
        """Calculate novelty of neural pattern."""
        
        # This would compare with stored patterns to calculate novelty
        # For demonstration, return a simple score
        return 0.5
    
    def get_motor_neuron_populations(self):
        """Get motor neuron populations from brain."""
        
        # This would extract motor neuron groups from brain
        # Implementation depends on brain interface
        return {}
    
    def get_action_neural_representation(self, action, state):
        """Get neural representation of action."""
        
        # This would retrieve or compute neural representation
        # Implementation depends on brain interface
        return {}
    
    def get_action_score(self, action):
        """Get score for action (simplified)."""
        
        # This would compute action score based on various factors
        # For demonstration, return random score
        return np.random.random()

# Example usage of custom agent
if __name__ == "__main__":
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create custom agent with advanced configuration
    agent_config = {
        'exploration_rate': 0.2,
        'exploitation_rate': 0.6,
        'epsilon_greedy': 0.05,
        'dopamine_sensitivity': 1.2,
        'curiosity_threshold': 0.4,
        'risk_tolerance': 0.6,
        'exploration_decay': 0.99,
        'adaptation_rate': 0.02,
        'memory_capacity': 200,
        'planning_depth': 5
    }
    
    agent = CustomAgent(brain, world, agent_config)
    
    # Run custom simulation
    print("Starting custom simulation with advanced agent...")
    metrics = agent.run_custom_simulation(1000)
    
    print("\nSimulation completed!")
    print(f"Final metrics: {metrics}")
```