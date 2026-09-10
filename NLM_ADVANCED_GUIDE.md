# NLM - Advanced User Guide

This guide provides advanced usage instructions, command-line interface, and detailed API documentation for power users.

---

# 1. Command Line Interface

## Installation

### Build from Source
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Python Bindings
```bash
# Install the Python package
pip install -e .
```

## CLI Commands

The NLM brain comes with an advanced command-line interface for controlling simulations:

### nlm-cli - The main control interface

```bash
# Show all available commands
nlm-cli --help

# Run a simulation with configuration
nlm-cli run --config configs/demo.cfg --steps 1000

# Interactive mode
nlm-cli interactive

# Export brain state
nlm-cli export --file brain_state.bin --steps 500

# Import brain state
nlm-cli import --file saved_brain.bin --steps 100

# Run advanced experiment
nlm-cli experiment --type phase6 --neurons 2000 --steps 5000

# Monitor running simulation
nlm-cli monitor --pid 12345

# Configure brain parameters
nlm-cli config set neuron_count 1000
nlm-cli config get neuron_count

# Check system status
nlm-cli status

# Run benchmark
nlm-cli benchmark --test stdp --iterations 100

# Visualize simulation
nlm-cli visualize --format svg --steps 100
```

### Configuration Management

```bash
# Show all configuration options
nlm-cli config list

# Get specific config value
nlm-cli config get [key] [--source [file|command|default]]

# Set configuration value
nlm-cli config set [key] [value] [--source [file|command|default]]

# Reset configuration to defaults
nlm-cli config reset [key]

# Export configuration
nlm-cli config export --file brain.cfg

# Import configuration
nlm-cli config import --file custom.cfg
```

### Experiment Management

```bash
# List available experiments
nlm-cli experiments list

# Run specific experiment
nlm-cli experiments run [name] [options]

# Run batch of experiments
nlm-cli experiments batch --file experiments.json

# Monitor experiment progress
nlm-cli experiments monitor [id]

# Analyze experiment results
nlm-cli experiments analyze [id] --metrics [reward|firing_rate|learning_rate]
```

### Performance Optimization

```bash
# Optimize brain for specific task
nlm-cli optimize --mode performance --target spikecount

# Monitor system resources
nlm-cli monitor system --interval 1

# Profile simulation
nlm-cli profile --mode time --steps 100

# Check memory usage
nlm-cli monitor memory --format json
```

## Advanced Python API

### Core Brain Control

```python
import pynlm
import asyncio

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set('neuron_count', 5000)
config.set('region_count', 3)
config.set('connection_probability', 0.2)
brain = pynlm.createBrain(config)

# Advanced configuration setup
brain.getConfig().set('stdp_ltp_weight', 0.05)
brain.getConfig().set('stdp_ltd_weight', 0.04)
brain.getConfig().set('synaptogenesis_rate', 0.0005)
brain.getConfig().set('pruning_rate', 0.00005)

# Initialize with development stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Run advanced simulation
async def run_advanced_simulation():
    await brain.initialize()
    
    # Create agent and world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePredictionError(True)
    
    # Advanced simulation loop
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and track learning
        if result.success:
            agent.applyRewardModulation(result.reward, 0.0)
        
        # Log advanced metrics every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")

# Run the simulation
asyncio.run(run_advanced_simulation())
```

### Advanced Memory Management

```python
# Working Memory Control
wm = brain.getWorkingMemory()
if wm:
    # Add persistent activity
    wm.addTrace(1.0, 5, 100)  # strength, pattern_id, duration
    
    # Get most active traces
    traces = wm.getMostActiveTraces(5)
    
    # Check for pattern completion
    completion = wm.checkPatternCompletion(pattern_id)
    
    # Trace cleanup
    wm.cleanupInactiveTraces(0.01)  # decay threshold

# Episodic Memory Management
em = brain.getEpisodicMemory()
if em:
    # Store episode with context
    em.storeEpisode({
        'step': step,
        'reward': total_reward,
        'action': action_type,
        'context': sensory_context,
        'outcome': outcome
    })
    
    # Retrieve episodes
    episodes = em.getEpisodesForReplay(10, time_window=1000)
    
    # Check episode importance
    importance = em.checkEpisodeImportance(episode_id)
    
    # Consolidation
    em.consolidateImportantEpisodes()

# Prediction System Advanced Usage
pred = brain.getPredictionSystem()
if pred:
    # Generate prediction
    prediction = pred.predictNextState(sensory_input)
    
    # Get prediction error
    error = pred.getPredictionError()
    
    # Update prediction model
    pred.updatePrediction(actual_result, error)
    
    # Get confidence
    confidence = pred.getConfidence()
```

