# NLM Phase 4-6 Tutorial: Building Advanced Cognitive Systems

## Overview

This tutorial provides step-by-step guidance for implementing and using NLM's Phase 4-6 advanced cognitive systems. It covers:

1. **Phase 4 Cognitive Systems**: Prediction, working memory, episodic memory, attention, planning, self-model, social learning
2. **Phase 5 Performance Infrastructure**: Memory pools, event queues, SIMD optimization
3. **Phase 6 Enhanced Integration**: Motor system, real environment, comprehensive error handling
4. **Modern C++17 Implementation**: Event-driven architecture, parallel processing
5. **Enhanced Python Bindings**: Advanced API with Phase 4-6 capabilities

## Prerequisites

- **C++ Compiler**: GCC 10+ with C++20 support
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ with pybind11 and scikit-build-core
- **Build Tools**: Make or Ninja

## Part 1: Building the Enhanced NLM System

### CMake Configuration

Create a build directory and configure with Phase 4-6 features:

```bash
mkdir build
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_PHASE4=ON \
  -DENABLE_PHASE5=ON \
  -DENABLE_PHASE6=ON \
  -DENABLE_PYTHON=ON \
  -DENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
  -DENABLE_ERROR_HANDLING=ON \
  -DENABLE_TESTING=ON
```

### Build Options

```bash
# Enable specific subsystems
cmake .. -DENABLE_PHASE4=ON -DENABLE_PHASE5=ON -DENABLE_PHASE6=ON

# Disable if needed
cmake .. -DENABLE_PHASE6=OFF -DENABLE_PYTHON=OFF

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Part 2: Installing Enhanced Python Bindings

### Install Dependencies

```bash
pip install pybind11 scikit-build-core pytest numpy matplotlib \
  numpy-quaternion scipy networkx graphviz
```

### Build and Install

```bash
pip install .

# Or in development mode
pip install -e .
```

### Verify Installation

```python
import pynlm

# Check enabled features
print(f"Phase 4 Enabled: {pynlm.isPhase4Enabled()}")
print(f"Phase 5 Enabled: {pynlm.isPhase5Enabled()}")
print(f"Phase 6 Enabled: {pynlm.isPhase6Enabled()}")

# Check Python API version
print(f"Python API Version: {pynlm.__version__}")
```

## Part 3: Basic Phase 4-6 Example

### Simple Advanced Brain Example

```python
import pynlm

