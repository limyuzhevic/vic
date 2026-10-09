# NLM Documentation

## Overview

This directory contains comprehensive documentation for the NLM (熙然) project, an experimental artificial developmental brain. The documentation provides extensive coverage of examples, tutorials, API references, and guides to help users understand, use, and extend NLM.

## Documentation Structure

### Core Documentation Components

1. **Examples Directory** (`docs/examples/`)
   - Working, executable code examples demonstrating NLM functionality
   - Examples cover all major features and subsystems
   - Each example is a complete, standalone program

2. **Tutorials Directory** (`docs/tutorials/`)
   - Step-by-step guides for different user levels
   - Comprehensive tutorials on experimental design, configuration, and usage
   - Progressive tutorials from basic to advanced topics

3. **API Documentation** (`docs/api/`)
   - Complete API references for all NLM classes and methods
   - Python and C++ API documentation
   - Configuration system documentation
   - Experimental framework documentation

4. **Guides Directory** (`docs/guides/`)
   - User guides for different tasks and scenarios
   - Best practices and troubleshooting guides
   - Performance optimization guides
   - Extension and customization guides

5. **Reference Documentation** (`docs/reference/`)
   - Reference materials and quick references
   - Class and function references
   - Configuration parameter references
   - Example collections

## Directory Contents

### Examples (`docs/examples/`)

| Example | Description | Languages |
|---------|-------------|------------|
| `basic_usage.cpp` | Basic brain simulation and analysis | C++ |
| `sensory_motor_loop.cpp` | Complete sensory-motor integration loop | C++ |
| `memory_systems.cpp` | Working with memory systems | C++ |
| `prediction_system.cpp` | Temporal prediction and learning | C++ |
| `neuromodulation.cpp` | Neuromodulation mechanisms | C++ |
| `development.cpp` | Developmental processes and plasticity | C++ |
| `advanced_features.cpp` | Advanced features showcase | C++ |
| `python_bindings.cpp` | Python API usage examples | C++/Python |

### Tutorials (`docs/tutorials/`)

| Tutorial | Description | Difficulty |
|----------|-------------|------------|
| `getting_started.md` | First brain simulation and setup | Beginner |
| `experimental_design.md` | Designing and running experiments | Intermediate |
| `configuration.md` | Complete configuration guide | Intermediate |
| `memory_usage.md` | Working with memory systems | Intermediate |
| `prediction_learning.md` | Prediction system usage | Advanced |
| `neuromodulation.md` | Neuromodulation basics | Intermediate |
| `development.md` | Developmental processes | Advanced |
| `advanced_topology.md` | Custom neural network architectures | Advanced |
| `performance_optimization.md` | Performance optimization tips | Advanced |
| `troubleshooting.md` | Common issues and solutions | Beginner |

### API Documentation (`docs/api/`)

| API File | Description | Languages |
|----------|-------------|------------|
| `brain_api.md` | Brain class documentation | C++, Python |
| `memory_api.md` | Memory system documentation | C++, Python |
| `prediction_api.md` | Prediction system documentation | C++, Python |
| `neuromodulation_api.md` | Neuromodulation documentation | C++, Python |
| `development_api.md` | Development system documentation | C++, Python |
| `python_api.md` | Python bindings documentation | Python |
| `config_api.md` | Configuration system documentation | C++, Python |
| `experimental_api.md` | Experimental framework documentation | C++, Python |

### Guides (`docs/guides/`)

| Guide | Description | Audience |
|-------|-------------|----------|
| `user_manual.md` | Complete user manual | All users |
| `experimental_setup.md` | Setting up and running experiments | Researchers |
| `data_analysis.md` | Analyzing simulation results | Data analysts |
| `troubleshooting.md` | Troubleshooting guide | All users |
| `performance_tuning.md` | Performance optimization | Advanced users |
| `extensions.md` | Extending NLM | Developers |

### Reference Documentation (`docs/reference/`)

| Reference | Description | Type |
|-----------|-------------|------|
| `classes.md` | Class reference | Quick reference |
| `functions.md` | Function reference | Quick reference |
| `parameters.md` | Parameter reference | Reference |
| `configuration.md` | Configuration reference | Reference |
| `examples.md` | Code examples collection | Reference |

## Usage Instructions

### Getting Started with NLM Documentation

1. **Choose Your Entry Point**
   - **New Users**: Start with `docs/tutorials/getting_started.md` and `docs/examples/basic_usage.cpp`
   - **Researchers**: Begin with `docs/tutorials/experimental_design.md` and `docs/api/experimental_api.md`
   - **Developers**: Use `docs/api/config_api.md` and `docs/api/python_api.md`
   - **Advanced Users**: Explore `docs/tutorials/advanced_topology.md` and `docs/api/neuromodulation_api.md`