### Advanced Agent Control

```python
# Custom AgentBrain subclass
class AdvancedAgentBrain(pynlm.AgentBrain):
    def __init__(self, brain):
        super().__init__(brain)
        self.exploration_strategy = 'boltzmann'
        self.exploration_temperature = 1.0
        self.reward_history = []
        
    def adaptive_action_selection(self):
        """Implement Boltzmann exploration"""
        if self.exploration_strategy == 'boltzmann':
            # Get action preferences from motor neuron activity
            preferences = self.get_action_preferences()
            
            # Convert to Boltzmann distribution
            import math
            exp_values = [math.exp(p / self.exploration_temperature) for p in preferences]
            total = sum(exp_values)
            probabilities = [e / total for e in exp_values]
            
            # Sample from distribution
            import random
            return random.choices(range(len(MotorCommand)), weights=probabilities)[0]
            
    def get_action_preferences(self):
        """Convert neural activity to action preferences"""
        preferences = []
        motor_groups = [
            self.motorForward_, self.motorBackward_,
            self.motorTurnLeft_, self.motorTurnRight_,
            self.motorInteract_, self.motorWait_
        ]
        
        for group in motor_groups:
            activity = self.calculate_group_activity(group)
            preferences.append(activity)
            
        return preferences

# Custom Visualization
class AdvancedVisualizer:
    def __init__(self, brain):
        self.brain = brain
        self.history = {
            'firing_rates': [],
            'weights': [],
            'spikes': [],
            'memory': []
        }
        
    def create_activity_heatmap(self, neurons, width=800, height=600):
        """Create visual heatmap of neural activity"""
        # Normalize activity values
        activities = [n.getMembranePotential() for n in neurons]
        min_activity = min(activities)
        max_activity = max(activities)
        
        # Create heatmap data
        heatmap_data = []
        for act in activities:
            intensity = (act - min_activity) / (max_activity - min_activity)
            heatmap_data.append((int(intensity * 255), int(intensity * 255), int(intensity * 255)))
            
        return heatmap_data
        
    def create_spike_raster(self, neuron_ids, spike_times, width=1200, height=400):
        """Create spike raster plot"""
        # This would use a plotting library like matplotlib or create custom SVG
        # For now, return structured data
        return {
            'neurons': neuron_ids,
            'spike_times': spike_times,
            'width': width,
            'height': height
        }
        
    def generate_report(self, output_file):
        """Generate comprehensive simulation report"""
        report = {
            'summary': {
                'total_steps': len(self.history['firing_rates']),
                'final_firing_rate': self.history['firing_rates'][-1],
                'max_spike_count': max(self.history['spikes']),
                'avg_weight': sum(self.history['weights']) / len(self.history['weights'])
            },
            'memory_stats': {
                'working_memory_traces': self.history['memory']
            },
            'time_series': {
                'firing_rate': self.history['firing_rates'],
                'spike_count': self.history['spikes'],
                'weight_evolution': self.history['weights']
            }
        }
        
        # Write to file
        import json
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
            
        return report
```

## Advanced Configuration Examples

### Performance-Optimized Configuration

```python
# High-performance configuration for large networks
performance_config = pynlm.createDefaultConfig()

# Network parameters
performance_config.set('neuron_count', 10000)
performance_config.set('region_count', 5)
performance_config.set('connection_probability', 0.1)

# Plasticity parameters for learning
performance_config.set('stdp_ltp_weight', 0.1)
performance_config.set('stdp_ltd_weight', 0.08)
performance_config.set('stdp_tau', 25.0)

# Development parameters
performance_config.set('synaptogenesis_rate', 0.001)
performance_config.set('pruning_rate', 0.0001)
performance_config.set('critical_period_end', 100.0)

# Memory parameters
performance_config.set('working_memory_capacity', 1000)
performance_config.set('episodic_memory_capacity', 5000)
performance_config.set('consolidation_interval', 50)

# World parameters for exploration
world_config = {
    'width': 100,
    'height': 100,
    'vision_width': 20,
    'vision_height': 20,
    'max_energy': 1000,
    'energy_decay_rate': 0.05,
    'resource_energy_gain': 100,
    'hazard_damage': 0.5
}
```

