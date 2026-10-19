# Advanced NLM Configuration Examples
# ================================

## Overview

This document provides advanced configuration examples for NLM (Neural Learning Machine)
Phase 6 and beyond. These examples demonstrate how to configure complex brain systems,
advanced features, and optimize performance for different use cases.

## Configuration Schema

Before diving into examples, understand the configuration structure:

```
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 100,
            "trace_decay": 0.95
        },
        "episodic_memory": {
            "max_episodes": 1000,
            "replay_frequency": 0.1
        }
    },
    "cognition": {
        "planner": {
            "horizon": 10,
            "exploration_rate": 0.1
        },
        "concept_formation": {
            "similarity_threshold": 0.8,
            "cluster_count": 10
        }
    },
    "development": {
        "stage_duration": {
            "initial": 100,
            "critical_period": 500,
            "maturation": 1000
        },
        "plasticity_rate": {
            "initial": 1.0,
            "critical_period": 2.0,
            "maturation": 0.5
        }
    }
}
```

## Advanced Configuration Examples

### 1. High-Performance Brain Configuration

```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.6,
        "initial_weight_std": 0.2
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.005,
            "tau_plus": 30.0,
            "tau_minus": 30.0
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0005,
            "pruning_rate": 0.00005
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": true
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 500,
            "trace_decay": 0.98
        },
        "episodic_memory": {
            "max_episodes": 5000,
            "replay_frequency": 0.2
        }
    },
    "development": {
        "stage_duration": {
            "initial": 50,
            "critical_period": 200,
            "maturation": 500
        }
    }
}
```

**Purpose**: Balanced configuration for complex tasks requiring good memory and learning capabilities.

### 2. Fast-Learning, Small Brain Configuration

```json
{
    "brain": {
        "neuron_count": 200,
        "region_count": 2,
        "connection_probability": 0.15,
        "initial_weight_mean": 0.7,
        "initial_weight_std": 0.15
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "tau_plus": 15.0,
            "tau_minus": 15.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.05
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 2.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "cognition": {
        "planner": {
            "horizon": 5,
            "exploration_rate": 0.3
        }
    }
}
```

**Purpose**: Fast learning and adaptation for quick tasks with limited resources.

### 3. Long-Term Memory and Development Configuration

```json
{
    "brain": {
        "neuron_count": 3000,
        "region_count": 3,
        "connection_probability": 0.08,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 50.0,
            "tau_minus": 50.0
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0002,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 0.8
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 1000,
            "trace_decay": 0.99
        },
        "episodic_memory": {
            "max_episodes": 20000,
            "replay_frequency": 0.05
        }
    },
    "development": {
        "stage_duration": {
            "initial": 100,
            "critical_period": 500,
            "maturation": 2000,
            "aging": 5000
        },
        "plasticity_rate": {
            "initial": 2.0,
            "critical_period": 5.0,
            "maturation": 0.3,
            "aging": 0.1
        }
    }
}
```

**Purpose**: Long-term memory consolidation and developmental stages for lifelong learning.

