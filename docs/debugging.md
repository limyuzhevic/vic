# NLM Debugging and Monitoring Documentation

## Overview

The NLM debugging and monitoring system provides comprehensive tools for debugging neural network simulations, monitoring performance, and analyzing system behavior. It includes real-time monitoring, performance profiling, activity pattern analysis, and debugging utilities.

## Monitoring System Architecture

### Core Monitoring Components

```
Monitoring System
├── Metrics Collection
│   ├── Real-time Metrics
│   ├── Performance Metrics
│   └── Activity Metrics
├── Visualization
│   ├── Real-time Brain State
│   ├── Neural Activity Plots
│   └── Performance Charts
├── Analysis
│   ├── Activity Pattern Analysis
│   ├── Learning Statistics
│   └── Error Analysis
└── Alerting
    ├── Performance Alerts
    ├── Error Alerts
    └── Custom Alerts
```

### Monitoring Components

#### 1. Metrics Collection
- **Real-time Metrics**: Current neural activity, simulation state
- **Performance Metrics**: CPU usage, memory usage, simulation speed
- **Activity Metrics**: Spike rates, synaptic activity, learning progress

#### 2. Visualization
- **Real-time Brain State**: Visual representation of neural activity patterns
- **Neural Activity Plots**: Time-series of neural population activity
- **Performance Charts**: Resource utilization and simulation progress

#### 3. Analysis
- **Activity Pattern Analysis**: Discovery of recurring activity patterns
- **Learning Statistics**: Tracking of learning progress and performance
- **Error Analysis**: Identification and analysis of simulation errors

#### 4. Alerting
- **Performance Alerts**: Notifications of performance degradation
- **Error Alerts**: Immediate notifications of simulation errors
- **Custom Alerts**: User-defined alert conditions

## Monitoring System Setup

### Basic Monitoring Setup

```python
import pynlm

# Initialize monitoring system
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableMetricsStreaming': True,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True,
    'metricsInterval': 1.0,  # seconds
    'visualizationPort': 8080,
    'performanceThresholds': {
        'maxMemoryUsage': '2GB',
        'maxCPUUsage': '90%',
        'minPerformance': '10%
    }
})
```

### Advanced Monitoring Setup

```python
import pynlm

# Advanced monitoring configuration
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableMetricsStreaming': True,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True,
    'metricsInterval': 0.5,  # More frequent metrics collection
    'visualizationPort': 8080,
    'visualizationHost': 'localhost',
    'dataRetentionHours': 24,
    'maxDataPoints': 10000,
    'enableCustomAlerts': True,
    'alertConditions': [
        {
            'type': 'performance',
            'metric': 'memory_usage',
            'threshold': '3GB',
            'action': 'log_warning'
        },
        {
            'type': 'learning',
            'metric': 'learning_rate',
            'threshold': '0.001',
            'action': 'raise_alert'
        }
    ],
    'performanceThresholds': {
        'maxMemoryUsage': '4GB',
        'maxCPUUsage': '95%',
        'minPerformance': '5%'
    },
    'analysisThresholds': {
        'minActivityPatternStrength': 0.8,
        'minLearningProgress': 0.01,
        'maxErrorRate': 0.05
    }
})
```

## Real-time Monitoring

### Metrics Streaming

```python
import pynlm

# Start real-time metrics streaming
brain = pynlm.createBrain(brain_config)
pynlm.python.MonitoringSystem.startMetricsStreaming(brain)

# Stop metrics streaming
pynlm.python.MonitoringSystem.stopMetricsStreaming(brain)

# Get current metrics
metrics = pynlm.python.MonitoringSystem.getCurrentMetrics(brain)
```

### Real-time Visualization

```python
import pynlm

# Start real-time visualization
pynlm.python.MonitoringSystem.startVisualization({
    'host': 'localhost',
    'port': 8080,
    'brain': brain,
    'updateInterval': 1.0,
    'visualElements': [
        'neural_activity',
        'spike_patterns',
        'plasticity_events',
        'neuromodulation_signals'
    ]
})

# Stop visualization
pynlm.python.MonitoringSystem.stopVisualization(brain)
```

## Performance Profiling

### Performance Analysis

