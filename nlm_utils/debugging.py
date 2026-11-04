# Debugging Module

This module provides comprehensive debugging and brain state examination tools for the enhanced NLM Python API.

## Overview

The debugging module extends the basic NLM API with advanced debugging capabilities, including brain state examination, anomaly detection, performance analysis, and comprehensive debugging utilities. It provides:

- **Brain state examination** tools for detailed analysis
- **Anomaly detection** with automatic identification of unusual patterns
- **Performance analysis** tools for optimization
- **Debugging utilities** for step-by-step debugging
- **Visualization helpers** for brain data visualization
- **State comparison** tools for analyzing changes over time
- **Memory analysis** tools for memory usage investigation

## Key Features

### Brain State Analysis

```python
from nlm_utils.debugging import BrainStateAnalyzer, DebugLevel, StateSnapshot

# Create brain state analyzer
analyzer = BrainStateAnalyzer("brain_state_tracker")

# Capture comprehensive brain state
snapshot = analyzer.capture_comprehensive_state(brain)

# Analyze neural dynamics
neural_analysis = analyzer.analyze_neural_dynamics(snapshot)

# Analyze plasticity dynamics
plasticity_analysis = analyzer.analyze_plasticity_dynamics(snapshot)

# Analyze memory state
memory_analysis = analyzer.analyze_memory_state(snapshot)

# Analyze development state
development_analysis = analyzer.analyze_development_state(snapshot)

# Analyze neuromodulation state
neuromodulation_analysis = analyzer.analyze_neuromodulation_state(snapshot)

# Analyze cognitive state
cognitive_analysis = analyzer.analyze_cognitive_state(snapshot)
```

### State Change Analysis

```python
from nlm_utils.debugging import (
    BrainStateAnalyzer,
    StateChangeDetector,
    LogLevel,
    DebugConfig
)

# Create state change detector
detector = StateChangeDetector("state_change_tracker")

# Capture state changes
old_state = analyzer.capture_comprehensive_state(brain)

# Run step
brain.step(step)

# Capture new state
new_state = analyzer.capture_comprehensive_state(brain)

# Analyze state changes
state_changes = detector.analyze_state_changes(old_state, new_state)

# Log state changes
detector.log_state_changes(step, state_changes)
```

### Anomaly Detection

```python
from nlm_utils.debugging import (
    AnomalyDetector,
    LogLevel,
    DebugConfig
)

# Create anomaly detector
anomaly_detector = AnomalyDetector("anomaly_detector")

# Capture brain state
snapshot = analyzer.capture_comprehensive_state(brain)

# Detect anomalies
anomalies = anomaly_detector.detect_anomalies(snapshot)

# Log anomalies
if anomalies:
    detector.log_anomalies(step, anomalies)
```

### Performance Analysis

```python
from nlm_utils.debugging import (
    PerformanceAnalyzer,
    LogLevel,
    DebugConfig
)

# Create performance analyzer
performance_analyzer = PerformanceAnalyzer("performance_analyzer")

# Capture performance metrics
performance_metrics = analyzer.capture_performance_metrics(brain)

# Analyze performance
performance_analysis = performance_analyzer.analyze_performance(performance_metrics)

# Log performance analysis
performance_analyzer.log_performance_analysis(step, performance_analysis)
```

## Advanced Debugging Features

### 1. Comprehensive Brain State Analysis

