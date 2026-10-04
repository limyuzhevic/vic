# NLM Python API - Advanced Features

## Enhanced Brain Configuration

### Custom Brain Factory Functions

```python
import pynlm

# Create brain with specific architecture
def createHierarchicalBrain(neuron_counts, connection_probabilities):
    """
    Create a multi-region hierarchical brain.
    
    Args:
        neuron_counts: List of neuron counts per region
        connection_probabilities: List of connection probabilities between regions
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.region_count", len(neuron_counts))
    config.set("brain.neuron_count", sum(neuron_counts))
    
    # Create regions with different characteristics
    regions = []
    neuron_offset = 0
    for i, count in enumerate(neuron_counts):
        region_config = config.copy()
        region_config.set("brain.neuron_count", count)
        region_config.set("brain.region_id", i)
        if i > 0:
            region_config.set("brain.input_connections", connection_probabilities[i-1])
        
        brain = pynlm.createBrain(region_config)
        brain.initialize()
        regions.append(brain)
        neuron_offset += count
    
    return regions

# Advanced configuration options
def configureAdvancedBrain(brain, options):
    """
    Apply advanced configuration to a brain.
    
    Args:
        brain: Brain object to configure
        options: Dictionary of advanced options
    """
    if "plasticity_enabled" in options:
        brain.enablePlasticity(options["plasticity_enabled"])
    
    if "neuromodulation_enabled" in options:
        brain.enableNeuromodulation(options["neuromodulation_enabled"])
    
    if "development_enabled" in options:
        brain.enableDevelopment(options["development_enabled"])
    
    if "curiosity_enabled" in options:
        brain.enableCuriosity(options["curiosity_enabled"])
    
    # Advanced plasticity parameters
    if "plasticity_params" in options:
        params = options["plasticity_params"]
        brain.setPlasticityParams(params)
```

## Advanced Agent Features

### Multi-Agent Systems

```python
import pynlm
import numpy as np

class MultiAgentSystem:
    """System with multiple interacting agents."""
    
    def __init__(self, num_agents, world_size):
        self.num_agents = num_agents
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=world_size, height=world_size, 
                           visionWidth=6, visionHeight=6)
        self.world.reset()
        
        # Create agents
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            # Add agent-specific configuration
            config.set("agent.id", i)
            config.set("agent.strategy", "curiosity")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Configure agent behavior
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
            
            self.agents.append(agent)
    
    def runSimulation(self, steps):
        """Run simulation with multiple agents."""
        for step in range(steps):
            self.world.update(0.1)
            
            # Each agent gets its own percept
            for agent in self.agents:
                percept = self.world.getSensoryPercept()
                
                # Agent-specific processing
                agent.processSensoryInput(percept)
                agent.brain.step(step)
                
                # Each agent may have different action strategies
                action = agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Agent-specific learning
                reward = self.calculateAgentReward(agent, step)
                agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.1)
    
    def calculateAgentReward(self, agent, step):
        """Calculate reward for individual agent."""
        # Example: reward for exploration and learning
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        # Complex reward function
        reward = 0.5 * curiosity + 0.3 * novelty
        
        # Add some exploration bonus
        if step % 100 == 0:
            reward += 0.2
            
        return reward
```

### Parallel Processing

```python
import pynlm
import threading
import multiprocessing
import time

class ParallelNLMExecutor:
    """Execute NLM simulations in parallel for faster research."""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or multiprocessing.cpu_count()
        self.pool = None
    
    def runExperiment(self, experiment_config, num_replications=10):
        """
        Run multiple experiment replications in parallel.
        
        Args:
            experiment_config: Configuration for the experiment
            num_replications: Number of parallel runs
        """
        if self.pool is None:
            self.pool = multiprocessing.Pool(self.num_workers)
        
        # Prepare experiment configurations
        configs = []
        for i in range(num_replications):
            config = experiment_config.copy()
            # Add random seed for each replication
            config.set("random_seed", 42 + i)
            configs.append(config)
        
        # Run experiments in parallel
        start_time = time.time()
        results = self.pool.map(self._runSingleExperiment, configs)
        end_time = time.time()
        
        if self.pool:
            self.pool.close()
            self.pool.join()
            self.pool = None
        
        return {
            "results": results,
            "total_time": end_time - start_time,
            "avg_time_per_experiment": (end_time - start_time) / num_replications,
            "experiments_per_second": num_replications / (end_time - start_time)
        }
    
    def _runSingleExperiment(self, config):
        """Run a single experiment (called in worker processes)."""
        import pynlm
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(config.getOr("brain.max_steps", 1000)):
            brain.step(step)
            
            # Collect metrics
            if step % 100 == 0:
                # Sample metrics for performance
                firing_rate = brain.getAverageFiringRate()
                spike_count = brain.getTotalSpikeCount()
        
        return {
            "final_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "final_synaptic_weight": brain.getExcitationInhibitionRatio()
        }
    
    def close(self):
        """Clean up pool resources."""
        if self.pool:
            self.pool.close()
            self.pool.join()
            self.pool = None
```

## Advanced Monitoring and Analysis

### Performance Monitoring

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

class BrainPerformanceMonitor:
    """Monitor and analyze brain performance over time."""
    
    def __init__(self, brain):
        self.brain = brain
        self.metrics_history = {
            'firing_rate': [],
            'spike_count': [],
            'synaptic_weight': [],
            'curiosity': [],
            'novelty': [],
            'dopamine': [],
            'timestamps': []
        }
        self.start_time = time.time()
    
    def recordMetrics(self):
        """Record current brain metrics."""
        self.metrics_history['firing_rate'].append(
            self.brain.getAverageFiringRate()
        )
        self.metrics_history['spike_count'].append(
            self.brain.getTotalSpikeCount()
        )
        self.metrics_history['synaptic_weight'].append(
            self.brain.getExcitationInhibitionRatio()
        )
        
        # Note: These would require additional brain state access
        # self.metrics_history['curiosity'].append(self.brain.getCuriosityLevel())
        # self.metrics_history['novelty'].append(self.brain.getNoveltyLevel())
        # self.metrics_history['dopamine'].append(self.brain.getDopamineLevel())
        
        self.metrics_history['timestamps'].append(time.time() - self.start_time)
    
    def analyzeLearning(self):
        """Analyze learning patterns from recorded metrics."""
        firing_rate = np.array(self.metrics_history['firing_rate'])
        spike_count = np.array(self.metrics_history['spike_count'])
        
        # Calculate key statistics
        analysis = {
            'firing_rate_stability': np.std(firing_rate[-100:]) / np.mean(firing_rate[-100:]) if np.mean(firing_rate[-100:]) > 0 else 0,
            'spike_count_growth': (spike_count[-1] - spike_count[0]) / spike_count[0] if spike_count[0] > 0 else 0,
            'learning_phase': self._determineLearningPhase(firing_rate, spike_count),
            'convergence_point': self._findConvergencePoint(firing_rate),
            'critical_transitions': self._detectCriticalTransitions(spike_count)
        }
        
        return analysis
    
    def _determineLearningPhase(self, firing_rate, spike_count):
        """Determine current learning phase based on metrics."""
        if len(firing_rate) < 10:
            return "initialization"
        
        recent_rate = firing_rate[-10:]
        recent_spikes = spike_count[-10:]
        
        # Check for different learning phases
        rate_std = np.std(recent_rate)
        spike_trend = np.polyfit(range(len(recent_spikes)), recent_spikes, 1)[0]
        
        if rate_std < 0.1 and spike_trend < 0.01:
            return "stable_learning"
        elif spike_trend > 0.05:
            return "exploration"
        elif rate_std > 0.3:
            return "chaotic_learning"
        else:
            return "pattern_formation"
    
    def plotResults(self, save_path=None):
        """Plot monitoring results."""
        timestamps = self.metrics_history['timestamps']
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Firing rate over time
        axes[0, 0].plot(timestamps, self.metrics_history['firing_rate'])
        axes[0, 0].set_title('Firing Rate Over Time')
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('Firing Rate')
        
        # Spike count over time
        axes[0, 1].plot(timestamps, self.metrics_history['spike_count'])
        axes[0, 1].set_title('Spike Count Over Time')
        axes[0, 1].set_xlabel('Time (s)')
        axes[0, 1].set_ylabel('Total Spikes')
        
        # Synaptic weight over time
        axes[1, 0].plot(timestamps, self.metrics_history['synaptic_weight'])
        axes[1, 0].set_title('Synaptic Weight Over Time')
        axes[1, 0].set_xlabel('Time (s)')
        axes[1, 0].set_ylabel('Weight (E/I Ratio)')
        
        # Learning analysis
        analysis = self.analyzeLearning()
        axes[1, 1].bar(analysis.keys(), analysis.values())
        axes[1, 1].set_title('Learning Analysis')
        axes[1, 1].set_ylabel('Value')
        plt.xticks(rotation=45)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path)
        
        return fig
