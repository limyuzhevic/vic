# Advanced Python API Examples for NLM

This document provides examples of the advanced Python API features for NLM (Neural Learning Machine), designed for researchers and advanced users.

## Overview

The enhanced Python API provides comprehensive access to NLM's advanced features, including:

- Advanced configuration management
- System-level control (memory, neuromodulation, plasticity, development)
- Performance monitoring and analysis
- Research data export
- Parallel simulation capabilities
- Advanced agent configuration

## Advanced Configuration

### Creating Advanced Configurations

```python
import pynlm

# Create advanced configuration from dictionary
config_dict = {
    "neuron_count": 2000,
    "region_count": 4,
    "plasticity.stdp.learning_rate": 0.02,
    "plasticity.hebbian.threshold": 0.7,
    "neuromodulation.curiosity_sensitivity": 0.8,
    "neuromodulation.novelty_sensitivity": 0.6,
    "memory.working_memory.capacity": 200,
    "memory.episodic_memory.max_episodes": 5000,
    "development.critical_period_age": 600.0,
    "development.maturation_age": 1800.0
}

config = pynlm.createAdvancedConfig(config_dict)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuring Brain Systems

```python
import pynlm

# Create brain with advanced system configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainWithSystem(
    config,
    enable_memory=True,
    enable_neuromodulation=True,
    enable_plasticity=True,
    enable_development=True
)

# Later, reconfigure systems
pynlm.configure_brain_systems(
    brain,
    enable_memory=True,
    enable_neuromodulation=True,
    enable_plasticity=False,  # Disable plasticity for stability
    enable_development=True
)
```

## Advanced World Configuration

### Creating Custom Environments

```python
import pynlm

# Create world with custom environment
world_dict = {
    "width": 30.0,
    "height": 30.0,
    "vision_width": 20,
    "vision_height": 20,
    "max_energy": 200.0,
    "energy_decay_rate": 0.005,
    "resource_energy_gain": 50.0
}

world = pynlm.createAdvancedWorld(world_dict)
```

### Environment Variants

```python
import pynlm

# Desert environment (harsh, sparse resources)
desert_config = {
    "width": 50.0,
    "height": 50.0,
    "vision_width": 16,
    "vision_height": 16,
    "max_energy": 150.0,
    "energy_decay_rate": 0.02,
    "resource_energy_gain": 25.0,
    "hazard_damage": 20.0
}

desert_world = pynlm.createAdvancedWorld(desert_config)

# Forest environment (abundant resources, complex terrain)
forest_config = {
    "width": 40.0,
    "height": 40.0,
    "vision_width": 16,
    "vision_height": 16,
    "max_energy": 300.0,
    "energy_decay_rate": 0.01,
    "resource_energy_gain": 40.0,
    "hazard_density": 0.3
}

forest_world = pynlm.createAdvancedWorld(forest_config)
```

## Advanced Agent Configuration

### Custom Agent Setup

```python
import pynlm

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("region_count", 3)
config.set("plasticity.stdp.learning_rate", 0.015)

brain = pynlm.createBrain(config)
brain.initialize()

# Configure agent with custom settings
agent_config = {
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": True
}

agent = pynlm.createAgentWithConfiguration(brain, agent_config)
```

### Advanced Agent Configuration

```python
import pynlm

# Create agent with advanced configuration
agent_config = {
    "enable_reward_modulation": True,
    "enable_structural_plasticity": False,  # For stable learning
    "enable_development": True,
    "enable_curiosity": True,
    "development_speed": "fast",  # Can be "fast", "medium", "slow"
    "plasticity_weight": 1.5,  # Multiplier for plasticity effects
    "novelty_threshold": 0.3,  # Threshold for novelty detection
    "curiosity_decay": 0.95   # How quickly curiosity decays
}

agent = pynlm.createAgentWithConfiguration(brain, agent_config)

# Configure agent with world
world = pynlm.createAdvancedWorld({"width": 20.0, "height": 20.0})
agent.initialize(world)
```

## Advanced Simulation

### Running Advanced Simulations

```python
import pynlm
import time

# Setup advanced simulation
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

agent_config = {
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": True
}

agent = pynlm.createAgentWithConfiguration(brain, agent_config)

world = pynlm.createAdvancedWorld({
    "width": 25.0,
    "height": 25.0,
    "vision_width": 16,
    "vision_height": 16,
    "max_energy": 100.0
})

