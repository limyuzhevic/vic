# Advanced Features for Experienced Users

This section documents advanced features available for experienced NLM users who want to:

- Perform detailed performance analysis
- Conduct research experiments
- Profile and optimize simulations
- Extend the system with custom components
- Analyze and visualize results

---

## Performance Analysis and Profiling

### Performance Monitoring

NLM provides comprehensive performance monitoring capabilities:

#### CPU and Memory Profiling

```bash
# Run with performance monitoring
./nlm --phase 6 --steps 5000 --progress --verbose --multithread --simd

# Detailed profiling with timers
./nlm --phase 5 --lifetime --progress --json perf_results.json
```

#### Performance Metrics

The system tracks and reports:

- **CPU Usage**: Processor utilization during simulation
- **Memory Footprint**: RAM consumption and allocation patterns
- **Simulation Speed**: Steps per second (SPS)
- **Parallel Efficiency**: Speedup with multiple threads
- **SIMD Utilization**: Vectorization effectiveness
- **Cache Performance**: Memory access patterns

### Benchmarking Framework

NLM includes built-in benchmarking capabilities:

#### System Benchmarking

```python
import pynlm
import time

# Benchmark different brain sizes
config = pynlm.createDefaultConfig()

for neuron_count in [1000, 5000, 10000, 50000]:
    config.set("brain.neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start = time.time()
    for step in range(1000):
        brain.step(step)
    end = time.time()
    
    sps = 1000 / (end - start)
    print(f"Neurons: {neuron_count:6d} | SPS: {sps:8.1f} | Memory: {brain.getMemoryUsage():6.1f} MB")
```

#### Comparative Benchmarking

```bash
#!/bin/bash
# Run comprehensive benchmark suite
./benchmark_suite.sh --phases 2 3 4 5 6 --neuron_counts 1000 5000 10000 --steps 1000

# Custom benchmark with analysis
python3 benchmark_analysis.py --input results.json --output analysis.html --metrics throughput,memory,accuracy
```

### Advanced Configuration

#### Custom Configuration Templates

```python
# Advanced configuration template
import pynlm

def create_advanced_config(phase=6, optimization="balanced"):
    config = pynlm.createDefaultConfig()
    
    # Basic settings
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.1)
    config.set("simulation_timestep", 0.001)
    
    # Phase-specific settings
    if phase == 2:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
    elif phase == 3:
        config.set("brain.environment.enable", True)
        config.set("brain.agent.enable", True)
        config.set("environment.width", 50)
        config.set("environment.height", 50)
        
    elif phase == 4:
        config.set("cognition.planner.enable", True)
        config.set("cognition.concept_formation.enable", True)
        config.set("cognition.attention.enable", True)
        
    elif phase == 5:
        config.set("lifetime.learning.enable", True)
        config.set("lifetime.multitask.enable", True)
        config.set("lifetime.continual_learning", True)
        config.set("lifetime.scaling.enable", True)
        
    elif phase == 6:
        config.set("integration.memory.enable", True)
        config.set("integration.neuromodulation.enable", True)
        config.set("integration.checkpointing.enable", True)
        config.set("integration.replay.enable", True)
    
    # Optimization settings
    if optimization == "speed":
        config.set("performance.optimization", "speed")
        config.set("performance.enable_simd", True)
        config.set("performance.enable_multithreading", True)
        config.set("performance.sparse_connectivity", True)
        
    elif optimization == "accuracy":
        config.set("performance.optimization", "accuracy")
        config.set("performance.high_precision_neurons", True)
        config.set("performance.detailed_logging", True)
        
    elif optimization == "balanced":
        config.set("performance.optimization", "balanced")
        config.set("performance.enable_simd", True)
        config.set("performance.enable_multithreading", False)
        config.set("performance.detailed_logging", True)
    
    return config
```

### Experiment Control

#### Custom Experiment Runner