```python
from nlm_utils.debugging import (
    BrainStateAnalyzer,
    LogLevel,
    DebugConfig
)

# Configure debugging
config = DebugConfig(
    log_level=LogLevel.DEBUG,
    enable_brain_analysis=True,
    brain_analysis_file="brain_analysis.log"
)

analyzer = BrainStateAnalyzer("comprehensive_analyzer")

@analyzer.analyze_brain_state("step_analysis")
def run_brain_state_analyzed_simulation(brain, steps):
    """Run simulation with comprehensive brain state analysis."""
    for step in range(steps):
        # Capture comprehensive brain state
        comprehensive_state = analyzer.capture_comprehensive_state(brain)
        
        # Perform analysis
        analyses = {
            'neural_dynamics': analyzer.analyze_neural_dynamics(comprehensive_state),
            'plasticity': analyzer.analyze_plasticity_dynamics(comprehensive_state),
            'memory': analyzer.analyze_memory_state(comprehensive_state),
            'development': analyzer.analyze_development_state(comprehensive_state),
            'neuromodulation': analyzer.analyze_neuromodulation_state(comprehensive_state),
            'cognition': analyzer.analyze_cognitive_state(comprehensive_state)
        }
        
        # Log comprehensive analysis
        analyzer.log_comprehensive_analysis(step, analyses)
        
        # Run step
        brain.step(step)
        
        # Log performance metrics
        performance_metrics = analyzer.capture_performance_metrics(brain)
        analyzer.log_performance_metrics(step, performance_metrics)
```

### 2. Anomaly Detection and Analysis

```python
from nlm_utils.debugging import (
    AnomalyDetector,
    BrainStateAnalyzer,
    LogLevel,
    DebugConfig
)

# Configure anomaly detection
config = DebugConfig(
    log_level=LogLevel.WARNING,
    enable_anomaly_detection=True,
    anomaly_detection_threshold=0.8
)

anomaly_detector = AnomalyDetector("anomaly_detector")
brain_analyzer = BrainStateAnalyzer("brain_analyzer")

@anomaly_detector.analyze_for_anomalies("step_anomaly_detection")
def run_anomaly_aware_simulation(brain, steps):
    """Run simulation with anomaly detection."""
    for step in range(steps):
        # Capture brain state
        snapshot = brain_analyzer.capture_comprehensive_state(brain)
        
        # Detect anomalies
        anomalies = anomaly_detector.detect_anomalies(snapshot)
        
        # Log anomalies
        if anomalies:
            anomaly_detector.log_anomalies(step, anomalies)
            
            # Perform anomaly analysis
            anomaly_analysis = anomaly_detector.analyze_anomalies(anomalies)
            anomaly_detector.log_anomaly_analysis(step, anomaly_analysis)
        
        # Run step
        brain.step(step)
```

### 3. State Comparison and Evolution Analysis

```python
from nlm_utils.debugging import (
    StateChangeDetector,
    LogLevel,
    DebugConfig
)

# Configure state comparison
config = DebugConfig(
    log_level=LogLevel.INFO,
    enable_state_comparison=True,
    state_comparison_file="state_evolution.log"
)

state_detector = StateChangeDetector("state_evolution_tracker")

@state_detector.track_state_changes("state_evolution")
def run_state_evolution_analyzed_simulation(brain, steps):
    """Run simulation with state evolution analysis."""
    # Capture initial state
    initial_state = state_detector.capture_state(brain)
    
    # Log initial state
    state_detector.log_initial_state(initial_state)
    
    for step in range(steps):
        # Capture state before step
        state_before = state_detector.capture_state(brain)
        
        # Run step
        brain.step(step)
        
        # Capture state after step
        state_after = state_detector.capture_state(brain)
        
        # Analyze state changes
        state_changes = state_detector.analyze_state_changes(state_before, state_after)
        
        # Log state changes
        state_detector.log_state_changes(step, state_changes)
        
        # Detect patterns in state changes
        pattern_analysis = state_detector.analyze_change_patterns(step, state_changes)
        state_detector.log_pattern_analysis(step, pattern_analysis)
```

### 4. Memory Analysis and Debugging

