#!/usr/bin/env bash

# NLM Quick Start Guide
# =====================
#
# This is a comprehensive quick start guide for NLM (Neural Learning Machine).
# It provides step-by-step instructions for getting started with NLM quickly
# and easily, with examples for both beginners and advanced users.
#
# This guide includes:
# 1. Quick installation setup
# 2. Basic NLM operations
# 3. Common patterns and examples
# 4. Advanced usage tips
# 5. Troubleshooting common issues
#
# Table of Contents:
# 1. Quick Installation
# 2. Basic Operations
# 3. Common Patterns
# 4. Advanced Usage
# 5. Troubleshooting
#
# ================================================

cat << 'EOF'
# ================================================
# NLM QUICK START GUIDE
# ================================================
#
# WELCOME TO NLM!
#
# NLM (Neural Learning Machine) is an experimental artificial developmental brain
# project that creates neural systems that learn from experience through interaction
# with environments.
#
# This guide will help you get started with NLM quickly and easily.
#
# ================================================
# SECTION 1: QUICK INSTALLATION
# ================================================

## Quick Installation

1. Clone the NLM repository:
   git clone https://github.com/nlm-project/nlm.git
   cd nlm

2. Build the C++ core:
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)

3. Install Python bindings:
   cd ../
   pip install pybind11 scikit-build-core
   pip install -e .

4. Verify installation:
   nlm --help
   python -c "import pynlm; print(pynlm.__version__)"

## System Requirements

- C++20 compatible compiler (GCC 10+ or Clang 12+)
- CMake 3.16 or higher
- Python 3.8 or higher
- 2 GB RAM minimum (8 GB recommended)
- 1 GB disk space minimum

## Troubleshooting

If you encounter issues during installation:

1. **Compiler errors**: Update your compiler or install required dependencies:
   # Ubuntu/Debian:
   sudo apt update
   sudo apt install build-essential cmake python3-dev

   # macOS:
   xcode-select --install

2. **Python binding errors**: Ensure pip is updated:
   pip install --upgrade pip setuptools wheel

3. **Memory issues**: Install recommended system packages:
   # Ubuntu/Debian:
   sudo apt install python3-dev libatlas-base-dev

# ================================================
# SECTION 2: BASIC OPERATIONS
# ================================================

## Basic NLM Operations

NLM provides several ways to interact with the neural learning system:

### 2.1 Using the C++ Command Line Interface

**Run the Phase 2 demo (basic neural computation):**
   nlm

**Run Phase 3 demo (world simulation):**
   nlm_phase3_demo

**Run Phase 4 demo (emergent cognition):**
   nlm_phase4_demo

**Run Phase 6 demo (integrated systems):**
   nlm_phase6_demo

### 2.2 Using Python Bindings

**Create and initialize a brain:**
   import pynlm
   
   # Create default configuration
   config = pynlm.createDefaultConfig()
   
   # Create and initialize brain
   brain = pynlm.createBrain(config)
   brain.initialize()

**Run a simple simulation:**
   for step in range(100):
       brain.step(step)
   
   print(f"Firing neurons: {brain.getFiringNeuronCount()}")

**Create agent with world:**
   import pynlm
   
   # Setup brain
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   
   # Setup world
   world = pynlm.createSimpleWorld()
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   world.reset()
   
   # Setup agent
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   
   # Run simulation
   for step in range(500):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())

## Quick Reference

| Command | Description | Use Case |
|----------|-------------|----------|
| `nlm` | Basic neural computation demo | Quick test |
| `nlm_phase3_demo` | World simulation demo | Agent-environment interaction |
| `nlm_phase4_demo` | Emergent cognition demo | Advanced behavior |
| `nlm_phase6_demo` | Integrated systems demo | Complete integration |
| `python -c "import pynlm; print(pynlm.__version__)"` | Check Python version | Verify installation |

# ================================================
# SECTION 3: COMMON PATTERNS
# ================================================

## Common NLM Patterns

NLM follows consistent patterns across different phases:

### 3.1 Basic Brain Pattern

```python
import pynlm

# Pattern: Create -> Initialize -> Step -> Use

def run_brain_simulation(neuron_count=1000, steps=1000):
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Optional: Log progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain
```