```python
import pynlm
import numpy as np

class CustomExperimentRunner:
    def __init__(self, config):
        self.config = config
        self.results = []
    
    def run_experiments(self, experiment_configs):
        """Run multiple experiments with different configurations"""
        for i, exp_config in enumerate(experiment_configs):
            print(f"Running experiment {i+1}/{len(experiment_configs)}: {exp_config.name}")
            
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            
            # Apply experiment-specific configuration
            exp_config.apply_to_config(self.config)
            
            # Run simulation
            experiment_result = self.run_single_experiment(
                brain, exp_config.steps, exp_config.monitor
            )
            
            self.results.append({
                'config': exp_config,
                'result': experiment_result,
                'timestamp': time.time()
            })
            
            # Save intermediate results
            self.save_results(f"experiment_{i+1}.json")
        
        return self.results
    
    def run_single_experiment(self, brain, steps, monitor=None):
        """Run a single experiment with optional monitoring"""
        experiment_result = {
            'steps_completed': 0,
            'final_metrics': {},
            'time_series': []
        }
        
        for step in range(steps):
            brain.step(step)
            
            if monitor and step % monitor.interval == 0:
                metrics = monitor.capture(brain, step)
                experiment_result['time_series'].append(metrics)
            
            if monitor and step % monitor.save_interval == 0:
                self.save_checkpoint(brain, step)
        
        experiment_result['steps_completed'] = steps
        experiment_result['final_metrics'] = self.collect_final_metrics(brain)
        
        return experiment_result
    
    def collect_final_metrics(self, brain):
        """Collect comprehensive metrics from brain"""
        return {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
            'memory_usage': brain.getMemoryUsage(),
            'processing_time': brain.getProcessingTime(),
            'concepts_formed': brain.getConceptCount(),
            'development_stage': brain.getDevelopmentalStage()
        }
```

#### Experiment Templates

```python
# Template for custom experiments
class ExperimentTemplate:
    def __init__(self, name, description):
        self.name = name
        self.description = description
        self.config_template = None
        self.monitor_template = None
    
    def run_with_template(self, parameters):
        """Run experiment using predefined template"""
        config = self.config_template.copy()
        
        # Apply parameters
        for key, value in parameters.items():
            config.set(key, value)
        
        # Create brain and run experiment
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return self.execute_experiment(brain, parameters)
    
    def execute_experiment(self, brain, parameters):
        """Override in subclass to implement specific experiment logic"""
        raise NotImplementedError("Subclasses must implement execute_experiment")
```

### Visualization and Analysis

#### Result Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

