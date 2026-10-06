# NLM - Advanced Usage Guide

## Advanced Python Examples

This section provides advanced examples for power users and researchers working with NLM.

### Example 1: Advanced Agent with Learning Management

```python
import pynlm
import numpy as np

class AdvancedAgent:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world for complex environment
        self.world.configure(
            width=30, height=30,
            visionWidth=16, visionHeight=16
        )
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable all learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Learning parameters
        self.learning_history = []
        self.episode_rewards = []
        
    def run_episode(self, max_steps=1000):
        """Run a complete learning episode with advanced features."""
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Get brain stats for monitoring
            brain_stats = self.getBrainStats()
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation with prediction error
            reward = self.calculateReward(episode=step)
            predicted = self.getPredictionError()
            self.agent.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Log learning progress
            if step % 100 == 0:
                self.logProgress(step, brain_stats, reward)
                
            # Check for completion conditions
            if self.checkEpisodeCompletion(step):
                break
                
        return self.getLearningSummary()
    
    def calculateReward(self, episode):
        """Calculate complex reward function based on agent behavior."""
        body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        # Base reward from energy
        reward = body.energy / 100.0
        
        # Bonus for curiosity (exploration)
        curiosity = self.agent.getCuriosityLevel()
        reward += curiosity * 2.0
        
        # Penalty for low vision quality
        vision = percept.getVision()
        if vision.size() > 0:
            avg_vision = np.mean(vision)
            vision_quality = avg_vision / 255.0
            reward += vision_quality * 0.5
            
        # Developmental stage bonus
        dev_stage = self.brain.getDevelopmentalStage()
        if dev_stage.value >= pynlm.DevelopmentalStage.Maturation.value:
            reward += 1.0
            
        return reward
    
    def getPredictionError(self):
        """Get prediction error for learning modulation."""
        return self.agent.getPredictionError()
    
    def getBrainStats(self):
        """Get comprehensive brain statistics."""
        stats = {}
        stats['neuron_count'] = self.brain.getTotalNeuronCount()
        stats['synapse_count'] = self.brain.getTotalSynapseCount()
        stats['firing_neuron_count'] = self.brain.getFiringNeuronCount()
        stats['active_neuron_count'] = self.brain.getActiveNeuronCount()
        stats['total_spikes'] = self.brain.getTotalSpikeCount()
        stats['average_firing_rate'] = self.brain.getAverageFiringRate()
        stats['e_i_ratio'] = self.brain.getExcitationInhibitionRatio()
        stats['developmental_stage'] = self.brain.getDevelopmentalStage().value
        
        return stats
    
    def logProgress(self, step, brain_stats, reward):
        """Log learning progress with detailed metrics."""
        print(f"Step {step}:")
        print(f"  Neurons: {brain_stats['neuron_count']}, "
              f"Firing: {brain_stats['firing_neuron_count']}, "
              f"Active: {brain_stats['active_neuron_count']}")
        print(f"  Learning rate: {brain_stats['average_firing_rate']:.3f}, "
              f"E/I ratio: {brain_stats['e_i_ratio']:.3f}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}, "
              f"Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Reward: {reward:.3f}, Prediction error: {self.getPredictionError():.3f}")
        print(f"  Developmental stage: {brain_stats['developmental_stage']}")
        
    def checkEpisodeCompletion(self, step):
        """Check if episode should be terminated."""
        body = self.world.getAgentBody()
        
        # Health threshold
        if body.health <= 0:
            return True
            
        # Energy depletion
        if body.energy <= 0:
            return True
            
        # Maximum steps reached
        if step >= 1000:
            return True
            
        # Learning plateau detection
        if len(self.episode_rewards) > 100:
            recent_rewards = self.episode_rewards[-100:]
            avg_reward = np.mean(recent_rewards)
            if avg_reward < 0.1 and step > 500:
                return True
                
        return False
    
    def getLearningSummary(self):
        """Get summary of learning performance."""
        summary = {}
        summary['total_steps'] = len(self.episode_rewards)
        summary['final_reward'] = np.mean(self.episode_rewards) if self.episode_rewards else 0
        summary['max_reward'] = max(self.episode_rewards) if self.episode_rewards else 0
        summary['min_reward'] = min(self.episode_rewards) if self.episode_rewards else 0
        summary['reward_variance'] = np.var(self.episode_rewards) if len(self.episode_rewards) > 1 else 0
        
        return summary

# Usage example
agent = AdvancedAgent()
result = agent.run_episode()
print(f"Learning completed: {result}")
```