```python
import pynlm

# Profile simulation performance
performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

# Analyze performance bottlenecks
bottlenecks = pynlm.python.MonitoringSystem.analyzePerformanceBottlenecks(brain)

# Get optimization recommendations
recommendations = pynlm.python.MonitoringSystem.getOptimizationRecommendations(bperformance_profile)

# Apply performance optimizations
pynlm.python.PerformanceOptimizer.applyOptimizations(brain, recommendations)
```

### Performance Metrics

```python
# Example performance profile output
performance_profile = {
    'simulationSpeed': {
        'stepsPerSecond': 1000.0,
        'realTimeFactor': 1.5,
        'timeStep': 0.001
    },
    'memoryUsage': {
        'current': '500MB',
        'peak': '1GB',
        'efficiency': 0.85
    },
    'cpuUsage': {
        'current': '45%',
        'peak': '90%',
        'efficiency': 0.50
    },
    'neuronActivity': {
        'activeNeurons': 850,
        'spikeRate': 50.0,
        'activityPattern': 'regular'
    }
}
```

## Activity Pattern Analysis

### Activity Pattern Detection

```python
import pynlm

# Detect activity patterns
activity_patterns = pynlm.python.MonitoringSystem.getActivityPatterns(brain)

# Analyze pattern characteristics
for pattern in activity_patterns:
    print(f"Pattern: {pattern['name']}")
    print(f"  Strength: {pattern['strength']}")
    print(f"  Duration: {pattern['duration']} seconds")
    print(f"  Recurrence: {pattern['recurrence']}")
    print(f" neurons Involved: {len(pattern['neurons'])}")

# Get pattern predictions
pattern_predictions = pynlm.python.MonitoringSystem.predictActivityPatterns(brain)
```

### Activity Pattern Types

#### Regular Patterns
- **Burst Patterns**: High-frequency spike bursts
- **Oscillatory Patterns**: Rhythmic activity oscillations
- **Sustained Activity**: Persistent neural firing
- **Transient Patterns**: Brief activity events

#### Complex Patterns
- **Rhythmic Patterns**: Coordinated oscillations across populations
- **Emergent Patterns**: Novel patterns that emerge from interactions
- **Cyclic Patterns**: Repeating activity cycles
- **Random Patterns**: Stochastic activity variations

## Learning Statistics

### Learning Progress Tracking

```python
import pynlm

# Get learning statistics
learning_stats = pynlm.python.MonitoringSystem.getLearningStatistics(brain)

# Analyze learning progress
learning_progress = learning_stats['learning_progress']
print(f"Learning rate: {learning_progress['learning_rate']}")
print(f"Performance improvement: {learning_progress['performance_improvement']}")
print(f"Convergence epoch: {learning_progress['convergence_epoch']}")

# Get reward statistics
reward_stats = learning_stats['reward_statistics']
print(f"Average reward: {reward_stats['average_reward']}")
print(f"Reward variance: {reward_stats['reward_variance']}")
print(f"Reward distribution: {reward_stats['reward_distribution']}")

# Get action statistics
action_stats = learning_stats['action_statistics']
print(f"Action frequency: {action_stats['action_frequency']}")
print(f"Action exploration: {action_stats['action_exploration']}")
print(f"Action exploitation: {action_stats['action_exploitation']}")
```

### Learning Analysis

#### Learning Metrics
- **Learning Rate**: Speed of learning progress
- **Performance Improvement**: Improvement in task performance
- **Convergence**: Point at which learning stabilizes
- **Exploration vs Exploitation**: Balance between trying new actions and using known actions

#### Reward Analysis
- **Average Reward**: Mean reward received
- **Reward Variance**: Variability in rewards received
- **Reward Distribution**: Distribution of reward values
- **Reward Patterns**: Patterns in reward timing and magnitude

#### Action Analysis
- **Action Frequency**: How often each action is taken
- **Action Exploration**: Frequency of trying new actions
- **Action Exploitation**: Frequency of using known actions
- **Action Selection**: Criteria for action selection

## Error Analysis

### Error Detection

```python
import pynlm

# Detect errors in simulation
errors = pynlm.python.MonitoringSystem.detectErrors(brain)

# Analyze error patterns
for error in errors:
    print(f"Error: {error['type']}")
    print(f"  Severity: {error['severity']}")
    print(f"  Frequency: {error['frequency']}")
    print(f"  Location: {error['location']}")
    print(f"  Impact: {error['impact']}")

# Get error statistics
error_stats = pynlm.python.MonitoringSystem.getErrorStatistics(brain)
```