```

## Command Line Interface

### NLM Command Line Tools

```bash
# Install the NLM command line tools
pip install nlm-cli

# Basic brain simulation
nlm-sim --config configs/default.cfg --steps 1000 --output results.json

# Advanced simulation with monitoring
nlm-sim --config configs/default.cfg --steps 5000 --enable-monitoring --save-checkpoint

# Batch experiments
nlm-sim-batch --experiment-config exp_config.yaml --replications 50 --output-dir ./experiments

# Real-time visualization
nlm-visualize --brain-file checkpoint.bin --update-rate 30 --window-size 1200x800

# Performance benchmarking
nlm-benchmark --target-cpu 4 --num-runs 10 --output benchmark.json

# Automated learning analysis
nlm-analyze --input results.json --output analysis.html --metrics learning_curves

# Agent training
nlm-train --environment GridWorld --agent-config agent_config.yaml --episodes 1000

# Experiment reproduction
nlm-reproduce --experiment-id exp_001 --seed 42 --output-folder ./reproduction
```

### Configuration File Examples

#### Advanced Experiment Configuration

```yaml
# experiment_config.yaml
name: "complex_learning_experiment"
description: "Advanced neural learning experiment with multiple conditions"

brain_config:
  neuron_count: 2000
  region_count: 3
  connection_probability: 0.15
  plasticity_enabled: true
  neuromodulation_enabled: true
  development_enabled: true

environment_config:
  type: "GridWorld"
  width: 20
  height: 20
  vision_width: 8
  vision_height: 8
  max_energy: 100

experiment_params:
  max_steps: 5000
  num_episodes: 100
  checkpoint_interval: 500
  enable_replay: true
  enable_development: true

agents:
  - id: "agent_1"
    strategy: "curiosity_driven"
    learning_rate: 0.01
    exploration_rate: 0.2
  - id: "agent_2"
    strategy: "reward_based"
    learning_rate: 0.005
    exploration_rate: 0.1

analysis:
  track_firing_rate: true
  track_synaptic_changes: true
  track_neuromodulation: true
  generate_plots: true
  save_intermediate_results: true
```

#### Agent Configuration

```yaml
# agent_config.yaml
agent_id: "research_agent"
strategy: "multi_phase_learning"

learning_phases:
  - name: "exploration"
    duration: 1000
    exploration_rate: 0.5
    learning_rate: 0.01
  - name: "exploitation"
    duration: 2000
    exploration_rate: 0.1
    learning_rate: 0.005
  - name: "consolidation"
    duration: 1000
    exploration_rate: 0.05
    learning_rate: 0.001

neuromodulation:
  dopamine_enabled: true
  curiosity_enabled: true
  novelty_enabled: true
  prediction_error_enabled: true

structural_plasticity:
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001
  connectivity_threshold: 0.5

checkpointing:
  enabled: true
  interval: 1000
  path: "./checkpoints/agent_1/"
```

## Research and Development Features

### Experiment Management

```python
import pynlm
import pandas as pd
import json

class ExperimentManager:
    """Manage complex neuroscience experiments with NLM."""
    
    def __init__(self, experiment_name):
        self.experiment_name = experiment_name
        self.results = {}
        self.configurations = {}
        self.metadata = {
            'date_created': time.time(),
            'description': '',
            'researcher': '',
            'version': '1.0.0'
        }
    
    def addConfiguration(self, name, config, description):
        """Add a new experimental configuration."""
        self.configurations[name] = {
            'config': config,
            'description': description,
            'created': time.time()
        }
    
    def runExperiment(self, config_name, num_replications=1):
        """Run an experiment with given configuration."""
        if config_name not in self.configurations:
            raise ValueError(f"Unknown configuration: {config_name}")
        
        config = self.configurations[config_name]['config']
        replication_results = []
        
        for rep in range(num_replications):
            # Add replication-specific seed
            rep_config = config.copy()
            rep_config.set("experiment.replication", rep)
            rep_config.set("random.seed", 42 + rep * 1000)
            
            # Run experiment
            result = self._executeExperiment(rep_config)
            replication_results.append(result)
        
        self.results[config_name] = replication_results
        return replication_results
    
    def _executeExperiment(self, config):
        """Execute a single experiment run."""
        # Create brain with configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        metrics = {
            'steps_completed': 0,
            'final_firing_rate': 0.0,
            'total_spikes': 0,
            'learning_detected': False,
            'time_taken': 0.0
        }
        
        start_time = time.time()
        for step in range(config.getOr("brain.max_steps", 1000)):
            brain.step(step)
            
            # Record metrics periodically
            if step % 100 == 0:
                metrics['steps_completed'] = step
                metrics['final_firing_rate'] = brain.getAverageFiringRate()
                metrics['total_spikes'] = brain.getTotalSpikeCount()
        
        metrics['time_taken'] = time.time() - start_time
        metrics['learning_detected'] = self._detectLearning(brain)
        
        return metrics
    
    def _detectLearning(self, brain):
        """Detect if significant learning occurred."""
        # Simple learning detection based on metrics
        firing_rate = brain.getAverageFiringRate()
        spike_count = brain.getTotalSpikeCount()
        
        # Learning criteria: firing rate > 0.1 and spike count > 100
        return firing_rate > 0.1 and spike_count > 100
    
    def exportResults(self, format='json'):
        """Export experiment results to file."""
        if format == 'json':
            with open(f"{self.experiment_name}_results.json", 'w') as f:
                json.dump(self.results, f, indent=2)
        elif format == 'csv':
            # Convert to DataFrame for CSV export
            all_data = []
            for config_name, replications in self.results.items():
                for i, result in enumerate(replications):
                    result['config'] = config_name
                    result['replication'] = i
                    all_data.append(result)
            
            df = pd.DataFrame(all_data)
            df.to_csv(f"{self.experiment_name}_results.csv", index=False)
    
    def generateReport(self):
        """Generate a comprehensive experiment report."""
        report = f"Experiment Report: {self.experiment_name}\n"
        report += f"Generated: {time.ctime(time.time())}\n"
        report += f"Configurations tested: {len(self.configurations)}\n"
        report += f"Total experiment runs: {sum(len(reps) for reps in self.results.values())}\n\n"
        
        for config_name, replications in self.results.items():
            report += f"Configuration: {config_name}\n"
            report += f"Description: {self.configurations[config_name]['description']}\n"
            
            # Calculate statistics for this configuration
            firing_rates = [rep['final_firing_rate'] for rep in replications]
            spike_counts = [rep['total_spikes'] for rep in replications]
            
            report += f"Average firing rate: {sum(firing_rates)/len(firing_rates):.4f}\n"
            report += f"Average spike count: {sum(spike_counts)/len(spike_counts):.0f}\n"
            report += f"Learning detected: {sum(1 for rep in replications if rep['learning_detected'])} out of {len(replications)}\n"
            report += f"Average time per run: {sum(rep['time_taken'] for rep in replications)/len(replications):.2f}s\n\n"
        
        return report
