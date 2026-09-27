# API Reference

This section provides comprehensive API documentation for all major components of the NLM framework.

## Core Components

### nlm::Config

The Config class provides a comprehensive configuration system that supports multiple configuration sources (defaults, files, command-line, runtime), various data types, and type-safe value retrieval.

**Key Features:**
- 8 supported value types (int, int64_t, double, bool, std::string, and vector variants)
- Multiple configuration sources with priority (Default > File > CommandLine > Runtime)
- Type-safe template-based value retrieval
- File-based configuration in JSON format
- Command-line argument parsing
- Thread-safe access patterns

**Usage Example:**
```cpp
nlm::Config config;
config.set("learning_rate", 0.01);
config.set("simulation_steps", 1000);

if (auto neurons = config.get<int>("num_neurons")) {
    std::cout << "Number of neurons: " << *neurons << std::endl;
}
```

### nlm::Logger

The Logger class provides a comprehensive logging system with multiple severity levels, flexible sinks, and thread-safe operation.

**Key Features:**
- Five severity levels: Debug, Info, Warning, Error, Critical
- Multiple logger sinks (console, file, custom implementations)
- Global and per-logger level control
- File and function/line number context for log entries
- Stream-style and convenience macros for easy logging

**Usage Example:**
```cpp
nlm::Logger::getGlobal().info("Starting neural network simulation");
NLM_LOG_WARNING("High memory usage detected");
```

### nlm::Exception

The Exception class provides a comprehensive exception system with specific exception types for different error conditions.

**Key Features:**
- Hierarchical exception classes for organized error handling
- Error codes categorizing different types of errors (26 different types)
- Source location tracking (file, line, function)
- Convenience macros for consistent error throwing

**Usage Example:**
```cpp
try {
    nlm::Config config;
    config.loadFromFile("config.json");
} catch (const nlm::NLMConfigError& e) {
    std::cout << "Configuration error: " << e.what() << std::endl;
}
```

## Neural Components

### nlm::Neuron

The Neuron class represents a single biological neuron implementing the Leaky Integrate-and-Fire (LIF) model.

**Key Features:**
- Leaky Integrate-and-Fire (LIF) dynamics
- Multiple input types (excitatory, inhibitory, modulatory)
- Spike detection and refractory period management
- Synaptic connectivity management
- Plasticity state management
- Type system for different neuron specializations

**Usage Example:**
```cpp
nlm::Neuron neuron(42);
neuron.setThreshold(-50.0f);
neuron.setLeakConductance(10.0f);
neuron.initializeRandom(rng);

while (simulation_steps-- > 0) {
    neuron.step(current_time);
}
```

### nlm::Synapse

The Synapse class represents a synaptic connection between two neurons with transmission and learning capabilities.

**Key Features:**
- Chemical synaptic transmission with configurable delay
- Excitatory/Inhibitory signal modulation
- Spike-timing dependent plasticity (STDP) support
- Hebbian and reward-modulated learning rules
- Short-term synaptic plasticity mechanisms
- Eligibility traces for delayed reward learning

**Usage Example:**
```cpp
nlm::Synapse synapse(0, 1, 2); // Source neuron 1, destination neuron 2
synapse.setWeight(0.5f);
synapse.setDelay(2);
synapse.setType(nlm::SynapseType::Excitatory);
```

### nlm::Brain

The Brain class is the central coordinator of the neural system, integrating all neural components and managing simulation flow.

**Key Features:**
- Complete neural architecture integration
- Event-driven neural computation
- Multiple subsystem coordination (memory, prediction, cognition, neuromodulation)
- Sensory-motor loop integration
- Developmental plasticity support
- Checkpointing and persistence

**Usage Example:**
```cpp
nlm::Config config;
config.set("simulation_steps", 1000);

nlm::Brain brain(std::make_shared<nlm::Config>(config));
if (brain.initialize()) {
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    brain.reset();
}
```

## Sensory Systems

### nlm::Vision

The Vision class provides visual processing and feature extraction capabilities.

**Key Features:**
- Edge detection and feature extraction
- Object recognition and classification
- Visual scene analysis
- Multimodal sensory integration

### nlm::Audio

The Audio class provides auditory processing and sound analysis capabilities.

**Key Features:**
- Sound frequency analysis
- Spatial audio processing
- Sound source localization
- Auditory scene analysis

### nlm::InternalSignals

The InternalSignals class provides proprioception and interoception capabilities.

**Key Features:**
- Body position and movement sensing
- Physiological state monitoring
- Internal stimulus processing
- Homeostatic regulation

## Motor System

### nlm::Action

The Action class generates and executes movements based on motor neuron activity.

