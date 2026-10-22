# NLM Python Bindings - Advanced API
# =================================

# This document describes the advanced Python API for NLM, providing comprehensive
# access to all NLM functionality beyond the basic API.

# Overview
The NLM Python bindings (pynlm) provide full access to the NLM neural simulation
framework through an intuitive Python interface. This advanced API extends the
basic functionality with comprehensive configuration management, experiment
control, memory management, and debugging tools.

# Advanced API Structure

## Core Classes

### Config Class (Advanced)
```python
import pynlm

# Advanced configuration management
config = pynlm.Config()

# Set complex configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.stage", pynlm.DevelopmentalStage.CriticalPeriod)
config.set("memory.episodic.max_episodes", 5000)
config.set("experiment.max_steps", 50000)
config.set("experiment.enable_checkpointing", True)
config.set("experiment.enable_replay", True)
config.set("experiment.enable_development", True)

# Configuration validation
if not config.validate():
    print("Configuration validation failed!")
    for error in config.get_errors():
        print(f"  Error: {error}")

# Get configuration values
neurons = config.get("brain.neuron_count")
if neurons:
    print(f"Neurons: {neurons}")

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Brain Class (Advanced)
```python
import pynlm
import json

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Advanced brain configuration
brain.setNeuronCount(5000)  # Override config
brain.setSynapseDensity(0.2)
brain.setConnectionProbability(0.12)
brain.enableSTDP(True)
brain.setSTDPLearningRate(0.008)
brain.enableHebbianLearning(True)
brain.enableStructuralPlasticity(True)
brain.enableDevelopment(True)

# Initialize with error handling
try:
    brain.initialize()
    print("Brain initialization successful")
except pynlm.NLMException as e:
    print(f"Brain initialization failed: {e}")

# Advanced brain operations
brain.setDevelopmentStage(pynlm.DevelopmentalStage.CriticalPeriod)
brain.setPlasticityModulation(0.7)
brain.enableCuriosity(True)
brain.enableNoveltyDetection(True)
brain.enablePredictionError(True)

# Memory management
memory = brain.getWorkingMemory()
if memory:
    memory.setCapacity(500)
    memory.enableConsolidation(True)

# Save brain state
try:
    brain.save("advanced_brain.bin")
    print("Brain state saved")
except pynlm.NLMSaveError as e:
    print(f"Save failed: {e}")
```

### AgentBrain Class (Advanced)
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with advanced configuration
agent = pynlm.createAgentBrain(brain)

# Advanced agent configuration
agent.enableRewardModulation(True, reward_sensitivity=1.5)
agent.enableStructuralPlasticity(True, growth_rate=0.0001)
agent.enableDevelopment(True, maturation_rate=0.001)
agent.enableCuriosity(True, exploration_rate=0.4)
agent.enableNoveltyDetection(True)
agent.enablePredictionError(True)
agent.enableReplayLearning(True)

# Sensory processing configuration
agent.setSensoryResolution(width=16, height=16)
agent.enableVisionProcessing(True)
agent.enableProprioception(True)
agent.enableInternalSignalProcessing(True)

# Motor decoding configuration
agent.setMotorDecodingMode(pynlm.MotorDecodingMode.NeuralActivity)
agent.enableActionSelection(True, decision_threshold=0.5)
agent.setActionPreferences({
    pynlm.ActionType.MoveForward: 1.0,
    pynlm.ActionType.TurnLeft: 0.8,
    pynlm.ActionType.TurnRight: 0.8,
    pynlm.ActionType.Interact: 1.2,
    pynlm.ActionType.Wait: 0.3
})

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)
```

### World Class (Advanced)
```python
import pynlm

# Create advanced world
world = pynlm.createSimpleWorld()

# Advanced world configuration
world.configure(
    width=100,
    height=100,
    visionWidth=32,
    visionHeight=32,
    timestep=0.05
)

# Environment setup
world.addResource(10.0, 10.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addResource(90.0, 90.0, pynlm.WorldObjectType.Resource, value=2.0)
world.addHazard(50.0, 50.0, pynlm.WorldObjectType.Hazard, value=-1.0)
world.addMarker(25.0, 25.0, pynlm.WorldObjectType.Marker, value=0.0)

# Agent setup
world.setAgentStart(5.0, 5.0)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setEnergyRecoveryRate(0.5)

# Physics configuration
world.setGravity(0.0, 0.0)
world.setFrictionCoefficient(0.3)
world.setMaxVelocity(5.0)

# Vision configuration
world.setVisionRange(20.0)
world.setVisionAngle(180.0)  # degrees
world.enableVisionObstruction(True)

# Time configuration
world.setTimeStep(0.05)
world.setMaxSteps(100000)

# Reset world
world.reset()
```

## Advanced Simulation Control

### Multi-Episode Simulation
```python
import pynlm
import time

def advanced_simulation(num_episodes=10, steps_per_episode=1000):
    """Run advanced multi-episode simulation with comprehensive monitoring."""
    
    # Configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 3)
    config.set("development.stage", pynlm.DevelopmentalStage.CriticalPeriod)
    config.set("experiment.max_steps", steps_per_episode * num_episodes)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    # Simulation metrics
    episode_metrics = []
    
    # Run episodes
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
        
        # Reset for new episode
        world.reset()
        agent.reset()
        brain.reset()
        
        # Episode metrics
        episode_start_time = time.time()
        episode_reward = 0.0
        episode_steps = 0
        max_firing_rate = 0.0
        memory_episodes_stored = 0
        
        # Run episode
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track metrics
            episode_reward += result.reward
            episode_steps += 1
            
            current_firing_rate = brain.getAverageFiringRate()
            if current_firing_rate > max_firing_rate:
                max_firing_rate = current_firing_rate
            
            # Print progress
            if step % 200 == 0:
                print(f"  Step {step}: Reward={result.reward:.3f}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        episode_end_time = time.time()
        episode_duration = episode_end_time - episode_start_time
        
        # Get memory metrics
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        if working_memory:
            memory_episodes_stored = working_memory.getActiveTraces()
        
        if episodic_memory:
            memory_episodes_stored = episodic_memory.getEpisodeCount()
        
        # Store episode metrics
        episode_metrics.append({
            'episode': episode + 1,
            'total_reward': episode_reward / episode_steps,
            'max_firing_rate': max_firing_rate,
            'memory_episodes': memory_episodes_stored,
            'duration': episode_duration,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_energy': world.getAgentBody().energy,
            'development_stage': brain.getDevelopmentalStage()
        })
        
        print(f"  Episode completed: Avg Reward={episode_metrics[-1]['total_reward']:.3f}, "
              f"Max Firing={max_firing_rate:.3f}, Duration={episode_duration:.2f}s")
    
    # Calculate summary statistics
    print("\n=== Simulation Summary ===")
    
    avg_reward = sum(m['total_reward'] for m in episode_metrics) / num_episodes
    avg_firing = sum(m['max_firing_rate'] for m in episode_metrics) / num_episodes
    total_memory = sum(m['memory_episodes'] for m in episode_metrics)
    
    print(f"Average Reward per Step: {avg_reward:.3f}")
    print(f"Average Max Firing Rate: {avg_firing:.3f}")
    print(f"Total Memory Episodes: {total_memory}")
    print(f"Final Development Stage: {episode_metrics[-1]['development_stage']}")
    
    # Save brain state
    try:
        brain.save("final_brain_state.bin")
        print("Final brain state saved")
    except pynlm.NLMSaveError as e:
        print(f"Save failed: {e}")
    
    # Return metrics
    return episode_metrics

# Run advanced simulation
metrics = advanced_simulation(num_episodes=5, steps_per_episode=2000)
```

