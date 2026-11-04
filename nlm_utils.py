# NLM Utilities for Enhanced Python API

This module provides comprehensive utilities for the enhanced NLM Python API, including error handling, performance monitoring, debugging, experiment management, and session utilities.

## Module Overview

The enhanced NLM utilities provide powerful tools to make the NLM Python API more user-friendly, robust, and powerful while maintaining backward compatibility. The modules are organized into logical categories:

### Core Utility Modules
- **`error_handling`**: Comprehensive error handling and validation utilities
- **`performance`**: Performance optimization utilities with benchmarking and monitoring
- **`profiling`**: Performance profiling tools for detailed analysis

### Development and Debugging Modules
- **`logging`**: Enhanced logging utilities with configurable levels and output
- **`debugging`**: Brain state examination and debugging tools
- **`visualization`**: Basic visualization helpers for brain data

### Advanced Application Modules
- **`experiment`**: Experiment runner utilities for systematic testing
- **`session`**: Session management tools for managing multiple brain instances

## Installation

```bash
# Install all enhanced utilities
pip install nlm-utils

# Or install in development mode
pip install -e .
```

The utilities can be imported as:

```python
from nlm_utils import (
    # Core utilities
    BrainDebugger,
    ErrorRecoveryManager,
    Benchmark,
    MemoryMonitor,
    Profiler,
    
    # Development tools
    AdvancedDebugLogger,
    AnomalyDetector,
    VisualizationHelper,
    
    # Application tools
    MultiObjectiveExperiment,
    ParameterSweeper,
    AdvancedBrainSession,
    SessionManagerAdvanced
)
```

## Core Enhanced Utilities

### 1. Error Handling and Validation (`error_handling`)

The error handling module provides comprehensive error handling with meaningful error messages, validation utilities, and automatic recovery mechanisms.

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
    BrainStateValidator,
    ConfigurationValidator,
    ErrorRecoveryManager
)

# Example: Enhanced error handling with recovery

try:
    brain = createBrain(config)
    brain.initialize()
except ConfigurationError as e:
    print(f"Configuration error: {e}")
    print(f"Invalid parameter: {e.parameter}")
    print(f"Error code: {e.error_code}")
    print(f"Suggested fix: {e.suggested_fix}")

# Example: Validation utilities
if validate_config(config):
    print("Configuration is valid")
else:
    errors = config.getErrors()
    for error in errors:
        print(f"Error: {error['message']}")
        print(f"Severity: {error['severity']}")
        print(f"Fix: {error['fix']}")

# Example: Safe wrapper functions with retry
@safe_function("brain_step", retries=3, timeout=1000, backoff=0.1)
def safe_brain_step(brain, step):
    brain.step(step)
    return True

# Example: Error recovery manager
recovery_manager = ErrorRecoveryManager("simulation_recovery")

@safe_function("simulation_step", max_retries=3, on_error="auto_recover")
def robust_simulation_step(session, step):
    try:
        # Simulation logic here
        return True
    except Exception as e:
        # Error recovery handled by decorator
        raise
```

#### Enhanced Error Types

- **NLMError**: Base error class for all NLM errors with detailed information
- **ConfigurationError**: Errors in configuration with parameter details
- **BrainInitializationError**: Errors during brain initialization with suggestions
- **ValidationError**: Validation errors with severity levels and fixes
- **SimulationError**: Simulation-related errors with context
- **MemoryError**: Memory-related errors with recommendations
- **PerformanceError**: Performance-related errors with optimization suggestions

### 2. Enhanced Validation Utilities

```python
from nlm_utils.error_handling import (
    ConfigurationValidator,
    BrainStateValidator,
    validate_config_comprehensive,
    validate_brain_state_comprehensive
)

# Comprehensive configuration validation
validator = ConfigurationValidator()

# Add custom validation rules
validator.add_rule("brain.neuron_count", 
                  lambda x: x > 0, 
                  "Neuron count must be positive",
                  lambda x: f"Use a smaller count like {max(1, x//2)}")
validator.add_rule("brain.synapse_density",
                  lambda x: 0.0 <= x <= 1.0,
                  "Synapse density must be between 0.0 and 1.0",
                  lambda x: "Use 0.05 for sparse connections or 0.2 for dense connections")
validator.add_rule("plasticity.stdp.learning_rate",
                  lambda x: 0.0 <= x <= 1.0,
                  "Learning rate must be between 0.0 and 1.0",
                  lambda x: f"Use {min(0.01, x)} for stable learning")

# Validate configuration with comprehensive results
validation_result = validate_config_comprehensive(config)
print(f"Validation score: {validation_result['score']}/100")
print(f"Issues found: {validation_result['issues']}")
print(f"Suggestions: {validation_result['suggestions']}")

# Brain state validation
brain_validator = BrainStateValidator()

# Add brain state validation rules
brain_validator.add_rule("firing_neurons",
                        lambda x: x <= brain_config["brain.neuron_count"],
                        "Firing neurons cannot exceed total neuron count")
brain_validator.add_rule("average_firing_rate",
                        lambda x: x < 10.0,
                        "Average firing rate seems unusually high",
                        lambda x: "Consider reducing excitation or increasing inhibition")

# Validate brain state
brain_state = {
    "neuron_count": brain.getTotalNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "development_stage": brain.getDevelopmentalStage()
}