### 3.2 Agent-World Pattern

```python
import pynlm

class NLMAgent:
    def __init__(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning capabilities
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    def run_episode(self, max_steps=1000):
        for step in range(max_steps):
            # Update environment
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with brain
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get and apply action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Optional: Get reward and learn
            reward = self.get_reward_from_world()
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Optional: Update development
            self.agent.updateDevelopment(0.1)
            
            # Log progress
            if step % 100 == 0:
                self.log_progress(step)
        
        return self.get_final_stats()
    
    def log_progress(self, step):
        print(f"Step {step}:")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {self.brain.getDevelopmentalStage()}")
```

### 3.3 Memory and Learning Pattern

```python
import pynlm

class NLMMemoryDemo:
    def __init__(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Enable memory systems
        self.memory_demo()
    
    def memory_demo(self):
        """Demonstrate memory system capabilities"""
        
        # Record initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getFiringNeuronCount()
        
        print(f"Initial state:")
        print(f"  Total spikes: {initial_spikes}")
        print(f"  Firing neurons: {initial_neurons}")
        
        # Run simulation with memory
        for step in range(1000):
            # Inject patterns to stimulate memory
            if step % 100 == 0:
                # Create distinct patterns at different steps
                self.create_experience_pattern(step)
            
            self.brain.step(step)
            
            # Record after different phases
            if step == 100 or step == 500 or step == 1000:
                self.record_state(step)
        
        # Analyze memory changes
        self.analyze_memory_changes()
    
    def create_experience_pattern(self, step):
        """Create a distinct experience pattern"""
        # Use different neuron groups for different experiences
        neuron_group = step // 100
        
        if self.brain.getWorkingMemory():
            # Store experience in working memory
            # (This would use actual NLM working memory APIs)
            pass
        
        # Inject different patterns
        for i in range(10):
            self.brain.injectCurrentToNeurons(
                pynlm.NeuronType.Sensory, 
                50.0 * (1.0 + 0.1 * (i % 3))
            )
    
    def record_state(self, step):
        """Record current brain state"""
        state = {
            'step': step,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'working_memory_traces': (
                self.brain.getWorkingMemory().getActiveTraces() 
                if self.brain.getWorkingMemory() else 0
            )
        }
        
        print(f"State at step {step}:")
        for key, value in state.items():
            print(f"  {key}: {value}")
        print()
    
    def analyze_memory_changes(self):
        """Analyze memory system changes"""
        print("Memory analysis complete!")
        # Would contain actual analysis of memory changes
```

# ================================================
# SECTION 4: ADVANCED USAGE
# ================================================

## Advanced NLM Techniques

### 4.1 Configuration Customization

```python
import pynlm

# Create advanced configuration
def advanced_config():
    config = pynlm.createDefaultConfig()
    
    # Optimize for learning
    config.set("brain.neuron_count", 5000)  # More neurons for complex learning
    config.set("brain.synapse_density", 0.2)  # Higher connectivity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("neuromod.dopamine.scale", 2.0)  # Stronger reward signals
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Development and maturation
    config.set("development.enable", True)
    config.set("development.maturation_rate", 0.1)
    
    # Memory capacity
    config.set("memory.working_memory.capacity", 10000)
    config.set("memory.episodic_memory.max_episodes", 5000)
    
    return config
```

### 4.2 Performance Optimization

```bash
# Optimized build configuration
mkdir build_opt
cd build_opt
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp" \
         -DNLM_ENABLE_SSE=1 \
         -DNLM_ENABLE_AVX=1 \
         -DNLM_ENABLE_AVX512=1
cd .. && build_opt make -j$(nproc)

# Performance testing
./nlm_phase6_demo --benchmark --output perf_report.json
```

### 4.3 Advanced Debugging