agent.initialize(world)

# Run advanced simulation with monitoring
print("Starting advanced simulation...")
num_steps = 2000
timestep = 0.05
enable_checkpointing = True

start_time = time.time()
result = pynlm.run_advanced_simulation(
    brain, agent, world, num_steps, timestep, enable_checkpointing
)

end_time = time.time()
steps_completed, duration_ms = result

print(f"Simulation completed in {duration_ms/1000:.2f} seconds")
print(f"Steps per second: {steps_completed/(duration_ms/1000):.0f}")
print(f"Average timestep duration: {timestep*1000:.1f} ms")
```

### Simulation Monitoring

```python
import pynlm
import matplotlib.pyplot as plt

# Run simulation and collect performance data
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentWithConfiguration(brain, {
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": True
})

world = pynlm.createAdvancedWorld({"width": 30.0, "height": 30.0})
agent.initialize(world)

# Run simulation with performance monitoring
num_steps = 1000
performance_data = pynlm.analyze_simulation_performance(
    brain, agent, world, num_steps
)

# Display performance metrics
print("Simulation Performance Metrics:")
for metric, value in performance_data.items():
    print(f"  {metric}: {value:.4f}")

# Visualize key metrics
metrics_to_plot = ['firing_rate', 'curiosity_level', 'novelty_level', 'dopamine_level']
fig, axes = plt.subplots(2, 2, figsize=(12, 10))
axes = axes.flatten()

for i, metric in enumerate(metrics_to_plot):
    if metric in performance_data:
        axes[i].bar([metric], [performance_data[metric]], color='skyblue')
        axes[i].set_ylabel('Value')
        axes[i].set_title(f'{metric.replace("_", " ").title()}')
        axes[i].set_ylim(0, max(1.0, performance_data[metric] * 1.2))

plt.tight_layout()
plt.show()
```

## Research Data Export

### Exporting Simulation Data

```python
import pynlm

# Setup simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentWithConfiguration(brain, {
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": True
})

world = pynlm.createAdvancedWorld({"width": 20.0, "height": 20.0})
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if not world.getSensoryPercept().getInternal().empty() else 0.0
    agent.applyRewardModulation(reward, 0.0f)

# Export research data
pynlm.export_research_data(brain, agent, world, "simulation_results.json", "json")

print("Research data exported to simulation_results.json")

# Export as CSV for spreadsheet analysis
pynlm.export_research_data(brain, agent, world, "simulation_results.csv", "csv")

print("Research data exported to simulation_results.csv")
```

## Parallel Processing

### Parallel Simulation

```python
import pynlm
import threading
import queue

class ParallelSimulator:
    def __init__(self, brain, agent, world, num_threads=4):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.num_threads = num_threads
        self.task_queue = queue.Queue()
        self.result_queue = queue.Queue()
        self.stop_signal = threading.Event()
    
    def simulate_step_parallel(self, step):
        """Distribute simulation steps across multiple threads"""
        for i in range(self.num_threads):
            self.task_queue.put((step + i, step * 0.001))
        
        # Start worker threads
        threads = []
        for _ in range(self.num_threads):
            t = threading.Thread(target=self._worker)
            t.start()
            threads.append(t)
        
        # Wait for completion
        for t in threads:
            t.join()
    
    def _worker(self):
        """Worker thread for simulation"""
        while not self.stop_signal.is_set():
            try:
                step, time = self.task_queue.get(timeout=0.1)
                self.brain.step(step, time)
                self.task_queue.task_done()
            except queue.Empty:
                break
    
    def cleanup(self):
        self.stop_signal.set()

# Use parallel simulator
parallel_sim = ParallelSimulator(brain, agent, world, num_threads=4)
print("Running parallel simulation...")
parallel_sim.simulate_step_parallel(1000)
parallel_sim.cleanup()
print("Parallel simulation completed")
```

## Experiment Parameter Optimization

### Automated Parameter Search

```python
import pynlm
import random