2. **Follow the Tutorial Structure**
   - Each tutorial follows a progressive learning path
   - Start with concepts, then move to implementation
   - Each tutorial includes practical examples and code snippets

3. **Use API References for Details**
   - Refer to API documentation for complete method signatures
   - Check parameter types and return values
   - Review examples in API documentation

### Working with Examples

1. **C++ Examples**
   - Compile examples using the NLM build system
   - Follow the example README instructions
   - Modify examples to understand specific features

2. **Python Examples**
   - Use Python bindings for interactive exploration
   - Run examples using Python installation
   - Leverage Python's interactive capabilities for experimentation

### Configuration and Setup

1. **Basic Setup**
   - Build NLM using CMake: `mkdir build && cd build && cmake .. && make -j4`
   - Install Python bindings: `pip install -e .`
   - Verify installation: `python -c "import pynlm; print(pynlm.__version__)"`

2. **Configuration**
   - Use `createDefaultConfig()` for basic configuration
   - Customize configuration using `set()` method
   - Load/save configurations using `loadFromFile()` and `saveToFile()`

### Experimental Workflows

1. **Designing Experiments**
   - Use `docs/tutorials/experimental_design.md` for guidance
   - Define research questions and hypotheses
   - Plan experimental parameters and procedures
   - Design data collection and analysis strategies

2. **Running Experiments**
   - Use existing demo programs for testing
   - Create custom experiment scripts
   - Monitor and analyze experimental results
   - Document findings and insights

## Code Examples

### Example 1: Basic Brain Simulation

This example demonstrates creating and running a basic brain simulation:

```cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 25 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    // Analyze results
    size_t total_spikes = brain->getTotalSpikeCount();
    float avg_firing = brain->getAverageFiringRate();
    std::cout << "Total spikes: " << total_spikes << std::endl;
    std::cout << "Average firing rate: " << avg_firing << " Hz" << std::endl;
    
    return 0;
}
```

### Example 2: Agent-World System

This example demonstrates a complete agent-world system:

```python
import pynlm

def run_agent_simulation():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = 1.0 if step % 50 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation()
```

### Example 3: Memory Systems

This example demonstrates working with memory systems:

```cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create brain with memory systems
    auto config = std::make_shared<Config>();
    config->set("memory.working.enable", true);
    config->set("memory.episodic.enable", true);
    config->set("memory.associative.enable", true);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Memory Systems Example" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Store experience
    Experience experience;
    experience.id = 123;
    experience.sensory_input = {0.5, 0.7, 0.3, 0.8, 0.4};
    experience.actions = {ActionType::MoveForward, ActionType::Interact};
    experience.rewards = {0.1, 1.0};
    experience.timestamp = 0;
    experience.context = "exploration";
    
    brain->storeEpisodicMemory(experience);
    
    // Attempt recall
    std::vector<float> search_pattern = {0.5, 0.7, 0.3, 0.8, 0.4};
    std::vector<float> recalled;
    bool success = brain->recallFromEpisodicMemory(search_pattern, recalled);
    
    if (success) {
        std::cout << "Successfully recalled experience " << experience.id << std::endl;
    }
    
    // Analyze memory statistics
    EpisodicMemoryStats stats = brain->getEpisodicMemoryStats();
    std::cout << "\nMemory Statistics:" << std::endl;
    std::cout << "  Episodes stored: " << stats.episodesStored << std::endl;
    std::cout << "  Recall accuracy: " << stats.recallAccuracy << std::endl;
    
    return 0;
}
```

## Contribution Guidelines

### Adding New Examples

1. **Create new example file** in `docs/examples/`
2. **Include detailed comments** explaining the purpose and usage
3. **Add build instructions** for compilation
4. **Include expected output** for verification
5. **Test example** to ensure it works correctly

### Contributing Documentation

1. **Maintain consistent formatting** with existing documentation
2. **Use appropriate markup** for code snippets and examples
3. **Include practical examples** for complex concepts
4. **Provide clear explanations** of terminology and concepts
5. **Update table of contents** and navigation links

### Improving Documentation

1. **Fix errors and typos** in existing documentation
2. **Add missing examples** for important features
3. **Improve code formatting** and readability
4. **Update outdated information**
5. **Add new sections** for emerging topics

## Documentation Tools

### Documentation Generation

This documentation is generated using:

- **Markdown** for documentation structure
- **C++ comments** for code examples
- **Python docstrings** for Python examples
- **Custom tools** for cross-referencing

### Example Integration