```python
from nlm_utils.debugging import (
    MemoryAnalyzer,
    LogLevel,
    DebugConfig
)

# Configure memory analysis
config = DebugConfig(
    log_level=LogLevel.INFO,
    enable_memory_analysis=True,
    memory_analysis_file="memory_debug.log"
)

memory_analyzer = MemoryAnalyzer("memory_debugger")

@memory_analyzer.analyze_memory_usage("memory_debug")
def run_memory_analyzed_simulation(brain, steps):
    """Run simulation with memory analysis."""
    for step in range(steps):
        # Capture memory metrics
        memory_metrics = memory_analyzer.capture_memory_metrics(brain)
        
        # Analyze memory usage
        memory_analysis = memory_analyzer.analyze_memory_usage(memory_metrics)
        memory_analyzer.log_memory_analysis(step, memory_analysis)
        
        # Detect memory leaks
        leaks = memory_analyzer.detect_memory_leaks()
        if leaks:
            memory_analyzer.log_memory_leaks(step, leaks)
        
        # Detect memory patterns
        patterns = memory_analyzer.detect_memory_patterns(memory_metrics)
        memory_analyzer.log_memory_patterns(step, patterns)
        
        # Run step
        brain.step(step)
```

### 5. Performance Bottleneck Detection

```python
from nlm_utils.debugging import (
    PerformanceBottleneckDetector,
    LogLevel,
    DebugConfig
)

# Configure bottleneck detection
config = DebugConfig(
    log_level=LogLevel.WARNING,
    enable_bottleneck_detection=True,
    bottleneck_detection_file="bottleneck.log"
)

bottleneck_detector = PerformanceBottleneckDetector("bottleneck_detector")

@bottleneck_detector.track_performance("bottleneck_detection")
def run_bottleneck_aware_simulation(brain, steps):
    """Run simulation with bottleneck detection."""
    for step in range(steps):
        # Capture performance metrics
        performance_metrics = bottleneck_detector.capture_performance_metrics(brain)
        
        # Detect performance bottlenecks
        bottlenecks = bottleneck_detector.detect_bottlenecks(performance_metrics)
        
        # Log bottlenecks
        if bottlenecks:
            bottleneck_detector.log_bottlenecks(step, bottlenecks)
            
            # Perform bottleneck analysis
            bottleneck_analysis = bottleneck_detector.analyze_bottlenecks(bottlenecks)
            bottleneck_detector.log_bottleneck_analysis(step, bottleneck_analysis)
        
        # Run step
        brain.step(step)
```

## Integration Examples

### Enhanced Brain Analysis with Comprehensive Debugging

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, BrainStateAnalyzer,
    AnomalyDetector, PerformanceAnalyzer
)

def run_enhanced_brain_analysis_simulation(brain_config, steps=1000):
    """Run enhanced simulation with comprehensive brain analysis."""
    
    # Configure debugging
    config = DebugConfig(
        log_level=LogLevel.DEBUG,
        enable_brain_analysis=True,
        enable_anomaly_detection=True,
        enable_performance_analysis=True,
        enable_memory_analysis=True,
        analysis_file="enhanced_brain_analysis.log"
    )
    
    logger = AdvancedDebugLogger(config)
    brain_analyzer = BrainStateAnalyzer("brain_analysis")
    anomaly_detector = AnomalyDetector("brain_anomalies")
    performance_analyzer = PerformanceAnalyzer("brain_performance")
    
    # Run enhanced simulation with analysis
    for step in range(steps):
        try:
            # Capture comprehensive brain state
            comprehensive_state = brain_analyzer.capture_comprehensive_state(brain)
            
            # Perform analysis
            analyses = {
                'neural_dynamics': brain_analyzer.analyze_neural_dynamics(comprehensive_state),
                'plasticity': brain_analyzer.analyze_plasticity_dynamics(comprehensive_state),
                'memory': brain_analyzer.analyze_memory_state(comprehensive_state),
                'development': brain_analyzer.analyze_development_state(comprehensive_state),
                'neuromodulation': brain_analyzer.analyze_neuromodulation_state(comprehensive_state),
                'cognition': brain_analyzer.analyze_cognitive_state(comprehensive_state)
            }
            
            # Log comprehensive analysis
            logger.log_comprehensive_analysis(step, analyses)
            
            # Detect anomalies
            anomalies = anomaly_detector.detect_anomalies(comprehensive_state)
            if anomalies:
                logger.log_anomalies(step, anomalies)
            
            # Perform performance analysis
            performance_metrics = brain_analyzer.capture_performance_metrics(brain)
            performance_analysis = performance_analyzer.analyze_performance(performance_metrics)
            logger.log_performance_analysis(step, performance_analysis)
            
            # Run step
            brain.step(step)
            
        except Exception as e:
            # Log error with context
            logger.log_error("step_execution", str(e), {
                "step": step,
                "brain_state": brain.get_state(),
                "timestamp": time.time()
            })
    
    return {
        'simulation_complete': True,
        'steps_completed': steps,
        'analysis_file': config.analysis_file
    }