**Key Features:**
- Movement generation and planning
- Action execution control
- Motor coordination
- Feedback control

### nlm::MotorSystem

The MotorSystem class provides neural control of actions and movements.

**Key Features:**
- Motor neuron activation
- Movement trajectory planning
- Motor adaptation and learning
- Sensor-motor integration

## Memory Systems

### nlm::WorkingMemory

The WorkingMemory class provides transient active information storage.

**Key Features:**
- Short-term memory storage
- Active maintenance of information
- Memory updating and refreshing
- Interference management

### nlm::EpisodicMemory

The EpisodicMemory class provides experience and event storage.

**Key Features:**
- Event recording and storage
- Temporal sequence memory
- Contextual association
- Memory replay and consolidation

### nlm::AssociativeMemory

The AssociativeMemory class provides pattern and concept associations.

**Key Features:**
- Pattern association and binding
- Concept formation and categorization
- Semantic memory
- Memory retrieval and completion

## Plasticity Systems

### nlm::STDP

The STDP (Spike-Timing Dependent Plasticity) class implements timing-based synaptic learning.

**Key Features:**
- Pre-post spike timing dependence
- Asymmetric synaptic modifications
- Learning window implementation
- Temporal credit assignment

### nlm::Hebbian

The Hebbian class implements co-activity based learning.

**Key Features:**
- Concurrent activity strengthening
- Correlated firing enhancement
- Memory consolidation
- Pattern completion

### nlm::StructuralPlasticity

The StructuralPlasticity class implements anatomical connectivity changes.

**Key Features:**
- New synapse formation
- Existing synapse elimination
- Dendritic spine dynamics
- Network rewiring

## Development Systems

### nlm::DevelopmentSystem

The DevelopmentSystem class coordinates overall brain development and maturation.

**Key Features:**
- Synaptogenesis (connection formation)
- Pruning (connection elimination)
- Maturation (system tuning)
- Activity-dependent development

### nlm::Synaptogenesis

The Synaptogenesis class implements initial connection formation.

**Key Features:**
- Initial neuron connectivity
- Competitive wiring
- Activity-dependent pruning
- Pattern formation

### nlm::Pruning

The Pruning class implements connection elimination and refinement.

**Key Features:**
- Synapse elimination
- Optimization of connectivity
- Efficiency improvement
- Refinement of circuits

### nlm::Maturation

The Maturation class implements system development and tuning.

**Key Features:**
- Parameter tuning
- Functional optimization
- Experience-dependent refinement
- Performance enhancement

## Neuromodulation Systems

### nlm::Neuromodulator

The Neuromodulator class provides general neuromodulatory signals.

**Key Features:**
- Global neural modulation
- State-dependent signaling
- Behavioral control
- Learning modulation

### nlm::Dopamine

The Dopamine class provides reward and reinforcement signaling.

**Key Features:**
- Reward prediction error
- Reinforcement learning
- Motivational control
- Hedonic signaling

### nlm::Reward

The Reward class provides reward signal processing.

**Key Features:**
- Reward detection and processing
- Valence assignment
- Motivational intensity
- Learning signal generation

### nlm::PredictionError

The PredictionError class provides surprise and learning signals.

**Key Features:**
- Unexpected event detection
- Learning signal generation
- Surprise computation
- Adaptive behavior

### nlm::Novelty

The Novelty class provides novelty detection and response.

**Key Features:**
- Novel stimulus detection
- Adaptation to novelty
- Exploration promotion
- Memory updating

### nlm::Curiosity

The Curiosity class provides exploration motivation.

**Key Features:**
- Intrinsic motivation
- Exploration drive
- Information seeking
- Behavioral flexibility

## Prediction Systems

### nlm::PredictionSystem

The PredictionSystem class provides predictive representations and forward models.

**Key Features:**
- Forward model implementation
- Predictive coding
- Expectation formation
- Error computation

### nlm::NeuralPrediction

The NeuralPrediction class provides neural implementation of prediction.

**Key Features:**
- Neural prediction mechanisms
- Predictive coding networks
- Expectation-based processing
- Prediction error minimization

## Cognition Systems

### nlm::NeuralPlanner

The NeuralPlanner class provides action planning and decision making.

**Key Features:**
- Goal-directed planning
- Action selection
- Sequential decision making
- Planning under uncertainty

### nlm::ConceptFormation

The ConceptFormation class provides pattern discovery and categorization.

**Key Features:**
- Pattern extraction
- Category formation
- Conceptual representation
- Abstract reasoning

### nlm::AttentionalSelection

The AttentionalSelection class provides focus and resource allocation.

**Key Features:**
- Attentional resource allocation
- Selective attention
- Feature binding
- Cognitive control