### Error Types

#### Simulation Errors
- **Numerical Instability**: Simulation becoming unstable
- **Memory Errors**: Out of memory conditions
- **Performance Degradation**: Simulation slowing down
- **Convergence Issues**: Failure to converge to solution

#### Data Errors
- **Invalid Input**: Invalid sensory input data
- **Corrupted Data**: Corrupted neural state data
- **Missing Data**: Missing required data
- **Data Format Errors**: Incorrect data format

#### Configuration Errors
- **Invalid Parameters**: Invalid configuration parameters
- **Missing Dependencies**: Missing required configuration
- **Inconsistent Configuration**: Inconsistent configuration values
- **Validation Errors**: Configuration validation failures

## Debugging Utilities

### Debug Neuron Information

```python
import pynlm

# Get information about specific neurons
brain = pynlm.createBrain(brain_config)
neuron_info = pynlm.python.MonitoringSystem.getNeuronInfo(brain, neuron_id=1)

print(f"Neuron {neuron_info['id']}:")
print(f"  Type: {neuron_info['type']}")
print(f"  Region: {neuron_info['region']}")
print(f"  Membrane Potential: {neuron_info['membrane_potential']}")
print(f"  Firing Rate: {neuron_info['firing_rate']}")
print(f"  Synaptic Input: {neuron_info['synaptic_input']}")
print(f"  Plasticity State: {neuron_info['plasticity_state']}")

# Debug specific neuron populations
def debug_neuron_population(brain, population_id, duration=10.0):
    """Debug a specific neuron population."""
    import time
    start_time = time.time()
    
    while time.time() - start_time < duration:
        # Get population activity
        activity = pynlm.python.MonitoringSystem.getPopulationActivity(
            brain, population_id
        )
        
        # Log activity
        print(f"Time {time.time() - start_time:.2f}s: "
              f"Activity {activity['activity_level']}, "
              f"Spike Rate {activity['spike_rate']}")
        
        time.sleep(1.0)

# Debug population
debug_neuron_population(brain, population_id=1)
```

### Debug Information

#### Neuron Information
- **Neuron ID**: Unique identifier for the neuron
- **Neuron Type**: Type of neuron (excitatory, inhibitory)
- **Region**: Brain region where neuron is located
- **Membrane Potential**: Current membrane potential
- **Firing Rate**: Current firing rate
- **Synaptic Input**: Current synaptic input
- **Plasticity State**: Current plasticity state

#### Population Information
- **Population ID**: Unique identifier for the population
- **Population Type**: Type of population
- **Neuron Count**: Number of neurons in population
- **Activity Level**: Current activity level
- **Spike Rate**: Average spike rate
- **Connection Pattern**: Pattern of connections
- **Function**: Function of the population

### Debugging Tools

#### 1. Spike Tracing
```python
# Enable spike tracing
pynlm.python.MonitoringSystem.enableSpikeTracing(brain, neuron_ids=[1, 2, 3])

# Get spike trace
def analyze_spike_trace(brain, duration=10.0):
    """Analyze spike trace for specific neurons."""
    import time
    start_time = time.time()
    
    spikes = []
    while time.time() - start_time < duration:
        # Get spike information
        spike_info = pynlm.python.MonitoringSystem.getSpikeInfo(brain)
        spikes.extend(spike_info)
        
        time.sleep(0.1)
    
    # Analyze spike pattern
    analyze_spike_pattern(spikes)
```

#### 2. Plasticity Tracing
```python
# Enable plasticity tracing
pynlm.python.MonitoringSystem.enablePlasticityTracing(brain)

# Get plasticity trace
def analyze_plasticity_trace(brain, duration=10.0):
    """Analyze plasticity trace."""
    import time
    start_time = time.time()
    
    plasticity_events = []
    while time.time() - start_time < duration:
        # Get plasticity information
        plasticity_info = pynlm.python.MonitoringSystem.getPlasticityInfo(brain)
        plasticity_events.extend(plasticity_info)
        
        time.sleep(1.0)
    
    # Analyze plasticity pattern
    analyze_plasticity_pattern(plasticity_events)
```

