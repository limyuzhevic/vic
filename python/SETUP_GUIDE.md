# Enhanced NLM Python Bindings - Complete Setup and Installation Guide

## Quick Start

This guide provides comprehensive instructions for setting up and using the enhanced NLM Python bindings with all new features including:

- Custom exception system
- Pythonic APIs with configuration presets
- Episode context management
- Enhanced debugging and profiling utilities
- Advanced configuration options
- Comprehensive test suite

## System Requirements

### Minimum Requirements
- **Operating System**: Linux, macOS, or Windows
- **Python**: 3.8 or higher
- **Compiler**: C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- **CMake**: 3.16 or higher
- **Build Tools**: make, ninja, or MSVC

### Recommended for Full Feature Set
- **NumPy**: For array operations and debugging utilities
- **pytest**: For running the enhanced test suite
- **matplotlib**: For visualization utilities (optional)

## Step 1: Install Dependencies

### Python Dependencies

```bash
# Install system packages for Python bindings
pip install pybind11 numpy pytest matplotlib

# For development
pip install scikit-build-core
```

### Build Dependencies

```bash
# On Ubuntu/Debian
 sudo apt update
sudo apt install build-essential cmake python3-dev python3-pip

# On macOS
brew install cmake

# On Windows (with Visual Studio)
# Install Visual Studio with C++ components
```

## Step 2: Clone and Configure the Repository

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Check the current branch
# Make sure you're on the development branch with enhancements
```

## Step 3: Build the Enhanced Python Bindings

### Option A: Using the Provided Build Script (Recommended)

```bash
# Make the build script executable
chmod +x build_enhanced_python.sh

# Run the enhanced build script
./build_enhanced_python.sh
```

This script will:
1. Create an enhanced build directory
2. Configure CMake with Python bindings
3. Build all C++ libraries and the Python module
4. Install the Python package in development mode
5. Run the comprehensive test suite

### Option B: Manual Build (Advanced)

```bash
# Create build directory
mkdir build_python
cd build_python

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build
make -j$(nproc)

# Install Python package
cd python
pip install -e .
```

## Step 4: Verify Installation

### Basic Verification

```python
import pynlm

print("=== NLM Enhanced Bindings Test ===")
print(f"Version: {pynlm.getVersion()}")
print(f"Platform: {pynlm.getPlatformInfo()}")

# Test custom exceptions
error = pynlm.NLMError("Test error")
print(f"Custom exception created: {error}")

# Test configuration presets
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createPerformanceConfig()
config3 = pynlm.createExplorationConfig()

print(f"Default config neurons: {config1['brain.neuron_count']}")
print(f"Performance config neurons: {config2['brain.neuron_count']}")
print(f"Exploration config neurons: {config3['brain.neuron_count']}")
```

### Run the Enhanced Test Suite

```bash
# Run the comprehensive test suite
python -m pytest python/test_enhanced_bindings.py -v

# Or run with verbose output
python python/test_enhanced_bindings.py
```

## Step 5: Create a Test Environment

### Create a Python Script for Testing

```bash
cat > test_nlm_enhanced.py << 'EOF'
import pynlm
import tempfile
import os

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    # Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain statistics
    stats = brain.getBrainStatistics()
    assert "Neuron Count:" in stats
    print("✓ Brain statistics work")
    
    # Test brain snapshot
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        filename = f.name
    
    try:
        brain.createSnapshot(filename)
        brain.reset()
        brain.loadSnapshot(filename)
        assert brain.getTotalNeuronCount() == 1000
        print("✓ Brain snapshot works")
    finally:
        os.unlink(filename)
    
    print("Basic functionality test passed!")