Each example is integrated with the NLM build system:

1. **Build system integration**: Examples can be compiled as standalone programs
2. **Test integration**: Examples are tested during build process
3. **Documentation integration**: Examples are included in documentation
4. **Continuous integration**: Examples are tested automatically

## Quality Assurance

### Documentation Standards

1. **Code formatting**: Use consistent indentation and style
2. **Documentation formatting**: Follow Markdown best practices
3. **Example quality**: Examples should be complete and runnable
4. **Cross-referencing**: Use consistent cross-references
5. **Update management**: Keep documentation up to date with code changes

### Review Process

1. **Technical accuracy**: Verify all examples and descriptions are accurate
2. **Code quality**: Ensure all code examples follow best practices
3. **Completeness**: Ensure all major features are documented
4. **Usability**: Ensure documentation is easy to understand and use
5. **Maintenance**: Regularly update documentation with new features

## Future Documentation

### Planned Documentation Improvements

1. **Architecture documentation**: Detailed system architecture diagrams
2. **Performance documentation**: Performance optimization and benchmarking
3. **Extension guide**: How to extend NLM for custom applications
4. **Migration guide**: Guide for migrating from older versions
5. **Community contributions**: Guidelines for contributing to documentation

### Upcoming Topics

- Advanced prediction system examples
- Neuromodulation tutorials
- Development process documentation
- Performance optimization examples
- Troubleshooting guides
- Best practices collections

## Conclusion

The NLM documentation provides comprehensive coverage of all major features and capabilities. Whether you're a beginner looking to get started, a researcher designing experiments, or a developer extending the system, this documentation provides the tools and information you need to succeed.

The documentation is continuously being improved and expanded. Contributions and feedback are welcome to help make the documentation more comprehensive and useful for the NLM community.

---

*For More Information:*
- **GitHub Repository**: Source code and documentation
- **Issue Tracker**: Report bugs and request features
- **Discussion Forums**: Community discussions and help
- **Documentation Wiki**: Community-maintained documentation

*Getting Started:*
1. **Read `docs/tutorials/getting_started.md`** for initial setup
2. **Try examples** in `docs/examples/` to understand basic functionality
3. **Explore API documentation** in `docs/api/` for detailed information
4. **Run demo programs** to see NLM in action

*Advanced Usage:*
1. **Study experimental designs** in `docs/tutorials/experimental_design.md`
2. **Configure advanced systems** using `docs/api/config_api.md`
3. **Integrate examples** into your own projects
4. **Contribute to documentation** to help the NLM community

This documentation is a living document that will continue to grow and evolve with the NLM project. Your contributions and feedback are essential for making this documentation comprehensive and useful for all NLM users!

---

*NLM Documentation Contributors:*
- **Documentation Team**: Authors and editors of documentation
- **Example Authors**: Creators of example programs
- **API Documentation**: API reference authors
- **Tutorial Authors**: Tutorial creators and maintainers
- **Community Contributors**: Users who contribute to documentation

*NLM Documentation Goals:*
- **Comprehensive Coverage**: Cover all major features and capabilities
- **Practical Examples**: Provide working, executable examples
- **Clear Explanations**: Explain complex concepts in understandable terms
- **Continuous Improvement**: Regularly update and improve documentation
- **Community Support**: Provide resources for all user levels

*NLM Documentation Future:*
- **Expanded Examples**: More comprehensive example coverage
- **Interactive Documentation**: Web-based documentation with live examples
- **Video Tutorials**: Visual documentation with video demonstrations
- **API Integration**: Better integration with development tools
- **Collaboration Tools**: Enhanced collaboration and feedback mechanisms

*NLM Documentation Mission:*nLM documentation serves as the primary resource for users, researchers, and developers working with the NLM system. It provides comprehensive, accurate, and accessible information to support the NLM community's goals of advancing neural system research.

---

*Ready to Start Your NLM Journey?*

1. **Begin with Basics**: Read `docs/tutorials/getting_started.md`
2. **Explore Examples**: Try `docs/examples/basic_usage.cpp`
3. **Study Tutorials**: Follow `docs/tutorials/experimental_design.md`
4. **Reference APIs**: Consult `docs/api/brain_api.md` and `docs/api/python_api.md`
5. **Contribute**: Share your findings and improvements with the community

The NLM documentation is your comprehensive resource for understanding, using, and extending the NLM neural simulation system. Whether you're just starting or are an experienced user, this documentation provides everything you need to succeed!

---

*Documentation Version*: 1.0
*Last Updated*: October 2026
*Status*: Production Ready

*This documentation is part of the NLM project and is maintained by the NLM documentation team.*

---