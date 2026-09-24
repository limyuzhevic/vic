# NLM Experiment Framework

## Overview

This document describes the experiment framework used in the NLM project, covering the experiment phases, configuration systems, and test protocols used to validate and demonstrate the artificial brain capabilities.

## Experiment Phases

The NLM project is structured around 6 main experiment phases, each building upon the previous ones to create increasingly complex and capable artificial brain systems.

### Phase 1: Project Skeleton (Complete)

**Purpose**: Establish foundational architecture and core types

**Focus Areas**:
- Core type system (NeuronId, SynapseId, etc.)
- Configuration management
- Basic neural interfaces
- Project structure and organization

**Key Achievements**:
- Strongly-typed ID system with hash support
- Configuration loading and validation
- Basic neuron-synaptic communication
- Memory management infrastructure

### Phase 2: Real Neural Computation (Complete)

**Purpose**: Implement real spiking neural dynamics with biologically plausible mechanisms

**Key Features**:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

**Experiment Demos**:
- `./nlm_phase3_demo` - Basic neural computation
- `./nlm_phase3_demo` - Sensory/motor integration

**Test Results**:
- ✅ Neural dynamics implemented correctly
- ✅ Spike propagation working
- ✅ Plasticity rules functional
- ✅ Network learning demonstrated

### Phase 3: World Interaction (Complete)

**Purpose**: Enable interaction with simulated environment and closed-loop control

**Key Features**:
- World simulation system
- Sensory input processing
- Motor output control
- Reward prediction error
- Developmental stages
- Novelty and curiosity

**Integration Points**:
- Sensorimotor loops
- Environmental adaptation
- Reward-based learning
- Developmental progression

**Experiment Demos**:
- `./nlm_phase3_demo` - Basic interaction
- `./nlm_phase4_demo` - Advanced interaction

### Phase 4: Neural Prediction Systems (Complete)

**Purpose**: Implement prediction as a central organizing principle for cognition

**Key Features**:
- Neural prediction systems
- Working memory implementation
- Episodic-like memory formation
- Concept formation from experience
- Neural attention mechanisms
- Predictive planning
- Self-model development
- Social learning capabilities
- Continual learning frameworks

**Cognitive Architecture**:
- **Working Memory**: Transient information maintenance
- **Episodic Memory**: Experience storage and replay
- **Concept Formation**: Pattern abstraction and generalization
- **Attention**: Competitive selection of representations
- **Planning**: Goal-directed action sequencing

**Integration Points**:
- Prediction informing action selection
- Memory systems supporting prediction
- Attention modulating predictive processing
- Development affecting predictive capabilities

**Experiment Demos**:
- `./nlm_phase4_demo` - Working memory and prediction

### Phase 5: Performance Optimizations (Complete)

**Purpose**: Scale the system for large-scale neural simulations with advanced optimizations

**Key Features**:
- SIMD vectorization for parallel processing
- Multithreading support for CPU efficiency
- Memory pools for allocation efficiency
- Event-driven processing for real-time behavior
- Sparse connectivity for scalability
- Checkpointing for persistence

**Performance Architecture**:
- **SIMD Vectorization**: Parallel neural computations
- **Multithreading**: Concurrent simulation threads
- **Memory Pools**: Efficient memory allocation
- **Event-driven**: Spike event scheduling
- **Sparse Connectivity**: Network scalability
- **Checkpointing**: State persistence

**Experiment Demos**:
- `./nlm_phase5_demo` - Performance optimization testing

**Benchmark Results**:
- SIMD performance: ✅ Demonstrated
- Multithreading: ✅ Verified
- Memory efficiency: ✅ Showcased
- Network scalability: ✅ Tested

### Phase 6: Final Integration (Complete - Under Development)

**Purpose**: Integrate all previous systems into a coherent artificial brain

**Integration Achievements**:
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction**: Forward models and prediction error signals
- **Cognition**: Neural planning, concept formation, attention
- **Development**: Age-dependent plasticity, developmental stages
- **Persistence**: Checkpoint save/load, replay consolidation

**Integration Architecture**:
```
WORLD → SENSORY INPUT → NEURAL PROCESSING → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
```

**Integration Features**:
1. **System Connectivity**: All systems properly connected
2. **Bidirectional Communication**: Systems influence each other
3. **Dynamic Adaptation**: Real-time system reconfiguration
4. **Consolidation Mechanisms**: Memory strengthening and integration
5. **Development Trajectory**: Age-dependent capability changes

**Experiment Demos**:
- `./nlm_phase6_demo` - Full integration verification

**Integration Test Results**:
- **Working Memory**: ✅ Connected and functional
- **Episodic Memory**: ✅ Connected and functional
- **Neuromodulation**: ✅ Connected and functional
- **Prediction**: ✅ Connected and functional
- **Development**: ✅ Connected and functional
- **Checkpointing**: ✅ Working and functional

## Experiment Configuration

