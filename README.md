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

### From Source (Recommended for Development)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Building Python Bindings

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python module
pip install .

# Or install in development mode
pip install -e .
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable (Phase 2, backward compatibility) |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration (lifetime learning) |
| `nlm_phase6_demo` | Executable | Phase 6 demonstration (integration test) |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase5` | Static Library | Phase 5 experiments |
| `nlm_phase6` | Static Library | Phase 6 experiments |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running

### Phase 6 Demo (Integration Test)

```bash
# Run the Phase 6 integration demo
./nlm_phase6_demo

# With options
./nlm_phase6_demo --neurons 500 --steps 2000 --quiet
./nlm_phase6_demo --log-file phase6.log --help
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 5 Demo (Lifetime Learning)

```bash
# Run the Phase 5 lifetime learning demo
./nlm_phase5_demo

# With options
./nlm_phase5_demo --neurons 20000 --output-dir ./results
```

### Phase 4 Demo

```bash
# Run the Phase 4 demonstration
./nlm_phase4_demo
```

### Phase 3 Demo

```bash
# Run the Phase 3 demonstration
./nlm_phase3_demo
```

### Phase 2 Demo (Backward Compatibility)

```bash
# Run the original Phase 2 demonstration
./nlm
```

## Python API Usage

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Research-Grade API

```python
import pynlm

# Create enhanced configuration
config = pynlm.ResearchConfig.createFromTemplate("advanced")

# Run exploration experiment
brain = pynlm.createBrain(config)
analysis = pynlm.AnalysisTools()
results = analysis.analyzeBrainResults(brain)

# Export results
pynlm.exportResultsToJSON(brain, "results.json")
pynlm.exportResultsToCSV(brain, "results.csv")

# Get configuration recommendations
recommendations = pynlm.getConfigurationRecommendations("cognitive_tests")

# Create advanced agent
agent = pynlm.createAdvancedAgent(brain, pynlm.createSimpleWorld())
agent.enableCuriosityDrivenExploration()
agent.enableMetaLearning()
```

### Experiment Framework

```python
import pynlm

# Create research experiment
experiment = pynlm.createResearchExperiment()

# Run parameter sweep
base_config = pynlm.createDefaultConfig()
parameter_ranges = {
    "neuron_count": [500, 1000, 2000],
    "connection_probability": [0.05, 0.1, 0.15]
}
results = experiment.runParameterSweep(base_config, parameter_ranges)

# Run random search
random_results = experiment.runRandomSearch(base_config, evolution_params, 100)

# Run evolutionary algorithm
evolution_results = experiment.runEvolutionaryAlgorithm(base_config, evolution_params)
```

### Analysis Tools

```python
import pynlm

# Create analysis tools
analysis = pynlm.createAnalysisTools()

# Analyze brain components
neural_activity = analysis.analyzeNeuralActivity(brain)
network_topology = analysis.analyzeNetworkTopology(brain)
plasticity = analysis.analyzePlasticity(brain)
memory_systems = analysis.analyzeMemorySystems(brain)
development = analysis.analyzeDevelopment(brain)
neuromodulation = analysis.analyzeNeuromodulation(brain)
prediction_system = analysis.analyzePredictionSystem(brain)
cognition = analysis.analyzeCognition(brain)

# Statistical analysis
data = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
labels = ["Group1", "Group2", "Group3"]
stats = analysis.statisticalAnalysis(data, labels)

# Time series analysis
time_series = [1.0, 2.0, 1.5, 2.5, 2.0, 3.0, 2.5]
ts_analysis = analysis.timeSeriesAnalysis(time_series)
```

### Advanced Agent Behaviors

```python
import pynlm

# Create advanced agent
agent = pynlm.createAdvancedAgent(brain, world)

# Enable advanced behaviors
agent.enableCuriosityDrivenExploration(1.0)
agent.enableIntrinsicMotivation(1.0)
agent.enableMetaLearning(0.01)
agent.enableTransferLearning(source_agent)
agent.enableLifelongLearning(0.9)
agent.enableSocialLearning([agent1, agent2, agent3])
agent.enableCreativeBehavior(0.5)
agent.enableProblemSolving(problems)
```

