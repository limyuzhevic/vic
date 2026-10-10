# NLM Public API Documentation Standards and Guidelines

## Overview

This document defines the comprehensive documentation standards and guidelines for the NLM project. These standards ensure consistency, completeness, and quality across all public API documentation, enabling users to effectively understand and integrate with the NLM artificial brain system.

## Documentation Philosophy

### 1. Purpose-Driven Documentation
- **Goal-oriented**: Documentation should enable users to accomplish specific tasks with the NLM system
- **Action-oriented**: Focus on what users can DO with APIs, not just what APIs ARE
- **Integration-oriented**: Document how components work together in the complete brain system
- **Educational-oriented**: Balance technical detail with conceptual understanding

### 2. Consistency with Project Conventions
- **Code style**: Maintain same formatting and style as existing NLM code
- **Comment style**: Use `//` style comments (not `/* */` blocks)
- **Namespace**: All documentation within `nlm` namespace
- **PImpl pattern**: Document through public interfaces, not private implementations
- **Move semantics**: Document copy/delete patterns consistently

### 3. User-Centered Approach
- **Multiple user levels**: Support both novice users and expert developers
- **Progressive disclosure**: Start simple, provide advanced details when needed
- **Practical examples**: Include working code snippets where helpful
- **Error handling**: Document error conditions and recovery strategies

## Documentation Standards

### 1. File-Level Documentation

#### Header File Structure
Each .hpp file should include:

```cpp
/**
 * @file [filename].hpp
 * 
 * [Brief file description]
 * 
 * [Key features and purposes]
 * 
 * @note [Important implementation details or limitations]
 * @see [Related files or classes]
 */
```

#### Example:
```cpp
/**
 * @file src/brain/Brain.hpp
 * 
 * The central coordinator of the NLM neural system
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems
 * 
 * Key features:
 * - Event-driven neural computation using LIF neurons
 * - Integrated memory systems (working, episodic, semantic, procedural, associative)
 * - Predictive coding and forward models
 * - Cognitive planning and attention mechanisms
 * - Neuromodulation for motivation and learning
 * - Developmental plasticity and maturation
 * - Checkpoint persistence and replay capabilities
 * 
 * The brain follows a strict processing pipeline:
 * WORLD → SENSORY → NEURAL PROCESSING → INTERNAL STATE →
 * MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION →
 * ACTION SELECTION → MOTOR → WORLD → REWARD/SURPRISE/ERROR →
 * PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN
 * 
 * @note This class implements Phase 6 of the NLM development timeline
 * @see src/brain/Neuron.hpp, src/memory/Memory.hpp, src/cognition/NeuralPlanner.hpp
 */
```

### 2. Class Documentation

#### Class Overview
Each public class should have comprehensive documentation covering:

```cpp
/**
 * @brief [Brief class description]
 * 
 * [Detailed explanation of the class's purpose and role]
 * 
 * Key features:
 * - [Feature 1 description]
 * - [Feature 2 description]
 * - [Feature 3 description]
 * 
 * [Architectural context and integration points]
 * [Phase 6 completion status]
 * [Future development notes]
 * 
 * @note [Important implementation details or limitations]
 * @see [Related classes and files]
 * @warning [Potential pitfalls or usage restrictions]
 */
class ClassName {
    // class implementation
};
```

#### Example (from Brain.hpp):
```cpp
/**
 * @brief The central coordinator of the NLM neural system
 * 
 * The Brain class implements a complete artificial developmental brain architecture,
 * integrating spiking neural computation with memory, prediction, cognition, and
 * neuromodulation systems. It serves as the main interface for controlling and
 * interacting with the artificial brain.
 * 
 * The brain operates as a closed-loop system processing sensory input, generating
 * actions, and learning from environmental feedback through plasticity mechanisms.
 * 
 * Key features:
 * - Event-driven neural computation with LIF neurons and spike propagation
 * - Integrated memory systems (working, episodic, semantic, procedural, associative)
 * - Predictive coding and forward models
 * - Cognitive planning and attention mechanisms
 * - Neuromodulation for motivation and learning
 * - Developmental plasticity and maturation
 * - Checkpoint persistence and replay capabilities
 * 
 * The brain follows a strict processing pipeline:
 * WORLD → SENSORY → NEURAL PROCESSING → INTERNAL STATE →
 * MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION →
 * ACTION SELECTION → MOTOR → WORLD → REWARD/SURPRISE/ERROR →
 * PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN
 * 
 * @note This class implements Phase 6 of the NLM development timeline
 * @see src/brain/Neuron.hpp, src/memory/Memory.hpp, src/cognition/NeuralPlanner.hpp
 */
class Brain {
    // class implementation
};
```

