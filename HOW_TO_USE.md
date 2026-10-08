# NLM - HOW TO USE

This document provides comprehensive guidance on using the NLM (Neural Learning Machine) system,
including the Python bindings, C++ API, and various usage patterns.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Python Bindings Overview](#python-bindings-overview)
3. [C++ API Reference](#c-api-reference)
4. [Advanced Usage Patterns](#advanced-usage-patterns)
5. [Troubleshooting](#troubleshooting)
6. [Performance Optimization](#performance-optimization)
7. [Integration Examples](#integration-examples)
8. [API Reference](#api-reference)

## Quick Start

### Prerequisites

- C++ compiler with C++20 support
- Python 3.8+
- CMake 3.16+

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Build the system**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Install Python bindings**
   ```bash
   # Option 1: System installation
   pip install pynlm
   
   # Option 2: Local development
   cd ..
   pip install -e .
   ```

4. **Run examples**
   ```bash
   cd examples
   python3 basic_simulation.py
   python3 advanced_usage.py
   ```

## Python Bindings Overview

The Python bindings provide a clean, Pythonic interface to the C++ neural simulation framework.

### Key Features

- **Type Safety**: Strong typing with validation
- **Error Handling**: Custom exceptions for different error types
- **Convenience Methods**: Factory functions and Pythonic interfaces
- **Type Conversions**: Automatic conversions between Python and NLM types
- **Documentation**: Comprehensive docstrings and examples

### Architecture

```
Python Layer
  ├── High-level convenience functions
  ├── Type conversion utilities
  ├── Error handling wrappers
  ├── Factory functions
C++ Layer (Pybind11)
  ├── Direct bindings to C++ classes
  ├── Enhanced constructors and methods
  ├── Template helpers for boilerplate reduction
Core C++ Library
  ├── Neural computation engine
  ├── Memory management
  ├── Configuration system
```

## C++ API Reference

### Core Classes

#### Brain
```cpp
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    bool initialize();
    void step(SimulationStep step, Timestamp time);
    void reset();
    // ... many more methods
    
    // Getters
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    double getAverageFiringRate() const;
    // ...
};
```

#### AgentBrain
```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    void initialize(const SimpleWorld& world);
    MotorCommand decodeMotorCommand();
    void processSensoryInput(const SensoryPercept& percept);
    void applyRewardModulation(float reward, float predictedReward);
    // ...
};
```

#### Config
```cpp
class Config {
public:
    Config();
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    void set(const std::string& key, const T& value, ConfigSource source);
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    bool has(const std::string& key) const;
    std::string summary() const;
    // ...
};
```

### Usage Examples

#### Basic C++ Usage
```cpp
#include "pynlm/pynlm.h"
#include <iostream>

int main() {
    // Create brain with configuration
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Log results
    brain->logStatus();
    
    return 0;
}
```

#### Advanced Configuration
```cpp
#include "pynlm/pynlm.h"

void setupAdvancedConfig(nlm::Config& config) {
    // Network parameters
    config.set("neuron_count", 5000, nlm::ConfigSource::User);
    config.set("region_count", 8, nlm::ConfigSource::User);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::User);
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.025f, nlm::ConfigSource::User);
    config.set("stdp_ltd_weight", 0.018f, nlm::ConfigSource::User);
    config.set("stdp_tau", 25.0f, nlm::ConfigSource::User);
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.0003f, nlm::ConfigSource::User);
    config.set("pruning_rate", 0.00003f, nlm::ConfigSource::User);
    
    // Development
    config.set("developmental_stage_duration", 300.0, nlm::ConfigSource::User);
    
    // Neuromodulation
    config.set("dopamine_sensitivity", 1.2f, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.3f, nlm::ConfigSource::User);
}
```

## Advanced Usage Patterns

### 1. Learning with Rewards

```python
import pynlm as nlm
import numpy as np

class LearningAgent:
    def __init__(self):
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.agent_brain = nlm.AgentBrain(self.brain)
        self.world = nlm.createSimpleWorld()
        
        # Setup world
        self.world.configure(100, 100, 32, 32)
        self.brain.initialize(self.world)
        self.agent_brain.initialize(self.world)
        
        # Enable learning
        self.agent_brain.enableRewardModulation(True)
        self.agent_brain.enableCuriosity(True)
    
    def episode(self, max_steps=100):
        rewards = []
        
        for step in range(max_steps):
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent_brain.processSensoryInput(percept)
            
            # Select action
            action = self.agent_brain.decodeMotorCommand()
            
            # Apply action
            if action != nlm.MotorCommand.Wait:
                result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                reward = result.reward
            else:
                reward = -0.1  # Small penalty for inaction
            
            rewards.append(reward)
            
            # Apply reward modulation (learning)
            self.agent_brain.applyRewardModulation(reward, self.agent_brain.getPredictionError())
            
            # Update development
            self.agent_brain.updateDevelopment(0.01)
        
        return np.mean(rewards)
```

### 2. Multi-Agent Simulation

```cpp
#include "pynlm/pynlm.h"
#include <vector>
#include <thread>
#include <mutex>

class MultiAgentSystem {
private:
    std::vector<std::shared_ptr<nlm::Brain>> brains_;
    std::vector<std::shared_ptr<nlm::AgentBrain>> agents_;
    std::vector<std::shared_ptr<nlm::SimpleWorld>> worlds_;
    mutable std::mutex mutex_;
    
public:
    MultiAgentSystem(size_t num_agents, const nlm::Config& config) {
        for (size_t i = 0; i < num_agents; ++i) {
            auto brain = nlm::createBrain(std::make_shared<nlm::Config>(config));
            brain->initialize();
            
            auto agent = std::make_shared<nlm::AgentBrain>(brain);
            auto world = std::make_shared<nlm::SimpleWorld>();
            world->configure(50, 50, 16, 16);
            world->setAgentStart(i * 5, i * 5);
            
            agents_.push_back(agent);
            brains_.push_back(brain);
            worlds_.push_back(world);
        }
    }
    
    void stepAll(size_t step) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            // Each agent processes its own world
            auto percept = worlds_[i]->getSensoryPercept();
            agents_[i]->processSensoryInput(percept);
            
            // Agents can interact with each other
            for (size_t j = 0; j < agents_.size(); ++j) {
                if (i != j) {
                    // Inter-agent learning
                    float reward = worlds_[i]->getAgentBody().health > worlds_[j]->getAgentBody().health ? 1.0f : -0.5f;
                    agents_[i]->applyRewardModulation(reward, 0.0f);
                }
            }
            
            // Execute action
            auto action = agents_[i]->decodeMotorCommand();
            if (action != nlm::MotorCommand.Wait) {
                worlds_[i]->applyMotorCommand(action, step * 0.001);
            }
        }
    }
    
    double getSystemPerformance() const {
        std::lock_guard<std::mutex> lock(mutex_);
        double total_spikes = 0.0;
        double total_neurons = 0.0;
        
        for (const auto& brain : brains_) {
            total_spikes += brain->getTotalSpikeCount();
            total_neurons += brain->getTotalNeuronCount();
        }
        
        return total_neurons > 0 ? total_spikes / total_neurons : 0.0;
    }
};
```

### 3. Visualization Integration

```python
import pynlm as nlm
import matplotlib.pyplot as plt
import numpy as np

class Visualizer:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.spike_history = []
        self.activity_history = []
    
    def record_step(self, step):
        """Record data for visualization."""
        self.spike_history.append(self.brain.getTotalSpikeCount())
        self.activity_history.append(self.brain.getActiveNeuronCount())
    
    def plot_activity(self):
        """Plot neural activity over time."""
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
        
        # Spike count over time
        ax1.plot(self.spike_history)
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Spike Count')
        ax1.set_title('Neural Activity Over Time')
        ax1.grid(True)
        
        # Active neurons over time
        ax2.plot(self.activity_history)
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Active Neurons')
        ax2.set_title('Active Neurons Over Time')
        ax2.grid(True)
        
        plt.tight_layout()
        plt.show()
    
    def plot_spatial_activity(self):
        """Plot spatial distribution of neural activity."""
        if self.brain.getRegionCount() > 0:
            region = self.brain.getRegion(nlm.RegionId(0))
            if region:
                auto neurons = region->getAllNeurons();
                if (!neurons.empty()) {
                    // Create spatial activity map
                    // (Implementation depends on region layout)
                }
            }
        
        # Create simple 2D visualization
        world_width = self.world.getWidth();
        world_height = self.world.getHeight();
        
        fig, ax = plt.subplots(figsize=(10, 8))
        ax.set_xlim(0, world_width);
        ax.set_ylim(0, world_height);
        ax.set_xlabel('X Position');
        ax.set_ylabel('Y Position');
        ax.set_title('Agent Movement and Neural Activity');
        ax.grid(True)
        
        # Plot agent trajectory (if available)
        # Plot neural hotspots
        plt.show()
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Python module not found"

**Problem**: The `pynlm` Python module cannot be imported.

**Solutions**:
```bash
# Check if module is installed
python3 -c "import pynlm; print(pynlm.__file__)"

# Reinstall if needed
pip install --force-reinstall pynlm

# Or install from source
pip install -e .
```

#### Issue 2: Build fails with Pybind11

**Problem**: CMake cannot find Pybind11 or compilation errors.

**Solutions**:
```bash
# Install Pybind11 development headers
apt-get install pybind11-dev  # Ubuntu/Debian
brew install pybind11      # macOS

# Or let CMake find it automatically
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/pybind11
```

#### Issue 3: Memory usage too high

**Problem**: The simulation consumes excessive memory.

**Solutions**:
```python
# Reduce configuration parameters
config.set("neuron_count", 100)  # Instead of 5000
config.set("region_count", 2)     # Instead of 8

# Use sparse connectivity
config.set("connection_probability", 0.05)  # Lower density
```

#### Issue 4: Type conversion errors

**Problem**: Issues converting between Python and NLM types.

**Solutions**:
```python
# Use explicit conversion functions
float_list = [0.1, 0.5, 0.9]
double_list = nlm.to_double_vector(float_list)

# Or use NumPy arrays (if available)
import numpy as np
numpy_array = np.array(float_list)
```

#### Issue 5: Error handling not working

**Problem**: Custom exceptions are not being raised properly.

**Solutions**:
```python
# Catch specific exceptions
try:
    # Your code here
    pass
except nlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Performance Optimization

### Python Optimization

#### 1. Batch Operations
```python
# Process multiple steps efficiently
steps = list(range(0, 1000, 10))  # Process every 10 steps

for step in steps:
    brain.step(step, step * 0.001)
    # Batch record results
```

#### 2. Vectorized Sensory Processing
```python
import numpy as np

# Create multiple vision inputs at once
def create_vision_batch(width, height, channels=1):
    """Create a batch of vision inputs."""
    batch_size = 10
    batch = []
    
    for i in range(batch_size):
        # Create different patterns
        pattern = np.random.rand(height, width, channels)
        vision = nlm.Vision(width, height, channels)
        vision.setData(pattern.flatten())
        batch.append(vision)
    
    return batch
```

#### 3. Memory Management
```python
import gc
import pynlm as nlm

class EfficientSimulation:
    def __init__(self, config):
        self.brain = nlm.createBrain(config)
        self.agent_brain = nlm.AgentBrain(self.brain)
    
    def run_episode(self, steps):
        """Run an episode with proper memory management."""
        for step in range(steps):
            # Process input
            self.agent_brain.processSensoryInput(self.percept)
            
            # Get output
            action = self.agent_brain.decodeMotorCommand()
            
            # Clear old references
            if step % 100 == 0:
                gc.collect()
                
                # Reset brain periodically to save memory
                self.brain.reset()
                self.brain.initialize()
    
    def __del__(self):
        """Cleanup resources."""
        del self.brain
        del self.agent_brain
        gc.collect()
```

### C++ Optimization

#### 1. SIMD Vectorization
```cpp
#include <nlm_performance/SIMDVectorization.hpp>

// Use SIMD for vector operations
void processBatch(const std::vector<float>& inputs,
                 std::vector<float>& outputs,
                 nlm::Brain* brain) {
    const size_t batch_size = inputs.size();
    const size_t vector_width = nlm::SIMDVectorization::getOptimalVectorWidth();
    
    size_t i = 0;
    for (; i + vector_width <= batch_size; i += vector_width) {
        // Process with SIMD
        auto input_vec = nlm::SIMDVectorization::load(&inputs[i]);
        auto output_vec = nlm::SIMDVectorization::process(input_vec, brain);
        nlm::SIMDVectorization::store(&outputs[i], output_vec);
    }
    
    // Process remaining elements
    for (; i < batch_size; ++i) {
        outputs[i] = inputs[i] * 0.5f;  // Simple processing
    }
}
```

#### 2. Memory Pools
```cpp
#include <nlm_performance/MemoryPool.hpp>

class EfficientBrain {
private:
    nlm::Performance::MemoryPool neuron_pool_;
    nlm::Performance::MemoryPool synapse_pool_;
    
public:
    void* allocateNeuronMemory(size_t size) {
        return neuron_pool_.allocate(size);
    }
    
    void* allocateSynapseMemory(size_t size) {
        return synapse_pool_.allocate(size);
    }
    
    void deallocateNeuronMemory(void* ptr, size_t size) {
        neuron_pool_.deallocate(ptr, size);
    }
    
    void deallocateSynapseMemory(void* ptr, size_t size) {
        synapse_pool_.deallocate(ptr, size);
    }
};
```

## Integration Examples

### 1. Robot Control Integration

```python
import pynlm as nlm
import rospy
from geometry_msgs.msg import Twist

class RobotController:
    def __init__(self):
        # Initialize NLM brain
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.agent_brain = nlm.AgentBrain(self.brain)
        self.world = nlm.createSimpleWorld()
        
        # Setup world
        self.world.configure(200, 200, 64, 64)
        self.brain.initialize(self.world)
        self.agent_brain.initialize(self.world)
        
        # ROS setup
        self.cmd_vel_pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
        self.odom_sub = rospy.Subscriber('/odom', Odometry, self.odom_callback)
    
    def odom_callback(self, msg):
        """Update robot position in the world."""
        # Update agent body (simplified)
        body = self.world.getAgentBody()
        body.x = msg.pose.pose.position.x
        body.y = msg.pose.pose.position.y
        # Update orientation, etc.
    
    def run_control_loop(self):
        """Main control loop."""
        rate = rospy.Rate(10)  # 10 Hz
        
        while not rospy.is_shutdown():
            # Update world
            self.world.update(rate.expectedCycleTime().toSec())
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent_brain.processSensoryInput(percept)
            
            # Get control command
            command = self.agent_brain.decodeMotorCommand()
            
            # Convert NLM command to ROS command
            twist = self.command_to_twist(command)
            self.cmd_vel_pub.publish(twist)
            
            rate.sleep()
    
    def command_to_twist(self, command):
        """Convert NLM motor command to ROS Twist message."""
        twist = Twist()
        
        if command == nlm.MotorCommand.MoveForward:
            twist.linear.x = 0.2
        elif command == nlm.MotorCommand.MoveBackward:
            twist.linear.x = -0.2
        elif command == nlm.MotorCommand.TurnLeft:
            twist.angular.z = 0.5
        elif command == nlm.MotorCommand.TurnRight:
            twist.angular.z = -0.5
        else:
            twist.linear.x = 0.0
            twist.angular.z = 0.0
        
        return twist
```

### 2. WebSocket Integration

```cpp
#include <nlohmann/json.hpp>
#include <uWebSockets/App.h>

class WebSocketController {
private:
    nlm::Brain* brain_;
    uWS::App app_;
    
public:
    WebSocketController(nlm::Brain* brain) : brain_(brain) {
        setupWebSocket();
    }
    
private:
    void setupWebSocket() {
        app_.ws<Connection>("/*")
            .onMessage([this](auto* ws, std::string_view message, Connection*) {
                handleMessage(ws, message);
            })
            .onConnection([this](auto* ws, auto*) {
                onConnection(ws);
            })
            .onDisconnection([this](auto* ws, int code, std::string_view message, Connection*) {
                onDisconnection(ws);
            });
    }
    
    void handleMessage(uWS::WebSocket<Connection>* ws, std::string_view message) {
        try {
            auto json = nlohmann::json::parse(message);
            
            if (json["type"] == "step") {
                // Execute simulation step
                size_t step = json["step"];
                double time = json["time"];
                
                brain_->step(step, time);
                
                // Send response
                auto response = nlohmann::json::object();
                response["type"] = "status";
                response["spikes"] = brain_->getTotalSpikeCount();
                response["neurons"] = brain_->getTotalNeuronCount();
                response["active"] = brain_->getActiveNeuronCount();
                
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }
            else if (json["type"] == "reset") {
                brain_->reset();
                brain_->initialize();
                
                ws->send("{\"type\": \"reset\", \"status\": \"ok\"}", uWS::OpCode::TEXT);
            }
            
        } catch (const std::exception& e) {
            auto error_response = nlohmann::json::object();
            error_response["type"] = "error";
            error_response["message"] = std::string("Error: ") + e.what();
            
            ws->send(error_response.dump(), uWS::OpCode::TEXT);
        }
    }
    
    void onConnection(uWS::WebSocket<Connection>* ws) {
        // New client connected
        auto response = nlohmann::json::object();
        response["type"] = "connection";
        response["message"] = "Connected to NLM simulation";
        response["brain_state"] = {
            {"spikes", brain_->getTotalSpikeCount()},
            {"neurons", brain_->getTotalNeuronCount()},
            {"regions", brain_->getRegionCount()}
        };
        
        ws->send(response.dump(), uWS::OpCode::TEXT);
    }
    
    void onDisconnection(uWS::WebSocket<Connection>* ws, int code, std::string_view message) {
        // Client disconnected
    }
    
public:
    void start(int port = 8080) {
        app_.listen(port, [port](auto* token) {
            if (token) {
                std::cout << "WebSocket server listening on port " << port << std::endl;
            }
        });
        
        app_.run();
    }
};
```

## API Reference

### Python API

#### Module-level Functions

| Function | Description |
|----------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create brain with configuration |
| `createSimpleWorld()` | Create simple world |
| `createAgentBrain(brain)` | Create agent brain interface |
| `to_float_vector(data)` | Convert to float vector |
| `to_double_vector(data)` | Convert to double vector |

#### Exception Classes

| Exception | Description |
|-----------|-------------|
| `NLMError` | Base NLM exception |
| `InvalidArgumentError` | Invalid argument error |
| `SimulationError` | Simulation error |
| `ConfigurationError` | Configuration error |

#### Enums

| Enum | Values |
|------|--------|
| `NeuronType` | `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal` |
| `SynapseType` | `Excitatory`, `Inhibitory`, `Modulatory`, `Electrical`, `GapJunction` |
| `ActionType` | `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`, ... |
| `MotorCommand` | `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, ... |
| `DevelopmentalStage` | `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging` |
| `FiringState` | `Resting`, `Active`, `Refractory`, `Inhibited` |

#### Classes

See individual class documentation above for detailed method references.

### C++ API

#### Core Classes

See individual class documentation above for detailed method references.

#### Namespace Functions

```cpp
namespace nlm {
    // Factory functions
    std::shared_ptr<Config> createDefaultConfig();
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    std::shared_ptr<SimpleWorld> createSimpleWorld();
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);
    
    // Type conversion utilities
    std::vector<float> toFloatVector(const std::vector<double>& input);
    std::vector<double> toDoubleVector(const std::vector<float>& input);
    
    // Constants
    extern const NeuronId INVALID_NEURON_ID;
    extern const SynapseId INVALID_SYNAPSE_ID;
    // ... more constants
}
```

## Performance Considerations

### Memory Usage

- **Per Neuron**: ~100-200 bytes (depends on configuration)
- **Per Synapse**: ~40-80 bytes
- **Recommendation**: Use 100-1000 neurons for testing, 1000-10000 for production

### CPU Usage

- **Simple network**: ~10-50 ms per simulation step
- **Complex network**: ~100-500 ms per simulation step
- **Real-time requirement**: <10 ms per step for 100 Hz control

### Scaling

```python
# Scale configuration parameters for different use cases

# Research/Benchmark
config.set("neuron_count", 100000)
config.set("region_count", 100)

# Real-time Control
config.set("neuron_count", 1000)
config.set("region_count", 10)

# Embedded/IoT
config.set("neuron_count", 100)
config.set("region_count", 2)
```

## Future Enhancements

### Planned Features

1. **Advanced Learning Algorithms**
   - Reinforcement learning integration
   - Hebbian learning with neuromodulation
   - Predictive coding

2. **Enhanced Visualization**
   - 3D neural visualization
   - Real-time streaming
   - Virtual reality integration

3. **Hardware Acceleration**
   - GPU support via CUDA
   - FPGA acceleration
   - Specialized neural hardware interfaces

4. **Distributed Computing**
   - Multi-agent systems
   - Cloud deployment
   - Edge computing

5. **Extended Sensorimotor Interface**
   - Virtual reality control
   - Augmented reality overlay
   - Haptic feedback integration

### Research Directions

- **Neuromorphic Computing**: Integration with true neuromorphic hardware
- **Developmental Robotics**: Learning through interaction with environments
- **Cognitive Architecture**: Full cognitive stack implementation
- **Theoretical Neuroscience**: Testing hypotheses about brain function

## Conclusion

The NLM Python bindings provide a powerful, flexible interface to the neural simulation framework. Whether you're building simple proof-of-concepts, complex multi-agent systems, or real-time control applications, the enhanced Python API offers the tools you need to succeed.

The improvements implemented in this version include:

1. **Reduced Boilerplate**: Template-based binding helpers
2. **Enhanced Error Handling**: Custom exceptions with clear messages
3. **Type Conversions**: Seamless Python-C++ data exchange
4. **Convenience Methods**: Factory functions and Pythonic interfaces
5. **Documentation**: Comprehensive examples and API references
6. **Performance Optimizations**: Memory-efficient operations

These improvements make NLM more accessible to Python developers while maintaining the powerful capabilities of the underlying C++ neural simulation engine.
