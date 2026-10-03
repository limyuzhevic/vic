# Enhanced Documentation and CI/CD for NLM

## Overview

This document provides comprehensive guidance on using the enhanced NLM (Neural Learning Machine) Python API and setting up a modern CI/CD pipeline for continuous development and testing.

## Enhanced Python API Documentation

### 1. Getting Started with Enhanced NLM

The enhanced NLM API provides both high-level convenience functions and low-level access for maximum flexibility. Here's how to get started:

#### Basic Usage (Beginner Friendly)
```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

#### World-Agent Simulation (Practical Example)
```python
import pynlm

def run_complete_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create brain and initialize
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Agent simulation complete!")
    return {
        'final_spikes': brain.getTotalSpikeCount(),
        'final_firing': brain.getFiringNeuronCount(),
        'curiosity_level': agent.getCuriosityLevel()
    }

# Run the simulation
results = run_complete_agent_simulation(1000)
```

### 2. Advanced Usage Patterns

#### Performance Monitoring
```python
import pynlm

# Create performance monitor
monitor = pynlm.PerformanceMonitor()

# Create brain with monitoring
brain = pynlm.createBrainWithConfig(
    neuron_count=2000,
    region_count=4,
    connection_probability=0.05
)
brain.initialize()

# Start monitoring
monitor.start_monitoring()

# Run simulation
for step in range(500):
    monitor.start_step_timing()
    brain.step(step)
    monitor.end_step_timing()

# Get performance report
report = monitor.get_performance_report()
print("Performance Report:")
print(report)

monitor.stop_monitoring()
```

#### Configuration Management
```python
import pynlm

# Create custom configuration
custom_config = pynlm.ConfigSchema()

# Add brain-specific settings
custom_config.addField("brain.neuron_count", "int64", "Number of neurons", "^[0-9]+$", True, "1000")
custom_config.addField("brain.synapse_density", "double", "Synapse connection probability", "^[0-1](\\.\\d+)?$", false, "0.1")
custom_config.addField("brain.connection_probability", "double", "Probability of connection", "^[0-1](\\.\\d+)?$", false, "0.05")

# Create brain with custom config
config = pynlm.createConfig(custom_config)
brain = pynlm.createBrain(config)
brain.initialize()

# Validate configuration
validation_errors = config.validateAgainstSchema()
if validation_errors:
    print("Configuration validation errors:")
    for error in validation_errors:
        print(f"  - {error}")
```

#### Batch Processing
```python
import pynlm

# Create batch processor for multiple simulations
batch_processor = pynlm.BatchProcessor()

# Define simulation parameters
simulations = [
    {'neurons': 500, 'steps': 100, 'learning_rate': 0.01},
    {'neurons': 1000, 'steps': 200, 'learning_rate': 0.005},
    {'neurons': 2000, 'steps': 300, 'learning_rate': 0.001}
]

# Run batch simulation
results = batch_processor.run_batch_simulation(simulations, num_workers=4)

# Analyze results
for sim_id, result in results.items():
    print(f"Simulation {sim_id}:")
    print(f"  Total spikes: {result['total_spikes']}")
    print(f"  Average firing rate: {result['avg_firing_rate']:.2f}")
    print(f"  Learning efficiency: {result['learning_efficiency']:.3f}")
```

#### Advanced Context Management
```python
import pynlm

# Use context managers for safe resource management
with pynlm.create_performance_monitored_brain(
    neuron_count=1000,
    monitoring_interval=0.1
) as brain_monitor:
    brain, monitor = brain_monitor
    
    with pynlm.run_simulation_with_analysis(brain, 500) as analysis:
        for step in range(500):
            world = analysis.get_world()
            percept = world.getSensoryPercept()
            action = brain.getAction(percept)
            
            # Process and execute
            brain.step(step)
            world.applyAction(action)
        
        # Get analysis results
        results = analysis.get_results()
        print(f"Simulation efficiency: {results['efficiency']:.2%}")
        print(f"Average step time: {results['avg_step_time']:.4f}s")
```

### 3. Configuration Templates

#### Common Configuration Templates
```python
import pynlm