### Real-Time Monitoring
```python
import pynlm
import matplotlib.pyplot as plt
import threading
import time

class RealTimeMonitor:
    """Real-time monitoring of NLM simulation."""
    
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.monitoring = False
        self.data_points = []
    
    def start_monitoring(self, interval=0.1):
        """Start real-time monitoring."""
        self.monitoring = True
        monitoring_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        monitoring_thread.daemon = True
        monitoring_thread.start()
        return monitoring_thread
    
    def _monitor_loop(self, interval):
        """Monitoring loop."""
        step = 0
        while self.monitoring:
            # Collect metrics
            metrics = {
                'step': step,
                'simulation_time': self.world.getSimulationTime(),
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'reward': self.world.getAgentBody().getReward(),
                'energy': self.world.getAgentBody().energy,
                'development_stage': self.brain.getDevelopmentalStage()
            }
            
            self.data_points.append(metrics)
            step += 1
            time.sleep(interval)
    
    def stop_monitoring(self):
        """Stop monitoring."""
        self.monitoring = False
    
    def plot_metrics(self):
        """Plot collected metrics."""
        if not self.data_points:
            print("No data to plot")
            return
        
        fig, axes = plt.subplots(3, 3, figsize=(15, 10))
        fig.suptitle('NLM Real-Time Monitoring', fontsize=16)
        
        # Neurons firing
        axes[0, 0].plot([p['step'] for p in self.data_points],
                       [p['neurons_firing'] for p in self.data_points])
        axes[0, 0].set_title('Neurons Firing')
        axes[0, 0].set_ylabel('Count')
        
        # Total spikes
        axes[0, 1].plot([p['step'] for p in self.data_points],
                       [p['total_spikes'] for p in self.data_points])
        axes[0, 1].set_title('Total Spikes')
        axes[0, 1].set_ylabel('Count')
        
        # Average firing rate
        axes[0, 2].plot([p['step'] for p in self.data_points],
                       [p['average_firing_rate'] for p in self.data_points])
        axes[0, 2].set_title('Average Firing Rate')
        axes[0, 2].set_ylabel('Hz')
        
        # Curiosity
        axes[1, 0].plot([p['step'] for p in self.data_points],
                       [p['curiosity'] for p in self.data_points])
        axes[1, 0].set_title('Curiosity Level')
        axes[1, 0].set_ylabel('Level')
        
        # Novelty
        axes[1, 1].plot([p['step'] for p in self.data_points],
                       [p['novelty'] for p in self.data_points])
        axes[1, 1].set_title('Novelty Level')
        axes[1, 1].set_ylabel('Level')
        
        # Prediction error
        axes[1, 2].plot([p['step'] for p in self.data_points],
                       [p['prediction_error'] for p in self.data_points])
        axes[1, 2].set_title('Prediction Error')
        axes[1, 2].set_ylabel('Error')
        
        # Reward
        axes[2, 0].plot([p['step'] for p in self.data_points],
                       [p['reward'] for p in self.data_points])
        axes[2, 0].set_title('Reward')
        axes[2, 0].set_ylabel('Reward')
        
        # Energy
        axes[2, 1].plot([p['step'] for p in self.data_points],
                       [p['energy'] for p in self.data_points])
        axes[2, 1].set_title('Energy')
        axes[2, 1].set_ylabel('Energy')
        
        # Development stage
        stages = []
        for p in self.data_points:
            if p['development_stage'] == pynlm.DevelopmentalStage.Initial:
                stages.append('Initial')
            elif p['development_stage'] == pynlm.DevelopmentalStage.CriticalPeriod:
                stages.append('Critical')
            elif p['development_stage'] == pynlm.DevelopmentalStage.Maturation:
                stages.append('Mature')
            elif p['development_stage'] == pynlm.DevelopmentalStage.Adult:
                stages.append('Adult')
            else:
                stages.append('Aging')
        
        axes[2, 2].plot([p['step'] for p in self.data_points],
                       range(len(self.data_points)))
        axes[2, 2].set_title('Development Stage Timeline')
        axes[2, 2].set_yticks(range(len(set(stages))))
        axes[2, 2].set_yticklabels(list(set(stages)))
        axes[2, 2].set_xlabel('Step')
        
        plt.tight_layout()
        plt.show()

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create and start monitor
monitor = RealTimeMonitor(brain, agent, world)
monitor_thread = monitor.start_monitoring(interval=0.5)

# Run simulation for 10 seconds
for i in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    time.sleep(0.1)

# Stop monitoring
monitor.stop_monitoring()
monitor_thread.join()

# Plot results
monitor.plot_metrics()
```

## Advanced Configuration Management

