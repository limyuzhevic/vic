# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project implementing Phase 6: Final Integration. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### What NLM IS NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

### What NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Introduction

NLM represents a significant advancement in computational neuroscience and artificial intelligence. It implements a brain-inspired architecture that integrates multiple cognitive systems into a unified computational framework. The system simulates brain-like computation through:

- **Spiking Neural Networks**: Leaky Integrate-and-Fire (LIF) neuron dynamics
- **Multiple Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Reward prediction, curiosity, and novelty detection
- **Developmental Processes**: Age-dependent plasticity and maturation
- **Predictive Processing**: Expectation-based learning and inference
- **Integrated Cognition**: Planning, concept formation, and attentional selection

## Current Phase: Phase 6 - Final Integration

### Phase 6 Integration Achievements

**Phase 6: FINAL INTEGRATION** represents the complete integration of all brain systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

#### Memory Systems Integration
- **Working Memory**: Transient active information storage with persistent activity and competition
- **Episodic Memory**: Experience encoding and replay with temporal organization
- **Associative Memory**: Hebbian pattern associations for memory recall
- **All memory systems connected to neural processing**

#### Neuromodulation Integration
- **Dopamine**: Affects neural excitability and plasticity based on reward prediction error
- **Curiosity**: Drives exploration behavior based on novelty detection
- **Novelty Detection**: Integrated with sensory processing for exploration motivation
- **All neuromodulators connected to plasticity rules**

#### Prediction System Integration
- **Prediction System**: Integrated into brain loop for sensory prediction and error computation
- **Prediction Error**: Signals affect learning and adaptation
- **Confidence Tracking**: Implemented for uncertainty estimation

#### Cognition Systems Integration
- **Neural Planner**: Action sequence evaluation with competitive selection
- **Concept Formation**: Pattern discovery from experience
- **Attentional Selection**: Competitive focus selection with inhibition
- **All cognition systems connected to perception and action**

#### Development Integration
- **Developmental Stages**: Age-dependent plasticity rates
- **Structural Plasticity**: Modulated by age and experience
- **Neural Excitability**: Changes with development

#### Persistence Features
- **Checkpoint System**: Save/load brain state with compression
- **Brain State Serialization**: Full state persistence
- **Resume Capability**: Can resume from saved checkpoints

#### Replay and Consolidation
- **Episodic Memory Replay**: During simulation and sleep/rest cycles
- **Memory Consolidation**: For important episodes
- **Sleep Integration**: Integration with rest cycles

## Building

### Linux/macOS
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Windows
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### Custom Configuration Options

#### Performance Optimizations
```bash
# Enable SIMD, MPI, and GPU acceleration
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_SIMD=ON \
         -DNLM_ENABLE_MPI=ON \
         -DNLM_ENABLE_GPU=ON

# Custom brain configuration
cmake .. -DNLM_NEURON_COUNT=2000 \
          -DNLM_REGION_COUNT=3 \
          -DNLM_ENABLE_MEMORY_POOLS=ON
```

## Running

### Phase 6 Demo (Integration Test)

```bash
cd build
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected and functioning together.

### Running Examples

#### Basic Usage
```bash
# Build examples
make examples

# Run basic brain simulation
./examples/ex01_basic_brain

# Run memory systems example
./examples/ex02_memory_systems

# Run neuromodulation example
./examples/ex03_neuromodulation

# Run full integration example
./examples/ex07_integration
```

#### Command Line Options
```bash
./nlm_simulation --help

