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

### Enhanced CLI Interface

The NLM project now includes a comprehensive command-line interface (CLI) that provides unified access to all experiments, demonstrations, and research workflows.

#### Available Commands

- **demo**: Run demonstrations and examples
- **experiment**: Run specific experiments
- **train**: Train agent models with reinforcement learning
- **analyze**: Analyze brain states and experimental results
- **visualize**: Create visualizations (brain, network, training, results)
- **benchmark**: Run performance benchmarks
- **sweep**: Run parameter sweeps for research
- **export**: Export configurations or brain states
- **import**: Import configurations or brain states
- **python**: Hybrid Python/C++ integration
- **help**: Show help information

#### CLI Usage Examples

```bash
# Run Phase 3 demo (world interaction)
nlm demo phase3 --episodes 10 --steps 500

# Run Phase 4 demo (emerging cognition)
nlm demo phase4 --episodes 5 --steps 200

# Run Phase 6 demo (integration test)
nlm demo phase6 --episodes 5 --steps 2000

# Run parameter sweep for research
nlm sweep phase3 --parameters _sweep_params.json --output-dir ./sweep_results

# Train an actor-critic agent
nlm train --episodes 500 --env-size 50,50 --agent-type actor_critic

# Analyze brain state file
nlm analyze brain_state.bin --format brain --output analysis.json

# Create visualization
nlm visualize brain --input brain.bin --output brain_network.png

# Run Python integration
nlm python train --script python/integration_script.py
```

#### Configuration Options

- `--help`: Show help message
- `--version`: Show version information
- `--log-level <L>`: Set logging level (debug, info, warning, error, fatal)
- `--config <file>`: Path to configuration file
- `--output-dir <dir>`: Output directory for results
- `--quiet`: Suppress non-error output
- `--json`: Output results as JSON format
- `--verbose`: Enable verbose output
- `--profile`: Enable performance profiling

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

## Enhanced Command Line Interface

The NLM project now includes a comprehensive command-line interface (CLI) that provides unified access to all experiments, demonstrations, and research workflows.

### CLI Commands

#### Demo Commands
- `nlm demo phase3`: Run Phase 3 demonstration (world interaction)
- `nlm demo phase4`: Run Phase 4 demonstration (emerging cognition)
- `nlm demo phase6`: Run Phase 6 demonstration (integration test)
- `nlm demo integrated`: Run integrated demonstration of all phases

#### Experiment Commands
- `nlm experiment phase3`: Run Phase 3 experiments (learning and retention)
- `nlm experiment phase4`: Run Phase 4 experiments (cognitive emergence)
- `nlm experiment phase5`: Run Phase 5 experiments (meta-learning)
- `nlm experiment phase6`: Run Phase 6 experiments (system integration)

#### Training Commands
- `nlm train`: Train agent models with reinforcement learning

#### Analysis Commands
- `nlm analyze`: Analyze brain states and experimental results

#### Visualization Commands
- `nlm visualize`: Create visualizations (brain, network, training, results)

#### Research Commands
- `nlm benchmark`: Run performance benchmarks
- `nlm sweep`: Run parameter sweeps for research
- `nlm export`: Export configurations or brain states
- `nlm import`: Import configurations or brain states
- `nlm python`: Hybrid Python/C++ integration

### Usage Examples

#### Basic Demos
```bash
# Run Phase 3 demo with JSON output
nlm demo phase3 --episodes 10 --steps 500 --json --output-dir ./demo_results

# Run comprehensive Phase 6 integration demo
nlm demo phase6 --episodes 5 --steps 2000 --json --output-dir ./demo_results
```

#### Parameter Sweeps
```bash
# Run parameter optimization study
nlm sweep phase3 --parameters _sweep_params.json --output-dir ./sweep_results

# Custom sweep with specific ranges
cat > custom_sweep.json << EOF
{
    "parameters": {
        "neuron_counts": [500, 1000, 2000],
        "learning_rates": [0.01, 0.05, 0.1]
    }
}
EOF
nlm sweep phase3 --parameters custom_sweep.json --name "neuron_study"
```

#### Agent Training
```bash
# Train an actor-critic agent
nlm train --episodes 500 --env-size 50,50 --agent-type actor_critic

# Train a DDPG agent with specific parameters
nlm train --episodes 1000 --env-size 30,30 --agent-type ddpg \
    --learning-rate 0.0005 --gamma 0.99 --output-model ddpg_agent.bin
```

#### Analysis and Visualization
```bash
# Analyze brain state file
nlm analyze brain_state.bin --format brain --metrics all --output analysis.json --statistics

# Create interactive brain network visualization
nlm visualize brain --input brain.bin --output brain_network.html --interactive --width 1600 --height 1200

# Create training performance visualization
nlm visualize training --input training_data.json --output learning_curves.png --format png
```

#### Benchmark Testing
```bash
# Run comprehensive benchmark suite
nlm benchmark --suite scaling --sizes 100,500,1000,5000,10000 --runs 10 --detailed

# Run integration benchmarks
nlm benchmark --suite integration --output integration_benchmarks.json
```

#### Configuration Management
```bash
# Export configuration for Python integration
nlm export config --input configs/default.cfg --output python_config.json --format json --metadata

# Import and validate brain state
nlm import brain --input saved_brain.bin --output restored_brain.bin --validate
```

#### Python Integration
```bash
# Train agent using hybrid Python/C++ workflow
nlm python train --script python/integration_script.py --model-file agent_model.bin

# Run Python-based simulation
nlm python simulate --script simulation.py --config-file simulation_config.json
```

