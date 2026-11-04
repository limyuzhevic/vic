# Logging Module

This module provides comprehensive logging utilities for the enhanced NLM Python API, including structured logging, configurable log levels, and advanced logging capabilities for simulation monitoring and debugging.

## Overview

The logging module extends the basic NLM API with advanced logging capabilities, enabling comprehensive monitoring, debugging, and analysis of simulation data. It provides:

- **Structured logging** with configurable levels and output formats
- **Brain state logging** for detailed neural activity tracking
- **Step-by-step logging** for simulation progress monitoring
- **Performance logging** for optimization analysis
- **Error logging** with detailed context and recovery information
- **Anomaly detection** with automatic logging
- **Memory usage logging** for resource management
- **Custom event logging** for user-defined events

## Key Features

### Log Levels and Configuration

```python
from nlm_utils.logging import (
    AdvancedDebugLogger,
    LogLevel,
    DebugConfig,
    StepLogger,
    AnomalyDetector
)

# Configure logging with multiple output options
config = DebugConfig(
    log_level=LogLevel.DEBUG,                    # Minimum log level
    file_output=True,                           # Enable file logging
    console_output=True,                        # Enable console logging
    log_file="enhanced_simulation.log",         # Log file path
    max_file_size=100 * 1024 * 1024,           # 100MB max file size
    enable_brain_logging=True,                 # Enable brain state logging
    enable_step_logging=True,                  # Enable step-by-step logging
    enable_performance_logging=True,           # Enable performance logging
    enable_error_logging=True,                  # Enable error logging
    enable_memory_logging=True,                 # Enable memory usage logging
    structured_logging=True,                    # Enable structured logging
    log_format="json"                          # JSON log format
)

logger = AdvancedDebugLogger(config)
```

### Structured Logging

```python
# Log structured data with automatic timestamp and level
logger.log("SimulationStep", {
    "step": step,
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "memory_activity": brain.getMemoryActivity(),
    "development_stage": brain.getDevelopmentalStage(),
    "reward": reward,
    "curiosity": curiosity_level,
    "novelty": novelty_level,
    "timestamp": time.time()
})

# Log with specific severity levels
logger.log_debug("Debug information for troubleshooting")
logger.log_info("Simulation started successfully")
logger.log_warning("High memory usage detected")
logger.log_error("Brain step execution failed", {"step": step, "error": str(e)})
logger.log_critical("Critical error, simulation stopping")
```

### Brain State Logging

```python
# Log comprehensive brain state information
logger.log_brain_initialization(brain, config)

# Log brain state changes
logger.log_brain_state_change(step, old_state, new_state)

# Log brain state at specific steps
logger.log_brain_state(step, brain.get_state())
```

### Step-by-step Logging

```python
# Create step logger for detailed step tracking
step_logger = StepLogger("simulation_step_tracker")

@step_logger.log_step("simulation_step")
def run_step_logged_simulation(brain, steps):
    """Run simulation with comprehensive step logging."""
    for step in range(steps):
        # Log step start
        step_logger.log_step_start(step)
        
        # Run simulation step
        brain.step(step)
        
        # Log step metrics
        metrics = {
            'step_time': step_time,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'memory_usage': get_memory_usage(),
            'cpu_usage': get_cpu_usage()
        }
        
        # Log step end with metrics
        step_logger.log_step_end(step, metrics)
```

### Performance Logging

```python
# Log performance metrics for optimization analysis
logger.log_performance_metrics(step, {
    'step_time': step_time,
    'memory_usage': memory_usage,
    'cpu_usage': cpu_usage,
    'throughput': throughput,
    'efficiency': efficiency,
    'bottlenecks': bottlenecks
})

# Log performance benchmarks
benchmark = Benchmark("simulation_benchmark")
@benchmark
def run_performance_monitored_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)
    
    return {
        'benchmark_results': benchmark.get_results(),
        'performance_metrics': benchmark.get_metrics()
    }
```

### Memory Usage Logging

