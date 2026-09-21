# NLM (Neural Learning Machine) - Advanced Usage Guide

This guide provides comprehensive examples and documentation for the NLM project, demonstrating the improved functionality and advanced features.

## Quick Start with Examples

### Example 1: Basic Brain Simulation

This example shows how to create and simulate a simple neural brain:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Check brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Example 2: Agent in World Simulation

This example demonstrates an agent interacting with a virtual world:

```python
import pynlm

# Setup configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 2)
config.set("random_seed", 42)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    # World updates
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

print("Simulation completed!")
print(f"Final reward: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Example 3: Machine Learning Research

This example demonstrates advanced research features:

```python
import pynlm
import time

# Advanced configuration for research
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 4)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.018)
config.set("synaptogenesis_rate", 0.0002)
config.set("pruning_rate", 0.00002)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure for research
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.setAgentStart(15.0, 15.0)
world.setMaxEnergy(200.0)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run research simulation with monitoring
metrics = {
    'total_reward': 0.0,
    'curiosity_levels': [],
    'firing_rates': [],
    'development_stages': [],
    'memory_episodes': []
}

start_time = time.time()

for step in range(5000):
    # Update world
    world.update(0.05)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Decision making
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.05)
    
    # Collect metrics
    metrics['total_reward'] += result.reward
    metrics['curiosity_levels'].append(agent.getCuriosityLevel())
    metrics['firing_rates'].append(brain.getAverageFiringRate())
    metrics['development_stages'].append(agent.getDevelopmentalStage())
    
    if agent.getDevelopmentalStage() == pynlm.DevelopmentalStage::Adult:
        if brain.getEpisodicMemory():
            metrics['memory_episodes'].append(brain.getEpisodicMemory()->getEpisodeCount())
    
    # Periodic checkpoint
    if step % 1000 == 0 and step > 0:
        brain.save(f"checkpoint_{step}.bin")
        print(f"Checkpoint saved at step {step}")

end_time = time.time()

# Compute statistics
final_metrics = {
    'total_steps': 5000,
    'total_reward': metrics['total_reward'],
    'avg_reward': metrics['total_reward'] / 5000,
    'final_curiosity': metrics['curiosity_levels'][-1],
    'max_curiosity': max(metrics['curiosity_levels']),
    'avg_firing_rate': sum(metrics['firing_rates']) / len(metrics['firing_rates']),
    'final_development_stage': metrics['development_stages'][-1],
    'memory_episodes': metrics['memory_episodes'][-1] if metrics['memory_episodes'] else 0,
    'wall_clock_time': end_time - start_time,
    'neurons_created': brain.getTotalNeuronCount(),
    'synapses_created': brain.getTotalSynapseCount()
}

print("Research simulation completed!")
print(f"Wall clock time: {final_metrics['wall_clock_time']:.2f}s")
print(f"Average reward per step: {final_metrics['avg_reward']:.4f}")
print(f"Final curiosity level: {final_metrics['final_curiosity']:.3f}")
print(f"Average firing rate: {final_metrics['avg_firing_rate']:.2f}")
print(f"Final development stage: {final_metrics['final_development_stage']}")
print(f"Memory episodes stored: {final_metrics['memory_episodes']}")
```

## Phase 6 Integration Examples

### Example 4: Integration Test Framework

This demonstrates how to use the Phase 6 integration test framework:

```python
import pynlm
from pynlm.experiments import Phase6IntegrationResult, Phase6Config, Phase6IntegratedExperiment

# Run integration test with custom configuration
experiment = Phase6IntegratedExperiment()

# Configure for testing
config = Phase6Config()
config.neuronCount = 1000
config.maxSteps = 3000
config.enableCheckpointing = True
config.enableReplay = True
config.enableDevelopment = True
config.checkpointPath = "./test_checkpoint.bin"

print("Running Phase 6 integration test...")
result = experiment.run(config)

print("\n=== Integration Test Results ===")
print(f"Total reward: {result.totalReward}")
print(f"Average firing rate: {result.avgFiringRate}")
print(f"Memory episodes stored: {result.memoryEpisodesStored}")
print(f"Dopamine level: {result.dopamineLevel}")

# Check integration status
systems_integrated = sum([
    result.memoryWorkingMemoryIntegrated,
    result.memoryEpisodicMemoryIntegrated,
    result.neuromodulationIntegrated,
    result.predictionIntegrated,
    result.developmentIntegrated,
    result.checkpointingWorks
])

print(f"\nSystems successfully integrated: {systems_integrated}/6")

if systems_integrated >= 5:
    print("✅ Phase 6 integration successful!")
else:
    print("❌ Integration verification failed - some systems may need attention")