## Key Classes and Methods

### Enhanced Configuration (ResearchConfig)

```python
# Create research-grade configuration
config = pynlm.ResearchConfig.createFromTemplate("advanced")

# Configuration management
config.loadAndValidate("config.json")
config.getResearchRecommendations()
config.validateForExperiment("cognitive_tests")
config.getStatistics()

# Export/import
config.exportConfig("config.json")
config.importConfig("config.json")

# Experiment metadata
config.addExperimentMetadata("author", "Research Team")
config.addExperimentMetadata("date", "2026-10-24")
config.getExperimentMetadata("author")
```

### Research API (ResearchAPI)

```python
# Analysis and experiments
api = pynlm.ResearchAPI()

# Brain analysis
results = api.analyzeBrainResults(brain)
metrics = api.generatePerformanceMetrics(brain)
comparison = api.compareConfigurations([config1, config2])

# Experiment running
experiment_results = api.runExplorationExperiment(config, "parameter_sweep", params)
api.exportResultsToJSON(brain, "results.json")
api.exportResultsToCSV(brain, "results.csv")

# Configuration management
recommendations = api.getConfigurationRecommendations("memory_tests")
validation = api.validateResearchConfiguration(config)
templates = api.getConfigurationTemplate("advanced")
```

### Research Experiment Framework (ResearchExperiment)

```python
# Advanced experiment framework
experiment = pynlm.createResearchExperiment()

# Parameter sweep
parameter_ranges = {"neuron_count": [500, 1000, 2000]}
sweep_results = experiment.runParameterSweep(base_config, parameter_ranges)

# Random search
random_results = experiment.runRandomSearch(base_config, evolution_params, 100)

# Evolutionary algorithm
evolution_results = experiment.runEvolutionaryAlgorithm(base_config, evolution_params)
```

### Analysis Tools (AnalysisTools)

```python
# Comprehensive analysis tools
analysis = pynlm.createAnalysisTools()

# Neural analysis
neural_analysis = analysis.analyzeNeuralActivity(brain)
network_analysis = analysis.analyzeNetworkTopology(brain)
plasticity_analysis = analysis.analyzePlasticity(brain)
memory_analysis = analysis.analyzeMemorySystems(brain)

# Other analyses
development_analysis = analysis.analyzeDevelopment(brain)
neuromodulation_analysis = analysis.analyzeNeuromodulation(brain)
prediction_analysis = analysis.analyzePredictionSystem(brain)
cognition_analysis = analysis.analyzeCognition(brain)

# Statistical analysis
stats = analysis.statisticalAnalysis(data, labels)
ts_analysis = analysis.timeSeriesAnalysis(time_series)

# Visualization
analysis.generateVisualization("analysis_report.png", {
    "neural": neural_analysis,
    "network": network_analysis,
    "plasticity": plasticity_analysis,
    "memory": memory_analysis
})
```

### Advanced Agent Behaviors (AdvancedAgent)

```python
# Advanced agent with research-grade features
agent = pynlm.createAdvancedAgent(brain, world)

# Enable advanced behaviors
agent.enableCuriosityDrivenExploration(1.0)
agent.enableIntrinsicMotivation(1.0)
agent.enableMetaLearning(0.01)
agent.enableTransferLearning(source_agent)
agent.enableLifelongLearning(0.9)
agent.enableSocialLearning([agent1, agent2, agent3])
agent.enableCreativeBehavior(0.5)
agent.enableProblemSolving(problems)
```

### Original Classes (for backward compatibility)

```python
# Original simple classes for basic usage
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

## Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom

# Motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Configuration Templates

### Research Configuration Templates

```python
# Get research templates
templates = pynlm.getResearchTemplates()

# Basic template
basic_config = templates["basic"]

# Exploration template
exploration_config = templates["exploration"]

