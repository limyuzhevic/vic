# NLM (Neural Learning Machine) - Enhanced Python API Documentation

## Complete API Reference Guide

This document provides comprehensive documentation for the NLM Python API, including advanced features, error handling, performance optimization, and debugging utilities.

## Overview

NLM is a brain-inspired spiking neural network simulator that allows you to create computational brains with neurons and synapses. The enhanced API provides extensive utilities for simulation, monitoring, and experimentation.

## Module Structure

The enhanced NLM Python API consists of several modules:

### Core Modules (`pynlm`)
- **Brain**: Main neural simulation brain class
- **Config**: Advanced configuration system
- **AgentBrain**: Bridge connecting brain to world
- **SimpleWorld**: Simple 2D world environment
- **SensoryInput**: Base class for sensory data
- **Action**: Motor command representation

### Utility Modules (`nlm_utils`)
- **error_handling**: Comprehensive error handling and validation
- **performance**: Benchmarking and memory monitoring
- **profiling**: Performance profiling tools
- **logging**: Enhanced logging capabilities
- **debugging**: Brain state examination tools
- **visualization**: Basic visualization helpers
- **experiment**: Experiment runner utilities
- **session**: Session management tools

## Quick Start Guide

### 1. Installation

```bash
# Install NLM with utilities
pip install pynlm nlm-utils

# Or install in development mode
pip install -e .
pip install -e ./utils
```

### 2. Basic Usage

```python
import pynlm
from nlm_utils import createBrain, createSimpleWorld, run_simulation

# Create a brain with enhanced utilities
brain = createBrain({
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.0
})

# Run a simulation with monitoring
stats = run_simulation(brain, steps=1000, monitor=True)
print(f"Simulation complete: {stats['total_spikes']} spikes")
```

## Enhanced Error Handling and Validation

### Module: `nlm_utils.error_handling`

The error handling module provides comprehensive error handling with meaningful error messages and validation utilities.

#### Key Classes and Functions

```python
from nlm_utils.error_handling import (
    NLMError,
    ConfigurationError,
    BrainInitializationError,
    ValidationError,
    safe_function,
    validate_config,
    validate_brain_state,
    validate_brain_config,
    BrainStateValidator,
    ConfigurationValidator
)

# Example usage

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Get specific error details
    print(f"Invalid parameter: {e.parameter}")
    print(f"Error code: {e.error_code}")

# Validation utilities
if validate_brain_config(config):
    print("Configuration is valid")
else:
    print("Configuration has errors:", config.getErrors())

# Safe wrapper functions
@safe_function("brain_step", retries=3, timeout=1000)
def step_brain(brain, step):
    brain.step(step)
    return True
```

#### Error Types

- **NLMError**: Base error class for all NLM errors
- **ConfigurationError**: Errors in configuration
- **BrainInitializationError**: Errors during brain initialization
- **ValidationError**: Validation errors
- **SimulationError**: Simulation-related errors
- **MemoryError**: Memory-related errors
- **PerformanceError**: Performance-related errors

### Error Handling Best Practices

```python
def robust_simulation(brain, world, agent, steps):
    """Robust simulation with comprehensive error handling."""
    from nlm_utils.error_handling import (
        NLMError,
        safe_function,
        validate_brain_config
    )
    
    errors = []
    success_count = 0
    
    @safe_function("simulation_step", max_retries=3)
    def run_step(step):
        nonlocal success_count
        try:
            # Validate state before step
            if not validate_brain_state(brain):
                raise ValidationError("Invalid brain state for step")
            
            # Run simulation
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            success_count += 1
            return True
            
        except NLMError as e:
            errors.append({
                'step': step,
                'error': str(e),
                'type': type(e).__name__
            })
            raise
    
    for step in range(steps):
        try:
            run_step(step)
        except NLMError:
            # Continue simulation even if some steps fail
            continue
    
    return {
        'success_rate': success_count / steps,
        'total_errors': len(errors),
        'errors': errors,
        'successful_steps': success_count
    }
```

## Performance Optimization Utilities

### Module: `nlm_utils.performance`