def basic_phase4_example():
    """Create and run a basic Phase 4 brain with memory and prediction."""
    
    # Create enhanced configuration
    config = pynlm.createDefaultConfig()
    
    # Enable Phase 4 cognitive systems
    config.set("brain.enableWorkingMemory", True)
    config.set("brain.enableEpisodicMemory", True)
    config.set("brain.enablePrediction", True)
    config.set("brain.enableCuriosity", True)
    config.set("brain.enableDevelopment", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Working memory capacity: {brain.getWorkingMemoryCapacity()}")
    print(f"Episodic memory capacity: {brain.getEpisodicMemoryCapacity()}")
    
    # Run simulation with advanced processing
    for step in range(100):
        brain.step(step)
        
        # Update Phase 4 cognitive systems
        brain.updateWorkingMemory()
        brain.updateEpisodicMemory()
        brain.updatePrediction()
        
        if step % 20 == 0:
            print(f"Step {step}: "
                  f"Working memory: {brain.getWorkingMemoryUsage()}, "
                  f"Prediction confidence: {brain.getPredictionConfidence():.3f}")
    
    print("✓ Basic Phase 4 example completed successfully!")

# Run the example
basic_phase4_example()
```

### Complete Agent Example with Phase 6 Features

```python
import pynlm
import time

def complete_advanced_agent_example():
    """Run a complete agent simulation with all Phase 4-6 features."""
    
    # 1. Create enhanced configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Configure Phase 4 cognitive systems
    config.set("brain.neuron_count", 1500)
    config.set("brain.enableWorkingMemory", True)
    config.set("brain.enableEpisodicMemory", True)
    config.set("brain.enablePrediction", True)
    config.set("brain.enableCuriosity", True)
    config.set("brain.enableDevelopment", True)
    
    # 3. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 4. Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # 5. Create enhanced environment
    world = pynlm.createRealEnvironment()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 6. Initialize agent with world
    agent.initialize(world)
    
    # 7. Enable all advanced subsystems
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enablePrediction(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableNeuromodulation(True)
    agent.enableStructuralPlasticity(True)
    
    # 8. Run simulation loop
    start_time = time.time()
    
    for step in range(500):
        # Update world with physics simulation
        world.update(0.1)
        
        # Get enhanced sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step with advanced processing
        brain.step(step)
        
        # Update Phase 4 cognitive systems
        brain.updateWorkingMemory()
        brain.updateEpisodicMemory()
        brain.updatePrediction()
        
        # Get action with advanced planning
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply advanced reward modulation
        reward = world.getEnvironmentalFeedback().getReward()
        prediction_error = agent.getPredictionError()
        agent.applyRewardModulation(reward, prediction_error)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Working memory usage: {brain.getWorkingMemoryUsage()}")
            print(f"  Episodic memory count: {brain.getEpisodicMemoryCount()}")
            print(f"  Prediction confidence: {agent.getPredictionConfidence():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    print(f"\n✓ Complete advanced agent example completed in {end_time - start_time:.2f} seconds!")

# Run the complete example
complete_advanced_agent_example()
```

## Part 4: Modern C++ Implementation Examples

### Event-Driven Processing Example

```cpp
#include "MotorSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>

void demonstrate_event_driven_processing() {
    std::cout << "=== Event-Driven Processing Demo ===\n\n";
    
    // Initialize logging
    nlm::Logger::initialize();
    nlm::Logger::setLevel(nlm::LogLevel::INFO);
    
    // Create configuration with Phase 6 features
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("enableWorkingMemory", true);
    config->set("enablePrediction", true);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return;
    }
    
    // Create motor system
    nlm::MotorSystem motorSystem;
    
    // Initialize motor system with dummy components
    // (In a real implementation, these would be properly initialized)
    nlm::Neuromodulator* neuromodulator = nullptr;
    nlm::PredictionSystem* predictionSystem = nullptr;
    nlm::SelfModel* selfModel = nullptr;
    
    motorSystem.initialize(brain.get(), neuromodulator, predictionSystem, selfModel);
    
    std，“iostream << "✓ Event-driven processing demonstration completed successfully!\n";
}
```

### Memory Pool Example

```cpp
#include "MemoryPool.hpp"
#include "../core/Types/Types.hpp"
#include <iostream>
#include <vector>

void demonstrate_memory_pools() {
    std::cout << "=== Memory Pool Demo ===\n\n";
    
    // Create memory pool for neurons
    nlm::MemoryPool<nlm::Neuron> neuronPool(1000);
    
    // Allocate neurons
    std::vector<nlm::Neuron*> neurons;
    for (int i = 0; i < 500; ++i) {
        neurons.push_back(neuronPool.allocate());
    }
    
    // Initialize neurons
    for (auto* neuron : neurons) {
        neuron->initialize(-70.0f, -50.0f, -75.0f, 20.0f, 2.0f);
    }
    
    // Use neurons (simulate some activity)
    for (int i = 0; i < 1000; ++i) {
        for (auto* neuron : neurons) {
            neuron->update(0.001f);
        }
    }
    
    // Deallocate neurons
    for (auto* neuron : neurons) {
        neuronPool.deallocate(neuron);
    }
    
    std::cout << "✓ Memory pool demonstration completed successfully!\n";
}
```

## Part 5: Error Handling and Recovery

### Enhanced Error Handling Example

```python
import pynlm

def demonstrate_error_handling():
    """Demonstrate enhanced error handling and recovery."""
    
    try:
        # Create brain with enhanced configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.enableWorkingMemory", True)
        config.set("brain.enableEpisodicMemory", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simulate an error condition
        if not brain.isValidConfiguration():
            raise pynlm.ConfigurationError(
                "Invalid brain configuration",
                1001,
                "Check all required parameters are set"
            )
        
        # Run simulation with error checking
        for step in range(100):
            if brain.shouldTerminate():
                print(f"Simulation terminated at step {step}")
                break
                
            if brain.isMemoryLimitExceeded():
                print("Memory limit exceeded, optimizing...")
                brain.optimizeMemoryUsage()
            
            brain.step(step)
            
            if brain.isNumericalInstable():
                print(f"Numerical instability detected at step {step}")
                brain.stabilizeNumerical()
        
        print("✓ Error handling demonstration completed successfully!")
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        print(f"Error code: {e.error_code}")
        print(f"Details: {e.details}")
        brain.recoverFromError()
        
    except pynlm.InitializationError as e:
        print(f"Initialization error: {e}")
        print(f"System state: {e.system_state}")
        brain.emergencyShutdown()
        
    except pynlm.RuntimeError as e:
        print(f"Runtime error: {e}")
        print(f"Step at error: {e.step_number}")
        print(f"Error context: {e.context}")
        brain.recoverFromError()
        
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
        brain.emergencyShutdown()
        raise

# Run error handling demonstration
demonstrate_error_handling()
```

### Error Recovery Example

```cpp
#include "Errors.hpp"
#include <iostream>

void demonstrate_error_recovery() {
    std::cout << "=== Error Recovery Demo ===\n\n";
    
    // Create error handler
    nlm::ErrorHandler errorHandler;
    
    // Simulate different error conditions
    try {
        // Configuration error
        nlm::ConfigurationError configError(
            "Invalid neuron count",
            2001,
            "Neuron count must be between 100 and 10000"
        );
        errorHandler.handleError(configError);
        
        // Runtime error
        nlm::RuntimeError runtimeError(
            "Memory allocation failed",
            0,
            "Insufficient memory for neural population"
        );
        errorHandler.handleError(runtimeError);
        
        // Recovery attempt
        if (errorHandler.attemptRecovery()) {
            std::cout << "Recovery successful!\n";
        } else {
            std::cout << "Recovery failed, attempting emergency shutdown...\n";
            errorHandler.emergencyShutdown();
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << "\n";
    }
    
    std::cout << "✓ Error recovery demonstration completed successfully!\n";
}
```

## Part 6: Testing and Validation

### Performance Testing Example

```bash
# Run performance benchmarks
./nlm_performance_test --suite=Phase6 --duration=60

# Compare different configurations
./nlm_performance_test --compare --modes=baseline,simd,parallel

# Profile memory usage
./nlm_performance_test --profile=memory --output=memory_report.txt
```

### Integration Testing Example

```python
import pynlm
import unittest

class TestPhase4System(unittest.TestCase):
    """Test Phase 4 cognitive systems."""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.enableWorkingMemory", True)
        self.config.set("brain.enableEpisodicMemory", True)
        self.config.set("brain.enablePrediction", True)
        
    def test_working_memory(self):
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Test working memory functionality
        brain.updateWorkingMemory()
        self.assertGreaterEqual(brain.getWorkingMemoryUsage(), 0.0)
        
    def test_episodic_memory(self):
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Test episodic memory functionality
        brain.updateEpisodicMemory()
        # Memory should be stored
        
    def test_prediction_system(self):
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Test prediction system
        brain.updatePrediction()
        self.assertGreaterEqual(brain.getPredictionConfidence(), 0.0)

# Run tests
if __name__ == '__main__':
    unittest.main()
```

## Conclusion

This tutorial provides a comprehensive introduction to NLM's Phase 4-6 advanced capabilities. It demonstrates:

1. **Building and running** enhanced NLM systems with Phase 4-6 features
2. **Using modern C++17** best practices in implementation
3. **Working with enhanced Python bindings** for advanced functionality
4. **Error handling and recovery** mechanisms
5. **Testing and validation** of Phase 4-6 systems

The NLM Phase 4-6 system represents a significant advancement in brain-inspired artificial intelligence, combining:

- **Advanced Cognitive Systems**: Prediction, memory, attention, planning, self-model, social learning
- **Performance Infrastructure**: Memory pools, event queues, SIMD optimization
- **Enhanced Integration**: Motor system, real environment, comprehensive error handling
- **Modern C++17 Implementation**: Event-driven architecture, parallel processing

This tutorial provides the foundation for building sophisticated AI systems that learn, adapt, and interact with their environment through experience, much like biological cognitive systems.