class ResultVisualizer:
    def __init__(self, output_dir="./visualizations"):
        self.output_dir = output_dir
        self.ensure_output_dir()
    
    def ensure_output_dir(self):
        import os
        os.makedirs(self.output_dir, exist_ok=True)
    
    def plot_simulation_results(self, brain_results, title="Simulation Results"):
        """Plot comprehensive simulation results"""
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        fig.suptitle(title, fontsize=16)
        
        # Plot 1: Spike count over time
        ax = axes[0, 0]
        for i, result in enumerate(brain_results):
            ax.plot(result['time_series']['step'], 
                   result['time_series']['spike_count'],
                   label=f'Brain {i+1}')
        ax.set_xlabel('Step')
        ax.set_ylabel('Spike Count')
        ax.legend()
        ax.grid(True)
        
        # Plot 2: Firing rate over time
        ax = axes[0, 1]
        for i, result in enumerate(brain_results):
            ax.plot(result['time_series']['step'],
                   result['time_series']['firing_rate'],
                   label=f'Brain {i+1}')
        ax.set_xlabel('Step')
        ax.set_ylabel('Firing Rate')
        ax.legend()
        ax.grid(True)
        
        # Plot 3: Memory usage over time
        ax = axes[0, 2]
        for i, result in enumerate(brain_results):
            if 'memory_usage' in result['time_series']:
                ax.plot(result['time_series']['step'],
                       result['time_series']['memory_usage'],
                       label=f'Brain {i+1}')
        ax.set_xlabel('Step')
        ax.set_ylabel('Memory Usage (MB)')
        ax.legend()
        ax.grid(True)
        
        # Plot 4: Developmental stage
        ax = axes[1, 0]
        for i, result in enumerate(brain_results):
            stages = [stage.value for stage in result['final_metrics']['development_stage']]
            ax.plot(stages, label=f'Brain {i+1}')
        ax.set_xlabel('Time')
        ax.set_ylabel('Development Stage')
        ax.legend()
        ax.grid(True)
        
        # Plot 5: Concept formation
        ax = axes[1, 1]
        for i, result in enumerate(brain_results):
            ax.bar(i, result['final_metrics']['concepts_formed'],
                   label=f'Brain {i+1}')
        ax.set_xlabel('Brain Index')
        ax.set_ylabel('Concepts Formed')
        ax.legend()
        ax.grid(True)
        
        # Plot 6: Network connectivity
        ax = axes[1, 2]
        for i, result in enumerate(brain_results):
            if 'connectivity_density' in result['final_metrics']:
                ax.bar(i, result['final_metrics']['connectivity_density'],
                       label=f'Brain {i+1}')
        ax.set_xlabel('Brain Index')
        ax.set_ylabel('Connectivity Density')
        ax.legend()
        ax.grid(True)
        
        plt.tight_layout()
        plt.savefig(f"{self.output_dir}/simulation_results.png")
        plt.close()
    
    def plot_phase_comparison(self, phase_results, metrics=['accuracy', 'speed', 'memory']):
        """Compare results across different phases"""
        fig, axes = plt.subplots(len(metrics), 1, figsize=(10, 5 * len(metrics)))
        
        if len(metrics) == 1:
            axes = [axes]
        
        for idx, metric in enumerate(metrics):
            ax = axes[idx]
            
            for phase, results in phase_results.items():
                ax.boxplot(results[metric], 
                          positions=[idx * len(phase_results) + list(phase_results.keys()).index(phase)],
                          widths=0.6,
                          patch_artist=True)
                
                # Add mean marker
                mean_val = np.mean(results[metric])
                ax.scatter(idx * len(phase_results) + list(phase_results.keys()).index(phase),
                          mean_val, color='red', s=50, zorder=5)
            
            ax.set_ylabel(metric.capitalize())
            ax.set_title(f'Comparison of {metric.capitalize()} Across Phases')
            ax.grid(True)
        
        # Set x-axis labels
        for idx in range(len(metrics)):
            phase_labels = list(phase_results.keys())
            axis_positions = [idx * len(phase_results) + i for i in range(len(phase_labels))]
            axes[idx].set_xticks(axis_positions)
            axes[idx].set_xticklabels(phase_labels)
        
        plt.tight_layout()
        plt.savefig(f"{self.output_dir}/phase_comparison.png")
        plt.close()
```

### Debugging and Development Tools

#### Advanced Debugging

```python
import pynlm