The performance module provides benchmarking tools, memory monitoring, and performance profiling capabilities.

#### Key Classes and Functions

```python
from nlm_utils.performance import (
    Benchmark,
    MemoryMonitor,
    Profiler,
    PerformanceProfiler,
    benchmark_simulation,
    profile_brain_step,
    monitor_memory_usage,
    PerformanceMetrics
)

# Example usage with benchmarking

# Create a benchmark for simulation performance
benchmark = Benchmark("simulation_benchmark", warmup_steps=10)

@benchmark
@profile_brain_step("step_execution")
def run_monitored_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)

# Run the benchmark
results = benchmark.run(run_monitored_simulation, 1000)
print(f"Benchmark results: {results}")
print(f"Average step time: {results['avg_time_ms']:.2f} ms")

# Memory monitoring
monitor = MemoryMonitor("simulation_memory", interval=0.1)

@monitor
def run_memory_monitored_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)
        if step % 100 == 0:
            mem_info = monitor.get_current_usage()
            print(f"Step {step}: Memory usage: {mem_info['rss_mb']:.2f} MB")

run_memory_monitored_simulation(brain, 1000)

# Performance profiling
profiler = PerformanceProfiler("brain_simulation")

@profiler.profile("simulation_loop")
def simulate_with_profiling(brain, steps):
    for step in range(steps):
        brain.step(step)

profile_results = profiler.get_results()
profiler.print_report()
```

#### Performance Metrics

The benchmarking system tracks:
- **Execution Time**: Wall clock time for simulations
- **Step Performance**: Average, min, max step times
- **Memory Usage**: RSS and VMS memory consumption
- **CPU Usage**: CPU utilization percentages
- **Speed Metrics**: Steps per second, operations per second
- **Memory Efficiency**: Memory usage per neuron/synapse

### Performance Optimization Best Practices

```python
def optimized_simulation(brain, world, agent, steps, optimization_level=1):
    """Run simulation with performance optimizations."""
    from nlm_utils.performance import (
        MemoryMonitor,
        PerformanceProfiler,
        benchmark_simulation
    )
    
    # Choose optimization strategy based on level
    optimizations = {
        1: ["memory_efficient", "adaptive_step"],
        2: ["memory_efficient", "adaptive_step", "batch_processing"],
        3: ["memory_efficient", "adaptive_step", "batch_processing", "threading"]
    }
    
    active_optimizations = optimizations.get(optimization_level, [])
    
    # Set up monitoring
    if "memory_efficient" in active_optimizations:
        monitor = MemoryMonitor("optimized_simulation")
    
    if "adaptive_step" in active_optimizations:
        profiler = PerformanceProfiler("optimized_simulation")
    
    # Implementation depends on optimization strategy
    if "memory_efficient" in active_optimizations:
        # Use memory-efficient data structures
        pass
    
    if "adaptive_step" in active_optimizations:
        # Adapt simulation based on performance
        pass
    
    # Run simulation with optimizations
    success = optimized_run(brain, world, agent, steps)
    
    # Collect optimization metrics
    metrics = {
        'optimizations': active_optimizations,
        'memory_stats': monitor.get_summary() if "memory_efficient" in active_optimizations else None,
        'performance_stats': profiler.get_summary() if "adaptive_step" in active_optimizations else None
    }
    
    return success, metrics
```

## Debugging and Logging Capabilities

### Module: `nlm_utils.debugging`

The debugging module provides enhanced logging utilities and brain state examination tools.

#### Key Classes and Functions

```python
from nlm_utils.debugging import (
    DebugLogger,
    BrainDebugger,
    LogLevel,
    DebugConfig,
    debug_brain_state,
    log_simulation_step,
    BrainStateSnapshot,
    StepLogger
)

# Configure logging
config = DebugConfig(
    log_level=LogLevel.DEBUG,
    file_output=True,
    console_output=True,
    log_file="simulation.log",
    max_file_size=100 * 1024 * 1024,  # 100MB
    enable_brain_logging=True,
    enable_step_logging=True
)

logger = DebugLogger(config)

# Brain debugging
tracker = BrainDebugger("brain_tracker")

@debug_brain_state("brain_initialization")
def initialize_and_debug(brain, config):
    # Initialize brain and capture detailed state
    brain.initialize()
    
    # Capture initial state snapshot
    snapshot = tracker.capture_state(brain)
    tracker.log_initialization(snapshot, config)
    
    return snapshot

# Step-by-step logging
step_logger = StepLogger("simulation_log")

@log_simulation_step("simulation_step")
def run_step_with_logging(brain, step):
    step_logger.log_step_start(step)
    brain.step(step)
    step_logger.log_step_end(step)
    return True
```

