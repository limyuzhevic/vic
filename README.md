# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Advanced Debugging Capabilities

The NLM Python package includes comprehensive debugging and logging capabilities to help with development, analysis, and troubleshooting:

### 1. Advanced Logging System

The logging module provides:

- **Multi-level logging**: DEBUG, INFO, WARNING, ERROR, CRITICAL
- **Context-aware logging**: Brain regions, simulation steps, agent IDs
- **Log file rotation**: Automatic rotation based on file size
- **Structured logging**: JSON format for machine-readable logs
- **Specialized loggers**: Separate loggers for brain, memory, simulation, plasticity, visualization, and checkpoint components

**Usage Example:**
```python
from pynlm.logging import setup_logging, get_brain_logger, log_simulation_step

# Setup logging with JSON format
logger = setup_logging("simulation.log", "DEBUG")

# Get specialized loggers
brain_logger = get_brain_logger()
sim_logger = get_simulation_logger()

# Context-aware logging
log_simulation_step("simulation", step=100, message="Completed time step", temperature=0.7)
```

### 2. Debug Utilities

The debug module provides tools for debugging and troubleshooting:

- **Brain state inspection**: Create and compare brain state snapshots
- **Simulation step debugging**: Add breakpoints and watch variables
- **Connection validation**: Validate neural connectivity
- **Memory usage monitoring**: Track memory consumption and detect leaks
- **Performance profiling**: Profile component performance and identify bottlenecks

**Usage Example:**
```python
from pynlm.debug import (
    get_brain_state_inspector, get_simulation_debugger,
    get_connection_validator, get_memory_monitor
)

# Inspect brain states
inspector = get_brain_state_inspector()
snapshot = inspector.create_snapshot(step=10, neurons=neuron_data)
comparison = inspector.compare_snapshots(5, 10)

# Debug simulation
debugger = get_simulation_debugger()
debugger.add_breakpoint(50)  # Stop at step 50
debugger.add_watch_var("error_rate")  # Watch error rate variable
```

### 3. State Inspection

Tools for inspecting brain state and simulation state:

- **Brain state snapshots**: Capture complete brain state at any step
- **Connection inspection**: Analyze neural connectivity patterns
- **Memory content inspection**: Examine memory contents and access patterns
- **Neural activity visualization**: Create heatmaps and connectivity plots

**Usage Example:**
```python
from pynlm.state_inspection import (
    get_brain_state_snapshotter, get_connection_inspector,
    get_memory_content_inspector
)

# Snapshot brain state
snapshotter = get_brain_state_snapshotter()
snapshot = snapshotter.inspect_state()

# Inspect connections
inspector = get_connection_inspector()
connectivity_analysis = inspector.inspect_connectivity(connectivity_data)
```

### 4. Visualization Debugging

Debugging tools for visualization components:

- **Visualization state inspection**: Track visualization state changes
- **Rendering debugging**: Debug rendering issues with hooks and capture mode
- **Data validation**: Validate visualization data structures
- **Interactive debugging**: Start debug sessions for interactive visualization debugging

**Usage Example:**
```python
from pynlm.visualization_debug import (
    get_visualization_debugger, get_rendering_debugger, get_data_validator
)

# Debug visualization
viz_debugger = get_visualization_debugger()
viz_debugger.start_debug_session("session1", {"type": "heatmap"})
validation = viz_debugger.validate_visualization_data("session1", data)
```

### 5. Error Reporting

Enhanced error reporting and diagnostics:

- **Detailed error messages**: Include context, stack traces, and recovery suggestions
- **Error categorization**: Classify errors by type and severity
- **Error recovery suggestions**: Provide actionable recovery suggestions
- **Error pattern analysis**: Analyze error patterns over time

**Usage Example:**
```python
from pynlm.error_reporting import (
    get_error_reporter, report_brain_state_error,
    ErrorSeverity, ErrorCategory
)

# Report errors with context
error_reporter = get_error_reporter()
error = report_brain_state_error(
    ValueError("Neurons exceeded firing rate threshold"),
    brain_region="hippocampus",
    simulation_step=50,
    recovery_suggestion="Reduce neural excitability"
)
```

### 6. Export Debugging

Tools for exporting debug information:

- **Debug report generation**: Generate comprehensive debug reports
- **State export**: Export brain state for analysis
- **Configuration export**: Export configuration settings
- **Performance reports**: Generate detailed performance reports

**Usage Example:**
```python
from pynlm.export_debug import (
    get_debug_report_generator, get_state_exporter,
    get_configuration_exporter
)

# Generate debug report
report_generator = get_debug_report_generator()
report = report_generator.generate_comprehensive_report("session1", data_sources)

# Export state
state_exporter = get_state_exporter()
exported_files = state_exporter.export_simulation_state(simulation_data)
```

### 7. Performance Monitoring

Performance tracking and profiling capabilities:

- **CPU and memory tracking**: Monitor resource usage
- **Simulation timing metrics**: Track simulation performance
- **Component-level monitoring**: Profile individual components
- **Bottleneck identification**: Identify performance bottlenecks

**Usage Example:**
```python
from pynlm.debug import get_performance_profiler

# Profile performance
profiler = get_performance_profiler()
profiler.start_profiling("simulation", interval=0.1)

# Run some work...

# Stop profiling
results = profiler.stop_profiling("simulation")
bottlenecks = profiler.analyze_bottlenecks()
```

## Comprehensive Debugging Workflows

### Workflow 1: Debugging Simulation Issues

1. **Setup logging** with appropriate levels and context
2. **Start debug session** for the simulation
3. **Add breakpoints** at specific steps for manual debugging
4. **Watch critical variables** that might indicate issues
5. **Monitor performance** to identify bottlenecks
6. **Capture render frames** if debugging visualization
7. **Generate debug reports** for analysis

### Workflow 2: Analyzing Brain State Evolution

1. **Take brain state snapshots** at regular intervals
2. **Compare snapshots** to identify changes
3. **Analyze neuron states** for abnormalities
4. **Validate connectivity** health
5. **Inspect memory contents** for corruption
6. **Export analysis** results

### Workflow 3: Performance Optimization

1. **Profile all components** using performance profiler
2. **Identify bottlenecks** using analysis tools
3. **Monitor memory usage** for leaks
4. **Validate data integrity** for performance issues
5. **Generate performance reports** with recommendations

## Command Line Tools

The debugging modules provide command-line interfaces for common tasks:

- `nlm-debug-report`: Generate debug reports
- `nlm-state-export`: Export simulation state
- `nlm-performance-report`: Generate performance reports
- `nlm-log-analyzer`: Analyze log files

For detailed usage information, refer to the documentation in the `docs/` directory.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