brain_validation = validate_brain_state_comprehensive(brain_state)
print(f"Brain validation score: {brain_validation['score']}/100")
```

### 3. Performance Monitoring and Optimization (`performance`)

The enhanced performance module provides advanced benchmarking tools, memory monitoring, and performance profiling capabilities with real-time analysis.

#### Key Classes and Functions

```python
from nlm_utils.performance import (
    Benchmark,
    MemoryMonitor,
    Profiler,
    PerformanceProfiler,
    PerformanceProfilerAdvanced,
    benchmark_simulation,
    profile_brain_step,
    monitor_memory_usage,
    PerformanceMetrics,
    PerformanceAnalysis
)

# Example: Advanced benchmarking with performance profiling
benchmark = Benchmark(
    "enhanced_simulation_benchmark",
    warmup_steps=20,
    cooldown_steps=10,
    detailed_metrics=True
)

@benchmark
@profile_brain_step("step_execution")
@monitor_memory_usage("step_memory")
@Profiler("step_profiler")
def run_enhanced_simulation(brain, steps):
    """Run enhanced simulation with comprehensive profiling."""
    for step in range(steps):
        # Run simulation with detailed profiling
        brain.step(step)
        
        # Collect metrics every 100 steps
        if step % 100 == 0:
            metrics = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage()
            }
            yield metrics

# Run enhanced benchmark
results = benchmark.run(run_enhanced_simulation, 1000)

# Analyze performance results
performance_analysis = PerformanceAnalysis(results)
performance_analysis.print_detailed_report()
performance_analysis.export_results("simulation_performance_report.json")

# Advanced memory monitoring
advanced_monitor = MemoryMonitor(
    "advanced_memory_monitor",
    interval=0.05,
    alert_thresholds={
        'memory_usage_mb': 500,
        'memory_growth_rate_mb_per_sec': 10,
        'step_time_ms': 100
    }
)

@advanced_monitor
@Profiler("memory_profiler")
def run_memory_monitored_simulation(brain, steps):
    """Run simulation with advanced memory monitoring."""
    for step in range(steps):
        brain.step(step)
        
        # Check memory alerts
        alerts = advanced_monitor.check_alerts()
        if alerts:
            print(f"Memory alerts: {alerts}")
        
        # Log memory usage
        memory_info = advanced_monitor.get_current_usage()
        print(f"Step {step}: Memory usage: {memory_info['rss_mb']:.2f} MB")
        
        # Check for memory leaks
        if advanced_monitor.detect_memory_leak():
            print("WARNING: Potential memory leak detected!")

# Performance profiling with custom metrics
profiler = PerformanceProfilerAdvanced("brain_performance")

@profiler.profile("simulation_loop")
@profiler.profile("memory_management")
@profiler.profile("plasticity_updates")
@profiler.profile("development_updates")
def profile_complete_simulation(brain, steps):
    """Profile complete simulation with custom metrics."""
    for step in range(steps):
        # Profile specific components
        brain.step(step)
        
        # Collect custom metrics
        metrics = {
            'plasticity_update_time': profiler.get_last_duration('plasticity_updates'),
            'development_update_time': profiler.get_last_duration('development_updates'),
            'memory_usage': profiler.get_current_memory_usage(),
            'cpu_usage': profiler.get_current_cpu_usage()
        }
```

#### Enhanced Performance Metrics

The enhanced benchmarking system tracks comprehensive performance metrics:

- **Execution Time**: Wall clock time with precision timing
- **Step Performance**: Microsecond-precision step timing
- **Memory Usage**: Detailed RSS/VMS tracking with alerts
- **CPU Usage**: Real-time CPU utilization monitoring
- **Speed Metrics**: Steps per second with accuracy
- **Memory Efficiency**: Memory usage per computational unit
- **Component Performance**: Individual component profiling
- **Scalability Metrics**: Performance scaling with problem size
- **Resource Utilization**: CPU, memory, and I/O usage
- **Thermal Metrics**: CPU temperature and cooling efficiency

### Advanced Performance Optimization

```python
from nlm_utils.performance import (
    AdaptivePerformanceOptimizer,
    PerformanceTuner,
    ResourceMonitor,
    bottleneck_detector
)

# Create adaptive performance optimizer
optimizer = AdaptivePerformanceOptimizer("simulation_optimizer")

@optimizer.optimize("memory_efficient")
@optimizer.optimize("adaptive_step")
@optimizer.optimize("batch_processing")
def optimized_simulation(brain, world, agent, steps, optimization_level=2):
    """Run simulation with adaptive performance optimization."""
    
    optimizations = {
        1: ["memory_efficient", "basic_adaptive"],
        2: ["memory_efficient", "adaptive_step", "batch_processing"],
        3: ["memory_efficient", "adaptive_step", "batch_processing", "threading", "caching"]
    }
    
    active_optimizations = optimizations.get(optimization_level, [])
    
    # Configure resource monitor
    resource_monitor = ResourceMonitor("optimized_simulation")
    resource_monitor.set_limits(
        memory_mb=1000,
        cpu_percent=80,
        step_time_ms=50
    )
    
    # Run simulation with optimizations
    for step in range(steps):
        # Adaptive performance based on current state
        if "adaptive_step" in active_optimizations:
            # Adapt step timing based on performance
            performance = optimizer.get_current_performance()
            if performance['step_time_ms'] > 50:
                # Reduce workload for slow steps
                continue
        
        # Memory-efficient processing
        if "memory_efficient" in active_optimizations:
            # Use memory-efficient data structures
            pass
        
        # Batch processing optimization
        if "batch_processing" in active_optimizations and step % 10 == 0:
            # Process in batches for efficiency
            batch_steps = min(10, steps - step)
            for batch_step in range(batch_steps):
                current_step = step + batch_step
                brain.step(current_step)
            continue
        
        # Run single step with optimizations
        brain.step(step)
        
        # Resource monitoring and throttling
        if resource_monitor.should_throttle():
            import time
            time.sleep(0.001)  # Throttle if resource limits exceeded
        
        # Log optimization metrics
        optimization_metrics = optimizer.get_optimization_metrics()
        if step % 100 == 0:
            print(f"Optimization metrics: {optimization_metrics}")
    
    # Get final optimization results
    final_results = optimizer.get_final_results()
    return final_results