#### Brain State Examination

```python
def examine_brain_state(brain, step, output_file=None):
    """Examine and analyze brain state at a specific step."""
    from nlm_utils.debugging import BrainDebugger, DebugLogger
    
    debugger = BrainDebugger("state_analysis")
    logger = DebugLogger()
    
    # Capture comprehensive state snapshot
    snapshot = debugger.capture_full_state(brain)
    
    # Log analysis results
    logger.log_brain_analysis(step, snapshot)
    
    if output_file:
        debugger.export_state_analysis(snapshot, output_file)
    
    # Return analysis results
    return {
        'total_neurons': snapshot.get('neuron_count', 0),
        'total_synapses': snapshot.get('synapse_count', 0),
        'firing_neurons': snapshot.get('firing_count', 0),
        'average_firing_rate': snapshot.get('avg_firing_rate', 0.0),
        'development_stage': snapshot.get('development_stage', None),
        'memory_metrics': snapshot.get('memory_metrics', {}),
        'plasticity_metrics': snapshot.get('plasticity_metrics', {})
    }

def track_simulation_brain_state(brain, steps, output_interval=100):
    """Track brain state throughout simulation."""
    from nlm_utils.debugging import BrainStateTracker, DebugLogger
    
    tracker = BrainStateTracker("simulation_tracker")
    logger = DebugLogger()
    
    state_history = []
    
    for step in range(steps):
        if step % output_interval == 0:
            # Capture current state
            snapshot = tracker.capture_state(brain)
            state_history.append(snapshot)
            
            # Log state changes
            tracker.log_state_change(step, snapshot)
            
            # Analyze state for anomalies
            anomalies = tracker.analyze_state_for_anomalies(snapshot)
            if anomalies:
                logger.log_anomalies(step, anomalies)
    
    return {
        'state_history': state_history,
        'total_captures': len(state_history),
        'analysis': tracker.summarize_state_changes(state_history)
    }
```

## Advanced Commands and Utilities

### Module: `nlm_utils.experiment`

The experiment module provides experiment runner utilities for running multiple simulations.

#### Key Classes and Functions

```python
from nlm_utils.experiment import (
    Experiment,
    ExperimentRunner,
    ExperimentResults,
    run_experiment,
    ExperimentSuite,
    batch_experiments,
    ExperimentConfig
)

# Basic experiment
config = ExperimentConfig(
    name="learning_rate_experiment",
    description="Test different learning rates",
    parameters=[
        {"learning_rate": 0.001},
        {"learning_rate": 0.01},
        {"learning_rate": 0.1}
    ]
)

def test_learning_rate(params):
    """Test brain learning with different learning rates."""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.learning_rate", params["learning_rate"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Collect results
    return {
        "final_spikes": brain.getTotalSpikeCount(),
        "final_firing": brain.getFiringNeuronCount(),
        "final_rate": brain.getAverageFiringRate()
    }

# Run experiment
experiment = Experiment("learning_rate_test", test_learning_rate)
results = experiment.run(config.parameters)
experiment.print_results(results)
```

#### Advanced Experiment Features

