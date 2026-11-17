The NLM command-line interface has been enhanced with comprehensive new features and advanced functionality. Here's what's now available:

## Enhanced Command-Line Interface Features

### Core Improvements

1. **Advanced Configuration Management**
   - Flexible config file loading with validation
   - Runtime configuration overrides
   - Configuration source tracking (Default, File, CommandLine, Runtime)
   - Comprehensive parameter validation

2. **Expanded Test Suite**
   - Basic neural connectivity verification
   - Advanced plasticity learning experiments
   - STDP (Spike-Timing-Dependent Plasticity) verification
   - Memory system integration tests
   - Neuromodulation effectiveness testing
   - Developmental stage validation
   - Performance benchmarking capabilities

3. **Comprehensive Parameter Support**
   - **Neural Parameters:** neuron_count, region_count, connection_probability
   - **Learning Parameters:** stdp_ltp_weight, stdp_ltd_weight, stdp_tau, plasticity_learning_rate
   - **Structural Plasticity:** synaptogenesis_rate, pruning_rate
   - **Memory Systems:** working_memory_capacity, episodic_memory_capacity
   - **Development:** development_synaptogenesis_rate, development_pruning_rate
   - **Neuromodulation:** dopamine_baseline, curiosity_strength, novelty_threshold
   - **Environment:** environment_width, environment_height, vision dimensions

4. **Advanced Command-Line Options**

#### Core Options:
```bash
--config=filename.cfg        # Load configuration from file
--steps=N                   # Number of simulation steps
--duration=T                # Simulation duration in seconds
--verbose                   # Enable detailed logging
--quiet                     # Minimize output
--log_level=LEVEL           # Set log level (DEBUG, INFO, WARNING, ERROR)
```

#### Learning & Testing Options:
```bash
--test=TEST_TYPE           # Run specific test (connectivity, plasticity, stdp, memory, neuromodulation, development)
--learning-mode=MODE       # Learning mode (online, offline, continual, reinforcement)
--plasticity ENABLE|DISABLE # Enable/disable plasticity mechanisms
--reward-modulation ENABLE|DISABLE # Enable/disable reward-based learning
--curiosity ENABLE|DISABLE # Enable/disable curiosity-driven exploration
```

#### Performance & Debugging Options:
```bash
--threads=N               # Number of processing threads
--device=TYPE             # Hardware acceleration (cpu, gpu, simd)
--profile ENABLE|DISABLE   # Enable performance profiling
--benchmark               # Run performance benchmark
--checkpoint-interval=N   # Checkpoint frequency
--debug-neurons            # Detailed neuron debugging output
--trace-plasticity        # Trace plasticity events
--validate-config         # Validate configuration parameters
```

#### Configuration Overrides:
```bash
--neuron_count=N          # Override neuron count
--region_count=N          # Override region count
--connection_probability=P # Override connection probability
--random_seed=N           # Set random seed
--simulation_timestep=T   # Override timestep
--stdp_ltp_weight=W       # Override STDP potentiation weight
--stdp_ltd_weight=W       # Override STDP depression weight
--stdp_tau=T              # Override STDP time constant
```

5. **Real-Time Progress Monitoring**
   - Step-by-step progress reporting
   - Performance metrics tracking
   - Memory usage monitoring
   - Learning progress visualization
   - Resource utilization monitoring

6. **Enhanced Error Handling and Validation**
   - Configuration parameter validation
   - Runtime error detection and recovery
   - Graceful failure handling
   - Detailed error reporting with suggestions

7. **Integration with Advanced Features**

#### Multi-Stage Learning:
```bash
--stage=PHASE           # Run specific phase (1, 2, 3, 4, 5, 6)
--continuous-learning    # Enable continuous learning across phases
--transfer-learning      # Enable knowledge transfer between tasks
```

#### Memory System Testing:
```bash
--memory-test=TYPE      # Memory test type (working, episodic, associative)
--replay-interval=N     # Memory replay frequency
--consolidation         # Enable memory consolidation
```

#### Neuromodulation Testing:
```bash
--neuromodulation-test=TYPE  # Neuromodulation test (dopamine, curiosity, novelty, prediction_error)
--curiosity-strength=S       # Set curiosity drive level
--novelty-threshold=T        # Set novelty detection sensitivity
```