class NLMDebugger:
    def __init__(self):
        self.debug_log = []
        self.error_patterns = []
        self.performance_bottlenecks = []
    
    def set_breakpoint(self, condition):
        """Set a conditional breakpoint for debugging"""
        self.breakpoints.append(condition)
    
    def watch_variable(self, brain, variable_name):
        """Monitor specific variables during simulation"""
        if hasattr(brain, variable_name):
            value = getattr(brain, variable_name)
            self.debug_log.append(f"WATCH {variable_name}: {value}")
            return value
        return None
    
    def trace_execution(self, brain, steps, callback=None):
        """Trace execution with custom callbacks"""
        for step in range(steps):
            # Capture state before step
            state_before = self.capture_state(brain)
            
            # Execute step
            brain.step(step)
            
            # Capture state after step
            state_after = self.capture_state(brain)
            
            # Invoke callback with execution trace
            if callback:
                callback(step, state_before, state_after)
            
            # Check breakpoints
            self.check_breakpoints(state_before, state_after)
    
    def capture_state(self, brain):
        """Capture complete brain state for debugging"""
        return {
            'step': brain.getCurrentStep(),
            'time': brain.getCurrentTime(),
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'memory_usage': brain.getMemoryUsage(),
            'development_stage': brain.getDevelopmentalStage(),
            'region_count': brain.getRegionCount()
        }
    
    def detect_performance_bottlenecks(self, brain, steps):
        """Detect performance bottlenecks in simulation"""
        timings = []
        
        for step in range(steps):
            start_time = time.time()
            brain.step(step)
            end_time = time.time()
            
            timings.append(end_time - start_time)
        
        avg_time = np.mean(timings)
        max_time = np.max(timings)
        min_time = np.min(timings)
        std_time = np.std(timings)
        
        # Detect bottlenecks
        if max_time > avg_time * 3:
            self.performance_bottlenecks.append(
                f"Large timing variance: max={max_time:.4f}, avg={avg_time:.4f}"
            )
        
        if std_time > avg_time * 0.5:
            self.performance_bottlenecks.append(
                f"High timing variance: std={std_time:.4f}, avg={avg_time:.4f}"
            )
        
        return {
            'average_time': avg_time,
            'max_time': max_time,
            'min_time': min_time,
            'std_time': std_time,
            'steps_per_second': 1.0 / avg_time if avg_time > 0 else 0,
            'bottlenecks': self.performance_bottlenecks
        }
```

#### Memory Analysis

```python
import pynlm
import tracemalloc
import gc

class MemoryAnalyzer:
    def __init__(self):
        self.memory_snapshots = []
        self.leak_detected = False
    
    def start_memory_tracing(self):
        """Start memory tracing"""
        tracemalloc.start()
        gc.collect()
    
    def take_memory_snapshot(self, label=""):
        """Take memory snapshot"""
        gc.collect()
        snapshot = tracemalloc.take_snapshot()
        
        # Top memory allocations
        top_stats = snapshot.statistics('lineno')
        
        self.memory_snapshots.append({
            'label': label,
            'timestamp': time.time(),
            'top_allocations': top_stats[:10],
            'total_allocated': sum(stat.size for stat in top_stats)
        })
        
        return self.memory_snapshots[-1]
    
    def analyze_memory_usage(self, brain_factory, experiment_config):
        """Analyze memory usage of experiment"""
        self.start_memory_tracing()
        
        # Initial snapshot
        initial_snapshot = self.take_memory_snapshot("initial")
        
        # Run experiment
        brain = brain_factory(experiment_config)
        brain.initialize()
        
        for step in range(experiment_config.steps):
            brain.step(step)
            
            if step % experiment_config.memory_check_interval == 0:
                snapshot = self.take_memory_snapshot(f"step_{step}")
        
        # Final snapshot
        final_snapshot = self.take_memory_snapshot("final")
        
        # Analyze for memory leaks
        self.detect_memory_leaks()
        
        return {
            'initial': initial_snapshot,
            'final': final_snapshot,
            'snapshots': self.memory_snapshots,
            'leaks_detected': self.leak_detected
        }
    
    def detect_memory_leaks(self):
        """Detect potential memory leaks"""
        if len(self.memory_snapshots) < 2:
            return
        
        initial = self.memory_snapshots[0]
        final = self.memory_snapshots[-1]
        
        # Check if memory usage increased significantly
        memory_growth = final['total_allocated'] - initial['total_allocated']
        if memory_growth > initial['total_allocated'] * 0.1:  # 10% growth
            self.leak_detected = True
            self.leak_info = {
                'growth_mb': memory_growth / (1024 * 1024),
                'initial_mb': initial['total_allocated'] / (1024 * 1024),
                'final_mb': final['total_allocated'] / (1024 * 1024),
                'growth_percentage': (memory_growth / initial['total_allocated']) * 100
            }
