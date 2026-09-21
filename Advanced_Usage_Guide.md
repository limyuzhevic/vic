# Advanced NLM Usage Guide

This guide covers advanced usage patterns, debugging techniques, and custom configurations for NLM power users.

## Advanced Configuration

### Custom Configuration File

Create a custom configuration file for complex experiments:

```json
{
  "brain": {
    "neuron_count": 5000,
    "synapse_density": 0.15,
    "connection_probability": 0.08,
    "v_thresh": -55.0,
    "v_rest": -65.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.002,
      "tau_plus": 25.0,
      "tau_minus": 25.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.005
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0005,
      "pruning_rate": 0.00005
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.2,
      "baseline": 0.1
    },
    "curiosity": {
      "enable": true,
      "decay_rate": 0.95
    },
    "novelty": {
      "enable": true,
      "threshold": 0.3
    }
  },
  "development": {
    "enable_critical_period": true,
    "critical_period_end": 120.0,
    "plasticity_decay_rate": 0.8
  },
  "world": {
    "width": 50,
    "height": 50,
    "vision_width": 16,
    "vision_height": 16,
    "max_energy": 200.0,
    "energy_decay": 0.99
  }
}
```

### Loading Custom Configuration

```python
import pynlm

# Create configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Override specific settings
config.set("brain.neuron_count", 10000)
config.set("plasticity.stdp.learning_rate", 0.005)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

## Advanced Agent Behavior

### Custom Agent with Specialized Processing

```python
import pynlm
import numpy as np

class CustomAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.world = world
        self.agent.initialize(world)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Custom parameters
        self.exploration_rate = 0.2
        self.exploitation_rate = 0.8
        self.memory_buffer = []
        self.max_memory = 1000
        
    def advanced_process(self, percept, reward, action):
        # Store experience for learning
        experience = {
            'percept': percept,
            'reward': reward,
            'action': action,
            'novelty': self.calculate_novelty(percept)
        }
        
        if len(self.memory_buffer) >= self.max_memory:
            self.memory_buffer.pop(0)
        self.memory_buffer.append(experience)
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Apply reward with temporal difference
        predicted_reward = self.predict_reward(percept)
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Get action with custom policy
        return self.custom_policy()
    
    def calculate_novelty(self, percept):
        # Advanced novelty detection
        vision = percept.getVision()
        if vision:
            # Calculate complexity of visual input
            mean_intensity = np.mean(vision)
            variance = np.var(vision)
            return variance / (mean_intensity + 0.001) * 0.1
        return 0.0
    
    def predict_reward(self, percept):
        # Simple reward prediction using recent experiences
        if self.memory_buffer:
            recent = self.memory_buffer[-10:]
            avg_reward = np.mean([exp['reward'] for exp in recent])
            return avg_reward
        return 0.0
    
    def custom_policy(self):
        # Custom exploration-exploitation policy
        import random
        
        curiosity = self.agent.getCuriosityLevel()
        prediction_error = self.agent.getPredictionError()
        neuromodulation = self.agent.getNeuromodulationLevel()
        
        # Dynamic exploration rate based on uncertainty
        uncertainty = curiosity + abs(prediction_error) * 0.5
        exploration = min(0.5, uncertainty * 2.0)
        
        if random.random() < exploration:
            # Explore: random action
            return random.choice([
                pynlm.ActionType.MoveForward,
                pynlm.ActionType.MoveBackward,
                pynlm.ActionType.TurnLeft,
                pynlm.ActionType.TurnRight,
                pynlm.ActionType.Interact,
                pynlm.ActionType.Wait
            ])
        else:
            # Exploit: use agent's learned policy
            return self.agent.decodeMotorCommand()

# Usage example
if __name__ == "__main__":
    # Setup brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Create custom agent
    agent = CustomAgent(brain, world)
    
    # Run simulation with advanced processing
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Get action from agent's custom policy
        action = agent.custom_policy()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Advanced processing
        agent.advanced_process(percept, reward, action)
        
        brain.step(step)
        
        # Log statistics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Curiosity: {agent.agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.agent.getPredictionError():.3f}")
            print(f"  Neuromodulation: {agent.agent.getNeuromodulationLevel():.3f}")
            print(f"  Memory size: {len(agent.memory_buffer)}")
            print(f"  Development stage: {agent.agent.getDevelopmentalStage()}")
