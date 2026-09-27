/**
 * @mainpage NLM Framework Documentation
 * 
 * # NLM Framework - Neural Network Modeling Framework
 * 
 * ## Overview
 * 
 * The NLM (Neural Network Modeling) Framework is a comprehensive C++ library for
 * simulating and modeling biological neural networks. It provides a complete
 * computational architecture for neural computation, learning, and cognition
 * based on biological principles.
 * 
 * ## Key Features
 * 
 * - **Biological Plausibility**: Implements Leaky Integrate-and-Fire (LIF) neuron
 *   models and synaptic transmission dynamics
 * - **Comprehensive Architecture**: Complete neural system with sensory, motor,
 *   memory, and cognitive systems
 * - **Learning Mechanisms**: Supports STDP, Hebbian, reward-modulated, and
 *   structural plasticity
 * - **Development**: Implements brain development with synaptogenesis,
 *   pruning, and maturation
 * - **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
 * - **Memory Systems**: Working memory, episodic memory, and associative memory
 * - **Prediction Systems**: Forward models and prediction error computation
 * - **Cognition**: Planning, concept formation, and attentional selection
 * - **Event-Driven Dynamics**: Real-time spiking neural computation with
 *   asynchronous event processing
 * - **Checkpointing**: Save/load functionality for persistent state
 * - **Flexible Configuration**: Type-safe configuration management
 * - **Comprehensive Logging**: Multi-level logging with file and console output
 * - **Exception Safety**: Robust error handling with detailed diagnostics
 * 
 * ## Architecture Overview
 * 
 * The NLM framework implements a complete cognitive architecture organized into
 * several major systems:
 * 
 * ### Core Components
 * - **Config**: Configuration management with type-safe value retrieval
 * - **Logger**: Multi-level logging with multiple output sinks
 * - **Exception**: Comprehensive exception hierarchy for error handling
 * - **Types**: Core neural data types and definitions
 * - **SimulationClock**: Time management for simulation steps
 * 
 * ### Neural Components
 * - **Neuron**: Single neuron implementation with LIF dynamics
 * - **Synapse**: Synaptic connection with transmission and learning
 * - **NeuralPopulation**: Group of neurons with shared properties
 * - **NeuralRegion**: Functional region containing multiple populations
 * - **Brain**: Central coordinator integrating all neural subsystems
 * 
 * ### Brain Systems
 * - **Brain**: Central brain coordinator
 * - **AgentBrain**: Agent-specific brain implementation
 * 
 * ### Sensory Systems
 * - **Vision**: Visual processing and feature extraction
 * - **Audio**: Auditory processing and sound analysis
 * - **InternalSignals**: Proprioception and interoception
 * - **SensoryInput**: Multimodal sensory input interface
 * 
 * ### Motor System
 * - **Action**: Movement generation and execution
 * - **MotorSystem**: Neural control of actions
 * 
 * ### Memory Systems
 * - **WorkingMemory**: Transient active information storage
 * - **EpisodicMemory**: Experience and event storage
 * - **AssociativeMemory**: Pattern and concept associations
 * 
 * ### Plasticity Systems
 * - **STDP**: Spike-timing dependent plasticity
 * - **Hebbian**: Co-activity based learning
 * - **StructuralPlasticity**: Anatomical connectivity changes
 * 
 * ### Development Systems
 * - **DevelopmentSystem**: Overall development coordination
 * - **Synaptogenesis**: Initial connection formation
 * - **Pruning**: Connection elimination and refinement
 * - **Maturation**: System development and tuning
 * 
 * ### Neuromodulation Systems
 * - **Neuromodulator**: General neuromodulatory signals
 * - **Dopamine**: Reward and reinforcement signaling
 * - **Reward**: Reward signal processing
 * - **PredictionError**: Surprise and learning signals
 * - **Novelty**: Novelty detection and response
 * - **Curiosity**: Exploration motivation
 * 
 * ### Prediction Systems
 * - **PredictionSystem**: Predictive representations and forward models
 * - **NeuralPrediction**: Neural implementation of prediction
 * 
 * ### Cognition Systems
 * - **NeuralPlanner**: Action planning and decision making
 * - **ConceptFormation**: Pattern discovery and categorization
 * - **AttentionalSelection**: Focus and resource allocation
 * 
 * ### Environment
 * - **SimpleWorld**: Simple environment simulation
 * - **Observation**: Environment observation interface
 * 
 * ### Experimental Components
 * - **Experiment**: Base class for experimental paradigms
 * - **Metrics**: Experimental measurement and analysis
 * - **Phase3Experiment**: Phase 3 experimental protocol
 * - **Phase4Experiment**: Phase 4 experimental protocol
 * - **Phase5Experiment**: Phase 5 experimental protocol
 * - **Phase6IntegratedExperiment**: Phase 6 integrated experimental protocol
 * - **ScalingBenchmark**: Performance benchmarking
 * - **AblationSystem**: System ablation studies
 * - **ExperimentRunner**: Experiment execution and management
 * 
 * ## Documentation Structure
 * 
 * This documentation provides:
 * 
 * ### API Reference
 * - **Class Documentation**: Detailed documentation for all public classes
 * - **Method Documentation**: Complete parameter and return value descriptions
 * - **Type Documentation**: All enum and type definitions
 * - **Example Usage**: Code examples for common tasks
 * 
 * ### Component Documentation
 * - **Core Components**: Config, Logger, Exception, Types, SimulationClock
 * - **Neural Components**: Neuron, Synapse, NeuralPopulation, NeuralRegion, Brain
 * - **Sensory Systems**: Vision, Audio, InternalSignals, SensoryInput
 * - **Motor System**: Action, MotorSystem
 * - **Memory Systems**: WorkingMemory, EpisodicMemory, AssociativeMemory
 * - **Plasticity Systems**: STDP, Hebbian, StructuralPlasticity
 * - **Development Systems**: DevelopmentSystem, Synaptogenesis, Pruning, Maturation
 * - **Neuromodulation Systems**: Neuromodulator, Reward, PredictionError, Novelty, Curiosity
 * - **Prediction Systems**: PredictionSystem, NeuralPrediction
 * - **Cognition Systems**: NeuralPlanner, ConceptFormation, AttentionalSelection
 * - **Environment**: SimpleWorld, Observation
 * - **Experimental Components**: All experiment types
 * 
 * ### Implementation Notes
 * - **Performance Considerations**: Optimization and performance characteristics
 * - **Error Handling**: Error conditions and recovery strategies
 * - **Design Decisions**: Rationale for architectural choices
 * - **Algorithm Descriptions**: Implementation details of neural dynamics
 * 
 * ### Usage Examples
 * - **Getting Started**: Basic framework initialization and usage
 * - **Network Construction**: Building and configuring neural networks
 * - **Simulation**: Running neural simulations
 * - **Learning**: Training networks with plasticity
 * - **Development**: Studying brain development processes
 * - **Cognition**: Implementing cognitive functions
 * - **Integration**: Combining different systems
 * 
 * ## Getting Started
 * 
 * ### Installation
 * 1. Clone the NLM framework repository
 * 2. Configure build system (CMake or similar)
 * 3. Build the framework
 * 4. Set up development environment
 * 
 * ### Basic Usage
 * 
 * @code
 * #include <nlm/nlm.hpp>
 * 
 * int main() {
 *     // Create configuration
 *     nlm::Config config;
 *     config.set("simulation_steps", 1000);
 *     config.set("learning_rate", 0.01);
 *     
 *     // Initialize brain
 *     nlm::Brain brain(std::make_shared<nlm::Config>(config));
 *     if (brain.initialize()) {
 *         // Run simulation
 *         for (int step = 0; step < 1000; ++step) {
 *             brain.step(step);
 *         }
 *     }
 *     return 0;
 * }
 * @endcode
 * 
 * ### Advanced Usage
 * 
 * @code
 * // Custom logger configuration
 * nlm::ConsoleLogger logger;
 * logger.setLevel(nlm::LogLevel::Warning);
 * logger.setUseColors(true);
 * logger.setOutputFile("simulation.log");
 * 
 * nlm::Logger customLogger;
 * customLogger.addLogger(std::make_shared<nlm::ConsoleLogger>(logger));
 * nlm::Logger::setGlobal(std::make_shared<nlm::Logger>(customLogger));
 * @endcode
 * 
 * ## Learning and Development
 * 
 * The NLM framework supports study of various aspects of neural computation:
 * 
 * ### Neural Dynamics
 * - Study LIF neuron dynamics and spiking patterns
 * - Explore synaptic transmission and plasticity
 * - Investigate network-level phenomena
 * 
 * ### Learning and Plasticity
 * - Implement and study different learning rules
 * - Explore reward-modulated learning
 * - Study developmental processes
 * 
 * ### Cognitive Functions
 * - Implement planning and decision making
 * - Study concept formation and categorization
 * - Explore attentional mechanisms
 * 
 * ### Experimental Paradigms
 * - Design and run neural experiments
 * - Measure and analyze experimental results
 * - Compare with theoretical predictions
 * 
 * ## Performance and Optimization
 * 
 * The NLM framework includes several performance optimization features:
 * - **Event-Driven Processing**: Asynchronous spike event handling
 * - **Memory Efficiency**: Efficient data structures and memory management
 * - **Vectorization Support**: Opportunities for SIMD optimization
 * - **Checkpointing**: Save/restore for long-running simulations
 * - **Configurable Performance**: Trade-offs between accuracy and speed
 * 
 * ## Contributing
 * 
 * The NLM framework welcomes contributions from the community. Please follow
 * the project's contribution guidelines when submitting changes.
 * 
 * ## Support and Resources
 * 
 * - **Documentation**: This documentation (readthedocs.io)
 * - **Source Code**: GitHub repository
 * - **Issues**: Bug tracking and feature requests
 * - **Community**: Discussion forums and mailing lists
 * - **Examples**: Additional example applications
 * 
 * @see nlm::Brain for main interface
 * @see nlm::Config for configuration
 * @see nlm::Logger for logging
 * @see nlm::NLMException for error handling
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning The NLM framework is under active development. APIs may change
 * in future versions. Always check documentation for updates.
 * 
 * This documentation is generated using Doxygen and provides comprehensive
 * coverage of the NLM framework API and functionality.
 */