### 3. Method Documentation

#### Method Structure
Each public method should be documented as:

```cpp
/**
 * @brief [Brief method description]
 * 
 * [Detailed explanation of what the method does and why]
 * 
 * @param [param_name] [Description of parameter and any constraints]
 * @param [param_name2] [Description of second parameter]
 * 
 * @return [Description of return value and any special cases]
 * 
 * @note [Important implementation details or caveats]
 * @see [Related methods or classes]
 * @warning [Potential errors or usage restrictions]
 * 
 * Example usage:
 * @code
 * Brain brain(config);
 * brain.initialize();
 * // ... brain operation ...
 * @endcode
 */
ReturnType methodName(ParamType1 param1, ParamType2 param2) {
    // implementation
}
```

#### Example (from Neuron.hpp):
```cpp
/**
 * @brief Execute one step of Leaky Integrate-and-Fire dynamics
 * 
 * @param currentTime Current simulation time (seconds)
 * @param dt Timestep duration (seconds)
 * @return bool true if neuron fired this step, false otherwise
 * 
 * @note This is the core computational method for LIF neurons. It:
 * 1. Integrates membrane potential over time (Leaky Integrate)
 * 2. Adds synaptic inputs
 * 3. Checks for threshold crossing
 * 4. Generates spike if threshold crossed
 * 5. Resets membrane potential if spike occurred
 * 6. Updates adaptation variables
 * 
 * @see step() for the general simulation step (Phase 3+)
 * @warning May throw exceptions for invalid parameter values
 * 
 * Example usage:
 * @code
 * Neuron neuron(id);
 * bool fired = neuron.stepLIF(currentTime, timestep);
 * if (fired) {
 *     // Handle spike
 * }
 * @endcode
 */
bool stepLIF(Timestamp currentTime, TimestepDuration dt);
```

### 4. Parameter Documentation

#### Parameter Guidelines
- **Use consistent naming**: Match parameter names to implementation
- **Describe constraints**: Include ranges, types, and valid values
- **Explain purpose**: Help users understand what the parameter controls
- **Document side effects**: Any changes to internal state or behavior
- **Special cases**: Document unusual or edge case behavior

#### Example Parameter Documentation:
```cpp
/**
 * @param weight Synaptic weight (-1.0 to 1.0 typical range)
 * @note Synaptic weight determines the strength and sign (excitatory/inhibitory)
 *       of the connection. Positive weights are typically excitatory, negative
 *       weights are inhibitory. Weight magnitude affects postsynaptic potential amplitude.
 */
void setWeight(SynapticWeight weight);
```

### 5. Return Value Documentation

#### Return Value Guidelines
- **Describe meaning**: Explain what the return value represents
- **Document special cases**: Document when return values have special meaning
- **Document constraints**: Include valid ranges or possible values
- **Error conditions**: Document when methods return error codes or null pointers

#### Example Return Documentation:
```cpp
/**
 * @return float Average firing rate in Hertz (spikes per second)
 * 
 * @note This is a global measure of brain activity level and can be used
 *       to monitor overall system engagement. Range typically 0.0 to 100.0 Hz.
 * @warning Returns 0.0 for inactive brains or when measurement is not available.
 */
float getAverageFiringRate() const;
```

### 6. Struct and Enum Documentation

#### Struct Documentation
Structs should be documented similarly to classes, with emphasis on fields:

```cpp
/**
 * @brief Neuron state structure for efficient storage and access
 * 
 * This structure contains all state variables needed for integrate-and-fire
 * neuron dynamics, organized for efficient memory access and computation.
 * Each neuron maintains its complete state in this structure for easy
 * access and modification by the brain system.
 * 
 * @note This structure is designed for performance and cache efficiency.
 *       Field order follows memory access patterns for optimal performance.
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV)
    // ... other fields
};
```