# Save results
import json
with open("integration_results.json", "w") as f:
    json.dump({
        'success': systems_integrated >= 5,
        'metrics': {
            'total_reward': result.totalReward,
            'avg_firing_rate': result.avgFiringRate,
            'memory_episodes': result.memoryEpisodesStored,
            'dopamine_level': result.dopamineLevel,
            'curiosity_level': result.curiosityLevel,
            'novelty_level': result.noveltyLevel
        },
        'integration_status': {
            'working_memory': result.memoryWorkingMemoryIntegrated,
            'episodic_memory': result.memoryEpisodicMemoryIntegrated,
            'neuromodulation': result.neuromodulationIntegrated,
            'prediction': result.predictionIntegrated,
            'development': result.developmentIntegrated,
            'checkpointing': result.checkpointingWorks,
            'replay': result.replayWorks
        }
    }, f, indent=2)
```

### Example 5: Command-Line Interface Example

This shows how to use the enhanced command-line interface:

```bash
# Run with default settings
./nlm_phase6_demo

# Run with custom configuration file
./nlm_phase6_demo --config=my_config.cfg

# Run with command-line overrides
./nlm_phase6_demo --neurons=2000 --steps=5000 --checkpoints=false --replay=false

# Run with multiple options
./nlm_phase6_demo --neurons=1000 --steps=2000 --development=true --curiosity=true
```

Configuration file example (`demo_config.cfg`):
```ini
# NLM Demo Configuration
# Set configuration values here

# Brain configuration
brain.neuron_count = 800
brain.region_count = 2
brain.connection_probability = 0.12

# Simulation settings
max_simulation_steps = 3000
simulation_timestep = 0.001

# Plasticity settings
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
synaptogenesis_rate = 0.00015
pruning_rate = 0.000015

# Development and neuromodulation
enable_development = true
enable_curiosity = true
enable_reward_modulation = true
enable_structural_plasticity = true

# World settings
world.width = 25
world.height = 25
world.vision_width = 10
world.vision_height = 10

# Logging
log_level = INFO
log_to_file = true
log_filename = demo_nlm.log
```

## Advanced Python Features

### Example 6: Custom Neuron Configuration

Create brains with custom neuron types and connectivity patterns:

```python
import pynlm

# Advanced configuration with custom neuron types
config = pynlm.createDefaultConfig()

# Configure specific neuron populations
config.set("brain.neuron_count", 1500)
config.set("brain.region_count", 3)
config.set("brain.synapse_density", 0.2)

# Custom STDP parameters
config.set("stdp_ltp_weight", 0.025)
config.set("stdp_ltd_weight", 0.022)
config.set("stdp_tau", 25.0)

# Development parameters
config.set("synaptogenesis_rate", 0.0003)
config.set("pruning_rate", 0.00003)
config.set("development_stage_duration", 500)

# Neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with custom settings
agent = pynlm.createAgentBrain(brain)

# Custom behavior configuration
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Advanced brain configuration created successfully")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")
```

### Example 7: Real-time Monitoring

Monitor brain activity in real-time:

```python
import pynlm
import time
import threading

class BrainMonitor:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.monitoring = False
        self.metrics = []
        
    def start_monitoring(self, interval=0.1):
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def _monitor_loop(self, interval):
        while self.monitoring:
            metrics = {
                'time': time.time(),
                'step': self.brain.getTotalSpikeCount(),
                'firing_neutrons': self.brain.getFiringNeuronCount(),
                'avg_firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'dopamine': self.agent.getNeuromodulationLevel(),
                'development_stage': self.agent.getDevelopmentalStage(),
                'world_energy': self.world.getAgentBody().energy,
                'world_age': self.world.getAgentBody().age
            }
            self.metrics.append(metrics)
            time.sleep(interval)
            
    def stop_monitoring(self):
        self.monitoring = False
        if hasattr(self, 'monitor_thread'):
            self.monitor_thread.join(timeout=1.0)
            
    def get_metrics(self):
        return self.metrics
        
    def compute_statistics(self):
        if not self.metrics:
            return {}
            
        return {
            'total_monitoring_points': len(self.metrics),
            'time_span': self.metrics[-1]['time'] - self.metrics[0]['time'],
            'avg_firing_rate': sum(m['avg_firing_rate'] for m in self.metrics) / len(self.metrics),
            'max_firing_rate': max(m['avg_firing_rate'] for m in self.metrics),
            'avg_curiosity': sum(m['curiosity'] for m in self.metrics) / len(self.metrics),
            'max_curiosity': max(m['curiosity'] for m in self.metrics),
            'final_energy': self.metrics[-1]['world_energy'],
            'final_stage': self.metrics[-1]['development_stage']
        }

# Usage example
def create_and_monitor():
    # Setup NLM system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Create monitor
    monitor = BrainMonitor(brain, world, agent)
    monitor.start_monitoring(interval=0.1)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
        agent.updateDevelopment(0.1)
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Get statistics
    stats = monitor.compute_statistics()
    print("Monitoring statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value:.4f}")
    
    return monitor.get_metrics()

