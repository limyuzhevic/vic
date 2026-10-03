// Comprehensive documentation and examples for NLM
// This file provides extensive documentation and examples for all NLM features

#pragma once

#include "BrainConvenience.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

// Documentation generator class
class DocumentationGenerator {
public:
    DocumentationGenerator() {
        std::cout << "=== NLM Documentation and Examples ===" << std::endl;
        std::cout << "Generating comprehensive documentation for NLM users..." << std::endl;
    }
    
    // Generate main documentation
    void generateMainDocumentation() {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "NLM (Neural Learning Machine) - Complete Documentation" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        generateOverview();
        generateQuickStart();
        generateBrainBasics();
        generateConfigurationManagement();
        generateAgentWorldIntegration();
        generateAdvancedFeatures();
        generatePerformanceConsiderations();
        generateTroubleshooting();
        generateAPIReference();
        
        std::cout << std::endl;
        std::cout << "=== Documentation Generation Complete ===" << std::endl;
    }
    
    // Generate overview section
    void generateOverview() {
        std::cout << "\n1. OVERVIEW" << std::endl;
        std::cout << "============" << std::endl;
        std::cout << R"(
NLM (熙然, meaning "serene flow") is an experimental computational brain project.
It implements a brain-inspired architecture with neurons and synapses that can
learn, remember, and make decisions through interaction with an environment.

Key Features:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Predictive processing and planning
- Developmental stages
- Checkpointing and replay capabilities
- Real-time simulation with configurable timestep

Architecture:
- Brain: Central neural processing unit
- World: Environment simulation
- Agent: Interface between brain and world
- Memory: Multiple memory systems
- Prediction: Forward and inverse models
- Cognition: Planning and decision making

This documentation provides comprehensive examples and best practices for
using NLM in research and applications.
)" << std::endl;
    }
    
    // Generate quick start section
    void generateQuickStart() {
        std::cout << "\n2. QUICK START" << std::endl;
        std::cout << "=============" << std::endl;
        std::cout << R"(
This section provides the quickest way to get started with NLM.

Step 1: Installation
-------------------
# Build from source
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install scikit-build-core pybind11
pip install .

Step 2: Basic Usage
------------------
The simplest way to create and use a brain:

#include "Brain.hpp"
#include "core/Config/Config.hpp"

using namespace nlm;

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->setInt("brain.neuronCount", 1000);
    config->setDouble("brain.connectionProbability", 0.1);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    std::cout << "Brain simulation complete!" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}

Step 3: Using Convenience Functions
----------------------------------
For even simpler usage, use the convenience functions:

#include "BrainConvenience.hpp"

// Quick brain creation with default settings
auto brain = createBrainWithConfig(1000, 0.1f, true, false);
brain->initialize();

// Quick simulation
runQuickSimulation(brain, nullptr, nullptr, 100);

Step 4: Running Examples
----------------------
After building, run the example executables:

# Basic convenience test
./nlm_convenience_test

# Documentation examples
./nlm_documentation

# Phase 3 demo
./nlm_phase3_demo

# Phase 4 demo
./nlm_phase4_demo

# Phase 6 integration demo
./nlm_phase6_python_manager

Step 5: Python Usage
-------------------
Python provides a high-level interface to NLM:

import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 1000)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")