Common options:
  --help                    Show help information
  --version                 Show version information
  --config <file>           Load configuration from file
  --neurons <count>         Set number of neurons
  --regions <count>         Set number of brain regions
  --steps <count>           Set simulation steps
  --checkpoint <file>       Save checkpoint to file
  --load <file>             Load brain state from file
  --verbose                 Enable verbose logging
  --debug                   Enable debug logging
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Comprehensive documentation
│   ├── ARCHITECTURE.md      # System architecture
│   ├── API_REFERENCE.md     # Complete API documentation
│   ├── QUICK_START.md       # Rapid introduction
│   ├── TUTORIALS.md         # Step-by-step tutorials
│   ├── BEST_PRACTICES.md    # Development guidelines
│   ├── PERFORMANCE_TUNING.md # Performance optimization
│   ├── TROUBLESHOOTING.md    # Common issues and solutions
│   ├── CONTRIBUTING.md       # Contribution guidelines
│   └── CHANGELOG.md         # Version history
├── examples/               # Example programs
│   ├── ex01_basic_brain.cpp
│   ├── ex02_memory_systems.cpp
│   ├── ex03_neuromodulation.cpp
│   ├── ex04_development.cpp
│   ├── ex05_prediction.cpp
│   ├── ex06_cognition.cpp
│   ├── ex07_integration.cpp
│   ├── ex08_advanced_usage.cpp
│   ├── ex09_visualization.cpp
│   └── ex10_performance.cpp
├── src/
│   ├── core/              # Core utilities
│   │   ├── Config/          # Configuration management
│   │   ├── Logger/          # Logging system
│   │   ├── Random/          # Random number generation
│   │   ├── SimulationClock/ # Time management
│   │   └── Types/           # Fundamental types
│   ├── brain/             # Neural core
│   │   ├── Neuron.cpp      # Basic neuron model
│   │   ├── Brain.cpp       # Central brain coordinator
│   │   ├── NeuralRegion.cpp # Functional brain regions
│   │   └── Synapse.cpp      # Synaptic communication
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/   # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   └── environment/       # Environment interface
├── tests/                # Test suite
│   ├── unit/              # Unit tests
│   └── integration/       # Integration tests
└── configs/              # Configuration templates
    ├── config.json
    ├── config.xml
    └── config.yaml
```

## System Architecture

### Core Architecture

The NLM brain operates as a closed-loop system with the following architecture:

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

### Component Integration

#### 1. Neural Core
**Neurons**: Basic spiking units with LIF dynamics
**Brain**: Central coordinator with spiking neural computation
**Regions**: Functional brain areas with connectivity
**Populations**: Groups of neurons with shared properties
**Synapses**: Communication channels between neurons

#### 2. Dynamics and Plasticity
**NeuralDynamics**: Spike propagation and neuron dynamics
**SpikeSystem**: Event-driven spike delivery
**STDP**: Spike-timing-dependent plasticity
**Hebbian**: Hebbian synaptic strengthening
**StructuralPlasticity**: Structural synaptic rewiring

#### 3. Cognitive Systems
**WorkingMemory**: Transient active information storage
**EpisodicMemory**: Experience encoding and replay
**AssociativeMemory**: Pattern association through Hebbian learning
**PredictionSystem**: Sensory prediction and error computation
**NeuralPlanner**: Action sequence evaluation and planning
**ConceptFormation**: Pattern discovery and categorization
**AttentionalSelection**: Competitive focus selection

#### 4. Neuromodulation Systems
**Dopamine**: Reward prediction and reinforcement signaling
**Curiosity**: Exploration motivation and novelty detection
**Novelty**: Sensory novelty detection for exploration
**PredictionError**: Prediction error signaling for learning

#### 5. Developmental System
**DevelopmentSystem**: Age-dependent plasticity modulation
**Maturation**: Neuronal maturation processes
**Pruning**: Synaptic pruning and refinement
**Synaptogenesis**: New synapse formation

#### 6. Agent Interface
**AgentBrain**: Sensory transduction and motor decoding
**AgentBody**: Interface to the world simulation
**SensoryPercept**: Processed sensory input representation

#### 7. Supporting Systems
**Config**: Configuration management
**Logger**: Logging and debugging
**RandomGenerator**: Random number generation
**SimulationClock**: Time management
**Environment**: World interface
**Observation**: Perceptual state representation

## Key Features

### Developmental Stages

#### Juvenile Stage
- **High Plasticity**: Rapid learning and adaptation
- **Exploation**: Broad exploration of environment
- **Memory Consolidation**: Strong replay and strengthening

#### Adult Stage
- **Optimized Performance**: Stable behavior and efficient processing
- **Specialized Skills**: Refined neural circuits
- **Memory Efficiency**: Balanced consolidation and retrieval

#### Aging Stage
- **Declining Plasticity**: Slower learning rate
- **Experience Accumulation**: Rich knowledge base
- **Behavioral Changes**: Adapted strategies and responses

### Memory Systems

#### Working Memory
- **Capacity**: Configurable via `brain.memory.workingMemory.capacity`
- **Decay Rate**: Configurable via `brain.memory.workingMemory.decayRate`
- **Application**: Active task maintenance and manipulation

#### Episodic Memory
- **Storage**: Configurable via `brain.memory.episodicMemory.maxEpisodes`
- **Replay Window**: Configurable via `brain.memory.episodicMemory.replayWindow`
- **Application**: Experience encoding and long-term storage

#### Associative Memory
- **Similarity Threshold**: Configurable via `brain.memory.associativeMemory.similarityThreshold`
- **Association Strength**: Learned through Hebbian processes
- **Application**: Pattern completion and generalization

### Learning Mechanisms

#### Spike-Timing-Dependent Plasticity (STDP)
- **Time Window**: Configurable via `plasticity.stdp.timeWindow`
- **Learning Rate**: Configurable via `plasticity.stdp.learningRate`
- **Mechanism**: Strengthen synapses for simultaneous pre- and post-synaptic activity

#### Hebbian Learning
- **Learning Rate**: Configurable via `plasticity.hebbian.learningRate`
- **Mechanism**: Co-activation-based synaptic strengthening
- **Application**: Pattern association and reinforcement

#### Prediction Error Learning
- **Integration**: Connects prediction system with neuromodulation
- **Mechanism**: Drives learning across all neural systems
- **Application**: Error-based optimization and adaptation

#### Curiosity-Driven Exploration
- **Novelty Detection**: Drives exploration of novel stimuli
- **Uncertainty Estimation**: Balances exploration and exploitation
- **Application**: Discovery and skill acquisition

## Usage Examples

### Example 1: Basic Brain Simulation

Create a basic simulation with configurable parameters:

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== NLM Phase 6 Basic Brain ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 100);
    config->set("brain.learningRate", 0.01);
    config->set("brain.simulation.maxSteps", 1000);
    config->set("brain.region.count", 2);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total regions: " << brain->getRegionCount() << std::endl;
    
    // Run simulation
    std::cout << "\nStarting simulation..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getActiveNeuronCount() << " active neurons, "
                      << brain->getFiringNeuronCount() << " firing neurons, "
                      << brain->getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    }
    
    // Save brain state
    brain->save("ex01_checkpoint.brain");
    std::cout << "\nBrain state saved to 'ex01_checkpoint.brain'" << std::endl;
    
    std::cout << "=== Example 1 Complete ===" << std::endl;
    return 0;
}
```