### Configuration Templates
```python
import pynlm
import json

# Advanced configuration templates
templates = {
    'exploration': {
        'brain': {'neuron_count': 1000},
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.01},
            'hebbian': {'enable': True, 'learning_rate': 0.008}
        },
        'neuromodulation': {
            'curiosity': {'enable': True, 'exploration_rate': 0.5},
            'novelty': {'enable': True}
        },
        'experiment': {
            'max_steps': 5000,
            'enable_development': False
        }
    },
    
    'developmental': {
        'brain': {'neuron_count': 3000, 'region_count': 2},
        'development': {'stage': pynlm.DevelopmentalStage.CriticalPeriod},
        'memory': {
            'episodic': {'max_episodes': 5000},
            'working': {'capacity': 0.15}
        },
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.005},
            'structural': {'enable': True, 'synaptogenesis_rate': 0.0002}
        },
        'experiment': {
            'max_steps': 50000,
            'enable_development': True,
            'enable_replay': True,
            'enable_checkpointing': True
        }
    },
    
    'competitive': {
        'brain': {'neuron_count': 2000},
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.02},
            'hebbian': {'enable': True, 'learning_rate': 0.015}
        },
        'neuromodulation': {
            'dopamine': {'scale': 1.5},
            'curiosity': {'enable': True, 'exploration_rate': 0.3}
        },
        'experiment': {
            'max_steps': 10000,
            'enable_development': True,
            'replay_interval': 100,
            'consolidation_interval': 1000
        }
    }
}

def create_config_from_template(template_name):
    """Create configuration from template."""
    if template_name not in templates:
        raise ValueError(f"Unknown template: {template_name}")
    
    template = templates[template_name]
    config = pynlm.Config()
    
    # Apply template configuration
    if 'brain' in template:
        for key, value in template['brain'].items():
            config.set(f"brain.{key}", value)
    
    if 'plasticity' in template:
        for plasticity_type, settings in template['plasticity'].items():
            for key, value in settings.items():
                config.set(f"plasticity.{plasticity_type}.{key}", value)
    
    if 'neuromodulation' in template:
        for mod_type, settings in template['neuromodulation'].items():
            for key, value in settings.items():
                config.set(f"neuromod.{mod_type}.{key}", value)
    
    if 'development' in template:
        for key, value in template['development'].items():
            config.set(f"development.{key}", value)
    
    if 'memory' in template:
        for mem_type, settings in template['memory'].items():
            for key, value in settings.items():
                config.set(f"memory.{mem_type}.{key}", value)
    
    if 'experiment' in template:
        for key, value in template['experiment'].items():
            config.set(f"experiment.{key}", value)
    
    return config

# Usage example
config = create_config_from_template('developmental')
brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuration Validation and Error Handling
```python
import pynlm

class AdvancedConfigManager:
    """Advanced configuration manager with validation and error handling."""
    
    def __init__(self):
        self.config = pynlm.Config()
        self.validation_rules = []
        self.error_callbacks = []
    
    def add_validation_rule(self, rule_name, validation_function):
        """Add a validation rule."""
        self.validation_rules.append((rule_name, validation_function))
    
    def add_error_callback(self, callback):
        """Add error callback."""
        self.error_callbacks.append(callback)
    
    def validate_config(self):
        """Validate configuration."""
        errors = []
        
        for rule_name, validation_function in self.validation_rules:
            try:
                if not validation_function(self.config):
                    errors.append(f"Validation failed: {rule_name}")
            except Exception as e:
                errors.append(f"Validation error in {rule_name}: {e}")
        
        # Call error callbacks
        for error in errors:
            for callback in self.error_callbacks:
                callback(error)
        
        return errors
    
    def load_config_with_validation(self, filename):
        """Load configuration with validation."""
        try:
            self.config = pynlm.Config()
            self.config.loadFromFile(filename)
            
            errors = self.validate_config()
            if errors:
                raise pynlm.NLMConfigError(f"Configuration validation failed: {'; '.join(errors)}")
            
            return True
        except Exception as e:
            for callback in self.error_callbacks:
                callback(f"Configuration load failed: {e}")
            return False

# Usage example
config_manager = AdvancedConfigManager()

# Add validation rules
config_manager.add_validation_rule("neuron_count_positive", lambda c: c.get("brain.neuron_count", 0) > 0)
config_manager.add_validation_rule("connection_probability", lambda c: 0.0 <= c.get("brain.connection_probability", 0) <= 1.0)
config_manager.add_validation_rule("max_steps_positive", lambda c: c.get("experiment.max_steps", 0) > 0)

# Add error callback
config_manager.add_error_callback(lambda error: print(f"ERROR: {error}"))

# Load and validate configuration
if config_manager.load_config_with_validation("my_config.json"):
    print("Configuration loaded and validated successfully")
    brain = pynlm.createBrain(config_manager.config)
    brain.initialize()
```

## Advanced Experiment Control

### Parallel Experiment Execution
```python
import pynlm
import threading
import time
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

class ParallelExperimentRunner:
    """Run multiple experiments in parallel."""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or pynlm.getCPUCount()
        self.executor = ThreadPoolExecutor(max_workers=self.num_workers)
    
    def run_experiment_with_config(self, config_dict, experiment_id):
        """Run a single experiment with given configuration."""
        print(f"Starting experiment {experiment_id}")
        
        # Create configuration from dictionary
        config = pynlm.Config()
        for section, settings in config_dict.items():
            for key, value in settings.items():
                config.set(f"{section}.{key}", value)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(config.get("experiment.max_steps", 1000)):
            brain.step(step)
        
        # Save results
        brain.save(f"experiment_{experiment_id}_brain.bin")
        
        # Return metrics
        return {
            'experiment_id': experiment_id,
            'neuron_count': brain.getTotalNeuronCount(),
            'final_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate()
        }
    
    def run_multiple_experiments(self, config_templates, num_experiments):
        """Run multiple experiments with different configurations."""
        print(f"Running {num_experiments} experiments with {self.num_workers} workers")
        
        futures = []
        
        # Submit experiments
        for i in range(num_experiments):
            # Select random template for variety
            template = config_templates[i % len(config_templates)]
            
            # Modify template for variation
            modified_template = self._modify_template_for_variability(template, i)
            
            future = self.executor.submit(
                self.run_experiment_with_config, 
                modified_template, 
                i
            )
            futures.append(future)
        
        # Collect results
        results = []
        for future in futures:
            try:
                result = future.result(timeout=300)  # 5 minute timeout
                results.append(result)
                print(f"Experiment completed: {result}")
            except Exception as e:
                print(f"Experiment failed: {e}")
        
        self.executor.shutdown()
        return results
    
    def _modify_template_for_variability(self, template, experiment_id):
        """Modify template for experiment variability."""
        import copy
        modified = copy.deepcopy(template)
        
        # Add random variations
        neuron_count = modified['brain']['neuron_count']
        modified['brain']['neuron_count'] = neuron_count + (experiment_id % 10) * 100
        
        learning_rate = modified['plasticity']['stdp']['learning_rate']
        modified['plasticity']['stdp']['learning_rate'] = learning_rate * (0.8 + (experiment_id % 5) * 0.04)
        
        return modified