Recommended Learning Path:
1. Start with "Quick Start" examples
2. Try the convenience functions for simple tasks
3. Study the detailed examples for complex applications
4. Refer to API reference for advanced usage
) " << std::endl;
    }
    
    // Generate brain basics section
    void generateBrainBasics() {
        std::cout << "\n3. BRAIN BASICS" << std::endl;
        std::cout << "==============" << std::endl;
        std::cout << R"(
This section explains the core brain components and concepts.

3.1 Brain Components
--------------------
A NLM brain consists of several key components:

Neuron Types:
- Excitatory: Positive current input, increases membrane potential
- Inhibitory: Negative current input, decreases membrane potential
- Modulatory: Affects plasticity and neuromodulation
- Sensory: Receive input from environment
- Motor: Produce output actions
- Internal: Maintain internal state

Brain Regions:
- Cortex: Primary processing region
- Thalamus: Sensory relay
- Hippocampus: Memory formation
- Amygdala: Emotional processing
- Cerebellum: Motor coordination

3.2 Key Concepts
---------------
Leaky Integrate-and-Fire (LIF) Dynamics:
V(t+dt) = V(t) + dt * ((V_rest - V(t))/τ + I/C)
where:
- V: Membrane potential
- V_rest: Resting potential (-70mV typical)
- τ: Time constant (20ms typical)
- I: Input current
- C: Membrane capacitance

Spike Threshold:
When V ≥ V_thresh (-55mV typical), neuron fires a spike
After spike: V = V_reset (-75mV typical)
Refractory period: τ_ref (2ms typical)

3.3 Memory Systems
-------------------
Working Memory:
- Short-term storage of active information
- Neural population activity
- Competitive dynamics

Episodic Memory:
- Experience encoding and replay
- Pattern completion
- Temporal sequence storage

Associative Memory:
- Pattern associations
- Hebbian learning rules
- Predictive coding

3.4 Plasticity Mechanisms
------------------------
STDP (Spike-Timing-Dependent Plasticity):
- Pre-before-post: Long-Term Potentiation (LTP)
- Post-before-pre: Long-Term Depression (LTD)
- Asymmetric weight changes

Hebbian Learning:
- Correlated activity strengthens connections
- Neuromodulator-dependent

Structural Plasticity:
- Synaptogenesis (formation of new connections)
- Pruning (elimination of weak connections)
- Experience-dependent

3.5 Development
---------------
Development stages:
- Initial: High plasticity, exploration
- Critical Period: Rapid learning
- Maturation: Stabilization
- Adult: Fine-tuning and maintenance
- Aging: Gradual decline

Developmental processes:
- Synaptogenesis: Formation of connections
- Pruning: Elimination of connections
- Myelination: Insulation of axons
- Neurogenesis: Formation of new neurons

3.6 Neuromodulation
------------------
Dopamine:
- Reward prediction error
- Reinforcement learning
- Motivation

Curiosity:
- Novelty detection
- Exploration drive
- Intrinsic motivation

Novelty:
- Unexpected events
- Change detection
- Attention allocation

Prediction Error:
- Model-based learning
- Surprise signals
- Learning rate modulation
) " << std::endl;
    }
    
    // Generate configuration management section
    void generateConfigurationManagement() {
        std::cout << "\n4. CONFIGURATION MANAGEMENT" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << R"(
This section covers configuration management and customization options.

4.1 Configuration Structure
---------------------------
NLM uses a hierarchical configuration system with key-value pairs.
Configuration is stored in Config objects and can be loaded from:
- Default settings
- Configuration files (JSON, key=value format)
- Command line arguments
- Runtime modifications

4.2 Configuration Categories
--------------------------
Brain Configuration:
- brain.neuronCount: Number of neurons
- brain.connectionProbability: Connection density
- brain.v_thresh: Spike threshold
- brain.v_rest: Resting potential
- brain.v_reset: Reset potential
- brain.tau_mem: Membrane time constant
- brain.tau_ref: Refractory period

Plasticity Configuration:
- plasticity.stdp.enable: STDP enabled
- plasticity.stdp.learningRate: STDP learning rate
- plasticity.hebbian.enable: Hebbian enabled
- plasticity.structural.enable: Structural plasticity enabled

Memory Configuration:
- memory.workingMemory.enable: Working memory enabled
- memory.episodicMemory.enable: Episodic memory enabled
- memory.associativeMemory.enable: Associative memory enabled

Neuromodulation Configuration:
- neuromod.dopamine.scale: Dopamine scale
- neuromod.curiosity.enable: Curiosity enabled
- neuromod.novelty.enable: Novelty detection enabled

Development Configuration:
- development.enabled: Development enabled
- development.plasticityRate: Developmental plasticity rate
- development.stageDuration: Duration per developmental stage

4.3 Configuration Presets
------------------------
NLM provides predefined configuration presets:

Minimal Configuration:
- Small brain size (100 neurons)
- No plasticity
- Basic working memory
- Fast simulation

Balanced Configuration:
- Medium brain size (1000 neurons)
- Basic plasticity
- Working and episodic memory
- Standard simulation

Advanced Configuration:
- Large brain size (5000 neurons)
- Full plasticity (STDP, Hebbian, structural)
- All memory systems
- Advanced neuromodulation
- Development enabled

Custom Configuration:
- Load from JSON file
- Command line arguments
- Runtime modifications

4.4 Configuration Examples
-------------------------
C++ Configuration Example:
#include "Config.hpp"

nlm::Config config;
config.setInt("brain.neuronCount", 1000);
config.setDouble("brain.connectionProbability", 0.1);
config.setBool("plasticity.enabled", true);
config.setString("experiment.name", "learning_study");

Python Configuration Example:
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 1000)
config.set("brain.connectionProbability", 0.1)
config.set("plasticity.enabled", True)
config.set("experiment.name", "learning_study")