```

## Research and Development Utilities

### Advanced Configuration Management

```python
import pynlm
from typing import Dict, List, Any
import hashlib

class AdvancedConfigManager:
    """Advanced configuration management for NLM experiments."""
    
    def __init__(self):
        self.config_registry = {}
        self.parameter_templates = {}
    
    def createParameterTemplate(self, name, base_config, variations):
        """
        Create a parameter template with variations for systematic experiments.
        
        Args:
            name: Template name
            base_config: Base configuration
            variations: Dictionary of parameter variations
        """
        template = {
            'name': name,
            'base_config': base_config,
            'variations': variations,
            'generated_configs': []
        }
        
        self.parameter_templates[name] = template
        return template
    
    def generateConfigs(self, template_name, param_combinations):
        """
        Generate multiple configuration combinations from a template.
        
        Args:
            template_name: Name of template to use
            param_combinations: List of parameter value combinations
        """
        template = self.parameter_templates[template_name]
        generated = []
        
        for i, combo in enumerate(param_combinations):
            # Create copy of base config
            config = template['base_config'].copy()
            
            # Apply variations based on combination
            for param_name, value in combo.items():
                if '.' in param_name:
                    # Handle nested parameters
                    section, param = param_name.split('.', 1)
                    if section not in config:
                        config[section] = {}
                    config[section][param] = value
                else:
                    config[param_name] = value
            
            # Add metadata
            config['template_name'] = template_name
            config['combination_index'] = i
            config['config_hash'] = self._hashConfig(config)
            
            generated.append(config)
            template['generated_configs'].append(config)
        
        return generated
    
    def _hashConfig(self, config):
        """Create hash of configuration for identification."""
        config_str = json.dumps(config, sort_keys=True)
        return hashlib.md5(config_str.encode()).hexdigest()[:8]
    
    def validateConfig(self, config):
        """Validate configuration parameters."""
        errors = []
        
        # Check for required parameters
        required_params = ['brain.neuron_count', 'simulation.timestep']
        for param in required_params:
            if not self._getNestedParam(config, param):
                errors.append(f"Missing required parameter: {param}")
        
        # Check parameter ranges
        if 'brain.neuron_count' in config:
            if config['brain.neuron_count'] <= 0:
                errors.append("brain.neuron_count must be positive")
        
        if 'simulation.timestep' in config:
            if config['simulation.timestep'] <= 0:
                errors.append("simulation.timestep must be positive")
        
        return errors
    
    def _getNestedParam(self, config, param_path):
        """Get nested parameter from configuration."""
        parts = param_path.split('.')
        current = config
        for part in parts:
            if isinstance(current, dict) and part in current:
                current = current[part]
            else:
                return None
        return current
    
    def exportConfig(self, config, format='json'):
        """Export configuration to file."""
        if format == 'json':
            with open(f"config_{config['config_hash']}.json", 'w') as f:
                json.dump(config, f, indent=2)
        elif format == 'python':
            # Export as Python code
            with open(f"config_{config['config_hash']}.py", 'w') as f:
                f.write("config = {")
                for key, value in config.items():
                    f.write(f"\n    '{key}': {repr(value)},")
                f.write("\n}\n")
    
    def importConfigs(self, file_patterns):
        """Import multiple configurations from files."""
        imported_configs = []
        
        for pattern in file_patterns:
            import glob
            for file_path in glob.glob(pattern):
                config = self._loadConfigFile(file_path)
                if config:
                    imported_configs.append(config)
        
        return imported_configs
    
    def _loadConfigFile(self, file_path):
        """Load configuration from file."""
        try:
            if file_path.endswith('.json'):
                with open(file_path, 'r') as f:
                    return json.load(f)
            elif file_path.endswith('.py'):
                # Load Python config file
                import importlib.util
                spec = importlib.util.spec_from_file_location("config", file_path)
                config_module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(config_module)
                return config_module.config
        except Exception as e:
            print(f"Error loading config from {file_path}: {e}")
            return None
```

## Performance Optimization Tools

### Memory Management

```python
import pynlm
import gc
import psutil
import time

class MemoryOptimizedNLM:
    """Memory-optimized NLM for large-scale simulations."""
    
    def __init__(self, enable_pooling=True, enable_compression=True):
        self.enable_pooling = enable_pooling
        self.enable_compression = enable_compression
        self.memory_monitor = MemoryMonitor()
        self.checkpoint_manager = CheckpointManager()
    
    def runOptimizedSimulation(self, config, steps, enable_checkpointing=True):
        """
        Run simulation with memory optimizations.
        
        Args:
            config: Brain configuration
            steps: Number of simulation steps
            enable_checkpointing: Enable checkpointing for memory management
        """
        # Monitor initial memory
        initial_memory = self.memory_monitor.getMemoryUsage()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Setup checkpointing if enabled
        if enable_checkpointing:
            checkpoint_path = f"./checkpoints/step_{steps}.bin"
            self.checkpoint_manager.setup(checkpoint_path)
        
        # Run simulation with memory management
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
            
            # Memory management at intervals
            if step % 1000 == 0:
                self._manageMemory(step)
                
                # Save checkpoint if enabled
                if enable_checkpointing and step % 5000 == 0:
                    self.checkpoint_manager.save(brain, step)
        
        # Save final state
        if enable_checkpointing:
            self.checkpoint_manager.save(brain, steps)
        
        end_time = time.time()
        final_memory = self.memory_monitor.getMemoryUsage()
        
        # Cleanup
        self._cleanupResources(brain)
        
        return {
            'brain': brain,
            'steps_completed': steps,
            'time_taken': end_time - start_time,
            'memory_peak': self.memory_monitor.getPeakMemory(),
            'memory_efficiency': (initial_memory / final_memory) if final_memory > 0 else 1.0
        }
    
    def _manageMemory(self, step):
        """Manage memory usage during simulation."""
        current_memory = self.memory_monitor.getMemoryUsage()
        peak_memory = self.memory_monitor.getPeakMemory()
        
        # Force garbage collection if memory is high
        if current_memory > peak_memory * 0.9:
            gc.collect()
            self.memory_monitor.resetPeak()
        
        # Enable compression if configured
        if self.enable_compression:
            self.checkpoint_manager.enableCompression()
        
        # Use memory pooling if configured
        if self.enable_pooling:
            self._useMemoryPooling()
    
    def _useMemoryPooling(self):
        """Use memory pooling for better allocation."""
        # Implementation would depend on specific memory pooling strategy
        pass
    
    def _cleanupResources(self, brain):
        """Cleanup resources after simulation."""
        # Explicit cleanup
        del brain
        gc.collect()

class MemoryMonitor:
    """Monitor memory usage during simulations."""
    
    def __init__(self):
        self.process = psutil.Process()
        self.memory_samples = []
        self.peak_memory = 0
    
    def getMemoryUsage(self):
        """Get current memory usage in MB."""
        memory_info = self.process.memory_info()
        current_memory = memory_info.rss / (1024 * 1024)  # Convert to MB
        self.memory_samples.append(current_memory)
        
        if current_memory > self.peak_memory:
            self.peak_memory = current_memory
        
        return current_memory
    
    def getPeakMemory(self):
        """Get peak memory usage."""
        return self.peak_memory
    
    def resetPeak(self):
        """Reset peak memory tracking."""
        self.peak_memory = 0
    
    def getMemoryStats(self):
        """Get memory statistics."""
        if not self.memory_samples:
            return {'current': 0, 'peak': 0, 'avg': 0}
        
        return {
            'current': self.memory_samples[-1],
            'peak': self.peak_memory,
            'avg': sum(self.memory_samples) / len(self.memory_samples),
            'samples': len(self.memory_samples)
        }