```

## Performance Monitoring and Debugging

### Brain Performance Monitor

```python
import time
import pynlm

class BrainPerformanceMonitor:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Performance metrics
        self.metrics = {
            'spike_count': [],
            'firing_rate': [],
            'curiosity_level': [],
            'novelty_level': [],
            'prediction_error': [],
            'neuromodulation_level': [],
            'step_times': [],
            'memory_usage': []
        }
        
        # Performance thresholds
        self.thresholds = {
            'max_spike_rate': 1000.0,  # spikes per second
            'max_curiosity': 1.0,
            'max_novelty': 1.0,
            'max_prediction_error': 10.0,
            'max_neuromodulation': 1.0
        }
        
    def run_monitoring(self, num_steps=1000):
        print("Starting performance monitoring...")
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Update world
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record performance metrics
            self.record_metrics(step, time.time() - step_start)
            
            # Check for performance issues
            self.check_performance_issues(step)
            
            if step % 100 == 0:
                self.print_summary(step)
        
        print("\nMonitoring complete!")
        self.generate_report()
    
    def record_metrics(self, step, step_time):
        # Record spike metrics
        spike_count = self.brain.getTotalSpikeCount()
        firing_rate = self.brain.getAverageFiringRate()
        
        self.metrics['spike_count'].append(spike_count)
        self.metrics['firing_rate'].append(firing_rate)
        self.metrics['curiosity_level'].append(self.agent.getCuriosityLevel())
        self.metrics['novelty_level'].append(self.agent.getNoveltyLevel())
        self.metrics['prediction_error'].append(self.agent.getPredictionError())
        self.metrics['neuromodulation_level'].append(self.agent.getNeuromodulationLevel())
        self.metrics['step_times'].append(step_time)
        
        # Simple memory usage estimation
        memory_usage = (
            len(self.agent.getBrain().getRegions()) * 1000 +
            len(self.agent.getBrain().getRegionIds()) * 100
        )
        self.metrics['memory_usage'].append(memory_usage)
    
    def check_performance_issues(self, step):
        # Check for unusual activity patterns
        firing_rate = self.brain.getAverageFiringRate()
        curiosity = self.agent.getCuriosityLevel()
        
        # Too high firing rate might indicate instability
        if firing_rate > self.thresholds['max_spike_rate']:
            print(f"WARNING (Step {step}): High firing rate detected: {firing_rate}")
        
        # Too high curiosity might indicate excessive exploration
        if curiosity > self.thresholds['max_curiosity']:
            print(f"WARNING (Step {step}): High curiosity level detected: {curiosity}")
        
        # Check step time (too slow might indicate performance issues)
        step_time = self.metrics['step_times'][-1]
        if step_time > 0.1:  # 100ms per step
            print(f"WARNING (Step {step}): Slow step time: {step_time:.3f}s")
    
    def print_summary(self, step):
        recent_spikes = self.metrics['spike_count'][-100:] if len(self.metrics['spike_count']) >= 100 else self.metrics['spike_count']
        avg_spike_rate = np.mean(recent_spikes) if recent_spikes else 0.0
        
        print(f"\nStep {step} Summary:")
        print(f"  Avg spike rate (last 100 steps): {avg_spike_rate:.1f}")
        print(f"  Current curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Current novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Current prediction error: {self.agent.getPredictionError():.3f}")
        print(f"  Current neuromodulation: {self.agent.getNeuromodulationLevel():.3f}")
        print(f"  Step time: {self.metrics['step_times'][-1]:.4f}s")
    
    def generate_report(self):
        import numpy as np
        
        print("\n" + "="*60)
        print("PERFORMANCE REPORT")
        print("="*60)
        
        # Calculate statistics
        if self.metrics['spike_count']:
            avg_total_spikes = np.mean(self.metrics['spike_count'])
            avg_firing_rate = np.mean(self.metrics['firing_rate'])
            avg_curiosity = np.mean(self.metrics['curiosity_level'])
            avg_novelty = np.mean(self.metrics['novelty_level'])
            avg_prediction_error = np.mean(self.metrics['prediction_error'])
            avg_neuromodulation = np.mean(self.metrics['neuromodulation_level'])
            avg_step_time = np.mean(self.metrics['step_times'])
            avg_memory = np.mean(self.metrics['memory_usage'])
            
            print(f"\nSpike Statistics:")
            print(f"  Average total spikes: {avg_total_spikes:.1f}")
            print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
            
            print(f"\nNeuromodulation Statistics:")
            print(f"  Average curiosity level: {avg_curiosity:.3f}")
            print(f"  Average novelty level: {avg_novelty:.3f}")
            print(f"  Average prediction error: {avg_prediction_error:.3f}")
            print(f"  Average neuromodulation level: {avg_neuromodulation:.3f}")
            
            print(f"\nPerformance Statistics:")
            print(f"  Average step time: {avg_step_time:.4f}s")
            print(f"  Average memory usage: {avg_memory:.0f} units")
            
            print(f"\nStability Metrics:")
            spike_std = np.std(self.metrics['spike_count']) if len(self.metrics['spike_count']) > 1 else 0.0
            curiosity_std = np.std(self.metrics['curiosity_level']) if len(self.metrics['curiosity_level']) > 1 else 0.0
            print(f"  Spike count standard deviation: {spike_std:.1f}")
            print(f"  Curiosity level standard deviation: {curiosity_std:.3f}")
            
            # Determine overall stability
            stability_score = 0.0
            if spike_std / (avg_total_spikes + 0.001) < 0.1:
                stability_score += 0.4
            if curiosity_std / (avg_curiosity + 0.001) < 0.2:
                stability_score += 0.3
            if avg_step_time < 0.05:
                stability_score += 0.3
                
            print(f"\nOverall Stability Score: {stability_score:.2f}/1.0")
            
            if stability_score > 0.8:
                print("Status: STABLE - System behaving as expected")
            elif stability_score > 0.6:
                print("Status: MODERATE - Some performance issues detected")
            else:
                print("Status: UNSTABLE - Significant performance issues")