#### 3. Neuromodulation Tracing
```python
# Enable neuromodulation tracing
pynlm.python.MonitoringSystem.enableNeuromodulationTracing(brain)

# Get neuromodulation trace
def analyze_neuromodulation_trace(brain, duration=10.0):
    """Analyze neuromodulation trace."""
    import time
    start_time = time.time()
    
    neuromodulation_events = []
    while time.time() - start_time < duration:
        # Get neuromodulation information
        neuromodulation_info = pynlm.python.MonitoringSystem.getNeuromodulationInfo(brain)
        neuromodulation_events.extend(neuromodulation_info)
        
        time.sleep(1.0)
    
    # Analyze neuromodulation pattern
    analyze_neuromodulation_pattern(neuromodulation_events)
```

## Alerting System

### Custom Alerts

```python
import pynlm

# Configure custom alerts
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableCustomAlerts': True,
    'alertConditions': [
        {
            'type': 'performance',
            'metric': 'memory_usage',
            'threshold': '2GB',
            'action': 'log_warning',
            'description': 'Memory usage exceeds 2GB threshold'
        },
        {
            'type': 'learning',
            'metric': 'learning_rate',
            'threshold': '0.001',
            'action': 'raise_alert',
            'description': 'Learning rate below 0.001 threshold'
        },
        {
            'type': 'error',
            'metric': 'error_rate',
            'threshold': '0.05',
            'action': 'raise_alert',
            'description': 'Error rate exceeds 5% threshold'
        }
    ]
})

# Get alert status
alert_status = pynlm.python.MonitoringSystem.getAlertStatus(brain)
print(f"Active alerts: {len(alert_status['active_alerts'])}")
print(f"Alert history: {len(alert_status['alert_history'])}")

# Get alert details
for alert in alert_status['active_alerts']:
    print(f"Alert: {alert['type']}")
    print(f"  Description: {alert['description']}")
    print(f"  Severity: {alert['severity']}")
    print(f"  Time: {alert['time']}")
```

### Alert Actions

#### Logging Alerts
```python
# Log warning alert
pynlm.python.MonitoringSystem.logAlert({
    'type': 'performance',
    'severity': 'warning',
    'message': 'Memory usage exceeds 2GB threshold',
    'value': '2.5GB',
    'threshold': '2GB'
})
```

#### Raising Alerts
```python
# Raise critical alert
try:
    pynlm.python.MonitoringSystem.raiseAlert({
        'type': 'error',
        'severity': 'critical',
        'message': 'Simulation failed to converge',
        'action': 'stop_simulation'
    })
    
    # Stop simulation
    pynlm.python.MonitoringSystem.stopSimulation(brain)
    
except AlertException as e:
    print(f"Alert raised: {e}")
```

#### Custom Alert Actions
```python
# Custom alert action
def custom_alert_action(alert):
    """Custom alert action."""
    if alert['type'] == 'performance':
        # Log to custom system
        log_performance_alert(alert)
    elif alert['type'] == 'learning':
        # Adjust learning parameters
        adjust_learning_parameters(alert)
    elif alert['type'] == 'error':
        # Take corrective action
        take_corrective_action(alert)

# Set custom alert action
pynlm.python.MonitoringSystem.setCustomAlertAction(custom_alert_action)
```

## Debugging Examples

### Example 1: Basic Debugging

```python
import pynlm

# Create brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 1000,
    'enable_plasticity': True,
    'enable_neuromodulation': True
})

# Create brain
brain = pynlm.createBrain(brain_config)

# Initialize monitoring with debugging
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableMetricsStreaming': True,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True,
    'log_level': 'debug'
})

# Start simulation
pynlm.python.MonitoringSystem.startSimulation(brain)

# Run simulation
for episode in range(10):
    for step in range(100):
        state = [float(i) for i in range(20)]
        action = select_action(state)
        reward = compute_reward(state, action)
        
        # Add experience
        brain.addExperience(state, action, reward, state, False)
        
        # Perform learning step
        brain.performLearningStep()

# Stop simulation
pynlm.python.MonitoringSystem.stopSimulation(brain)

# Analyze results
learning_stats = pynlm.python.MonitoringSystem.getLearningStatistics(brain)
performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

print(f"Learning statistics: {learning_stats}")
print(f"Performance profile: {performance_profile}")
```

