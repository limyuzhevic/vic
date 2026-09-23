# NLM Project Command-Line Interface Enhancement Design Document

## 1. Current CLI State Analysis

### Existing CLI Components

#### Primary Entry Point (`src/main.cpp`)
- **Phase**: Phase 2: Real Neural Computation
- **CLI Capabilities**:
  - Basic argument parsing for `--config=file.cfg`
  - Configuration file loading with command-line overrides
  - Limited to configuration management only
  - No runtime control or inspection capabilities
  - No system monitoring or debugging tools

#### Phase 6 Demo (`src/experiments/Phase6Demo.cpp`)
- **Phase**: Phase 6: Final Integration
- **CLI Capabilities**:
  - No command-line arguments
  - Hardcoded configuration (`neuronCount = 500`, `maxSteps = 2000`)
  - Fixed test scenarios with no runtime control
  - Limited to demonstration purposes only

#### Phase 4 Demo (`src/experiments/Phase4Demo.cpp`)
- **Phase**: Phase 4: Advanced Capabilities
- **CLI Capabilities**:
  - Basic argument parsing (experimental mode, neuron count, etc.)
  - Limited configuration options
  - Still no runtime control or system inspection

### Current Limitations

1. **Command Availability**: Only basic config file loading
2. **Runtime Control**: No ability to control simulation during execution
3. **System Inspection**: No tools to examine brain state, memory contents, or system metrics
4. **Debugging Capabilities**: Minimal debugging information
5. **Research Tools**: No specialized tools for research and experimentation
6. **User Roles**: No distinction between basic and advanced user access

## 2. Missing Advanced Commands for Advanced Users

### Simulation Control Commands
```bash
# Pause/resume simulation
nlm pause
nlm resume

# Speed control
nlm speed 10.0
nlm speed 0.5

# Step execution
nlm step 10

# Simulation control
nlm stop
nlm restart
```

### System Inspection Commands
```bash
# Brain state inspection
nlm inspect brain --regions
nlm inspect neurons --statistics
nlm inspect synapses --distribution

# Memory system inspection
nlm inspect memory working --content
nlm inspect memory episodic --recent
nlm inspect memory associative --patterns

# Neuromodulation inspection
nlm inspect neuromodulation --levels
nlm inspect dopamine --history
nlm inspect curiosity --triggers

# Prediction system inspection
nlm inspect prediction --confidence
nlm inspect errors --history

# Cognition system inspection
nlm inspect attention --focus
nlm inspect concepts --formation
nlm inspect planning --sequences
```

### Debugging and Research Tools
```bash
# Debugging commands
nlm debug trace --start --duration 1000
nlm debug log-level set info
nlm debug memory-leak --detect
nlm debug performance --benchmark

# Research commands
nlm research replay --episodes 10 --strategy random
nlm research ablation --region 1 --duration 500
nlm research perturbation --neuron-type sensory --intensity 2.0
nlm research experiment --template learning-curve --parameters "epochs=100,lr=0.01"
```

### Memory Management Commands
```bash
# Memory operations
nlm memory save --path /tmp/brain_state.json --compress
nlm memory load --path /tmp/brain_state.json
nlm memory replay --episodes 5 --strategy ltp-reinforced
nlm memory consolidate --importance-threshold 0.8
nlm memory query --pattern "sensory_" --region 1
```

### Development and Persistence Commands
```bash
# Development commands
nlm development set-stage 3
nlm development plasticity-rate 0.01
nlm development excitability 1.5

# Checkpoint commands
nlm checkpoint create --name "after_1000-steps"
nlm checkpoint list
nlm checkpoint restore --name "after_1000-steps"
nlm checkpoint compare --before "step_500" --after "step_1500"
```

## 3. New Command Categories and Functionality

### Category 1: Simulation Control
- **Purpose**: Runtime control of neural simulation
- **Commands**: Pause/Resume, Speed Control, Step Execution, Simulation Management
- **Integration**: Direct brain control through `Brain::pause()`, `Brain::resume()`
- **Advanced Features**: Multi-threaded control, simulation scripting