```

### Batch Processing and Experiment Management

#### Batch Experiment Runner

```python
import pynlm
import json
import multiprocessing as mp
from pathlib import Path

class BatchExperimentRunner:
    def __init__(self, output_dir="./batch_results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
    
    def run_batch_experiments(self, experiment_matrix):
        """Run batch of experiments defined in matrix"""
        results = []
        
        for i, experiment_config in enumerate(experiment_matrix):
            print(f"Running experiment {i+1}/{len(experiment_matrix)}")
            
            # Create configuration
            config = self.create_config_from_template(experiment_config)
            
            # Run experiment
            experiment_result = self.run_single_experiment(
                config, experiment_config.get('parameters', {})
            )
            
            # Save results
            result_file = self.output_dir / f"experiment_{i+1}.json"
            self.save_experiment_result(experiment_result, result_file)
            
            results.append(experiment_result)
        
        return results
    
    def create_config_from_template(self, template):
        """Create configuration from template"""
        config = pynlm.createDefaultConfig()
        
        # Apply template configuration
        for key, value in template.get('config', {}).items():
            config.set(key, value)
        
        return config
    
    def run_single_experiment(self, config, parameters):
        """Run single experiment with given config and parameters"""
        # Apply parameters
        for key, value in parameters.items():
            config.set(key, value)
        
        # Create brain and run
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        experiment_result = {
            'config': {
                'neuron_count': config.get('neuron_count', 1000),
                'steps': parameters.get('steps', 1000),
                'seed': parameters.get('seed', 42)
            },
            'parameters': parameters,
            'metrics': self.run_experiment(brain, parameters),
            'timestamp': time.time()
        }
        
        return experiment_result
    
    def run_experiment(self, brain, parameters):
        """Override in subclass or use default implementation"""
        steps = parameters.get('steps', 1000)
        
        for step in range(steps):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"  Step {step}/{steps}")
        
        return {
            'final_spike_count': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_memory_usage': brain.getMemoryUsage(),
            'final_concepts': brain.getConceptCount(),
            'final_development_stage': brain.getDevelopmentalStage()
        }
    
    def save_experiment_result(self, result, filepath):
        """Save experiment result to JSON file"""
        with open(filepath, 'w') as f:
            json.dump(result, f, indent=2, default=str)
    
    def aggregate_batch_results(self, result_files):
        """Aggregate results from multiple experiment runs"""
        all_results = []
        
        for filepath in result_files:
            with open(filepath, 'r') as f:
                result = json.load(f)
                all_results.append(result)
        
        # Aggregate metrics
        aggregated = {
            'experiment_count': len(all_results),
            'config_variations': len(set(r['config'] for r in all_results)),
            'metric_summary': {},
            'performance_stats': {}
        }
        
        # Aggregate metrics
        for metric in ['final_spike_count', 'final_firing_rate', 
                      'final_memory_usage', 'final_concepts']:
            values = [r['metrics'][metric] for r in all_results]
            aggregated['metric_summary'][metric] = {
                'mean': np.mean(values),
                'std': np.std(values),
                'min': np.min(values),
                'max': np.max(values),
                'median': np.median(values)
            }
        
        return aggregated
```

### Custom Extension Points

#### Extending NLM with Custom Components

NLM is designed to be extensible through several extension points:

```cpp
// Example: Custom neuron type
class CustomNeuron : public Neuron {
public:
    CustomNeuron() : Neuron() {
        // Custom initialization
        learning_rate = 0.001;
        threshold = -55.0f;
    }
    
    void update(double timestep) override {
        // Custom update logic
        // Integration
        v_ += i_ * (timestep / tau_m_);
        
        // Custom spike detection
        if (v_ >= threshold_ && !refractory_) {
            spike_ = true;
            v_ = v_reset_;
            refractory_ = true;
            refractory_timer_ = refractory_period_;
            
            // Custom post-synaptic effects
            on_spike();
        }
        
        // Custom plasticity
        apply_custom_plasticity(timestep);
        
        // Refractory period management
        if (refractory_) {
            refractory_timer_ -= timestep;
            if (refractory_timer_ <= 0) {
                refractory_ = false;
            }
        }
    }
    
private:
    void on_spike() {
        // Custom spike handling
        // e.g., increased synaptic efficacy
    }
    