```

### Step-by-Step Debugging with State Tracking

```python
from nlm_utils.debugging import (
    AdvancedDebugLogger,
    StateChangeDetector,
    LogLevel,
    DebugConfig
)

def run_step_by_step_debugging_simulation(brain_config, steps=1000):
    """Run simulation with step-by-step debugging and state tracking."""
    
    # Configure debugging
    config = DebugConfig(
        log_level=LogLevel.INFO,
        enable_state_tracking=True,
        enable_step_by_step_logging=True,
        state_tracking_file="step_debug.log"
    )
    
    logger = AdvancedDebugLogger(config)
    state_detector = StateChangeDetector("step_debugger")
    
    # Run step-by-step simulation with debugging
    for step in range(steps):
        # Capture state before step
        state_before = state_detector.capture_state(brain)
        
        # Log step start
        logger.log_step_start(step)
        
        try:
            # Run step
            brain.step(step)
            
            # Capture state after step
            state_after = state_detector.capture_state(brain)
            
            # Analyze state changes
            state_changes = state_detector.analyze_state_changes(state_before, state_after)
            
            # Log step details
            logger.log_step_details(step, {
                'state_changes': state_changes,
                'duration': time.time() - step_start,
                'memory_usage': get_memory_usage(),
                'cpu_usage': get_cpu_usage()
            })
            
            # Log successful step
            logger.log_step_success(step)
            
        except Exception as e:
            # Log error with context
            logger.log_step_error(step, str(e), type(e).__name__)
            
            # Attempt recovery
            try:
                perform_recovery(brain, step)
                logger.log_recovery_success(step)
            except Exception as recovery_error:
                logger.log_recovery_failure(step, str(recovery_error))
                raise
    
    return {
        'simulation_complete': True,
        'steps_completed': steps,
        'state_tracking_file': config.state_tracking_file
    }
```

## Best Practices

### 1. Configure Comprehensive Debugging Early

Configure debugging at the start of your application:

```python
from nlm_utils.debugging import (
    BrainStateAnalyzer, AnomalyDetector, PerformanceAnalyzer,
    LogLevel, DebugConfig
)

# Configure comprehensive debugging
config = DebugConfig(
    log_level=LogLevel.DEBUG,
    enable_brain_analysis=True,
    enable_anomaly_detection=True,
    enable_performance_analysis=True,
    enable_memory_analysis=True
)
```

### 2. Use State Change Detection

Use state change detection to track evolution:

```python
from nlm_utils.debugging import StateChangeDetector

# Create state change detector
detector = StateChangeDetector("state_tracker")

# Capture initial state
initial_state = detector.capture_state(brain)

# Run simulation for steps
for step in range(steps):
    brain.step(step)
    
    # Capture final state
    final_state = detector.capture_state(brain)
    
    # Analyze state changes
    state_changes = detector.analyze_state_changes(initial_state, final_state)
    
    # Log state changes
    detector.log_state_changes(step, state_changes)
```

### 3. Implement Anomaly Detection

Implement anomaly detection for early problem detection:

```python
from nlm_utils.debugging import AnomalyDetector

# Create anomaly detector
anomaly_detector = AnomalyDetector("anomaly_detector")

# Detect anomalies in brain state
snapshot = brain_analyzer.capture_comprehensive_state(brain)
anomalies = anomaly_detector.detect_anomalies(snapshot)