### Example 2: Memory Systems

Demonstrate integrated memory systems:

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>
#include <vector>

int main() {
    // Configure with memory systems
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 500);
    config->set("brain.memory.workingMemory.enabled", true);
    config->set("brain.memory.workingMemory.capacity", 200);
    config->set("brain.memory.episodicMemory.enabled", true);
    config->set("brain.memory.associativeMemory.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Access memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    auto associativeMemory = brain->getAssociativeMemory();
    
    // Create sample experiences
    std::vector<std::vector<float>> experiences = {
        {0.1f, 0.5f, 0.8f, 0.3f, 0.7f},
        {0.9f, 0.2f, 0.4f, 0.6f, 0.1f},
        {0.3f, 0.7f, 0.2f, 0.9f, 0.4f},
    };
    
    // Store in working memory
    for (size_t i = 0; i < experiences.size(); ++i) {
        workingMemory->store(experiences[i]);
        std::cout << "Stored experience " << i << " in working memory\n";
    }
    
    // Retrieve from working memory
    auto retrieved = workingMemory->retrieve();
    std::cout << "Retrieved pattern: ";
    for (float val : retrieved) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Store in episodic memory
    auto currentTime = std::chrono::duration<double>::zero();
    for (size_t i = 0; i < experiences.size(); ++i) {
        auto episodeTime = currentTime + std::chrono::duration<double>(i * 10.0);
        episodicMemory->encode(experiences[i], episodeTime);
        std::cout << "Encoded experience at time " << std::chrono::duration<double>(episodeTime).count() << "s\n";
    }
    
    // Create pattern associations
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.8f};
    std::vector<float> pattern2 = {0.9f, 0.2f, 0.4f};
    associativeMemory->associate(pattern1, pattern2);
    
    std::cout << "Memory systems demonstration completed!" << std::endl;
    return 0;
}
```

### Example 3: Neuromodulation

Explore reward-based and curiosity-driven learning:

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Configure neuromodulation
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 300);
    config->set("brain.neuromodulation.dopamine.enabled", true);
    config->set("brain.neuromodulation.dopamine.baselineLevel", 0.1f);
    config->set("brain.neuromodulation.curiosity.enabled", true);
    config->set("brain.neuromodulation.novelty.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Get neuromodulation systems
    auto dopamine = brain->getDopamine();
    auto curiosity = brain->getCuriosity();
    auto novelty = brain->getNovelty();
    
    // Get agent for neuromodulation application
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Simulate reward learning
    float reward = 1.0f;
    float predictionError = 0.3f;
    
    // Apply neuromodulation
    agent->applyRewardModulation(reward, predictionError);
    
    std::cout << "Neuromodulation simulation completed!" << std::endl;
    std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
    std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
    std::cout << "Novelty level: " << novelty->getLevel() << std::endl;
    
    return 0;
}
```

