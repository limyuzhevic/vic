# Tutorial 1: Basic Setup

## Overview

This tutorial introduces you to the basics of NLM (Neural Learning Machine). It covers:

- Creating your first NLM brain
- Basic brain initialization
- Simple simulation execution
- Brain statistics and monitoring

This tutorial is designed for beginners who are new to NLM and want to get started quickly.

## Prerequisites

Before starting this tutorial, you should:

1. **Have NLM installed**: Install NLM using the instructions in the main README
2. **Understand basic Python concepts**: If you're using the Python bindings
3. **Have a C++ compiler** (if compiling from source)

## What You'll Learn

By the end of this tutorial, you will:

- Create a basic NLM brain with default configuration
- Initialize the brain for simulation
- Run a simple simulation loop
- Monitor brain statistics and performance
- Save and load brain states

## Setting Up Your Environment

### Option 1: Using Python (Recommended for beginners)

```python
# Install NLM Python bindings
pip install pynlm

# Verify installation
import pynlm
print(f"NLM version: {pynlm.__version__}")
```

### Option 2: Using C++ (For advanced users)

```bash
# Clone NLM repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Creating Your First Brain

### Python Example

```python
import pynlm

print("=== NLM Tutorial 1: Basic Setup ===")

# 1. Create a brain with default configuration
print("\n1. Creating brain...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

print(f"   Brain created successfully!")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")
print(f"   Total synapses: {brain.getTotalSynapseCount()}")

# 2. Initialize the brain
print("\n2. Initializing brain...")
brain.initialize()

print("   Brain initialization completed!")
print(f"   Initial firing neurons: {brain.getFiringNeuronCount()}")

# 3. Run a simple simulation
print("\n3. Running simple simulation...")
for step in range(10):
    brain.step(step)
    if step % 3 == 0:  # Print every 3 steps to avoid too much output
        print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("   ✓ Simple simulation completed!")

# 4. Monitor brain statistics
print("\n4. Monitoring brain statistics...")
print(f"   Final firing neurons: {brain.getFiringNeuronCount()}")
print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"   Developmental stage: {brain.getDevelopmentalStage()}")

# 5. Save brain state (optional)
print("\n5. Saving brain state...")
brain.save("tutorial_brain.bin")
print("   Brain saved to 'tutorial_brain.bin'")

# 6. Load brain state (optional)
print("\n6. Loading brain state...")
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load("tutorial_brain.bin")
print("   Brain loaded successfully!")
print(f"   Loaded brain has {new_brain.getTotalNeuronCount()} neurons")

print("\n=== Tutorial 1 Complete! ===")
```

### C++ Example

```cpp
#include <iostream>
#include "src/brain/Brain.hpp"