```

## Batch Processing and Distributed Computing

### Parallel Experiment Execution

```python
import pynlm
import multiprocessing as mp
import itertools
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import time

class DistributedNLMExecutor:
    """Execute NLM experiments in distributed computing environments."""
    
    def __init__(self, num_workers=None, use_threads=False):
        self.num_workers = num_workers or mp.cpu_count()
        self.use_threads = use_threads
        self.executor = None
    
    def __enter__(self):
        if self.use_threads:
            self.executor = ThreadPoolExecutor(max_workers=self.num_workers)
        else:
            self.executor = ProcessPoolExecutor(max_workers=self.num_workers)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.executor:
            self.executor.shutdown(wait=True)
            self.executor = None
    
    def executeBatch(self, experiment_configs, max_parallel=None):
        """
        Execute multiple experiments in parallel.
        
        Args:
            experiment_configs: List of experiment configurations
            max_parallel: Maximum number of parallel executions
        """
        if max_parallel is None:
            max_parallel = self.num_workers
        
        # Limit parallel executions
        config_batches = self._batchConfigs(experiment_configs, max_parallel)
        
        all_results = []
        
        for batch in config_batches:
            # Execute batch in parallel
            futures = []
            for config in batch:
                future = self.executor.submit(self._runSingleExperiment, config)
                futures.append(future)
            
            # Collect results
            for future in futures:
                result = future.result()
                all_results.append(result)
        
        return all_results
    
    def _batchConfigs(self, configs, batch_size):
        """Split configurations into batches."""
        for i in range(0, len(configs), batch_size):
            yield configs[i:i + batch_size]
    
    def _runSingleExperiment(self, config):
        """Run a single experiment (worker process)."""
        # Worker process imports pynlm
        import pynlm
        
        # Add worker-specific configuration
        config.set("worker.id", mp.current_process().pid)
        config.set("random.seed", 42 + mp.current_process().pid)
        
        # Run experiment
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(config.getOr("brain.max_steps", 1000)):
            brain.step(step)
        
        # Return results
        return {
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'worker_id': mp.current_process().pid,
            'step_count': config.getOr("brain.max_steps", 1000)
        }
    
    def executeGridSearch(self, parameter_grid, base_config):
        """
        Execute grid search over parameter combinations.
        
        Args:
            parameter_grid: Dictionary of parameter lists for grid search
            base_config: Base configuration
        """
        # Generate all parameter combinations
        param_names = list(parameter_grid.keys())
        param_values = list(parameter_grid.values())
        
        # Create all combinations
        combinations = list(itertools.product(*param_values))
        
        # Create experiment configs
        experiment_configs = []
        for combo in combinations:
            config = base_config.copy()
            
            for i, param_name in enumerate(param_names):
                config.set(param_name, combo[i])
            
            experiment_configs.append(config)
        
        # Execute in parallel
        return self.executeBatch(experiment_configs)
```

## Advanced Development Tools

### Configuration Builder

```python
class NLMConfigurationBuilder:
    """Builder for creating complex NLM configurations programmatically."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def setBrainParameters(self, neuron_count=None, region_count=None,
                          connection_probability=None, initial_weight_mean=None,
                          initial_weight_std=None, v_thresh=None, v_rest=None,
                          v_reset=None, tau_mem=None, tau_ref=None):
        """Set brain parameters."""
        if neuron_count is not None:
            self.config.set("brain.neuron_count", neuron_count)
        if region_count is not None:
            self.config.set("brain.region_count", region_count)
        if connection_probability is not None:
            self.config.set("brain.connection_probability", connection_probability)
        if initial_weight_mean is not None:
            self.config.set("brain.initial_weight_mean", initial_weight_mean)
        if initial_weight_std is not None:
            self.config.set("brain.initial_weight_std", initial_weight_std)
        if v_thresh is not None:
            self.config.set("brain.v_thresh", v_thresh)
        if v_rest is not None:
            self.config.set("brain.v_rest", v_rest)
        if v_reset is not None:
            self.config.set("brain.v_reset", v_reset)
        if tau_mem is not None:
            self.config.set("brain.tau_mem", tau_mem)
        if tau_ref is not None:
            self.config.set("brain.tau_ref", tau_ref)
        
        return self
    
    def setPlasticityParameters(self, enable_stdp=None, stdp_ltp_weight=None,
                               stdp_ltd_weight=None, stdp_tau=None,
                               enable_hebbian=None, hebbian_learning_rate=None,
                               enable_structural=None, synaptogenesis_rate=None,
                               pruning_rate=None):
        """Set plasticity parameters."""
        if enable_stdp is not None:
            self.config.set("plasticity.stdp.enable", enable_stdp)
        if stdp_ltp_weight is not None:
            self.config.set("plasticity.stdp.learning_rate", stdp_ltp_weight)
        if stdp_ltd_weight is not None:
            self.config.set("plasticity.stdp.tau_minus", stdp_ltd_weight)
        if stdp_tau is not None:
            self.config.set("plasticity.stdp.tau_plus", stdp_tau)
        if enable_hebbian is not None:
            self.config.set("plasticity.hebbian.enable", enable_hebbian)
        if hebbian_learning_rate is not None:
            self.config.set("plasticity.hebbian.learning_rate", hebbian_learning_rate)
        if enable_structural is not None:
            self.config.set("plasticity.structural.enable", enable_structural)
        if synaptogenesis_rate is not None:
            self.config.set("plasticity.structural.synaptogenesis_rate", synaptogenesis_rate)
        if pruning_rate is not None:
            self.config.set("plasticity.structural.pruning_rate", pruning_rate)
        
        return self
    
    def setNeuromodulationParameters(self, enable_dopamine=None, dopamine_scale=None,
                                     enable_curiosity=None, curiosity_threshold=None,
                                     enable_novelty=None, novelty_weight=None,
                                     enable_prediction_error=None,
                                     prediction_error_decay=None):
        """Set neuromodulation parameters."""
        if enable_dopamine is not None:
            self.config.set("neuromod.dopamine.enable", enable_dopamine)
        if dopamine_scale is not None:
            self.config.set("neuromod.dopamine.scale", dopamine_scale)
        if enable_curiosity is not None:
            self.config.set("neuromod.curiosity.enable", enable_curiosity)
        if curiosity_threshold is not None:
            self.config.set("neuromod.curiosity.threshold", curiosity_threshold)
        if enable_novelty is not None:
            self.config.set("neuromod.novelty.enable", enable_novelty)
        if novelty_weight is not None:
            self.config.set("neuromod.novelty.weight", novelty_weight)
        if enable_prediction_error is not None:
            self.config.set("neuromod.prediction_error.enable", enable_prediction_error)
        if prediction_error_decay is not None:
            self.config.set("neuromod.prediction_error.decay", prediction_error_decay)
        
        return self
    
    def setDevelopmentParameters(self, enable_development=None,
                                maturation_rate=None, plasticity_modifier=None,
                                critical_period_duration=None):
        """Set development parameters."""
        if enable_development is not None:
            self.config.set("development.enable", enable_development)
        if maturation_rate is not None:
            self.config.set("development.maturation_rate", maturation_rate)
        if plasticity_modifier is not None:
            self.config.set("development.plasticity_modifier", plasticity_modifier)
        if critical_period_duration is not None:
            self.config.set("development.critical_period_duration", critical_period_duration)
        
        return self
    
    def setMemoryParameters(self, enable_working_memory=None,
                           working_memory_capacity=None, enable_episodic=None,
                           episodic_memory_capacity=None, memory_decay_rate=None):
        """Set memory parameters."""
        if enable_working_memory is not None:
            self.config.set("memory.working.enable", enable_working_memory)
        if working_memory_capacity is not None:
            self.config.set("memory.working.capacity", working_memory_capacity)
        if enable_episodic is not None:
            self.config.set("memory.episodic.enable", enable_episodic)
        if episodic_memory_capacity is not None:
            self.config.set("memory.episodic.capacity", episodic_memory_capacity)
        if memory_decay_rate is not None:
            self.config.set("memory.decay_rate", memory_decay_rate)
        
        return self
    
    def setEnvironmentParameters(self, width=None, height=None, vision_width=None,
                               vision_height=None, max_energy=None,
                               timestep=None):
        """Set environment parameters."""
        if width is not None:
            self.config.set("environment.width", width)
        if height is not None:
            self.config.set("environment.height", height)
        if vision_width is not None:
            self.config.set("environment.vision_width", vision_width)
        if vision_height is not None:
            self.config.set("environment.vision_height", vision_height)
        if max_energy is not None:
            self.config.set("environment.max_energy", max_energy)
        if timestep is not None:
            self.config.set("simulation.timestep", timestep)
        
        return self
    
    def enableAdvancedFeatures(self, features):
        """Enable advanced features."""
        for feature in features:
            self.config.set(f"features.{feature}.enabled", True)
        
        return self
    
    def build(self):
        """Build the final configuration."""
        return self.config
```

## Documentation and Code Generation

### Automatic Documentation Generation

```python
import pynlm
import jinja2
from datetime import datetime

class NLMDocumentationGenerator:
    """Generate comprehensive documentation for NLM configurations and experiments."""
    
    def __init__(self, template_dir=None):
        self.template_dir = template_dir or "./templates"
        self.template_engine = jinja2.Environment(
            loader=jinja2.FileSystemLoader(self.template_dir),
            autoescape=True
        )
    
    def generateConfigDocumentation(self, config, output_format='markdown'):
        """
        Generate documentation for a configuration.
        
        Args:
            config: Configuration object
            output_format: Output format ('markdown', 'html', 'latex')
        """
        template_name = f'config_{output_format}.jinja'
        template = self.template_engine.get_template(template_name)
        
        # Extract configuration information
        config_info = self._extractConfigInfo(config)
        
        # Generate documentation
        documentation = template.render(
            config_info=config_info,
            generated_at=datetime.now(),
            nlm_version="Phase 6"
        )
        
        return documentation
    
    def _extractConfigInfo(self, config):
        """Extract configuration information for documentation."""
        info = {
            'brain': {},
            'plasticity': {},
            'neuromodulation': {},
            'memory': {},
            'development': {},
            'environment': {},
            'simulation': {}
        }
        
        # Helper to extract nested parameters
        def extractNested(prefix, params):
            for key, value in params.items():
                full_key = f"{prefix}.{key}" if prefix else key
                if isinstance(value, dict):
                    extractNested(full_key, value)
                else:
                    info[full_key] = value
        
        # Extract all parameters
        extractNested('', config)
        
        return info
    
    def generateExperimentReport(self, experiment_results, config, output_format='html'):
        """
        Generate comprehensive experiment report.
        
        Args:
            experiment_results: Dictionary of experiment results
            config: Configuration used
            output_format: Output format
        """
        template_name = f'experiment_{output_format}.jinja'
        template = self.template_engine.get_template(template_name)
        
        # Prepare report data
        report_data = {
            'experiment_name': experiment_results.get('name', 'Unnamed Experiment'),
            'configuration': config,
            'results': experiment_results.get('results', {}),
            'metrics': experiment_results.get('metrics', {}),
            'timestamp': datetime.now(),
            'analysis': self._analyzeResults(experiment_results)
        }
        
        # Generate report
        report = template.render(**report_data)
        
        return report
    
    def _analyzeResults(self, results):
        """Analyze experiment results."""
        analysis = {}
        
        # Add various analyses based on experiment type
        if 'firing_rates' in results:
            analysis['firing_rate_stats'] = {
                'mean': sum(results['firing_rates']) / len(results['firing_rates']),
                'std': self._calculateStd(results['firing_rates']),
                'min': min(results['firing_rates']),
                'max': max(results['firing_rates'])
            }
        
        if 'spike_counts' in results:
            analysis['spike_stats'] = {
                'total': sum(results['spike_counts']),
                'mean_per_run': sum(results['spike_counts']) / len(results['spike_counts']),
                'growth_rate': self._calculateGrowthRate(results['spike_counts'])
            }
        
        return analysis
    
    def _calculateStd(self, values):
        """Calculate standard deviation."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _calculateGrowthRate(self, values):
        """Calculate growth rate from sequence."""
        if len(values) <= 1:
            return 0.0
        
        start = values[0]
        end = values[-1]
        
        if start == 0:
            return end
        
        return (end - start) / start