# Detect and fix performance bottlenecks
bottleneck_detector = bottleneck_detector("performance_analyzer")

@bottleneck_detector.track("simulation_bottleneck")
@bottleneck_detector.track("memory_bottleneck")
@bottleneck_detector.track("cpu_bottleneck")
def detect_simulation_bottlenecks(brain, steps):
    """Detect and analyze performance bottlenecks."""
    
    bottlenecks = bottleneck_detector.analyze_bottlenecks()
    
    if bottlenecks:
        print("Performance bottlenecks detected:")
        for bottleneck in bottlenecks:
            print(f"- {bottleneck['type']}: {bottleneck['severity']}")
            print(f"  Location: {bottleneck['location']}")
            print(f"  Impact: {bottleneck['impact']}")
            print(f"  Suggested fix: {bottleneck['suggested_fix']}")
    
    return bottlenecks
```

## Development and Debugging Utilities

### 4. Enhanced Logging and Debugging (`logging`, `debugging`)

The enhanced debugging module provides advanced logging utilities, brain state examination tools, and comprehensive debugging capabilities.

#### Key Features

```python
from nlm_utils.debugging import (
    AdvancedDebugLogger,
    BrainStateAnalyzer,
    StepLoggerAdvanced,
    LogLevel,
    DebugConfigAdvanced,
    debug_brain_state_advanced,
    log_simulation_step_advanced,
    BrainStateSnapshotAdvanced,
    AnomalyDetector,
    PerformanceProfilerDebug
)

# Configure advanced debugging
config = DebugConfigAdvanced(
    log_level=LogLevel.DEBUG,
    file_output=True,
    console_output=True,
    log_file="enhanced_simulation.log",
    max_file_size=500 * 1024 * 1024,  # 500MB
    enable_brain_logging=True,
    enable_step_logging=True,
    enable_performance_logging=True,
    enable_memory_logging=True,
    structured_logging=True,
    log_format="json"
)

logger = AdvancedDebugLogger(config)

# Advanced brain debugging
analyzer = BrainStateAnalyzer("advanced_brain_analyzer")

@debug_brain_state_advanced("brain_initialization")
def initialize_and_analyze(brain, config):
    """Initialize brain and perform comprehensive analysis."""
    # Initialize brain
    brain.initialize()
    
    # Capture comprehensive state snapshot
    snapshot = analyzer.capture_comprehensive_state(brain)
    
    # Log initialization details
    logger.log_brain_initialization(snapshot, config)
    
    # Analyze initial state
    initial_analysis = analyzer.analyze_initial_state(snapshot)
    logger.log_initial_analysis(initial_analysis)
    
    # Detect anomalies in initial state
    anomalies = analyzer.detect_anomalies(snapshot, "initialization")
    if anomalies:
        logger.log_anomalies("initialization", anomalies)
    
    return snapshot, initial_analysis, anomalies

# Advanced step-by-step logging
step_logger = StepLoggerAdvanced("advanced_step_logger")

@log_simulation_step_advanced("simulation_step")
@debug_brain_state_advanced("step_state")
@AnomalyDetector("step_anomalies")
def run_step_with_advanced_logging(session, step):
    """Run simulation step with comprehensive logging and analysis."""
    
    # Log step start
    step_logger.log_step_start(step)
    
    try:
        # Update world
        session.world.update(0.1)
        
        # Get sensory input
        percept = session.world.getSensoryPercept()
        
        # Process sensory input with logging
        session.agent.processSensoryInput(percept)
        
        # Run brain step with detailed profiling
        brain_step_start = time.time()
        session.brain.step(step)
        brain_step_duration = time.time() - brain_step_start
        
        # Log brain step metrics
        step_logger.log_brain_step(
            step,
            duration=brain_step_duration,
            firing_neurons=session.brain.getFiringNeuronCount(),
            total_spikes=session.brain.getTotalSpikeCount(),
            average_firing_rate=session.brain.getAverageFiringRate()
        )
        
        # Decode motor command
        action = session.agent.decodeMotorCommand()
        
        # Apply motor command
        session.world.applyMotorCommand(action, session.world.getSimulationTime())
        
        # Apply neuromodulation
        reward = session.get_reward()
        session.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        development_start = time.time()
        session.agent.updateDevelopment(0.1)
        development_duration = time.time() - development_start
        
        step_logger.log_development_update(step, duration=development_duration)
        
        # Log step end with performance metrics
        step_logger.log_step_end(step, {
            'brain_step_duration': brain_step_duration,
            'development_duration': development_duration,
            'memory_usage': get_memory_usage(),
            'cpu_usage': get_cpu_usage()
        })
        
        # Detect step anomalies
        anomalies = detector.detect_step_anomalies(step, {
            'brain_step_duration': brain_step_duration,
            'development_duration': development_duration,
            'reward': reward
        })
        if anomalies:
            logger.log_anomalies(step, anomalies)
        
        return True
        
    except Exception as e:
        # Log error with context
        step_logger.log_step_error(step, str(e), e.__class__.__name__)
        raise