4.5 Configuration Validation
--------------------------
NLM provides configuration validation:

Type checking: Ensure values match expected types
Range checking: Validate numeric ranges
Format checking: Validate string formats
Dependency checking: Validate configuration dependencies
Custom validation: User-defined validation functions

Validation examples:
bool validateNeuronCount(const nlm::ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) >= 10 && std::get<int>(value) <= 10000;
    }
    return false;
}

config.addCustomValidator("brain.neuronCount", validateNeuronCount);

4.6 Advanced Configuration Features
---------------------------------
Dynamic Configuration:
- Enable/disable features at runtime
- Modify parameters during simulation
- Save/load configuration states

Configuration Comparison:
- Compare two configurations
- Find differences
- Merge configurations

Configuration History:
- Save configuration snapshots
- Load previous configurations
- Configuration versioning

Performance Configuration:
- Optimization settings
- Memory allocation strategies
- Parallel processing options
) " << std::endl;
    }
    
    // Generate agent world integration section
    void generateAgentWorldIntegration() {
        std::cout << "\n5. AGENT WORLD INTEGRATION" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << R"(
This section covers integration between brain, agent, and world components.

5.1 Agent System
---------------
The agent system provides an interface between the brain and the world:

AgentBrain:
- Sensory processing: Convert world percepts to neural input
- Motor decoding: Convert neural activity to actions
- Neuromodulation: Apply reward-based learning
- Development: Update developmental state
- Configuration: Feature enable/disable

AgentBody:
- Position: (x, y) coordinates
- Orientation: Facing direction
- Velocity: Movement speed and direction
- Energy: Energy resources
- Health: Health status
- Age: Developmental age

5.2 World System
---------------
The world system simulates the environment:

SimpleWorld:
- 2D grid world
- Vision sensing: Field of view (width x height)
- Collision detection
- Object interaction
- Reward system
- Time stepping

World Objects:
- Resources: Items that can be collected
- Hazards: Dangerous items
- Walls: Static obstacles
- Markers: Waypoints or goals

5.3 Integration Patterns
-----------------------
Basic Integration:
1. Create world
2. Configure world dimensions
3. Create brain with appropriate configuration
4. Create agent brain interface
5. Initialize agent with world
6. Run simulation loop

Complete Integration Example:
#include "SimpleWorld.hpp"
#include "BrainConvenience.hpp"

using namespace nlm;

int main() {
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Create brain and agent using convenience function
    auto [brain, agent] = createBrainAndAgent(*world, 200, 0.05f);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run simulation
    for (int i = 0; i < 100; ++i) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(i);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Optional: Apply reward and development
        agent->applyRewardModulation(0.1f, 0.0f);
        agent->updateDevelopment(0.1);
        
        if (i % 20 == 0) {
            std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() 
                      << " firing neurons, " << agent->getCuriosityLevel() << " curiosity" << std::endl;
        }
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}

5.4 Sensory Processing
--------------------
Sensory input comes in different forms:

Vision:
- 2D grid of visual information
- Resolution: world.visionWidth x world.visionHeight
- Values: 0.0 (dark) to 1.0 (bright)
- Used by visual neurons

Touch:
- Collision information
- 1D array of proximity values
- Binary (collision/no collision)
- Used by tactile neurons

Internal Signals:
- Internal state representation
- Energy, hunger, thirst, etc.
- Normalized to 0.0-1.0 range
- Used by homeostatic neurons

Proprioception:
- Body position and orientation
- Movement information
- Used by motor planning neurons

5.5 Motor Output
---------------
Motor commands:
- Movement actions: MoveForward, MoveBackward, TurnLeft, TurnRight
- Interaction actions: Interact, Eat, Drink, Look
- Rest actions: Wait, Rest

Action Structure:
- Type: ActionType enumeration
- Parameters: Additional action parameters (force, duration, etc.)
- Context: Situation or conditions
- Constraints: Limitations or restrictions

5.6 Learning and Adaptation
--------------------------
The agent learns through interaction with the world:

Reward Modulation:
- Receive rewards from world
- Update expected rewards
- Compute prediction errors
- Modify synaptic weights

Development:
- Update developmental age
- Modify plasticity based on age
- Change behavior patterns
- Adapt to environment

Curiosity:
- Detect novel events
- Explore new areas
- Learn about environment
- Maximize information gain

5.7 Debugging and Monitoring
---------------------------
Monitor agent performance:
- Sensory input quality
- Motor output effectiveness
- Reward accumulation
- Learning progress
- Curiosity levels
- Developmental stage
) " << std::endl;
    }
    
    // Generate advanced features section
    void generateAdvancedFeatures() {
        std::cout << "\n6. ADVANCED FEATURES" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << R"(
This section covers advanced NLM features for research and complex applications.

6.1 Phase 6 Integration
----------------------
Phase 6 represents the final integration of all brain systems:

Integration Verification:
- Memory systems are connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
- Development affects plasticity rates
- Prediction system is integrated
- Replay and consolidation are functional
- Checkpoint save/load works

Phase 6 Features:
- Working memory integration with neural processing
- Episodic memory encoding and replay
- Neuromodulation affecting synaptic plasticity
- Prediction error signals guiding learning
- Developmental stages affecting behavior
- Memory consolidation during rest
- Checkpoint saving for large simulations
- Replay of important experiences

6.2 Advanced Memory Systems
-------------------------
Neural Working Memory:
- Persistent activity neural populations
- Competitive selection dynamics
- Sustained firing for working memory
- Integration with attention

Neural Episodic Memory:
- Experience encoding and storage
- Pattern completion during recall
- Temporal sequence representation
- Memory consolidation

Associative Memory:
- Pattern associations
- Semantic relationships
- Concept formation
- Transfer learning

6.3 Advanced Plasticity
---------------------
Multi-Factor Plasticity:
- Combine STDP, Hebbian, and reward modulation
- Weight dependence
- Activity dependence
- Neuromodulator dependence

Structural Plasticity:
- Synaptogenesis: Formation of new connections
- Pruning: Elimination of weak connections
- Rewiring: Reorganization of existing connections
- Experience-dependent

Metaplasticity:
- Plasticity of plasticity
- Learning rate modulation
- Homeostatic regulation
- Experience-dependent calibration

6.4 Advanced Neuromodulation
-------------------------
Multi-Neuromodulator System:
- Dopamine: Reward prediction error
- Serotonin: Aversive prediction error
- Acetylcholine: Attention and arousal
- Norepinephrine: Novelty and alertness
- Endorphins: Pain and pleasure

Integration with Learning:
- Reward prediction error signals
- Curiosity-driven exploration
- Novelty detection and response
- Aversive conditioning

6.5 Advanced Prediction
--------------------
Predictive Coding:
- Forward models: Predict sensory input
- Inverse models: Calculate actions
- Prediction errors: Compare predictions with actual input
- Hierarchical prediction: Multiple levels of abstraction

Bayesian Inference:
- Probabilistic reasoning
- Belief updating
- Uncertainty quantification
- Decision making under uncertainty

6.6 Advanced Cognition
--------------------
Neural Planning:
- Action sequence evaluation
- Value-based planning
- Model-based planning
- Hierarchical planning

Concept Formation:
- Pattern discovery
- Abstraction learning
- Category formation
- Transfer learning

Social Learning:
- Observation of others
- Imitation learning
- Cultural transmission
- Social influence

6.7 Advanced Development
----------------------
Life Span Development:
- Infant: High plasticity, exploration
- Child: Rapid learning, skill acquisition
- Adult: Optimization, expertise
- Aging: Decline, compensation

Experience-Dependent Development:
- Sensitive periods
- Critical learning windows
- Experience-expectant development
- Neuroplasticity modulation

6.8 Performance Optimizations
---------------------------
Memory Pools:
- Pre-allocated memory blocks
- Reduced garbage collection
- Faster object creation
- Better cache locality

Event Queues:
- Asynchronous processing
- Spike event queues
- Learning event queues
- World update queues

Parallel Processing:
- Multi-threading
- SIMD vectorization
- GPU acceleration
- Distributed computing

6.9 Visualization and Analysis
----------------------------
Visualization Interface:
- Neural activity visualization
- Memory traces visualization
- Learning progress visualization
- Behavior analysis

Analysis Tools:
- Statistical analysis
- Time series analysis
- Network analysis
- Machine learning integration

6.10 Research Applications
-------------------------
Neuroscience Research:
- Model of biological neural systems
- Testing hypotheses
- Computational neuroscience
- Brain-inspired AI

Artificial Intelligence:
- Neuromorphic computing
- Brain-inspired AI
- Self-organizing systems
- Adaptive control

Robotics:
- Neural control systems
- Learning from interaction
- Autonomous behavior
- Skill acquisition

Education:
- Neural learning demonstrations
- Interactive simulations
- Educational games
- Research tools
) " << std::endl;
    }
    
    // Generate performance considerations section
    void generatePerformanceConsiderations() {
        std::cout << "\n7. PERFORMANCE CONSIDERATIONS" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << R"(
This section covers performance optimization and scalability considerations.

7.1 Performance Bottlenecks
--------------------------
Common performance bottlenecks and solutions:

Memory Usage:
- Issue: Large neural populations consume memory
- Solution: Use memory pools, compressed representations
- Impact: Can reduce memory by 50%+

Spike Processing:
- Issue: Event-driven spike propagation can be expensive
- Solution: Batch processing, optimized data structures
- Impact: Can improve by 10-100x

Plasticity Computation:
- Issue: Synaptic weight updates require synchronization
- Solution: Local plasticity, asynchronous updates
- Impact: Can scale to millions of neurons

Plasticity Computation:
- Issue: Synaptic weight updates require synchronization
- Solution: Local plasticity, asynchronous updates
- Impact: Can scale to millions of neurons

7.2 Scalability
--------------
NLM can scale to:

Small Systems (10-100 neurons):
- Single-threaded simulation
- Real-time performance
- Interactive exploration
- Educational use

Medium Systems (1,000-10,000 neurons):
- Multi-threaded simulation
- Sub-second timesteps
- Research applications
- Complex behavior

Large Systems (10,000-100,000 neurons):
- Distributed computing
- GPU acceleration
- Scientific computing
- Large-scale simulations

Very Large Systems (100,000+ neurons):
- Supercomputing
- Special hardware
- Massive parallel processing
- Extreme scale modeling

7.3 Memory Management
--------------------
Memory optimization strategies:

Memory Pools:
- Pre-allocate memory blocks
- Reduce fragmentation
- Improve cache locality
- Lower garbage collection overhead

Compressed Representations:
- Sparse matrices
- Quantized weights
- Compressed spike trains
- Differential encoding

In-Place Operations:
- Vectorized operations
- Template metaprogramming
- Compile-time optimization
- Runtime optimization

7.4 Performance Profiling
------------------------
NLM provides comprehensive performance profiling:

Timing Metrics:
- Neuron simulation time
- Spike propagation time
- Plasticity computation time
- Memory allocation time

Memory Metrics:
- Memory allocation patterns
- Garbage collection frequency
- Cache performance
- Memory bandwidth usage

CPU Metrics:
- Core utilization
- Instruction throughput
- Branch prediction
- Pipeline efficiency

7.5 Benchmarking
----------------
NLM benchmark suites:

Microbenchmarks:
- Neuron simulation speed
- Spike propagation speed
- Plasticity update speed
- Memory access speed

Integration benchmarks:
- Complete simulation performance
- Memory system integration
- Learning efficiency
- Behavioral benchmarks

Real-world benchmarks:
- Task completion time
- Learning speed
- Accuracy metrics
- Resource utilization

7.6 Optimization Strategies
--------------------------
Algorithm Optimization:
- Use efficient data structures
- Minimize memory access
- Optimize for cache locality
- Use vectorized operations

Implementation Optimization:
- Compiler optimizations
- Link-time optimization
- Profile-guided optimization
- Architecture-specific optimizations

Hardware Optimization:
- SIMD vectorization
- GPU acceleration
- Multi-core threading
- Specialized hardware

7.7 Performance Trade-offs
-------------------------
Speed vs. Accuracy:
- Larger timesteps improve speed but reduce accuracy
- Fewer neurons improve speed but reduce complexity
- Simpler models improve speed but reduce realism

Complexity vs. Performance:
- More features improve realism but reduce performance
- Advanced plasticity improves learning but increases computation
- Multiple memory systems improve capability but increase complexity

7.8 Monitoring and Analysis
-------------------------
Performance monitoring tools:
- Real-time profiling
- Performance counters
- Memory tracking
- CPU utilization

Performance analysis:
- Bottleneck identification
- Optimization opportunities
- Scalability analysis
- Resource planning
) " << std::endl;
    }
    
    // Generate troubleshooting section
    void generateTroubleshooting() {
        std::cout << "\n8. TROUBLESHOOTING" << std::endl;
        std::cout << "=================" << std::endl;
        std::cout << R"(
This section provides common troubleshooting techniques and solutions.

8.1 Common Issues
-----------------
Memory Issues:
- Symptom: Excessive memory usage
- Cause: Memory leaks, inefficient data structures
- Solution: Use memory pools, profile memory usage

Performance Issues:
- Symptom: Slow simulation
- Cause: Bottlenecks, inefficient algorithms
- Solution: Profile, optimize critical paths

Stability Issues:
- Symptom: Crashes or freezes
- Cause: Null pointers, invalid parameters
- Solution: Validate inputs, use proper error handling

Configuration Issues:
- Symptom: Unexpected behavior
- Cause: Incorrect configuration values
- Solution: Validate configuration, use presets

8.2 Error Messages
-----------------
Configuration Errors:
- "Configuration key not found: <key>"
- "Type mismatch for key \"<key>\": expected <type>, got <actual>"
- "Configuration validation failed: <reason>"

Simulation Errors:
- "Brain not initialized"
- "Invalid timestep"
- "Memory allocation failed"

Runtime Errors:
- "Division by zero"
- "Null pointer access"
- "File not found"

8.3 Debugging Techniques
----------------------
Enable Debug Logging:
- Set log level to Debug
- Use NLM_LOG_DEBUG macros
- Check console output for detailed information

Use Logging Tools:
- Enable file logging
- Monitor log files
- Search for specific patterns

Performance Profiling:
- Use PerformanceTimer for profiling
- Monitor performance metrics
- Identify bottlenecks

Memory Debugging:
- Use memory debugging tools
- Check for memory leaks
- Validate memory access patterns

8.4 Common Solutions
------------------
Memory Management:
system_info = (
    "Memory Usage: {memory_usage:.2f} MB\n"
    "Peak Memory: {peak_memory:.2f} MB\n"
    "Allocation Count: {allocation_count}\n"
    "Deallocation Count: {deallocation_count}"
)

Performance Optimization:
configuration = (
    "Simulation Time: {simulation_time:.3f} ms\n"
    "Neuron Count: {neuron_count}\n"
    "Spike Events: {spike_events}\n"
    "Plasticity Updates: {plasticity_updates}"
)

Stability Verification:
verification = (
    "Brain State: {brain_state}\n"
    "Configuration Validity: {config_valid}\n"
    "Memory Validation: {memory_valid}\n"
    "Thread Safety: {thread_safe}"
)

8.5 Error Recovery
------------------
Graceful Degradation:
- Try alternative configurations
- Fall back to simpler models
- Use default parameters

Retry Logic:
- Retry failed operations
- Exponential backoff
- Circuit breaker pattern

Fallback Strategies:
- Use safe default values
- Disable problematic features
- Reduce complexity

8.6 Diagnostic Tools
------------------
Debug Brain State:
- Check neuron counts
- Verify firing rates
- Validate synaptic weights
- Monitor memory usage

Configuration Validation:
- Check required parameters
- Validate type constraints
- Verify range constraints
- Test custom validators

Performance Monitoring:
- Track execution time
- Monitor memory usage
- Check CPU utilization
- Analyze I/O patterns

8.7 Getting Help
--------------
Documentation:
- Read this documentation
- Check API reference
- Review examples
- Study tutorials

Community Support:
- Join NLM mailing list
- Participate in forums
- Report issues
- Contribute code

Technical Support:
- Contact technical team
- File bug reports
- Request features
- Get assistance

8.8 Best Practices
-----------------
Development Practices:
- Use modern C++ features
- Write clean, maintainable code
- Validate inputs thoroughly
- Use proper error handling

Testing Practices:
- Unit tests for all components
- Integration tests for systems
- Performance benchmarks
- Stress testing

Deployment Practices:
- Profile performance
- Monitor resource usage
- Set up alerting
- Implement rollback
) " << std::endl;
    }
    
    // Generate API reference section
    void generateAPIReference() {
        std::cout << "\n9. API REFERENCE" << std::endl;
        std::cout << "=================" << std::endl;
        std::cout << R"(
This section provides a comprehensive API reference for NLM.

9.1 Core Classes
---------------
Config:
- Configuration management
- Key-value storage
- File I/O
- Validation

Brain:
- Neural simulation
- Memory management
- Plasticity
- Development

AgentBrain:
- Sensor-motor interface
- Neuromodulation
- Learning
- Development

SimpleWorld:
- Environment simulation
- Sensory input
- Motor output
- World objects

9.2 Convenience Functions
------------------------
Convenience functions for simplified usage:

createBrainWithConfig(...)
createBrainAndAgent(...)
runQuickSimulation(...)
createQuickConfig(...)
setupQuickExperiment(...)
validateBrainSetup(...)
saveBrainSnapshot(...)
loadBrainSnapshot(...)
getBrainStatsSummary(...)
calculateSimilarity(...)
debugBrainState(...)

9.3 Configuration Presets
------------------------
Predefined configuration sets:

Minimal:
- 100 neurons
- No plasticity
- Basic working memory

Balanced:
- 1000 neurons
- Basic plasticity
- Working and episodic memory

Advanced:
- 5000 neurons
- Full plasticity
- All memory systems
- Development enabled

9.4 Performance Monitoring
------------------------
Performance monitoring utilities:

PerformanceMonitor::getInstance()
PerformanceMonitor::recordMetric(...)
PerformanceMonitor::getMetric(...)
PerformanceMonitor::getAllMetrics()
PerformanceMonitor::getReport()

PerformanceTimer:
- Performance measurement
- Automatic logging
- Configurable categories

9.5 Logging Categories
--------------------
Log categories for organized logging:

LogCategory::Core
LogCategory::Brain
LogCategory::Memory
LogCategory::Plasticity
LogCategory::Development
LogCategory::Neuromodulation
LogCategory::Prediction
LogCategory::Cognition
LogCategory::Sensory
LogCategory::Motor
LogCategory::Environment
LogCategory::Experiment
LogCategory::Performance
LogCategory::Debug
LogCategory::User

9.6 Error Types
--------------
NLM provides comprehensive error handling:

ConfigError
ConfigKeyError
ConfigTypeError
ConfigValidationError

Standard exceptions:
std::runtime_error
std::bad_variant_access
std::out_of_range

9.7 Template Functions
--------------------
Template functions for flexible programming:

template<typename T>
std::optional<T> Config::getSafe(const std::string& key)

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue)