### Example 2: Performance Benchmarking and Profiling

```python
import pynlm
import time
import matplotlib.pyplot as plt
import numpy as np

class PerformanceProfiler:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure for benchmarking
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Performance metrics
        self.timings = []
        self.memory_usage = []
        self.neuron_activity = []
        self.simulation_steps = []
        
    def profile_simulation(self, steps=100):
        """Profile simulation performance with detailed metrics."""
        print("Starting performance profiling...")
        
        for step in range(steps):
            start_time = time.time()
            
            # Profile each component
            step_start = time.time()
            self.world.update(0.1)
            world_time = time.time() - step_start
            
            step_start = time.time()
            percept = self.world.getSensoryPercept()
            sensor_time = time.time() - step_start
            
            step_start = time.time()
            self.agent.processSensoryInput(percept)
            processing_time = time.time() - step_start
            
            step_start = time.time()
            self.brain.step(step)
            brain_time = time.time() - step_start
            
            step_start = time.time()
            action = self.agent.decodeMotorCommand()
            decode_time = time.time() - step_start
            
            step_start = time.time()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            world_action_time = time.time() - step_start
            
            # Record timing
            total_step_time = time.time() - start_time
            self.timings.append(total_step_time)
            
            # Record component timings
            self.simulation_steps.append({
                'total': total_step_time,
                'world_update': world_time,
                'sensory': sensor_time,
                'processing': processing_time,
                'brain': brain_time,
                'decoding': decode_time,
                'world_action': world_action_time
            })
            
            # Record neural activity
            self.neuron_activity.append({
                'firing': self.brain.getFiringNeuronCount(),
                'active': self.brain.getActiveNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel()
            })
            
            if step % 20 == 0:
                print(f"Step {step}/{steps}: "
                      f"Total: {total_step_time*1000:.1f}ms, "
                      f"Brain: {brain_time*1000:.1f}ms, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
                
        return self.generateProfileReport()
    
    def generateProfileReport(self):
        """Generate comprehensive performance report."""
        report = {}
        
        # Timing analysis
        report['timing_stats'] = {
            'total_steps': len(self.timings),
            'avg_step_time': np.mean(self.timings),
            'std_step_time': np.std(self.timings),
            'min_step_time': np.min(self.timings),
            'max_step_time': np.max(self.timings),
            'avg_component_times': self.calculateAverageComponentTimes()
        }
        
        # Neural activity analysis
        report['neural_stats'] = {
            'avg_firing_rate': np.mean([a['firing_rate'] for a in self.neuron_activity]),
            'avg_curiosity': np.mean([a['curiosity'] for a in self.neuron_activity]),
            'avg_novelty': np.mean([a['novelty'] for a in self.neuron_activity]),
            'firing_rate_variance': np.var([a['firing_rate'] for a in self.neuron_activity]),
            'activity_trend': self.calculateActivityTrend()
        }
        
        # Performance bottlenecks
        report['bottlenecks'] = self.identifyBottlenecks()
        
        # Recommendations
        report['recommendations'] = self.generateRecommendations()
        
        return report
    
    def calculateAverageComponentTimes(self):
        """Calculate average times for each component."""
        components = ['world_update', 'sensory', 'processing', 'brain', 'decoding', 'world_action']
        return {comp: np.mean([s[comp] for s in self.simulation_steps]) for comp in components}
    
    def calculateActivityTrend(self):
        """Calculate neural activity trend over time."""
        rates = [a['firing_rate'] for a in self.neuron_activity]
        if len(rates) < 10:
            return 'insufficient_data'
            
        # Calculate linear trend
        x = np.arange(len(rates))
        slope = np.polyfit(x, rates, 1)[0]
        
        if slope > 0.01:
            return 'increasing'
        elif slope < -0.01:
            return 'decreasing'
        else:
            return 'stable'
    
    def identifyBottlenecks(self):
        """Identify performance bottlenecks."""
        avg_times = self.calculateAverageComponentTimes()
        bottlenecks = []
        
        for component, avg_time in avg_times.items():
            if avg_time > 0.01:  # Component taking more than 10ms
                bottlenecks.append({
                    'component': component,
                    'avg_time': avg_time,
                    'impact': 'high' if avg_time > 0.05 else 'medium'
                })
                
        return bottlenecks
    
    def generateRecommendations(self):
        """Generate performance optimization recommendations."""
        recommendations = []
        avg_times = self.calculateAverageComponentTimes()
        
        for component, avg_time in avg_times.items():
            if avg_time > 0.05:  # Component taking more than 50ms
                if component == 'brain':
                    recommendations.append("Consider using SIMD optimizations for neural computations")
                elif component == 'processing':
                    recommendations.append("Optimize sensory processing pipeline")
                elif component == 'world_update':
                    recommendations.append("Consider spatial partitioning for world updates")
                    
        if not recommendations:
            recommendations.append("Performance is within acceptable range")
            
        return recommendations

# Usage example
profiler = PerformanceProfiler()
report = profiler.profile_simulation(200)
print("Performance Report:")
print(f"Average step time: {report['timing_stats']['avg_step_time']*1000:.1f}ms")
print(f"Average firing rate: {report['neural_stats']['avg_firing_rate']:.3f}")
print("Bottlenecks:", report['bottlenecks'])
```