### Example 2: Advanced Debugging

```python
import pynlm

# Create brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True
})

# Create brain and agent
brain = pynlm.createBrain(brain_config)
agent_brain = pynlm.createAgentBrain(brain)
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'DEEP_Q_LEARNING',
    'learning_rate': 0.001,
    'discount_factor': 0.99
})

# Initialize advanced monitoring
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableMetricsStreaming': True,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True,
    'enableCustomAlerts': True,
    'alertConditions': [
        {
            'type': 'performance',
            'metric': 'memory_usage',
            'threshold': '1GB',
            'action': 'log_warning'
        },
        {
            'type': 'learning',
            'metric': 'learning_rate',
            'threshold': '0.0001',
            'action': 'raise_alert'
        }
    ]
})

# Enable debugging for specific neurons
pynlm.python.MonitoringSystem.enableSpikeTracing(brain, neuron_ids=[1, 2, 3, 4, 5])

# Start monitoring
pynlm.python.MonitoringSystem.startMetricsStreaming(brain)
pynlm.python.MonitoringSystem.startVisualization({
    'host': 'localhost',
    'port': 8080
})

# Run simulation with detailed logging
for episode in range(20):
    state = generate_state()
    
    for step in range(200):
        # Get current metrics
        metrics = pynlm.python.MonitoringSystem.getCurrentMetrics(brain)
        print(f"Episode {episode}, Step {step}: "
              f"Metrics {metrics}")
        
        # Select action
        action = advanced_agent.selectAction(state)
        
        # Get action statistics
        action_stats = pynlm.python.MonitoringSystem.getActionStatistics(brain)
        print(f"Action statistics: {action_stats}")
        
        # Compute reward
        reward = advanced_agent.computeCustomReward(state, action, state)
        
        # Add experience
        advanced_agent.addExperience(state, action, reward, state, False)
        
        # Perform learning step
        advanced_agent.performRLStep()
        
        # Get learning statistics
        learning_stats = pynlm.python.MonitoringSystem.getLearningStatistics(brain)
        print(f"Learning statistics: {learning_stats}")
        
        # Get performance profile
        performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)
        print(f"Performance profile: {performance_profile}")
        
        # Update state
        state = next_state(state)
        
        # Sleep for monitoring interval
        import time
        time.sleep(0.1)

# Stop monitoring
pynlm.python.MonitoringSystem.stopMetricsStreaming(brain)
pynlm.python.MonitoringSystem.stopVisualization(brain)

# Generate final reports
final_stats = pynlm.python.MonitoringSystem.getFinalStatistics(brain)
performance_analysis = pynlm.python.MonitoringSystem.analyzePerformance(brain)

print(f"Final statistics: {final_stats}")
print(f"Performance analysis: {performance_analysis}")
```

### Example 3: Performance Debugging

```python
import pynlm

# Create brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 5000,
    'enable_plasticity': True,
    'enable_neuromodulation': True
})

# Create brain
brain = pynlm.createBrain(brain_config)

# Initialize performance monitoring
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enablePerformanceProfiling': True,
    'enableMetricsStreaming': True
})

# Start simulation
pynlm.python.MonitoringSystem.startSimulation(brain)

# Profile simulation performance
performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

# Analyze performance bottlenecks
bottlenecks = pynlm.python.MonitoringSystem.analyzePerformanceBottlenecks(brain)

# Get optimization recommendations
recommendations = pynlm.python.MonitoringSystem.getOptimizationRecommendations(performance_profile)

# Apply optimizations
pynlm.python.PerformanceOptimizer.applyOptimizations(brain, recommendations)

# Run simulation
for episode in range(10):
    for step in range(500):
        # Simulate neural activity
        brain.simulateStep()

# Stop simulation
pynlm.python.MonitoringSystem.stopSimulation(brain)

# Analyze results
final_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

print(f"Final performance profile: {final_profile}")
print(f"Optimization improvements: {recommendations}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: No Metrics Output
**Solution**: Ensure monitoring system is initialized and started.

```python
# Check if monitoring is initialized
if not pynlm.python.MonitoringSystem.isInitialized():
    print("Monitoring system not initialized")
    # Initialize monitoring
    pynlm.python.MonitoringSystem.initializeMonitoring({...})