```python
class MultiParameterExperiment(Experiment):
    """Multi-parameter experiment with combinatorial search."""
    
    def __init__(self, name, parameter_grids):
        super().__init__(name)
        self.parameter_grids = parameter_grids
    
    def generate_parameter_combinations(self):
        """Generate all parameter combinations."""
        import itertools
        
        # Get all parameter names
        param_names = list(self.parameter_grids.keys())
        
        # Generate Cartesian product
        param_values = list(self.parameter_grids.values())
        combinations = list(itertools.product(*param_values))
        
        # Convert to parameter dictionaries
        param_dicts = []
        for combo in combinations:
            param_dict = dict(zip(param_names, combo))
            param_dicts.append(param_dict)
        
        return param_dicts
    
    def run(self, output_callback=None):
        """Run experiment with progress reporting."""
        combinations = self.generate_parameter_combinations()
        total = len(combinations)
        
        for i, params in enumerate(combinations):
            try:
                result = self.run_trial(params)
                
                if output_callback:
                    output_callback(i, total, params, result)
                
            except Exception as e:
                self.record_error(i, params, e)
        
        return self.get_results()

# Batch experiments
experiment_suite = ExperimentSuite("multi_experiment")

@batch_experiments("learning_rate_test", test_learning_rate)
@batch_experiments("neuron_count_test", test_neuron_count)
@batch_experiments("plasticity_test", test_plasticity_config)

def run_experiment_suite():
    """Run complete experiment suite."""
    results = experiment_suite.run()
    experiment_suite.export_results("experiment_results.json")
    return results
```

### Module: `nlm_utils.session`

The session module provides session management tools for managing multiple brain instances.

#### Key Classes and Functions

```python
from nlm_utils.session import (
    BrainSession,
    SessionManager,
    SessionConfig,
    SessionStats,
    create_session,
    restore_session,
    list_sessions,
    SessionCheckpoint
)

# Create and manage sessions
config = SessionConfig(
    name="learning_session",
    description="Session for testing learning dynamics",
    brain_config={
        "brain.neuron_count": 500,
        "plasticity.stdp.enable": True
    },
    checkpoint_interval=100,
    auto_save=True
)

# Create a new session
session = create_session(config)

# Run simulation with session management
for step in range(1000):
    # Update brain
    session.brain.step(step)
    
    # Get action
    action = session.agent.decodeMotorCommand()
    
    # Update world
    session.world.update(0.1)
    session.world.applyMotorCommand(action, session.world.getSimulationTime())
    
    # Get reward and apply neuromodulation
    reward = session.get_reward()
    session.agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    session.agent.updateDevelopment(0.1)

# Save session
session.save("learning_session_001")

# Restore session
restored_session = restore_session("learning_session_001")

# Session management utilities
manager = SessionManager("session_manager")

# List all sessions
for session_info in manager.list_sessions():
    print(f"Session: {session_info.name}, Created: {session_info.created_time}")

# Advanced session features
class ParallelSessionManager:
    """Manage multiple parallel brain sessions."""
    
    def __init__(self, session_configs):
        self.sessions = []
        self.configs = session_configs
        self.simulation_steps = 0
    
    def start_all_sessions(self):
        """Start all sessions in parallel."""
        for config in self.configs:
            session = create_session(config)
            self.sessions.append(session)
    
    def run_parallel_simulation(self, steps):
        """Run simulation steps in parallel across sessions."""
        for step in range(steps):
            for session in self.sessions:
                # Update each session
                session.brain.step(step)
                session.agent.updateDevelopment(0.1)
            
            self.simulation_steps += 1
    
    def sync_sessions(self):
        """Synchronize state across all sessions."""
        if len(self.sessions) < 2:
            return
        
        # Get reference state from first session
        reference_state = self.sessions[0].get_brain_state()
        
        # Apply to all other sessions
        for i, session in enumerate(self.sessions[1:], 1):
            session.set_brain_state(reference_state)
    
    def compare_sessions(self):
        """Compare states across all sessions."""
        comparison = {
            'session_count': len(self.sessions),
            'state_differences': [],
            'performance_metrics': []
        }
        
        # Compare brain states
        for i, session in enumerate(self.sessions):
            state = session.get_brain_state()
            comparison['state_differences'].append({
                'session_id': i,
                'neuron_count': state.get('neuron_count', 0),
                'firing_rate': state.get('firing_rate', 0.0),
                'total_spikes': state.get('total_spikes', 0)
            })
        
        return comparison
```

## API Reference: Core Utilities