### Example 3: Advanced Configuration Management

```python
import pynlm
import json
import os

class ConfigurationManager:
    def __init__(self, config_file=None):
        self.config_file = config_file or "nlm_config.json"
        self.config = pynlm.createDefaultConfig()
        self.load_configuration()
        
    def load_configuration(self):
        """Load configuration from file or create default."""
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    config_data = json.load(f)
                self.apply_configuration(config_data)
                print(f"Loaded configuration from {self.config_file}")
            except Exception as e:
                print(f"Error loading configuration: {e}")
                print("Using default configuration")
        else:
            print(f"Configuration file {self.config_file} not found. Using defaults.")
            
    def apply_configuration(self, config_data):
        """Apply configuration data to the config object."""
        # Brain configuration
        if 'brain' in config_data:
            brain_config = config_data['brain']
            if 'neuron_count' in brain_config:
                self.config.set('brain.neuron_count', brain_config['neuron_count'])
            if 'synapse_density' in brain_config:
                self.config.set('brain.synapse_density', brain_config['synapse_density'])
                
        # Plasticity configuration
        if 'plasticity' in config_data:
            plasticity_config = config_data['plasticity']
            if 'stdp' in plasticity_config:
                for key, value in plasticity_config['stdp'].items():
                    self.config.set(f'plasticity.stdp.{key}', value)
                    
        # Neuromodulation configuration
        if 'neuromod' in config_data:
            neuromod_config = config_data['neuromod']
            for key, value in neuromod_config.items():
                self.config.set(f'neuromod.{key}', value)
                
    def create_brain(self):
        """Create a brain with current configuration."""
        return pynlm.createBrain(self.config)
        
    def save_configuration(self, filename=None):
        """Save current configuration to file."""
        output_file = filename or self.config_file
        config_data = self.export_configuration()
        
        try:
            with open(output_file, 'w') as f:
                json.dump(config_data, f, indent=2)
            print(f"Configuration saved to {output_file}")
        except Exception as e:
            print(f"Error saving configuration: {e}")
            
    def export_configuration(self):
        """Export current configuration as dictionary."""
        config_data = {
            'brain': {
                'neuron_count': self.config.getOr('brain.neuron_count', 1000),
                'synapse_density': self.config.getOr('brain.synapse_density', 0.1)
            },
            'plasticity': {
                'stdp': {
                    'enable': self.config.getOr('plasticity.stdp.enable', True),
                    'learning_rate': self.config.getOr('plasticity.stdp.learning_rate', 0.001),
                    'tau_plus': self.config.getOr('plasticity.stdp.tau_plus', 20.0),
                    'tau_minus': self.config.getOr('plasticity.stdp.tau_minus', 20.0)
                }
            },
            'neuromod': {
                'dopamine': {'scale': self.config.getOr('neuromod.dopamine.scale', 1.0)},
                'curiosity': {'enable': self.config.getOr('neuromod.curiosity.enable', True)},
                'novelty': {'enable': self.config.getOr('neuromod.novelty.enable', True)}
            }
        }
        return config_data
        
    def validate_configuration(self):
        """Validate configuration parameters."""
        errors = []
        
        # Check neuron count
        neuron_count = self.config.getOr('brain.neuron_count', 1000)
        if neuron_count < 100 or neuron_count > 100000:
            errors.append(f"Invalid neuron count: {neuron_count}. Must be between 100 and 100000")
            
        # Check learning rates
        learning_rate = self.config.getOr('plasticity.stdp.learning_rate', 0.001)
        if learning_rate < 0 or learning_rate > 1:
            errors.append(f"Invalid learning rate: {learning_rate}. Must be between 0 and 1")
            
        # Check time constants
        tau_plus = self.config.getOr('plasticity.stdp.tau_plus', 20.0)
        tau_minus = self.config.getOr('plasticity.stdp.tau_minus', 20.0)
        if tau_plus <= 0 or tau_minus <= 0:
            errors.append("Time constants must be positive")
            
        return errors

# Usage example
config_manager = ConfigurationManager("advanced_config.json")
print("Configuration errors:", config_manager.validate_configuration())

# Create custom brain
custom_brain = config_manager.create_brain()
print(f"Created brain with {custom_brain.getTotalNeuronCount()} neurons")
```