### Learning-Focused Configuration

```python
# Configuration optimized for rapid learning
learning_config = pynlm.createDefaultConfig()

# High plasticity for fast learning
learning_config.set('stdp_ltp_weight', 0.05)
learning_config.set('stdp_ltd_weight', 0.04)
learning_config.set('stdp_tau', 15.0)

# Strong structural plasticity
learning_config.set('synaptogenesis_rate', 0.002)
learning_config.set('pruning_rate', 0.0002)

# Enhanced neuromodulation
learning_config.set('dopamine_sensitivity', 1.5)
learning_config.set('curiosity_threshold', 0.2)

# Memory-focused settings
learning_config.set('working_memory_boost', True)
learning_config.set('replay_frequency', 0.1)  # Replay every 10%
learning_config.set('consolidation_strength', 0.8)
```

### Developmental Configuration

```python
# Configuration for developmental learning
developmental_config = pynlm.createDefaultConfig()

# Phase-based development
developmental_config.set('developmental_stage', 'critical_period')
developmental_config.set('plasticity_modifier', 2.0)  # Boost plasticity

# Early learning phase
developmental_config.set('synaptogenesis_rate', 0.003)
developmental_config.set('pruning_rate', 0.00005)

# Learning rate increases with development
developmental_config.set('learning_rate_multiplier', [1.0, 2.0, 1.5, 1.0])

# Critical period extensions
developmental_config.set('critical_period_extensions', ['language', 'social', 'motor'])
```

## Error Handling and Debugging

### Advanced Error Handling

```python
def safe_simulation_run(brain, max_steps=1000):
    """Run simulation with comprehensive error handling"""
    try:
        brain.initialize()
        
        for step in range(max_steps):
            # Check for brain state validity
            if not brain.isHealthy():
                raise BrainStateError("Brain became unstable")
                
            # Run step with error recovery
            try:
                brain.step(step)
            except SimulationError as e:
                if e.can_recover():
                    brain.reset()
                    continue
                else:
                    raise
                    
            # Check for memory leaks
            if step % 100 == 0:
                check_memory_usage()
                
            # Validate neural integrity
            if step % 500 == 0:
                validate_neural_connectivity(brain)
                
    except Exception as e:
        # Handle simulation errors gracefully
        handle_simulation_error(e, brain)
        raise
        
    finally:
        # Cleanup resources
        cleanup_resources(brain)

# Custom exceptions
class BrainStateError(Exception):
    pass

class SimulationError(Exception):
    def __init__(self, message, can_recover=True):
        super().__init__(message)
        self.can_recover = can_recover
```

### Debugging Tools

```python
class DebugTools:
    def __init__(self, brain):
        self.brain = brain
        self.trace_data = []
        self.breakpoint_steps = set()
        
    def trace_execution(self, step):
        """Record detailed execution trace"""
        trace = {
            'step': step,
            'timestamp': time.time(),
            'neuron_activity': self.get_neuron_activity_summary(),
            'memory_state': self.get_memory_state(),
            'rewards': self.get_recent_rewards()
        }
        self.trace_data.append(trace)
        
    def add_breakpoint(self, step):
        """Set a breakpoint at specific step"""
        self.breakpoint_steps.add(step)
        
    def check_breakpoint(self, step):
        """Check if we're at a breakpoint"""
        if step in self.breakpoint_steps:
            self.debug_at_breakpoint(step)
            
    def debug_at_breakpoint(self, step):
        """Debug at breakpoint"""
        print(f"=== BREAKPOINT at step {step} ===")
        print(f"Brain status: {self.brain.getStatus()}")
        print(f"Memory: {self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0}")
        print(f"Firing rate: {self.brain.getAverageFiringRate()}")
        
        # Allow user to inspect state
        import readline
        user_input = input("Debug commands (help for options): ")
        
    def visualize_trace(self, start_step, end_step):
        """Create visualization of execution trace"""
        filtered_trace = [t for t in self.trace_data 
                         if start_step <= t['step'] <= end_step]
        
        # Create time series plots
        steps = [t['step'] for t in filtered_trace]
        firing_rates = [t['neuron_activity']['avg_firing_rate'] for t in filtered_trace]
        
        # Plot using matplotlib (if available)
        try:
            import matplotlib.pyplot as plt
            plt.figure(figsize=(12, 8))
            
            plt.subplot(2, 1, 1)
            plt.plot(steps, firing_rates)
            plt.title('Firing Rate Over Time')
            plt.xlabel('Step')
            plt.ylabel('Firing Rate')
            
            plt.subplot(2, 1, 2)
            memory_traces = [t['memory_state']['working_memory_traces'] for t in filtered_trace]
            plt.plot(steps, memory_traces)
            plt.title('Working Memory Traces')
            plt.xlabel('Step')
            plt.ylabel('Active Traces')
            
            plt.tight_layout()
            plt.savefig('debug_trace.png')
            plt.close()
            
            print("Debug trace visualization saved to 'debug_trace.png'")
            
        except ImportError:
            print("matplotlib not available for visualization")
```