# Log anomalies if detected
if anomalies:
    anomaly_detector.log_anomalies(step, anomalies)
```

### 4. Log State Changes for Debugging

Log state changes for better debugging:

```python
from nlm_utils.debugging import StateChangeDetector

# Create state change detector
detector = StateChangeDetector("state_debugger")

# Log state changes
logger.log_state_change(step, old_state, new_state)
```

### 5. Analyze Performance for Optimization

Analyze performance data for optimization:

```python
from nlm_utils.debugging import PerformanceAnalyzer

# Create performance analyzer
performance_analyzer = PerformanceAnalyzer("performance_analyzer")

# Analyze performance metrics
performance_metrics = performance_analyzer.capture_performance_metrics(brain)
performance_analysis = performance_analyzer.analyze_performance(performance_metrics)
```

## Performance Considerations

### Debugging Overhead

- Debugging has minimal overhead when configured appropriately
- Configure debug levels to reduce overhead in production
- Use efficient data structures for state tracking

### State Tracking

- State tracking uses efficient algorithms with minimal overhead
- Configure state tracking frequency based on requirements
- Use incremental state tracking for better performance

### Memory Usage

- State tracking uses memory efficiently
- Configure state retention based on memory constraints
- Use state compression for long-running simulations

## Migration Guide

### From Basic Simulation to Enhanced Debugging

```python
# Basic simulation
for step in range(1000):
    brain.step(step)

# Enhanced simulation with debugging
from nlm_utils import AdvancedBrainSession, SessionConfigAdvanced, BrainStateAnalyzer

# Configure debugging
config = DebugConfig(
    log_level=LogLevel.DEBUG,
    enable_brain_analysis=True,
    enable_anomaly_detection=True
)

brain_analyzer = BrainStateAnalyzer("brain_analysis")

# Run simulation with brain analysis
for step in range(1000):
    # Capture brain state
    snapshot = brain_analyzer.capture_comprehensive_state(brain)
    
    # Perform analysis
    analyses = brain_analyzer.analyze_neural_dynamics(snapshot)
    
    # Run step
    brain.step(step)
```

### Adding State Change Analysis

```python
# Basic simulation
for step in range(1000):
    brain.step(step)

# Enhanced simulation with state change analysis
from nlm_utils import AdvancedBrainSession, SessionConfigAdvanced, StateChangeDetector

# Create state change detector
detector = StateChangeDetector("state_tracker")

# Capture initial state
initial_state = detector.capture_state(brain)

# Run simulation with state tracking
for step in range(1000):
    brain.step(step)
    
    # Capture final state
    final_state = detector.capture_state(brain)
    
    # Analyze state changes
    state_changes = detector.analyze_state_changes(initial_state, final_state)
```

### Adding Anomaly Detection

```python
# Basic simulation
for step in range(1000):
    brain.step(step)

# Enhanced simulation with anomaly detection
from nlm_utils import AdvancedBrainSession, SessionConfigAdvanced, AnomalyDetector

# Create anomaly detector
anomaly_detector = AnomalyDetector("anomaly_detector")

# Run simulation with anomaly detection
for step in range(1000):
    # Capture brain state
    snapshot = brain_analyzer.capture_comprehensive_state(brain)
    
    # Detect anomalies
    anomalies = anomaly_detector.detect_anomalies(snapshot)
    
    # Log anomalies
    if anomalies:
        anomaly_detector.log_anomalies(step, anomalies)
```

## Conclusion

The debugging module provides comprehensive debugging and brain state examination capabilities for NLM applications. It enables:

- **Comprehensive brain state analysis** for detailed understanding
- **Anomaly detection** for early problem identification
- **State change tracking** for evolution analysis
- **Performance analysis** for optimization
- **Memory analysis** for resource management
- **Performance bottleneck detection** for performance tuning

These enhancements make NLM applications more observable and easier to debug while maintaining backward compatibility.

For more advanced debugging features, see the other utility modules in the nlm_utils package.