### Advanced Usage Pattern: Modular Agent Design

```python
import pynlm

class ModularAgent:
    """Advanced agent design with modular components."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize world
        self.world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Modular components
        self.components = {
            'memory': MemoryManager(),
            'prediction': PredictionManager(),
            'attention': AttentionManager(),
            'planning': PlanningManager(),
            'sensory_fusion': SensoryFusionManager(),
            'action_selection': ActionSelectionManager()
        }
        
        # Performance monitoring
        self.performance_metrics = PerformanceMonitor()
        
    def run_step(self, step_number):
        """Execute a single simulation step with modular components."""
        # Record start time
        self.performance_metrics.start_step(step_number)
        
        # Sensory processing pipeline
        percept = self.world.getSensoryPercept()
        fused_sensory = self.components['sensory_fusion'].process(percept)
        
        # Memory integration
        memory_context = self.components['memory'].get_context(fused_sensory)
        
        # Prediction and novelty detection
        prediction = self.components['prediction'].predict(fused_sensory)
        novelty = self.components['prediction'].detect_novelty(fused_sensory)
        
        # Attention allocation
        attended_features = self.components['attention'].allocate(fused_sensory, prediction)
        
        # Concept formation and planning
        concepts = self.components['concept_formation'].form(attended_features)
        plan = self.components['planning'].execute(concepts, prediction)
        
        # Inject into brain
        self.agent.processSensoryInput(percept)
        self.brain.step(step_number)
        
        # Action selection
        action = self.components['action_selection'].select(plan, prediction)
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Record performance metrics
        self.performance_metrics.end_step(step_number)
        
        # Return step results
        return {
            'step': step_number,
            'action': action,
            'prediction': prediction,
            'novelty': novelty,
            'performance': self.performance_metrics.get_step_metrics(step_number)
        }
    
    def run_simulation(self, num_steps):
        """Run complete simulation with monitoring."""
        results = []
        
        for step in range(num_steps):
            step_result = self.run_step(step)
            results.append(step_result)
            
            # Periodic reporting
            if step % 100 == 0:
                self.performance_metrics.report_progress(step, results)
                
            # Component health checks
            if step % 500 == 0:
                health_report = self.check_component_health()
                print(f"Component health: {health_report}")
                
        return self.generate_simulation_report(results)
    
    def check_component_health(self):
        """Check health of all modular components."""
        health_status = {}
        
        for name, component in self.components.items():
            try:
                health_status[name] = component.health_check()
            except Exception as e:
                health_status[name] = {'status': 'error', 'message': str(e)}
                
        return health_status
    
    def generate_simulation_report(self, results):
        """Generate comprehensive simulation report."""
        report = {
            'total_steps': len(results),
            'components': self.components,
            'performance_summary': self.performance_metrics.get_summary(),
            'learning_metrics': self.calculate_learning_metrics(results),
            'component_performance': self.analyze_component_performance(results)
        }
        
        return report

# Usage example
agent = ModularAgent()
simulation_results = agent.run_simulation(5000)
print(f"Simulation completed: {simulation_results['total_steps']} steps")
```