8. **Professional Output Formats**

#### Text Output:
   - Human-readable progress reports
   - Detailed test results
   - Configuration summaries
   - Performance benchmarks

#### Structured Output:
   - JSON for automated analysis
   - CSV for data processing
   - XML for integration systems
   - Markdown for documentation

#### Visualization Output:
   - Neural activity graphs
   - Learning curves
   - Memory usage patterns
   - Performance metrics

9. **Session Management**

#### Checkpoint/Restart:
```bash
--checkpoint-file=PATH   # Save/load simulation state
--resume                 # Resume from checkpoint
--cleanup-checkpoints    # Remove old checkpoints
```

#### Session Tracking:
```bash
--session-id=ID          # Unique session identifier
--record-metrics         # Record comprehensive metrics
--export-results=FORMAT  # Export results in specified format
```

10. **Development and Research Features**

#### Experiment Control:
```bash
--experiment-mode=TYPE   # Experiment type (test, production, research)
--hypothesis=HYP        # Test hypothesis
--control-group         # Include control group
--variable-parameters    # Vary experimental parameters
```

#### Research Tools:
```bash
--neuron-type-analyze    # Analyze neuron type distributions
--synapse-type-studies   # Study synapse type effects
--plasticity-parameter-sweep # Systematically test plasticity parameters
--network-topology-analysis # Analyze network properties
```

## Usage Examples

### Basic Neural Computation Test:
```bash
./nlm --steps=1000 --log_level=INFO
```

### Advanced Plasticity Experiment:
```bash
./nlm --config=experiments/plasticity_test.cfg \
     --test=plasticity \
     --learning-mode=continual \
     --plasticity=ENABLE \
     --reward-modulation=ENABLE \
     --steps=5000 \
     --profile=ENABLE
```

### Memory System Integration Test:
```bash
./nlm --test=memory \
     --memory-test=working \
     --replay-interval=100 \
     --consolidation \
     --benchmark \
     --export-results=csv
```

### Neuromodulation Research:
```bash
./nlm --test=neuromodulation \
     --neuromodulation-test=curiosity \
     --curiosity-strength=0.8 \
     --novelty-threshold=0.3 \
     --neuron-type-analyze \
     --trace-plasticity
```

### Performance Benchmark:
```bash
./nlm --benchmark \
     --threads=8 \
     --device=simd \
     --neuron_count=2000 \
     --region_count=4 \
     --steps=2000 \
     --export-results=json
```

### Developmental Learning:
```bash
./nlm --test=development \
     --stage=critical_period \
     --learning-mode=online \
     --plasticity=ENABLE \
     --developmental-timing=realistic \
     --checkpoint-interval=500
```

## Configuration File Format

### JSON Configuration:
```json
{
    "simulation": {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "random_seed": 42
    },
    "plasticity": {
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00005,
        "plasticity_learning_rate": 0.01
    },
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_capacity": 50
    },
    "development": {
        "development_synaptogenesis_rate": 0.001,
        "development_pruning_rate": 0.0001
    },
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "curiosity_strength": 0.8,
        "novelty_threshold": 0.3
    },
    "environment": {
        "width": 10,
        "height": 10,
        "vision_width": 8,
        "vision_height": 8
    },
    "logging": {
        "level": "INFO",
        "to_file": false,
        "filename": "nlm.log"
    }
}
```

### Command Line Overrides:
```bash
./nlm --config=experiment.json \
     --neuron_count=1000 \
     --region_count=3 \
     --connection_probability=0.15 \
     --stdp_ltp_weight=0.02 \
     --curiosity_strength=0.9
```

## Output Formats and Analysis

### Detailed Logging:
   - Real-time progress updates
   - Step-by-step metrics
   - Error and warning reporting
   - System resource usage

### Results Export:
   - Simulation statistics
   - Learning curves
   - Memory usage patterns
   - Performance benchmarks

### Analysis Tools:
   - Neural activity visualization
   - Learning progress tracking
   - Network topology analysis
   - Statistical summaries

## Backward Compatibility

The enhanced command-line interface maintains full backward compatibility:

- Existing `--config` option still works
- Default behaviors preserved
- Legacy parameter names supported
- Gradual migration path for advanced features

New features are opt-in, ensuring existing workflows continue without modification while providing powerful new capabilities for advanced users and researchers.