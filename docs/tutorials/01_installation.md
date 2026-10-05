# NLM - Tutorials: Step-by-Step Learning Guides

## Introduction

This section contains step-by-step tutorials designed to help you learn and master NLM (Neural Learning Machine). Each tutorial builds upon previous knowledge, guiding you through the process of creating working NLM applications.

## Tutorials Directory Structure

```
tutorials/
├── 01_installation.md    # Installation and setup guide
├── 02_first-simulation.md # Your first NLM simulation
```

## Tutorial 01: Installation and Setup

## Overview

This tutorial guides you through setting up your development environment and creating your first NLM project. It covers all the essential steps from installing dependencies to running your first simulation.

### What You'll Learn

1. **Environment Setup**: Installing required dependencies
2. **Project Structure**: Understanding NLM project layout
3. **Configuration**: Setting up NLM parameters
4. **Basic Build**: Compiling and running NLM
5. **First Simulation**: Running your first neural learning experiment

### Prerequisites

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **Build Tools**: CMake 3.10 or higher
- **Memory**: At least 1GB RAM
- **Disk Space**: 500MB for build and source

## Step 1: System Preparation

### 1.1 Check Your System

```bash
# Check compiler version
mkdir test_build
cd test_build
echo "Testing compiler..."
# Check compiler in the next command
```

```bash
# Check compiler availability
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_aebf51b4-444a-4024-8e6a-d732cb992ef6
mkdir test_build && cd test_build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase6_demo
```

**Expected Output:**
```
-- The NLM Brain is ready for simulation!
Phase 6 Integration Test Results:
✅ Memory systems integrated
✅ Neuromodulation functional
✅ Prediction system active
✅ All cognitive systems connected
✅ Development system working
✅ Simulation complete!
```

### 1.2 Install Build Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libomp-dev
```

**macOS:**
```bash
xcode-select --install
brew install cmake
```

**Windows (with Chocolatey):**
```bash
choco install visualstudio2017buildtools
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
```

**Windows (Manual):**
1. Download from https://cmake.org/download/
2. Install with default settings
3. Add CMake to PATH

## Step 2: Project Setup

### 2.1 Create Project Directory

```bash
# Create your project directory
mkdir -p ~/nlm_projects/my_first_nlm
cd ~/nlm_projects/my_first_nlm

# Initialize git repository (optional but recommended)
git init
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

### 2.2 Copy NLM Source Code

**Option A: Download from GitHub**
```bash
# Clone NLM repository (replace with actual URL)
git clone https://github.com/your-username/nlm.git nlm-source
cp -r nlm-source/* .
rmdir nlm-source
```

**Option B: Copy from Existing Workspace**
```bash
# Copy from current workspace
cp -r /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_aebf51b4-444a-4024-8e6a-d732cb992ef6/* .
```

### 2.3 Project Structure

After setup, your project should look like this:

```
my_first_nlm/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project documentation
├── src/
│   ├── brain/                 # Neural components
│   ├── agent/                 # Agent interface
│   ├── world/                 # Environment simulation
│   └── ...                    # Other modules
├── docs/                      # Documentation (created in our case)
├── examples/                  # Example code
└── build/                     # Build directory (created during build)
```

## Step 3: Configuration

### 3.1 Configure Basic Settings

Edit `CMakeLists.txt` to set your configuration:

```cmake
# Project configuration
cmake_minimum_required(VERSION 3.10)
project(NLM-Learning-Example LANGUAGES CXX)

# NLM configuration
nlm_configure(
    SIMULATION_TIMESTEP 0.001
    MAX_NEURON_COUNT 1000
    LEARNING_RATE 0.01
    EXPLORATION_RATE 0.3
    ENABLE_REWARD_MODULATION ON
    ENABLE_CURIOUSITY ON
    ENABLE_DEVELOPMENT ON
)
```

### 3.2 Create Configuration File

Create `nlm_config.json` for runtime configuration:

```json
{
    "simulation": {
        "timestep": 0.001,
        "maxSteps": 1000
    },
    "brain": {
        "neuronCount": 1000,
        "learningRate": 0.01,
        "plasticityType": "STDP"
    },
    "agent": {
        "explorationRate": 0.3,
        "curiosityEnabled": true,
        "developmentEnabled": true
    },
    "world": {
        "width": 50.0,
        "height": 50.0,
        "visionWidth": 15,
        "visionHeight": 15
    }
}
```

## Step 4: Build and Compile

### 4.1 Create Build Directory

```bash
# Create build directory
mkdir build
cd build
```

### 4.2 Configure Build

```bash
# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_TESTS=ON \
         -DNLM_ENABLE_PERF_MEASUREMENTS=OFF
```

### 4.3 Build the Project

```bash
# Build with all available cores
make -j$(nproc)

# Or for Windows
# msbuild ALL_BUILD.vcxproj
```

### 4.4 Verify Build