int main() {
    std::cout << "=== NLM Tutorial 1: Basic Setup ===" << std::endl;
    
    // 1. Create a brain with default configuration
    std::cout << "\n1. Creating brain..." << std::endl;
    nlm::Config config;
    auto brain = nlm::createBrain(config);
    
    std::cout << "   Brain created successfully!" << std::endl;
    std::cout << "   Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "   Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // 2. Initialize the brain
    std::cout << "\n2. Initializing brain..." << std::endl;
    brain->initialize();
    
    std::cout << "   Brain initialization completed!" << std::endl;
    std::cout << "   Initial firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    // 3. Run a simple simulation
    std::cout << "\n3. Running simple simulation..." << std::endl;
    for (int step = 0; step < 10; ++step) {
        brain->step(step);
        if (step % 3 == 0) {  // Print every 3 steps to avoid too much output
            std::cout << "   Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "   ✓ Simple simulation completed!" << std::endl;
    
    // 4. Monitor brain statistics
    std::cout << "\n4. Monitoring brain statistics..." << std::endl;
    std::cout << "   Final firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "   Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "   Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    // 5. Save brain state (optional)
    std::cout << "\n5. Saving brain state..." << std::endl;
    brain->save("tutorial_brain.bin");
    std::cout << "   Brain saved to 'tutorial_brain.bin'" << std::endl;
    
    // 6. Load brain state (optional)
    std::cout << "\n6. Loading brain state..." << std::endl;
    auto new_brain = nlm::createBrain(nlm::Config());
    new_brain->load("tutorial_brain.bin");
    std::cout << "   Brain loaded successfully!" << std::endl;
    std::cout << "   Loaded brain has " << new_brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    std::cout << "\n=== Tutorial 1 Complete! ===" << std::endl;
    
    return 0;
}
```

## Understanding the Code

### Brain Creation

```python
# In Python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# In C++
nlm::Config config;
auto brain = nlm::createBrain(config);
```

**What this does:**
- Creates a new NLM brain with default configuration
- The brain contains neurons and synapses ready for simulation
- The brain is not yet initialized (neurons are in resting state)

### Brain Initialization

```python
# In Python
brain.initialize()

# In C++
brain->initialize()
```

**What this does:**
- Sets up the brain for simulation
- Initializes neural parameters
- Prepares neurons for spiking activity
- Sets up internal brain systems

### Simulation Loop

```python
# In Python
for step in range(10):
    brain.step(step)

# In C++
for (int step = 0; step < 10; ++step) {
    brain->step(step);
}
```

**What this does:**
- Runs the brain simulation step by step
- Each step represents a time unit in the neural simulation
- Neurons process inputs and produce outputs
- Synaptic connections are activated and updated

### Brain Statistics

```python
# Get brain statistics
neuron_count = brain.getTotalNeuronCount()
synapse_count = brain.getTotalSynapseCount()
firing_neurons = brain.getFiringNeuronCount()
average_firing = brain.getAverageFiringRate()
```

**What these methods do:**
- `getTotalNeuronCount()`: Returns the total number of neurons in the brain
- `getTotalSynapseCount()`: Returns the total number of synapses
- `getFiringNeuronCount()`: Returns the number of currently firing neurons
- `getAverageFiringRate()`: Returns the average firing rate of all neurons

## What You've Accomplished

✅ Created your first NLM brain
✅ Initialized the brain for simulation
✅ Ran a simple simulation loop
✅ Monitored brain statistics
✅ Saved and loaded brain states

## Next Steps

Now that you've completed Tutorial 1, you're ready to move on to:

1. **Tutorial 2: Advanced Configuration** - Learn about schema-based configuration and factory presets
2. **Tutorial 3: Batch Processing** - Learn about efficient batch processing and performance optimization
3. **Tutorial 4: Performance Profiling** - Learn about profiling tools and optimization
4. **Tutorial 5: Advanced Python Features** - Learn advanced Python bindings and features
5. **Tutorial 6: Real-world Application** - Learn complete agent simulation and world interaction

## Additional Resources

- **Documentation**: docs/COMPREHENSIVE_DEVELOPMENT_GUIDE.md - Complete development guide
- **Configuration System**: docs/CONFIGURATION_SYSTEM.md - Type-safe configuration system
- **Python Bindings**: docs/PYTHON_BINDINGS.md - Complete Python API documentation
- **Performance**: docs/PERFORMANCE.md - Performance profiling and optimization
- **Best Practices**: docs/BEST_PRACTICES.md - Best practices for NLM development

## Common Issues and Solutions

### Issue 1: Brain initialization fails

**Problem**: Brain fails to initialize or throws an exception.

**Solution**:
```python
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Success!")
except Exception as e:
    print(f"Error: {e}")
    print("Try using default configuration")
```

### Issue 2: Simulation doesn't progress

**Problem**: Simulation runs but doesn't seem to progress.

**Solution**:
```python
# Check if brain is initialized
if brain is not None and brain.getTotalNeuronCount() > 0:
    print("Brain is properly initialized")
    
    # Run simulation with verbose output
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
else:
    print("Brain is not properly initialized")
```

### Issue 3: Memory issues

**Problem**: Simulation runs out of memory.

**Solution**:
```python
import psutil
import os

# Check available memory
process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Available memory: {memory_info.available / 1024 / 1024:.2f} MB")

# Create brain with smaller configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain
brain = pynlm.createBrain(config)
brain.initialize()
```

## Conclusion

Congratulations! You've successfully completed Tutorial 1 and created your first NLM brain. You've learned the fundamentals of:

1. **Brain Creation**: Creating a new NLM brain with default configuration
2. **Brain Initialization**: Initializing the brain for simulation
3. **Simulation Execution**: Running a simple simulation loop
4. **Statistics Monitoring**: Monitoring brain statistics and performance
5. **State Management**: Saving and loading brain states

This foundation will help you understand and work with NLM throughout your learning journey. The concepts and skills you've gained here will be valuable as you explore more advanced features and tutorials.

Remember:
- Start simple and build up
- Read the documentation thoroughly
- Experiment and explore
- Ask questions when you're stuck
- Share your discoveries and contributions

Happy simulating!