## Environment

### nlm::SimpleWorld

The SimpleWorld class provides environment simulation capabilities.

**Key Features:**
- Simple environment dynamics
- Stimulus presentation
- Response recording
- State management

### nlm::Observation

The Observation class provides environment observation interface.

**Key Features:**
- Sensory input processing
- Environmental analysis
- Feature extraction
- Multimodal integration

## Experimental Components

### nlm::Experiment

The Experiment class provides base functionality for experimental paradigms.

**Key Features:**
- Experimental protocol definition
- Data collection and analysis
- Result reporting
- Experimental control

### nlm::Metrics

The Metrics class provides experimental measurement and analysis.

**Key Features:**
- Performance metrics
- Statistical analysis
- Data visualization
- Results reporting

### nlm::Phase3Experiment

The Phase3Experiment class implements Phase 3 experimental protocol.

**Key Features:**
- Specific experimental paradigm
- Controlled conditions
- Data collection
- Analysis procedures

### nlm::Phase4Experiment

The Phase4Experiment class implements Phase 4 experimental protocol.

**Key Features:**
- Specific experimental paradigm
- Extended protocols
- Complex scenarios
- Advanced analysis

### nlm::Phase5Experiment

The Phase5Experiment class implements Phase 5 experimental protocol.

**Key Features:**
- Long-duration experiments
- Longitudinal studies
- Behavioral analysis
- Developmental assessment

### nlm::Phase6IntegratedExperiment

The Phase6IntegratedExperiment class implements Phase 6 integrated experimental protocol.

**Key Features:**
- Multimodal integration
- Complex behavioral tasks
- Advanced analysis
- Comprehensive assessment

### nlm::ScalingBenchmark

The ScalingBenchmark class provides performance benchmarking.

**Key Features:**
- Performance measurement
- Scalability analysis
- Resource monitoring
- Optimization opportunities

### nlm::AblationSystem

The AblationSystem class provides system ablation studies.

**Key Features:**
- Component removal
- Function analysis
- System mapping
- Recovery assessment

### nlm::ExperimentRunner

The ExperimentRunner class provides experiment execution and management.

**Key Features:**
- Experiment orchestration
- Protocol execution
- Data management
- Result aggregation

## Configuration Options

The NLM framework supports the following configuration options:

### Core Configuration
- **simulation_steps**: Total number of simulation steps
- **dt**: Simulation time step duration
- **seed**: Random number generator seed
- **log_level**: Logging verbosity level
- **enable_plasticity**: Enable/disable plasticity mechanisms

### Neural Configuration
- **num_neurons**: Number of neurons in the network
- **learning_rate**: Neural learning rate for plasticity
- **max_memory**: Maximum memory usage for working memory
- **neuron_type**: Default neuron type for population creation

### Plasticity Configuration
- **stdp_enabled**: Enable STDP learning
- **hebbian_enabled**: Enable Hebbian learning
- **reward_modulated_enabled**: Enable reward-modulated learning
- **plasticity_decay_rate**: Plasticity trace decay rate

### Development Configuration
- **synaptogenesis_enabled**: Enable developmental synaptogenesis
- **pruning_enabled**: Enable developmental pruning
- **maturation_enabled**: Enable developmental maturation
- **developmental_stage**: Current developmental stage

### Neuromodulation Configuration
- **dopamine_strength**: Dopamine signal strength
- **curiosity_strength**: Curiosity signal strength
- **novelty_strength**: Novelty signal strength
- **prediction_error_strength**: Prediction error signal strength

### Memory Configuration
- **working_memory_capacity**: Working memory capacity
- **episodic_memory_capacity**: Episodic memory capacity
- **associative_memory_capacity**: Associative memory capacity

## Examples

### Basic Simulation
```cpp
#include <nlm/nlm.hpp>

int main() {
    // Initialize configuration
    nlm::Config config;
    config.set("simulation_steps", 1000);
    config.set("learning_rate", 0.01);
    
    // Create and initialize brain
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    if (!brain.initialize()) {
        nlm::Logger::getGlobal().error("Brain initialization failed");
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Optional: Log progress periodically
        if (step % 100 == 0) {
            nlm::Logger::getGlobal().info(
                "Simulation step " + std::to_string(step)
            );
        }
    }
    
    // Reset and exit
    brain.reset();
    nlm::Logger::getGlobal().info("Simulation completed successfully");
    
    return 0;
}
```