### Category 2: System Inspection
- **Purpose**: Examine internal brain state and metrics
- **Commands**: Brain State, Memory Contents, Neuromodulation Levels, Prediction Confidence
- **Integration**: Through existing getter methods (`brain->getTotalSpikeCount()`, `memory->getActiveMemories()`)
- **Advanced Features**: Real-time monitoring, historical data analysis

### Category 3: Debugging and Research
- **Purpose**: Advanced debugging and experimental control
- **Commands**: Trace Logging, Memory Leak Detection, Performance Benchmarking, Experimental Ablation
- **Integration**: Through debug logging systems and existing experiment frameworks
- **Advanced Features**: Automated experiment tracking, data export

### Category 4: Memory Management
- **Purpose**: Memory system operations and analysis
- **Commands**: Save/Load, Replay, Consolidation, Query, Pattern Analysis
- **Integration**: Through existing memory system APIs
- **Advanced Features**: Memory compression, cross-region queries, semantic search

### Category 5: Development and Persistence
- **Purpose**: Development stage control and checkpoint management
- **Commands**: Development Stage Setting, Plasticity Control, Checkpoint Management
- **Integration**: Through development system and checkpoint APIs
- **Advanced Features**: Development timeline visualization, checkpoint branching

### Category 6: Research and Analysis
- **Purpose**: Specialized research tools and data analysis
- **Commands**: Replay Strategies, Ablation Studies, Perturbation Analysis, Experiment Templates
- **Integration**: Through experiment runner and analysis frameworks
- **Advanced Features**: Parameter optimization, result correlation, automated reporting

## 4. Integration Points with Existing Architecture

### Brain System Integration

#### Memory Systems Integration
```cpp
// Command: nlm inspect memory working --content
void MemoryInspectionCommand::execute() {
    auto workingMemory = brain->getWorkingMemory();
    auto traces = workingMemory->getActiveTraces();
    for (const auto& trace : traces) {
        logMemoryTrace(trace);
    }
}
```

#### Neuromodulation Integration
```cpp
// Command: nlm inspect neuromodulation --levels
void NeuromodulationInspectionCommand::execute() {
    auto neuromod = brain->getNeuromodulation();
    auto dopamine = neuromod->getDopamineLevel();
    auto curiosity = neuromod->getCuriosityLevel();
    auto novelty = neuromod->getNoveltyLevel();
    logNeuromodulationState(dopamine, curiosity, novelty);
}
```

#### Prediction System Integration
```cpp
// Command: nlm inspect prediction --confidence
void PredictionInspectionCommand::execute() {
    auto prediction = brain->getPredictionSystem();
    auto confidence = prediction->getCurrentConfidence();
    auto predictionError = prediction->getPredictionError();
    logPredictionMetrics(confidence, predictionError);
}
```

### Development System Integration
```cpp
// Command: nlm development set-stage 3
void DevelopmentCommand::execute() {
    auto development = brain->getDevelopment();
    development->setCurrentStage(3);
    logDevelopmentStage(3);
}
```

### Checkpoint Integration
```cpp
// Command: nlm checkpoint create --name "experiment_1"
void CheckpointCommand::execute() {
    auto persistence = brain->getPersistence();
    persistence->createCheckpoint("experiment_1");
    logCheckpointCreation("experiment_1");
}
```

### Experiment System Integration
```cpp
// Command: nlm research experiment --template learning-curve
void ResearchCommand::execute() {
    auto experimentRunner = brain->getExperimentRunner();
    auto result = experimentRunner->runExperiment(
        "learning-curve",
        {{"epochs", "100"}, {"lr", "0.01"}}
    );
    logExperimentResults(result);
}
```

## 5. User Role-Based Command Access

### Basic User Role
**Available Commands**:
- `nlm run --config file.cfg` (Basic execution)
- `nlm memory load --path file.json` (Load saved states)
- `nlm help` (Basic help)