    void apply_custom_plasticity(double timestep) {
        // Custom plasticity rules
        // e.g., activity-dependent changes
    }
};
```

```cpp
// Example: Custom plasticity rule
class CustomPlasticityRule : public PlasticityRule {
public:
    CustomPlasticityRule() {
        learning_rate = 0.01;
        time_constant = 20.0;
    }
    
    void applyPlasticity(Synapse& synapse, 
                        const NeuralContext& context) override {
        // Custom STDP implementation
        if (context.pre_spike_time && context.post_spike_time) {
            double dt = context.post_spike_time - context.pre_spike_time;
            
            if (dt > 0) {
                // LTP (Long-Term Potentiation)
                double weight_change = learning_rate * 
                                     exp(-dt / time_constant);
                synapse.setWeight(synapse.getWeight() + weight_change);
            } else {
                // LTD (Long-Term Depression)
                double weight_change = learning_rate * 
                                     exp(dt / time_constant);
                synapse.setWeight(synapse.getWeight() + weight_change);
            }
        }
        
        // Additional custom plasticity mechanisms
        apply_additional_mechanisms(synapse, context);
    }
    
private:
    void apply_additional_mechanisms(Synapse& synapse, 
                                    const NeuralContext& context) {
        // Custom mechanisms like
        // neuromodulation effects
        // homeostatic plasticity
        // structural plasticity
        // etc.
    }
};
```

```python
# Example: Custom Python extension
import pynlm

class CustomBrainExtension:
    def __init__(self, brain):
        self.brain = brain
        self.custom_metrics = {}
        self.custom_state = {}
    
    def add_custom_metric(self, name, calculator):
        """Add custom metric to brain"""
        self.custom_metrics[name] = calculator
    
    def calculate_custom_metrics(self):
        """Calculate all custom metrics"""
        metrics = {}
        
        for name, calculator in self.custom_metrics.items():
            try:
                metrics[name] = calculator(self.brain)
            except Exception as e:
                print(f"Error calculating {name}: {e}")
                metrics[name] = None
        
        return metrics
    
    def custom_analysis(self, data_type="statistical"):
        """Perform custom analysis on brain data"""
        if data_type == "statistical":
            return self.statistical_analysis()
        elif data_type == "topological":
            return self.topological_analysis()
        elif data_type == "dynamical":
            return self.dynamical_analysis()
        else:
            raise ValueError(f"Unknown analysis type: {data_type}")
    
    def statistical_analysis(self):
        """Statistical analysis of brain state"""
        # Calculate various statistics
        return {
            'neuron_count_stats': {
                'mean': self.brain.getTotalNeuronCount(),
                'std': 0,  # Static for this example
                'min': self.brain.getTotalNeuronCount(),
                'max': self.brain.getTotalNeuronCount()
            },
            'spike_stats': {
                'total': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate()
            }
        }
```

### Research and Development Tools

#### Experiment Control Framework

```python
import pynlm