class ParameterOptimizer:
    def __init__(self, experiment_system):
        self.experiment_system = experiment_system
        self.parameter_space = self._define_parameter_space()
        self.historical_results = []
    
    def _define_parameter_space(self):
        """Define parameter space for optimization"""
        return {
            'brain_parameters': {
                'neuron_count': {'min': 500, 'max': 5000, 'type': 'int'},
                'plasticity_rate': {'min': 0.001, 'max': 0.1, 'type': 'float'},
                'region_count': {'min': 1, 'max': 10, 'type': 'int'}
            },
            'environment_parameters': {
                'world_width': {'min': 10.0, 'max': 100.0, 'type': 'float'},
                'world_height': {'min': 10.0, 'max': 100.0, 'type': 'float'},
                'vision_range': {'min': 5.0, 'max': 50.0, 'type': 'float'}
            }
        }
    
    def optimize_parameters(self, experiment_specification, objectives):
        """Optimize experiment parameters for specified objectives"""
        best_parameters = None
        best_score = -float('inf')
        
        # Random search (placeholder for more sophisticated optimization)
        for iteration in range(100):
            parameters = self._generate_random_parameters()
            
            # Run experiment with current parameters
            experiment = self._create_experiment(experiment_specification, parameters)
            results = self._run_experiment(experiment)
            
            # Calculate score based on objectives
            score = self._calculate_objective_score(results, objectives)
            
            if score > best_score:
                best_score = score
                best_parameters = parameters
            
            self.historical_results.append({
                'parameters': parameters,
                'results': results,
                'score': score
            })
        
        return best_parameters, best_score
    
    def _generate_random_parameters(self):
        """Generate random parameters"""
        parameters = {}
        
        for category, param_info in self.parameter_space.items():
            parameters[category] = {}
            
            for param_name, param_spec in param_info.items():
                if param_spec['type'] == 'int':
                    parameters[category][param_name] = random.randint(
                        param_spec['min'], param_spec['max']
                    )
                elif param_spec['type'] == 'float':
                    parameters[category][param_name] = random.uniform(
                        param_spec['min'], param_spec['max']
                    )
        
        return parameters
    
    def _create_experiment(self, specification, parameters):
        """Create experiment with parameters"""
        # Create experiment based on specification and parameters
        # ... implementation would depend on specific experiment type
        
        return None  # Placeholder
    
    def _run_experiment(self, experiment):
        """Run experiment"""
        # Run experiment
        # ... implementation would depend on specific experiment type
        
        return None  # Placeholder
    
    def _calculate_objective_score(self, results, objectives):
        """Calculate objective score for results"""
        # Calculate score based on objectives
        # ... would include:
        # - Performance metrics
        # - Learning efficiency
        # - Stability measures
        # - Resource utilization
        
        return random.uniform(0.5, 1.0)  # Example score

# Use parameter optimizer
optimizer = ParameterOptimizer(None)
print("Running parameter optimization...")
# best_params, best_score = optimizer.optimize_parameters(specification, objectives)
print(f"Optimization completed. Best score: {best_score:.4f}")
```

## Research Workflow Integration

### Complete Research Workflow

```python
import pynlm

class ResearchWorkflow:
    def __init__(self):
        self.workflow_manager = pynlm.ResearchWorkflowManager()
    
    def run_complete_research_workflow(self, workflow_name, parameters):
        """Run complete research workflow"""
        # Execute research workflow
        results = self.workflow_manager.execute_research_workflow(
            workflow_name, parameters
        )
        
        # Analyze results
        analysis = self._analyze_workflow_results(results)
        
        # Generate research insights
        insights = self._generate_research_insights(results, analysis)
        
        return {
            'results': results,
            'analysis': analysis,
            'insights': insights
        }
    
    def _analyze_workflow_results(self, results):
        """Analyze workflow results"""
        # Analyze results
        # ... would include:
        # - Statistical analysis
        # - Performance evaluation
        # - Comparison with baselines
        # - Anomaly detection
        
        return {}  # Example analysis
    
    def _generate_research_insights(self, results, analysis):
        """Generate research insights"""
        # Generate insights
        # ... would include:
        # - Key findings
        # - Statistical significance
        # - Practical implications
        # - Future research directions
        
        return []  # Example insights