## Advanced Troubleshooting Guide

### Performance Issues

**Symptom:** Simulation runs slowly or hangs

**Possible Causes:**
1. Too many neurons or synapses
2. Complex sensory processing
3. Memory leaks in custom components
4. Inefficient world updates

**Solutions:**
1. Reduce brain size: `config.set("brain.neuron_count", 500)`
2. Simplify sensory input: Use smaller vision dimensions
3. Implement component health checks and cleanup
4. Use spatial partitioning for large worlds

**Debugging Steps:**
```python
# Add performance monitoring
profiler = PerformanceProfiler()
report = profiler.profile_simulation(100)

# Check for bottlenecks
bottlenecks = report['bottlenecks']
for bottleneck in bottlenecks:
    print(f"Bottleneck: {bottleneck['component']} "
          f"({bottleneck['avg_time']*1000:.1f}ms avg)")
```

### Learning Issues

**Symptom:** Agent stops learning or behavior plateaus

**Possible Causes:**
1. Neuromodulation disabled
2. Reward signals too small
3. Developmental stage not reached
4. Prediction errors too low

**Solutions:**
```python
# Ensure neuromodulation is enabled
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Check reward signals
reward = agent.getCuriosityLevel() * 2.0
agent.applyRewardModulation(reward, 0.0)

# Monitor prediction errors
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### Memory Issues

**Symptom:** High memory usage or crashes

**Possible Causes:**
1. Large configuration files
2. Memory leaks in custom components
3. Inefficient data structures
4. Unbounded growth of learning history

**Solutions:**
```python
# Implement memory management
class MemoryManager:
    def __init__(self, max_history=10000):
        self.max_history = max_history
        self.history = []
        
    def add_entry(self, entry):
        self.history.append(entry)
        if len(self.history) > self.max_history:
            self.history.pop(0)  # Remove oldest entry
            
    def get_recent_history(self, num_entries):
        return self.history[-num_entries:]
```

### Integration Issues

**Symptom:** Components don't work together properly

**Possible Causes:**
1. Incompatible component interfaces
2. Data format mismatches
3. Timing synchronization issues
4. State management problems

**Solutions:**
1. Implement interface contracts and validation
2. Use consistent data formats throughout
3. Add timing synchronization
4. Implement proper state management

### Testing and Validation

**Automated Testing:**
```python
import pytest

def test_brain_initialization():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain.getTotalNeuronCount() > 0
    brain.initialize()
    
def test_agent_step():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(width=10, height=10)
    world.reset()
    agent.initialize(world)
    
    # Run a few steps
    for i in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify results
    assert brain.getTotalSpikeCount() >= 0
    assert agent.getCuriosityLevel() >= 0
```

**Performance Validation:**
```python
def test_performance_benchmark():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(width=20, height=20)
    world.reset()
    agent.initialize(world)
    
    # Measure performance
    start_time = time.time()
    steps = 100
    
    for i in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    avg_step_time = (end_time - start_time) / steps
    
    # Assert performance requirements
    assert avg_step_time < 0.1, f"Step time too slow: {avg_step_time:.3f}s"
```

## Advanced Features Summary

This advanced guide provides comprehensive examples for:

1. **Learning Management**: Advanced agent with sophisticated learning algorithms
2. **Performance Profiling**: Detailed benchmarking and optimization
3. **Configuration Management**: Advanced configuration with validation
4. **Modular Design**: Component-based architecture for extensibility
5. **Comprehensive Testing**: Automated testing and validation frameworks

These examples demonstrate how to build production-ready NLM applications with advanced features, performance optimization, and robust error handling.

---

**Next Steps:**
1. Review the examples and adapt them to your specific use case
2. Explore the experimental features in the source code
3. Contribute to the community by sharing your advanced implementations
4. Consider joining the NLM research community for collaboration