```python
# Log memory usage for resource management
logger.log_memory_usage(step, {
    'rss': rss_memory,
    'vms': vms_memory,
    'shared': shared_memory,
    'text': text_memory,
    'data': data_memory,
    'dirty': dirty_memory,
    'uspace': user_space,
    'dspace': data_space
})

# Monitor memory usage with alerts
memory_monitor = MemoryMonitor("simulation_memory")
@memory_monitor
def run_memory_monitored_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)
        
        # Check for memory alerts
        alerts = memory_monitor.check_alerts()
        if alerts:
            logger.log_memory_alerts(step, alerts)
```

### Error Logging and Recovery

```python
# Log errors with detailed context
logger.log_error("step_execution", str(error), {
    "step": step,
    "brain_state": brain.get_state(),
    "error_type": type(error).__name__,
    "timestamp": time.time(),
    "context": get_error_context()
})

# Log recovery actions
logger.log_recovery_action(step, recovery_action, {
    "previous_error": str(error),
    "recovery_time": recovery_time,
    "success": success,
    "recovery_method": recovery_method
})
```

## Advanced Logging Features

### 1. Structured Logging with JSON Format

```python
# Configure structured logging with JSON format
config = DebugConfig(
    log_level=LogLevel.INFO,
    file_output=True,
    console_output=True,
    structured_logging=True,
    log_format="json"
)

logger = AdvancedDebugLogger(config)

# Log structured events
logger.log_event("SimulationStarted", {
    "simulation_id": "sim_001",
    "brain_config": config.brain_config,
    "start_time": time.time(),
    "parameters": {
        "neuron_count": 1000,
        "learning_rate": 0.001,
        "plasticity_enabled": True
    }
})
```

### 2. Performance Analysis Logging

```python
from nlm_utils.logging import AdvancedDebugLogger, LogLevel, DebugConfig

# Configure performance logging
config = DebugConfig(
    log_level=LogLevel.INFO,
    enable_performance_logging=True,
    performance_log_file="performance.log"
)

logger = AdvancedDebugLogger(config)

@logger.log_performance("step_execution")
def run_performance_monitored_step(brain, step):
    """Run step with performance monitoring."""
    step_start = time.time()
    
    # Run simulation step
    brain.step(step)
    
    # Calculate performance metrics
    step_time = time.time() - step_start
    memory_usage = get_memory_usage()
    cpu_usage = get_cpu_usage()
    throughput = 1.0 / step_time if step_time > 0 else 0.0
    
    # Log performance metrics
    logger.log_performance_metrics(step, {
        'step_time': step_time,
        'memory_usage': memory_usage,
        'cpu_usage': cpu_usage,
        'throughput': throughput,
        'efficiency': calculate_efficiency(step, step_time, memory_usage)
    })
```

### 3. Anomaly Detection and Logging

```python
from nlm_utils.logging import (
    AdvancedDebugLogger,
    AnomalyDetector,
    LogLevel,
    DebugConfig
)

# Configure anomaly detection
config = DebugConfig(
    log_level=LogLevel.WARNING,
    enable_anomaly_detection=True,
    anomaly_detection_threshold=0.8
)

logger = AdvancedDebugLogger(config)
anomaly_detector = AnomalyDetector("anomaly_detector")

@logger.log_step("step_with_anomaly_detection")
def run_anomaly_aware_simulation(brain, steps):
    """Run simulation with anomaly detection."""
    for step in range(steps):
        # Capture brain metrics
        metrics = capture_brain_metrics(brain)
        
        # Detect anomalies
        anomalies = anomaly_detector.detect_anomalies(metrics)
        
        # Log anomalies
        if anomalies:
            logger.log_anomalies(step, anomalies)
            
            # Perform anomaly analysis
            anomaly_analysis = anomaly_detector.analyze_anomalies(anomalies)
            logger.log_anomaly_analysis(step, anomaly_analysis)
        
        # Run simulation step
        brain.step(step)
```

### 4. Custom Event Logging

```python
# Log custom events for user-defined tracking
logger.log_event("UserDefinedEvent", {
    "event_type": "experiment_completed",
    "experiment_name": "learning_experiment",
    "parameters": {
        "learning_rate": 0.001,
        "neuron_count": 500,
        "training_steps": 100
    },
    "results": {
        "final_performance": 0.95,
        "training_time": 120.5,
        "memory_usage": 2048.0
    },
    "timestamp": time.time()
})
```