# Comprehensive brain state analysis
def analyze_brain_state_comprehensive(brain, step, output_file=None):
    """Perform comprehensive brain state analysis."""
    analyzer = BrainStateAnalyzer("comprehensive_analyzer")
    
    # Capture comprehensive brain state
    comprehensive_state = analyzer.capture_comprehensive_state(brain)
    
    # Perform multiple types of analysis
    analyses = {}
    
    # Neural dynamics analysis
    analyses['neural_dynamics'] = analyzer.analyze_neural_dynamics(comprehensive_state)
    
    # Plasticity analysis
    analyses['plasticity'] = analyzer.analyze_plasticity_dynamics(comprehensive_state)
    
    # Memory analysis
    analyses['memory'] = analyzer.analyze_memory_state(comprehensive_state)
    
    # Development analysis
    analyses['development'] = analyzer.analyze_development_state(comprehensive_state)
    
    # Neuromodulation analysis
    analyses['neuromodulation'] = analyzer.analyze_neuromodulation_state(comprehensive_state)
    
    # Cognition analysis
    analyses['cognition'] = analyzer.analyze_cognitive_state(comprehensive_state)
    
    # Detect anomalies
    anomalies = analyzer.detect_anomalies(comprehensive_state, analyses)
    
    # Performance analysis
    performance_analysis = analyzer.analyze_performance(comprehensive_state)
    
    # Log comprehensive analysis
    logger.log_comprehensive_analysis(step, {
        'snapshot': comprehensive_state,
        'analyses': analyses,
        'anomalies': anomalies,
        'performance': performance_analysis
    })
    
    if output_file:
        analyzer.export_comprehensive_analysis({
            'snapshot': comprehensive_state,
            'analyses': analyses,
            'anomalies': anomalies,
            'performance': performance_analysis
        }, output_file)
    
    # Generate detailed report
    detailed_report = analyzer.generate_detailed_report(analyses)
    
    return {
        'comprehensive_state': comprehensive_state,
        'analyses': analyses,
        'anomalies': anomalies,
        'performance_analysis': performance_analysis,
        'detailed_report': detailed_report,
        'step': step
    }
```

#### Advanced Brain State Examination

```python
from nlm_utils.debugging import (
    BrainStateSnapshotAdvanced,
    StateChangeDetector,
    AnomalyDetector,
    PerformanceProfilerDebug,
    NeuralDynamicsAnalyzer,
    PlasticityAnalyzer
)

def examine_brain_state_advanced(brain, step, output_file=None):
    """Examine and analyze brain state at a specific step with advanced analysis."""
    
    # Create advanced analyzers
    neural_analyzer = NeuralDynamicsAnalyzer("neural_dynamics")
    plasticity_analyzer = PlasticityAnalyzer("plasticity")
    state_analyzer = BrainStateSnapshotAdvanced("comprehensive_snapshot")
    anomaly_detector = AnomalyDetector("state_anomalies")
    performance_profiler = PerformanceProfilerDebug("performance_analysis")
    
    # Capture advanced state snapshot
    advanced_snapshot = state_analyzer.capture_advanced_state(brain)
    
    # Perform advanced analysis
    analyses = {}
    
    # Neural dynamics analysis
    analyses['neural_dynamics'] = {
        'spike_patterns': neural_analyzer.analyze_spike_patterns(advanced_snapshot),
        'network_dynamics': neural_analyzer.analyze_network_dynamics(advanced_snapshot),
        'stability_metrics': neural_analyzer.analyze_stability(advanced_snapshot),
        'synchronization': neural_analyzer.analyze_synchronization(advanced_snapshot)
    }
    
    # Plasticity analysis
    analyses['plasticity'] = {
        'weight_dynamics': plasticity_analyzer.analyze_weight_dynamics(advanced_snapshot),
        'learning_efficiency': plasticity_analyzer.analyze_learning_efficiency(advanced_snapshot),
        'stability': plasticity_analyzer.analyze_plasticity_stability(advanced_snapshot),
        'adaptation': plasticity_analyzer.analyze_adaptation(advanced_snapshot)
    }
    
    # Memory analysis
    from nlm_utils.debugging import MemoryAnalyzer
    memory_analyzer = MemoryAnalyzer("memory_analysis")
    analyses['memory'] = memory_analyzer.analyze_memory_state(advanced_snapshot)
    
    # Development analysis
    from nlm_utils.debugging import DevelopmentAnalyzer
    development_analyzer = DevelopmentAnalyzer("development_analysis")
    analyses['development'] = development_analyzer.analyze_development_state(advanced_snapshot)
    
    # Neuromodulation analysis
    from nlm_utils.debugging import NeuromodulationAnalyzer
    neuromodulation_analyzer = NeuromodulationAnalyzer("neuromodulation_analysis")
    analyses['neuromodulation'] = neuromodulation_analyzer.analyze_neuromodulation_state(advanced_snapshot)
    
    # Cognition analysis
    from nlm_utils.debugging import CognitiveAnalyzer
    cognitive_analyzer = CognitiveAnalyzer("cognitive_analysis")
    analyses['cognition'] = cognitive_analyzer.analyze_cognitive_state(advanced_snapshot)
    
    # Detect anomalies
    anomalies = anomaly_detector.detect_comprehensive_anomalies(advanced_snapshot, analyses)
    
    # Performance analysis
    performance_analysis = performance_profiler.analyze_comprehensive_performance(advanced_snapshot)
    
    # Log comprehensive analysis
    from nlm_utils.debugging import AdvancedDebugLogger
    logger = AdvancedDebugLogger()
    logger.log_comprehensive_analysis(step, {
        'snapshot': advanced_snapshot,
        'analyses': analyses,
        'anomalies': anomalies,
        'performance': performance_analysis
    })
    
    if output_file:
        state_analyzer.export_comprehensive_analysis({
            'snapshot': advanced_snapshot,
            'analyses': analyses,
            'anomalies': anomalies,
            'performance': performance_analysis
        }, output_file)
    
    # Generate detailed report
    detailed_report = state_analyzer.generate_detailed_report(analyses)
    
    return {
        'advanced_snapshot': advanced_snapshot,
        'analyses': analyses,
        'anomalies': anomalies,
        'performance_analysis': performance_analysis,
        'detailed_report': detailed_report,
        'step': step
    }