## Performance Monitoring

### System Monitoring

```python
class SystemMonitor:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.metrics = {
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_time': [],
            'spike_counts': [],
            'action_times': []
        }
        
    def start_monitoring(self, interval=0.1):
        """Start continuous monitoring"""
        import threading
        import time
        
        def monitor_loop():
            while self.monitoring:
                self.collect_metrics()
                time.sleep(interval)
                
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=monitor_loop)
        self.monitor_thread.start()
        
    def collect_metrics(self):
        """Collect current system metrics"""
        import psutil
        import time
        
        # System metrics
        self.metrics['memory_usage'].append(psutil.virtual_memory().percent)
        self.metrics['cpu_usage'].append(psutil.cpu_percent())
        
        # Simulation metrics
        self.metrics['simulation_time'].append(self.world.getSimulationTime())
        self.metrics['spike_counts'].append(self.brain.getTotalSpikeCount())
        
        # Performance metrics
        start_time = time.time()
        brain.step(0)  # Minimal step
        action_time = time.time() - start_time
        self.metrics['action_times'].append(action_time)
        
    def generate_performance_report(self, output_file):
        """Generate detailed performance report"""
        report = {
            'summary': {
                'total_steps_monitored': len(self.metrics['spike_counts']),
                'avg_memory_usage': sum(self.metrics['memory_usage']) / len(self.metrics['memory_usage']),
                'avg_cpu_usage': sum(self.metrics['cpu_usage']) / len(self.metrics['cpu_usage']),
                'avg_action_time': sum(self.metrics['action_times']) / len(self.metrics['action_times'])
            },
            'time_series': self.metrics,
            'recommendations': self.generate_recommendations()
        }
        
        # Write report
        import json
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
            
        return report
        
    def generate_recommendations(self):
        """Generate performance recommendations"""
        recommendations = []
        
        # Check memory usage
        avg_memory = sum(self.metrics['memory_usage']) / len(self.metrics['memory_usage'])
        if avg_memory > 80:
            recommendations.append("High memory usage detected. Consider reducing network size or enabling memory pooling.")
            
        # Check CPU usage
        avg_cpu = sum(self.metrics['cpu_usage']) / len(self.metrics['cpu_usage'])
        if avg_cpu > 70:
            recommendations.append("High CPU usage. Consider enabling parallel processing or optimizing spike propagation.")
            
        # Check action times
        avg_action_time = sum(self.metrics['action_times']) / len(self.metrics['action_times'])
        if avg_action_time > 0.1:  # 100ms
            recommendations.append("Slow action processing. Consider optimizing motor decoding or enabling SIMD.")
            
        return recommendations
```

## Advanced Experiment Framework

### Custom Experiment Class