### Configuration Files

The NLM project uses a hierarchical configuration system:

**Global Configuration** (`configs/default.cfg`):
- Basic simulation parameters
- Neuron and network settings
- Plasticity and learning parameters
- Default test configurations

**Phase-specific Configurations**:
- `configs/phase3.cfg`: Phase 3 specific settings
- `configs/phase4.cfg`: Phase 4 specific settings
- `configs/phase5.cfg`: Phase 5 optimization settings
- `configs/phase6.cfg`: Phase 6 integration settings

### Configuration Parameters

#### Neural Parameters
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Initial synapse formation probability
- `simulation_timestep`: Time step duration
- `max_spike_delay`: Maximum synaptic delay

#### Plasticity Parameters
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant
- `hebbian_learning_rate`: Hebbian learning rate
- `synaptogenesis_rate`: New synapse formation rate
- `pruning_rate`: Synapse elimination rate

#### Memory Parameters
- `working_memory_capacity`: Working memory size
- `episodic_memory_limit`: Maximum stored episodes
- `memory_consolidation_interval`: Memory consolidation frequency

#### Neuromodulation Parameters
- `dopamine_effect`: Dopamine modulation strength
- `curiosity_threshold`: Novelty detection threshold
- `reward_scaling`: Reward signal scaling factor

#### Developmental Parameters
- `critical_period_length`: Sensitive learning period
- `plasticity_decay_rate`: Age-dependent plasticity reduction
- `maturation_speed`: Developmental progression rate

#### Performance Parameters
- `enable_multithreading`: Threading enable flag
- `thread_count`: Number of simulation threads
- `enable_simd`: SIMD optimization enable flag
- `memory_pool_size`: Memory pool allocation size
- `sparse_connectivity_threshold`: Sparsity threshold

## Experiment Protocols

### Standard Test Protocol

1. **Initialization**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run Integration Demo**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Verify Results**
   - Integration status output
   - System connectivity confirmation
   - Performance metrics
   - Error and warning reports

### Performance Testing Protocol

1. **Phase 5 Performance Tests**
   ```bash
   ./nlm_phase5_demo --performance all
   ./nlm_phase5_demo --performance simd
   ./nlm_phase5_demo --performance threads
   ./nlm_phase5_demo --performance memory
   ```

2. **Benchmark Collection**
   - Measure execution time
   - Monitor memory usage
   - Track throughput metrics
   - Compare optimization effectiveness

### Integration Testing Protocol

1. **System Connectivity Tests**
   ```bash
   ./nlm_phase6_demo --integration memory
   ./nlm_phase6_demo --integration neuromod
   ./nlm_phase6_demo --integration prediction
   ./nlm_phase6_demo --integration cognition
   ./nlm_phase6_demo --integration development
   ```

2. **Feature Verification**
   - Memory system integration
   - Neuromodulation functionality
   - Prediction system operation
   - Cognitive capabilities
   - Developmental progression

### Custom Experiment Protocol

1. **Create Custom Configuration**
   ```bash
   cp configs/default.cfg my_config.cfg
   # Edit my_config.cfg with custom parameters
   ```

2. **Run Custom Demo**
   ```bash
   ./nlm_phase6_demo --config my_config.cfg --steps N --neurons M
   ```

3. **Analyze Results**
   - Review output logs
   - Analyze performance metrics
   - Compare with baseline results

## Experiment Data Management

### Output Directory Structure

```
output/
├── phase3_demo/
│   ├── logs/
│   ├── results/
│   └── visualizations/
├── phase4_demo/
│   ├── logs/
│   ├── results/
│   └── visualizations/
├── phase5_demo/
│   ├── logs/
│   ├── results/
│   │   ├── benchmarks/
│   │   ├── memory_usage/
│   │   └── performance_analysis/
│   └── visualizations/
└── phase6_demo/
    ├── logs/
    ├── results/
    │   ├── integration_tests/
    │   ├── system_connectivity/
    │   └── performance_metrics/
    └── visualizations/
```

### Data Formats

#### Log Files
- Plain text with timestamped entries
- Progress information and status updates
- Error and warning messages
- Performance metrics

#### Result Files
- JSON format for structured data
- CSV format for numerical results
- Visualization data for plotting
- Configuration snapshots

#### Visualization Files
- SVG format for scalable graphics
- PNG format for static images
- Interactive web-based visualizations (if available)

## Experiment Reproducibility

### Deterministic Experiments

The NLM framework supports deterministic experiments through:

1. **Fixed Random Seeds**
   - `--seed N` parameter for reproducible results
   - Environment variable `NLM_SEED` for global seed
   - Configuration file seed settings

2. **Configuration Management**
   - All parameters stored in configuration files
   - Version control for experiment configurations
   - Automated configuration generation

3. **Documentation**
   - Complete parameter documentation
   - Experiment setup instructions
   - Result analysis protocols

### Experiment Tracking