```

### Debugging Tools

```python
import pynlm

class NLMDebugger:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.debug_log = []
        
    def capture_state(self, label=""):
        """Capture current brain and agent state for debugging"""
        state = {
            'label': label,
            'timestamp': time.time(),
            'brain_stats': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'firing_neuron_count': self.brain.getFiringNeuronCount(),
                'total_spike_count': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage()
            },
            'agent_stats': {
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
                'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled(),
                'development_enabled': self.agent.isDevelopmentEnabled(),
                'curiosity_enabled': self.agent.isCuriosityEnabled()
            },
            'world_stats': {
                'simulation_time': self.world.getSimulationTime(),
                'agent_x': self.world.getAgentBody().x,
                'agent_y': self.world.getAgentBody().y,
                'agent_orientation': self.world.getAgentBody().orientation
            }
        }
        
        self.debug_log.append(state)
        return state
    
    def compare_states(self, state1, state2):
        """Compare two captured states to identify changes"""
        print(f"\nComparing states '{state1['label']}' and '{state2['label']}'")
        
        brain1 = state1['brain_stats']
        brain2 = state2['brain_stats']
        
        print("\nBrain Changes:")
        print(f"  Neurons: {brain1['total_neurons']} -> {brain2['total_neurons']}")
        print(f"  Synapses: {brain1['total_synapses']} -> {brain2['total_synapses']}")
        print(f"  Firing neurons: {brain1['firing_neuron_count']} -> {brain2['firing_neuron_count']}")
        print(f"  Total spikes: {brain1['total_spike_count']} -> {brain2['total_spike_count']}")
        print(f"  Firing rate: {brain1['average_firing_rate']:.2f} -> {brain2['average_firing_rate']:.2f} Hz")
        
        agent1 = state1['agent_stats']
        agent2 = state2['agent_stats']
        
        print("\nAgent Changes:")
        print(f"  Curiosity: {agent1['curiosity_level']:.3f} -> {agent2['curiosity_level']:.3f}")
        print(f"  Novelty: {agent1['novelty_level']:.3f} -> {agent2['novelty_level']:.3f}")
        print(f"  Prediction error: {agent1['prediction_error']:.3f} -> {agent2['prediction_error']:.3f}")
        print(f"  Neuromodulation: {agent1['neuromodulation_level']:.3f} -> {agent2['neuromodulation_level']:.3f}")
        
        world1 = state1['world_stats']
        world2 = state2['world_stats']
        
        print("\nWorld Changes:")
        print(f"  Simulation time: {world1['simulation_time']:.3f} -> {world2['simulation_time']:.3f}")
        print(f"  Agent position: ({world1['agent_x']:.1f}, {world1['agent_y']:.1f}) -> ({world2['agent_x']:.1f}, {world2['agent_y']:.1f})")
        print(f"  Agent orientation: {world1['agent_orientation']:.1f} -> {world2['agent_orientation']:.1f}")
    
    def export_debug_log(self, filename):
        """Export debug log to JSON file"""
        import json
        
        # Convert state objects to serializable format
        serializable_log = []
        for state in self.debug_log:
            serializable_state = state.copy()
            serializable_state['timestamp'] = state['timestamp']
            serializable_log.append(serializable_state)
        
        with open(filename, 'w') as f:
            json.dump(serializable_log, f, indent=2)
        
        print(f"Debug log exported to {filename}")