### Error Handling Utilities

| Class/Function | Description | Example Usage |
|---------------|-------------|---------------|
| `NLMError` | Base exception class | `raise NLMError("Error message")` |
| `ConfigurationError` | Configuration errors | `raise ConfigurationError("Invalid parameter: {param}", parameter=param)` |
| `BrainInitializationError` | Brain initialization errors | `raise BrainInitializationError("Failed to initialize brain")` |
| `validate_config(config)` | Validate configuration | `validate_config(config)` |
| `validate_brain_state(brain)` | Validate brain state | `validate_brain_state(brain)` |
| `safe_function(name, retries=3)` | Decorator for safe function execution | `@safe_function("brain_step", retries=3)` |

### Performance Monitoring Utilities

| Class/Function | Description | Example Usage |
|---------------|-------------|---------------|
| `Benchmark(name, warmup_steps=10)` | Performance benchmarking | `benchmark = Benchmark("test")` |
| `MemoryMonitor(name, interval=0.1)` | Memory usage monitoring | `monitor = MemoryMonitor("memory")` |
| `Profiler(name)` | Performance profiling | `profiler = Profiler("profiler")` |
| `benchmark_simulation(func, steps)` | Benchmark simulation function | `benchmark_simulation(run_simulation, 1000)` |
| `monitor_memory_usage(func, steps)` | Monitor memory usage of function | `monitor_memory_usage(simulate, 1000)` |

### Debugging and Logging Utilities

| Class/Function | Description | Example Usage |
|---------------|-------------|---------------|
| `DebugLogger(config)` | Enhanced logging | `logger = DebugLogger(config)` |
| `BrainDebugger(name)` | Brain state debugging | `debugger = BrainDebugger("debugger")` |
| `StepLogger(name)` | Step-by-step logging | `step_logger = StepLogger("step_logger")` |
| `debug_brain_state(name)` | Decorate brain state debugging | `@debug_brain_state("init")` |
| `log_simulation_step(name)` | Decorate step logging | `@log_simulation_step("step")` |

### Experiment and Session Utilities

| Class/Function | Description | Example Usage |
|---------------|-------------|---------------|
| `Experiment(name, func)` | Experiment runner | `experiment = Experiment("test", func)` |
| `ExperimentRunner(config, experiments)` | Multiple experiment runner | `runner = ExperimentRunner(config, [exp1, exp2])` |
| `ExperimentSuite(name)` | Experiment suite manager | `suite = ExperimentSuite("suite")` |
| `BrainSession(config)` | Brain session manager | `session = BrainSession(config)` |
| `SessionManager(name)` | Session manager | `manager = SessionManager("manager")` |
| `create_session(config)` | Create new session | `session = create_session(config)` |

## Best Practices and Examples

### Complete Example: Robust Simulation with Monitoring