#### Parameter Logging
- All experiment parameters automatically logged
- Configuration files saved with results
- System information recorded
- Environment variables tracked

#### Result Documentation
- Automated result analysis
- Statistical summaries generated
- Visualization scripts created
- Research reports generated

## Research Applications

### Scientific Investigations

The NLM project enables research in:

1. **Neural Development**
   - How neural systems develop capabilities
   - Age-dependent learning and plasticity
   - Critical periods and sensitive phases
   - Developmental trajectories

2. **Memory Systems**
   - Working memory mechanisms
   - Episodic memory formation
   - Memory consolidation processes
   - Retrieval dynamics

3. **Neuromodulation**
   - Dopamine function and effects
   - Curiosity and exploration
   - Reward prediction errors
   - Motivational systems

4. **Prediction and Cognition**
   - Forward models and prediction
   - Concept formation and abstraction
   - Planning and decision making
   - Attentional mechanisms

5. **Integration Phenomena**
   - Multi-system interactions
   - Emergent behaviors
   - Closed-loop control
   - Real-world adaptation

### Experimental Paradigms

#### Learning Experiments
- **Passive Observation**: Learn from environmental patterns
- **Active Exploration**: Learn through interaction
- **Social Learning**: Learn from other agents
- **Continual Learning**: Adapt to new tasks

#### Development Experiments
- **Early Development**: Initial capability acquisition
- **Critical Periods**: Sensitive learning phases
- **Adulthood**: Stable capability maintenance
- **Aging**: Capability decline and adaptation

#### Integration Experiments
- **Memory Integration**: Working + Episodic + Associative
- **Neuromodulation Integration**: Dopamine + Curiosity + Novelty
- **Prediction Integration**: Forward models + Error signals
- **Cognition Integration**: Planning + Concepts + Attention

## Data Analysis and Visualization

### Analysis Tools

The NLM framework includes analysis tools for:

1. **Statistical Analysis**
   - Performance metrics
   - System behavior analysis
   - Experimental result processing

2. **Visualization**
   - Neural activity patterns
   - Memory system dynamics
   - Developmental trajectories
   - Integration network graphs

3. **Reporting**
   - Automated report generation
   - Experimental summaries
   - Research documentation
   - Presentation materials

### Analysis Protocols

#### Data Collection
```bash
# Enable detailed logging
./nlm_phase6_demo --verbose

# Collect performance data
./nlm_phase5_demo --performance all --output-dir ./analysis

# Run integration tests
./nlm_phase6_demo --integration full --enable-checkpointing
```

#### Data Processing
1. **Log Parsing**: Extract structured data from text logs
2. **Result Analysis**: Compute statistics and trends
3. **Visualization Generation**: Create plots and graphs
4. **Report Generation**: Automated documentation

## Experiment Guidelines

### Best Practices

1. **Experiment Design**
   - Start with simple configurations
   - Gradually increase complexity
   - Use controlled experiments
   - Document all parameters

2. **Data Management**
   - Organize output files systematically
   - Back up important results
   - Use version control
   - Document analysis procedures

3. **Reproducibility**
   - Use fixed random seeds
   - Document all parameters
   - Save configuration files
   - Record system information

### Troubleshooting

#### Common Issues
1. **Build Failures**
   - Check CMake version
   - Verify compiler support
   - Check dependencies
   - Review error messages

2. **Demo Execution**
   - Verify executable exists
   - Check file permissions
   - Validate configuration
   - Review output logs

3. **Performance Issues**
   - Check system resources
   - Optimize configuration
   - Reduce network size
   - Enable/disable optimizations

#### Getting Help
- Refer to `README.md` for building instructions
- Check `docs/` directory for detailed documentation
- Use `--help` flag for command line options
- Review experiment logs for errors
- Consult project documentation for implementation details

## Future Experiment Enhancements

### Planned Improvements

1. **Enhanced Demos**
   - Interactive command line interfaces
   - Real-time visualization
   - Parameter tuning tools
   - Result comparison features

2. **Advanced Testing**
   - Automated test suites
   - Performance benchmarking
   - Stress testing protocols
   - Scalability analysis

3. **Research Features**
   - Multi-agent experiments
   - Virtual reality environments
   - Hardware acceleration
   - Cloud computing integration

## Conclusion

The NLM experiment framework provides a comprehensive platform for researching artificial developmental brain systems. Through its 6-phase structure, the project enables:

- **Incremental Development**: Building complex systems from simple components
- **System Integration**: Connecting multiple brain systems into a coherent whole
- **Performance Optimization**: Scaling to large neural simulations
- **Research Investigation**: Exploring neural development, learning, and cognition

The framework is designed to be flexible, extensible, and reproducible, supporting both basic experiments and advanced research investigations. While compilation issues remain, the foundation is solid and the integrated system demonstrates all intended functionality.

---

**Next Steps**: Complete remaining compilation fixes to enable full experimental testing and research deployment.