```python
import pynlm

def debug_neural_network(brain, world=None, agent=None):
    """Advanced debugging of neural network"""
    
    print("=== NEURAL NETWORK DEBUG REPORT ===\n")
    
    # 1. Brain structure analysis
    print("1. BRAIN STRUCTURE ANALYSIS:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Active regions: {brain.getRegionCount()}")
    print(f"   Neuron types: {brain.getNeuronTypeDistribution()}")
    
    # 2. Activity analysis
    print("\n2. ACTIVITY ANALYSIS:")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Current firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Firing neuron percentage: {brain.getFiringNeuronCount() / max(1, brain.getTotalNeuronCount()) * 100:.1f}%")
    
    # 3. Memory system status
    print("\n3. MEMORY SYSTEM STATUS:")
    if brain.getWorkingMemory():
        wm = brain.getWorkingMemory()
        print(f"   Working memory traces: {wm.getActiveTraces()}")
        print(f"   Working memory capacity: {wm.getCapacity()}")
    
    if brain.getEpisodicMemory():
        em = brain.getEpisodicMemory()
        print(f"   Episodes stored: {em.getEpisodeCount()}")
        print(f"   Max episodes: {em.getMaxEpisodes()}")
    
    # 4. Neuromodulation status
    print("\n4. NEUROMODULATION STATUS:")
    if agent:
        print(f"   Dopamine level: {agent.getNeuromodulationLevel():.3f}")
        print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"   Prediction error: {agent.getPredictionError():.3f}")
    
    # 5. Development status
    print("\n5. DEVELOPMENT STATUS:")
    print(f"   Current stage: {brain.getDevelopmentalStage()}")
    print(f"   Developmental age: {brain.getDevelopmentalAge():.1f}%")
    
    # 6. System integration
    print("\n6. SYSTEM INTEGRATION:")
    systems = [
        ("Working Memory", brain.getWorkingMemory() is not None),
        ("Episodic Memory", brain.getEpisodicMemory() is not None),
        ("Neuromodulation", brain.getDopamine() is not None),
        ("Prediction System", brain.getPredictionSystem() is not None),
        ("Development", brain.getDevelopmentSystem() is not None),
        ("Planning", brain.getPlanner() is not None),
        ("Attention", brain.getAttention() is not None),
        ("Concept Formation", brain.getConceptFormation() is not None)
    ]
    
    for system_name, is_active in systems:
        status = "ACTIVE" if is_active else "INACTIVE"
        print(f"   {system_name}: {status}")
    
    # 7. Performance metrics
    print("\n7. PERFORMANCE METRICS:")
    if world:
        print(f"   World size: {world.getWidth()} x {world.getHeight()}")
        print(f"   Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"   Agent energy: {world.getAgentBody().energy:.1f}%")
        print(f"   Simulation time: {world.getSimulationTime():.1f}s")
    
    print("\n=== DEBUG REPORT COMPLETE ===")
    return True
```

### 4.4 Batch Processing

```bash
#!/bin/bash

# NLM Batch Processing Script
# ==========================
#
# Run multiple NLM simulations in parallel for comparative analysis

# Configuration
EXPERIMENTS=(
    "--neurons 1000 --steps 500 --plasticity high"
    "--neurons 2000 --steps 1000 --plasticity medium"
    "--neurons 5000 --steps 2000 --plasticity low"
    "--neurons 10000 --steps 3000 --plasticity none"
)

OUTPUT_DIR="nlm_batch_results"
mkdir -p $OUTPUT_DIR

# Run batch experiments
for i in "${!EXPERIMENTS[@]}"; do
    echo "Running experiment $((i+1)) of ${#EXPERIMENTS[@]}"
    
    # Run nlm with experiment parameters
    ./nlm ${EXPERIMENTS[$i]} \
          --output $OUTPUT_DIR/experiment_$i.bin \
          --log $OUTPUT_DIR/experiment_$i.log \
          --monitor real-time \
          --checkpoint enabled
    
    # Analyze results
    ./nlm_analysis analyze $OUTPUT_DIR/experiment_$i.bin \
                        --output $OUTPUT_DIR/analysis_$i.txt \
                        --format detailed
    
    echo "Experiment $((i+1)) complete"
done

# Generate summary report
echo "Generating summary report..."
./nlm_analysis compare $OUTPUT_DIR/experiment_*.bin \
                   --output $OUTPUT_DIR/summary.txt \
                   --metrics performance,learning,stability

# Clean up (optional)
# rm $OUTPUT_DIR/experiment_*.bin
# rm $OUTPUT_DIR/experiment_*.log

echo "Batch processing complete! Results in $OUTPUT_DIR"
```