```python
import pynlm
from nlm_utils import (
    Benchmark,
    MemoryMonitor,
    DebugLogger,
    BrainDebugger,
    Experiment,
    BrainSession,
    SessionManager
)

def robust_simulation_with_monitoring(config, steps=1000):
    """Robust simulation with comprehensive monitoring and error handling."""
    
    # Set up monitoring
    benchmark = Benchmark("simulation_benchmark", warmup_steps=10)
    monitor = MemoryMonitor("simulation_memory", interval=0.1)
    debug_logger = DebugLogger()
    brain_debugger = BrainDebugger("brain_tracker")
    
    # Create session
    session_config = SessionConfig(
        name="robust_simulation",
        brain_config=config,
        checkpoint_interval=100
    )
    session = BrainSession(session_config)
    
    @benchmark
    @monitor
    def run_simulation():
        """Run simulation with monitoring."""
        for step in range(steps):
            try:
                # Update world
                session.world.update(0.1)
                
                # Get sensory input
                percept = session.world.getSensoryPercept()
                
                # Process sensory input
                session.agent.processSensoryInput(percept)
                
                # Run brain step
                session.brain.step(step)
                
                # Decode motor command
                action = session.agent.decodeMotorCommand()
                
                # Apply motor command
                session.world.applyMotorCommand(action, session.world.getSimulationTime())
                
                # Apply reward modulation
                reward = session.get_reward()
                session.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                session.agent.updateDevelopment(0.1)
                
                # Log step periodically
                if step % 100 == 0:
                    debug_logger.log_step(step, {
                        'firing_neurons': session.brain.getFiringNeuronCount(),
                        'total_spikes': session.brain.getTotalSpikeCount(),
                        'memory_activity': session.brain.getMemoryActivity(),
                        'development_stage': session.brain.getDevelopmentalStage()
                    })
                
                # Debug brain state
                if step % 500 == 0:
                    brain_debugger.capture_state(session.brain, step)
                
            except Exception as e:
                debug_logger.log_error(step, str(e))
                raise
    
    # Run simulation
    run_simulation()
    
    # Collect metrics
    metrics = {
        'benchmark': benchmark.get_results(),
        'memory': monitor.get_summary(),
        'steps_completed': steps,
        'session_stats': session.get_stats()
    }
    
    # Save session
    session.save(f"simulation_{steps}_steps")
    
    return metrics

# Run complete example
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Run simulation with monitoring
    results = robust_simulation_with_monitoring(config, 1000)
    
    # Print results
    print("=== Simulation Results ===")
    print(f"Benchmark results: {results['benchmark']}")
    print(f"Memory usage: {results['memory']}")
    print(f"Session stats: {results['session_stats']}")
```

### Advanced Error Recovery Example

```python
from nlm_utils.error_handling import (
    NLMError,
    ConfigurationError,
    BrainInitializationError,
    safe_function,
    validate_brain_config
)

def recovery_robust_simulation(config, steps=1000):
    """Simulation with advanced error recovery."""
    
    import pynlm
    import time
    
    errors = []
    recovery_actions = []
    
    @safe_function("simulation_step", max_retries=3, timeout=5.0)
    def robust_step(session, step):
        """Robust step execution with recovery."""
        try:
            # Validate configuration before step
            if not validate_brain_config(config):
                raise ConfigurationError("Invalid configuration")
            
            # Validate brain state
            if not session.brain.validate_state():
                raise BrainInitializationError("Invalid brain state")
            
            # Execute step
            session.world.update(0.1)
            percept = session.world.getSensoryPercept()
            session.agent.processSensoryInput(percept)
            session.brain.step(step)
            action = session.agent.decodeMotorCommand()
            session.world.applyMotorCommand(action, session.world.getSimulationTime())
            
            return True
            
        except NLMError as e:
            errors.append({
                'step': step,
                'error': str(e),
                'type': type(e).__name__,
                'timestamp': time.time()
            })
            
            # Apply recovery actions
            if "brain" in str(e).lower():
                recovery_actions.append("reset_brain")
                session.brain.reset()
            elif "world" in str(e).lower():
                recovery_actions.append("reset_world")
                session.world.reset()
            elif "agent" in str(e).lower():
                recovery_actions.append("reset_agent")
                session.agent.reset()
            
            # Continue with next step after recovery
            return True
    
    # Create session
    session = pynlm.create_session(SessionConfig(
        name="recovery_simulation",
        brain_config=config,
        checkpoint_interval=50
    ))
    
    # Run simulation with recovery
    successful_steps = 0
    failed_steps = 0
    
    for step in range(steps):
        try:
            if robust_step(session, step):
                successful_steps += 1
            else:
                failed_steps += 1
                
        except Exception as e:
            # Log unrecoverable error
            errors.append({
                'step': step,
                'error': str(e),
                'type': type(e).__name__,
                'unrecoverable': True,
                'timestamp': time.time()
            })
            failed_steps += 1
    
    # Generate recovery report
    recovery_report = {
        'total_steps': steps,
        'successful_steps': successful_steps,
        'failed_steps': failed_steps,
        'error_count': len(errors),
        'recovery_actions': recovery_actions,
        'errors': errors,
        'success_rate': successful_steps / steps if steps > 0 else 0.0
    }
    
    return recovery_report
```

## Migration Guide

### From Basic NLM to Enhanced NLM

