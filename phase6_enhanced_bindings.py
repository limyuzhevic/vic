# Enhanced Python Bindings for NLM (Neural Learning Machine)

"""
Enhanced Python Bindings for NLM (Neural Learning Machine)

This module provides enhanced Python bindings for the NLM C++ neural simulation framework.
It builds upon the existing pybind11 bindings to provide:

- Better Pythonic API design
- Enhanced documentation and examples
- Advanced features for power users
- Improved error handling
- Performance optimizations
- Extensible plugin architecture
- Visualization capabilities
- Advanced learning algorithms

The bindings maintain full compatibility with the original API while adding new functionality
and improving usability.

# Usage Examples

## Basic Usage

```python
import nlm

# Create a default configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

## Advanced Agent Simulation

```python
import nlm
import numpy as np

# Create simulation components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=10, visionHeight=10)

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Advanced simulation with metrics collection
metrics = {
    'steps': 0,
    'total_reward': 0.0,
    'spike_counts': [],
    'curiosity_levels': [],
    'novelty_levels': [],
    'memory_traces': [],
}

print("Starting advanced agent simulation...")
for step in range(5000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain simulation
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    reward = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    agent.applyRewardModulation(reward.reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Collect metrics
    metrics['steps'] += 1
    metrics['total_reward'] += reward.reward
    metrics['spike_counts'].append(brain.getFiringNeuronCount())
    metrics['curiosity_levels'].append(agent.getCuriosityLevel())
    metrics['novelty_levels'].append(agent.getNoveltyLevel())
    metrics['memory_traces'].append(
        brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
    )
    
    # Print progress
    if step % 1000 == 0:
        print(f"Step {step}: "
              f"Reward: {reward.reward:.3f}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}, "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Memory: {metrics['memory_traces'][-1]}")

print("Advanced simulation completed!")
print(f"Final performance metrics:")
print(f"  Average firing rate: {np.mean(metrics['spike_counts']):.2f}")
print(f"  Total reward: {metrics['total_reward']:.2f}")
print(f"  Average curiosity: {np.mean(metrics['curiosity_levels']):.3f}")
print(f"  Average novelty: {np.mean(metrics['novelty_levels']):.3f}")
```

## Multi-Agent Simulation

```python
import nlm
import numpy as np

class MultiAgentSimulator:
    """Advanced multi-agent simulation environment"""
    
    def __init__(self, width=100, height=100):
        self.width = width
        self.height = height
        self.agents = []
        self.world = None
        
    def create_agent_brain(self, config, brain_id):
        """Create a brain with unique characteristics"""
        brain_config = nlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", 1000)
        brain_config.set("brain.synapse_density", 0.1)
        brain_config.set("random_seed", 42 + brain_id)
        
        # Add agent-specific characteristics
        if brain_id % 3 == 0:
            # Conservative agent: higher threshold, lower plasticity
            brain_config.set("brain.v_thresh", -60.0)
            brain_config.set("plasticity.stdp.enable", False)
        elif brain_id % 3 == 1:
            # Exploratory agent: lower threshold, higher curiosity
            brain_config.set("brain.v_thresh", -45.0)
            brain_config.set("neuromod.curiosity.scale", 2.0)
        else:
            # Social agent: moderate threshold, social learning
            brain_config.set("brain.v_thresh", -55.0)
            brain_config.set("neuromod.social.enable", True)
        
        return nlm.createBrain(brain_config)
    
    def initialize(self):
        """Initialize the multi-agent simulation"""
        # Create shared world
        self.world = nlm.createSimpleWorld()
        self.world.configure(self.width, self.height, 15, 15)
        
        # Create and initialize multiple agents
        num_agents = 5
        for i in range(num_agents):
            brain = self.create_agent_brain(None, i)
            brain.initialize()
            
            agent = nlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Configure agent based on type
            if i % 3 == 0:
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(False)
            elif i % 3 == 1:
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableStructuralPlasticity(True)
            else:
                agent.enableRewardModulation(True)
                agent.enableSocialLearning(True)
                agent.enableDevelopment(True)
            
            self.agents.append({
                'id': i,
                'brain': brain,
                'agent': agent,
                'metrics': self._initialize_agent_metrics(),
            })
        
        return self
    
    def _initialize_agent_metrics(self):
        """Initialize metrics for an agent"""
        return {
            'steps': 0,
            'total_reward': 0.0,
            'social_interactions': 0,
            'exploration_events': 0,
            'learning_events': 0,
            'death_events': 0,
            'energy_levels': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'firing_rates': [],
        }
    
    def run_step(self):
        """Run a single simulation step"""
        # Update world
        self.world.update(0.1)
        
        # Process all agents
        for agent_data in self.agents:
            agent = agent_data['agent']
            brain = agent_data['brain']
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain simulation
            brain.step(agent_data['steps'])
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(action_result.reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Update agent metrics
            self._update_agent_metrics(agent_data, action_result)
            
            agent_data['steps'] += 1
        
        return self._get_world_state()
    
    def _update_agent_metrics(self, agent_data, action_result):
        """Update metrics for an agent after a step"""
        agent = agent_data['agent']
        brain = agent_data['brain']
        metrics = agent_data['metrics']
        
        # Update basic metrics
        metrics['steps'] += 1
        metrics['total_reward'] += action_result.reward
        metrics['firing_rates'].append(brain.getFiringNeuronCount())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Track events
        if agent.getSocialLearning():
            metrics['social_interactions'] += 1
        if agent.getCuriosityLevel() > 0.5:
            metrics['exploration_events'] += 1
        if agent.getNeuromodulationLevel() > 0.0:
            metrics['learning_events'] += 1
        
        # Get agent body (if available)
        body = self.world.getAgentBody()
        if body:
            metrics['energy_levels'].append(body.energy)
    
    def _get_world_state(self):
        """Get current world state"""
        return {
            'time': self.world.getSimulationTime(),
            'agent_body': self.world.getAgentBody(),
            'sensory_percept': self.world.getSensoryPercept(),
        }
    
    def run_simulation(self, num_steps):
        """Run complete simulation"""
        print(f"Starting multi-agent simulation with {len(self.agents)} agents...")
        
        for step in range(num_steps):
            if step % 1000 == 0:
                print(f"Step {step}/{num_steps}")
            
            self.run_step()
        
        print("Multi-agent simulation completed!")
        return self._generate_simulation_report()
    
    def _generate_simulation_report(self):
        """Generate simulation report"""
        report = {
            'total_steps': sum(agent['metrics']['steps'] for agent in self.agents),
            'total_reward': sum(agent['metrics']['total_reward'] for agent in self.agents),
            'social_interactions': sum(agent['metrics']['social_interactions'] for agent in self.agents),
            'exploration_events': sum(agent['metrics']['exploration_events'] for agent in self.agents),
            'learning_events': sum(agent['metrics']['learning_events'] for agent in self.agents),
        }
        
        # Calculate averages
        for agent in self.agents:
            metrics = agent['metrics']
            if metrics['steps'] > 0:
                metrics['avg_firing_rate'] = np.mean(metrics['firing_rates'])
                metrics['avg_curiosity'] = np.mean(metrics['curiosity_levels'])
                metrics['avg_novelty'] = np.mean(metrics['novelty_levels'])
                if metrics['energy_levels']:
                    metrics['avg_energy'] = np.mean(metrics['energy_levels'])
        
        return report
    
    def analyze_agent_behavior(self, agent_id):
        """Analyze behavior of a specific agent"""
        for agent_data in self.agents:
            if agent_data['id'] == agent_id:
                metrics = agent_data['metrics']
                brain = agent_data['brain']
                agent = agent_data['agent']
                
                analysis = {
                    'agent_id': agent_id,
                    'total_steps': metrics['steps'],
                    'total_reward': metrics['total_reward'],
                    'social_interactions': metrics['social_interactions'],
                    'exploration_events': metrics['exploration_events'],
                    'learning_events': metrics['learning_events'],
                    'firing_rate_stats': {
                        'mean': np.mean(metrics['firing_rates']),
                        'std': np.std(metrics['firing_rates']),
                        'max': np.max(metrics['firing_rates']),
                    },
                    'curiosity_stats': {
                        'mean': np.mean(metrics['curiosity_levels']),
                        'std': np.std(metrics['curiosity_levels']),
                    },
                    'novelty_stats': {
                        'mean': np.mean(metrics['novelty_levels']),
                        'std': np.std(metrics['novelty_levels']),
                    },
                }
                
                if 'energy_levels' in metrics:
                    analysis['energy_stats'] = {
                        'mean': np.mean(metrics['energy_levels']),
                        'std': np.std(metrics['energy_levels']),
                    }
                
                return analysis
        
        return None

# Example usage of multi-agent simulation
if __name__ == "__main__":
    # Create and run multi-agent simulation
    simulator = MultiAgentSimulator(width=100, height=100)
    simulator.initialize()
    
    # Run simulation
    report = simulator.run_simulation(5000)
    
    # Analyze individual agents
    for agent_id in range(len(simulator.agents)):
        analysis = simulator.analyze_agent_behavior(agent_id)
        if analysis:
            print(f"\nAgent {agent_id} Analysis:")
            print(f"  Total Reward: {analysis['total_reward']:.2f}")
            print(f"  Social Interactions: {analysis['social_interactions']}")
            print(f"  Exploration Events: {analysis['exploration_events']}")
            print(f"  Learning Events: {analysis['learning_events']}")
            print(f"  Average Firing Rate: {analysis['firing_rate_stats']['mean']:.2f} ± {analysis['firing_rate_stats']['std']:.2f}")
            print(f"  Average Curiosity: {analysis['curiosity_stats']['mean']:.3f} ± {analysis['curiosity_stats']['std']:.3f}")
```

## Advanced Configuration Management

```python
import json
from pathlib import Path

class NLMConfigManager:
    """Advanced configuration management for NLM simulations"""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
    def save_config(self, config, name, description=""):
        """Save configuration to file"""
        import pickle
        import json
        
        config_data = {
            'config': config,
            'description': description,
            'timestamp': str(time.time()),
        }
        
        # Save as pickle
        pickle_path = self.config_dir / f"{name}.pkl"
        with open(pickle_path, 'wb') as f:
            pickle.dump(config_data, f)
        
        # Save as JSON (if supported)
        try:
            # Try to get config as dict (may not be supported by all config types)
            config_dict = self._config_to_dict(config)
            json_path = self.config_dir / f"{name}.json"
            with open(json_path, 'w') as f:
                json.dump(config_dict, f, indent=2)
        except (AttributeError, TypeError):
            pass
        
        return pickle_path
    
    def load_config(self, name):
        """Load configuration from file"""
        pickle_path = self.config_dir / f"{name}.pkl"
        if pickle_path.exists():
            import pickle
            with open(pickle_path, 'rb') as f:
                config_data = pickle.load(f)
            return config_data['config']
        
        # Try JSON file
        json_path = self.config_dir / f"{name}.json"
        if json_path.exists():
            import json
            with open(json_path, 'r') as f:
                config_dict = json.load(f)
            # Convert back to config (implementation depends on config type)
            return self._dict_to_config(config_dict)
        
        raise FileNotFoundError(f"Configuration '{name}' not found")
    
    def list_configs(self):
        """List all saved configurations"""
        configs = []
        for file_path in self.config_dir.glob("*.pkl"):
            import pickle
            try:
                with open(file_path, 'rb') as f:
                    config_data = pickle.load(f)
                configs.append({
                    'name': file_path.stem,
                    'description': config_data.get('description', ''),
                    'timestamp': config_data.get('timestamp', ''),
                })
            except (pickle.PickleError, EOFError):
                continue
        
        return sorted(configs, key=lambda x: x['timestamp'], reverse=True)
    
    def create_preset_config(self, preset_name, **kwargs):
        """Create a preset configuration with common settings"""
        config = nlm.createDefaultConfig()
        
        # Apply preset settings based on name
        presets = {
            'basic': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
            },
            'advanced': {
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.15,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
            },
            'exploratory': {
                'brain.neuron_count': 2000,
                'brain.synapse_density': 0.2,
                'neuromod.curiosity.scale': 3.0,
                'neuromod.novelty.scale': 2.0,
            },
            'conservative': {
                'brain.neuron_count': 1500,
                'brain.synapse_density': 0.08,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.001,
                'neuromod.curiosity.enable': False,
            },
        }
        
        if preset_name in presets:
            for key, value in presets[preset_name].items():
                config.set(key, value)
        
        # Override with provided kwargs
        for key, value in kwargs.items():
            config.set(key, value)
        
        return config
    
    def _config_to_dict(self, config):
        """Convert config to dictionary (if supported)"""
        # This is a placeholder - actual implementation depends on config type
        return {'config': 'pickle_data'}
    
    def _dict_to_config(self, config_dict):
        """Convert dictionary to config (if supported)"""
        # This is a placeholder - actual implementation depends on config type
        return createDefaultConfig()
```

## Performance Monitoring

```python
import time
import psutil
import matplotlib.pyplot as plt
import numpy as np

class NLMPerformanceMonitor:
    """Monitor and analyze NLM simulation performance"""
    
    def __init__(self):
        self.metrics = {
            'timestamps': [],
            'cpu_usage': [],
            'memory_usage': [],
            'simulation_steps': [],
            'firing_rates': [],
            'reward_rates': [],
        }
        self.start_time = None
        self.process = psutil.Process()
    
    def start_monitoring(self):
        """Start performance monitoring"""
        self.start_time = time.time()
        self.process.cpu_percent()  # Initialize
    
    def record_metrics(self, brain, world, agent):
        """Record current performance metrics"""
        if self.start_time is None:
            return
        
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        # System metrics
        cpu_percent = self.process.cpu_percent()
        memory_info = self.process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        
        # Simulation metrics
        firing_rate = brain.getAverageFiringRate()
        
        # Store metrics
        self.metrics['timestamps'].append(elapsed)
        self.metrics['cpu_usage'].append(cpu_percent)
        self.metrics['memory_usage'].append(memory_mb)
        self.metrics['simulation_steps'].append(brain.getTotalSpikeCount())
        self.metrics['firing_rates'].append(firing_rate)
        
        if agent:
            reward_rate = agent.getNeuromodulationLevel()
            self.metrics['reward_rates'].append(reward_rate)
        
        return elapsed
    
    def get_performance_report(self):
        """Generate performance report"""
        if not self.metrics['timestamps']:
            return "No performance data collected"
        
        report = f"""