```

## Advanced Experiment Management

### 5. Experiment Runner Utilities (`experiment`)

The experiment module provides advanced experiment runner utilities with parameter sweep capabilities, multi-objective optimization, and comprehensive experiment management.

#### Key Features

```python
from nlm_utils.experiment import (
    AdvancedExperiment,
    ExperimentRunnerAdvanced,
    ExperimentResultsAdvanced,
    MultiObjectiveExperiment,
    ParameterSweeper,
    ExperimentSuiteAdvanced,
    run_experiment_advanced,
    CrossValidationExperiment,
    BayesianOptimizationExperiment
)

# Advanced experiment with multi-objective optimization
advanced_experiment = AdvancedExperiment(
    name="multi_objective_learning",
    description="Optimize brain learning with multiple objectives",
    function=multi_objective_learning_function,
    parameters={
        "learning_rate": {"type": "continuous", "range": [0.001, 0.1]},
        "neuron_count": {"type": "integer", "range": [100, 1000]},
        "plasticity_strength": {"type": "continuous", "range": [0.1, 2.0]},
        "batch_size": {"type": "integer", "range": [10, 100]}
    },
    objectives=[
        {"name": "learning_speed", "weight": 0.4, "higher_is_better": True},
        {"name": "memory_retention", "weight": 0.3, "higher_is_better": True},
        {"name": "efficiency", "weight": 0.3, "higher_is_better": True}
    ]
)

# Run advanced experiment
results = advanced_experiment.run(max_iterations=100, parallel=True)
advanced_experiment.print_results(results)
advanced_experiment.export_results("advanced_experiment_results.json")
```

#### Multi-Objective Experiment

```python
from nlm_utils.experiment import (
    MultiObjectiveExperiment,
    ParetoOptimizer,
    ObjectiveWeights
)