### 4. High-Frequency Trading Simulation Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 5,
        "connection_probability": 0.2,
        "initial_weight_mean": 0.8,
        "initial_weight_std": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 10.0,
            "tau_minus": 10.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.02
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 3.0
        },
        "curiosity": {
            "enable": true
        }
    },
    "cognition": {
        "planner": {
            "horizon": 3,
            "exploration_rate": 0.5
        },
        "attention": {
            "selection_strength": 2.0,
            "competition_strength": 1.5
        }
    },
    "prediction": {
        "prediction_error_threshold": 0.01,
        "confidence_threshold": 0.9
    }
}
```

**Purpose**: Fast decision-making and prediction for high-frequency scenarios.

### 5. Developmental Robot Configuration

```json
{
    "brain": {
        "neuron_count": 1500,
        "region_count": 3,
        "connection_probability": 0.12,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.15
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.008,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0002
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.2
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "memory": {
        "episodic_memory": {
            "max_episodes": 10000,
            "replay_frequency": 0.15
        }
    },
    "development": {
        "stage_duration": {
            "initial": 50,
            "critical_period": 300,
            "maturation": 1000
        },
        "plasticity_rate": {
            "initial": 1.5,
            "critical_period": 3.0,
            "maturation": 0.8
        }
    }
}
```

**Purpose**: Developmental learning and maturation for robotic applications.

## Performance Optimization Examples

### 1. Memory-Optimized Configuration

```json
{
    "brain": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.05
    },
    "memory": {
        "working_memory": {
            "capacity": 50,
            "trace_decay": 0.9
        },
        "episodic_memory": {
            "max_episodes": 1000,
            "replay_frequency": 0.05
        }
    },
    "performance": {
        "enable_memory_pools": true,
        "enable_event_queues": true,
        "enable_vectorization": true,
        "simd_width": 8
    }
}
```

**Purpose**: Reduce memory usage while maintaining performance for embedded systems.

### 2. Speed-Optimized Configuration

```json
{
    "brain": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.08
    },
    "performance": {
        "enable_memory_pools": true,
        "enable_event_queues": true,
        "enable_vectorization": true,
        "enable_parallel_processing": true,
        "simd_width": 16,
        "thread_count": 4
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    }
}
```

**Purpose**: Maximize processing speed using parallel processing and SIMD.

## Environment-Specific Configurations

### 1. Cloud Computing Configuration

```json
{
    "brain": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.03
    },
    "performance": {
        "enable_memory_pools": true,
        "enable_event_queues": true,
        "enable_vectorization": true,
        "enable_parallel_processing": true,
        "simd_width": 32,
        "thread_count": 8,
        "checkpoint_interval": 1000
    },
    "memory": {
        "episodic_memory": {
            "max_episodes": 50000,
            "replay_frequency": 0.02
        }
    }
}
```

**Purpose**: Leverage cloud computing resources for large-scale simulations.

### 2. Embedded System Configuration

```json
{
    "brain": {
        "neuron_count": 300,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "performance": {
        "enable_memory_pools": true,
        "enable_event_queues": true,
        "enable_vectorization": true,
        "simd_width": 4,
        "thread_count": 2,
        "checkpoint_interval": 500
    },
    "power_management": {
        "enable_dynamic_voltage": true,
        "enable_sleep_mode": true,
        "wakeup_interval": 100
    }
}
```

**Purpose**: Optimize for low-power embedded systems with limited resources.

## Configuration Validation and Testing

### 1. Configuration Validation Script

```python
import pynlm
import json

def validate_config(config_file):
    """Validate a NLM configuration file."""
    # Load configuration
    config = pynlm.createDefaultConfig()
    
    if config.loadFromFile(config_file):
        print(f"✓ Loaded configuration from: {config_file}")
        
        # Validate configuration
        if config.validate():
            print("✓ Configuration validation passed")
            
            # Show configuration summary
            print("\nConfiguration Summary:")
            print(config.summary())
            
            return True
        else:
            print("✗ Configuration validation failed")
            return False
    else:
        print(f"✗ Could not load configuration from: {config_file}")
        return False

# Example usage
if __name__ == "__main__":
    config_file = "advanced_config.json"
    if validate_config(config_file):
        print("Configuration is valid!")
    else:
        print("Configuration has errors!")
```

### 2. Performance Benchmark Script

```python
import pynlm
import time
import json
from datetime import datetime

def run_benchmark(config_file, steps, benchmark_type):
    """Run a performance benchmark."""
    # Load configuration
    config = pynlm.createDefaultConfig()
    
    if config.loadFromFile(config_file):
        print(f"✓ Loaded configuration from: {config_file}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Warm up
        for i in range(100):
            brain.step(i)
        
        # Run benchmark
        start_time = time.time()
        
        for i in range(steps):
            brain.step(i)
        
        end_time = time.time()
        
        # Calculate metrics
        elapsed = end_time - start_time
        steps_per_second = steps / elapsed
        
        # Create results
        results = {
            "benchmark_type": benchmark_type,
            "steps": steps,
            "total_time": elapsed,
            "steps_per_second": steps_per_second,
            "neurons_per_second": brain.getTotalNeuronCount() * steps_per_second,
            "spikes_per_second": brain.getTotalSpikeCount() / elapsed,
            "total_neurons": brain.getTotalNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "timestamp": datetime.now().isoformat()
        }
        
        # Save results
        filename = f"benchmark_{benchmark_type}_{int(time.time())}.json"
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\n=== Benchmark Results ===")
        print(f"Benchmark type: {benchmark_type}")
        print(f"Total steps: {steps}")
        print(f"Total time: {elapsed:.2f} seconds")
        print(f"Steps per second: {steps_per_second:.1f}")
        print(f"Results saved to: {filename}")
        
        return results
    else:
        print(f"✗ Could not load configuration from: {config_file}")
        return None

# Example usage
if __name__ == "__main__":
    config_file = "advanced_config.json"
    steps = 5000
    benchmark_type = "spiking"
    
    results = run_benchmark(config_file, steps, benchmark_type)
    
    if results:
        print(f"\nBenchmark completed successfully!")
        print(f"Performance: {results['steps_per_second']:.1f} steps/second")
```

## Integration Examples

### 1. Complete Agent Simulation

```python
import pynlm
import time

def run_complete_agent_simulation(config_file, steps):
    """Run a complete agent simulation with brain and world."""
    # Load configuration
    config = pynlm.createDefaultConfig()
    if config_file and config.loadFromFile(config_file):
        print(f"✓ Loaded configuration from: {config_file}")
    
    # Create components
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    print(f"Starting {steps} steps of simulation...")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # Print final results
    print(f"\n=== Simulation Complete ===")
    print(f"Total steps: {steps}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    return {
        "steps": steps,
        "total_neurons": brain.getTotalNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "development_stage": brain.getDevelopmentalStage()
    }

# Example usage
if __name__ == "__main__":
    config_file = "advanced_config.json"
    steps = 1000
    
    results = run_complete_agent_simulation(config_file, steps)
    
    if results:
        print(f"\nAgent simulation completed successfully!")
        print(f"Performance: {results['average_firing_rate']:.3f} firing rate")
```

### 2. Batch Experiment Runner

```python
import pynlm
import time

def run_batch_experiments(config_file, experiment_configs):
    """Run multiple experiments in batch."""
    all_results = []
    
    for exp_config in experiment_configs:
        print(f"\n=== Running Experiment: {exp_config['name']} ===")
        
        # Load configuration
        config = pynlm.createDefaultConfig()
        if config_file and config.loadFromFile(config_file):
            print(f"✓ Loaded configuration from: {config_file}")
        
        # Override with experiment-specific config
        for key, value in exp_config.get('config_overrides', {}).items():
            config.set(key, value)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run experiment
        steps = exp_config.get('steps', 1000)
        
        start_time = time.time()
        for i in range(steps):
            brain.step(i)
        end_time = time.time()
        
        # Collect results
        result = {
            "experiment_name": exp_config['name'],
            "steps": steps,
            "total_time": end_time - start_time,
            "total_neurons": brain.getTotalNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "development_stage": brain.getDevelopmentalStage(),
            "timestamp": time.time()
        }
        
        all_results.append(result)
        print(f"✓ Experiment completed: {exp_config['name']}")
    
    return all_results

# Example usage
if __name__ == "__main__":
    config_file = "advanced_config.json"
    
    experiment_configs = [
        {
            "name": "Fast Learning",
            "steps": 500,
            "config_overrides": {
                "brain.neuron_count": 500,
                "plasticity.stdp.learning_rate": 0.01
            }
        },
        {
            "name": "Slow Learning",
            "steps": 2000,
            "config_overrides": {
                "brain.neuron_count": 1000,
                "plasticity.stdp.learning_rate": 0.001
            }
        },
        {
            "name": "Memory Focus",
            "steps": 1000,
            "config_overrides": {
                "brain.neuron_count": 800,
                "memory.episodic_memory.max_episodes": 5000
            }
        }
    ]
    
    results = run_batch_experiments(config_file, experiment_configs)
    
    if results:
        print(f"\n=== Batch Results ===")
        print(f"Total experiments: {len(results)}")
        for result in results:
            print(f"  {result['experiment_name']}: {result['steps']} steps in {result['total_time']:.2f}s")
```

## Conclusion

These advanced configuration examples provide a comprehensive guide for configuring NLM
Phase 6 and beyond. Users can adapt these examples for specific use cases, optimize for
different hardware platforms, or create custom configurations for their particular
application requirements.

The key is to understand the trade-offs between different configuration options and
choose the right balance for the intended application. Always validate configurations
before running experiments to ensure they meet your requirements and system capabilities.