# Run monitoring
metrics = create_and_monitor()
```

## Research and Development Features

### Example 8: Experimental Framework

Use the experimental framework for systematic research:

```python
import pynlm
from pynlm.experiments import Phase4Experiment, Phase5Experiment, Phase6IntegratedExperiment

class NLMResearchFramework:
    def __init__(self):
        self.results = {}
        
    def run_attention_experiment(self):
        """Run attention mechanism experiments"""
        print("Running Attention Experiment...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("attention.inhibition_strength", 0.5)
        config.set("attention.excitation_strength", 1.5)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test attention mechanisms
        attention_results = {
            'competition_effectiveness': 0.0,
            'selection_bias': 0.0,
            'stability_over_time': 0.0
        }
        
        # Run simulation and collect data
        for step in range(500):
            # Inject competing inputs
            for i in range(10):
                brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 10.0)
            
            brain.step(step)
            
            # Measure attention effects
            # This would involve analyzing neural activity patterns
        
        self.results['attention'] = attention_results
        return attention_results
        
    def run_planning_experiment(self):
        """Run planning and decision-making experiments"""
        print("Running Planning Experiment...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("planning.planning_depth", 5)
        config.set("planning.prediction_horizon", 10)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test planning capabilities
        planning_results = {
            'plan_accuracy': 0.0,
            'planning_time': 0.0,
            'adaptation_rate': 0.0
        }
        
        self.results['planning'] = planning_results
        return planning_results
        
    def run_social_learning_experiment(self):
        """Run social learning experiments"""
        print("Running Social Learning Experiment...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 600)
        config.set("social.imitation_rate", 0.7)
        config.set("social.observation_window", 50)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test social learning mechanisms
        social_results = {
            'imitation_success_rate': 0.0,
            'observation_effectiveness': 0.0,
            'social_coordination': 0.0
        }
        
        self.results['social'] = social_results
        return social_results
        
    def run_development_experiment(self):
        """Run development and maturation experiments"""
        print("Running Development Experiment...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1200)
        config.set("development.initial_plasticity", 1.0)
        config.set("development.adult_plasticity", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Track development over time
        development_stages = []
        plasticity_changes = []
        
        agent = pynlm.createAgentBrain(brain)
        
        for step in range(1000):
            agent.updateDevelopment(0.1)
            development_stages.append(agent.getDevelopmentalStage())
            plasticity_changes.append(agent.plasticityModifier_)
        
        development_results = {
            'stage_transitions': len(set(development_stages)),
            'plasticity_decay_rate': (plasticity_changes[0] - plasticity_changes[-1]) / plasticity_changes[0],
            'stable_adult_phase_duration': len([s for s in development_stages if s == pynlm.DevelopmentalStage::Adult]),
            'critical_period_effectiveness': sum(1 for s in development_stages if s == pynlm.DevelopmentalStage::CriticalPeriod) / len(development_stages)
        }
        
        self.results['development'] = development_results
        return development_results
        
    def run_full_research_suite(self):
        """Run complete research suite"""
        print("Running Full Research Suite...")
        
        experiments = [
            self.run_attention_experiment,
            self.run_planning_experiment,
            self.run_social_learning_experiment,
            self.run_development_experiment
        ]
        
        results = {}
        for experiment in experiments:
            try:
                results[experiment.__name__] = experiment()
            except Exception as e:
                print(f"Error in {experiment.__name__}: {e}")
                results[experiment.__name__] = {'error': str(e)}
        
        self.results = results
        return results
        
    def save_research_data(self, filename="nlm_research_data.json"):
        """Save research results to file"""
        import json
        import datetime
        
        data = {
            'timestamp': datetime.datetime.now().isoformat(),
            'framework_version': "0.1.0",
            'experiments': self.results,
            'summary': self._compute_summary()
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Research data saved to {filename}")
        
    def _compute_summary(self):
        """Compute summary statistics"""
        summary = {}
        for exp_name, results in self.results.items():
            if isinstance(results, dict) and 'error' not in results:
                for key, value in results.items():
                    if isinstance(value, (int, float)):
                        summary[f"{exp_name}_{key}"] = value
        return summary

# Usage
if __name__ == "__main__":
    framework = NLMResearchFramework()
    
    # Run specific experiments
    attention_results = framework.run_attention_experiment()
    planning_results = framework.run_planning_experiment()
    
    # Or run full suite
    # all_results = framework.run_full_research_suite()
    
    # Save results
    framework.save_research_data("my_nlm_research.json")
    
    print("\nResearch framework completed successfully!")
```

## Configuration Management

### Example 9: Advanced Configuration Management

Manage complex experiment configurations:

```python
import pynlm
import json
from pathlib import Path

class NLMConfigManager:
    def __init__(self, base_dir="configs"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
        
    def create_experiment_config(self, experiment_name, config_template):
        """Create a configuration for an experiment"""
        config = pynlm.createDefaultConfig()
        
        # Apply template
        for key, value in config_template.items():
            if isinstance(value, dict):
                # Nested configuration
                for sub_key, sub_value in value.items():
                    config.set(f"{key}.{sub_key}", sub_value)
            else:
                config.set(key, value)
        
        # Save configuration
        config_path = self.base_dir / f"{experiment_name}.cfg"
        config.saveToFile(str(config_path))
        
        return config
        
    def load_experiment_config(self, experiment_name):
        """Load an experiment configuration"""
        config = pynlm.createDefaultConfig()
        
        config_path = self.base_dir / f"{experiment_name}.cfg"
        if config_path.exists():
            config.loadFromFile(str(config_path))
            
        return config
        
    def compare_configurations(self, config1, config2):
        """Compare two configurations and show differences"""
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        
        print("Configuration Comparison:")
        print("=" * 50)
        
        # Keys in config1 but not in config2
        only_in_1 = set(keys1) - set(keys2)
        if only_in_1:
            print(f"Only in config1 ({len(only_in_1)} keys):")
            for key in sorted(only_in_1):
                print(f"  {key}: {config1.getOr(key, 'N/A')}")
        
        # Keys in config2 but not in config1
        only_in_2 = set(keys2) - set(keys1)
        if only_in_2:
            print(f"Only in config2 ({len(only_in_2)} keys):")
            for key in sorted(only_in_2):
                print(f"  {key}: {config2.getOr(key, 'N/A')}")
        
        # Common keys with different values
        common_keys = set(keys1) & set(keys2)
        different_values = []
        
        for key in common_keys:
            val1 = config1.getOr(key, None)
            val2 = config2.getOr(key, None)
            if val1 != val2:
                different_values.append((key, val1, val2))
        
        if different_values:
            print(f"Different values in {len(different_values)} common keys:")
            for key, val1, val2 in different_values[:10]:  # Show first 10
                print(f"  {key}:")
                print(f"    config1: {val1}")
                print(f"    config2: {val2}")
        
        print("=" * 50)
        
    def create_research_suite(self, experiment_name, param_grid):
        """Create a suite of configurations for parameter sweep"""
        configs = []
        
        # Generate all combinations of parameters
        import itertools
        
        param_names = list(param_grid.keys())
        param_values = list(param_grid.values())
        
        for combination in itertools.product(*param_values):
            config = pynlm.createDefaultConfig()
            
            for name, value in zip(param_names, combination):
                # Convert parameter name to config key
                config_key = self._param_to_config_key(name)
                config.set(config_key, value)
            
            config_path = self.base_dir / f"{experiment_name}_param_{len(configs)}.cfg"
            config.saveToFile(str(config_path))
            
            configs.append(config)
        
        print(f"Created {len(configs)} configurations for {experiment_name}")
        return configs
        
    def _param_to_config_key(self, param_name):
        """Convert parameter name to configuration key"""
        # Common parameter mappings
        mappings = {
            'neurons': 'brain.neuron_count',
            'regions': 'brain.region_count',
            'plasticity_rate': 'plasticity.learning_rate',
            'stdp_ltp': 'stdp_ltp_weight',
            'stdp_ltd': 'stdp_ltd_weight',
            'curiosity': 'neuromod.curiosity.enable',
            'development': 'neuromod.development.enable'
        }
        
        return mappings.get(param_name, param_name)

# Example usage
if __name__ == "__main__":
    # Initialize config manager
    config_manager = NLMConfigManager()
    
    # Create a base configuration
    base_config = {
        'brain.neuron_count': 1000,
        'brain.region_count': 2,
        'brain.connection_probability': 0.1,
        'stdp_ltp_weight': 0.01,
        'stdp_ltd_weight': 0.012,
        'enable_development': True,
        'enable_curiosity': True
    }
    
    # Create experiment configuration
    experiment_config = config_manager.create_experiment_config("baseline", base_config)
    
    # Load and compare with another configuration
    another_config = pynlm.createDefaultConfig()
    another_config.set("brain.neuron_count", 2000)
    another_config.set("brain.region_count", 4)
    
    config_manager.compare_configurations(experiment_config, another_config)
    
    # Create parameter sweep configuration
    param_grid = {
        'neurons': [500, 1000, 2000],
        'plasticity_rate': [0.005, 0.01, 0.02],
        'curiosity': [True, False]
    }
    
    param_configs = config_manager.create_research_suite("parameter_sweep", param_grid)
```

## Performance Monitoring and Optimization

### Example 10: Performance Monitoring

Monitor and optimize NLM performance:

```python
import pynlm
import time
import psutil
import threading
from typing import Dict, List

class PerformanceMonitor:
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.monitoring = False
        self.performance_data = []
        self.cpu_samples = []
        self.memory_samples = []
        
    def start_monitoring(self, interval=0.1):
        """Start performance monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def _monitor_loop(self, interval):
        """Main monitoring loop"""
        while self.monitoring:
            start_time = time.time()
            
            # Collect system metrics
            cpu_percent = psutil.cpu_percent()
            memory = psutil.virtual_memory()
            
            self.cpu_samples.append(cpu_percent)
            self.memory_samples.append(memory.used)
            
            # Collect NLM-specific metrics
            nlm_metrics = self._collect_nlm_metrics()
            
            # Combine metrics
            timestamp = time.time()
            data_point = {
                'timestamp': timestamp,
                'cpu_percent': cpu_percent,
                'memory_used': memory.used,
                'memory_percent': memory.percent,
                'nlm': nlm_metrics
            }
            
            self.performance_data.append(data_point)
            
            # Calculate monitoring interval
            elapsed = time.time() - start_time
            if elapsed < interval:
                time.sleep(interval - elapsed)
                
    def _collect_nlm_metrics(self):
        """Collect NLM system metrics"""
        if not self.brain:
            return {}
            
        metrics = {}
        
        # Brain metrics
        metrics['neuron_count'] = self.brain.getTotalNeuronCount()
        metrics['synapse_count'] = self.brain.getTotalSynapseCount()
        metrics['active_neurons'] = self.brain.getActiveNeuronCount()
        metrics['firing_neurons'] = self.brain.getFiringNeuronCount()
        metrics['total_spikes'] = self.brain.getTotalSpikeCount()
        metrics['avg_firing_rate'] = self.brain.getAverageFiringRate()
        metrics['e_i_ratio'] = self.brain.getExcitationInhibitionRatio()
        
        # Memory system metrics
        if self.brain.getWorkingMemory():
            metrics['working_memory_traces'] = self.brain.getWorkingMemory().getActiveTraces()
        if self.brain.getEpisodicMemory():
            metrics['episodic_memory_episodes'] = self.brain.getEpisodicMemory().getEpisodeCount()
        if self.brain.getAssociativeMemory():
            metrics['associative_memory_associations'] = self.brain.getAssociativeMemory().getAssociationCount()
        
        # Neuromodulation metrics
        if self.agent:
            metrics['curiosity_level'] = self.agent.getCuriosityLevel()
            metrics['novelty_level'] = self.agent.getNoveltyLevel()
            metrics['dopamine_level'] = self.agent.getNeuromodulationLevel()
            metrics['prediction_error'] = self.agent.getPredictionError()
            metrics['developmental_stage'] = self.agent.getDevelopmentalStage()
        
        # Environment metrics
        if self.world:
            metrics['world_simulation_time'] = self.world.getSimulationTime()
            metrics['world_agent_x'] = self.world.getAgentBody().x
            metrics['world_agent_y'] = self.world.getAgentBody().y
            metrics['world_agent_energy'] = self.world.getAgentBody().energy
            
        return metrics
        
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring = False
        if hasattr(self, 'monitor_thread'):
            self.monitor_thread.join(timeout=1.0)
            
    def get_performance_stats(self, window=None):
        """Get performance statistics"""
        if not self.performance_data:
            return {}
            
        if window:
            # Filter data within time window
            cutoff = time.time() - window
            data = [d for d in self.performance_data if d['timestamp'] >= cutoff]
        else:
            data = self.performance_data
            
        if not data:
            return {}
            
        # CPU statistics
        cpu_values = [d['cpu_percent'] for d in data]
        
        # Memory statistics
        memory_values = [d['memory_used'] for d in data]
        memory_mb = [m / (1024 * 1024) for m in memory_values]
        
        # NLM-specific statistics
        nlm_data = [d['nlm'] for d in data if d['nlm']]
        
        stats = {
            'monitoring_points': len(data),
            'time_span_seconds': data[-1]['timestamp'] - data[0]['timestamp'],
            'cpu_stats': {
                'avg_percent': sum(cpu_values) / len(cpu_values),
                'max_percent': max(cpu_values),
                'min_percent': min(cpu_values)
            },
            'memory_stats': {
                'avg_mb': sum(memory_mb) / len(memory_mb),
                'max_mb': max(memory_mb),
                'min_mb': min(memory_mb),
                'growth_rate': (memory_mb[-1] - memory_mb[0]) / len(memory_mb) if len(memory_mb) > 1 else 0
            }
        }
        
        # Add NLM metrics if available
        if nlm_data:
            stats['nlm'] = {}
            for key in nlm_data[0].keys():
                values = [d[key] for d in nlm_data if key in d]
                if values:
                    stats['nlm'][key] = {
                        'avg': sum(values) / len(values),
                        'max': max(values),
                        'min': min(values),
                        'current': values[-1]
                    }
        
        return stats
        
    def detect_anomalies(self):
        """Detect performance anomalies"""
        if not self.performance_data:
            return []
            
        anomalies = []
        
        # Detect CPU spikes
        for i, data_point in enumerate(self.performance_data):
            if data_point['cpu_percent'] > 90:  # High CPU usage
                anomalies.append({
                    'timestamp': data_point['timestamp'],
                    'type': 'cpu_spike',
                    'severity': 'high' if data_point['cpu_percent'] > 95 else 'medium',
                    'value': data_point['cpu_percent']
                })
            
            # Detect memory growth
            if i > 10 and 'memory_used' in data_point:
                recent_avg = sum(d['memory_used'] for d in self.performance_data[max(0, i-10):i]) / 10
                current = data_point['memory_used']
                if current > recent_avg * 1.5:  # 50% increase
                    anomalies.append({
                        'timestamp': data_point['timestamp'],
                        'type': 'memory_growth',
                        'severity': 'high' if current > recent_avg * 2 else 'medium',
                        'value': current - recent_avg
                    })
        
        return anomalies
        
    def generate_performance_report(self):
        """Generate a performance report"""
        stats = self.get_performance_stats()
        anomalies = self.detect_anomalies()
        
        report = {
            'summary': {
                'total_monitoring_time': stats.get('time_span_seconds', 0),
                'monitoring_points': stats.get('monitoring_points', 0),
                'average_cpu_usage': stats.get('cpu_stats', {}).get('avg_percent', 0),
                'peak_cpu_usage': stats.get('cpu_stats', {}).get('max_percent', 0),
                'average_memory_usage': stats.get('memory_stats', {}).get('avg_mb', 0)
            },
            'anomalies': anomalies,
            'recommendations': self._generate_recommendations(stats, anomalies)
        }
        
        return report
        
    def _generate_recommendations(self, stats, anomalies):
        """Generate performance recommendations"""
        recommendations = []
        
        # CPU recommendations
        avg_cpu = stats.get('cpu_stats', {}).get('avg_percent', 0)
        if avg_cpu > 80:
            recommendations.append({
                'priority': 'high',
                'category': 'cpu_usage',
                'message': 'High average CPU usage detected (>80%). Consider optimizing neural computations or reducing simulation complexity.',
                'action': 'Optimize plasticity rules or reduce brain complexity'
            })
        
        # Memory recommendations
        mem_growth = stats.get('memory_stats', {}).get('growth_rate', 0)
        if mem_growth > 10:  # 10MB per sample growth rate
            recommendations.append({
                'priority': 'medium',
                'category': 'memory_usage',
                'message': f'Memory usage growing at {mem_growth:.1f} MB per sample. Monitor memory leaks.',
                'action': 'Implement memory pools or check for memory leaks in plasticity systems'
            })
        
        # Anomaly recommendations
        high_anomalies = [a for a in anomalies if a['severity'] == 'high']
        if high_anomalies:
            recommendations.append({
                'priority': 'high',
                'category': 'system_stability',
                'message': f'{len(high_anomalies)} high-severity anomalies detected.',
                'action': 'Investigate system crashes or unstable neural dynamics'
            })
        
        if not recommendations:
            recommendations.append({
                'priority': 'low',
                'category': 'optimization',
                'message': 'Performance within normal ranges. Consider incremental optimizations for research experiments.',
                'action': 'Continue current configuration or try minor optimizations'
            })
        
        return recommendations

# Usage example
if __name__ == "__main__":
    # Create NLM system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create performance monitor
    monitor = PerformanceMonitor(brain, world, agent)
    
    # Start monitoring
    print("Starting performance monitoring...")
    monitor.start_monitoring(interval=0.1)
    
    # Run simulation
    print("Running simulation with performance monitoring...")
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
        agent.updateDevelopment(0.1)
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Generate performance report
    report = monitor.generate_performance_report()
    
    print("\n=== Performance Report ===")
    summary = report['summary']
    print(f"Monitoring time: {summary['total_monitoring_time']:.1f} seconds")
    print(f"Monitoring points: {summary['monitoring_points']}")
    print(f"Average CPU usage: {summary['average_cpu_usage']:.1f}%")
    print(f"Peak CPU usage: {summary['peak_cpu_usage']:.1f}%")
    print(f"Average memory usage: {summary['average_memory_usage']:.1f} MB")
    
    print(f"\nAnomalies detected: {len(report['anomalies'])}")
    for anomaly in report['anomalies']:
        print(f"  - {anomaly['type']}: {anomaly['severity']} severity")
    
    print(f"\nRecommendations ({len(report['recommendations'])}):")
    for rec in report['recommendations']:
        print(f"  [{rec['priority'].upper()}] {rec['category']}: {rec['message']}")
        print(f"     Action: {rec['action']}")
```

## Advanced Topics

### Example 12: Neuromodulation Research

Research-level neuromodulation configuration:

```python
import pynlm
import numpy as np
from typing import Dict, List, Optional

class NeuromodulationResearcher:
    def __init__(self):
        self.research_data = {}
        
    def design_dopamine_experiment(self):
        """Design an experiment to study dopamine effects"""
        
        # Dopamine modulation parameters
        dopamine_conditions = {
            'baseline': {'scale': 1.0, 'variability': 0.1},
            'high': {'scale': 2.0, 'variability': 0.2},
            'low': {'scale': 0.5, 'variability': 0.05},
            'erratic': {'scale': 1.0, 'variability': 0.5}
        }
        
        # Learning rate parameters under different dopamine levels
        learning_rates = {
            'baseline': {'stdp': 0.01, 'hebbian': 0.02, 'structural': 0.0001},
            'high': {'stdp': 0.015, 'hebbian': 0.03, 'structural': 0.0002},
            'low': {'stdp': 0.008, 'hebbian': 0.015, 'structural': 0.00005},
            'erratic': {'stdp': 0.02, 'hebbian': 0.04, 'structural': 0.0003}
        }
        
        return {
            'dopamine_conditions': dopamine_conditions,
            'learning_rates': learning_rates
        }
        
    def run_dopamine_manipulation_experiment(self):
        """Run an experiment manipulating dopamine levels"""
        
        print("Running dopamine manipulation experiment...")
        
        # Create baseline configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("neuromod.dopamine.scale", 1.0)
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        agent.initialize(world)
        
        # Enable neuromodulation
        agent.enableRewardModulation(True)
        
        # Run multiple trials with different dopamine levels
        dopamine_levels = [0.0, 0.5, 1.0, -0.5, -1.0]  # Negative = low dopamine
        results = {}
        
        for dopamine_level in dopamine_levels:
            print(f"Testing dopamine level: {dopamine_level}")
            
            # Reset agent
            agent.reset()
            agent.enableDevelopment(True)
            
            # Modify agent behavior based on dopamine level
            # This would involve modifying internal state or plasticity parameters
            
            # Run simulation
            trial_rewards = []
            trial_curiosities = []
            trial_plasticity = []
            
            for step in range(300):
                # Simulate environment
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation (would be affected by dopamine level)
                agent.applyRewardModulation(result.reward * (1.0 + dopamine_level), 0.0)
                agent.updateDevelopment(0.1)
                
                # Collect data
                trial_rewards.append(result.reward)
                trial_curiosities.append(agent.getCuriosityLevel())
                trial_plasticity.append(agent.plasticityModifier_)
            
            # Compute trial statistics
            results[f"dopamine_{dopamine_level}"] = {
                'avg_reward': np.mean(trial_rewards),
                'std_reward': np.std(trial_rewards),
                'max_reward': np.max(trial_rewards),
                'min_reward': np.min(trial_rewards),
                'avg_curiosity': np.mean(trial_curiosities),
                'plasticity_decay': trial_plasticity[-1] - trial_plasticity[0],
                'steps_above_threshold': sum(1 for r in trial_rewards if r > 0.1)
            }
            
            print(f"  Average reward: {results[f'dopamine_{dopamine_level}']['avg_reward']:.4f}")
        
        return results
        
    def analyze_neuromodulation_effects(self, results):
        """Analyze the effects of different neuromodulation conditions"""
        
        print("\nAnalyzing neuromodulation effects...")
        
        analysis = {
            'dopamine_reward_relationship': {},
            'curiosity_impact': {},
            'plasticity_dynamics': {},
            'behavioral_strategies': {}
        }
        
        # Analyze reward vs dopamine level
        dopamine_levels = [level for level in results.keys() if level.startswith('dopamine_')]
        rewards = [results[level]['avg_reward'] for level in dopamine_levels]
        
        # Find optimal dopamine level for reward
        optimal_idx = np.argmax(rewards)
        optimal_dopamine = float(dopamine_levels[optimal_idx].split('_')[1])
        
        analysis['dopamine_reward_relationship'] = {
            'optimal_dopamine_level': optimal_dopamine,
            'optimal_reward': rewards[optimal_idx],
            'reward_curve': dict(zip([float(l.split('_')[1]) for l in dopamine_levels], rewards)),
            'monotonic': self._is_monotonic(rewards),
            'inverted_u': self._is_inverted_u(rewards)
        }
        
        # Analyze curiosity vs reward
        curiosities = [results[level]['avg_curiosity'] for level in dopamine_levels]
        analysis['curiosity_impact'] = {
            'curiosity_reward_correlation': np.corrcoef(rewards, curiosities)[0,1],
            'curiosity_optimal_point': self._find_optimal_curiosity_point(rewards, curiosities)
        }
        
        # Analyze plasticity dynamics
        plasticities = [results[level]['plasticity_decay'] for level in dopamine_levels]
        analysis['plasticity_dynamics'] = {
            'plasticity_dopamine_correlation': np.corrcoef([float(l.split('_')[1]) for l in dopamine_levels], plasticities)[0,1],
            'plasticity_stability': self._assess_plasticity_stability(plasticities)
        }
        
        return analysis
        
    def _is_monotonic(self, values):
        """Check if values are monotonic"""
        increasing = all(x <= y for x, y in zip(values, values[1:]))
        decreasing = all(x >= y for x, y in zip(values, values[1:]))
        return increasing or decreasing
        
    def _is_inverted_u(self, values):
        """Check if values form an inverted U shape"""
        if len(values) < 3:
            return False
            
        # Simple check: find peak and see if values decrease on both sides
        peak_idx = np.argmax(values)
        if peak_idx == 0 or peak_idx == len(values) - 1:
            return False
            
        left_decreasing = all(values[i] <= values[i+1] for i in range(peak_idx))
        right_decreasing = all(values[i] >= values[i+1] for i in range(peak_idx, len(values)-1))
        
        return left_decreasing and right_decreasing
        
    def _find_optimal_curiosity_point(self, rewards, curiosities):
        """Find the curiosity level that maximizes reward"""
        if not rewards or not curiosities:
            return None
            
        # Find index of maximum reward
        max_reward_idx = np.argmax(rewards)
        return {
            'curiosity': curiosities[max_reward_idx],
            'reward': rewards[max_reward_idx],
            'rank': max_reward_idx / len(rewards)  # Normalized rank
        }
        
    def _assess_plasticity_stability(self, plasticities):
        """Assess stability of plasticity changes"""
        if len(plasticities) < 2:
            return 'unknown'
            
        # Check for large fluctuations
        fluctuations = [abs(plasticities[i] - plasticities[i-1]) for i in range(1, len(plasticities))]
        max_fluctuation = max(fluctuations)
        
        if max_fluctuation < 0.1:
            return 'stable'
        elif max_fluctuation < 0.3:
            return 'moderately_volatile'
        else:
            return 'highly_volatile'

# Usage
if __name__ == "__main__":
    researcher = NeuromodulationResearcher()
    
    # Design experiment
    experiment_design = researcher.design_dopamine_experiment()
    print("Experiment design created")
    
    # Run experiment
    results = researcher.run_dopamine_manipulation_experiment()
    
    # Analyze results
    analysis = researcher.analyze_neuromodulation_effects(results)
    
    print("\n=== Neuromodulation Research Analysis ===")
    print(f"Optimal dopamine level: {analysis['dopamine_reward_relationship']['optimal_dopamine_level']:.3f}")
    print(f"Optimal reward: {analysis['dopamine_reward_relationship']['optimal_reward']:.4f}")
    print(f"Curiosity-reward correlation: {analysis['curiosity_impact']['curiosity_reward_correlation']:.4f}")
    print(f"Plasticity stability: {analysis['plasticity_dynamics']['plasticity_stability']}")
    
    # Save results
    import json
    import datetime
    
    research_data = {
        'timestamp': datetime.datetime.now().isoformat(),
        'experiment': 'dopamine_manipulation',
        'results': results,
        'analysis': analysis,
        'findings': [
            f"Optimal dopamine level for reward: {analysis['dopamine_reward_relationship']['optimal_dopamine_level']:.3f}",
            f"Curiosity-reward relationship: {analysis['curiosity_impact']['curiosity_reward_correlation']:.4f}",
            f"Plasticity stability: {analysis['plasticity_dynamics']['plasticity_stability']}"
        ]
    }
    
    with open("neuromodulation_research_results.json", "w") as f:
        json.dump(research_data, f, indent=2)
    
    print("\nResearch data saved to neuromodulation_research_results.json")
```

## Summary

This comprehensive documentation provides:

1. **Quick Start Examples** - Beginner-friendly examples for rapid NLM adoption
2. **Phase 6 Integration Examples** - Advanced examples for complete system integration
3. **Command-Line Interface** - Enhanced CLI with configuration file support
4. **Advanced Python Features** - Custom neuron configuration and real-time monitoring
5. **Research Framework** - Systematic experimentation and data collection tools
6. **Configuration Management** - Advanced configuration comparison and parameter sweep capabilities
7. **Performance Monitoring** - Comprehensive performance tracking and optimization
8. **Neuromodulation Research** - Specialized tools for neuromodulation experimentation

The NLM project now includes extensive examples, documentation, and tools that support:

- **Research**: Systematic experimentation and data analysis
- **Development**: Configuration management and testing workflows
- **Performance**: Monitoring and optimization capabilities
- **Education**: Learning through examples and documentation

These improvements make NLM more accessible to researchers, developers, and educators while maintaining its scientific rigor and performance characteristics.

---

**Key Improvements Made:**

1. **Enhanced Python Bindings** - Comprehensive API with error handling and type conversions
2. **Improved AgentBrain** - Robust validation and error handling with neuron pointer validation
3. **Advanced Experiment Framework** - Configurable experiment design and data collection
4. **Performance Monitoring Tools** - Real-time system monitoring and anomaly detection
5. **Research Framework** - Systematic experimentation tools for neuroscience research
6. **Configuration Management** - Advanced configuration comparison and parameter sweep capabilities
7. **Comprehensive Documentation** - Detailed examples and documentation for all NLM features
8. **Neuromodulation Research Tools** - Specialized tools for studying neuromodulation effects

These improvements significantly enhance the usability, accessibility, and research capabilities of NLM while maintaining its scientific integrity and performance characteristics.