### Configuration Files

#### Parameter Sweep Configuration (`_sweep_params.json`)
```json
{
    "experiment": "phase3",
    "description": "Phase 3 parameter sweep - learning efficiency",
    
    "parameters": {
        "neuron_counts": [500, 1000, 2000, 5000],
        "learning_rates": [0.01, 0.05, 0.1, 0.2],
        "connection_probabilities": [0.05, 0.1, 0.2],
        "region_counts": [2, 4, 8],
        "plasticity_enabled": [true, false],
        "reward_modulation_enabled": [true, false]
    },
    
    "sweeps": {
        "full": {
            "type": "grid",
            "description": "Full factorial design of all parameters",
            "parameters": ["neuron_counts", "learning_rates", "connection_probabilities", "region_counts", "plasticity_enabled", "reward_modulation_enabled"]
        },
        "random": {
            "type": "random",
            "description": "Random parameter sampling",
            "num_samples": 50,
            "seed": 42
        },
        "sequential": {
            "type": "sequential",
            "description": "Sequential parameter variation",
            "parameters": ["neuron_counts"],
            "steps": 4
        }
    },
    
    "output": {
        "format": "json",
        "directory": "./sweep_results",
        "prefix": "sweep_"
    },
    
    "defaults": {
        "trials": 20,
        "max_steps": 1000,
        "evaluation_metrics": ["learning_rate", "retention_score", "generalization_score", "total_reward"]
    }
}
```

### Python Integration Scripts

The `python/` directory contains comprehensive tools for hybrid workflows:

#### `integration_script.py`
- Load/save brain states between C++ and Python
- Run training episodes with Python-based environments
- Evaluate agent performance with statistical analysis
- Export results for further analysis

#### `AdvancedAnalysisTools.py`
- Statistical analysis of experimental results
- Machine learning model training and evaluation
- Visualization and plotting capabilities
- Data export and reporting

#### `AdvancedConfig.py`
- Configuration management and validation
- Environment setup and dependency management
- Docker and container orchestration
- Project configuration management

#### `AdvancedLearningCapabilities.py`
- Advanced reinforcement learning algorithms
- Transfer learning and domain adaptation
- Meta-learning and continuous learning
- Curriculum learning and task switching

#### `AdvancedMemoryAndPerformance.py`
- Memory-efficient data structures
- Performance optimization and profiling
- Parallel processing and distributed computing
- GPU acceleration and vectorization

#### `AdvancedSimulationControl.py`
- Complex environment control and management
- Multi-agent simulation and coordination
- Scenario generation and testing
- Real-time monitoring and control

#### `AdvancedVisualization.py`
- 3D neural network visualizations
- Interactive dashboards and monitoring
- Animation and time-series plotting
- Export for presentations and publications

#### `IntegrationWithExternalTools.py`
- Integration with external scientific computing tools
- Data exchange with other neuroscience software
- Cloud computing and distributed resources
- High-performance computing clusters

### File Structure

```
workspace/
├── main.cpp                    # Enhanced NLM CLI implementation
├── _sweep_params.json          # Parameter sweep configuration
├── pyproject.toml             # Python project configuration
├── configs/
│   └── default.cfg            # Default configuration
├── python/
│   ├── __init__.py            # Python package initialization
│   ├── integration_script.py  # Core integration script
│   └── *.py                   # Advanced tools and capabilities
├── sessions/                   # Execution logs and results
└── docs/                       # Documentation
```

### System Requirements

#### Minimum Requirements
- **CPU**: Dual-core processor (quad-core recommended)
- **RAM**: 4 GB (8 GB recommended for larger networks)
- **Storage**: 100 MB for installation, 1 GB for simulations
- **OS**: Linux, macOS, or Windows

#### Recommended Requirements
- **CPU**: 8+ cores for parallel processing
- **RAM**: 16+ GB for large-scale simulations
- **Storage**: 10+ GB for comprehensive experiments
- **GPU**: NVIDIA GPU with CUDA support for acceleration

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running the Enhanced CLI

```bash
# Show help information
nlm --help

# Run a basic demo
nlm demo phase3 --episodes 10 --steps 500

# Run parameter sweep
nlm sweep phase3 --parameters _sweep_params.json

# Analyze results
nlm analyze brain_state.bin

# Create visualization
nlm visualize brain --input brain.bin
```

### Python Integration Quick Start

```python
import pynlm

# Create a medium-sized brain with plasticity
brain = pynlm.createMediumBrain()

# Create world
world = pynlm.createSimulationWorld(100, 100)

# Create agent brain
agent_brain = pynlm.createAgentBrainWithWorld(world)

# Enable all modules
agent_brain.enableAllModules()

# Run training episode
for step in range(100):
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Simulate brain (multiple steps per action)
    for neural_step in range(10):
        brain.step(neural_step)
    
    # Get motor command
    motor_command = agent_brain.decodeMotorCommand()
    
    # Apply motor command to world
    result = world.applyMotorCommand(motor_command, world.getSimulationTime())
    
    # Apply reward modulation
    agent_brain.applyRewardModulation(result.reward, 0.0)
    
    # Update world
    world.update(0.01)
    
    # Update development
    agent_brain.updateDevelopment(0.01)

# Get final metrics
stats = pynlm.getBrainStats(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Active neurons: {stats['active_neurons']}")
```