class MultiObjectiveLearningExperiment(MultiObjectiveExperiment):
    """Multi-objective experiment for brain learning optimization."""
    
    def __init__(self):
        objectives = [
            {"name": "learning_speed", "weight": 0.3, "higher_is_better": True},
            {"name": "memory_efficiency", "weight": 0.3, "higher_is_better": True},
            {"name": "stability", "weight": 0.2, "higher_is_better": True},
            {"name": "resource_efficiency", "weight": 0.2, "higher_is_better": True}
        ]
        
        super().__init__("multi_objective_learning", objectives)
    
    def run_trial(self, parameters):
        """Run a single trial with given parameters."""
        import pynlm
        
        # Create brain with parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", parameters["neuron_count"])
        config.set("plasticity.stdp.learning_rate", parameters["learning_rate"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Collect multi-objective metrics
        metrics = {
            "learning_speed": brain.getAverageFiringRate(),
            "memory_efficiency": brain.getMemoryActivity(),
            "stability": brain.getStabilityMetric(),
            "resource_efficiency": brain.getResourceEfficiency()
        }
        
        return metrics
    
    def evaluate_objectives(self, metrics, parameters):
        """Evaluate all objectives for given metrics."""
        return {
            "learning_speed": metrics["learning_speed"],
            "memory_efficiency": metrics["memory_efficiency"], 
            "stability": metrics["stability"],
            "resource_efficiency": metrics["resource_efficiency"]
        }

# Run multi-objective experiment
experiment = MultiObjectiveLearningExperiment()
results = experiment.run(max_iterations=50, population_size=20)

# Get Pareto front
pareto_front = experiment.get_pareto_front(results)
experiment.visualize_pareto_front(pareto_front)

# Export results
experiment.export_results("multi_objective_results.json")

# Print best solutions
experiment.print_best_solutions(results, top_k=5)
```

#### Parameter Sweeper

```python
from nlm_utils.experiment import ParameterSweeper

class ComprehensiveParameterSweeper(ParameterSweeper):
    """Comprehensive parameter sweeper for brain experiments."""
    
    def __init__(self):
        # Define parameter grids for different experiments
        self.parameter_grids = {
            "learning_rate_sweep": {
                "learning_rate": [0.001, 0.005, 0.01, 0.05, 0.1],
                "plasticity_stdp": [True, False]
            },
            "neuron_count_sweep": {
                "neuron_count": [100, 500, 1000, 2000, 5000],
                "synapse_density": [0.05, 0.1, 0.2]
            },
            "neuromodulation_sweep": {
                "dopamine_scale": [0.5, 1.0, 2.0],
                "curiosity_enable": [True, False],
                "novelty_enable": [True, False]
            },
            "development_sweep": {
                "development_enable": [True, False],
                "critical_period": [True, False],
                "maturation_rate": [0.01, 0.05, 0.1]
            },
            "combined_sweep": {
                # Combine multiple parameters for complex experiments
                "learning_rate": [0.001, 0.01, 0.1],
                "neuron_count": [500, 1000],
                "plasticity_stdp": [True, False],
                "dopamine_scale": [1.0, 2.0]
            }
        }
    
    def create_experiment_configs(self, sweep_name):
        """Create experiment configurations from parameter grid."""
        if sweep_name not in self.parameter_grids:
            raise ValueError(f"Unknown sweep: {sweep_name}")
        
        param_grid = self.parameter_grids[sweep_name]
        
        # Generate all combinations
        import itertools
        param_names = list(param_grid.keys())
        param_values = list(param_grid.values())
        
        combinations = list(itertools.product(*param_values))
        
        # Create experiment configurations
        experiments = []
        for combination in combinations:
            config = dict(zip(param_names, combination))
            
            experiments.append({
                "name": f"{sweep_name}_{len(experiments)}",
                "parameters": config,
                "description": f"Experiment with parameters: {config}"
            })
        
        return experiments
    
    def run_sweep(self, sweep_name, experiment_func, output_callback=None):
        """Run parameter sweep with progress reporting."""
        experiments = self.create_experiment_configs(sweep_name)
        total = len(experiments)
        
        print(f"Running {sweep_name} with {total} parameter combinations...")
        
        results = {}
        for i, experiment in enumerate(experiments):
            print(f"Running experiment {i+1}/{total}: {experiment['name']}")
            
            try:
                # Run experiment
                experiment_result = experiment_func(experiment["parameters"])
                results[experiment["name"]] = experiment_result
                
                # Call output callback if provided
                if output_callback:
                    output_callback(i, total, experiment["parameters"], experiment_result)
                    
            except Exception as e:
                print(f"Experiment {experiment['name']} failed: {e}")
                results[experiment["name"]] = {
                    "success": False,
                    "error": str(e),
                    "parameters": experiment["parameters"]
                }
        
        # Analyze sweep results
        analysis = self.analyze_sweep_results(results)
        
        return {
            "sweep_name": sweep_name,
            "results": results,
            "analysis": analysis,
            "total_experiments": total,
            "successful_experiments": sum(1 for r in results.values() if r.get("success", True))
        }
```

## Advanced Session Management

### 6. Session Management Tools (`session`)

The session module provides session management tools for managing multiple brain instances.

#### Key Features

```python
from nlm_utils.session import (
    AdvancedBrainSession,
    SessionManagerAdvanced,
    SessionConfigAdvanced,
    SessionStatsAdvanced,
    create_session_advanced,
    restore_session_advanced,
    list_sessions_advanced,
    SessionCheckpointAdvanced,
    SessionSyncManager,
    ParallelSessionManager
)

# Create advanced session with comprehensive configuration
config = SessionConfigAdvanced(
    name="advanced_learning_session",
    description="Advanced session for testing learning dynamics with comprehensive monitoring",
    brain_config={
        "brain.neuron_count": 1000,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.enable": True,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    checkpoint_interval=100,
    auto_save=True,
    compression=True,
    encryption=False,
    max_checkpoints=100,
    checkpoint_retention_days=30,
    enable_state_diff=True,
    enable_incremental_checkpoints=True
)

# Create advanced session
advanced_session = AdvancedBrainSession(config)

# Run advanced session with comprehensive management
for step in range(2000):
    # Update world with timing and monitoring
    world_start_time = time.time()
    advanced_session.world.update(0.1)
    world_update_time = time.time() - world_start_time
    
    # Check world update performance
    if world_update_time > 0.1:  # If world update takes too long
        advanced_session.log_performance_issue("world_update_slow", world_update_time)
    
    # Get sensory input
    percept = advanced_session.world.getSensoryPercept()
    
    # Process sensory input with error handling
    try:
        advanced_session.agent.processSensoryInput(percept)
    except Exception as e:
        advanced_session.log_error("sensory_processing", str(e))
        advanced_session.agent.reset_sensory_processing()
    
    # Run brain step with comprehensive monitoring
    brain_step_start = time.time()
    try:
        advanced_session.brain.step(step)
        brain_step_time = time.time() - brain_step_start
        
        # Log brain step performance
        advanced_session.log_brain_step_performance(step, brain_step_time)
        
        # Check brain step performance
        if brain_step_time > 0.05:  # If brain step takes too long
            advanced_session.log_performance_issue("brain_step_slow", brain_step_time)
            
    except Exception as e:
        advanced_session.log_error("brain_step", str(e))
        advanced_session.recover_brain_state()
    
    # Decode motor command
    action = advanced_session.agent.decodeMotorCommand()
    
    # Apply motor command with validation
    try:
        advanced_session.world.applyMotorCommand(action, advanced_session.world.getSimulationTime())
    except Exception as e:
        advanced_session.log_error("motor_command", str(e))
        advanced_session.world.reset_last_action()
    
    # Apply neuromodulation with error handling
    reward = advanced_session.get_reward()
    try:
        advanced_session.agent.applyRewardModulation(reward, 0.0)
    except Exception as e:
        advanced_session.log_error("reward_modulation", str(e))
        advanced_session.agent.reset_reward_modulation()
    
    # Update development with monitoring
    development_start = time.time()
    try:
        advanced_session.agent.updateDevelopment(0.1)
        development_time = time.time() - development_start
        advanced_session.log_development_performance(step, development_time)
    except Exception as e:
        advanced_session.log_error("development_update", str(e))
        advanced_session.agent.reset_development()
    
    # Checkpoint session periodically
    if step % advanced_session.config.checkpoint_interval == 0:
        checkpoint_name = f"checkpoint_step_{step}"
        advanced_session.checkpoint(checkpoint_name)
        advanced_session.log_checkpoint_created(checkpoint_name, step)
    
    # Log step metrics
    if step % 100 == 0:
        step_metrics = {
            'step': step,
            'world_update_time': world_update_time,
            'brain_step_time': brain_step_time,
            'development_time': development_time,
            'reward': reward,
            'firing_neurons': advanced_session.brain.getFiringNeuronCount(),
            'total_spikes': advanced_session.brain.getTotalSpikeCount(),
            'average_firing_rate': advanced_session.brain.getAverageFiringRate(),
            'memory_activity': advanced_session.brain.getMemoryActivity(),
            'development_stage': advanced_session.brain.getDevelopmentalStage(),
            'session_duration': advanced_session.get_session_duration()
        }
        advanced_session.log_step_metrics(step_metrics)

# Save advanced session
advanced_session.save("advanced_session_2000")

# Restore advanced session
restored_session = restore_session_advanced("advanced_session_2000")
```

#### Advanced Session Management with Synchronization

```python
from nlm_utils.session import (
    SessionSyncManager,
    ParallelSessionManager,
    SessionStateSynchronizer,
    SessionPerformanceMonitor
)

class AdvancedSessionSyncManager:
    """Advanced session synchronization manager with conflict resolution."""
    
    def __init__(self, session_configs):
        self.sessions = []
        self.configs = session_configs
        self.sync_manager = SessionSyncManager("advanced_sync")
        self.performance_monitor = SessionPerformanceMonitor("session_performance")
        self.state_synchronizer = SessionStateSynchronizer("state_sync")
        self.conflict_resolver = ConflictResolver("sync_conflicts")
    
    def start_all_sessions(self):
        """Start all sessions with synchronization."""
        for config in self.configs:
            session = AdvancedBrainSession(config)
            self.sessions.append(session)
            
            # Register session with sync manager
            self.sync_manager.register_session(session)
            
            # Register session with performance monitor
            self.performance_monitor.register_session(session)
    
    def run_synchronized_simulation(self, steps):
        """Run simulation with synchronized session state updates."""
        for step in range(steps):
            # Synchronize session states before step
            self.sync_manager.synchronize_states(step)
            
            # Check for synchronization conflicts
            conflicts = self.sync_manager.get_conflicts()
            if conflicts:
                # Resolve conflicts
                resolved_conflicts = self.conflict_resolver.resolve_conflicts(conflicts)
                self.sync_manager.apply_resolutions(resolved_conflicts)
            
            # Run simulation step for each session
            session_metrics = []
            for session in self.sessions:
                step_metrics = self.run_synchronized_step(session, step)
                session_metrics.append(step_metrics)
            
            # Collect and analyze session metrics
            self.analyze_session_metrics(step, session_metrics)
            
            # Synchronize performance metrics across sessions
            performance_metrics = self.performance_monitor.collect_metrics()
            self.sync_manager.synchronize_performance(performance_metrics)
    
    def run_synchronized_step(self, session, step):
        """Run a single step with synchronization."""
        # Pre-synchronization checks
        self.sync_manager.check_pre_sync_requirements(session, step)
        
        # Run synchronized step
        step_result = session.run_step(step)
        
        # Post-synchronization updates
        self.sync_manager.update_after_step(session, step, step_result)
        
        # Return step metrics
        return {
            'session_id': session.id,
            'step': step,
            'brain_step_time': step_result.get('brain_step_time', 0.0),
            'world_update_time': step_result.get('world_update_time', 0.0),
            'development_time': step_result.get('development_time', 0.0),
            'reward': step_result.get('reward', 0.0),
            'firing_neurons': step_result.get('firing_neurons', 0),
            'total_spikes': step_result.get('total_spikes', 0)
        }
```

## Usage Examples

### Complete Enhanced Simulation Example

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, Benchmark, MemoryMonitor,
    AdvancedExperiment, MultiObjectiveExperiment
)

def run_complete_enhanced_simulation():
    """Run complete enhanced simulation with all monitoring and analysis."""
    
    # Configure session with advanced settings
    config = SessionConfigAdvanced(
        name="complete_enhanced_simulation",
        description="Complete simulation with comprehensive monitoring and analysis",
        brain_config={
            "brain.neuron_count": 2000,
            "brain.synapse_density": 0.15,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.001,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development.enable": True,
            "performance.monitoring": True,
            "error_handling.recovery": True,
            "debugging.enable": True
        },
        checkpoint_interval=100,
        auto_save=True,
        compression=True,
        enable_state_diff=True,
        enable_incremental_checkpoints=True
    )
    
    # Create session
    session = AdvancedBrainSession(config)
    
    # Set up comprehensive monitoring
    debug_logger = AdvancedDebugLogger()
    benchmark = Benchmark("complete_enhanced_simulation", warmup_steps=20)
    memory_monitor = MemoryMonitor("simulation_memory")
    
    # Configure multi-objective experiment
    experiment = MultiObjectiveExperiment(
        name="enhanced_learning_optimization",
        objectives=[
            {"name": "learning_speed", "weight": 0.3, "higher_is_better": True},
            {"name": "memory_efficiency", "weight": 0.3, "higher_is_better": True},
            {"name": "stability", "weight": 0.2, "higher_is_better": True},
            {"name": "resource_efficiency", "weight": 0.2, "higher_is_better": True}
        ]
    )
    
    @benchmark
    @memory_monitor
    def run_enhanced_simulation(session, steps):
        """Run enhanced simulation with comprehensive monitoring."""
        for step in range(steps):
            # Run step with comprehensive logging
            step_result = session.run_step_with_logging(step)
            
            # Analyze step results
            analysis = session.analyze_step_results(step, step_result)
            
            # Log comprehensive metrics
            debug_logger.log_simulation_step(step, analysis)
            
            # Run experiment objective evaluation
            experiment_metrics = experiment.evaluate_objectives(
                analysis['brain_metrics'], 
                session.config.brain_config
            )
            
            # Collect experiment data
            experiment.collect_data(step, experiment_metrics)
            
            # Checkpoint periodically
            if step % session.config.checkpoint_interval == 0:
                session.checkpoint(f"enhanced_checkpoint_{step}")
            
            # Update experiment progress
            experiment.update_progress(step, analysis['brain_metrics'])
        
        return experiment.get_results()
    
    # Run enhanced simulation
    experiment_results = run_enhanced_simulation(session, 3000)
    
    # Save session
    session.save("complete_enhanced_simulation")
    
    # Analyze experiment results
    experiment_analysis = experiment.analyze_results(experiment_results)
    experiment.print_results(experiment_analysis)
    
    # Export results
    experiment.export_results("complete_enhanced_results.json")
    
    # Generate comprehensive report
    comprehensive_report = generate_comprehensive_report(session, experiment_results, experiment_analysis)
    
    return {
        'session': session,
        'benchmark_results': benchmark.get_results(),
        'memory_results': memory_monitor.get_summary(),
        'experiment_results': experiment_results,
        'experiment_analysis': experiment_analysis,
        'comprehensive_report': comprehensive_report
    }
```

### Migration from Basic to Enhanced NLM

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, Benchmark, MemoryMonitor,
    safe_function, validate_config
)

# Enhanced brain creation with monitoring
config = SessionConfigAdvanced(
    name="enhanced_simulation",
    brain_config={
        "brain.neuron_count": 1000,
        "plasticity.stdp.enable": True,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    checkpoint_interval=100,
    auto_save=True
)

# Create enhanced session
session = AdvancedBrainSession(config)

# Set up monitoring
debug_logger = AdvancedDebugLogger()
benchmark = Benchmark("enhanced_simulation")

@safe_function("brain_step", retries=3)
def run_step_with_retry(session, step):
    return session.run_step(step)

# Run enhanced simulation
for step in range(1000):
    try:
        run_step_with_retry(session, step)
        
        if step % 100 == 0:
            metrics = session.get_step_metrics(step)
            debug_logger.log_step(step, metrics)
            
    except Exception as e:
        debug_logger.log_error(step, str(e))
        session.recover_from_error(step, e)

# Save enhanced simulation
session.save("enhanced_simulation")
```

## Key Improvements

| Feature | Basic API | Enhanced API |
|---------|-----------|--------------|
| Error Handling | None | Comprehensive error handling with recovery |
| Performance Monitoring | None | Benchmarking, memory monitoring, profiling |
| Brain State Debugging | None | Advanced brain state examination and logging |
| Session Management | None | Advanced session checkpointing and restoration |
| Safe Execution | None | Retry mechanisms and timeout protection |
| Configuration Validation | None | Comprehensive validation utilities |
| Memory Monitoring | None | Real-time memory usage tracking |
| Performance Profiling | None | Detailed performance analysis tools |
| Experiment Management | None | Multi-objective experiment with parameter sweeps |
| Parallel Execution | None | Advanced parallel session management |

## Conclusion

The enhanced NLM Python API provides comprehensive tools for:

- **Robust error handling** with automatic recovery mechanisms
- **Performance monitoring** with benchmarking, memory tracking, and profiling
- **Brain state debugging** with advanced analysis and anomaly detection
- **Session management** with checkpointing, restoration, and synchronization
- **Experiment management** with multi-objective optimization and parameter sweeps
- **Parallel execution** with advanced resource management and scheduling
- **Safe execution** with retry mechanisms and comprehensive error handling

These enhancements make the NLM API more powerful, reliable, and user-friendly while maintaining backward compatibility. Whether you're running simple simulations or complex experiment suites, the enhanced API provides the tools you need for successful NLM development.

For more detailed information about specific features, refer to the individual module documentation and examples provided throughout this guide.