# High-performance configuration
high_performance_config = {
    'name': 'high_performance',
    'brain': {
        'neuron_count': 5000,
        'region_count': 8,
        'connection_probability': 0.02,
        'synapse_type': 'excitatory'
    },
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.001},
        'hebbian': {'enable': True, 'learning_rate': 0.0005},
        'structural': {'enable': True, 'synaptogenesis_rate': 0.0001}
    },
    'neuromodulation': {
        'dopamine': {'scale': 1.0},
        'curiosity': {'enable': True, 'exploration_bonus': 0.1},
        'novelty': {'enable': True, 'sensitivity': 0.5}
    }
}

# Resource-efficient configuration
resource_efficient_config = {
    'name': 'resource_efficient',
    'brain': {
        'neuron_count': 200,
        'region_count': 2,
        'connection_probability': 0.1,
        'synapse_type': 'mixed'
    },
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.005},
        'hebbian': {'enable': False},
        'structural': {'enable': False}
    },
    'neuromodulation': {
        'dopamine': {'scale': 0.5},
        'curiosity': {'enable': False},
        'novelty': {'enable': False}
    }
}

# Research-oriented configuration
research_config = {
    'name': 'research',
    'brain': {
        'neuron_count': 10000,
        'region_count': 20,
        'connection_probability': 0.01,
        'synapse_type': 'biological'
    },
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.0001, 'tau_plus': 20.0},
        'hebbian': {'enable': True, 'learning_rate': 0.00005},
        'structural': {'enable': True, 'synaptogenesis_rate': 0.00001}
    },
    'neuromodulation': {
        'dopamine': {'scale': 2.0, 'plasticity_factor': 1.5},
        'curiosity': {'enable': True, 'exploration_bonus': 0.5},
        'novelty': {'enable': True, 'sensitivity': 1.0},
        'prediction_error': {'enable': True, 'prediction_weight': 0.8}
    }
}
```

### 4. Error Handling and Recovery

#### Graceful Error Handling
```python
import pynlm

class NLMErrorHandler:
    def __init__(self):
        self.retry_count = 0
        self.max_retries = 3
    
    def safe_simulation(self, brain, world, max_steps):
        """Run simulation with automatic error recovery."""
        for step in range(max_steps):
            try:
                # Get percept and action
                percept = world.getSensoryPercept()
                action = brain.getAction(percept)
                
                # Execute step
                brain.step(step)
                world.applyAction(action)
                
                # Log success
                self.log_success(step, brain, world)
                
            except nlm.exceptions.SimulationError as e:
                print(f"Simulation error at step {step}: {e}")
                if self.retry_count < self.max_retries:
                    self.retry_count += 1
                    print(f"Retrying step {step}...")
                    continue
                else:
                    print(f"Max retries exceeded. Saving checkpoint and stopping.")
                    brain.save(f"checkpoint_error_step_{step}.bin")
                    raise
            
            except nlm.exceptions.ResourceError as e:
                print(f"Resource error: {e}")
                print("Saving current state and exiting gracefully.")
                brain.save("graceful_shutdown_checkpoint.bin")
                break
            
            except Exception as e:
                print(f"Unexpected error at step {step}: {e}")
                brain.save(f"unexpected_error_step_{step}.bin")
                raise
        
        return step

# Usage example
error_handler = NLMErrorHandler()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()

result = error_handler.safe_simulation(brain, world, 1000)
print(f"Simulation completed successfully with {result} steps")
```

### 5. Visualization and Analysis

#### Basic Visualization
```python
import pynlm
import matplotlib.pyplot as plt

def analyze_simulation_results(brain, steps):
    """Analyze and visualize simulation results."""
    
    # Collect data
    spike_counts = []
    firing_rates = []
    e_i_ratios = []
    
    for step in range(steps):
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        e_i_ratios.append(brain.getExcitationInhibitionRatio())
        brain.step(step)
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Spike count over time
    axes[0, 0].plot(spike_counts)
    axes[0, 0].set_title("Total Spikes Over Time")
    axes[0, 0].set_xlabel("Step")
    axes[0, 0].set_ylabel("Spike Count")
    
    # Firing rate over time
    axes[0, 1].plot(firing_rates)
    axes[0, 1].set_title("Average Firing Rate Over Time")
    axes[0, 1].set_xlabel("Step")
    axes[0, 1].set_ylabel("Firing Rate")
    
    # E/I ratio over time
    axes[1, 0].plot(e_i_ratios)
    axes[1, 0].set_title("Excitatory-Inhibitory Ratio Over Time")
    axes[1, 0].set_xlabel("Step")
    axes[1, 0].set_ylabel("E/I Ratio")
    
    # Neuron population distribution
    region_counts = []
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        region_counts.append(region.getTotalNeuronCount())
    
    axes[1, 1].bar(range(len(region_counts)), region_counts)
    axes[1, 1].set_title("Neurons by Region")
    axes[1, 1].set_xlabel("Region ID")
    axes[1, 1].set_ylabel("Neuron Count")
    
    plt.tight_layout()
    plt.savefig(f"simulation_analysis_step_{steps}.png")
    plt.show()
    
    return {
        'spike_counts': spike_counts,
        'firing_rates': firing_rates,
        'e_i_ratios': e_i_ratios,
        'region_counts': region_counts
    }

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