```

## Testing and Validation Tools

### Automated Testing

```python
import pynlm
import unittest
import json
import tempfile

class NLMTestCase(unittest.TestCase):
    """Base test case for NLM testing."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.config.set("simulation.timestep", 0.001)
    
    def tearDown(self):
        """Clean up after test."""
        pass

class BrainConfigurationTest(NLMTestCase):
    """Test brain configuration."""
    
    def testConfigurationCreation(self):
        """Test that configuration is created correctly."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        self.assertEqual(brain.getTotalNeuronCount(), 100)
        self.assertTrue(brain.getTotalNeuronCount() > 0)
    
    def testStepExecution(self):
        """Test that simulation steps execute correctly."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        for step in range(100):
            brain.step(step)
        
        final_spikes = brain.getTotalSpikeCount()
        
        self.assertGreaterEqual(final_spikes, initial_spikes)

class PlasticityTest(NLMTestCase):
    """Test plasticity mechanisms."""
    
    def testSTDPLearning(self):
        """Test STDP learning mechanism."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Get initial weights
        region = brain.getRegion(0)
        initial_weights = [syn->getWeight() for syn in region->getSynapses()]
        
        # Apply correlated activity to trigger STDP
        self._applyCorrelatedActivity(brain)
        
        # Check for weight changes
        final_weights = [syn->getWeight() for syn in region->getSynapses()]
        
        # Calculate weight changes
        weight_changes = [final - initial for final, initial in zip(final_weights, initial_weights)]
        
        # Check that some weights changed
        changed_weights = [change for change in weight_changes if abs(change) > 0.01]
        
        self.assertGreater(len(changed_weights), 0)
    
    def _applyCorrelatedActivity(self, brain):
        """Apply correlated pre-post activity to trigger STDP."""
        for trial in range(50):
            # Get neurons
            region = brain.getRegion(0)
            neurons = region->getAllNeurons()
            
            if len(neurons) >= 2:
                pre_neuron = neurons[0]
                post_neuron = neurons[1]
                
                # Pre fires first
                pre_neuron->injectCurrent(60.0f)
                brain->step(trial * 2, trial * 2 * 0.001)
                
                # Then post fires
                post_neuron->injectCurrent(60.0f)
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
class IntegrationTestSuite:
    """Suite of integration tests for NLM."""
    
    def __init__(self):
        self.test_cases = []
    
    def addTestCase(self, test_case, name):
        """Add a test case to the suite."""
        self.test_cases.append({
            'test_case': test_case,
            'name': name,
            'start_time': time.time()
        })
    
    def runAllTests(self):
        """Run all tests in the suite."""
        results = []
        
        for test_case in self.test_cases:
            try:
                # Run test
                result = test_case['test_case'].run()
                execution_time = time.time() - test_case['start_time']
                
                results.append({
                    'name': test_case['name'],
                    'status': 'PASSED' if result else 'FAILED',
                    'execution_time': execution_time,
                    'result': result
                })
            except Exception as e:
                execution_time = time.time() - test_case['start_time']
                
                results.append({
                    'name': test_case['name'],
                    'status': 'ERROR',
                    'execution_time': execution_time,
                    'error': str(e)
                })
        
        return results
    
    def generateTestReport(self, results, output_format='json'):
        """Generate test report."""
        report = {
            'total_tests': len(results),
            'passed_tests': len([r for r in results if r['status'] == 'PASSED']),
            'failed_tests': len([r for r in results if r['status'] == 'FAILED']),
            'error_tests': len([r for r in results if r['status'] == 'ERROR']),
            'total_execution_time': sum(r['execution_time'] for r in results),
            'test_results': results
        }
        
        if output_format == 'json':
            return json.dumps(report, indent=2)
        elif output_format == 'xml':
            return self._generateXMLReport(report)
        else:
            return report
    
    def _generateXMLReport(self, report):
        """Generate XML test report."""
        xml = '<?xml version="1.0" encoding="UTF-8"?>\n'
        xml += '<testsuite>\n'
        xml += f'  <testsuite name="NLM Tests" tests="{report["total_tests"]}" '
        xml += f'passed="{report["passed_tests"]}" failed="{report["failed_tests"]}" '
        xml += f'errors="{report["error_tests"]}" '\
        xml += f'execution_time="{report["total_execution_time"]}">\n'
        
        for test in report['test_results']:
            xml += f'    <test name="{test["name"]}" '\
            xml += f'status="{test["status"]}" '\
            xml += f'execution_time="{test["execution_time"]}"\n'
            
            if test['status'] == 'ERROR':
                xml += f'      <error message="{test["error"]}"/>\n'
            
            xml += '    </test>\n'
        
        xml += '  </testsuite>\n'
        xml += '</testsuite>\n'
        
        return xml