### 5. Real-time Logging with Queues

```python
from nlm_utils.logging import (
    AdvancedDebugLogger,
    LogLevel,
    DebugConfig
)

# Configure real-time logging with queues
config = DebugConfig(
    log_level=LogLevel.INFO,
    enable_real_time_logging=True,
    log_queue_size=10000,
    batch_size=100,
    flush_interval=1.0
)

logger = AdvancedDebugLogger(config)

# Log with real-time batching
@logger.log_event("real_time_batch")
def log_real_time_events(events):
    """Log real-time events with batching."""
    for event in events:
        logger.log_event("SimulationEvent", event)
    
    # Flush logs periodically
    if time.time() % config.flush_interval < 0.1:
        logger.flush_logs()
```

## Integration Examples

### Enhanced Brain Simulation with Comprehensive Logging

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, LogLevel, DebugConfig
)

def run_enhanced_simulation_with_logging(brain_config, steps=1000):
    """Run enhanced simulation with comprehensive logging."""
    
    # Configure logging
    config = DebugConfig(
        log_level=LogLevel.DEBUG,
        file_output=True,
        console_output=True,
        log_file="enhanced_simulation.log",
        enable_brain_logging=True,
        enable_step_logging=True,
        enable_performance_logging=True,
        enable_error_logging=True,
        enable_memory_logging=True,
        structured_logging=True,
        log_format="json"
    )
    
    logger = AdvancedDebugLogger(config)
    
    # Create session
    session_config = SessionConfigAdvanced(
        name="enhanced_logging_simulation",
        brain_config=brain_config,
        checkpoint_interval=100,
        auto_save=True
    )
    
    session = AdvancedBrainSession(session_config)
    
    # Run enhanced simulation with logging
    for step in range(steps):
        try:
            # Run simulation step
            session.brain.step(step)
            
            # Log step metrics
            metrics = session.get_step_metrics(step)
            logger.log_step(step, metrics)
            
            # Log performance metrics
            if step % 100 == 0:
                performance_metrics = session.get_performance_metrics(step)
                logger.log_performance_metrics(step, performance_metrics)
            
            # Log brain state periodically
            if step % 500 == 0:
                brain_state = session.brain.get_state()
                logger.log_brain_state(step, brain_state)
                
        except Exception as e:
            # Log error with context
            logger.log_error("step_execution", str(e), {
                "step": step,
                "brain_state": session.brain.get_state(),
                "timestamp": time.time()
            })
    
    return {
        'simulation_complete': True,
        'steps_completed': steps,
        'log_file': config.log_file
    }
```

### Performance Monitoring with Logging

```python
from nlm_utils.logging import (
    AdvancedDebugLogger,
    Benchmark,
    MemoryMonitor,
    LogLevel,
    DebugConfig
)

def run_performance_monitored_simulation(brain_config, steps=1000):
    """Run simulation with performance monitoring and logging."""
    
    # Configure logging
    config = DebugConfig(
        log_level=LogLevel.INFO,
        enable_performance_logging=True,
        performance_log_file="performance.log"
    )
    
    logger = AdvancedDebugLogger(config)
    benchmark = Benchmark("performance_monitored_simulation", warmup_steps=10)
    memory_monitor = MemoryMonitor("simulation_memory")
    
    @benchmark
    @memory_monitor
    def run_performance_monitored_simulation(session, steps):
        """Run simulation with performance monitoring."""
        for step in range(steps):
            # Run step
            session.brain.step(step)
            
            # Log performance metrics
            performance_metrics = session.get_performance_metrics(step)
            logger.log_performance_metrics(step, performance_metrics)
            
            # Check memory usage
            memory_info = memory_monitor.get_current_usage()
            logger.log_memory_usage(step, memory_info)
        
        return {
            'benchmark_results': benchmark.get_results(),
            'memory_stats': memory_monitor.get_summary()
        }
    
    # Create session and run simulation
    config = SessionConfigAdvanced(
        name="performance_monitored",
        brain_config=brain_config,
        checkpoint_interval=100
    )
    
    session = AdvancedBrainSession(config)
    
    return run_performance_monitored_simulation(session, steps)