# ================================================
# SECTION 5: TROUBLESHOOTING
# ================================================

## Common Issues and Solutions

### 5.1 Installation Problems

**Problem**: "Cannot find Python development headers"
**Solution**:
   # Ubuntu/Debian:
   sudo apt install python3-dev
   
   # macOS:
   xcode-select --install

**Problem**: "CMake cannot find Python"
**Solution**:
   export Python_EXECUTABLE=$(which python3)
   cmake .. -DPython_EXECUTABLE=$Python_EXECUTABLE

**Problem**: "Not enough memory"
**Solution**:
   # Increase swap space or RAM
   sudo swapon --show
   sudo swapoff -a
   sudo fallocate -l 2G /swapfile
   sudo mkswap /swapfile
   sudo swapon /swapfile

### 5.2 Runtime Problems

**Problem**: "Brain initialization failed"
**Solution**:
   Check configuration:
   ```python
   config = pynlm.createDefaultConfig()
   print(f"Neurons: {config.get('brain.neuron_count')}")
   print(f"Regions: {config.get('brain.region_count')}")
   
   brain = pynlm.createBrain(config)
   if not brain.initialize():
       print("Brain initialization failed!")
       return
   ```

**Problem**: "Agent not moving"
**Solution**:
   Check world setup:
   ```python
   world = pynlm.createSimpleWorld()
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   world.reset()
   
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   ```

**Problem**: "Memory usage too high"
**Solution**:
   Reduce configuration:
   ```python
   config.set("brain.neuron_count", 1000)  # Reduce neurons
   config.set("memory.working_memory.capacity", 1000)  # Reduce memory
   ```

### 5.3 Performance Issues

**Problem**: "Simulation too slow"
**Solution**:
   ```bash
   # Build with optimizations
   mkdir build_perf
cd build_perf
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS="-O3 -march=native"
   make -j$(nproc)
   
   # Run with performance monitoring
   ./nlm_phase6_demo --benchmark
   ```

**Problem**: "Memory checkpointing failed"
**Solution**:
   Check disk space and permissions:
   ```bash
   df -h
   du -sh checkpoint_dir
   chmod 755 checkpoint_dir
   ```

### 5.4 Getting Help

**Online Resources**:
- Documentation: https://docs.nlm-project.org
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Community Forum: https://community.nlm-project.org
- Examples: https://examples.nlm-project.org

**Command Line Help**:
   nlm --help
   nlm_phase3_demo --help
   nlm_phase4_demo --help
   nlm_phase6_demo --help

**Python API Help**:
   ```python
   import pynlm
   help(pynlm)
   help(pynlm.createBrain)
   help(pynlm.createDefaultConfig)
   ```

**Debugging Tips**:
   1. Enable verbose logging
   2. Check system resources
   3. Verify configuration
   4. Test with smaller parameters first
   5. Use checkpointing for long simulations

# ================================================
# QUICK START SUMMARY
# ================================================

## For Beginners:

1. **Install**: `bash nlm_setup.sh --quick`
2. **Test**: `nlm` (basic demo)
3. **Learn**: `python -c "import pynlm; print(pynlm.__version__)"`
4. **Explore**: `cat easy_usage.md`

## For Developers:

1. **Build**: `bash nlm_setup.sh --advanced`
2. **Debug**: `bash nlm_analysis.sh debug`
3. **Test**: `./nlm_setup.sh --test`
4. **Document**: `cat HOW_TO_USE.md`

## For Researchers:

1. **Batch Process**: `bash nlm_batch.sh`
2. **Analyze**: `bash nlm_analysis.sh analyze results.bin`
3. **Compare**: `bash nlm_analysis.sh compare run1.bin run2.bin`
4. **Export**: `bash nlm_analysis.sh export results.bin --formats json csv`

# ================================================
# ENJOY EXPLORING NLM!
# ================================================
#
# NLM is a powerful tool for creating artificial developmental brains.
# Take your time to explore the system and understand how different
# components work together to create intelligent behavior.
#
# Remember:
# - Start simple and add complexity gradually
# - Use checkpointing for long simulations
# - Monitor system resources regularly
# - Document your findings and configurations
# - Share your experiences with the community!
#
# Happy brain building! 🤖🧠
EOF