```bash
# Check if build succeeded
ls -la

# Run basic tests (if enabled)
# ./nlm_test_runner
```

**Expected Build Output:**
```
Scanning dependencies... 100% Declining 0 warnings.
Generating 0 targets... done.
Building C++ object CMakeFiles/nlm_phase6_demo.dir/src/main.cpp.o
[ 50%] Building C++ object CMakeFiles/nlm_phase6_demo.dir/src/brain/Brain.cpp.o
[100%] config/Config.cpp.o
... [more compilation messages] ...
[100%] Linking C++ executable nlm_phase6_demo
```

## Step 5: Run First Simulation

### 5.1 Basic Phase 6 Demo

```bash
# Run the integration test
./nlm_phase6_demo
```

**Expected Output:**
```
NLM Phase 6 Demo - Integration Test
==================================

Initializing brain systems...
- Neural core: ✅ Operational
- Memory systems: ✅ Integrated
- Neuromodulation: ✅ Functional
- Prediction systems: ✅ Active
- Cognition systems: ✅ Connected
- Development system: ✅ Working

Running simulation for 100 steps...
Step 1: Neural activity detected
Step 2: Sensory processing initiated
Step 3: Reward prediction active
Step 50: Curiosity exploration triggered
Step 100: Integration complete

=== Integration Test Results ===
✅ All systems working together
✅ Neural plasticity active
✅ Learning mechanisms functional
✅ Memory consolidation successful
✅ Agent behavior adaptive

Phase 6 Demo completed successfully!
```

### 5.2 Run Custom Example

Create your first custom example:

```bash
# Create example source
cat > examples/first_example.cpp << 'EOF'
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== First NLM Example ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(brain);
    nlm::SimpleWorld world;
    
    world.configure(100.0f, 100.0f, 10, 10);
    agentBrain.initialize(world);
    
    for (int step = 0; step < 50; ++step) {
        agentBrain.processSensoryInput(world.getSensoryPercept());
        auto action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action, step * 0.001);
    }
    
    std::cout << "First simulation complete!" << std::endl;
    return 0;
}
EOF

# Compile and run
mkdir build_examples
cd build_examples
cmake ../examples
make -j4
./first_example
```

## Step 6: Documentation and Examples

### 6.1 Explore Examples Directory

```bash
# Browse examples
ls -la examples/

# Read example documentation
cat examples/01_basic-brain.md
```

### 6.2 Create Your First Documentation

Create a README for your project:

```bash
cat > README.md << 'EOF'
# My NLM Learning Project

## Overview
This project demonstrates NLM (Neural Learning Machine) capabilities through custom examples and simulations.

## Getting Started
See [Tutorials/01_installation.md](tutorials/01_installation.md) for setup instructions.

## Examples
- [examples/01_basic-brain.md](examples/01_basic-brain.md) - Basic neural processing
- [examples/02_brain-in-world.md](examples/02_brain-in-world.md) - Agent-environment interaction

## Running
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase6_demo
```

## License
MIT License
EOF
```

## Step 7: Advanced Configuration

### 7.1 Optimize Performance

Edit `CMakeLists.txt` for optimization:

```cmake
# Performance optimizations
nlm_optimize(
    ENABLE_VECTORIZATION ON
    ENABLE_PARALLEL_PROCESSING ON
    ENABLE_MEMORY_POOLS ON
    SIMULATION_MODE FAST
)
```

### 7.2 Enable Debug Features

```bash
# Build with debug information
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_DEBUG=ON
make -j4
```

### 7.3 Run with Custom Configuration

```bash
# Run with specific configuration
cat > custom_config.json << 'EOF'
{
    "simulation": {
        "timestep": 0.01,
        "maxSteps": 500
    },
    "brain": {
        "learningRate": 0.05,
        "neuronCount": 500
    }
}
EOF

# Use custom config
./nlm_phase6_demo --config custom_config.json
```

## Step 8: Extend and Customize

### 8.1 Create Custom Examples

```bash
# Create your custom example
cat > examples/my_custom_example.cpp << 'EOF'
// Your custom NLM implementation
EOF

# Add to build
# Update CMakeLists.txt to include new example
```

### 8.2 Modify Documentation

Create your own documentation:

```bash
cat > docs/my_tutorials.md << 'EOF'
# My Custom Tutorials

## Topic 1: Basic Concepts
## Topic 2: Advanced Usage
## Topic 3: Troubleshooting
EOF
```

## Step 9: Testing and Validation

### 9.1 Run Unit Tests

```bash
# Run if tests are enabled
./nlm_unit_tests
```

### 9.2 Performance Testing

```bash
# Profile performance
./nlm_phase6_demo --profile
```

### 9.3 Integration Testing

```bash
# Test all systems together
./nlm_phase6_demo --integration-test
```

## Step 10: Learning Progress

### 10.1 Track Your Progress

Create a learning log:

```bash
cat > learning_log.md << 'EOF'
# NLM Learning Progress