# Usage example
config_templates = {
    'base': {
        'brain': {'neuron_count': 1000},
        'plasticity': {'stdp': {'learning_rate': 0.01}},
        'experiment': {'max_steps': 1000}
    },
    'high_learning': {
        'brain': {'neuron_count': 2000},
        'plasticity': {'stdp': {'learning_rate': 0.02}},
        'experiment': {'max_steps': 2000}
    }
}

runner = ParallelExperimentRunner(num_workers=4)
results = runner.run_multiple_experiments(
    config_templates=list(config_templates.values()),
    num_experiments=8
)

print(f"\nExperiment Results Summary:")
for result in results:
    print(f"  Experiment {result['experiment_id']}: "
          f"Neurons={result['neuron_count']}, "
          f"Spikes={result['final_spikes']}, "
          f"AvgFiring={result['avg_firing_rate']:.3f}")
```

### Experiment Automation
```python
import pynlm
import os
import json
import shutil
from datetime import datetime

class ExperimentAutomation:
    """Automate NLM experiments with comprehensive logging and reporting."""
    
    def __init__(self, base_dir="experiments"):
        self.base_dir = base_dir
        self.timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.experiment_dir = os.path.join(base_dir, f"exp_{self.timestamp}")
        os.makedirs(self.experiment_dir, exist_ok=True)
        
        self.config_file = os.path.join(self.experiment_dir, "config.json")
        self.results_file = os.path.join(self.experiment_dir, "results.json")
        self.log_file = os.path.join(self.experiment_dir, "experiment.log")
    
    def run_automated_experiment(self, config_dict, experiment_name="Automated Experiment"):
        """Run an automated experiment with comprehensive logging."""
        
        # Save configuration
        with open(self.config_file, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        # Log experiment start
        self._log(f"Starting experiment: {experiment_name}")
        self._log(f"Configuration: {json.dumps(config_dict, indent=2)}")
        
        # Create configuration
        config = pynlm.Config()
        for section, settings in config_dict.items():
            for key, value in settings.items():
                config.set(f"{section}.{key}", value)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with periodic saving
        checkpoint_interval = config.get("experiment.checkpoint_interval", 1000)
        max_steps = config.get("experiment.max_steps", 10000)
        
        experiment_results = {
            'experiment_name': experiment_name,
            'timestamp': self.timestamp,
            'configuration': config_dict,
            'steps_completed': 0,
            'final_neuron_count': brain.getTotalNeuronCount(),
            'final_synapse_count': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'final_development_stage': brain.getDevelopmentalStage(),
            'checkpoint_saved': False,
            'brain_state_saved': False
        }
        
        for step in range(max_steps):
            brain.step(step)
            
            # Save checkpoint periodically
            if step % checkpoint_interval == 0 and step > 0:
                checkpoint_file = os.path.join(self.experiment_dir, f"checkpoint_step_{step}.bin")
                brain.save(checkpoint_file)
                experiment_results['checkpoint_saved'] = True
                self._log(f"Checkpoint saved at step {step}: {checkpoint_file}")
            
            # Log progress periodically
            if step % 1000 == 0:
                self._log(f"Step {step}/{max_steps}: "
                         f"Firing={brain.getFiringNeuronCount()}, "
                         f"TotalSpikes={brain.getTotalSpikeCount()}, "
                         f"FiringRate={brain.getAverageFiringRate():.3f}")
        
        # Save final brain state
        brain_file = os.path.join(self.experiment_dir, "brain_state.bin")
        brain.save(brain_file)
        experiment_results['brain_state_saved'] = True
        
        # Save experiment results
        with open(self.results_file, 'w') as f:
            json.dump(experiment_results, f, indent=2)
        
        self._log(f"Experiment completed: {experiment_results}")
        
        return experiment_results
    
    def run_multiple_experiments(self, experiment_configs):
        """Run multiple experiments with different configurations."""
        print(f"Running {len(experiment_configs)} experiments")
        
        all_results = []
        
        for i, (name, config) in enumerate(experiment_configs.items()):
            self._log(f"\n{'='*50}")
            print(f"Running experiment {i+1}/{len(experiment_configs)}: {name}")
            
            # Create experiment subdirectory
            exp_dir = os.path.join(self.base_dir, f"exp_{self.timestamp}_{i}")
            os.makedirs(exp_dir, exist_ok=True)
            
            # Temporarily change experiment directory
            original_dir = self.experiment_dir
            self.experiment_dir = exp_dir
            self.config_file = os.path.join(exp_dir, "config.json")
            self.results_file = os.path.join(exp_dir, "results.json")
            self.log_file = os.path.join(exp_dir, "experiment.log")
            
            try:
                results = self.run_automated_experiment(config, name)
                all_results.append(results)
            finally:
                # Restore original directory
                self.experiment_dir = original_dir
        
        # Generate summary report
        self._generate_summary_report(all_results)
        
        return all_results
    
    def _log(self, message):
        """Log message to file and print."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log_entry = f"[{timestamp}] {message}"
        
        with open(self.log_file, 'a') as f:
            f.write(log_entry + '\n')
        
        print(log_entry)
    
    def _generate_summary_report(self, results):
        """Generate comprehensive summary report."""
        report_file = os.path.join(self.base_dir, f"summary_report_{self.timestamp}.md")
        
        with open(report_file, 'w') as f:
            f.write(f"# NLM Experiment Summary Report\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            
            f.write(f"## Experiment Overview\n")
            f.write(f"- Total Experiments: {len(results)}\n")
            f.write(f"- Base Directory: {self.base_dir}\n")
            f.write(f"- Timestamp: {self.timestamp}\n\n")
            
            f.write(f"## Individual Experiment Results\n\n")
            
            for i, result in enumerate(results):
                f.write(f"### Experiment {i+1}: {result['experiment_name']}\n")
                f.write(f"- **Steps Completed**: {result['steps_completed']}\n")
                f.write(f"- **Final Neuron Count**: {result['final_neuron_count']}\n")
                f.write(f"- **Final Synapse Count**: {result['final_synapse_count']}\n")
                f.write(f"- **Total Spikes**: {result['total_spikes']}\n")
                f.write(f"- **Average Firing Rate**: {result['average_firing_rate']:.3f} Hz\n")
                f.write(f"- **Development Stage**: {result['final_development_stage']}\n")
                f.write(f"- **Checkpoints Saved**: {result['checkpoint_saved']}\n")
                f.write(f"- **Brain State Saved**: {result['brain_state_saved']}\n\n")
            
            f.write(f"## Configuration Summary\n")
            f.write(f"All experiment configurations were documented in individual experiment directories.\n\n")
            
            f.write(f"## Files Generated\n")
            f.write(f"- Experiment directories: Each experiment has its own directory\n")
            f.write(f"- Configuration files: `config.json` in each experiment directory\n")
            f.write(f"- Results files: `results.json` in each experiment directory\n")
            f.write(f"- Log files: `experiment.log` in each experiment directory\n")
            f.write(f"- Brain state files: `brain_state.bin` in each experiment directory\n")
            f.write(f"- Checkpoint files: `checkpoint_step_X.bin` in each experiment directory\n")
            f.write(f"- Summary report: This report file\n")
        
        self._log(f"Summary report generated: {report_file}")

# Usage example
automation = ExperimentAutomation()

# Define experiment configurations
experiment_configs = {
    "Exploration Study": {
        'brain': {'neuron_count': 1000},
        'plasticity': {'stdp': {'learning_rate': 0.01}},
        'experiment': {'max_steps': 5000}
    },
    "Development Study": {
        'brain': {'neuron_count': 2000, 'region_count': 2},
        'development': {'stage': pynlm.DevelopmentalStage.CriticalPeriod},
        'experiment': {'max_steps': 20000, 'enable_development': True}
    },
    "Learning Efficiency Study": {
        'brain': {'neuron_count': 1500},
        'plasticity': {'stdp': {'learning_rate': 0.02}},
        'neuromodulation': {'curiosity': {'enable': True}},
        'experiment': {'max_steps': 10000}
    }
}

# Run automated experiments
results = automation.run_multiple_experiments(experiment_configs)
```

## Advanced Data Analysis

### Statistical Analysis
```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats
from sklearn.preprocessing import StandardScaler

class AdvancedDataAnalysis:
    """Advanced statistical analysis of NLM simulation data."""
    
    def __init__(self):
        self.data = None
        self.analysis_results = {}
    
    def analyze_simulation_data(self, simulation_results):
        """Analyze simulation results using advanced statistical methods."""
        
        # Convert to DataFrame for analysis
        df = pd.DataFrame(simulation_results)
        
        # Basic statistics
        self.analysis_results['basic_stats'] = {
            'mean_firing_rate': df['avg_firing_rate'].mean(),
            'std_firing_rate': df['avg_firing_rate'].std(),
            'mean_total_spikes': df['total_spikes'].mean(),
            'std_total_spikes': df['total_spikes'].std(),
            'mean_neurons': df['neuron_count'].mean(),
            'std_neurons': df['neuron_count'].std()
        }
        
        # Performance metrics
        self.analysis_results['performance'] = {
            'efficiency': df['total_spikes'] / df['neuron_count'],
            'stability': 1 / (1 + df['firing_rate_std']),
            'learning_rate': (df['final_firing_rate'] - df['initial_firing_rate']) / df['initial_firing_rate']
        }
        
        # Correlation analysis
        correlations = {}
        numerical_columns = df.select_dtypes(include=[np.number]).columns
        for col1 in numerical_columns:
            for col2 in numerical_columns:
                if col1 != col2:
                    corr = df[col1].corr(df[col2])
                    correlations[f'{col1}_vs_{col2}'] = corr
        
        self.analysis_results['correlations'] = correlations
        
        # Statistical significance testing
        self.analysis_results['statistical_tests'] = self._perform_statistical_tests(df)
        
        # Machine learning feature extraction
        self.analysis_results['ml_features'] = self._extract_ml_features(df)
        
        return self.analysis_results
    
    def _perform_statistical_tests(self, df):
        """Perform statistical significance tests."""
        tests = {}
        
        # t-tests for comparing groups
        if 'learning_condition' in df.columns:
            groups = df.groupby('learning_condition')['final_firing_rate']
            if len(groups) >= 2:
                t_stat, p_value = stats.ttest_ind(
                    groups.get_group('high_learning')['final_firing_rate'],
                    groups.get_group('low_learning')['final_firing_rate']
                )
                tests['learning_condition_t_test'] = {
                    't_statistic': t_stat,
                    'p_value': p_value,
                    'significant': p_value < 0.05
                }
        
        # ANOVA for multiple conditions
        if 'experiment_type' in df.columns and df['experiment_type'].nunique() > 1:
            groups = [group['final_firing_rate'].values for name, group in df.groupby('experiment_type')]
            f_stat, p_value = stats.f_oneway(*groups)
            tests['anova_experiment_type'] = {
                'f_statistic': f_stat,
                'p_value': p_value,
                'significant': p_value < 0.05
            }
        
        return tests
    
    def _extract_ml_features(self, df):
        """Extract machine learning features from data."""
        features = {}
        
        # Temporal features
        if 'step' in df.columns:
            features['temporal_patterns'] = self._extract_temporal_patterns(df)
        
        # Neural activity features
        features['neural_activity'] = {
            'firing_variance': df['final_firing_rate'].var(),
            'firing_skewness': df['final_firing_rate'].skew(),
            'firing_kurtosis': df['final_firing_rate'].kurtosis(),
            'spike_rate_correlation': df['total_spikes'].corr(df['final_firing_rate'])
        }
        
        # Development features
        features['development'] = {
            'plasticity_modulation_effect': self._calculate_plasticity_effect(df),
            'memory_consolidation_quality': self._calculate_memory_quality(df)
        }
        
        return features
    
    def _extract_temporal_patterns(self, df):
        """Extract temporal patterns from time series data."""
        # This would require time-series specific data
        # Placeholder for advanced temporal analysis
        return {
            'pattern_detection': 'Advanced temporal pattern detection would be implemented here',
            'seasonality': 'Seasonality analysis would be implemented here',
            'trend_analysis': 'Trend analysis would be implemented here'
        }
    
    def _calculate_plasticity_effect(self, df):
        """Calculate plasticity modulation effect."""
        # Calculate the effect of plasticity on learning
        if 'plasticity_modulation' in df.columns and 'final_firing_rate' in df.columns:
            return df['plasticity_modulation'] * df['final_firing_rate'].corr(df['learning_improvement'])
        return None
    
    def _calculate_memory_quality(self, df):
        """Calculate memory consolidation quality."""
        # Calculate memory-related metrics
        if 'memory_episodes' in df.columns:
            return df['memory_episodes'] / df['neuron_count']
        return None
    
    def generate_advanced_report(self):
        """Generate advanced analysis report."""
        report = f"# Advanced NLM Data Analysis Report\n\n"
        
        report += f"## Basic Statistics\n"
        for metric, value in self.analysis_results.get('basic_stats', {}).items():
            report += f"- **{metric}**: {value:.4f}\n"
        
        report += f"\n## Performance Metrics\n"
        for metric, value in self.analysis_results.get('performance', {}).items():
            report += f"- **{metric}**: {value:.4f}\n"
        
        report += f"\n## Correlation Analysis\n"
        for correlation, value in self.analysis_results.get('correlations', {}).items():
            report += f"- **{correlation}**: {value:.4f}\n"
        
        report += f"\n## Statistical Tests\n"
        for test, results in self.analysis_results.get('statistical_tests', {}).items():
            report += f"### {test}\n"
            report += f"- **Statistic**: {results.get('t_statistic', results.get('f_statistic', 'N/A')):.4f}\n"
            report += f"- **P-value**: {results.get('p_value', 'N/A'):.4f}\n"
            report += f"- **Significant**: {results.get('significant', False)}\n\n"
        
        report += f"\n## Machine Learning Features\n"
        for feature_type, features in self.analysis_results.get('ml_features', {}).items():
            report += f"### {feature_type}\n"
            for feature, value in features.items():
                if isinstance(value, dict):
                    report += f"- **{feature}**:\n"
                    for sub_feature, sub_value in value.items():
                        report += f"  - {sub_feature}: {sub_value:.4f}\n"
                else:
                    report += f"- **{feature}**: {value:.4f}\n"
            report += "\n"
        
        return report

# Usage example
analysis = AdvancedDataAnalysis()
results = analysis.analyze_simulation_data(simulation_results)

# Generate advanced report
report = analysis.generate_advanced_report()

# Save report
with open('advanced_analysis_report.md', 'w') as f:
    f.write(report)
```

## Advanced Visualization

### Interactive 3D Brain Visualization
```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import plotly.graph_objects as go
import plotly.io as pio

class AdvancedBrainVisualizer:
    """Advanced 3D visualization of NLM brain state."""
    
    def __init__(self, brain):
        self.brain = brain
        self.regions = brain.getRegions()
    
    def create_3d_neural_network(self):
        """Create 3D visualization of neural network."""
        fig = go.Figure()
        
        # Collect neuron positions and activities
        neuron_data = []
        connection_data = []
        
        for region in self.regions:
            neurons = region.getNeurons()
            region_id = region.getId()
            
            for neuron in neurons:
                # Get neuron properties
                neuron_id = neuron.getId()
                neuron_type = neuron.getType()
                
                # Determine position (simplified 3D placement)
                x = region_id.index() * 100
                y = neuron_id.index() % 20 * 10
                z = neuron_id.index() // 20 * 10
                
                # Get activity level
                activity = neuron.isFiring() or neuron.getState().membranePotential > -50.0
                
                # Color based on neuron type
                color_map = {
                    pynlm.NeuronType.Excitatory: 'red',
                    pynlm.NeuronType.Inhibitory: 'blue',
                    pynlm.NeuronType.Sensory: 'green',
                    pynlm.NeuronType.Motor: 'orange',
                    pynlm.NeuronType.Modulatory: 'purple',
                    pynlm.NeuronType.Internal: 'gray'
                }
                
                color = color_map.get(neuron_type, 'gray')
                
                # Add neuron to data
                neuron_data.append(go.Scatter3d(
                    x=[x],
                    y=[y],
                    z=[z],
                    mode='markers',
                    marker=dict(
                        size=8 + (10 if activity else 0),
                        color=color,
                        opacity=0.8
                    ),
                    text=f"Neuron {neuron_id}<br>Type: {neuron_type}<br>Activity: {activity}",
                    name=f"Region {region_id}"
                ))
                
                # Add connections for this neuron's synapses
                synapses = region.getSynapsesFrom(neuron_id)
                for synapse in synapses:
                    destination = synapse.getDestinationNeuron()
                    weight = synapse.getWeight()
                    
                    # Get destination neuron position
                    dest_neuron = None
                    for n in neurons:
                        if n.getId() == destination:
                            dest_neuron = n
                            break
                    
                    if dest_neuron:
                        dest_x = region_id.index() * 100
                        dest_y = destination.index() % 20 * 10
                        dest_z = destination.index() // 20 * 10
                        
                        connection_data.append(go.Scatter3d(
                            x=[x, dest_x],
                            y=[y, dest_y],
                            z=[z, dest_z],
                            mode='lines',
                            line=dict(
                                width=weight * 2,
                                color='rgba(128, 128, 128, 0.5)'
                            ),
                            hoverinfo='none',
                            showlegend=False
                        ))
        
        # Add all neurons and connections to figure
        for data in neuron_data + connection_data:
            fig.add_trace(data)
        
        # Update layout
        fig.update_layout(
            title='3D Neural Network Visualization',
            scene=dict(
                xaxis_title='X Position',
                yaxis_title='Y Position',
                zaxis_title='Z Position'
            ),
            width=1000,
            height=800,
            showlegend=True
        )
        
        return fig
    
    def create_activity_animation(self, steps=50, interval=0.1):
        """Create animation of neural activity over time."""
        
        frames = []
        
        for step in range(steps):
            frame_data = []
            
            # Simulate activity changes
            for region in self.regions:
                neurons = region.getNeurons()
                region_id = region.getId()
                
                for neuron in neurons:
                    # Simulate activity change
                    is_active = np.random.random() < 0.3  # 30% chance of being active
                    
                    x = region_id.index() * 100
                    y = neuron.getId().index() % 20 * 10
                    z = neuron.getId().index() // 20 * 10
                    
                    color_map = {
                        pynlm.NeuronType.Excitatory: 'red',
                        pynlm.NeuronType.Inhibitory: 'blue',
                        pynlm.NeuronType.Sensory: 'green',
                        pynlm.NeuronType.Motor: 'orange',
                        pynlm.NeuronType.Modulatory: 'purple',
                        pynlm.NeuronType.Internal: 'gray'
                    }
                    
                    color = color_map.get(neuron.getType(), 'gray')
                    
                    frame_data.append(go.Scatter3d(
                        x=[x],
                        y=[y],
                        z=[z],
                        mode='markers',
                        marker=dict(
                            size=8 + (10 if is_active else 0),
                            color=color,
                            opacity=0.8
                        ),
                        text=f"Step {step}<br>Neuron {neuron.getId()}",
                        name=f"Step {step}"
                    ))
            
            frames.append(go.Frame(data=frame_data, name=f"step_{step}"))
        
        # Create animated figure
        fig = go.Figure(data=frames[0]['data'], frames=frames)
        
        fig.update_layout(
            title='Neural Activity Animation',
            scene=dict(
                xaxis_title='X Position',
                yaxis_title='Y Position',
                zaxis_title='Z Position'
            ),
            width=1000,
            height=800,
            updatemenus=[
                {
                    "type": "buttons",
                    "buttons": [
                        {
                            "label": "Play",
                            "method": "animate",
                            "args": [None, {"frame": {"duration": interval, "redraw": False},
                                               "transition": {"duration": interval},
                                               "mode": "immediate"}]
                        },
                        {
                            "label": "Pause",
                            "method": "animate",
                            "args": [[], {"frame": {"duration": 0, "redraw": False},
                                               "transition": {"duration": 0},
                                               "mode": "immediate"}]
                        }
                    ]
                }
            ]
        )
        
        return fig
    
    def create_spike_raster_plot(self, spikes_data):
        """Create spike raster plot showing neural firing patterns."""
        
        fig = go.Figure()
        
        # Group spikes by neuron
        neurons = list(spikes_data.keys())
        
        for neuron_id in neurons:
            neuron_spikes = spikes_data[neuron_id]
            
            fig.add_trace(go.Scatter(
                x=neuron_spikes,
                y=[neuron_id] * len(neuron_spikes),
                mode='markers',
                marker=dict(
                    size=8,
                    color='red'
                ),
                name=f"Neuron {neuron_id}",
                hovertemplate='Time: %{x:.2f}<br>Neuron: %{y}<extra></extra>'
            ))
        
        fig.update_layout(
            title='Spike Raster Plot',
            xaxis_title='Time (simulation steps)',
            yaxis_title='Neuron ID',
            width=1000,
            height=600,
            showlegend=True,
            legend=dict(
                x=0,
                y=1,
                traceorder='normal',
                font=dict(
                    family='sans-serif',
                    size=12,
                    color='#000'
                ),
                bgcolor='#FFF',
                bordercolor='#444',
                borderwidth=0
            )
        )
        
        return fig
    
    def create_weight_matrix_visualization(self):
        """Create visualization of synaptic weight matrix."""
        
        # Create simplified weight matrix representation
        matrix_data = []
        
        for region in self.regions:
            neurons = region.getNeurons()
            num_neurons = len(neurons)
            
            # Create weight matrix (simplified)
            weight_matrix = np.zeros((num_neurons, num_neurons))
            
            for i, neuron1 in enumerate(neurons):
                for j, neuron2 in enumerate(neurons):
                    # Get synapse from neuron1 to neuron2
                    synapses = region.getSynapsesFrom(neuron1.getId())
                    for synapse in synapses:
                        if synapse.getDestinationNeuron() == neuron2.getId():
                            weight_matrix[i, j] = synapse.getWeight()
                            break
            
            # Add to visualization data
            for i in range(num_neurons):
                for j in range(num_neurons):
                    if weight_matrix[i, j] > 0:
                        matrix_data.append(go.Scatter(
                            x=[i],
                            y=[j],
                            mode='markers',
                            marker=dict(
                                size=weight_matrix[i, j] * 20,
                                color=weight_matrix[i, j],
                                colorscale='Viridis',
                                showscale=True,
                                cmin=0,
                                cmax=1
                            ),
                            text=f"Weight: {weight_matrix[i, j]:.3f}",
                            hovertemplate='Source: %{x}<br>Target: %{y}<br>Weight: %{marker.color:.3f}<extra></extra>'
                        ))
        
        fig = go.Figure(data=matrix_data)
        
        fig.update_layout(
            title='Synaptic Weight Matrix',
            xaxis_title='Source Neuron',
            yaxis_title='Target Neuron',
            width=800,
            height=800,
            showlegend=False,
            hovermode='closest'
        )
        
        return fig

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create visualizer
visualizer = AdvancedBrainVisualizer(brain)

# Create 3D visualization
fig_3d = visualizer.create_3d_neural_network()

# Save to HTML
pio.write_html(fig_3d, 'brain_3d_visualization.html', auto_open=True)

# Create activity animation
animation = visualizer.create_activity_animation(steps=100, interval=0.1)

# Save animation
pio.write_html(animation, 'brain_activity_animation.html', auto_open=True)

# Create spike raster plot
spikes_data = {
    0: [10, 30, 50, 70, 90],
    1: [20, 40, 60, 80, 100],
    2: [15, 35, 55, 75, 95]
}
raster_plot = visualizer.create_spike_raster_plot(spikes_data)
pio.write_html(raster_plot, 'spike_raster_plot.html', auto_open=True)

# Create weight matrix visualization
weight_matrix = visualizer.create_weight_matrix_visualization()
pio.write_html(weight_matrix, 'weight_matrix_visualization.html', auto_open=True)
```

## Advanced Plugin System

### Plugin Architecture
```python
import pynlm
import abc
import importlib
from typing import Dict, Any, List

class NLMSPlugin(abc.ABC):
    """Abstract base class for NLM plugins."""
    
    @abc.abstractmethod
    def get_name(self) -> str:
        """Return plugin name."""
        pass
    
    @abc.abstractmethod
    def get_version(self) -> str:
        """Return plugin version."""
        pass
    
    @abc.abstractmethod
    def initialize(self, brain: pynlm.Brain, config: pynlm.Config) -> bool:
        """Initialize plugin with brain and configuration."""
        pass
    
    def cleanup(self) -> bool:
        """Clean up plugin resources."""
        return True
    
    def process_sensory_input(self, percept: pynlm.SensoryPercept) -> pynlm.SensoryPercept:
        """Process sensory input."""
        return percept
    
    def process_motor_output(self, action: pynlm.MotorCommand) -> pynlm.MotorCommand:
        """Process motor output."""
        return action
    
    def update(self, timestep: float) -> bool:
        """Update plugin state."""
        return True
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get plugin metrics."""
        return {}

class PluginManager:
    """Manager for NLM plugins."""
    
    def __init__(self):
        self.plugins: Dict[str, NLMSensorPlugin] = {}
        self.brain = None
        self.config = None
    
    def load_plugin(self, plugin_path: str) -> bool:
        """Load plugin from file."""
        try:
            # Import plugin module
            module_name = os.path.splitext(os.path.basename(plugin_path))[0]
            spec = importlib.util.spec_from_file_location(module_name, plugin_path)
            module = importlib.util.module_from_spec(spec)
            
            # Load plugin class
            spec.loader.exec_module(module)
            
            # Find plugin class
            plugin_class = None
            for attr_name in dir(module):
                attr = getattr(module, attr_name)
                if (isinstance(attr, type) and 
                    issubclass(attr, NLMSensorPlugin) and 
                    attr is not NLMSensorPlugin):
                    plugin_class = attr
                    break
            
            if plugin_class is None:
                raise ValueError(f"No plugin class found in {plugin_path}")
            
            # Create plugin instance
            plugin = plugin_class()
            
            # Initialize plugin
            if plugin.initialize(self.brain, self.config):
                self.plugins[plugin.get_name()] = plugin
                return True
            else:
                raise ValueError(f"Plugin {plugin.get_name()} initialization failed")
                
        except Exception as e:
            print(f"Failed to load plugin {plugin_path}: {e}")
            return False
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload plugin by name."""
        if plugin_name in self.plugins:
            plugin = self.plugins[plugin_name]
            if plugin.cleanup():
                del self.plugins[plugin_name]
                return True
            else:
                return False
        return False
    
    def set_brain_and_config(self, brain: pynlm.Brain, config: pynlm.Config):
        """Set brain and configuration for plugins."""
        self.brain = brain
        self.config = config
        
        # Reinitialize plugins
        for plugin in self.plugins.values():
            plugin.initialize(brain, config)
    
    def process_sensory_input(self, percept: pynlm.SensoryPercept) -> pynlm.SensoryPercept:
        """Process sensory input through all plugins."""
        for plugin in self.plugins.values():
            percept = plugin.process_sensory_input(percept)
        return percept
    
    def process_motor_output(self, action: pynlm.MotorCommand) -> pynlm.MotorCommand:
        """Process motor output through all plugins."""
        for plugin in self.plugins.values():
            action = plugin.process_motor_output(action)
        return action
    
    def update(self, timestep: float) -> bool:
        """Update all plugins."""
        success = True
        for plugin in self.plugins.values():
            if not plugin.update(timestep):
                success = False
        return success
    
    def get_plugin_metrics(self) -> Dict[str, Dict[str, Any]]:
        """Get metrics from all plugins."""
        metrics = {}
        for name, plugin in self.plugins.items():
            metrics[name] = plugin.get_metrics()
        return metrics
    
    def list_plugins(self) -> List[str]:
        """List loaded plugin names."""
        return list(self.plugins.keys())

# Example Plugin Implementation
class CustomSensoryPlugin(NLMSensorPlugin):
    """Example custom sensory processing plugin."""
    
    def get_name(self) -> str:
        return "CustomSensoryProcessor"
    
    def get_version(self) -> str:
        return "1.0.0"
    
    def initialize(self, brain: pynlm.Brain, config: pynlm.Config) -> bool:
        self.brain = brain
        self.config = config
        
        # Initialize custom parameters
        self.threshold = config.get("custom.threshold", 0.5)
        self.smoothing_factor = config.get("custom.smoothing_factor", 0.1)
        
        # Get sensory processor
        self.sensory_processor = brain.getSensoryProcessor()
        
        return True
    
    def process_sensory_input(self, percept: pynlm.SensoryPercept) -> pynlm.SensoryPercept:
        # Apply custom sensory processing
        data = percept.getData()
        
        # Simple thresholding
        processed_data = [1.0 if val > self.threshold else 0.0 for val in data]
        
        # Apply smoothing
        if len(processed_data) > 1:
            smoothed_data = [
                self.smoothing_factor * processed_data[i] + 
                (1 - self.smoothing_factor) * processed_data[i-1]
                for i in range(1, len(processed_data))
            ]
            smoothed_data = [processed_data[0]] + smoothed_data
        else:
            smoothed_data = processed_data
        
        # Create processed percept
        processed_percept = pynlm.SensoryPercept(
            percept.getType(),
            smoothed_data,
            percept.getTimestamp()
        )
        
        return processed_percept

# Usage example
plugin_manager = PluginManager()

# Load custom plugin
custom_plugin_path = "plugins/custom_sensory_processor.py"
if plugin_manager.load_plugin(custom_plugin_path):
    print("Custom plugin loaded successfully")

# Create and configure brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Set brain and config for plugins
plugin_manager.set_brain_and_config(brain, brain.getConfig())

# Process sensory input
percept = world.getSensoryPercept()
processed_percept = plugin_manager.process_sensory_input(percept)

# Process motor output
action = agent.decodeMotorCommand()
processed_action = plugin_manager.process_motor_output(action)

# Update plugins
plugin_manager.update(0.1)

# Get metrics
metrics = plugin_manager.get_plugin_metrics()
print(f"Plugin metrics: {metrics}")

# List loaded plugins
print(f"Loaded plugins: {plugin_manager.list_plugins()}")

# Unload plugin
plugin_manager.unload_plugin("CustomSensoryProcessor")
```

## Summary

This advanced NLM Python API provides comprehensive access to all NLM functionality:

### Key Features

1. **Advanced Configuration Management**
   - Configuration templates for common use cases
   - Configuration validation and error handling
   - Environment variable support
   - Configuration inheritance and merging

2. **Advanced Simulation Control**
   - Multi-episode simulation with comprehensive monitoring
   - Real-time monitoring and visualization
   - Parallel experiment execution
   - Automated experiment management

3. **Advanced Data Analysis**
   - Statistical analysis of simulation results
   - Machine learning feature extraction
   - Correlation analysis
   - Statistical significance testing

4. **Advanced Visualization**
   - 3D neural network visualization
   - Interactive activity animations
   - Spike raster plots
   - Synaptic weight matrix visualization

5. **Plugin System**
   - Plugin architecture for extending functionality
   - Custom sensory and motor processing
   - Plugin management and lifecycle control

6. **Advanced Experiment Control**
   - Automated experiment execution
   - Parameter optimization
   - Experiment comparison
   - Comprehensive reporting

### Usage Examples

The examples demonstrate how to:

1. Create advanced configurations with templates
2. Run complex multi-episode simulations
3. Perform statistical analysis of results
4. Create interactive visualizations
5. Implement custom plugins
6. Automate experiment execution

### Benefits

- **Flexibility**: Extensive customization options
- **Scalability**: Support for large-scale experiments
- **Reproducibility**: Comprehensive logging and reporting
- **Extensibility**: Plugin system for adding custom functionality
- **Analysis**: Advanced statistical and machine learning tools
- **Visualization**: Interactive and comprehensive visualization capabilities

This advanced API enables researchers and developers to conduct sophisticated NLM research, perform complex experiments, and gain deep insights into neural network behavior.