If you're migrating from the basic NLM API to the enhanced version:

#### Old Basic Usage

```python
import pynlm

# Basic brain creation and simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### New Enhanced Usage

```python
import pynlm
from nlm_utils import (
    BrainDebugger,
    MemoryMonitor,
    Experiment,
    safe_function
)

# Enhanced brain creation with monitoring
config = pynlm.createDefaultConfig()

# Create session for better management
session = pynlm.create_session(SessionConfig(
    name="enhanced_simulation",
    brain_config=config,
    checkpoint_interval=100
))

# Set up monitoring
brain_debugger = BrainDebugger("brain_tracker")
memory_monitor = MemoryMonitor("simulation_memory")

@safe_function("brain_step", retries=3)
def run_step_with_retry(session, step):
    session.brain.step(step)

for step in range(1000):
    try:
        run_step_with_retry(session, step)
        
        if step % 100 == 0:
            # Log performance metrics
            metrics = {
                'step': step,
                'firing_neurons': session.brain.getFiringNeuronCount(),
                'total_spikes': session.brain.getTotalSpikeCount(),
                'memory_usage': memory_monitor.get_current_usage(),
                'development_stage': session.brain.getDevelopmentalStage()
            }
            print(f"Step {step}: {metrics['firing_neurons']} firing neurons")
            
            # Debug brain state periodically
            if step % 500 == 0:
                brain_debugger.capture_state(session.brain, step)
                
    except Exception as e:
        print(f"Error at step {step}: {e}")
        # Error recovery handled by safe_function decorator

# Save enhanced simulation results
session.save("enhanced_simulation_results")
```

### Key Improvements

| Feature | Basic API | Enhanced API |
|---------|-----------|--------------|
| Error Handling | None | Comprehensive error handling with recovery |
| Performance Monitoring | None | Benchmarking, memory monitoring, profiling |
| Brain State Debugging | None | Detailed brain state examination and logging |
| Experiment Management | None | Experiment runner with parameter grids |
| Session Management | None | Session checkpointing and restoration |
| Safe Execution | None | Retry mechanisms and timeout protection |
| Configuration Validation | None | Comprehensive validation utilities |
| Memory Monitoring | None | Real-time memory usage tracking |
| Performance Profiling | None | Detailed performance analysis tools |

## API Usage Examples

### 1. Creating and Running a Brain with Enhanced Monitoring

```python
import pynlm
from nlm_utils import (
    Benchmark,
    MemoryMonitor,
    BrainDebugger,
    SessionConfig,
    create_session
)

# Configure enhanced simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Create session with monitoring
config = SessionConfig(
    name="enhanced_simulation",
    brain_config=config,
    checkpoint_interval=100
)

session = create_session(config)
benchmark = Benchmark("enhanced_simulation")
monitor = MemoryMonitor("simulation_memory")
debugger = BrainDebugger("brain_tracker")

@benchmark
@monitor
def run_enhanced_simulation(session, steps):
    for step in range(steps):
        # Run simulation with detailed logging
        session.world.update(0.1)
        
        # Process sensory input
        percept = session.world.getSensoryPercept()
        session.agent.processSensoryInput(percept)
        
        # Run brain step
        session.brain.step(step)
        
        # Get and apply action
        action = session.agent.decodeMotorCommand()
        session.world.applyMotorCommand(action, session.world.getSimulationTime())
        
        # Apply neuromodulation
        reward = session.get_reward()
        session.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        session.agent.updateDevelopment(0.1)
        
        # Debug brain state
        if step % 500 == 0:
            debugger.capture_state(session.brain, step)

# Run enhanced simulation
metrics = run_enhanced_simulation(session, 1000)
session.save("enhanced_simulation")

print(f"Benchmark results: {benchmark.get_results()}")
print(f"Memory usage: {monitor.get_summary()}")
```

### 2. Running Experiments with Parameter Sweeps

```python
from nlm_utils import (
    Experiment,
    ExperimentSuite,
    batch_experiments
)

# Define experiments with parameter sweeps
experiment_suite = ExperimentSuite("parameter_sweep")