```

## Batch Experiments

### Running Multiple Experiments

```python
import pynlm
import json
import time
import numpy as np

class ExperimentRunner:
    def __init__(self, num_experiments=10):
        self.num_experiments = num_experiments
        self.results = []
        
    def run_experiment(self, experiment_id, config_overrides=None):
        """Run a single experiment"""
        print(f"\nRunning Experiment {experiment_id}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply overrides
        if config_overrides:
            for key, value in config_overrides.items():
                if '.' in key:
                    # Handle nested keys
                    keys = key.split('.')
                    current = config
                    for i, k in enumerate(keys[:-1]):
                        if i == 0:
                            if hasattr(current, keys[0]):
                                current = getattr(current, keys[0])
                            else:
                                # Handle configuration key-value access
                                pass
                    # For simplicity, we'll use the string key approach
                    config.set(key, value)
                else:
                    config.set(key, value)
        
        # Create brain and world
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        experiment_results = {
            'experiment_id': experiment_id,
            'steps_completed': 0,
            'total_spikes': 0,
            'final_curiosity': 0.0,
            'final_novelty': 0.0,
            'final_prediction_error': 0.0,
            'final_neuromodulation': 0.0,
            'final_development_stage': None,
            'learning_detected': False,
            'completion_time': 0.0,
            'avg_step_time': 0.0
        }
        
        start_time = time.time()
        step_times = []
        
        for step in range(1000):
            step_start = time.time()
            
            world.update(0.1)
            percept = world.getSensoryPercept()
            
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            if step % 100 == 0:
                print(f"  Step {step}")
        
        # Record results
        end_time = time.time()
        completion_time = end_time - start_time
        
        experiment_results['steps_completed'] = 1000
        experiment_results['total_spikes'] = brain.getTotalSpikeCount()
        experiment_results['final_curiosity'] = agent.getCuriosityLevel()
        experiment_results['final_novelty'] = agent.getNoveltyLevel()
        experiment_results['final_prediction_error'] = agent.getPredictionError()
        experiment_results['final_neuromodulation'] = agent.getNeuromodulationLevel()
        experiment_results['final_development_stage'] = agent.getDevelopmentalStage()
        experiment_results['completion_time'] = completion_time
        experiment_results['avg_step_time'] = np.mean(step_times)
        
        # Detect learning
        initial_spikes = brain.getTotalSpikeCount() / 10  # Estimate from end
        final_spikes = brain.getTotalSpikeCount()
        spike_change = (final_spikes - initial_spikes) / (initial_spikes + 1)
        experiment_results['learning_detected'] = abs(spike_change) > 0.1
        
        return experiment_results
    
    def run_batch(self, experiment_configs=None):
        """Run multiple experiments"""
        print(f"Running batch of {self.num_experiments} experiments")
        
        if experiment_configs is None:
            # Default configurations for variations
            experiment_configs = [
                {},  # Baseline
                {'brain.neuron_count': 500},
                {'brain.neuron_count': 2000},
                {'plasticity.stdp.learning_rate': 0.005},
                {'plasticity.stdp.learning_rate': 0.002},
                {'neuromod.dopamine.scale': 1.5},
                {'neuromod.dopamine.scale': 0.5},
                {'neuromod.curiosity.enable': False},
                {'neuromod.novelty.enable': False},
                {'world.width': 30, 'world.height': 30}
            ]
        
        # Run experiments
        for i in range(min(self.num_experiments, len(experiment_configs))):
            config = experiment_configs[i]
            results = self.run_experiment(i, config)
            self.results.append(results)
            
            # Save intermediate results
            if (i + 1) % 5 == 0:
                self.save_results(f"experiment_results_part_{i+1}.json")
        
        # Save final results
        self.save_results("experiment_results_final.json")
        
        # Generate summary
        self.generate_summary()
        
        return self.results
    
    def save_results(self, filename):
        """Save experiment results to JSON file"""
        import json
        
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2, default=str)
        
        print(f"Results saved to {filename}")
    
    def generate_summary(self):
        """Generate summary of batch experiment results"""
        if not self.results:
            return
        
        print("\n" + "="*60)
        print("BATCH EXPERIMENT SUMMARY")
        print("="*60)
        
        # Calculate statistics
        num_experiments = len(self.results)
        total_spikes = sum(r['total_spikes'] for r in self.results)
        avg_spikes = total_spikes / num_experiments
        
        spikes_std = np.std([r['total_spikes'] for r in self.results]) if num_experiments > 1 else 0.0
        
        avg_curiosity = np.mean([r['final_curiosity'] for r in self.results])
        avg_novelty = np.mean([r['final_novelty'] for r in self.results])
        avg_prediction_error = np.mean([r['final_prediction_error'] for r in self.results])
        avg_neuromodulation = np.mean([r['final_neuromodulation'] for r in self.results])
        
        avg_completion_time = np.mean([r['completion_time'] for r in self.results])
        avg_step_time = np.mean([r['avg_step_time'] for r in self.results])
        
        learning_experiments = sum(1 for r in self.results if r['learning_detected'])
        learning_rate = learning_experiments / num_experiments
        
        print(f"\nExperiment Statistics:")
        print(f"  Total experiments: {num_experiments}")
        print(f"  Average total spikes: {avg_spikes:.1f} ± {spikes_std:.1f}")
        print(f"  Learning experiments: {learning_experiments}/{num_experiments} ({learning_rate:.1%})")
        
        print(f"\nFinal State Statistics:")
        print(f"  Average curiosity: {avg_curiosity:.3f}")
        print(f"  Average novelty: {avg_novelty:.3f}")
        print(f"  Average prediction error: {avg_prediction_error:.3f}")
        print(f"  Average neuromodulation: {avg_neuromodulation:.3f}")
        
        print(f"\nPerformance Statistics:")
        print(f"  Average completion time: {avg_completion_time:.2f}s")
        print(f"  Average step time: {avg_step_time:.4f}s")
        
        print(f"\nDevelopment Stages:")
        stages = {}
        for r in self.results:
            stage = str(r['final_development_stage'])
            stages[stage] = stages.get(stage, 0) + 1
        
        for stage, count in stages.items():
            print(f"  {stage}: {count} experiments")

