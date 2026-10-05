# NLM - QUICK_START: 5-Minute Getting Started Guide

## Introduction

Welcome to NLM (Neural Learning Machine)! This guide will help you get started with NLM in about 5 minutes. We'll explore the core concepts and run a simple simulation.

## What You'll Learn

- Basic brain structure and components
- How neural learning works
- Simple agent-environment interaction
- Running your first NLM simulation

## Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- About 30 seconds of your time

## Quick Start Code Example

Here's a complete working example that demonstrates a basic NLM brain in action:

```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <memory>
#include <iostream>

int main() {
    // Create a basic NLM brain
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    // Create agent brain interface
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 20, 20);  // 100x100 world, 20x20 vision
    world.setAgentStart(50.0f, 50.0f);  // Start in center
    
    // Run for 100 simulation steps
    for (int step = 0; step < 100; ++step) {
        // Agent receives sensory input
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Brain processes input and produces action
        auto action = agentBrain.decodeMotorCommand();
        
        // Apply action in world
        auto result = world.applyMotorCommand(action, step);
        
        // Print progress every 20 steps
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": Position (" 
                     << world.getAgentBody().x << ", " 
                     << world.getAgentBody().y << ")" << std::endl;
        }
    }
    
    std::cout << "\nSimulation completed!";
    std::cout << "\nFinal position: (" 
              << world.getAgentBody().x << ", " 
              << world.getAgentBody().y << ")" << std::endl;
    
    return 0;
}
```

## Key Concepts Explained

### 1. Brain (Neuronal Processing Core)
- Contains neurons, synapses, and memory systems
- Receives sensory input and produces motor output
- Learns through neural plasticity

### 2. AgentBrain (Interface Layer)
- Connects brain to world
- Handles sensory transduction and motor decoding
- Applies neuromodulation based on reward

### 3. SimpleWorld (Environment)
- 2D simulation space with objects
- Provides sensory input (vision, touch)
- Executes motor commands and provides feedback

## Running the Example

### Method 1: Build and Run
```bash
# Create build directory and compile
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the example
./nlm_example
```

### Method 2: Using Existing Demo
The project includes a Phase 6 integration demo:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase6_demo
```

## Common Use Cases

### 1. Learning Simple Navigation
The example above shows an agent learning to navigate a 2D world by receiving reward for exploring and avoiding hazards.

### 2. Studying Neural Plasticity
Configure the brain with different plasticity rules to study learning mechanisms.

### 3. Testing Prediction Systems
Enable prediction systems to see how anticipation affects behavior.

## Next Steps for Learning More

1. **Study the architecture**: Read [CORE_CONCEPTS.md](CORE_CONCEPTS.md) to understand how all components work together
2. **Try working examples**: Explore [examples/01_basic-brain.md](examples/01_basic-brain.md) through [examples/04_complete-agent.md](examples/04_complete-agent.md)
3. **Learn advanced usage**: Check [best-practices/01_performance.md](best-practices/01_performance.md) for optimization tips
4. **Debug issues**: See [troubleshooting/01_common-issues.md](troubleshooting/01_common-issues.md) for common problems and solutions

## What Makes NLM Special?

- **Real-time neural computation**: Uses biologically-inspired LIF neurons
- **Integrated cognition**: Combines memory, prediction, and attention
- **Developmental learning**: Improves with experience
- **Open-source research**: Transparent implementation for studying brain principles

## Quick Reference

| Component | Purpose | Key Methods |
|-----------|---------|-------------|
| `Brain` | Neural processing core | `step()`, `receiveSensoryInput()`, `produceAction()` |
| `AgentBrain` | World interface | `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()` |
| `SimpleWorld` | Environment | `update()`, `applyMotorCommand()`, `getSensoryPercept()` |

## Troubleshooting

**Common Issue**: Build errors
- Ensure you have a C++17 compiler
- Check CMake version is 3.10+
- Make sure SSE/AVX instructions are available

**Next**: If you get stuck, explore the documentation or ask for help!

## Conclusion

You've just run your first NLM simulation! The brain learned from its experiences, adapted its behavior, and successfully navigated its environment.

NLM is just getting started - the full system includes dozens of interconnected components working together to create an artificial developmental brain. Explore the documentation and examples to dive deeper into this fascinating field!

---

*This guide was generated to help you quickly understand NLM's core concepts and get running quickly. For more detailed information, continue with the other documentation files.*