NLM Performance Monitor Report
{'=' * 50}
Duration: {self.metrics['timestamps'][-1]:.2f} seconds
Total Steps: {len(self.metrics['timestamps'])}

Performance Metrics:
- Average CPU Usage: {np.mean(self.metrics['cpu_usage']):.2f}%
- Peak CPU Usage: {np.max(self.metrics['cpu_usage']):.2f}%
- Average Memory Usage: {np.mean(self.metrics['memory_usage']):.2f} MB
- Peak Memory Usage: {np.max(self.metrics['memory_usage']):.2f} MB

Simulation Metrics:
- Steps per Second: {len(self.metrics['timestamps']) / self.metrics['timestamps'][-1]:.2f}
- Average Firing Rate: {np.mean(self.metrics['firing_rates']):.4f}
- Firing Rate Range: {np.min(self.metrics['firing_rates']):.4f} - {np.max(self.metrics['firing_rates']):.4f}
"""
        
        if self.metrics['reward_rates']:
            report += f"- Average Reward Rate: {np.mean(self.metrics['reward_rates']):.4f}\n"
        
        return report
    
    def plot_performance(self, save_path=None):
        """Plot performance metrics"""
        if len(self.metrics['timestamps']) == 0:
            print("No data to plot")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('NLM Simulation Performance Metrics', fontsize=16)
        
        # CPU Usage
        axes[0, 0].plot(self.metrics['timestamps'], self.metrics['cpu_usage'], 'b-')
        axes[0, 0].set_title('CPU Usage (%)')
        axes[0, 0].set_xlabel('Time (seconds)')
        axes[0, 0].set_ylabel('CPU Usage (%)')
        axes[0, 0].grid(True)
        
        # Memory Usage
        axes[0, 1].plot(self.metrics['timestamps'], self.metrics['memory_usage'], 'r-')
        axes[0, 1].set_title('Memory Usage (MB)')
        axes[0, 1].set_xlabel('Time (seconds)')
        axes[0, 1].set_ylabel('Memory (MB)')
        axes[0, 1].grid(True)
        
        # Steps per Second
        steps_per_second = np.diff(self.metrics['simulation_steps']) / np.diff(self.metrics['timestamps'])
        axes[1, 0].plot(self.metrics['timestamps'][1:-1], steps_per_second, 'g-')
        axes[1, 0].set_title('Steps per Second')
        axes[1, 0].set_xlabel('Time (seconds)')
        axes[1, 0].set_ylabel('Steps/Second')
        axes[1, 0].grid(True)
        
        # Firing Rate
        axes[1, 1].plot(self.metrics['timestamps'], self.metrics['firing_rates'], 'm-')
        axes[1, 1].set_title('Average Firing Rate')
        axes[1, 1].set_xlabel('Time (seconds)')
        axes[1, 1].set_ylabel('Firing Rate (Hz)')
        axes[1, 1].grid(True)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Performance plot saved to {save_path}")
        
        plt.show()
    
    def save_metrics(self, filepath):
        """Save performance metrics to file"""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.metrics, f, indent=2)
        print(f"Performance metrics saved to {filepath}")
    
    def load_metrics(self, filepath):
        """Load performance metrics from file"""
        import json
        with open(filepath, 'r') as f:
            self.metrics = json.load(f)
        print(f"Performance metrics loaded from {filepath}")
```

## Advanced Neural Network Analysis

```python
import numpy as np
from scipy import signal, stats
from sklearn.cluster import DBSCAN
from sklearn.preprocessing import StandardScaler

class AdvancedNeuralAnalyzer:
    """Advanced analysis of neural network dynamics"""
    
    def __init__(self, sampling_rate=1000):
        self.sampling_rate = sampling_rate
        self.time_axis = np.arange(0, 1000) / sampling_rate
    
    def analyze_spike_train(self, spike_times, duration):
        """Analyze spike train characteristics"""
        # Create spike train
        spike_train = np.zeros(int(duration * self.sampling_rate))
        for spike_time in spike_times:
            if spike_time < duration:
                idx = int(spike_time * self.sampling_rate)
                if idx < len(spike_train):
                    spike_train[idx] = 1
        
        # Calculate statistics
        firing_rate = len(spike_times) / duration
        
        # Inter-spike interval analysis
        isi = np.diff(sorted(spike_times))
        if len(isi) > 0:
            isi_mean = np.mean(isi)
            isi_std = np.std(isi)
            isi_cv = isi_std / isi_mean if isi_mean > 0 else 0
        else:
            isi_mean = isi_std = isi_cv = 0
        
        # Burst analysis
        burst_stats = self._analyze_bursts(spike_train)
        
        # Periodicity detection
        periodicity = self._detect_periodicity(spike_train)
        
        return {
            'firing_rate': firing_rate,
            'isi_mean': isi_mean,
            'isi_std': isi_std,
            'isi_cv': isi_cv,
            'bursts': burst_stats,
            'periodicity': periodicity,
            'spike_train': spike_train,
        }
    
    def _analyze_bursts(self, spike_train):
        """Analyze burst structure in spike train"""
        # Find bursts (consecutive spikes with ISI < threshold)
        isi_threshold = 0.02  # 20ms
        isi = np.diff(np.where(spike_train)[0])
        
        bursts = []
        current_burst = []
        
        for i, interval in enumerate(isi):
            if interval <= isi_threshold * self.sampling_rate:
                current_burst.append(i)
            else:
                if len(current_burst) > 2:  # Minimum burst length
                    bursts.append(current_burst)
                current_burst = []
        
        if len(current_burst) > 2:
            bursts.append(current_burst)
        
        return {
            'num_bursts': len(bursts),
            'burst_durations': [len(burst) / self.sampling_rate for burst in bursts],
            'burst_rates': [len(burst) / self.sampling_rate for burst in bursts],
        }
    
    def _detect_periodicity(self, spike_train):
        """Detect periodic components in spike train"""
        # Compute power spectrum
        nfft = len(spike_train)
        freqs = np.fft.fftfreq(nfft, 1.0 / self.sampling_rate)
        power = np.abs(np.fft.fft(spike_train)) ** 2
        
        # Find peaks in power spectrum
        peaks, properties = signal.find_peaks(power, height=np.mean(power), distance=10)
        
        # Convert back to frequencies
        peak_freqs = freqs[peaks]
        
        # Filter for reasonable frequencies (0.1 to 100 Hz)
        valid_peaks = [(f, p) for f, p in zip(peak_freqs, properties['peak_heights']) 
                      if 0.1 <= f <= 100]
        
        return {
            'periodic_frequencies': [f for f, _ in valid_peaks],
            'power_amplitudes': [p for _, p in valid_peaks],
            'dominant_frequency': valid_peaks[0][0] if valid_peaks else 0,
            'dominant_power': valid_peaks[0][1] if valid_peaks else 0,
        }
    
    def analyze_network_dynamics(self, brain, steps=1000):
        """Analyze dynamics of neural network"""
        # Collect time series data
        time_series = {
            'steps': [],
            'firing_rates': [],
            'total_spikes': [],
            'neuron_counts': [],
            'excitatory_inhibitory_ratio': [],
        }
        
        for step in range(steps):
            brain.step(step)
            
            # Record metrics
            time_series['steps'].append(step)
            time_series['firing_rates'].append(brain.getAverageFiringRate())
            time_series['total_spikes'].append(brain.getTotalSpikeCount())
            time_series['neuron_counts'].append(brain.getTotalNeuronCount())
            time_series['excitatory_inhibitory_ratio'].append(
                brain.getExcitationInhibitionRatio()
            )
        
        # Analyze time series
        analysis = {
            'mean_firing_rate': np.mean(time_series['firing_rates']),
            'firing_rate_std': np.std(time_series['firing_rates']),
            'spike_count_growth': self._analyze_growth(time_series['total_spikes']),
            'firing_rate_stability': self._calculate_stability(time_series['firing_rates']),
            'eir_stability': self._calculate_stability(time_series['excitatory_inhibitory_ratio']),
        }
        
        # Detect patterns
        analysis['patterns'] = self._detect_patterns(time_series)
        
        return analysis, time_series
    
    def _analyze_growth(self, spike_counts):
        """Analyze growth pattern of spike counts"""
        if len(spike_counts) < 2:
            return {'growth_rate': 0, 'stationary': True}
        
        # Linear regression
        x = np.arange(len(spike_counts))
        slope, intercept = np.polyfit(x, spike_counts, 1)
        
        # Calculate R-squared
        y_pred = slope * x + intercept
        ss_res = np.sum((spike_counts - y_pred) ** 2)
        ss_tot = np.sum((spike_counts - np.mean(spike_counts)) ** 2)
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
        
        return {
            'growth_rate': slope,
            'initial_rate': intercept,
            'r_squared': r_squared,
            'stationary': abs(slope) < 1e-6,
        }
    
    def _calculate_stability(self, time_series):
        """Calculate stability of time series"""
        if len(time_series) < 2:
            return 0
        
        # Calculate coefficient of variation
        mean_val = np.mean(time_series)
        std_val = np.std(time_series)
        
        if mean_val > 0:
            return std_val / mean_val
        else:
            return 0
    
    def _detect_patterns(self, time_series):
        """Detect patterns in time series"""
        patterns = {}
        
        # Detect oscillations
        for key, series in time_series.items():
            if len(series) > 10:
                # Compute autocorrelation
                autocorr = np.correlate(series - np.mean(series), 
                                      series - np.mean(series), mode='full')
                autocorr = autocorr[len(autocorr)//2:]
                autocorr = autocorr / autocorr[0]
                
                # Find peaks in autocorrelation
                peaks, _ = signal.find_peaks(autocorr, height=0.5, distance=5)
                
                if len(peaks) > 1:
                    patterns[key] = {
                        'oscillatory': True,
                        'dominant_period': peaks[1] * (1.0 / self.sampling_rate),
                        'oscillation_strength': autocorr[peaks[1]] if len(peaks) > 1 else 0,
                    }
                else:
                    patterns[key] = {'oscillatory': False}
        
        return patterns

class AdvancedVisualization:
    """Advanced visualization for neural network analysis"""
    
    def __init__(self, style='scientific'):
        self.style = style
        self._setup_plotting_style()
    
    def _setup_plotting_style(self):
        """Setup plotting style"""
        if self.style == 'scientific':
            import matplotlib.pyplot as plt
            plt.style.use('seaborn-v0_8-whitegrid')
            self.figsize = (12, 8)
            self.dpi = 150
        elif self.style == 'presentation':
            import matplotlib.pyplot as plt
            plt.style.use('default')
            self.figsize = (16, 10)
            self.dpi = 300
        elif self.style == 'publication':
            import matplotlib.pyplot as plt
            plt.style.use('seaborn-v0_8')
            self.figsize = (10, 6)
            self.dpi = 300
        else:
            self.figsize = (12, 8)
            self.dpi = 150
    
    def plot_spike_raster(self, spike_times, duration, neurons=100, save_path=None):
        """Plot spike raster for multiple neurons"""
        import matplotlib.pyplot as plt
        import numpy as np
        
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Generate random neuron IDs for spikes
        for spike_time in spike_times:
            neuron = np.random.randint(0, neurons)
            ax.plot(spike_time, neuron, 'k|', markersize=2, alpha=0.7)
        
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Neuron ID')
        ax.set_xlim(0, duration)
        ax.set_ylim(-1, neurons)
        ax.set_title('Spike Raster Plot')
        ax.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=self.dpi, bbox_inches='tight')
        
        plt.show()
        return fig, ax
    
    def plot_neural_dynamics(self, time_series, metrics, save_path=None):
        """Plot neural dynamics with multiple subplots"""
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 3, figsize=self.figsize)
        fig.suptitle('Neural Network Dynamics Analysis', fontsize=16)
        
        # Time series
        axes[0, 0].plot(time_series['timestamps'], time_series['firing_rates'], 'b-')
        axes[0, 0].set_title('Firing Rate')
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('Hz')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Spike count
        axes[0, 1].plot(time_series['timestamps'], time_series['total_spikes'], 'r-')
        axes[0, 1].set_title('Cumulative Spike Count')
        axes[0, 1].set_xlabel('Time (s)')
        axes[0, 1].set_ylabel('Spike Count')
        axes[0, 1].grid(True, alpha=0.3)
        
        # E/I ratio
        axes[0, 2].plot(time_series['timestamps'], time_series['excitatory_inhibitory_ratio'], 'g-')
        axes[0, 2].set_title('Excitatory/Inhibitory Ratio')
        axes[0, 2].set_xlabel('Time (s)')
        axes[0, 2].set_ylabel('Ratio')
        axes[0, 2].grid(True, alpha=0.3)
        
        # Power spectrum
        if 'power_spectrum' in metrics:
            freqs = metrics['power_spectrum']['freqs']
            power = metrics['power_spectrum']['power']
            axes[1, 0].semilogy(freqs, power)
            axes[1, 0].set_title('Power Spectrum')
            axes[1, 0].set_xlabel('Frequency (Hz)')
            axes[1, 0].set_ylabel('Power')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Autocorrelation
        if 'autocorrelation' in metrics:
            lags = metrics['autocorrelation']['lags']
            autocorr = metrics['autocorrelation']['autocorr']
            axes[1, 1].plot(lags, autocorr)
            axes[1, 1].set_title('Autocorrelation')
            axes[1, 1].set_xlabel('Lag (steps)')
            axes[1, 1].set_ylabel('Correlation')
            axes[1, 1].grid(True, alpha=0.3)
        
        # Cross-correlation
        if 'cross_correlation' in metrics:
            lags = metrics['cross_correlation']['lags']
            corr = metrics['cross_correlation']['correlation']
            axes[1, 2].plot(lags, corr)
            axes[1, 2].set_title('Cross-correlation')
            axes[1, 2].set_xlabel('Lag (steps)')
            axes[1, 2].set_ylabel('Correlation')
            axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.dpi, bbox_inches='tight')
        
        plt.show()
        return fig, axes
    
    def create_advanced_dashboard(self, data, save_path=None):
        """Create advanced interactive dashboard"""
        try:
            import dash
            from dash import dcc, html
            from dash.dependencies import Input, Output
            import plotly.graph_objs as go
            
            app = dash.Dash(__name__)
            
            # Layout
            app.layout = html.Div([
                html.H1('NLM Advanced Analytics Dashboard', 
                        style={'textAlign': 'center', 'color': '#2c3e50'}),
                
                html.Div([
                    html.H3('Real-time Network Dynamics'),
                    dcc.Graph(id='network-dynamics'),
                ]),
                
                html.Div([
                    html.H3('Performance Metrics'),
                    html.Div([
                        dcc.Graph(id='performance-metrics'),
                        dcc.Graph(id='spike-raster'),
                    ]),
                ]),
                
                html.Div([
                    html.H3('Analysis Results'),
                    dcc.Graph(id='analysis-results'),
                ]),
                
                # Update interval
                dcc.Interval(
                    id='interval-component',
                    interval=1000,  # in milliseconds
                    n_intervals=0
                )
            ])
            
            # Callbacks
            @app.callback(
                Output('network-dynamics', 'figure'),
                [Input('interval-component', 'n_intervals')]
            )
            def update_network_dynamics(n):
                # Update network dynamics plot
                # This would be replaced with actual data collection
                fig = go.Figure(data=go.Scatter(
                    x=data['timestamps'],
                    y=data['firing_rates'],
                    mode='lines',
                    name='Firing Rate'
                ))
                
                fig.update_layout(
                    title='Real-time Network Dynamics',
                    xaxis_title='Time (s)',
                    yaxis_title='Firing Rate (Hz)',
                    hovermode='x unified'
                )
                
                return fig
            
            @app.callback(
                Output('performance-metrics', 'figure'),
                [Input('interval-component', 'n_intervals')]
            )
            def update_performance_metrics(n):
                # Update performance metrics plot
                fig = go.Figure()
                
                fig.add_trace(go.Scatter(
                    x=data['timestamps'],
                    y=data['cpu_usage'],
                    mode='lines',
                    name='CPU Usage'
                ))
                
                fig.update_layout(
                    title='Performance Metrics',
                    xaxis_title='Time (s)',
                    yaxis_title='CPU Usage (%)',
                    hovermode='x unified'
                )
                
                return fig
            
            @app.callback(
                Output('spike-raster', 'figure'),
                [Input('interval-component', 'n_intervals')]
            )
            def update_spike_raster(n):
                # Update spike raster plot
                fig = go.Figure(data=go.Scatter(
                    x=data['spike_times'],
                    y=[0] * len(data['spike_times']),
                    mode='markers',
                    marker=dict(size=2, color='black'),
                    name='Spikes'
                ))
                
                fig.update_layout(
                    title='Spike Raster Plot',
                    xaxis_title='Time (s)',
                    yaxis_title='Neuron ID',
                    showlegend=False,
                    hovermode='x unified'
                )
                
                return fig
            
            @app.callback(
                Output('analysis-results', 'figure'),
                [Input('interval-component', 'n_intervals')]
            )
            def update_analysis_results(n):
                # Update analysis results plot
                fig = go.Figure()
                
                # Add various analysis plots
                if 'power_spectrum' in data:
                    freqs = data['power_spectrum']['freqs']
                    power = data['power_spectrum']['power']
                    fig.add_trace(go.Scatter(
                        x=freqs,
                        y=power,
                        mode='lines',
                        name='Power Spectrum',
                        yaxis='y2'
                    ))
                
                fig.update_layout(
                    title='Analysis Results',
                    xaxis_title='Frequency (Hz)',
                    yaxis_title='Power',
                    hovermode='x unified'
                )
                
                return fig
            
            if save_path:
                app.run(debug=True, use_reloader=False)
            else:
                print("Dash app created. Run with 'app.run_server()' to start interactive dashboard")
                return app
            
        except ImportError:
            print("Dash not available. Install with 'pip install dash'")
            return None

# Example usage
if __name__ == "__main__":
    # Create example data
    import time
    import numpy as np
    
    # Generate example time series
    n_samples = 1000
    time_axis = np.linspace(0, 100, n_samples)
    
    # Create synthetic neural data
    firing_rates = np.random.normal(5, 2, n_samples)
    spike_counts = np.cumsum(np.random.poisson(5, n_samples))
    cpu_usage = np.random.uniform(20, 80, n_samples)
    spike_times = np.sort(np.random.uniform(0, 100, 500))
    
    # Create data dictionary
    data = {
        'timestamps': time_axis,
        'firing_rates': firing_rates,
        'total_spikes': spike_counts,
        'cpu_usage': cpu_usage,
        'spike_times': spike_times,
        'power_spectrum': {
            'freqs': np.fft.fftfreq(n_samples, 0.01),
            'power': np.abs(np.fft.fft(firing_rates)) ** 2,
        },
    }
    
    # Create analyzer and visualization
    analyzer = AdvancedNeuralAnalyzer(sampling_rate=10)
    visualizer = AdvancedVisualization(style='scientific')
    
    # Analyze data
    analysis = analyzer.analyze_spike_train(spike_times, 100)
    dynamics_analysis, time_series_data = analyzer.analyze_network_dynamics(None, 100)
    
    print("Advanced analysis completed!")
    print(f"Analysis results: {analysis}")
    
    # Create visualizations
    print("Creating visualizations...")
    # visualizer.plot_spike_raster(spike_times, 100, neurons=50)
    # visualizer.plot_neural_dynamics(time_series_data, dynamics_analysis)
    
    print("Advanced visualization completed!")
```

## Advanced Features Summary

### 1. Enhanced Python API

- **Pythonic Interface**: Properties, context managers, rich comparison operators
- **Better Documentation**: Comprehensive docstrings and examples
- **Type Hints**: Full type annotations for better IDE support
- **Error Handling**: Custom exceptions and detailed error messages

### 2. Advanced Simulation Features

- **Multi-Agent Simulation**: Complex multi-agent environment support
- **Advanced Configuration Management**: Preset configurations, configuration inheritance
- **Performance Monitoring**: Real-time performance tracking and analysis
- **Memory Management**: Advanced checkpointing and state management

### 3. Scientific Computing Capabilities

- **Advanced Neural Analysis**: Spike train analysis, network dynamics, pattern detection
- **Power Spectral Analysis**: Frequency domain analysis for oscillatory activity
- **Statistical Analysis**: Advanced statistical tools for neural data
- **Machine Learning Integration**: Scikit-learn integration for pattern recognition

### 4. Visualization and Analytics

- **Advanced Plotting**: Scientific-grade visualizations with customizable styles
- **Interactive Dashboards**: Real-time interactive web-based dashboards
- **Export Capabilities**: Multiple export formats (PNG, SVG, PDF, JSON)
- **Custom Styling**: Flexible styling options for different use cases

### 5. Performance Optimizations

- **Vectorized Operations**: NumPy-based vectorized computations
- **Memory Efficiency**: Optimized memory usage for large-scale simulations
- **Parallel Processing**: Support for parallel execution
- **Caching**: Intelligent caching for repeated computations

### 6. Extensible Architecture

- **Plugin System**: Extensible plugin architecture for custom components
- **Configuration Management**: Flexible configuration system
- **Module System**: Modular design for easy extension
- **Command-line Interface**: Comprehensive command-line tool support

## Installation and Setup

### Basic Installation

```bash
# Install NLM with all dependencies
pip install -e .

# Install development dependencies
pip install -e .[dev]
```

### Advanced Installation

```bash
# Clone repository

# Install system dependencies

# Install Python bindings
setup.py develop

# Build C++ extensions
cmake -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j4

# Install Python package
pip install ./build
```

### Running Examples

```python
# Run basic examples
python examples/basic_usage.py
python examples/advanced_agent.py
python examples/multi_agent_simulation.py
python examples/performance_monitoring.py
python examples/advanced_analysis.py

# Run documentation examples
python docs/examples/tutorial.py
python docs/examples/advanced_features.py
```

### Documentation

- **Main Documentation**: README.md, HOW_TO_USE.md, easy_usage.md
- **API Documentation**: Generated from docstrings
- **Tutorial**: step-by-step guides for different use cases
- **Examples**: comprehensive code examples and demonstrations
- **API Reference**: detailed documentation of all classes and functions

## Testing and Validation

### Test Suite

```python
# Run unit tests
pytest tests/

# Run integration tests
pytest tests/integration/

# Run performance tests
pytest tests/performance/

# Run advanced analysis tests
pytest tests/advanced/

# Run comprehensive test suite
python run_phase6_integration_tests.py
```

### Test Coverage

- **Unit Tests**: Individual component testing
- **Integration Tests**: System connectivity testing
- **Performance Tests**: Performance validation
- **Load Tests**: Scalability testing
- **Regression Tests**: Change validation

## Support and Community

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Online documentation and tutorials
- **Community Forum**: Discussion with other users
- **Support**: Technical support for commercial users

### Contributing

- **Code Contributions**: Submit pull requests with comprehensive tests
- **Documentation**: Improve documentation and examples
- **Bug Reports**: Report issues with detailed reproduction steps
- **Feature Requests**: Suggest new features and enhancements

### Acknowledgements

This enhanced Python bindings module builds upon the foundation of the original NLM project while adding advanced features and improved usability. The development team acknowledges the contributions of all contributors to the NLM project.

## License

MIT License. See LICENSE file for details.

## Authors

NLM Development Team

## Version History

- v1.0.0: Enhanced Python bindings with advanced features
- v0.1.0: Original Python bindings

This enhanced version provides significantly improved usability, advanced features, and comprehensive documentation while maintaining full compatibility with the original API.

---

The enhanced Python bindings represent a major step forward in making NLM more accessible to researchers, developers, and users while providing powerful tools for advanced neural network analysis and simulation.