# Example usage
if __name__ == "__main__":
    # Run single experiment
    print("Running single experiment...")
    runner = ExperimentRunner(num_experiments=1)
    results = runner.run_batch()
    
    # Run batch experiments
    print("\n\nRunning batch experiments...")
    runner = ExperimentRunner(num_experiments=10)
    results = runner.run_batch()
```

## Advanced Configuration Management

### Configuration Builder

```python
import pynlm
import json

class ConfigurationBuilder:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        
    def add_brain_config(self, neuron_count=1000, synapse_density=0.1,
                         connection_probability=0.05, v_thresh=-50.0, v_rest=-70.0):
        """Add brain configuration"""
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("brain.synapse_density", synapse_density)
        self.config.set("brain.connection_probability", connection_probability)
        self.config.set("brain.v_thresh", v_thresh)
        self.config.set("brain.v_rest", v_rest)
        return self
        
    def add_plasticity_config(self, stdp_enabled=True, stdp_learning_rate=0.001,
                              stdp_tau_plus=20.0, stdp_tau_minus=20.0,
                              hebbian_enabled=True, hebbian_learning_rate=0.001,
                              structural_enabled=True, synaptogenesis_rate=0.0001,
                              pruning_rate=0.00001):
        """Add plasticity configuration"""
        self.config.set("plasticity.stdp.enable", stdp_enabled)
        self.config.set("plasticity.stdp.learning_rate", stdp_learning_rate)
        self.config.set("plasticity.stdp.tau_plus", stdp_tau_plus)
        self.config.set("plasticity.stdp.tau_minus", stdp_tau_minus)
        self.config.set("plasticity.hebbian.enable", hebbian_enabled)
        self.config.set("plasticity.hebbian.learning_rate", hebbian_learning_rate)
        self.config.set("plasticity.structural.enable", structural_enabled)
        self.config.set("plasticity.structural.synaptogenesis_rate", synaptogenesis_rate)
        self.config.set("plasticity.structural.pruning_rate", pruning_rate)
        return self
        
    def add_neuromodulation_config(self, dopamine_scale=1.0, dopamine_baseline=0.0,
                                   curiosity_enabled=True, novelty_enabled=True,
                                   curiosity_decay=0.99):
        """Add neuromodulation configuration"""
        self.config.set("neuromod.dopamine.scale", dopamine_scale)
        self.config.set("neuromod.dopamine.baseline", dopamine_baseline)
        self.config.set("neuromod.curiosity.enable", curiosity_enabled)
        self.config.set("neuromod.novelty.enable", novelty_enabled)
        self.config.set("neuromod.curiosity.decay_rate", curiosity_decay)
        return self
        
    def add_development_config(self, enable_critical_period=True,
                               critical_period_end=120.0, plasticity_decay_rate=0.8):
        """Add development configuration"""
        self.config.set("development.enable_critical_period", enable_critical_period)
        self.config.set("development.critical_period_end", critical_period_end)
        self.config.set("development.plasticity_decay_rate", plasticity_decay_rate)
        return self
        
    def add_world_config(self, width=10, height=10, vision_width=8, vision_height=8,
                        max_energy=100.0, energy_decay=0.99):
        """Add world configuration"""
        self.config.set("world.width", width)
        self.config.set("world.height", height)
        self.config.set("world.vision_width", vision_width)
        self.config.set("world.vision_height", vision_height)
        self.config.set("world.max_energy", max_energy)
        self.config.set("world.energy_decay_rate", energy_decay)
        return self
        
    def add_logging_config(self, log_level="INFO", log_to_file=False, log_filename="nlm.log"):
        """Add logging configuration"""
        self.config.set("logging.level", log_level)
        self.config.set("logging.to_file", log_to_file)
        self.config.set("logging.filename", log_filename)
        return self
        
    def get_config(self):
        """Get the built configuration"""
        return self.config
        
    def export_config(self, filename):
        """Export configuration to JSON file"""
        import json
        
        # Extract configuration entries
        config_dict = {}
        keys = self.config.getKeys()
        
        for key in keys:
            if hasattr(self.config, 'get'):
                value = self.config.get(key)
                if value:
                    config_dict[key] = value
        
        with open(filename, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        print(f"Configuration exported to {filename}")

# Usage example
if __name__ == "__main__":
    # Build configuration step by step
    builder = ConfigurationBuilder()
    
    # Create a complex configuration
    config = (builder
              .add_brain_config(neuron_count=2000, v_thresh=-55.0, v_rest=-65.0)
              .add_plasticity_config(
                  stdp_learning_rate=0.002,
                  hebbian_learning_rate=0.005,
                  synaptogenesis_rate=0.0005,
                  pruning_rate=0.00005
              )
              .add_neuromodulation_config(
                  dopamine_scale=1.2,
                  curiosity_enabled=True,
                  novelty_enabled=True
              )
              .add_development_config(
                  critical_period_end=180.0,
                  plasticity_decay_rate=0.7
              )
              .add_world_config(
                  width=30, height=30, vision_width=12, vision_height=12,
                  max_energy=200.0
              )
              .add_logging_config(
                  log_level="DEBUG",
                  log_to_file=True
              )
              .get_config())
    
    # Create brain with built configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Export configuration
    builder.export_config("custom_config.json")
    
    print("Brain created with custom configuration!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Installation and Setup

### System Requirements

NLM requires:
- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 2.11.0+

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the C++ library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Build Python bindings
pip install pybind11 scikit-build-core
pip install .

# Run tests (if built)
ctest --output-on-failure
```

### Quick Development Setup

```bash
# Install development dependencies
pip install -e .[dev]

# Create a development build
mkdir build_dev
cd build_dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON
make -j$(nproc)

# Run the phase 6 demo to test installation
cd /path/to/nlm/build_dev
./nlm_phase6_demo
```

### Advanced Building Options

```bash
# Build with specific features
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
    -DNLM_ENABLE_MPI=ON \
    -DNLM_ENABLE_CUDA=ON \
    -DPYTHON_EXECUTABLE=/usr/bin/python3

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
make install
```

### Running Phase 6 Integration Demo

The Phase 6 demo provides comprehensive integration testing:

```bash
# Run the demo from the build directory
cd build
./nlm_phase6_demo

# Run with custom configuration
./nlm_phase6_demo --config my_config.cfg

# Run with visualization
./nlm_phase6_demo --enable_visualization

# Run with performance profiling
./nlm_phase6_demo --profile_performance
```

## Key Features Summary

### Core Brain Features
- **Leaky Integrate-and-Fire (LIF) neurons** with biological fidelity
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms**: STDP, Hebbian, structural
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Memory systems**: Working memory, episodic memory, associative memory
- **Prediction systems**: Forward models, prediction error computation
- **Cognition**: Neural planning, concept formation, attention
- **Development**: Critical periods, synaptogenesis, pruning, maturation

### Agent Features
- **Sensory processing**: Vision, touch, internal signals, proprioception
- **Motor control**: Multiple action types with smooth integration
- **Reward-based learning** with prediction error
- **Curiosity-driven exploration** for intrinsic motivation
- **Developmental plasticity** that changes with age
- **Novelty detection** for adaptive behavior

### Performance Features
- **Memory pools** for efficient allocation
- **Event queues** for parallel processing
- **SIMD vectorization** for numerical computation
- **Parallel processing** with multiple threads
- **Checkpoint system** for long-running simulations
- **Sparse connectivity** for scalability

### Visualization Features
- **Real-time brain state visualization**
- **Network graph visualization**
- **Spike raster plots**
- **Weight matrix visualization**
- **Neural population activity**

## Troubleshooting

### Common Issues

**Q: "ImportError: No module named 'pynlm'"**
A: Install the package properly:
```bash
pip install --force-reinstall .
```

**Q: "Segmentation fault"**
A: Ensure initialization is done correctly:
```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!
```

**Q: "Too many open files"**
A: Increase system limits or optimize configuration:
```bash
ulimit -n 65536
```

**Q: "Memory allocation failed"**
A: Reduce brain size or increase available memory:
```python
config.set("brain.neuron_count", 1000)  # Smaller brain
```

### Performance Optimization

For better performance:
1. **Use larger brain sizes** (≥10,000 neurons) to benefit from vectorization
2. **Enable MPI** for distributed computing
3. **Use specialized hardware** (SIMD, GPU acceleration where available)
4. **Optimize configuration** for your use case
5. **Use checkpointing** for long-running simulations

### Getting Help

- **Documentation**: Read `docs/ARCHITECTURE.md`, `docs/SCIENCE.md`
- **Examples**: Check `easy_usage.md`, `HOW_TO_USE.md`
- **GitHub**: Report issues at `https://github.com/nlm-project/nlm`
- **Community**: Join the NLM discussion forum

This advanced usage guide provides comprehensive tools for power users to:
- Create complex, custom configurations
- Run batch experiments and analyze results
- Monitor and debug system performance
- Build custom agents with specialized behavior
- Optimize for high-performance computing
- Handle advanced troubleshooting scenarios

All features are designed to be accessible while providing deep control for advanced use cases. The modular architecture ensures you can customize any aspect of the NLM system to suit your specific research or application needs.