**Restrictions**:
- No simulation control (pause/resume/step)
- No system inspection
- No debugging tools
- Limited to read-only operations
- No advanced research capabilities

### Advanced User Role
**Available Commands**:
- All basic user commands
- `nlm inspect *` (Full system inspection)
- `nlm debug *` (Full debugging capabilities)
- `nlm research *` (All research tools)
- `nlm development *` (Development system control)
- `nlm checkpoint *` (Checkpoint management)
- `nlm memory *` (Advanced memory operations)

**Restrictions**:
- No system-destructive commands (should be handled by safety checks)
- Audit logging for all operations
- Resource usage monitoring

### Research User Role
**Available Commands**:
- All advanced user commands
- `nlm research experiment *` (Experiment control)
- `nlm research ablation *` (System ablation studies)
- `nlm research perturbation *` (Parameter perturbation studies)
- `nlm research replay *` (Memory replay experiments)
- `nlm analyze *` (Data analysis tools)

**Restrictions**:
- Automated experiment execution only
- Results must be logged and exportable
- No direct brain manipulation

## 6. Integration Problems Solved

### Problem 1: Lack of Runtime Control
**Solution**: Comprehensive simulation control commands
- **Before**: Fixed execution with no runtime intervention
- **After**: Full runtime control with pause/resume, speed control, step execution
- **Implementation**: Direct integration with brain simulation loop

### Problem 2: Limited System Visibility
**Solution**: System inspection commands
- **Before**: No access to internal brain state
- **After**: Complete visibility into all brain systems
- **Implementation**: Integration with existing getter and logging methods

### Problem 3: No Research Capabilities
**Solution**: Research and debugging tools
- **Before**: Only basic demonstrations
- **After**: Full experimental control and analysis
- **Implementation**: Integration with experiment runner and analysis frameworks

### Problem 4: Poor User Experience
**Solution**: Role-based access and comprehensive help
- **Before**: Single command set for all users
- **After**: Tailored command sets for different user roles
- **Implementation**: Command filtering based on user permissions

### Problem 5: Integration Issues
**Solution**: Unified CLI architecture
- **Before**: Disparate demo executables with different interfaces
- **After**: Consistent CLI across all phases and capabilities
- **Implementation**: Centralized command parsing with modular command execution

## Implementation Requirements

### Architecture Design
- **Command Framework**: Modular command system with clear separation of concerns
- **Argument Parsing**: Sophisticated argument parsing with validation
- **Role Management**: Flexible role-based access control system
- **Logging**: Comprehensive logging for all operations
- **Error Handling**: Robust error handling with user-friendly messages

### Integration Points
- **Brain Control**: Direct integration with brain simulation loop
- **Memory System**: Full integration with all memory subsystems
- **Neuromodulation**: Complete neuromodulation system control
- **Prediction**: Full prediction system integration
- **Development**: Development stage and plasticity control
- **Persistence**: Checkpoint and memory management integration
- **Experiments**: Experiment runner integration

### Performance Considerations
- **Real-time**: Low-latency commands for interactive use
- **Resource Management**: Proper resource cleanup and monitoring
- **Scalability**: Support for large-scale brain simulations
- **Thread Safety**: Safe concurrent command execution

### Development Guidelines
- **Modularity**: Each command as a separate module
- **Testability**: Comprehensive test coverage for all commands
- **Documentation**: Detailed command help and examples
- **Extensibility**: Easy addition of new commands
- **Backward Compatibility**: Maintain existing CLI functionality

## Conclusion

This design document provides a comprehensive framework for enhancing the NLM project's command-line interface to support advanced research and experimentation while maintaining user-friendly access for basic users. The integration points leverage existing brain system APIs, ensuring seamless connectivity and consistent functionality across all command categories. The role-based access control provides appropriate security and usability for different user groups, while the extensive debugging and research tools enable sophisticated experimentation with the artificial brain systems.