```

## Best Practices

### 1. Configure Logging Early

Configure logging at the start of your application:

```python
from nlm_utils.logging import AdvancedDebugLogger, LogLevel, DebugConfig

# Configure logging
config = DebugConfig(
    log_level=LogLevel.INFO,
    file_output=True,
    console_output=True,
    enable_brain_logging=True,
    enable_step_logging=True
)

logger = AdvancedDebugLogger(config)
```

### 2. Use Structured Logging

Use structured logging for better analysis:

```python
# Log structured data
logger.log("SimulationStep", {
    "step": step,
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "memory_activity": brain.getMemoryActivity(),
    "timestamp": time.time()
})
```

### 3. Implement Performance Logging

Log performance metrics for optimization:

```python
# Log performance metrics
logger.log_performance_metrics(step, {
    'step_time': step_time,
    'memory_usage': memory_usage,
    'cpu_usage': cpu_usage,
    'throughput': throughput
})
```

### 4. Log Brain State Changes

Log brain state changes for debugging:

```python
# Log brain state changes
logger.log_brain_state_change(step, old_state, new_state)
```

### 5. Log Errors with Context

Log errors with sufficient context:

```python
# Log errors with context
logger.log_error("operation_name", str(error), {
    "step": step,
    "context": get_context(),
    "timestamp": time.time()
})
```

## Performance Considerations

### Logging Overhead

- Logging has minimal overhead when configured appropriately
- Configure log levels to reduce overhead in production
- Use structured logging for better performance

### File Logging

- Configure log file sizes and rotation to manage disk usage
- Use append mode for efficient file logging
- Consider log rotation for long-running simulations

### Memory Usage

- Log objects store context; use sparingly in long-running simulations
- Implement log buffering for better performance
- Use weak references for context to avoid memory leaks

## Migration Guide

### From Basic Logging to Enhanced Logging

```python
# Basic logging
import logging
logging.basicConfig(level=logging.INFO)
logging.info("Simulation step %d", step)

# Enhanced logging
from nlm_utils.logging import AdvancedDebugLogger, LogLevel

# Configure logging
config = DebugConfig(
    log_level=LogLevel.INFO,
    file_output=True,
    console_output=True
)

logger = AdvancedDebugLogger(config)

# Log structured data
logger.log("SimulationStep", {
    "step": step,
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount()
})
```

### Adding Brain State Logging

```python
# Basic logging
logging.info("Step %d completed", step)

# Enhanced brain state logging
from nlm_utils.logging import AdvancedDebugLogger

# Configure logging
config = DebugConfig(
    log_level=LogLevel.DEBUG,
    enable_brain_logging=True
)

logger = AdvancedDebugLogger(config)

# Log brain state changes
logger.log_brain_state_change(step, old_state, new_state)
```

### Adding Performance Logging

```python
# Basic performance logging
import time
start_time = time.time()
brain.step(step)
step_time = time.time() - start_time
logging.info("Step %d took %f seconds", step, step_time)

# Enhanced performance logging
from nlm_utils.logging import AdvancedDebugLogger

# Configure logging
config = DebugConfig(
    log_level=LogLevel.INFO,
    enable_performance_logging=True
)

logger = AdvancedDebugLogger(config)

# Log performance metrics
logger.log_performance_metrics(step, {
    'step_time': step_time,
    'memory_usage': get_memory_usage(),
    'cpu_usage': get_cpu_usage()
})
```

## Conclusion

The logging module provides comprehensive logging capabilities for NLM applications. It enables:

- **Structured logging** with configurable levels and output formats
- **Brain state logging** for detailed neural activity tracking
- **Step-by-step logging** for simulation progress monitoring
- **Performance logging** for optimization analysis
- **Error logging** with detailed context and recovery information
- **Anomaly detection** with automatic logging
- **Memory usage logging** for resource management
- **Custom event logging** for user-defined events

These enhancements make NLM applications more observable and easier to debug while maintaining backward compatibility.

For more advanced logging features, see the other utility modules in the nlm_utils package.