class ExperimentControl:
    """Framework for controlling and documenting experiments"""
    
    def __init__(self, experiment_name, researcher):
        self.experiment_name = experiment_name
        self.researcher = researcher
        self.start_time = time.time()
        self.config_history = []
        self.results_history = []
    
    def run_experiment(self, config, parameters):
        """Run experiment with full documentation"""
        experiment_id = f"{self.experiment_name}_{int(self.start_time)}"
        
        experiment_data = {
            'id': experiment_id,
            'name': self.experiment_name,
            'researcher': self.researcher,
            'timestamp': self.start_time,
            'config': config,
            'parameters': parameters,
            'brain': None,
            'results': None
        }
        
        # Save configuration
        self.save_config(config, experiment_id)
        
        # Run experiment
        brain = pynlm.createBrain(config)
        brain.initialize()
        experiment_data['brain'] = brain
        
        results = self.execute_experiment(brain, parameters)
        experiment_data['results'] = results
        
        # Save results
        self.save_results(experiment_data, experiment_id)
        
        # Update history
        self.results_history.append(experiment_data)
        
        return experiment_data
    
    def execute_experiment(self, brain, parameters):
        """Execute experiment with given brain and parameters"""
        steps = parameters.get('steps', 1000)
        
        experiment_results = {
            'steps_completed': 0,
            'metrics_over_time': [],
            'final_metrics': {},
            'behavioral_data': {}
        }
        
        for step in range(steps):
            # Record pre-step state
            pre_step_state = self.capture_brain_state(brain)
            
            # Execute step
            brain.step(step)
            
            # Record post-step state and metrics
            post_step_state = self.capture_brain_state(brain)
            metrics = self.calculate_step_metrics(pre_step_state, post_step_state)
            
            experiment_results['metrics_over_time'].append({
                'step': step,
                'pre_state': pre_step_state,
                'post_state': post_step_state,
                'metrics': metrics
            })
            
            # Save intermediate results if requested
            if parameters.get('save_intermediate', False) and step % 100 == 0:
                self.save_intermediate_results(experiment_results, step)
        
        experiment_results['steps_completed'] = steps
        experiment_results['final_metrics'] = self.calculate_final_metrics(brain)
        experiment_results['behavioral_data'] = self.extract_behavioral_data(brain)
        
        return experiment_results
    
    def capture_brain_state(self, brain):
        """Capture complete brain state"""
        return {
            'step': brain.getCurrentStep(),
            'time': brain.getCurrentTime(),
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'memory_usage': brain.getMemoryUsage(),
            'development_stage': brain.getDevelopmentalStage(),
            'region_count': brain.getRegionCount(),
            'synaptic_weight_stats': self.get_synaptic_weight_stats(brain)
        }
    
    def calculate_step_metrics(self, pre_state, post_state):
        """Calculate metrics for a single step"""
        return {
            'spike_change': post_state['spike_count'] - pre_state['spike_count'],
            'firing_rate_change': post_state['firing_rate'] - pre_state['firing_rate'],
            'memory_change': post_state['memory_usage'] - pre_state['memory_usage'],
            'network_activity_change': self.calculate_network_activity_change(pre_state, post_state),
            'learning_proxy': self.calculate_learning_proxy(pre_state, post_state)
        }
    
    def save_config(self, config, experiment_id):
        """Save experiment configuration"""
        config_file = self.output_dir / f"config_{experiment_id}.json"
        config.to_file(config_file)
    
    def save_results(self, experiment_data, experiment_id):
        """Save complete experiment results"""
        results_file = self.output_dir / f"results_{experiment_id}.json"
        
        with open(results_file, 'w') as f:
            json.dump(experiment_data, f, indent=2, default=str)
    
    def generate_research_report(self, experiment_data):
        """Generate research report from experiment data"""
        report = {
            'experiment_id': experiment_data['id'],
            'title': self.experiment_name,
            'researcher': self.researcher,
            'execution_date': time.ctime(experiment_data['timestamp']),
            'configuration': experiment_data['config'],
            'parameters': experiment_data['parameters'],
            'key_findings': self.extract_key_findings(experiment_data),
            'statistical_analysis': self.perform_statistical_analysis(experiment_data),
            'reproducibility': self.assess_reproducibility(experiment_data),
            'limitations': self.identify_limitations(experiment_data),
            'future_directions': self.suggest_future_directions(experiment_data)
        }
        
        report_file = self.output_dir / f"report_{experiment_data['id']}.md"
        self.write_markdown_report(report, report_file)
        
        return report
```