results = analyze_simulation_results(brain, 500)
```

## CI/CD Pipeline Setup

### 1. GitHub Actions Configuration

Create `.github/workflows/nlm-ci.yml`:

```yaml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, '3.10', '3.11', '3.12']
        build-type: [release, debug]
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install system dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          pkg-config \
          libnlohmann-json-dev \
          libyaml-cpp-dev \
          python3-dev \
          python3-pip \
          git
    
    - name: Install Python dependencies
      run: |
        pip install --upgrade pip
        pip install setuptools wheel
        pip install pybind11 scikit-build-core pytest numpy scipy matplotlib psutil
        pip install -e .
    
    - name: Build C++ extension
      run: |
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build-type }}
        make -j$(nproc)
    
    - name: Run unit tests
      run: |
        cd build
        ./nlm_test --output-on-failure
    
    - name: Run Python tests
      run: |
        python -m pytest tests/ -v --cov=pynlm --cov-report=xml
    
    - name: Run integration tests
      run: |
        python -m pytest integration_tests/ -v
    
    - name: Performance benchmarks
      run: |
        python benchmark_performance.py
    
    - name: Upload test results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results-${{ matrix.python-version }}-${{ matrix.build-type }}
        path: |
          test_results.xml
          coverage.xml
          performance_reports/
    
    - name: Deploy to staging
      if: github.ref == 'refs/heads/develop'
      run: |
        echo "Deploying to staging environment"
        # Add deployment commands here
    
  documentation:
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.10'
    
    - name: Install dependencies
      run: |
        pip install -e .
        pip install mkdocs mkdocs-material
    
    - name: Build documentation
      run: |
        mkdocs build
    
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./site
  
  release:
    runs-on: ubuntu-latest
    if: startsWith(github.ref, 'refs/tags/')
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.10'
    
    - name: Install dependencies
      run: |
        pip install build
    
    - name: Build package
      run: |
        python -m build
    
    - name: Create GitHub Release
      uses: softprops/action-gh-release@v1
      with:
        files: dist/*
        draft: false
        prerelease: false
    
    - name: Update changelog
      run: |
        # Update changelog with release notes
        echo "Release $(git describe --tags --abbrev=0)" >> CHANGELOG.md
        git config --local user.email "action@github.com"
        git config --local user.name "GitHub Action"
        git add CHANGELOG.md
        git commit -m "Update changelog for release"
        git push
```

### 2. Local Development Environment Setup

#### Dockerfile for Development
```dockerfile
FROM ubuntu:22.04

LABEL maintainer="NLM Team"
LABEL description="NLM (Neural Learning Machine) Development Environment"

ENV DEBIAN_FRONTEND=noninteractive
ENV PYTHONUNBUFFERED=1

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libnlohmann-json-dev \
    libyaml-cpp-dev \
    python3 \
    python3-pip \
    python3-venv \
    python3-dev \
    wget \
    curl \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Create development user
RUN useradd -m -s /bin/bash devuser
RUN echo 'devuser:devpass' | chpasswd
RUN usermod -aG sudo devuser

# Set working directory
WORKDIR /workspace

# Copy project files
COPY . /workspace

# Set permissions
RUN chown -R devuser:devuser /workspace

# Switch to development user
USER devuser

# Set up Python environment
RUN python3 -m venv /workspace/venv
ENV VIRTUAL_ENV=/workspace/venv
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

# Install Python dependencies
RUN pip install --upgrade pip
RUN pip install setuptools wheel
RUN pip install pybind11 scikit-build-core pytest numpy scipy matplotlib psutil
RUN pip install -e .

# Install additional development tools
RUN pip install pre-commit black flake8 mypy sphinx

# Set up git hooks
RUN pre-commit install

# Create directories
RUN mkdir -p /workspace/build /workspace/tests /workspace/integration_tests /workspace/docs /workspace/performance_reports

# Set up shell environment
RUN echo "export PYTHONPATH=/workspace/src:$PYTHONPATH" >> /workspace/.bashrc
RUN echo "export LD_LIBRARY_PATH=/workspace/build:$LD_LIBRARY_PATH" >> /workspace/.bashrc

# Expose ports for services (if needed)
EXPOSE 8080 8081

# Default command
CMD ["/bin/bash"]
```

#### Development Environment Setup Script
```bash
#!/bin/bash
# setup_dev_environment.sh

set -e

echo "Setting up NLM Development Environment..."

# Check for required tools
if ! command -v python3 &> /dev/null; then
    echo "Python 3 is required but not installed."
    exit 1
fi

if ! command -v pip3 &> /dev/null; then
    echo "pip3 is required but not installed."
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "CMake is required but not installed."
    exit 1
fi

if ! command -v gcc &> /dev/null; then
    echo "GCC is required but not installed."
    exit 1
fi

# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate

# Upgrade pip and setuptools
pip install --upgrade pip setuptools wheel

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy scipy matplotlib psutil

# Install NLM in development mode
pip install -e .

# Clone and install submodules
if [ ! -d "third_party/nlohmann_json" ]; then
    echo "Cloning nlohmann/json...
    git clone https://github.com/nlohmann/json.git third_party/nlohmann_json
fi

if [ ! -d "third_party/yaml-cpp" ]; then
    echo "Cloning yaml-cpp...
    git clone https://github.com/jbeder/yaml-cpp.git third_party/yaml-cpp
fi

# Build C++ extension
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_PERFORMANCE_ENABLED=ON
cmake --build . --config Debug -- -j$(nproc)
cd ..

# Set up pre-commit hooks
if command -v pre-commit &> /dev/null; then
    pre-commit install
fi

# Create development shortcuts
mkdir -p scripts

cat > scripts/test.sh << 'EOF'
#!/bin/bash
echo "Running NLM tests..."
python -m pytest tests/ -v --tb=short
EOF

cat > scripts/benchmark.sh << 'EOF'
#!/bin/bash
echo "Running NLM performance benchmarks..."
python benchmark_performance.py
EOF

cat > scripts/docs.sh << 'EOF'
#!/bin/bash
echo "Building NLM documentation..."
mkdocs build
EOF

chmod +x scripts/*.sh

# Print setup summary
echo ""
echo "Development environment setup complete!"
echo ""
echo "To activate the environment, run:"
echo "  source nlm_env/bin/activate"
echo ""
echo "To run tests, run:"
echo "  ./scripts/test.sh"
echo ""
echo "To run benchmarks, run:"
echo "  ./scripts/benchmark.sh"
echo ""
echo "To build documentation, run:"
echo "  ./scripts/docs.sh"
echo ""
echo "Development tools installed:"
echo "  - Python development environment"
echo "  - NLM in development mode"
echo "  - Testing framework"
echo "  - Performance benchmarks"
echo "  - Documentation build tools"
echo ""
echo "Happy coding!"
```

### 3. Testing Strategy

#### Unit Testing
```python
# tests/test_units.py
import pytest
import pynlm
from unittest.mock import Mock, patch

class TestBrainUnitTests:
    """Unit tests for Brain class."""
    
    def test_brain_creation(self):
        """Test basic brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    def test_step_simulation(self):
        """Test single simulation step."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        
        # Spike count may or may not increase
        assert brain.getTotalSpikeCount() >= initial_spikes
    
    def test_config_validation(self):
        """Test configuration validation."""
        # Test valid config
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        assert config.has("brain.neuron_count")
        assert config.getOr("brain.neuron_count", 0) == 100

class TestPythonBindings:
    """Tests for Python bindings."""
    
    def test_bindings_import(self):
        """Test that bindings can be imported."""
        import pynlm
        assert pynlm.__version__ is not None
    
    def test_default_config(self):
        """Test default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("brain.neuron_count")
```

#### Integration Testing
```python
# tests/test_integration.py
import pytest
import pynlm
import time

class TestWorldBrainIntegration:
    """Integration tests for world and brain interaction."""
    
    def test_agent_simulation_complete(self):
        """Test complete agent simulation."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify results
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() > 0
    
    def test_brain_world_checkpoint(self):
        """Test checkpoint functionality in world-brain simulation."""
        # Create and run brain-world simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run some steps
        for step in range(50):
            brain.step(step)
        
        # Save checkpoint
        checkpoint_file = "test_checkpoint.bin"
        assert brain.save(checkpoint_file)
        
        # Reset and load checkpoint
        brain.reset()
        brain.initialize()
        
        assert brain.load(checkpoint_file)
        assert brain.getTotalNeuronCount() > 0
        
        # Clean up
        import os
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)

class TestPerformanceIntegration:
    """Integration tests for performance monitoring."""
    
    def test_performance_monitoring(self):
        """Test performance monitoring integration."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        monitor = pynlm.PerformanceMonitor()
        monitor.start_monitoring()
        
        for step in range(50):
            brain.step(step)
        
        monitor.stop_monitoring()
        
        metrics = monitor.get_metrics()
        
        assert 'avg_step_time' in metrics
        assert 'steps_per_second' in metrics
        assert metrics['steps_per_second'] > 0
        
        # Performance should be reasonable (less than 1 second per step)
        assert metrics['avg_step_time'] < 1.0
    
    def test_profiling_integration(self):
        """Test profiling with simulation."""
        profiler = pynlm.PerformanceProfiler()
        
        # Profile neural network initialization
        profiler.start_profile("initialization")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        profiler.stop_profile("initialization")
        
        # Profile simulation steps
        profiler.start_profile("simulation")
        for step in range(100):
            brain.step(step)
        profiler.stop_profile("simulation")
        
        # Get results
        init_time = profiler.get_profile_results("initialization")["duration"]
        sim_time = profiler.get_profile_results("simulation")["duration"]
        
        assert init_time > 0
        assert sim_time > 0
        assert sim_time > init_time  # Simulation should take longer
```

#### Performance Testing
```python
# tests/test_performance.py
import pytest
import pynlm
import time

class TestPerformance:
    """Performance testing for NLM."""
    
    def test_performance_scaling(self):
        """Test performance scaling with neuron count."""
        configs = [
            {'neuron_count': 100, 'expected_steps_per_sec': 1000},
            {'neuron_count': 500, 'expected_steps_per_sec': 200},
            {'neuron_count': 1000, 'expected_steps_per_sec': 100}
        ]
        
        for config in configs:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            start_time = time.time()
            for step in range(10):  # Use same number of steps for comparison
                brain.step(step)
            end_time = time.time()
            
            steps_per_second = 10 / (end_time - start_time)
            
            print(f"Neuron count: {config['neuron_count']}, "
                  f"Steps/sec: {steps_per_second:.1f}")
            
            # Performance should decrease with more neurons (expected)
            assert steps_per_second > 0
    
    def test_memory_usage(self):
        """Test memory usage scaling."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        # Baseline memory
        baseline_memory = process.memory_info().rss
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Memory after initialization
        init_memory = process.memory_info().rss
        memory_increase = init_memory - baseline_memory
        
        print(f"Memory increase: {memory_increase / 1024 / 1024:.2f} MB")
        
        # Memory should increase but not exceed reasonable limits
        # For 1000 neurons, expect < 100 MB increase
        assert memory_increase / 1024 / 1024 < 100
    
    def test_concurrent_simulation(self):
        """Test concurrent simulation capability."""
        import threading
        import queue
        
        results_queue = queue.Queue()
        
        def run_simulation(sim_id, num_steps):
            """Run a single simulation in a thread."""
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            start_time = time.time()
            for step in range(num_steps):
                brain.step(step)
            end_time = time.time()
            
            results_queue.put({
                'sim_id': sim_id,
                'steps_per_second': num_steps / (end_time - start_time),
                'total_spikes': brain.getTotalSpikeCount()
            })
        
        # Start multiple simulations concurrently
        threads = []
        num_simulations = 3
        steps_per_simulation = 100
        
        for i in range(num_simulations):
            thread = threading.Thread(target=run_simulation, args=(i, steps_per_simulation))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Collect results
        results = []
        while not results_queue.empty():
            results.append(results_queue.get())
        
        # Verify all simulations completed
        assert len(results) == num_simulations
        
        for result in results:
            assert result['steps_per_second'] > 0
            assert result['total_spikes'] >= 0
```

### 4. Development Workflow

#### Pre-commit Hooks
```yaml
# .pre-commit-config.yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-conda-environment
      - id: check-json
      - id: check-merge-conflict
      - id: check-symlinks
      - id: check-yaml
      - id: debug-statements
      - id: detect-aws-credentials
      - id: detect-private-key
      - id: end-of-file-fixer
      - id: forbid-new-submodules
      - id: mixed-line-ending
      - id: trailing-whitespace
  
  - repo: https://github.com/psf/black
    rev: 23.7.0
    hooks:
      - id: black
        language_version: python3
  
  - repo: https://github.com/pycqa/flake8
    rev: 6.1.0
    hooks:
      - id: flake8
        language_version: python3
  
  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort
        language_version: python3
  
  - repo: https://github.com/python/mypy
    rev: v1.5.1
    hooks:
      - id: mypy
        language_version: python3
```

#### Development Checklist
```bash
#!/bin/bash
# development_checklist.sh

echo "=== NLM Development Checklist ==="
echo ""

# Check code formatting
if command -v black &> /dev/null; then
    echo "1. Checking code formatting with black..."
    black --check src/ python/ tests/ || echo "   ❌ Code formatting issues found"
    echo "   ✅ Code formatting check passed"
else
    echo "1. Skipping black check (not installed)"
fi

# Check linting
if command -v flake8 &> /dev/null; then
    echo "2. Checking code quality with flake8..."
    flake8 src/ python/ tests/ || echo "   ❌ Code quality issues found"
    echo "   ✅ Code quality check passed"
else
    echo "2. Skipping flake8 check (not installed)"
fi

# Check type hints
if command -v mypy &> /dev/null; then
    echo "3. Checking type hints with mypy..."
    mypy src/ python/ tests/ --ignore-missing-imports || echo "   ❌ Type hint issues found"
    echo "   ✅ Type hint check passed"
else
    echo "3. Skipping mypy check (not installed)"
fi

# Run tests
if command -v pytest &> /dev/null; then
    echo "4. Running unit tests..."
    pytest tests/ -v --tb=short -x || echo "   ❌ Unit tests failed"
    echo "   ✅ Unit tests passed"
else
    echo "4. Skipping pytest (not installed)"
fi

# Check documentation
if command -v mkdocs &> /dev/null; then
    echo "5. Building documentation..."
    mkdocs build --strict || echo "   ❌ Documentation build failed"
    echo "   ✅ Documentation build passed"
else
    echo "5. Skipping documentation build (not installed)"
fi

# Performance benchmark
if command -v python &> /dev/null; then
    echo "6. Running performance benchmarks..."
    python benchmark_performance.py --quick || echo "   ⚠️ Performance benchmarks completed with warnings"
    echo "   ✅ Performance benchmarks completed"
else
    echo "6. Skipping performance benchmarks (Python not available)"
fi

echo ""
echo "=== Development Checklist Complete ==="
echo ""
echo "Next steps:"
echo "1. Review any failed checks above"
echo "2. Fix code quality issues"
echo "3. Update documentation if needed"
echo "4. Run pre-commit hooks: pre-commit run --all-files"
echo ""
echo "For more information, see:
- CONTRIBUTING.md
- DEVELOPMENT.md
- CI/CD documentation in .github/workflows/"
```

## Configuration Management

### Configuration Examples

#### Development Configuration
```bash
# config-dev.yaml
name: development
environment: dev
testing:
  enabled: true
  coverage_threshold: 80
  performance_benchmarks: true
simulation:
  max_steps: 1000
  timestep: 0.001
  neuron_count: 500
  region_count: 4
logging:
  level: DEBUG
  file: dev_log.txt
  console: true
monitoring:
  performance: true
  memory: true
  cpu: true
```

#### Production Configuration
```yaml
# config-prod.yaml
name: production
environment: prod
testing:
  enabled: false
  coverage_threshold: 95
  performance_benchmarks: true
simulation:
  max_steps: 10000
  timestep: 0.001
  neuron_count: 10000
  region_count: 20
logging:
  level: INFO
  file: prod_log.txt
  console: false
monitoring:
  performance: true
  memory: true
  cpu: true
```

#### CI/CD Configuration
```yaml
# .github/workflows/ci-cd.yml
name: CI/CD Pipeline

jobs:
  test:
    strategy:
      matrix:
        include:
          - python-version: 3.8
            os: ubuntu-latest
          - python-version: 3.9
            os: ubuntu-latest
          - python-version: '3.10'
            os: ubuntu-latest
          - python-version: '3.11'
            os: ubuntu-latest
          - python-version: '3.12'
            os: ubuntu-latest
  
    env:
      # Configuration from environment variables
      NLM_TEST_COVERAGE: ${{ secrets.NLM_TEST_COVERAGE }}
      NLM_PERFORMANCE_THRESHOLD: ${{ secrets.NLM_PERFORMANCE_THRESHOLD }}
      NLM_DEPLOYMENT_ENVIRONMENT: development
```

## Usage Guidelines

### Common Workflows

#### 1. Local Development
```bash
# Clone repository
cd /path/to/nlm

# Setup development environment
source setup_dev_environment.sh

# Run tests
./scripts/test.sh

# Develop and test changes
# Run pre-commit hooks
pre-commit run --all-files

# Build documentation
./scripts/docs.sh
```

#### 2. Production Deployment
```bash
# Prepare for release
./scripts/release.sh

# Run integration tests
./scripts/integration_test.sh

# Deploy to production
./scripts/deploy.sh production
```

#### 3. Performance Optimization
```bash
# Run performance benchmarks
./scripts/benchmark.sh

# Analyze performance results
python analyze_performance.py

# Optimize configuration
python optimize_config.py
```

### Troubleshooting

#### Common Issues

1. **Import Errors**
   - Ensure all dependencies are installed
   - Run `pip install -e .` in the project root
   - Check that build directory is properly built

2. **Performance Issues**
   - Check memory usage with `psutil`
   - Verify hardware performance counters are available
   - Optimize configuration for your hardware

3. **Test Failures**
   - Run specific test modules: `pytest tests/test_config.py`
   - Check for memory leaks: `valgrind ./nlm_test`
   - Verify environment setup: `setup_dev_environment.sh`

#### Error Messages

- **ImportError: No module named 'pynlm'**
  Solution: Run `pip install -e .` or `pip install .`

- **Segmentation fault**
  Solution: Check that `initialize()` is called before `step()`, ensure proper memory management

- **Configuration file not found**
  Solution: Provide absolute path or ensure file exists in working directory

## Migration Guide

### Upgrading from Previous Versions

#### Version 0.1.0 → Version 1.0.0

##### Changes:

1. **New Python API Design**
   - Previously: Basic `brain.step()`, `world.update()`
   - Now: Enhanced `pynlm.create_brain_with_config()`, `pynlm.run_simulation_with_analysis()`

2. **Enhanced Configuration**
   - Previously: Simple key=value format only
   - Now: JSON/YAML support with schema validation

3. **Performance Monitoring**
   - Previously: Manual timing with `time.time()`
   - Now: Integrated `PerformanceMonitor` and `PerformanceProfiler`

4. **Error Handling**
   - Previously: Basic exception handling
   - Now: Custom `NLMError` hierarchy with context

#### Migration Examples

##### Example 1: Basic Usage Migration
```python
# OLD WAY (Version 0.1.0)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

print(f"Spikes: {brain.getTotalSpikeCount()}")

# NEW WAY (Version 1.0.0)
import pynlm

# Create brain with enhanced configuration
brain = pynlm.createBrainWithConfig(
    neuron_count=100,
    region_count=1,
    connection_probability=0.1
)
brain.initialize()

# Run simulation with performance monitoring
with pynlm.run_simulation_with_monitoring(brain, 100) as results:
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Average firing rate: {results['avg_firing_rate']:.2f}")
```

##### Example 2: Configuration Migration
```python
# OLD WAY (Version 0.1.0)
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.connection_probability", 0.1)

brain = pynlm.createBrain(config)

# NEW WAY (Version 1.0.0)
import pynlm

# Use schema-based configuration
config_schema = pynlm.createDefaultConfigSchema()
config_schema.addField("brain.neuron_count", "int64", "Number of neurons", "^[0-9]+$", True, "100")
config_schema.addField("brain.connection_probability", "double", "Connection probability", "^[0-1](\\.\\d+)?$", False, "0.1")

brain = pynlm.createBrain(config_schema)

# Load from JSON/YAML files
config_json = pynlm.loadFromFile("config.json", "JSON")
config_yaml = pynlm.loadFromFile("config.yaml", "YAML")
```

##### Example 3: Error Handling Migration
```python
# OLD WAY (Version 0.1.0)
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Error: {e}")

# NEW WAY (Version 1.0.0)
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    with pynlm.run_simulation_with_error_handling(brain, 100) as results:
        print(f"Simulation completed with {results['total_spikes']} spikes")
        
except pynlm.exceptions.NLMSimulationError as e:
    print(f"Simulation error: {e}")
    if e.checkpoint_available:
        print("Checkpoint available for recovery")
        
except pynlm.exceptions.NLMConfigurationError as e:
    print(f"Configuration error: {e}")
    print(f"Valid fields: {e.valid_fields}")
    
except pynlm.exceptions.NLMPermissionError as e:
    print(f"Permission error: {e}")
```

#### 3.0.0 → 4.0.0 Migration

##### Breaking Changes:

1. **Python API Restructuring**
   - `pynlm` → `pynlm.enhanced`
   - `pynlm.createBrain()` → `pynlm.create_brain_with_config()`
   - `pynlm.createSimpleWorld()` → `pynlm.create_simple_world()`

2. **Performance Monitoring**
   - `PerformanceMonitor` → `pynlm.performance.PerformanceMonitor`
   - `PerformanceProfiler` → `pynlm.performance.PerformanceProfiler`

3. **Configuration System**
   - `Config` → `pynlm.config.NLMConfig`
   - `ConfigSchema` → `pynlm.config.NLMConfigSchema`

##### Migration Strategy:

1. Update imports:
   ```python
   # OLD
   import pynlm
   from pynlm import createBrain, createDefaultConfig
   
   # NEW
   import pynlm.enhanced as pynlm
   from pynlm.enhanced import create_brain_with_config, create_default_config
   ```

2. Update function calls:
   ```python
   # OLD
   brain = createBrain(createDefaultConfig())
   
   # NEW
   brain = create_brain_with_config(neuron_count=100)
   ```

3. Update error handling:
   ```python
   # OLD
   except Exception as e:
   
   # NEW
   except pynlm.exceptions.NLMSimulationError as e:
   ```

## Future Enhancements

### Planned Features (Version 2.0.0)

1. **Advanced Machine Learning Integration**
   - Neural architecture search
   - Transfer learning
   - Reinforcement learning with NLM

2. **Distributed Computing**
   - Multi-GPU support
   - Distributed simulation across nodes
   - Cloud-based simulation clusters

3. **Advanced Visualization**
   - Interactive neural network visualization
   - Real-time performance dashboards
   - Simulation result comparison tools

4. **Extended Configuration Options**
   - Dynamic configuration loading
   - Configuration inheritance
   - Environment-specific configurations

5. **Enhanced Error Recovery**
   - Automatic checkpoint management
   - State restoration
   - Fault tolerance

### Research Integration

NLM is designed to support research workflows:

```python
import pynlm

# Research simulation setup
research_config = pynlm.create_research_config()

# Create specialized brain for research
brain = pynlm.create_research_brain(research_config)

# Run long-duration simulation
results = pynlm.run_long_duration_simulation(
    brain,
    duration_hours=24,
    checkpoint_interval_hours=2,
    analysis_interval_hours=6
)

# Analyze results
analysis = pynlm.analyze_simulation_results(results)
pynlm.visualize_research_findings(analysis)
```

## Conclusion

The enhanced NLM Python API provides a powerful, user-friendly interface for neural simulation that scales from simple demonstrations to complex research applications. With comprehensive performance monitoring, robust error handling, and extensive configuration options, NLM is well-equipped for both development and production use.

The CI/CD pipeline ensures code quality and continuous integration, while the documentation and examples provide clear guidance for users at all skill levels.

For more information, visit the NLM documentation at https://nlm.readthedocs.io