#### Enum Documentation
Enums should document each value's meaning:

```cpp
/**
 * @brief Neuron type enumeration
 * 
 * @note Neuron type determines the neuron's behavior and role in the network.
 *       Affects synaptic transmission, plasticity eligibility, and computational role.
 */
enum class NeuronType : uint8_t {
    Excitatory,      ///< Neurons that increase postsynaptic membrane potential (typical)
    Inhibitory,      ///< Neurons that decrease postsynaptic membrane potential  
    Modulatory,      ///< Neurons that affect plasticity or gain control
    Sensory,         ///< Input neurons receiving external stimuli
    Motor,           ///< Output neurons generating motor responses
    Internal         ///< Internal processing neurons without direct external connection
};
```

### 7. Code Examples and Usage

#### Inline Examples
Include practical code examples where helpful:

```cpp
/**
 * Example usage of the Brain class:
 * 
 * @code{.cpp}
 * // Create brain with configuration
 * auto config = std::make_shared<Config>();
 * config->set("simulation.steps", 10000);
 * config->set("simulation.dt", 0.001);
 * config->set("brain.region.count", 4);
 * 
 * Brain brain(config);
 * if (brain.initialize()) {
 *     // Main simulation loop
 *     for (SimulationStep step = 0; step < 10000; ++step) {
 *         Timestamp currentTime = step * 0.001f;
 *         brain.step(step, currentTime);
 *     }
 *     
 *     // Save final state
 *     brain.save("brain_checkpoint.nlm");
 * } else {
 *     std::cerr << "Failed to initialize brain" << std::endl;
 * }
 * @endcode
 */
 ```

## Specialized Documentation Areas

### 1. Type System Documentation
The NLM has a comprehensive strongly-typed system that deserves special attention:

#### Type Alias Documentation
```cpp
/**
 * @brief Simulation step counter - represents discrete time steps in the simulation
 * @note Used for event-driven computation and spike timing
 */
using SimulationStep = uint64_t;

/**
 * @brief Strongly-typed identifiers for safety
 * 
 * Provides type-safe references to neural entities throughout the system,
 * preventing accidental type confusion and improving code safety.
 * 
 * @note All identifier types are designed to be used throughout the NLM
 *       architecture, ensuring consistency and preventing programming errors.
 */
struct NeuronId {
    uint64_t value;
    // ... constructor and operators
};
```

### 2. Architecture Documentation
Document the integration and flow of information between components:

#### Brain System Architecture
```cpp
/**
 * @brief Overview of the NLM brain system architecture
 * 
 * The NLM brain implements a comprehensive artificial developmental brain with
 * the following architectural components:
 * 
 * @image html nlm_architecture.png "NLM Brain Architecture"
 * 
 * Key architectural principles:
 * - **Modular design**: Each subsystem operates semi-independently
 * - **Event-driven computation**: Real-time neural processing
 * - **Closed-loop control**: Complete sensorimotor integration
 * - **Developmental trajectory**: Progressive maturation and specialization
 * - **Predictive processing**: Forward models for action planning
 * 
 * @see src/brain/Brain.hpp for implementation details
 * @see docs/ARCHITECTURE.md for comprehensive architectural documentation
 */
```

### 3. Development Phase Documentation
Document the current development status and future roadmap:

#### Phase 6 Integration Status
```cpp
/**
 * @brief Phase 6: Final Integration of all NLM systems
 * 
 * Phase 6 focuses on integrating all existing systems into a coherent artificial
 * brain. Previous phases built individual components; Phase 6 ensures they
 * work together as a unified system.
 * 
 * @note Status: COMPLETE - All Phase 6 integration tests pass successfully
 * 
 * Integration Achievements:
 * - Memory systems: All memory types integrated and functional
 * - Neuromodulation: Complete dopamine, curiosity, novelty integration
 * - Prediction systems: Forward models fully operational
 * - Cognition: Planning, attention, and concept formation integrated
 * - Development: Age-dependent plasticity and maturation working
 * - Performance: Optimized with memory pools and checkpointing
 * - Persistence: Save/load and replay systems functional
 * 
 * Phase 7 Extensions (Planned):
 * - Enhanced visualization and analysis tools
 * - Advanced learning algorithms
 * - Multi-agent and social learning capabilities
 * - Bio-inspired learning mechanisms
 * 
 * @see README.md for detailed Phase 6 documentation
 * @see docs/PHASE6_FINAL_AUDIT.md for integration audit results
 */
```