```python
class AdvancedExperiment:
    def __init__(self, config, brain, world, agent):
        self.config = config
        self.brain = brain
        self.world = world
        self.agent = agent
        self.results = {}
        self.checkpoints = []
        
    def run_with_validation(self):
        """Run experiment with comprehensive validation"""
        print("Starting advanced experiment...")
        
        # Pre-experiment validation
        if not self.validate_initial_state():
            raise ValueError("Initial brain state is invalid")
            
        # Run main experiment
        for step in range(self.config.max_steps):
            try:
                self.run_step(step)
                
                # Validate during experiment
                if step % self.config.validation_interval == 0:
                    self.validate_during_experiment(step)
                    
                # Save checkpoints
                if step % self.config.checkpoint_interval == 0:
                    self.save_checkpoint(step)
                    
            except ExperimentError as e:
                self.handle_experiment_error(e, step)
                
        # Post-experiment analysis
        self.analyze_results()
        
    def validate_initial_state(self):
        """Validate initial brain and world state"""
        # Check brain connectivity
        if self.brain.getTotalNeuronCount() == 0:
            return False
            
        # Check world configuration
        if self.world.getWidth() <= 0 or self.world.getHeight() <= 0:
            return False
            
        # Check agent setup
        if not self.agent.isInitialized():
            return False
            
        return True
        
    def validate_during_experiment(self, step):
        """Validate experiment state during run"""
        # Check for anomalies
        firing_rate = self.brain.getAverageFiringRate()
        if firing_rate < 0 or firing_rate > 100:  # Reasonable bounds
            raise ExperimentError(f"Abnormal firing rate: {firing_rate}")
            
        # Check memory systems
        wm = self.brain.getWorkingMemory()
        if wm and wm.getActiveTraces() > 1000:  # Capacity limit
            raise ExperimentError("Working memory at capacity")
            
    def save_checkpoint(self, step):
        """Save experiment checkpoint"""
        checkpoint = {
            'step': step,
            'brain_state': self.brain.save(),
            'world_state': self.world.save(),
            'agent_state': self.agent.save(),
            'timestamp': time.time()
        }
        self.checkpoints.append(checkpoint)
        
    def handle_experiment_error(self, error, step):
        """Handle experiment errors"""
        print(f"Experiment error at step {step}: {error}")
        
        if error.recoverable:
            # Attempt recovery
            if error.type == 'brain_crash':
                self.brain.reset()
            elif error.type == 'world_corruption':
                self.world.reset()
            elif error.type == 'agent_failure':
                self.agent.reset()
                
            # Continue experiment
            print("Recovery successful, continuing experiment...")
        else:
            # Abort experiment
            print("Unrecoverable error, aborting experiment")
            self.abort_experiment()
            
    def analyze_results(self):
        """Analyze experiment results"""
        self.results['total_steps'] = self.config.max_steps
        self.results['final_firing_rate'] = self.brain.getAverageFiringRate()
        self.results['total_spikes'] = self.brain.getTotalSpikeCount()
        self.results['final_reward'] = getattr(self, 'total_reward', 0)
        
        # Memory analysis
        wm = self.brain.getWorkingMemory()
        if wm:
            self.results['working_memory_traces'] = wm.getActiveTraces()
            
        em = self.brain.getEpisodicMemory()
        if em:
            self.results['episodic_memory_episodes'] = em.getEpisodeCount()
            
        # Learning analysis
        self.results['learning_rate'] = self.calculate_learning_rate()
        self.results['convergence'] = self.check_convergence()
        
    def calculate_learning_rate(self):
        """Calculate learning rate from results"""
        # Simple implementation - could be more sophisticated
        if len(self.checkpoints) < 2:
            return 0
            
        weight_changes = []
        for i in range(1, len(self.checkpoints)):
            # Compare weight distributions
            weight_change = self.calculate_weight_difference(
                self.checkpoints[i-1]['brain_state'],
                self.checkpoints[i]['brain_state']
            )
            weight_changes.append(weight_change)
            
        return sum(weight_changes) / len(weight_changes)
```

---

# Appendix: Advanced Function Reference

## pynlm.createAdvancedConfig()

Creates an advanced configuration with all optimization settings:

```python
config = pynlm.createAdvancedConfig()
config.optimize_for('learning')
config.set('development_stage', 'critical_period')
```

## pynlm.createCustomWorld()

Creates a world with custom dynamics and object behaviors:

```python
world = pynlm.createCustomWorld()
world.addDynamicObject(type='predator', behavior='hunt')
world.addDynamicObject(type='prey', behavior='flee')
world.setPhysics(gravity=9.8, friction=0.5)
```

## pynlm.createLearningAgent()

Creates an agent with advanced learning capabilities:

```python
agent = pynlm.createLearningAgent(brain)
agent.setLearningStrategy('meta_learning')
agent.setExploration('boltzmann', temperature=1.0)
agent.setRewardScaling('exponential', base=1.5)
```

## pynlm.runBenchmark()

Runs comprehensive benchmarks on brain performance:

```python
results = pynlm.runBenchmark(brain, tests=['spike_time', 'memory', 'plasticity'])
```

## pynlm.analyzeBrainState()

Analyzes brain state for anomalies and optimization opportunities:

```python
analysis = pynlm.analyzeBrainState(brain)
```

---

*This advanced user guide covers comprehensive NLM usage for power users. For detailed API documentation, refer to the specific function documentation in the pynlm module.*