### Custom Logger Configuration
```cpp
#include <nlm/nlm.hpp>

int main() {
    // Create and configure custom logger
    nlm::ConsoleLogger logger;
    logger.setLevel(nlm::LogLevel::Warning);
    logger.setUseColors(true);
    logger.setOutputFile("simulation.log");
    
    // Create custom logger manager
    nlm::Logger customLogger;
    customLogger.addLogger(std::make_shared<nlm::ConsoleLogger>(logger));
    
    // Set as global logger
    nlm::Logger::setGlobal(std::make_shared<nlm::Logger>(customLogger));
    
    // Use the custom logger
    nlm::Logger::getGlobal().warning("This warning will go to custom logger");
    
    return 0;
}
```

### Network Configuration with File I/O
```cpp
#include <nlm/nlm.hpp>

int main(int argc, char** argv) {
    // Create configuration
    nlm::Config config;
    
    // Load from file if specified
    std::string configFile = "config.json";
    if (argc > 1) {
        configFile = argv[1];
    }
    
    if (!config.loadFromFile(configFile)) {
        nlm::Logger::getGlobal().warning(
            "Failed to load configuration from " + configFile
        );
    }
    
    // Load from command line arguments
    config.loadFromArgs(argc, argv);
    
    // Override specific settings
    config.set("learning_rate", 0.05, nlm::ConfigSource::Runtime);
    config.set("simulation_steps", 2000, nlm::ConfigSource::Runtime);
    
    // Display configuration summary
    nlm::Logger::getGlobal().info("Configuration:\n" + config.summary());
    
    // Create and run brain
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    if (brain.initialize()) {
        for (int step = 0; step < config.getOr("simulation_steps", 1000); ++step) {
            brain.step(step);
        }
        brain.save("final_state.brain");
    }
    
    return 0;
}
```

### Error Handling and Recovery
```cpp
#include <nlm/nlm.hpp>

void runSimulation(nlm::Brain& brain) {
    try {
        // Run simulation with error handling
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            
            // Check for errors and recover if needed
            if (brain.getTotalSpikeCount() > 1000000) {
                nlm::Logger::getGlobal().warning(
                    "High spike count detected, applying recovery"
                );
                brain.reset();
            }
        }
        
    } catch (const nlm::NLMException& e) {
        // Handle NLM-specific exceptions
        nlm::Logger::getGlobal().error(
            "NLM exception: " + std::string(e.what())
        );
        throw; // Re-throw after logging
        
    } catch (const std::exception& e) {
        // Handle standard exceptions
        nlm::Logger::getGlobal().error(
            "Standard exception: " + std::string(e.what())
        );
        throw nlm::NLMRuntimeError(
            "Runtime error in simulation: " + std::string(e.what())
        );
        
    } catch (...) {
        // Handle unexpected exceptions
        nlm::Logger::getGlobal().critical(
            "Unknown exception in simulation"
        );
        throw nlm::NLMRuntimeError("Unknown exception in simulation");
    }
}

int main() {
    // Initialize brain
    nlm::Brain brain(std::make_shared<nlm::Config>());
    if (!brain.initialize()) {
        nlm::Logger::getGlobal().error("Brain initialization failed");
        return 1;
    }
    
    // Run simulation with comprehensive error handling
    try {
        runSimulation(brain);
        nlm::Logger::getGlobal().info("Simulation completed successfully");
        
    } catch (const nlm::NLMException& e) {
        nlm::Logger::getGlobal().error(
            "Simulation failed: " + std::string(e.what())
        );
        return 1;
    }
    
    return 0;
}
```

## Getting Started

### Installation
1. Clone the NLM framework repository
2. Configure build system (CMake or similar)
3. Build the framework
4. Set up development environment

### Basic Usage
```bash
# Create a simple simulation
cat > simple_sim.cpp << EOF
#include <nlm/nlm.hpp>

int main() {
    nlm::Brain brain(std::make_shared<nlm::Config>());
    if (brain.initialize()) {
        brain.step(100);
        brain.reset();
    }
    return 0;
}
EOF

# Compile and run
g++ -std=c++11 simple_sim.cpp -o simple_sim -lnlm
./simple_sim
```

### Documentation Tools
The NLM framework uses Doxygen for documentation generation. To generate documentation:

```bash
# Install doxygen
apt-get install doxygen

# Generate documentation
doxygen Doxyfile

# View documentation
open html/index.html  # On macOS
xdg-open html/index.html  # On Linux
```

## Contributing

The NLM framework welcomes contributions from the community. Please follow the project's contribution guidelines when submitting changes.

## Support and Resources

- **Documentation**: This documentation (readthedocs.io)
- **Source Code**: GitHub repository
- **Issues**: Bug tracking and feature requests
- **Community**: Discussion forums and mailing lists
- **Examples**: Additional example applications

## License

This framework is licensed under the MIT License. See the LICENSE file for details.

---

*Last updated: 2024-12-27*
*Version: 1.0*