# Advanced template
advanced_config = templates["advanced"]
```

## Experiment Results and Analysis

### Result Export

```python
# Export results in multiple formats
pynlm.exportResultsToJSON(brain, "results.json")
pynlm.exportResultsToCSV(brain, "results.csv")
```

### Performance Analysis

```python
# Generate performance metrics
analysis = pynlm.createAnalysisTools()
metrics = analysis.generatePerformanceMetrics(brain, timeWindow=1.0)
```

### Configuration Recommendations

```python
# Get configuration recommendations based on experiment goals
recommendations = pynlm.getConfigurationRecommendations("cognitive_tests")
```

## Advanced Features

### 1. Configuration Validation

- Parameter validation with type checking
- Range validation for numeric parameters
- Consistency checking between related parameters
- Research-standard validation rules

### 2. Experiment Framework

- Parameter sweep for systematic exploration
- Random search for stochastic optimization
- Evolutionary algorithms for adaptive optimization
- Multi-seed experiments for statistical significance

### 3. Advanced Analysis Tools

- Neural activity pattern analysis
- Network topology analysis
- Plasticity dynamics analysis
- Memory system analysis
- Development trajectory analysis
- Neuromodulation analysis
- Prediction system analysis
- Cognition analysis

### 4. Enhanced Agent Behaviors

- Curiosity-driven exploration
- Intrinsic motivation
- Meta-learning
- Transfer learning
- Lifelong learning
- Social learning
- Creative behavior
- Problem solving

### 5. Research-Grade APIs

- Programmatic experiment control
- Batch processing capabilities
- Result aggregation and analysis
- Visualization generation
- Configuration management
- Experiment tracking

### 6. Performance Monitoring

- Real-time performance metrics
- Memory usage tracking
- Computational complexity analysis
- Bottleneck identification

## Installation and Setup

### System Requirements

- C++20 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 2.11.0 or higher

### Quick Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install pybind11 scikit-build-core pytest numpy
pip install .
```

### Development Installation

```bash
# For development, install in development mode
pip install -e .

# For editing Python bindings
pip install pybind11 setuptools cmake
```

## Usage Examples

### Basic Neural Simulation

```python
import pynlm

# Create and run a simple simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Research-Grade Experiment

```python
import pynlm

# Setup research experiment
brain = pynlm.createBrain(pynlm.ResearchConfig.createFromTemplate("advanced"))
agent = pynlm.createAdvancedAgent(brain, pynlm.createSimpleWorld())

# Run with curiosity and meta-learning
agent.enableCuriosityDrivenExploration()
agent.enableMetaLearning()

# Execute experiment
for step in range(5000):
    # Agent-environment interaction logic here
    pass
```

### Advanced Analysis

```python
import pynlm

# Setup analysis
analysis = pynlm.createAnalysisTools()

# Run comprehensive analysis
neural_data = analysis.analyzeNeuralActivity(brain)
network_data = analysis.analyzeNetworkTopology(brain)
plasticity_data = analysis.analyzePlasticity(brain)
memory_data = analysis.analyzeMemorySystems(brain)

# Generate reports and visualizations
analysis.generateVisualization("analysis_report.png", {
    "neural": neural_data,
    "network": network_data,
    "plasticity": plasticity_data,
    "memory": memory_data
})
```

## Troubleshooting

### Common Issues

**Build errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**Python import errors:**
```bash
pip install --force-reinstall .
```

**Runtime errors:**
- Ensure `initialize()` is called before `step()`
- Check for proper error handling in your code
- Validate configuration parameters

**Performance issues:**
- Adjust simulation timestep for stability
- Use appropriate neuron counts for your experiment
- Monitor memory usage during long simulations

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations and future directions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit report

## License

MIT

## Authors

Research project — See docs for scientific background.

## Acknowledgements

This project represents the culmination of the NLM research program, integrating decades of work on artificial neural systems, developmental psychology, and computational neuroscience into a coherent framework for studying brain-like intelligence.