```

#### Issue: High Memory Usage
**Solution**: Optimize memory usage and check for memory leaks.

```python
# Optimize memory usage
pynlm.python.PerformanceOptimizer.optimizeMemoryUsage(brain)

# Check memory usage
memory_usage = pynlm.python.MonitoringSystem.getMemoryUsage(brain)
print(f"Current memory usage: {memory_usage}")
```

#### Issue: Poor Performance
**Solution**: Profile performance and apply optimizations.

```python
# Profile performance
performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

# Analyze bottlenecks
bottlenecks = pynlm.python.MonitoringSystem.analyzePerformanceBottlenecks(brain)

# Apply optimizations
pynlm.python.PerformanceOptimizer.applyOptimizations(brain, bottlenecks)
```

#### Issue: Incorrect Activity Patterns
**Solution**: Analyze activity patterns and adjust simulation parameters.

```python
# Get activity patterns
activity_patterns = pynlm.python.MonitoringSystem.getActivityPatterns(brain)

# Analyze pattern characteristics
for pattern in activity_patterns:
    print(f"Pattern: {pattern['name']}")
    print(f"  Strength: {pattern['strength']}")
    print(f"  Duration: {pattern['duration']}")
```

#### Issue: Simulation Failures
**Solution**: Use debugging tools to identify and fix issues.

```python
# Enable detailed logging
pynlm.python.MonitoringSystem.initializeMonitoring({
    'log_level': 'debug',
    'enableMetricsStreaming': True,
    'enablePerformanceProfiling': True
})

# Run simulation with debugging
try:
    pynlm.python.MonitoringSystem.startSimulation(brain)
    
    for episode in range(10):
        for step in range(100):
            # Simulate with error handling
            try:
                brain.simulateStep()
            except Exception as e:
                print(f"Error at episode {episode}, step {step}: {e}")
                # Take corrective action
                handle_error(e)
    
    pynlm.python.MonitoringSystem.stopSimulation(brain)
    
except Exception as e:
    print(f"Simulation failed: {e}")
    # Stop simulation
    pynlm.python.MonitoringSystem.stopSimulation(brain)
    raise
```

### Getting Help

#### Debug Information
- **Current Metrics**: `pynlm.python.MonitoringSystem.getCurrentMetrics(brain)`
- **Performance Profile**: `pynlm.python.MonitoringSystem.getPerformanceProfile(brain)`
- **Learning Statistics**: `pynlm.python.MonitoringSystem.getLearningStatistics(brain)`
- **Activity Patterns**: `pynlm.python.MonitoringSystem.getActivityPatterns(brain)`
- **Error Statistics**: `pynlm.python.MonitoringSystem.getErrorStatistics(brain)`
- **Alert Status**: `pynlm.python.MonitoringSystem.getAlertStatus(brain)`

#### Help Commands
- **Help Documentation**: `pynlm.python.MonitoringSystem.showHelp()`
- **Debug Information**: `pynlm.python.MonitoringSystem.showDebugInfo()`
- **System Status**: `pynlm.python.MonitoringSystem.showSystemStatus()`

#### Support Resources
- **Documentation**: `/workspace/app/docs/debugging.md`
- **Python API**: `/workspace/app/docs/python_api.md`
- **Examples**: `/workspace/app/docs/examples/debugging_examples.md`

## Conclusion

The NLM debugging and monitoring system provides comprehensive tools for debugging neural network simulations, monitoring performance, and analyzing system behavior. Key features include:

1. **Real-time Monitoring**: Live metrics collection and visualization
2. **Performance Profiling**: Detailed performance analysis and optimization
3. **Activity Pattern Analysis**: Discovery and analysis of neural activity patterns
4. **Learning Statistics**: Tracking and analysis of learning progress
5. **Error Analysis**: Detection and analysis of simulation errors
6. **Alerting System**: Real-time notifications of issues and problems
7. **Debugging Tools**: Comprehensive debugging utilities for various components
8. **Custom Alerts**: User-defined alert conditions and actions

The debugging and monitoring system is designed to support the full range of NLM capabilities, from simple demos to large-scale research simulations, while providing comprehensive visibility into system behavior and performance.

For more information, refer to the specific sections of this documentation or consult the performance optimization documentation for detailed implementation details.

Happy debugging and monitoring with NLM's advanced debugging system! 🧠✨