## Quality Assurance

### 1. Documentation Validation
- **Cross-reference validation**: Ensure all documented APIs actually exist
- **Consistency checking**: Verify parameter types and return types match implementation
- **Completeness review**: Ensure all public APIs are documented
- **Technical accuracy**: Validate technical content against source code

### 2. Review Process
- **Technical review**: Subject matter experts review accuracy
- **User review**: Representative users test documentation usability
- **Style review**: Ensure consistency with project standards
- **Integration testing**: Verify documentation examples work correctly

### 3. Maintenance Guidelines
- **Version control**: Document changes and maintain history
- **Incremental updates**: Add documentation as APIs evolve
- **Deprecation notices**: Clearly mark deprecated APIs
- **Breaking changes**: Document API changes with migration guidance

## Tools and Automation

### 1. Doxygen Configuration
```
PROJECT_NAME           = NLM
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
GENERATE_XML          = YES
GENERATE_SQLITE_SCHEMA = NO
GENERATE_MAN          = NO
GENERATE_CPPXML_SEC_UI = NO
JAVADOC_AUTOLINK_STYLESHEET_HREF = https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css
JAVADOC_AUTOLINK_STYLESHEET_HREF = https://cdn.jsdelivr.net/npm/font-awesome@5.0.2/css/font-awesome.min.css
ALIASES               = rst=".*"
```

### 2. Automated Documentation Generation
- **Script automation**: Use scripts to generate initial documentation structure
- **Continuous integration**: Include documentation checks in CI/CD pipeline
- **Quality metrics**: Track documentation completeness and quality metrics
- **Version tracking**: Link documentation to specific code versions

## Appendices

### A. Common Patterns
Document frequently used patterns and idioms:

```cpp
/**
 * @brief Example: Creating and using a brain configuration
 * 
 * This example shows how to create and configure a brain for simulation.
 * 
 * @code{.cpp}
 * // Create configuration
 * auto config = std::make_shared<Config>();
 * 
 * // Set basic parameters
 * config->set("simulation.duration", 1000);  // 1000 simulation steps
 * config->set("simulation.dt", 0.001);       // 1ms time step
 * config->set("brain.regions", 2);           // Create 2 brain regions
 * config->set("neuron.type", "Excitatory");   // Set neuron type
 * 
 * // Create brain
 * Brain brain(config);
 * 
 * // Initialize
 * if (!brain.initialize()) {
 *     std::cerr << "Failed to initialize brain" << std::endl;
 *     return 1;
 * }
 * 
 * // Main simulation loop
 * for (SimulationStep step = 0; step < 1000; ++step) {
 *     Timestamp time = step * 0.001f;
 *     brain.step(step, time);
 * }
 * 
 * // Save final state
 * brain.save("final_brain.nlm");
 * @endcode
 */
```

### B. Glossary
- **LIF**: Leaky Integrate-and-Fire (neuron model)
- **STDP**: Spike-Timing-Dependent Plasticity
- **pImpl**: Private Implementation pattern
- **PIMPL**: Private Implementation idiom
- **TODO**: Placeholder comments for incomplete implementations
- **PHASE 6**: Final integration phase of NLM development

## Conclusion

These documentation standards provide a comprehensive framework for creating high-quality, consistent documentation for the NLM project. By following these guidelines, developers can produce documentation that:

- Is accurate, complete, and technically sound
- Is consistent with NLM coding conventions and architecture
- Is useful for users at all experience levels
- Supports the development and maintenance of the NLM system

The standards balance technical detail with accessibility, ensuring that both expert developers and newcomers can effectively understand and work with the NLM public APIs.

## References

- NLM README.md - Project documentation and usage instructions
- docs/ARCHITECTURE.md - Detailed system architecture documentation
- docs/SCIENCE.md - Scientific background and motivation
- docs/ROADMAP.md - Development roadmap and future plans
- NLM coding style conventions - In-source code style guidelines

---
*This documentation was generated following NLM project standards for comprehensive API documentation.*