@batch_experiments("learning_rate_experiment")
@batch_experiments("neuron_count_experiment") 
@batch_experiments("plasticity_strength_experiment")

def run_parameter_sweeps():
    """Run complete parameter sweep experiment."""
    
    # Define parameter grids for each experiment
    experiments = [
        {
            "name": "learning_rate_sweep",
            "parameters": {"learning_rate": [0.001, 0.01, 0.1]},
            "func": test_learning_rate_effect
        },
        {
            "name": "neuron_count_sweep", 
            "parameters": {"neuron_count": [500, 1000, 2000]},
            "func": test_neuron_count_effect
        },
        {
            "name": "plasticity_strength_sweep",
            "parameters": {"plasticity_strength": [0.1, 0.5, 1.0]},
            "func": test_plasticity_effect
        }
    ]
    
    # Run all experiments
    results = experiment_suite.run(experiments)
    experiment_suite.export_results("parameter_sweep_results.json")
    
    return results

# Run parameter sweeps
if __name__ == "__main__":
    results = run_parameter_sweeps()
    print("Parameter sweep results:", results)
```

### 3. Advanced Session Management with Recovery

```python
from nlm_utils import (
    SessionManager,
    SessionConfig,
    create_session,
    restore_session
)

def advanced_session_management():
    """Advanced session management with recovery and checkpointing."""
    
    # Create session manager
    manager = SessionManager("advanced_manager")
    
    # Create multiple sessions with different configurations
    session_configs = [
        SessionConfig(
            name="session_1",
            brain_config={"brain.neuron_count": 500, "plasticity.stdp.enable": True},
            checkpoint_interval=50
        ),
        SessionConfig(
            name="session_2", 
            brain_config={"brain.neuron_count": 1000, "plasticity.stdp.learning_rate": 0.01},
            checkpoint_interval=100
        ),
        SessionConfig(
            name="session_3",
            brain_config={"brain.neuron_count": 1500, "neuromod.dopamine.scale": 2.0},
            checkpoint_interval=75
        )
    ]
    
    # Create and start all sessions
    sessions = []
    for config in session_configs:
        session = create_session(config)
        sessions.append(session)
    
    # Run parallel simulation
    for step in range(1000):
        for session in sessions:
            # Update each session
            session.world.update(0.1)
            
            # Process sensory input
            percept = session.world.getSensoryPercept()
            session.agent.processSensoryInput(percept)
            
            # Run brain step
            session.brain.step(step)
            
            # Apply actions
            action = session.agent.decodeMotorCommand()
            session.world.applyMotorCommand(action, session.world.getSimulationTime())
            
            # Apply neuromodulation
            reward = session.get_reward()
            session.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            session.agent.updateDevelopment(0.1)
        
        # Checkpoint sessions periodically
        if step % 200 == 0:
            for session in sessions:
                session.save(f"checkpoint_step_{step}")
            
            # Compare sessions
            comparison = sessions[0].compare_with(sessions[1:])
            print(f"Step {step} comparison:", comparison)
    
    # Restore from checkpoint
    restored_session = restore_session("checkpoint_step_200")
    
    # Compare sessions
    session_comparison = manager.compare_sessions(sessions)
    
    return {
        'sessions': sessions,
        'comparison': session_comparison,
        'checkpoint_count': len(sessions)
    }

# Run advanced session management
if __name__ == "__main__":
    results = advanced_session_management()
    print("Advanced session management complete")
```

## Conclusion

The enhanced NLM Python API provides comprehensive tools for:

- **Robust error handling** with automatic recovery
- **Performance monitoring** with benchmarking and memory tracking
- **Brain state debugging** with detailed logging and analysis
- **Experiment management** with parameter sweep capabilities
- **Session management** with checkpointing and restoration
- **Safe execution** with retry mechanisms and timeouts

These enhancements make the NLM API more powerful, reliable, and user-friendly while maintaining backward compatibility with the basic API. Whether you're running simple simulations or complex experiment suites, the enhanced API provides the tools you need for successful NLM development.

For more detailed information about specific features, refer to the individual module documentation and examples provided throughout this guide.