# Run complete research workflow
workflow = ResearchWorkflow()
print("Running complete research workflow...")
# results = workflow.run_complete_research_workflow("neuroscience_research", parameters)
print("Research workflow completed")
```

## Configuration Files

### Advanced Configuration Examples

Below are examples of advanced configuration files for research:

#### neuron_research_config.json

```json
{
    "brain_parameters": {
        "neuron_count": 2000,
        "region_count": 4,
        "connection_probability": 0.15,
        "plasticity_learning_rate": 0.02
    },
    "neuromodulation_parameters": {
        "curiosity_sensitivity": 0.8,
        "novelty_sensitivity": 0.6,
        "dopamine_baseline": 0.1
    },
    "memory_parameters": {
        "working_memory_capacity": 200,
        "episodic_memory_max_episodes": 5000,
        "consolidation_threshold": 0.7
    },
    "development_parameters": {
        "critical_period_age": 300.0,
        "maturation_age": 900.0,
        "adult_plasticity_modifier": 0.2
    }
}
```

#### learning_efficiency_config.json

```json
{
    "experiment_name": "learning_efficiency_study",
    "brain_config": {
        "neuron_count": 1000,
        "plasticity_learning_rate": 0.015
    },
    "environment_config": {
        "world_width": 50.0,
        "world_height": 50.0,
        "resource_density": 0.3,
        "hazard_density": 0.2
    },
    "agent_config": {
        "enable_reward_modulation": true,
        "enable_structural_plasticity": true,
        "enable_development": true,
        "enable_curiosity": true
    },
    "experiment_parameters": {
        "num_trials": 50,
        "steps_per_trial": 200,
        "timestep": 0.1,
        "enable_checkpointing": true
    }
}
```

## Performance Monitoring

### Advanced Performance Monitoring

```python
import pynlm
import time
import threading

class AdvancedPerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.monitoring_data = []
        self.monitoring_thread = None
        self.stop_monitoring = False
    
    def start_monitoring(self, interval=1.0):
        """Start performance monitoring"""
        self.stop_monitoring = False
        self.monitoring_thread = threading.Thread(
            target=self._monitoring_loop, args=(interval,)
        )
        self.monitoring_thread.start()
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.stop_monitoring = True
        if self.monitoring_thread:
            self.monitoring_thread.join()
    
    def _monitoring_loop(self, interval):
        """Monitoring loop"""
        while not self.stop_monitoring:
            timestamp = time.time()
            monitoring_data = self._collect_monitoring_data()
            monitoring_data['timestamp'] = timestamp
            self.monitoring_data.append(monitoring_data)
            
            time.sleep(interval)
    
    def _collect_monitoring_data(self):
        """Collect monitoring data"""
        return {
            'brain_neurons': self.brain.getTotalNeuronCount(),
            'brain_synapses': self.brain.getTotalSynapseCount(),
            'brain_firing_rate': self.brain.getAverageFiringRate(),
            'agent_curiosity': self.agent.getCuriosityLevel(),
            'agent_novelty': self.agent.getNoveltyLevel(),
            'agent_dopamine': self.agent.getNeuromodulationLevel(),
            'agent_development_stage': int(self.agent.getDevelopmentalStage()),
            'world_time': self.world.getSimulationTime(),
            'world_energy': self.world.getAgentBody().energy,
            'world_health': self.world.getAgentBody().health
        }
    
    def generate_performance_report(self):
        """Generate performance report"""
        if not self.monitoring_data:
            return {}
        
        # Calculate statistics
        report = {
            'total_monitoring_points': len(self.monitoring_data),
            'time_span_seconds': (
                self.monitoring_data[-1]['timestamp'] - 
                self.monitoring_data[0]['timestamp']
            ),
            'brain_metrics': self._analyze_brain_metrics(),
            'agent_metrics': self._analyze_agent_metrics(),
            'world_metrics': self._analyze_world_metrics(),
            'performance_trends': self._analyze_trends()
        }
        
        return report
    
    def _analyze_brain_metrics(self):
        """Analyze brain metrics"""
        if not self.monitoring_data:
            return {}
        
        firing_rates = [d['brain_firing_rate'] for d in self.monitoring_data]
        neurons = [d['brain_neurons'] for d in self.monitoring_data]
        synapses = [d['brain_synapses'] for d in self.monitoring_data()]
        
        return {
            'average_firing_rate': sum(firing_rates) / len(firing_rates),
            'firing_rate_variance': self._calculate_variance(firing_rates),
            'total_neurons_monitored': neurons,
            'total_synapses_monitored': synapses,
            'neuron_growth_rate': self._calculate_growth_rate(neurons),
            'synapse_growth_rate': self._calculate_growth_rate(synapses)
        }
    
    def _analyze_agent_metrics(self):
        """Analyze agent metrics"""
        if not self.monitoring_data:
            return {}
        
        curiosity_levels = [d['agent_curiosity'] for d in self.monitoring_data]
        novelty_levels = [d['agent_novelty'] for d in self.monitoring_data]
        dopamine_levels = [d['agent_dopamine'] for d in self.monitoring_data]
        
        return {
            'average_curiosity': sum(curiosity_levels) / len(curiosity_levels),
            'average_novelty': sum(novelty_levels) / len(novelty_levels),
            'average_dopamine': sum(dopamine_levels) / len(dopamine_levels),
            'curiosity_variance': self._calculate_variance(curiosity_levels),
            'novelty_variance': self._calculate_variance(novelty_levels),
            'dopamine_variance': self._calculate_variance(dopamine_levels)
        }
    
    def _analyze_world_metrics(self):
        """Analyze world metrics"""
        if not self.monitoring_data:
            return {}
        
        times = [d['world_time'] for d in self.monitoring_data]
        energies = [d['world_energy'] for d in self.monitoring_data]
        healths = [d['world_health'] for d in self.monitoring_data]
        
        return {
            'total_simulation_time': sum(times),
            'average_energy': sum(energies) / len(energies),
            'average_health': sum(healths) / len(healths),
            'energy_variance': self._calculate_variance(energies),
            'health_variance': self._calculate_variance(healths)
        }
    
    def _analyze_trends(self):
        """Analyze trends"""
        if not self.monitoring_data or len(self.monitoring_data) < 2:
            return {}
        
        trends = {}
        
        # Analyze firing rate trend
        firing_rates = [d['brain_firing_rate'] for d in self.monitoring_data]
        trends['firing_rate_trend'] = self._calculate_trend(firing_rates)
        
        # Analyze curiosity trend
        curiosity_levels = [d['agent_curiosity'] for d in self.monitoring_data]
        trends['curiosity_trend'] = self._calculate_trend(curiosity_levels)
        
        return trends
    
    def _calculate_variance(self, values):
        """Calculate variance of values"""
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance
    
    def _calculate_growth_rate(self, values):
        """Calculate growth rate of values"""
        if len(values) < 2:
            return 0.0
        
        first = values[0]
        last = values[-1]
        
        if first == 0:
            return 0.0
        
        return (last - first) / first * 100.0  # Percentage
    
    def _calculate_trend(self, values):
        """Calculate trend of values"""
        if len(values) < 2:
            return 'stable'
        
        # Calculate linear regression slope
        n = len(values)
        sum_x = sum(range(n))
        sum_y = sum(values)
        sum_xx = sum(x * x for x in range(n))
        sum_xy = sum(i * values[i] for i in range(n))
        
        denominator = n * sum_xx - sum_x * sum_x
        
        if denominator == 0:
            return 'stable'
        
        slope = (n * sum_xy - sum_x * sum_y) / denominator
        
        if slope > 0.01:
            return 'increasing'
        elif slope < -0.01:
            return 'decreasing'
        else:
            return 'stable'

# Use advanced performance monitoring
monitor = AdvancedPerformanceMonitor(brain, agent, world)
print("Starting advanced performance monitoring...")
monitor.start_monitoring(interval=0.5)

# Run simulation
for step in range(100):
    world.update(0.1)
    # ... simulation code

# Stop monitoring
monitor.stop_monitoring()

# Generate performance report
report = monitor.generate_performance_report()
print("Performance report:", report)
```

## Conclusion

The advanced Python API for NLM provides comprehensive tools for researchers and advanced users to:

1. **Configure complex brain architectures** with custom regions and populations
2. **Control simulation systems** with granular control over memory, neuromodulation, plasticity, and development
3. **Monitor performance** with detailed metrics and real-time monitoring
4. **Export research data** in various formats for analysis and sharing
5. **Optimize experiment parameters** using intelligent algorithms
6. **Integrate with research workflows** for reproducible experiments
7. **Analyze performance trends** with sophisticated statistical analysis

These features enable researchers to conduct cutting-edge neuroscience experiments, analyze complex brain dynamics, and generate reproducible research results with the NLM framework.

For more information on specific features or to report issues, please refer to the NLM documentation or contact the development team.

---

*Last updated: November 14, 2026*
*NLM Version 0.1.0 - Advanced Python API Guide*