## Week 1: Setup
- [x] Installed NLM dependencies
- [x] Created project structure
- [x] Built first example

## Week 2: Learning
- [ ] Completed examples/01_basic-brain.md
- [ ] Completed examples/02_brain-in-world.md
- [ ] Created custom example

## Week 3: Integration
- [ ] Integrated memory systems
- [ ] Added prediction capabilities
- [ ] Implemented neuromodulation
EOF
```

## Troubleshooting Common Issues

### Issue 1: Build Errors
```bash
# Check CMake version
cmake --version

# Ensure you have C++17 support
g++ --version

# Try clean rebuild
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Issue 2: Runtime Errors
```bash
# Check if binary exists
ls -la nlm_phase6_demo

# Run with strace (Linux)
strace ./nlm_phase6_demo

# Check system resources
free -h
```

### Issue 3: Configuration Problems
```bash
# Verify configuration file
cat nlm_config.json | python -m json.tool

# Check for syntax errors
python -m py_compile nlm_config.json
```

## Resources and Next Steps

### Study Materials
- [NLM Documentation](docs/README.md) - Complete API reference
- [Examples Directory](examples/) - Working code examples
- [Core Concepts](CORE_CONCEPTS.md) - Architecture explanation

### Community Resources
- [NLM GitHub Repository](https://github.com/your-username/nlm) - Source code
- [NLM Documentation Wiki](https://wiki.example.com/nlm) - Community wiki
- [NLM Discussion Forum](https://forum.example.com/nlm) - User discussions

### Advanced Topics
- [Neural Networks](docs/advanced/neural-networks.md) - Deep learning integration
- [Memory Systems](docs/advanced/memory-systems.md) - Advanced memory techniques
- [Prediction Systems](docs/advanced/prediction.md) - Predictive coding

## Summary

Congratulations! You have successfully completed the NLM installation and setup tutorial. You now have:

1. ✅ **Working Environment**: NLM project built and ready to use
2. ✅ **Documentation**: Complete documentation structure
3. ✅ **Examples**: Working code examples
4. ✅ **Configuration**: Custom configuration files
5. ✅ **Build System**: Optimized build configuration

### Next Steps

1. **Study the Examples**: Read through `examples/01_basic-brain.md` and `examples/02_brain-in-world.md`
2. **Create Your Own Example**: Build a custom NLM application
3. **Explore Advanced Topics**: Study memory systems, prediction, and neuromodulation
4. **Contribute**: Add to the documentation or examples
5. **Share**: Help others by sharing your NLM projects

## Key Commands Summary

| Command | Description |
|---------|-------------|
| `mkdir build && cd build && cmake .. && make -j4` | Build NLM from source |
| `./nlm_phase6_demo` | Run integration test |
| `ls examples/` | Browse examples directory |
| `cat docs/README.md` | View project documentation |

Remember: The best way to learn NLM is through hands-on practice. Start with the basic examples and gradually move to more complex applications. Good luck with your NLM learning journey!
EOF
```

## Expected Output Structure

After completing this tutorial, your project directory should look like this:

```
my_first_nlm/
├── CMakeLists.txt
├── README.md
├── tutorials/
│   ├── 01_installation.md
│   └── 02_first-simulation.md
├── examples/
│   ├── 01_basic-brain.md
│   ├── 02_brain-in-world.md
│   ├── 03_learning-agent.md
│   └── 04_complete-agent.md
├── docs/
│   ├── README.md
│   ├── QUICK_START.md
│   ├── CORE_CONCEPTS.md
│   ├── API_REFERENCE.md
│   ├── examples/          (symlink to examples/)
│   ├── tutorials/         (symlink to tutorials/)
│   ├── best-practices/    (symlink to best-practices/)
│   └── troubleshooting/  (symlink to troubleshooting/)
├── best-practices/
│   ├── 01_performance.md
│   └── 02_configuration.md
└── troubleshooting/
    └── 01_common-issues.md
```

## Key Takeaways

### 1. Setup is Foundational
- Proper project structure enables efficient development
- Configuration management ensures reproducibility
- Build automation speeds up testing and deployment

### 2. Documentation is Essential
- Clear documentation helps you and others understand the project
- Examples provide practical guidance
- Troubleshooting guides help resolve issues quickly

### 3. Learning is Incremental
- Start with basic examples and build up complexity
- Practice regularly to reinforce concepts
- Document your learning progress

### 4. Community Matters
- Share your work with others
- Learn from existing examples and documentation
- Contribute back to the community

## Next Tutorial

Move on to **Tutorial 02: Your First NLM Simulation** to create your first working neural learning example using the concepts and setup from this tutorial.

This tutorial provides you with a solid foundation for working with NLM. Take your time to understand each step, experiment with different configurations, and don't hesitate to ask for help when needed. Happy learning with NLM! 🚀