9.8 Constants and Enums
--------------------
NeuronType:
- Excitatory
- Inhibitory
- Modulatory
- Sensory
- Motor
- Internal

DevelopmentalStage:
- Initial
- CriticalPeriod
- Maturation
- Adult
- Aging

ActionType:
- MoveForward
- MoveBackward
- TurnLeft
- TurnRight
- Look
- LookUp
- LookDown
- Interact
- Eat
- Drink
- Rest
- Wait
- Custom

WorldObjectType:
- Empty
- Resource
- Hazard
- Wall
- Marker

9.9 Macro Utilities
------------------
Logging macros:
NLM_LOG_DEBUG(msg)
NLM_LOG_INFO(msg)
NLM_LOG_WARNING(msg)
NLM_LOG_ERROR(msg)
NLM_LOG_CRITICAL(msg)

Performance macros:
NLM_PERFORMANCE_TIMER(name, category)

Enhanced logging:
NLM_LOG_DEBUG_CATEGORY(msg, category)
NLM_LOG_INFO_CATEGORY(msg, category)
NLM_LOG_WARNING_CATEGORY(msg, category)
NLM_LOG_ERROR_CATEGORY(msg, category)

9.10 Python Bindings
------------------
Python API for NLM:

Configuration:
pynlm.createDefaultConfig()
config.set("key", "value")
config.saveToFile("config.txt")

Brain:
pynlm.createBrain(config)
brain.initialize()
brain.step(step)

Agent:
pynlm.createAgentBrain(brain)
agent.initialize(world)

World:
pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)

Statistics:
brain.getTotalNeuronCount()
brain.getAverageFiringRate()
agent.getCuriosityLevel()
agent.getNoveltyLevel()

Advanced Python Features:
pynlm.NLMPhase6PythonManager
createExplorationConfig()
createExploitationConfig()
createBalancedConfig()
calculateNovelty(...)
calculateCuriosity(...)
suggestExplorationAction(...)
" << std::endl;
    }
};