## Configuration Options

### Core Brain Configuration
```json
{
    "brain": {
        "maxNeurons": 1000,
        "learningRate": 0.01,
        "plasticityEnabled": true,
        "regionCount": 3,
        "connectionProbability": 0.1,
        "simulation": {
            "maxSteps": 10000,
            "timestep": 0.001,
            "checkpointInterval": 1000
        },
        "memory": {
            "workingMemory": {
                "enabled": true,
                "capacity": 500,
                "decayRate": 0.01
            },
            "episodicMemory": {
                "enabled": true,
                "maxEpisodes": 1000,
                "replayWindow": 1000
            },
            "associativeMemory": {
                "enabled": true,
                "similarityThreshold": 0.8
            }
        },
        "neuromodulation": {
            "dopamine": {
                "enabled": true,
                "baselineLevel": 0.1,
                "gain": 10.0
            },
            "curiosity": {
                "enabled": true,
                "baselineLevel": 0.1,
                "noveltyGain": 5.0
            },
            "novelty": {
                "enabled": true,
                "decayRate": 0.1
            },
            "predictionError": {
                "enabled": true,
                "baselineError": 0.1
            }
        },
        "development": {
            "enabled": true,
            "plasticityDecayRate": 0.02,
            "maturationRate": 0.1
        },
        "prediction": {
            "enabled": true,
            "errorThreshold": 0.5,
            "learningRate": 0.01
        },
        "cognition": {
            "attentionEnabled": true,
            "planningEnabled": true,
            "conceptFormationEnabled": true
        }
    }
}
```

## Performance Characteristics

### Computational Complexity
- **Time Complexity**: O(N²) for synaptic connections (optimized with sparsity)
- **Memory Usage**: O(N) for neural populations with compression
- **Scalability**: Parallel processing for large networks (SMP and MPI support)

### Real-Time Performance
- **Time Step**: Configurable simulation granularity (1μs to 1s steps)
- **Event-Driven**: Efficient spike propagation via `SpikeSystem`
- **Checkpointing**: State persistence for long simulations (interval configurable via `simulation.checkpointInterval`)

### Hardware Requirements

#### Minimum Requirements
- **CPU**: 2-core processor
- **Memory**: 4GB RAM
- **Storage**: 100MB for brain state

#### Recommended Requirements
- **CPU**: 8-core processor or GPU
- **Memory**: 16GB RAM
- **Storage**: 1GB for checkpoints and simulations

#### High-Performance Requirements
- **CPU**: Many-core processor (16+ cores)
- **Memory**: 64GB RAM or GPU with 8GB+ VRAM
- **Storage**: 10GB for large-scale simulations

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Authors

NLM is a research project investigating computational brain-like systems. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture

**Phase 6 does NOT claim**:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

**Phase 6 DOES investigate**:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Support

### Documentation
- **Comprehensive Documentation**: See docs/ for complete documentation
- **API Reference**: docs/API_REFERENCE.md for complete API documentation
- **Tutorials**: docs/TUTORIALS.md for step-by-step learning
- **Best Practices**: docs/BEST_PRACTICES.md for development guidelines

### Getting Help
- **GitHub Issues**: Report bugs and request features
- **Community Forum**: Join discussions with other NLM users
- **Technical Support**: Contact for technical assistance

### Contributing
- **Code**: Submit pull requests with appropriate documentation
- **Documentation**: Improve and expand documentation
- **Examples**: Add new example programs
- **Testing**: Add tests for new features

## Acknowledgments

The NLM project builds on decades of research in computational neuroscience and artificial intelligence. Special thanks to the research community for contributing to the field of brain-inspired computation.

---

**Last Updated:** October 16, 2026
**Version:** Phase 6 - Final Integration
**Authors:** NLM Development Team