```

## Command Line Interface

### NLM Command Line Tools

```python
import pynlm
import argparse
import json
import time
from typing import Dict, Any

class NLMCommandLineInterface:
    """Command line interface for NLM."""
    
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="NLM - Neural Learning Machine Command Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        self._setupCommands()
    
    def _setupCommands(self):
        """Setup command line commands."""
        # Basic commands
        subparsers = self.parser.add_subparsers(dest='command', help='Available commands')
        
        # Simulation command
        sim_parser = subparsers.add_parser('simulate', help='Run NLM simulation')
        sim_parser.add_argument('--config', type=str, help='Configuration file')
        sim_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        sim_parser.add_argument('--output', type=str, help='Output file for results')
        sim_parser.add_argument('--verbose', action='store_true', help='Enable verbose output')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Manage configurations')
        config_subparsers = config_parser.add_subparsers(dest='config_command', help='Configuration commands')
        
        config_list_parser = config_subparsers.add_parser('list', help='List configurations')
        config_load_parser = config_subparsers.add_parser('load', help='Load configuration')
        config_load_parser.add_argument('filename', type=str, help='Configuration file')
        
        # Analysis command
        analysis_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
        analysis_parser.add_argument('input', type=str, help='Input file with results')
        analysis_parser.add_argument('--output', type=str, help='Output file for analysis')
        analysis_parser.add_argument('--metrics', type=str, nargs='+', 
                                   default=['firing_rate', 'spike_count'], help='Metrics to analyze')
        
        # Monitor command
        monitor_parser = subparsers.add_parser('monitor', help='Monitor simulation')
        monitor_parser.add_argument('--config', type=str, help='Configuration file')
        monitor_parser.add_argument('--duration', type=int, default=60, help='Monitoring duration in seconds')
        monitor_parser.add_argument('--update-rate', type=float, default=1.0, help='Update rate in Hz')
        
        # Benchmark command
        benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmark')
        benchmark_parser.add_argument('--config', type=str, help='Configuration file')
        benchmark_parser.add_argument('--steps', type=int, default=1000, help='Number of steps to benchmark')
        benchmark_parser.add_argument('--workers', type=int, help='Number of worker processes')
        
        # Experiment command
        experiment_parser = subparsers.add_parser('experiment', help='Run experiment')
        experiment_parser.add_argument('--config', type=str, help='Experiment configuration')
        experiment_parser.add_argument('--replications', type=int, default=10, help='Number of replications')
        experiment_parser.add_argument('--output', type=str, help='Output directory for results')
    
    def run(self, args=None):
        """Run command line interface."""
        if args is None:
            args = sys.argv[1:]
        
        parsed_args = self.parser.parse_args(args)
        
        # Dispatch to command handler
        if parsed_args.command == 'simulate':
            self._handleSimulateCommand(parsed_args)
        elif parsed_args.command == 'config':
            self._handleConfigCommand(parsed_args)
        elif parsed_args.command == 'analyze':
            self._handleAnalyzeCommand(parsed_args)
        elif parsed_args.command == 'monitor':
            self._handleMonitorCommand(parsed_args)
        elif parsed_args.command == 'benchmark':
            self._handleBenchmarkCommand(parsed_args)
        elif parsed_args.command == 'experiment':
            self._handleExperimentCommand(parsed_args)
        else:
            self.parser.print_help()
    
    def _handleSimulateCommand(self, args):
        """Handle simulate command."""
        if args.config:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(args.config)
        else:
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Starting simulation for {args.steps} steps...")
        
        start_time = time.time()
        for step in range(args.steps):
            brain.step(step)
            
            if args.verbose and step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"{brain.getTotalSpikeCount()} total spikes")
        
        end_time = time.time()
        
        print(f"Simulation completed in {end_time - start_time:.2f} seconds")
        print(f"Final firing rate: {brain.getAverageFiringRate():.4f}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        
        # Save results if requested
        if args.output:
            results = {
                'steps_completed': args.steps,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'execution_time': end_time - start_time
            }
            
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"Results saved to {args.output}")
    
    def _handleConfigCommand(self, args):
        """Handle config command."""
        if args.config_command == 'list':
            self._listConfigurations()
        elif args.config_command == 'load':
            self._loadConfiguration(args.filename)
    
    def _listConfigurations(self):
        """List available configurations."""
        import glob
        
        config_files = glob.glob("*.json")
        config_files.extend(glob.glob("*.py"))
        
        print("Available configurations:")
        for config_file in sorted(config_files):
            print(f"  {config_file}")
    
    def _loadConfiguration(self, filename):
        """Load configuration from file."""
        if filename.endswith('.json'):
            with open(filename, 'r') as f:
                config_dict = json.load(f)
            
            config = pynlm.createDefaultConfig()
            for key, value in config_dict.items():
                if '.' in key:
                    section, param = key.split('.', 1)
                    # Note: This is a simplified implementation
                    print(f"  Loading {section}.{param} = {value}")
                else:
                    config.set(key, value)
            
            print(f"Configuration loaded from {filename}")
        else:
            print(f"Unsupported configuration file format: {filename}")
    
    def _handleAnalyzeCommand(self, args):
        """Handle analyze command."""
        try:
            with open(args.input, 'r') as f:
                data = json.load(f)
            
            print(f"Analyzing results from {args.input}")
            print(f"Metrics to analyze: {', '.join(args.metrics)}")
            
            # Simple analysis
            analysis = {}
            
            for metric in args.metrics:
                if metric in data:
                    values = data[metric]
                    analysis[metric] = {
                        'mean': sum(values) / len(values),
                        'min': min(values),
                        'max': max(values),
                        'std': self._calculateStd(values)
                    }
            
            # Save analysis if requested
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(analysis, f, indent=2)
                
                print(f"Analysis saved to {args.output}")
            
            # Print analysis
            print("\nAnalysis Results:")
            for metric, stats in analysis.items():
                print(f"  {metric}:")
                print(f"    Mean: {stats['mean']:.4f}")
                print(f"    Min: {stats['min']:.4f}")
                print(f"    Max: {stats['max']:.4f}")
                print(f"    Std: {stats['std']:.4f}")
        
        except FileNotFoundError:
            print(f"Error: File not found: {args.input}")
        except json.JSONDecodeError:
            print(f"Error: Invalid JSON in file: {args.input}")
    
    def _calculateStd(self, values):
        """Calculate standard deviation."""
        if len(values) <= 1:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def _handleMonitorCommand(self, args):
        """Handle monitor command."""
        if args.config:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(args.config)
        else:
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Starting monitoring for {args.duration} seconds at {args.update_rate} Hz")
        print("Press Ctrl+C to stop monitoring")
        
        start_time = time.time()
        
        try:
            while time.time() - start_time < args.duration:
                # Record metrics
                print(f"Time: {time.time() - start_time:.1f}s, "
                      f"Firing rate: {brain.getAverageFiringRate():.4f}, "
                      f"Spikes: {brain.getTotalSpikeCount()}")
                
                # Step brain
                brain.step(int(time.time() * 1000))  # Convert to milliseconds
                
                # Sleep for update interval
                time.sleep(1.0 / args.update_rate)
        
        except KeyboardInterrupt:
            print("\nMonitoring stopped by user")
        
        print("Monitoring completed")
    
    def _handleBenchmarkCommand(self, args):
        """Handle benchmark command."""
        if args.config:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(args.config)
        else:
            config = pynlm.createDefaultConfig()
        
        print(f"Running benchmark for {args.steps} steps")
        print(f"Number of workers: {args.workers if args.workers else 'auto'}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        
        for step in range(args.steps):
            brain.step(step)
        
        end_time = time.time()
        
        execution_time = end_time - start_time
        steps_per_second = args.steps / execution_time
        
        print(f"Benchmark completed in {execution_time:.2f} seconds")
        print(f"Performance: {steps_per_second:.2f} steps/second")
        
        # Create benchmark result
        benchmark_result = {
            'steps': args.steps,
            'execution_time': execution_time,
            'steps_per_second': steps_per_second,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        # Save benchmark result
        benchmark_file = f"benchmark_{int(time.time())}.json"
        
        with open(benchmark_file, 'w') as f:
            json.dump(benchmark_result, f, indent=2)
        
        print(f"Benchmark results saved to {benchmark_file}")
    
    def _handleExperimentCommand(self, args):
        """Handle experiment command."""
        if args.config:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(args.config)
        else:
            config = pynlm.createDefaultConfig()
        
        print(f"Running experiment with {args.replications} replications")
        print(f"Configuration: {args.config if args.config else 'default'}")
        
        # Run replications
        experiment_results = []
        
        for rep in range(args.replications):
            print(f"Replication {rep + 1}/{args.replications}")
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(1000):  # Fixed number of steps per replication
                brain.step(step)
            
            # Record results
            result = {
                'replication': rep + 1,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'learning_detected': brain.getAverageFiringRate() > 0.1
            }
            
            experiment_results.append(result)
        
        # Calculate statistics
        firing_rates = [r['final_firing_rate'] for r in experiment_results]
        spike_counts = [r['total_spikes'] for r in experiment_results]
        learning_detected = sum(1 for r in experiment_results if r['learning_detected'])
        
        stats = {
            'total_replications': args.replications,
            'mean_firing_rate': sum(firing_rates) / len(firing_rates),
            'std_firing_rate': self._calculateStd(firing_rates),
            'mean_spike_count': sum(spike_counts) / len(spike_counts),
            'learning_success_rate': learning_detected / args.replications,
            'replication_results': experiment_results
        }
        
        # Save results if requested
        if args.output:
            results_file = f"{args.output}/experiment_{int(time.time())}.json"
            
            with open(results_file, 'w') as f:
                json.dump(stats, f, indent=2)
            
            print(f"Experiment results saved to {results_file}")
        
        # Print summary
        print("\nExperiment Summary:")
        print(f"  Total replications: {args.replications}")
        print(f"  Mean firing rate: {stats['mean_firing_rate']:.4f} ± {stats['std_firing_rate']:.4f}")
        print(f"  Mean spike count: {stats['mean_spike_count']:.0f}")
        print(f"  Learning success rate: {stats['learning_success_rate']:.2%}")
if __name__ == "__main__":
    import sys
    cli = NLMCommandLineInterface()
    cli.run()
```

## Installation and Setup

### Prerequisites

```bash
# Install system dependencies
apt-get update
apt-get install -y build-essential cmake git python3-pip

# Install Python dependencies
pip install numpy matplotlib pandas psutil scikit-build-core pybind11 pytest

# Clone NLM repository
# git clone https://github.com/nlm-project/nlm.git
# cd nlm

# Build NLM
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install -e .

# Install command line tools
pip install nlm-cli
```

### Virtual Environment Setup

```bash
# Create virtual environment
python3 -m venv nlm_env

# Activate virtual environment
source nlm_env/bin/activate

# Install dependencies
pip install -r requirements.txt

# Build and install NLM
pip install -e .

# Install development tools
pip install pytest black flake8 mypy sphinx
```

### Docker Setup

```dockerfile
# Dockerfile
FROM python:3.11-slim

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy requirements and install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy NLM source
COPY . .

# Build NLM
RUN mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Install Python bindings
RUN pip install --no-cache-dir -e .

# Copy command line tools
COPY src/ /app/src/

# Set environment variables
ENV PYTHONPATH=/app/src:$PYTHONPATH
ENV NLM_HOME=/app

# Create volumes
VOLUME ["/app/checkpoints", "/app/experiments"]

# Set entrypoint
ENTRYPOINT ["nlm"]
```

### Development Setup

```bash
# Install development dependencies
pip install -e .[dev]

# Install pre-commit hooks
pip install pre-commit
pre-commit install

# Create development environment
python3 -m venv .dev_env
source .dev_env/bin/activate
pip install -e .[dev]

# Setup git hooks
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# Pre-commit hook for NLM development

# Run black formatting check
black --check src/

# Run flake8 linting
flake8 src/

# Run mypy type checking
mypy src/

# Run tests
pytest tests/
EOF

chmod +x .git/hooks/pre-commit
```

## Advanced Usage Examples

### Complex Brain Configuration

```python
import pynlm

# Create a multi-region brain with specialized functions
config = pynlm.createDefaultConfig()

# Sensory region
config.set("brain.region_count", 5)
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Configure different regions for different functions
brain.addRegion("sensory_cortex")
brain.addRegion("motor_cortex")
brain.addRegion("working_memory")
brain.addRegion("episodic_memory")
brain.addRegion("reward_system")

# Enable specific plasticity for different regions
# (Implementation would depend on specific brain APIs)
```

### Advanced Learning Scenarios

```python
import pynlm
import time

class AdvancedLearningScenario:
    """Advanced learning scenario with multiple learning mechanisms."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("neuromodulation.dopamine.enable", True)
        self.config.set("neuromodulation.curiosity.enable", True)
        self.config.set("neuromodulation.novelty.enable", True)
        
        self.brain = None
        self.simulation_time = 0.0
    
    def setupEnvironment(self):
        """Setup environment for advanced learning."""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable advanced learning features
        self.brain.enablePlasticity(True)
        self.brain.enableNeuromodulation(True)
        self.brain.enableCuriosity(True)
        self.brain.enableDevelopment(True)
    
    def runAdaptiveLearningPhase(self, duration):
        """Run adaptive learning phase with curiosity-driven exploration."""
        start_time = time.time()
        
        while time.time() - start_time < duration:
            # Determine if curiosity-driven exploration is needed
            curiosity_level = self.brain.getCuriosityLevel()
            novelty_level = self.brain.getNoveltyLevel()
            
            if curiosity_level > 0.7:
                # Curiosity-driven exploration
                self._executeExplorationStep()
            else:
                # Regular learning
                self._executeRegularLearningStep()
            
            # Record metrics
            self._recordMetrics()
            
            # Check for learning plateaus
            if self._isLearningPlateau():
                # Trigger restructuring
                self._triggerBrainRestructuring()
    
    def _executeExplorationStep(self):
        """Execute curiosity-driven exploration step."""
        # Inject novel sensory inputs
        for i in range(10):
            self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
        
        self.brain.step(int(self.simulation_time))
        self.simulation_time += 0.001
    
    def _executeRegularLearningStep(self):
        """Execute regular learning step."""
        self.brain.step(int(self.simulation_time))
        self.simulation_time += 0.001
    
    def _triggerBrainRestructuring(self):
        """Trigger brain restructuring to overcome plateaus."""
        # Enable structural plasticity for reorganization
        self.brain.enableStructuralPlasticity(True)
        
        # Apply developmental signals
        self.brain.develop()
        
        # Reset some connections to allow new patterns
        # (Implementation would depend on specific APIs)
    
    def _isLearningPlateau(self):
        """Check if learning has plateaued."""
        # Simple plateau detection based on firing rate stability
        recent_firing = self._getRecentFiringRates(100)
        
        if len(recent_firing) < 10:
            return False
        
        # Calculate coefficient of variation of firing rates
        mean_firing = sum(recent_firing) / len(recent_firing)
        if mean_firing == 0:
            return False
        
        variance = sum((x - mean_firing) ** 2 for x in recent_firing) / len(recent_firing)
        std_firing = variance ** 0.5
        cv = std_firing / mean_firing
        
        return cv < 0.05  # Low variation indicates plateau
    
    def _getRecentFiringRates(self, count):
        """Get recent firing rates."""
        # Implementation would depend on available APIs
        return []
    
    def _recordMetrics(self):
        """Record learning metrics."""
        # Record various metrics for analysis
        pass

# Run advanced learning scenario
if __name__ == "__main__":
    scenario = AdvancedLearningScenario()
    scenario.setupEnvironment()
    scenario.runAdaptiveLearningPhase(300)  # Run for 5 minutes
```

### Performance Monitoring and Optimization

```python
import pynlm
import psutil
import time
import threading

class PerformanceMonitor:
    """Monitor and optimize NLM performance."""
    
    def __init__(self, brain):
        self.brain = brain
        self.monitor_thread = None
        self.stop_monitor = threading.Event()
        self.performance_data = {
            'memory_usage': [],
            'cpu_usage': [],
            'firing_rates': [],
            'spike_counts': [],
            'timestamps': []
        }
        self.process = psutil.Process()
    
    def startMonitoring(self, interval=1.0):
        """Start performance monitoring."""
        self.stop_monitor.clear()
        self.monitor_thread = threading.Thread(target=self._monitorLoop, args=(interval,))
        self.monitor_thread.start()
    
    def stopMonitoring(self):
        """Stop performance monitoring."""
        self.stop_monitor.set()
        if self.monitor_thread:
            self.monitor_thread.join()
    
    def _monitorLoop(self, interval):
        """Monitoring loop."""
        while not self.stop_monitor.is_set():
            self._recordPerformanceData()
            time.sleep(interval)
    
    def _recordPerformanceData(self):
        """Record performance data."""
        timestamp = time.time()
        
        self.performance_data['timestamps'].append(timestamp)
        self.performance_data['memory_usage'].append(
            self.process.memory_info().rss / (1024 * 1024)  # MB
        )
        self.performance_data['cpu_usage'].append(self.process.cpu_percent())
        self.performance_data['firing_rates'].append(self.brain.getAverageFiringRate())
        self.performance_data['spike_counts'].append(self.brain.getTotalSpikeCount())
    
    def getPerformanceStats(self):
        """Get performance statistics."""
        if not self.performance_data['timestamps']:
            return {}
        
        stats = {
            'memory': {
                'mean': sum(self.performance_data['memory_usage']) / len(self.performance_data['memory_usage']),
                'max': max(self.performance_data['memory_usage']),
                'min': min(self.performance_data['memory_usage'])
            },
            'cpu': {
                'mean': sum(self.performance_data['cpu_usage']) / len(self.performance_data['cpu_usage']),
                'max': max(self.performance_data['cpu_usage'])
            },
            'brain': {
                'mean_firing_rate': sum(self.performance_data['firing_rates']) / len(self.performance_data['firing_rates']),
                'mean_spike_count': sum(self.performance_data['spike_counts']) / len(self.performance_data['spike_counts'])
            },
            'duration': self.performance_data['timestamps'][-1] - self.performance_data['timestamps'][0]
        }
        
        return stats
    
    def analyzePerformance(self):
        """Analyze performance for optimization recommendations."""
        stats = self.getPerformanceStats()
        
        recommendations = []
        
        # Memory analysis
        if stats['memory']['max'] > 1000:  # If memory usage > 1GB
            recommendations.append("Consider reducing memory usage with compression")
        
        # CPU analysis
        if stats['cpu']['mean'] > 80:  # If CPU usage > 80%
            recommendations.append("Consider parallel processing or optimization")
        
        # Brain activity analysis
        if stats['brain']['mean_firing_rate'] < 0.1:
            recommendations.append("Brain may be underutilized - consider more input")
        
        if stats['brain']['mean_spike_count'] < 100:
            recommendations.append("Spike production may be low - check neuron parameters")
        
        return recommendations
```

## Troubleshooting

### Common Issues and Solutions

#### Import Errors

**Issue:** `ImportError: No module named 'pynlm'`

**Solution:**
```bash
# Install NLM Python bindings
pip install -e .

# Or install from source
pip install .
```

#### Compilation Errors

**Issue:** CMake configuration fails

**Solution:**
```bash
# Ensure you have the required dependencies
# For Ubuntu/Debian:
apt-get install -y build-essential cmake python3-dev

# For macOS:
brew install cmake python3

# Rebuild
rm -rf build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Runtime Errors

**Issue:** Segmentation fault during simulation

**Solution:**
```python
# Check configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start with small brain
brain = pynlm.createBrain(config)
brain.initialize()

# Ensure initialize() is called before step()
brain.step(0)  # This should work now

# Check for configuration errors
try:
    brain.initialize()
except Exception as e:
    print(f"Error during initialization: {e}")
    print("Check configuration parameters")
```

#### Performance Issues

**Issue:** Simulation too slow

**Solution:**
```python
# Optimize configuration for performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller brain
config.set("simulation.timestep", 0.01)  # Larger timestep
brain = pynlm.createBrain(config)
brain.initialize()

# Use checkpointing for long simulations
brain.save("checkpoint.bin")

# Use memory optimization
import pynlm.performance
brain = pynlm.performance.OptimizedBrain(brain)
```

#### Memory Issues

**Issue:** Out of memory

**Solution:**
```python
import pynlm
import gc

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Force garbage collection
gc.collect()

# Use checkpointing
brain.save("checkpoint.bin")

# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / (1024 * 1024):.2f} MB")
```

### Getting Help

#### Documentation

- **Main Documentation:** `README.md`
- **Easy Usage Guide:** `easy_usage.md`
- **Advanced Usage Guide:** `HOW_TO_USE.md`
- **Architecture Documentation:** `docs/ARCHITECTURE.md`
- **Scientific Background:** `docs/SCIENCE.md`
- **Experiments Documentation:** `docs/EXPERIMENTS.md`
- **Phase 6 Audit:** `docs/PHASE6_FINAL_AUDIT.md`

#### Online Resources

- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Documentation:** https://nlm.readthedocs.io
- **Discussions:** https://github.com/nlm-project/nlm/discussions
- **Issues:** https://github.com/nlm-project/nlm/issues

#### Support

For technical support:
1. Check the documentation
2. Search existing issues
3. Create a new issue with detailed information
4. Include:
   - Your environment (OS, Python version, NLM version)
   - Configuration files
   - Error messages
   - Code snippets
   - Expected vs. actual behavior

#### Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Follow the code style
4. Add tests for new functionality
5. Update documentation
6. Submit a pull request

### License

MIT License. See LICENSE file for details.

### Acknowledgements

- Thanks to all contributors for making NLM better
- Special thanks to the research community for feedback and suggestions
- Built with ❤️ for computational neuroscience