def test_agent_functionality():
    """Test agent brain functionality."""
    print("Testing agent brain functionality...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configureSimple(15, 15)
    agent.initialize(world)
    
    # Test subsystems
    agent.enableAllSubsystems()
    status = agent.getSubsystemStatus()
    assert all(status)
    print("✓ Agent subsystems work")
    
    # Test agent info
    info = agent.getAgentInfo()
    assert "Developmental Stage:" in info
    print("✓ Agent info works")
    
    print("Agent functionality test passed!")

def test_world_functionality():
    """Test world functionality."""
    print("Testing world functionality...")
    
    world = pynlm.createSimpleWorld()
    world.configureSimple(10, 10)
    
    # Test position utilities
    world.setAgentStart(5.0, 5.0)
    assert world.isAgentNear(5.0, 5.0, 1.0)
    assert not world.isAgentNear(10.0, 10.0, 1.0)
    print("✓ World position utilities work")
    
    # Test object management
    world.addResource(2.0, 2.0, 5.0)
    world.addHazard(8.0, 8.0, 2.0)
    print("✓ World object management works")
    
    # Test world bounds
    width, height = world.getWorldBounds()
    assert width == 10 and height == 10
    print("✓ World bounds work")
    
    print("World functionality test passed!")

def test_configuration():
    """Test configuration features."""
    print("Testing configuration features...")
    
    # Test all preset configurations
    default_config = pynlm.createDefaultConfig()
    perf_config = pynlm.createPerformanceConfig()
    explore_config = pynlm.createExplorationConfig()
    debug_config = pynlm.createDebugConfig()
    
    assert default_config["brain.neuron_count"] == 1000
    assert perf_config["brain.neuron_count"] == 500
    assert explore_config["brain.neuron_count"] == 2000
    assert debug_config["brain.neuron_count"] == 100
    print("✓ Configuration presets work")
    
    # Test Pythonic API
    config = pynlm.createDefaultConfig()
    config["brain.neuron_count"] = 3000
    config["test.value"] = 42.5
    config["test.enabled"] = True
    
    assert config["brain.neuron_count"] == 3000
    assert config["test.value"] == 42.5
    assert config["test.enabled"] is True
    print("✓ Pythonic configuration API works")
    
    # Test configuration merge
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.createPerformanceConfig()
    config1.merge(config2)
    assert config1["brain.neuron_count"] == 500
    print("✓ Configuration merge works")
    
    print("Configuration test passed!")

def test_episode_management():
    """Test episode context management."""
    print("Testing episode management...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Test episode context
    with pynlm.episode_context(brain, agent, world) as episode:
        assert episode.isInitialized()
        assert "Episode initialized:" in episode.getStatus()
        print("✓ Episode context manager works")
    
    # Test episode after exit
    assert not episode.isInitialized()
    print("✓ Episode cleanup works")
    
    print("Episode management test passed!")

if __name__ == "__main__":
    try:
        test_basic_functionality()
        test_agent_functionality()
        test_world_functionality()
        test_configuration()
        test_episode_management()
        
        print("\n=== All tests passed! ===")
        print("NLM enhanced bindings are working correctly.")
        
    except Exception as e:
        print(f"\nTest failed with error: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
EOF

python test_nlm_enhanced.py
```

## Step 6: Advanced Usage Examples

### Example 1: Learning Environment with Exploration

```python
import pynlm
import time

class LearningEnvironment:
    def __init__(self, config_type="exploration"):
        # Create configuration
        if config_type == "exploration":
            self.config = pynlm.createExplorationConfig()
        elif config_type == "performance":
            self.config = pynlm.createPerformanceConfig()
        else:
            self.config = pynlm.createDefaultConfig()
        
        # Create components
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure world
        self.world.configure(20, 20, 10, 10)
        
        # Initialize
        self.brain.initialize()
        self.agent.initialize(self.world)
        
        # Enable all subsystems for learning
        self.agent.enableAllSubsystems()
        
        # Performance monitoring
        self.step_times = []
        self.memory_samples = []
        
        print(f"Learning environment initialized with {self.config['brain.neuron_count']} neurons")
    
    def run_episode(self, max_steps=100):
        """Run a complete episode."""
        with pynlm.episode_context(self.brain, self.agent, self.world) as episode:
            print(f"Starting episode {episode}")
            
            for step in range(max_steps):
                start_time = time.time()
                
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Brain step
                self.brain.step(step)
                
                # Decode motor command
                motor_cmd = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Apply neuromodulation
                reward = self.calculate_reward(perpcept)
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Record performance
                end_time = time.time()
                self.step_times.append(end_time - start_time)
                
                # Print progress
                if step % 10 == 0:
                    self.print_step_info(step)
                
                # Check for termination conditions
                if self.terminate_episode(step, percept):
                    break
            
            # Episode summary
            self.print_episode_summary()
    
    def calculate_reward(self, percept):
        """Calculate reward based on agent state and world state."""
        # Example reward function
        body = self.world.getAgentBody()
        reward = 0.0
        
        # Energy reward
        if body.energy > 0:
            reward += body.energy * 0.1
        
        # Novelty reward (encourage exploration)
        curiosity = self.agent.getCuriosityLevel()
        reward += curiosity * 0.05
        
        # Novelty detection
        novelty = self.agent.getNoveltyLevel()
        if novelty > 0.5:
            reward += novelty * 0.2
        
        return reward
    
    def print_step_info(self, step):
        """Print information about current step."""
        brain = self.brain
        agent = self.agent
        
        stats = {
            'step': step,
            'neurons': brain.getTotalNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development': agent.getDevelopmentalStage(),
            'world_time': self.world.getSimulationTime()
        }
        
        print(f"Step {step}: "
              f"Neurons={stats['neurons']}, "
              f"Firing={stats['firing']}, "
              f"Rate={stats['rate']:.2f}, "
              f"Curiosity={stats['curiosity']:.3f}, "
              f"Novelty={stats['novelty']:.3f}, "
              f"Dev={stats['development']}")
    
    def print_episode_summary(self):
        """Print episode summary."""
        brain = self.brain
        agent = self.agent
        
        print(f"\n=== Episode Summary ===")
        print(f"Final statistics:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
        print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
        
        if self.step_times:
            avg_time = sum(self.step_times) / len(self.step_times)
            max_time = max(self.step_times)
            print(f"  Performance: avg_step={avg_time:.4f}s, max_step={max_time:.4f}s")
    
    def terminate_episode(self, step, percept):
        """Check if episode should terminate."""
        # Check for completion conditions
        body = self.world.getAgentBody()
        
        # Terminate if agent dies
        if body.health <= 0 or body.energy <= 0:
            print(f"Episode terminated: Agent died at step {step}")
            return True
        
        # Terminate if reached maximum steps
        if step >= 100:
            print(f"Episode completed: Reached maximum steps")
            return True
        
        # Terminate if converged (low novelty and curiosity)
        if agent.getNoveltyLevel() < 0.01 and agent.getCuriosityLevel() < 0.01:
            print(f"Episode terminated: Low activity at step {step}")
            return True
        
        return False

# Example usage
if __name__ == "__main__":
    # Create learning environment
    env = LearningEnvironment("exploration")
    
    # Run learning episodes
    for episode_num in range(3):
        print(f"\n{'='*50}")
        print(f"LEARNING EPISODE {episode_num + 1}")
        print(f"{'='*50}")
        
        env.run_episode(max_steps=50)
    
    print(f"\n{'='*50}")
    print("LEARNING COMPLETE")
    print(f"{'='*50}")
```

### Example 2: Configuration Validation and Error Handling

```python
import pynlm
import json

def safe_config_from_file(filepath):
    """Safely load configuration from file with validation."""
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filepath)
        
        # Validate configuration
        if not config.validate():
            raise pynlm.ConfigError(f"Invalid configuration in {filepath}")
        
        # Apply additional custom validation
        validate_custom_requirements(config)
        
        return config
        
    except pynlm.ConfigError as e:
        print(f"Configuration error: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error loading configuration: {e}")
        raise pynlm.ConfigError(f"Failed to load configuration from {filepath}")

def validate_custom_requirements(config):
    """Validate custom configuration requirements."""
    # Example custom validation
    neuron_count = config["brain.neuron_count"]
    if neuron_count < 1 or neuron_count > 10000:
        raise pynlm.ConfigError(f"Invalid neuron count: {neuron_count}")
    
    # Check for required settings
    required_settings = [
        "brain.synapse_density",
        "brain.region_count",
        "plasticity.stdp.enable",
        "neuromod.curiosity.enable"
    ]
    
    for setting in required_settings:
        if not config.has(setting):
            raise pynlm.ConfigError(f"Missing required setting: {setting}")

def create_adaptive_config(base_config_type="default"):
    """Create adaptive configuration based on system capabilities."""
    # Determine system capabilities
    cpu_count = os.cpu_count() or 4
    memory_gb = get_available_memory()  # Implement this function
    
    # Choose configuration based on system
    if cpu_count >= 8 and memory_gb >= 16:
        config_type = "exploration"
    elif cpu_count >= 4 and memory_gb >= 8:
        config_type = "performance"
    else:
        config_type = "debug"
    
    # Create base configuration
    if config_type == "exploration":
        config = pynlm.createExplorationConfig()
    elif config_type == "performance":
        config = pynlm.createPerformanceConfig()
    else:
        config = pynlm.createDebugConfig()
    
    # Adapt based on system capabilities
    config["brain.neuron_count"] = min(
        config["brain.neuron_count"],
        max(100, int(1000 * (cpu_count / 8)))
    )
    
    return config

# Usage example
if __name__ == "__main__":
    # Create safe configuration
    try:
        config = safe_config_from_file("my_config.json")
        print("Configuration loaded successfully")
        
        # Create adaptive configuration
        adaptive_config = create_adaptive_config()
        print(f"Adaptive configuration created with {adaptive_config['brain.neuron_count']} neurons")
        
    except Exception as e:
        print(f"Failed to create configuration: {e}")
```

### Example 3: Performance Monitoring and Profiling

```python
import pynlm
import time
import psutil
import os

class PerformanceProfiler:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.metrics = {
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'neuron_activity': [],
            'agent_metrics': []
        }
        self.start_time = None
        self.process = psutil.Process(os.getpid())
    
    def start_profile(self):
        """Start performance profiling."""
        self.start_time = time.time()
        print("Performance profiling started")
    
    def record_step(self, step):
        """Record performance metrics for a step."""
        if self.start_time is None:
            return
        
        step_time = time.time() - self.start_time
        self.metrics['step_times'].append(step_time)
        
        # Record system metrics
        self.metrics['memory_usage'].append(
            self.process.memory_info().rss / 1024 / 1024  # MB
        )
        self.metrics['cpu_usage'].append(
            self.process.cpu_percent()
        )
        
        # Record neural metrics
        self.metrics['neuron_activity'].append({
            'step': step,
            'neurons': self.brain.getTotalNeuronCount(),
            'firing': self.brain.getFiringNeuronCount(),
            'rate': self.brain.getAverageFiringRate(),
            'spikes': self.brain.getTotalSpikeCount()
        })
        
        # Record agent metrics
        self.metrics['agent_metrics'].append({
            'step': step,
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'development': self.agent.getDevelopmentalStage(),
            'prediction_error': self.agent.getPredictionError()
        })
        
        self.start_time = time.time()
    
    def generate_report(self):
        """Generate performance report."""
        if not self.metrics['step_times']:
            return "No performance data collected"
        
        report = "=== Performance Report ===\n"
        
        # Step performance
        avg_step_time = sum(self.metrics['step_times']) / len(self.metrics['step_times'])
        max_step_time = max(self.metrics['step_times'])
        min_step_time = min(self.metrics['step_times'])
        
        report += f"Step Performance:\n"
        report += f"  Average step time: {avg_step_time:.4f}s\n"
        report += f"  Max step time: {max_step_time:.4f}s\n"
        report += f"  Min step time: {min_step_time:.4f}s\n"
        report += f"  Total steps: {len(self.metrics['step_times'])}\n"
        
        # Memory performance
        if self.metrics['memory_usage']:
            avg_memory = sum(self.metrics['memory_usage']) / len(self.metrics['memory_usage'])
            max_memory = max(self.metrics['memory_usage'])
            
            report += f"\nMemory Usage:\n"
            report += f"  Average: {avg_memory:.1f} MB\n"
            report += f"  Maximum: {max_memory:.1f} MB\n"
        
        # CPU performance
        if self.metrics['cpu_usage']:
            avg_cpu = sum(self.metrics['cpu_usage']) / len(self.metrics['cpu_usage'])
            max_cpu = max(self.metrics['cpu_usage'])
            
            report += f"\nCPU Usage:\n"
            report += f"  Average: {avg_cpu:.1f}%\n"
            report += f"  Maximum: {max_cpu:.1f}%\n"
        
        # Neural performance
        if self.metrics['neuron_activity']:
            neural_activity = self.metrics['neuron_activity']
            avg_neurons = sum(n['neurons'] for n in neural_activity) / len(neural_activity)
            avg_firing = sum(n['firing'] for n in neural_activity) / len(neural_activity)
            total_spikes = sum(n['spikes'] for n in neural_activity)
            
            report += f"\nNeural Performance:\n"
            report += f"  Average neurons: {avg_neurons:.0f}\n"
            report += f"  Average firing neurons: {avg_firing:.0f}\n"
            report += f"  Total spikes: {total_spikes}\n"
        
        # Agent performance
        if self.metrics['agent_metrics']:
            agent_metrics = self.metrics['agent_metrics']
            avg_curiosity = sum(a['curiosity'] for a in agent_metrics) / len(agent_metrics)
            avg_novelty = sum(a['novelty'] for a in agent_metrics) / len(agent_metrics)
            
            report += f"\nAgent Performance:\n"
            report += f"  Average curiosity: {avg_curiosity:.3f}\n"
            report += f"  Average novelty: {avg_novelty:.3f}\n"
        
        return report
    
    def save_metrics(self, filepath):
        """Save performance metrics to file."""
        with open(filepath, 'w') as f:
            json.dump(self.metrics, f, indent=2)
        print(f"Performance metrics saved to {filepath}")

# Usage example
if __name__ == "__main__":
    # Create NLM system
    config = pynlm.createExplorationConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Initialize
    brain.initialize()
    agent.initialize(world)
    
    # Create profiler
    profiler = PerformanceProfiler(brain, agent, world)
    profiler.start_profile()
    
    # Run simulation with profiling
    for step in range(20):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        profiler.record_step(step)
    
    # Generate and print report
    report = profiler.generate_report()
    print(report)
    
    # Save metrics
    profiler.save_metrics("performance_metrics.json")
```

## Step 7: Advanced Configuration Examples

### 1. Environment-Specific Configurations

```python
import pynlm

# Development configuration (small, fast, debuggable)
def create_dev_config():
    config = pynlm.createDebugConfig()
    config.set("brain.connection_probability", 0.3, nlm.ConfigSource::Runtime)
    config.set("neuromod.curiosity.enable", True, nlm.ConfigSource::Runtime)
    config.set("neuromod.novelty.enable", True, nlm.ConfigSource::Runtime)
    return config

# Testing configuration (consistent, repeatable)
def create_test_config():
    config = pynlm.createPerformanceConfig()
    config.set("brain.neuron_count", 500, nlm.ConfigSource::Runtime)
    config.set("brain.synapse_density", 0.02, nlm.ConfigSource::Runtime)
    config.set("world.width", 10, nlm.ConfigSource::Runtime)
    config.set("world.height", 10, nlm.ConfigSource::Runtime)
    return config

# Production configuration (balanced, optimized)
def create_prod_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000, nlm.ConfigSource::Runtime)
    config.set("brain.synapse_density", 0.1, nlm.ConfigSource::Runtime)
    config.set("plasticity.stdp.enable", True, nlm.ConfigSource::Runtime)
    config.set("plasticity.hebbian.enable", True, nlm.ConfigSource::Runtime)
    config.set("plasticity.structural.enable", False, nlm.ConfigSource::Runtime)  # Disable for stability
    return config

# Usage
prod_config = create_prod_config()
```

### 2. Dynamic Configuration Adaptation

```python
import pynlm
import time

class AdaptiveConfiguration:
    def __init__(self, initial_config_type="default"):
        self.config_types = {
            "initial": pynlm.createDefaultConfig(),
            "exploration": pynlm.createExplorationConfig(),
            "performance": pynlm.createPerformanceConfig(),
            "debug": pynlm.createDebugConfig()
        }
        
        self.current_type = initial_config_type
        self.performance_history = []
        self.adaptation_counter = 0
        
        # Start with the specified configuration
        self.current_config = self.config_types[initial_config_type].copy()
    
    def evaluate_performance(self, metrics):
        """Evaluate performance and decide if adaptation is needed."""
        # Calculate performance score
        score = 0
        
        # Higher curiosity and novelty indicate exploration
        if metrics.get('curiosity', 0) > 0.5:
            score += 1
        
        # Lower step time indicates good performance
        if metrics.get('step_time', 0) < 0.1:
            score += 1
        
        # Store performance history
        self.performance_history.append({
            'timestamp': time.time(),
            'score': score,
            'metrics': metrics
        })
        
        # Keep only recent history
        if len(self.performance_history) > 10:
            self.performance_history.pop(0)
        
        # Adapt based on performance
        self.adapt_config(metrics)
    
    def adapt_config(self, metrics):
        """Adapt configuration based on performance."""
        self.adaptation_counter += 1
        
        # Simple adaptation logic
        if metrics.get('curiosity', 0) > 0.7 and metrics.get('novelty', 0) > 0.7:
            # Too much exploration, switch to performance
            self.current_type = "performance"
            self.current_config = self.config_types["performance"].copy()
            print("Switching to performance configuration")
        
        elif metrics.get('curiosity', 0) < 0.1 and metrics.get('novelty', 0) < 0.1:
            # Too much exploitation, switch to exploration
            self.current_type = "exploration"
            self.current_config = self.config_types["exploration"].copy()
            print("Switching to exploration configuration")
        
        elif metrics.get('step_time', 0) > 0.5:
            # Too slow, switch to performance
            self.current_type = "performance"
            self.current_config = self.config_types["performance"].copy()
            print("Switching to performance configuration (too slow)")
        
        # Save configuration to file
        self.save_config(f"config_adaptation_{self.adaptation_counter}.json")
    
    def get_config(self):
        """Get current configuration."""
        return self.current_config
    
    def set_config_type(self, config_type):
        """Set configuration type."""
        if config_type in self.config_types:
            self.current_type = config_type
            self.current_config = self.config_types[config_type].copy()
            print(f"Configuration switched to {config_type}")
        else:
            print(f"Unknown configuration type: {config_type}")
    
    def save_config(self, filepath):
        """Save current configuration to file."""
        self.current_config.saveToFile(filepath)
    
    def load_config(self, filepath):
        """Load configuration from file."""
        try:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(filepath)
            self.current_config = config
            
            # Determine configuration type based on neuron count
            neuron_count = config["brain.neuron_count"]
            if neuron_count <= 500:
                self.current_type = "debug"
            elif neuron_count <= 1500:
                self.current_type = "performance"
            else:
                self.current_type = "exploration"
                
        except Exception as e:
            print(f"Failed to load configuration: {e}")

# Usage example
if __name__ == "__main__":
    # Create adaptive configuration
    adaptive_config = AdaptiveConfiguration("default")
    
    # Get initial configuration
    config = adaptive_config.get_config()
    print(f"Initial configuration type: {adaptive_config.current_type}")
    print(f"Neuron count: {config['brain.neuron_count']}")
    
    # Simulate performance evaluation
    sample_metrics = {
        'step_time': 0.05,
        'curiosity': 0.8,
        'novelty': 0.7
    }
    
    adaptive_config.evaluate_performance(sample_metrics)
    print(f"After evaluation - Configuration type: {adaptive_config.current_type}")
    
    # Change configuration type
    adaptive_config.set_config_type("exploration")
    print(f"New configuration type: {adaptive_config.current_type}")
```

## Step 8: Integration with External Tools

### 1. Integration with ML Frameworks

```python
import pynlm
import numpy as np
import torch
import json

class NLMInterface:
    """Interface for integrating NLM with machine learning frameworks."""
    
    def __init__(self):
        self.brain = None
        self.agent = None
        self.world = None
        self.config = None
    
    def initialize_from_ml_config(self, ml_config):
        """Initialize NLM from ML configuration."""
        # Convert ML configuration to NLM configuration
        nlm_config = pynlm.createDefaultConfig()
        
        # Map ML parameters to NLM parameters
        ml_to_nlm_mapping = {
            'network_size': 'brain.neuron_count',
            'connection_probability': 'brain.connection_probability',
            'learning_rate': 'plasticity.stdp.learning_rate',
            'exploration_rate': 'neuromod.curiosity.scale',
            'environment_width': 'world.width',
            'environment_height': 'world.height'
        }
        
        for ml_key, nlm_key in ml_to_nlm_mapping.items():
            if ml_key in ml_config:
                nlm_config[nlm_key] = ml_config[ml_key]
        
        # Create NLM system
        self.brain = pynlm.createBrain(nlm_config)
        self.config = nlm_config
        
        return self
    
    def create_torch_interface(self, observation_space, action_space):
        """Create PyTorch interface for NLM control."""
        # This would integrate NLM with neural networks
        # For demonstration, we'll create a simple wrapper
        
        class NLMController(torch.nn.Module):
            def __init__(self, nlm_interface):
                super().__init__()
                self.nlm = nlm_interface
                
                # Neural network layers for action prediction
                self.action_predictor = torch.nn.Sequential(
                    torch.nn.Linear(observation_space, 128),
                    torch.nn.ReLU(),
                    torch.nn.Linear(128, action_space)
                )
            
            def forward(self, observation):
                # Process observation with NLM
                sensory_input = self.nlm.create_sensory_input(observation)
                self.nlm.brain.receiveSensoryInput(sensory_input)
                
                # Predict action
                action = self.action_predictor(observation)
                
                return action
        
        return NLMController(self)
    
    def save_ml_format(self, filepath, additional_data=None):
        """Save NLM state in ML format."""
        save_data = {
            'brain_state': {
                'neuron_count': self.brain.getTotalNeuronCount() if self.brain else 0,
                'synapse_count': self.brain.getTotalSynapseCount() if self.brain else 0,
                'firing_neurons': self.brain.getFiringNeuronCount() if self.brain else 0,
                'average_firing_rate': self.brain.getAverageFiringRate() if self.brain else 0
            },
            'agent_state': {
                'curiosity': self.agent.getCuriosityLevel() if self.agent else 0,
                'novelty': self.agent.getNoveltyLevel() if self.agent else 0,
                'development': self.agent.getDevelopmentalStage() if self.agent else 0,
                'prediction_error': self.agent.getPredictionError() if self.agent else 0
            },
            'world_state': {
                'width': self.world.getWidth() if self.world else 0,
                'height': self.world.getHeight() if self.world else 0,
                'agent_x': self.world.getAgentBody().x if self.world else 0,
                'agent_y': self.world.getAgentBody().y if self.world else 0
            }
        }
        
        if additional_data:
            save_data.update(additional_data)
        
        with open(filepath, 'w') as f:
            json.dump(save_data, f, indent=2)
    
    def load_ml_format(self, filepath):
        """Load NLM state from ML format."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # This would restore NLM state from ML format
        # Implementation depends on specific ML format requirements
        
        print("ML format loading not fully implemented - customize for specific ML format")
        return self

# Usage example
if __name__ == "__main__":
    # Create ML configuration
    ml_config = {
        'network_size': 1000,
        'connection_probability': 0.1,
        'learning_rate': 0.001,
        'exploration_rate': 1.0,
        'environment_width': 20,
        'environment_height': 20
    }
    
    # Initialize NLM interface
    nlm_ml = NLMInterface().initialize_from_ml_config(ml_config)
    
    # Create PyTorch interface (if needed)
    # controller = nlm_ml.create_torch_interface(observation_space=10, action_space=4)
    
    # Save in ML format
    nlm_ml.save_ml_format("nlm_ml_state.json", 
                         additional_data={'timestamp': time.time(),
                                        'episode': 1})
    
    print("NLM saved in ML format")
```

### 2. Integration with Data Science Workflows

```python
import pynlm
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

class NLSDataPipeline:
    """Data pipeline for NLM experimentation."""
    
    def __init__(self):
        self.experiments = []
        self.results = []
    
    def run_experiment(self, config, experiment_name, max_steps=100):
        """Run an experiment with given configuration."""
        print(f"Running experiment: {experiment_name}")
        
        # Create NLM system
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(config["world.width"], config["world.height"],
                       config["world.vision_width"], config["world.vision_height"])
        
        # Initialize
        brain.initialize()
        agent.initialize(world)
        
        # Run experiment
        experiment_data = {
            'experiment_name': experiment_name,
            'config': config.getKeys(),
            'results': [],
            'start_time': time.time()
        }
        
        with pynlm.episode_context(brain, agent, world) as episode:
            for step in range(max_steps):
                # Run simulation step
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                agent.applyRewardModulation(0.0, 0.0)  # Simplified reward
                agent.updateDevelopment(0.1)
                
                # Record data
                step_data = {
                    'step': step,
                    'world_time': world.getSimulationTime(),
                    'neurons': brain.getTotalNeuronCount(),
                    'firing': brain.getFiringNeuronCount(),
                    'rate': brain.getAverageFiringRate(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'development': agent.getDevelopmentalStage()
                }
                
                experiment_data['results'].append(step_data)
        
        # Calculate experiment metrics
        experiment_data['end_time'] = time.time()
        experiment_data['duration'] = experiment_data['end_time'] - experiment_data['start_time']
        experiment_data['steps_per_second'] = len(experiment_data['results']) / experiment_data['duration']
        
        if experiment_data['results']:
            experiment_data['final_neurons'] = experiment_data['results'][-1]['neurons']
            experiment_data['final_firing'] = experiment_data['results'][-1]['firing']
            experiment_data['final_curiosity'] = experiment_data['results'][-1]['curiosity']
            experiment_data['final_novelty'] = experiment_data['results'][-1]['novelty']
        
        self.experiments.append(experiment_data)
        self.results.append(experiment_data['results'])
        
        print(f"Experiment {experiment_name} completed")
        return experiment_data
    
    def analyze_results(self):
        """Analyze experiment results."""
        if not self.results:
            return "No results to analyze"
        
        # Create DataFrame
        all_data = []
        for experiment_results in self.results:
            all_data.extend(experiment_results)
        
        df = pd.DataFrame(all_data)
        
        # Basic statistics
        analysis = {
            'total_steps': len(df),
            'unique_experiments': len(self.experiments),
            'average_neuron_count': df['neurons'].mean(),
            'average_firing_rate': df['rate'].mean(),
            'average_curiosity': df['curiosity'].mean(),
            'average_novelty': df['novelty'].mean()
        }
        
        return analysis
    
    def plot_results(self, save_path=None):
        """Plot experiment results."""
        if not self.results:
            print("No results to plot")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Plot 1: Learning curves
        for i, experiment_results in enumerate(self.results):
            df = pd.DataFrame(experiment_results)
            axes[0, 0].plot(df['step'], df['rate'], 
                           label=f"Experiment {i+1}", 
                           alpha=0.7)
        
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Average Firing Rate')
        axes[0, 0].set_title('Learning Curves')
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Curiosity and Novelty over time
        for i, experiment_results in enumerate(self.results):
            df = pd.DataFrame(experiment_results)
            axes[0, 1].plot(df['step'], df['curiosity'], 
                           label=f"Experiment {i+1} (Curiosity)", 
                           alpha=0.7)
            axes[0, 1].plot(df['step'], df['novelty'], 
                           label=f"Experiment {i+1} (Novelty)", 
                           alpha=0.7, linestyle='--')
        
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Level')
        axes[0, 1].set_title('Curiosity and Novelty')
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Neuron count distribution
        axes[1, 0].hist(df['neurons'], bins=20, alpha=0.7, edgecolor='black')
        axes[1, 0].set_xlabel('Neuron Count')
        axes[1, 0].set_ylabel('Frequency')
        axes[1, 0].set_title('Neuron Count Distribution')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Experimental comparison
        experiment_summary = []
        for i, experiment in enumerate(self.experiments):
            experiment_summary.append({
                'Experiment': f"Exp {i+1}",
                'Final Neurons': experiment.get('final_neurons', 0),
                'Final Firing': experiment.get('final_firing', 0),
                'Final Curiosity': experiment.get('final_curiosity', 0),
                'Final Novelty': experiment.get('final_novelty', 0),
                'Duration': experiment.get('duration', 0)
            })
        
        summary_df = pd.DataFrame(experiment_summary)
        summary_df.set_index('Experiment')[['Final Neurons', 'Final Firing', 
                                           'Final Curiosity', 'Final Novelty']].plot(
            kind='bar', ax=axes[1, 1])
        axes[1, 1].set_title('Experimental Comparison')
        axes[1, 1].legend()
        axes[1, 1].tick_params(axis='x', rotation=45)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            print(f"Results saved to {save_path}")
        
        plt.show()
    
    def save_results(self, filepath):
        """Save all experiment results to file."""
        results_data = {
            'experiments': self.experiments,
            'analysis': self.analyze_results()
        }
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2)
        
        print(f"Experiment results saved to {filepath}")

# Usage example
if __name__ == "__main__":
    # Create data pipeline
    pipeline = NLSDataPipeline()
    
    # Run experiments with different configurations
    default_config = pynlm.createDefaultConfig()
    perf_config = pynlm.createPerformanceConfig()
    explore_config = pynlm.createExplorationConfig()
    
    # Run experiments
    pipeline.run_experiment(default_config, "Default Configuration", max_steps=50)
    pipeline.run_experiment(perf_config, "Performance Configuration", max_steps=50)
    pipeline.run_experiment(explore_config, "Exploration Configuration", max_steps=50)
    
    # Analyze results
    analysis = pipeline.analyze_results()
    print("\n=== Experiment Analysis ===")
    for key, value in analysis.items():
        print(f"{key}: {value}")
    
    # Plot results
    pipeline.plot_results("experiment_results.png")
    
    # Save results
    pipeline.save_results("experiment_data.json")
```

## Step 9: Troubleshooting and Common Issues

### 9.1 Build Issues

#### CMake Configuration Errors

**Problem:** CMake cannot find Python or pybind11.

**Solution:**
```bash
# Install required dependencies
pip install pybind11 scikit-build-core

# Ensure CMake can find Python
cmake .. -DPYTHON_EXECUTABLE=$(which python3)

# Or install Python development packages
# On Ubuntu: sudo apt install python3-dev python3-pip
# On macOS: brew install python3
```

#### Compilation Errors

**Problem:** Compilation fails with compiler errors.

**Solution:**
```bash
# Try to clear cache and rebuild
rm -rf build_python
cd build_python
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 9.2 Runtime Issues

#### Import Errors

**Problem:** Cannot import pynlm.

**Solution:**
```bash
# Ensure the module is installed
cd python
pip install -e .

# Check if the module is in Python path
python -c "import sys; print(sys.path)"

# Try to build again
python setup.py build_ext --inplace
```

#### Missing Dependencies

**Problem:** Missing NumPy or other optional dependencies.

**Solution:**
```bash
pip install numpy matplotlib seaborn pandas
```

### 9.3 Test Failures

#### Test Suite Issues

**Problem:** Enhanced test suite fails to run.

**Solution:**
```bash
# Ensure Python package is installed
cd python
pip install -e .

# Run tests with verbose output
python test_enhanced_bindings.py

# Or use pytest
pytest test_enhanced_bindings.py -v
```

#### Configuration Issues

**Problem:** Configuration validation fails.

**Solution:**
```python
import pynlm

# Create a valid configuration
config = pynlm.createDefaultConfig()

# Check validation
if not config.validate():
    print("Configuration is invalid")
    print("Keys:", config.getKeys())
else:
    print("Configuration is valid")
```

### 9.4 Performance Issues

#### Slow Performance

**Problem:** NLM simulation is slow.

**Solution:**
```python
import pynlm

# Use performance configuration
config = pynlm.createPerformanceConfig()

# Reduce neuron count
config["brain.neuron_count"] = 500

# Disable expensive features
config["plasticity.structural.enable"] = False
config["neuromod.novelty.enable"] = False
```

#### Memory Usage

**Problem:** High memory usage.

**Solution:**
```python
import pynlm

# Use performance configuration
config = pynlm.createPerformanceConfig()

# Limit network size
config["brain.neuron_count"] = 1000
config["brain.synapse_density"] = 0.05
```

## Step 10: Advanced Features and Customization

### 10.1 Custom Exceptions

```python
import pynlm

# Create custom exception
class CustomNLMError(pynlm.NLMError):
    def __init__(self, message, error_code=None):
        super().__init__(message)
        self.error_code = error_code

# Use custom exception
try:
    config = pynlm.createDefaultConfig()
    # Your code that might fail
except CustomNLMError as e:
    print(f"Custom error: {e}")
    print(f"Error code: {e.error_code}")
```

### 10.2 Custom Configuration Classes

```python
import pynlm

class CustomConfig:
    """Custom configuration class with additional methods."""
    
    def __init__(self, base_config=None):
        if base_config:
            self.config = base_config
        else:
            self.config = pynlm.createDefaultConfig()
    
    def set_neurons(self, count, region_count=1):
        """Set neuron count and related parameters."""
        self.config["brain.neuron_count"] = count
        self.config["brain.region_count"] = region_count
        # Calculate synapse density based on neuron count
        self.config["brain.synapse_density"] = min(0.2, count / 5000)
        return self
    
    def enable_all_learning(self):
        """Enable all learning mechanisms."""
        self.config["plasticity.stdp.enable"] = True
        self.config["plasticity.hebbian.enable"] = True
        self.config["plasticity.structural.enable"] = True
        self.config["neuromod.curiosity.enable"] = True
        self.config["neuromod.novelty.enable"] = True
        return self
    
    def create_brain(self):
        """Create brain with current configuration."""
        return pynlm.createBrain(self.config)
    
    def get_summary(self):
        """Get configuration summary."""
        return f"CustomConfig: {self.config.summary()}"

# Usage
custom_config = CustomConfig().set_neurons(2000).enable_all_learning()
brain = custom_config.create_brain()
print(custom_config.get_summary())
```

### 10.3 Custom World Implementation

```python
import pynlm

class CustomWorld(pynlm.SimpleWorld):
    """Custom world with additional features."""
    
    def __init__(self):
        super().__init__()
        self.event_log = []
        self.object_history = []
    
    def add_special_object(self, x, y, obj_type, special_value):
        """Add special object with additional properties."""
        # Create standard world object
        world_obj = nlm.WorldObject(x, y, obj_type, special_value, 0.5f)
        self.addObject(world_obj)
        
        # Log event
        self.event_log.append({
            'type': 'add_object',
            'x': x,
            'y': y,
            'obj_type': obj_type,
            'value': special_value,
            'timestamp': self.getSimulationTime()
        })
        
        # Record in history
        self.object_history.append({
            'action': 'add',
            'object': world_obj,
            'time': self.getSimulationTime()
        })
    
    def get_object_history(self, since_time=0):
        """Get object history since given time."""
        return [h for h in self.object_history if h['time'] >= since_time]
    
    def get_events(self, event_type=None, since_time=0):
        """Get events of specific type since given time."""
        if event_type:
            return [e for e in self.event_log 
                   if e['type'] == event_type and e['timestamp'] >= since_time]
        return [e for e in self.event_log if e['timestamp'] >= since_time]

# Usage
custom_world = CustomWorld()
custom_world.configureSimple(10, 10)
custom_world.add_special_object(2.0, 2.0, pynlm.WorldObjectType.Resource, 10.0)
print(f"Events: {custom_world.get_events()}")
```

## Step 11: Migration Guide

### From Old to New API

#### Configuration

**Old:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
value = config.get("brain.neuron_count")
```

**New:**
```python
config = pynlm.createDefaultConfig()
config["brain.neuron_count"] = 1000
value = config["brain.neuron_count"]
```

#### Brain Initialization

**Old:**
```python
success = brain.initialize()
```

**New:**
```python
success = brain.initializeWithValidation()
```

#### Agent Brain

**Old:**
```python
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

**New:**
```python
agent.enableAllSubsystems()
```

#### World Configuration

**Old:**
```python
world.configure(20, 20, 8, 8)
```

**New:**
```python
world.configureSimple(20, 20)
```

## Step 12: Advanced Examples

### Complete Advanced Example

```python
import pynlm
import time
import json

def advanced_nlm_simulation():
    """Complete advanced NLM simulation example."""
    
    # 1. Create adaptive configuration based on system resources
    print("Creating adaptive configuration...")
    config = create_adaptive_config()
    
    # 2. Create NLM components
    print("Initializing NLM components...")
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # 3. Configure world
    world.configure(config["world.width"], config["world.height"],
                   config["world.vision_width"], config["world.vision_height"])
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize all components
    print("Initializing brain...")
    brain.initialize()
    
    print("Initializing agent...")
    agent.initialize(world)
    
    # 5. Enable all subsystems for full functionality
    print("Enabling all subsystems...")
    agent.enableAllSubsystems()
    
    # 6. Setup performance monitoring
    print("Setting up performance monitoring...")
    performance_data = {
        'steps': [],
        'neuron_counts': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'step_times': []
    }
    
    start_time = time.time()
    
    # 7. Run simulation with episode management
    print("Starting simulation...")
    with pynlm.episode_context(brain, agent, world) as episode:
        for step in range(100):
            step_start = time.time()
            
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Decode motor command
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = calculate_reward(world, agent)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record performance metrics
            step_time = time.time() - step_start
            performance_data['steps'].append(step)
            performance_data['neuron_counts'].append(brain.getTotalNeuronCount())
            performance_data['firing_rates'].append(brain.getAverageFiringRate())
            performance_data['curiosity_levels'].append(agent.getCuriosityLevel())
            performance_data['novelty_levels'].append(agent.getNoveltyLevel())
            performance_data['step_times'].append(step_time)
            
            # Print progress
            if step % 20 == 0:
                print(f"  Step {step}: "
                      f"Neurons={brain.getTotalNeuronCount()}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
    
    # 8. Calculate final statistics
    total_time = time.time() - start_time
    
    print("\n=== Simulation Complete ===")
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Steps per second: {100/total_time:.2f}")
    
    # 9. Generate performance report
    print("\n=== Performance Report ===")
    if performance_data['step_times']:
        avg_step_time = sum(performance_data['step_times']) / len(performance_data['step_times'])
        max_step_time = max(performance_data['step_times'])
        
        print(f"Average step time: {avg_step_time:.4f}s")
        print(f"Max step time: {max_step_time:.4f}s")
        print(f"Min step time: {min(performance_data['step_times']):.4f}s")
    
    print(f"\nFinal statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Final average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Final developmental stage: {agent.getDevelopmentalStage()}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    
    # 10. Save performance data
    with open("simulation_performance.json", 'w') as f:
        json.dump(performance_data, f, indent=2)
    
    print("\nPerformance data saved to simulation_performance.json")
    
    return performance_data

def calculate_reward(world, agent):
    """Calculate reward based on agent state and world state."""
    reward = 0.0
    
    # Energy-based reward
    body = world.getAgentBody()
    if body.energy > 0:
        reward += body.energy * 0.1
    
    # Curiosity-based reward
    curiosity = agent.getCuriosityLevel()
    reward += curiosity * 0.05
    
    # Novelty-based reward
    novelty = agent.getNoveltyLevel()
    if novelty > 0.5:
        reward += novelty * 0.2
    
    return reward

# Run the advanced simulation
if __name__ == "__main__":
    advanced_nlm_simulation()
```

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface for neural network simulation with the following key improvements:

### Key Improvements

1. **Custom Exception System**: Comprehensive error handling with specific exception types
2. **Pythonic APIs**: Intuitive, Pythonic APIs with support for [] operators and method chaining
3. **Configuration Presets**: Multiple configuration presets for different use cases
4. **Episode Context Management**: Context managers for better simulation episode management
5. **Enhanced Debugging**: Performance monitoring and profiling utilities
6. **Advanced Configuration**: Dynamic configuration adaptation and validation
7. **Comprehensive Testing**: Extensive test suite covering all new features
8. **Integration Support**: Integration with ML frameworks and data science workflows

### Usage Scenarios

- **Research**: Advanced neural network simulation with full control over all parameters
- **Development**: Development and testing with debug configuration
- **Production**: Performance-optimized deployment with production configuration
- **Education**: Learning and teaching with comprehensive documentation
- **Research**: Data collection and analysis with built-in data pipeline

### Benefits

- **Ease of Use**: Pythonic APIs make NLM accessible to Python developers
- **Flexibility**: Multiple configuration options support various use cases
- **Robustness**: Comprehensive error handling and validation
- **Performance**: Optimized configurations for different scenarios
- **Extensibility**: Easy to extend with custom classes and functionality
- **Documentation**: Comprehensive documentation with examples

The enhanced NLM Python bindings make advanced neural network simulation accessible to Python developers while maintaining the power and flexibility of the underlying C++ implementation.

## Acknowledgments

This enhanced documentation and feature set builds upon the original NLM project and contributes to the open source neuroscience simulation community. Special thanks to the original NLM developers and contributors.

---

*This documentation covers all enhancements in NLM Python bindings version 0.1.0.*

## Quick Links

- [Installation Guide](#step-1-install-system-requirements)
- [Build Script](#step-3-build-the-enhanced-python-bindings)
- [Basic Usage](#step-4-verify-installation)
- [Advanced Features](#step-10-advanced-features-and-customization)
- [Troubleshooting](#step-9-troubleshooting-and-common-issues)
- [API Reference](#enhanced-features)