# NLM Troubleshooting Guide

## Overview

This guide provides comprehensive troubleshooting for NLM issues, from basic setup problems to advanced debugging scenarios. It covers common issues, their solutions, and prevention strategies.

## Problem Classification

### 1. Setup and Installation Issues
**Level**: Easy to Fix
**Impact**: Prevents system from running

### 2. Configuration Issues
**Level**: Medium
**Impact**: Affects specific functionality

### 3. Runtime Errors
**Level**: Hard
**Impact**: Prevents specific operations

### 4. Performance Issues
**Level**: Medium
**Impact**: Affects efficiency

### 5. Scientific Issues
**Level**: Hard
**Impact**: Affects research validity

## Common Troubleshooting Scenarios

### Scenario 1: "Brain is not doing anything"
**Problem**: Brain initialization fails or brain steps produce no activity
**Symptoms**:
- `print("Brain initialized")` but no activity observed
- All neurons have 0 firing rate
- No spikes generated during simulation

**Root Causes**:
1. **Missing initialization**: `brain.initialize()` not called
2. **Wrong configuration**: Invalid brain configuration
3. **No input**: No sensory input provided
4. **Network disconnected**: No recurrent connectivity
5. **Parameters too low**: All neurons below threshold

**Solutions**:

#### 1.1 Check Initialization
```python
# Problem: Brain not initialized
brain = pynlm.createBrain(config)
brain.initialize()  # Add this line!

# Problem: World not initialized
world = pynlm.createSimulationWorld(100, 100)
agent.initialize(world)  # Add this line!

# Problem: Agent not initialized
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Add this line!
```

#### 1.2 Check Configuration
```python
# Problem: Invalid configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Explicitly set parameters
config.set("brain.connection_probability", 0.1)
brain = pynlm.createBrain(config)

# Problem: Missing required parameters
if not config.has("brain.neuron_count"):
    config.set("brain.neuron_count", 100)
```

#### 1.3 Check Input and Connectivity
```python
# Problem: No sensory input
percept = world.getSensoryPercept()
if percept is None:
    print("ERROR: No sensory input available")
    return

agent.processSensoryInput(percept)  # Process the input

# Problem: No recurrent connections
# Check if brain has recurrent neurons
region = brain.getRegion(0)
if region.getPopulations().empty():
    print("WARNING: No populations found")
```

#### 1.4 Check Parameters
```python
# Problem: All neurons below threshold
config.set("brain.v_thresh", -55.0)  # Adjust threshold
config.set("brain.v_rest", -65.0)   # Adjust resting potential

# Problem: No plasticity
config.set("plasticity.enabled", true)
```

### Scenario 2: "Agent isn't moving"
**Problem**: Agent processes sensory input but produces no motor commands
**Symptoms**:
- Agent processes percepts correctly
- Brain steps execute without errors
- No motor commands decoded
- World state doesn't change

**Root Causes**:
1. **No action selected**: Brain produces no motor activity
2. **Action ignored**: Action not applied to world
3. **Reward not received**: No feedback loop
4. **Curiosity disabled**: No exploration

**Solutions**:

#### 2.1 Check Brain Activity
```python
# Problem: Brain not producing motor activity
brain_step(brain)  # Run brain step
motor_command = agent.decodeMotorCommand()

if motor_command is None:
    print("ERROR: No motor command decoded")
    print(f"Brain firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
else:
    print(f"Motor command: {motor_command}")
```

#### 2.2 Check Motor System
```python
# Problem: Motor system not initialized
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check if motor neurons exist
brain = agent.getBrain()
motor_region = brain.getRegion("motor")
if motor_region is None:
    print("ERROR: No motor region found")
```

#### 2.3 Check Reward Modality
```python
# Problem: Reward modulation disabled
agent.enableRewardModulation(True)  # Enable reward

# Check reward processing
reward = 1.0  # Example reward
agent.applyRewardModulation(reward, 0.5)  # Apply with prediction
```

#### 2.4 Check Curiosity
```python
# Problem: Curiosity disabled
agent.enableCuriosity(True)  # Enable curiosity for exploration

# Check curiosity level
if agent.getCuriosityLevel() < 0.1:
    print("WARNING: Low curiosity - agent may not explore")
```

### Scenario 3: "Everything is 0"
**Problem**: All metrics and activities are zero
**Symptoms**:
- Total reward = 0
- Firing rate = 0
- Active neurons = 0
- All neural activity = 0

**Root Causes**:
1. **No input**: No sensory input provided
2. **No learning**: No plasticity or learning rules
3. **No recurrence**: No recurrent connections
4. **Too few neurons**: Insufficient population size
5. **Wrong parameters**: Incorrect neural parameters

**Solutions**:

#### 3.1 Check Input
```python
# Problem: No sensory input
percept = world.getSensoryPercept()
if percept is None:
    print("ERROR: No sensory input")
    # Provide some test input
    test_percept = pynlm.Vision(8, 8, 3)
    np.random.seed(42)
    test_data = np.random.randn(8*8*3).astype(float)
    test_percept.setData(test_data)
    agent.processSensoryInput(test_percept)
else:
    agent.processSensoryInput(percept)
```

#### 3.2 Check Learning
```python
# Problem: No learning
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

#### 3.3 Check Recurrence
```python
# Problem: No recurrent connections
# Check if brain has recurrent populations
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    for population in region.getPopulations():
        if population.getType() == PopulationType::Recurrent:
            print(f"Found recurrent population in region {region_id}")
            break
```

#### 3.4 Check Parameters
```python
# Problem: Insufficient neurons
config.set("brain.neuron_count", 1000)  # More neurons

# Problem: Too low connectivity
config.set("brain.connection_probability", 0.05)  # Higher connectivity

# Problem: Threshold too high
config.set("brain.v_thresh", -50.0)  # Lower threshold
```

### Scenario 4: "Memory traces decay quickly"
**Problem**: Memory traces decay too rapidly
**Symptoms**:
- Working memory traces disappear within 1-2 steps
- Episodic memories are not stored
- No long-term retention

**Root Causes**:
1. **No working memory**: Working memory system not integrated
2. **No episodic storage**: Episodic memory not updated
3. **No replay**: No memory consolidation mechanism
4. **Insufficient connectivity**: Weak recurrent connections

**Solutions**:

#### 4.1 Check Working Memory Integration
```python
# Problem: Working memory not connected to brain
if brain.getWorkingMemory() == nullptr:
    print("ERROR: Working memory not integrated")
    # Manually connect working memory
    brain.setWorkingMemory(new NeuralWorkingMemory());

# Check working memory updates
working_memory = brain.getWorkingMemory()
if working_memory:
    active_traces = working_memory.getActiveTraces();
    print(f"Working memory traces: {active_traces}")
```

#### 4.2 Check Episodic Memory
```python
# Problem: Episodic memory not updated
if brain.getEpisodicMemory() == nullptr:
    print("ERROR: Episodic memory not integrated")
    # Manually connect episodic memory
    brain.setEpisodicMemory(new NeuralEpisodicMemory());

# Check episodic storage
episodic_memory = brain.getEpisodicMemory();
if episodic_memory:
    episodes = episodic_memory.getStoredEpisodes();
    print(f"Episodic episodes stored: {episodes}")
```

#### 4.3 Check Replay Mechanism
```python
# Problem: No replay mechanism
# Replay should happen periodically (e.g., every 100 steps)
if brain.step() % 100 != 0:
    print("WARNING: Not time for replay (should be every 100 steps)")

# Check if replay is enabled
if !brain.isReplayEnabled():
    brain.enableReplay(true);
```

### Scenario 5: "Learning is too slow"
**Problem**: Learning progresses too slowly
**Symptoms**:
- Learning curve flat for many steps
- Performance doesn't improve over time
- Low final performance

**Root Causes**:
1. **No reward modulation**: No learning signals
2. **No plasticity**: No weight changes
3. **No curiosity**: No exploration for new experiences
4. **Learning rate too low**: Plasticity too weak

**Solutions**:

#### 5.1 Check Reward Modulation
```python
# Problem: Reward modulation disabled
agent.enableRewardModulation(True)

# Check reward processing
reward = 1.0  # Example reward
agent.applyRewardModulation(reward, 0.5)  # Apply with prediction

# Verify neuromodulation level
if agent.getNeuromodulationLevel() < 0.1:
    print("WARNING: Low neuromodulation level")
```

#### 5.2 Check Plasticity
```python
# Problem: No plasticity
config.set("plasticity.stdp.enable", true)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)

# Check plasticity effectiveness
if !brain.hasPlasticity():
    print("ERROR: No plasticity mechanisms found")
```

#### 5.3 Check Learning Rate
```python
# Problem: Learning rate too low
config.set("plasticity.stdp.learning_rate", 0.01)  # Increase STDP rate
config.set("plasticity.hebbian.learning_rate", 0.05)  # Increase Hebbian rate

# Problem: Learning rate too high
if config.get("plasticity.stdp.learning_rate") > 0.1:
    print("WARNING: High STDP learning rate may cause instability")
```

### Scenario 6: "Performance is unstable"
**Problem**: Performance varies wildly between runs
**Symptoms**:
- Different results with same parameters
- High variance in metrics
- Unreliable experiments

**Root Causes**:
1. **Random seeds**: Different random seeds each run
2. **Stochastic elements**: Random connectivity, activity
3. **No averaging**: Single runs reported
4. **Outliers**: Extreme values affecting results

**Solutions**:

#### 6.1 Check Random Seeds
```python
# Problem: Different random seeds
import random
import numpy as np

# Set consistent seeds
random.seed(42)
np.random.seed(42)
config.set("random.seed", 42)  # Set NLM random seed

# Ensure reproducibility
brain.reset();  # Reset brain state
brain.initialize();  # Re-initialize
```

#### 6.2 Check Stochastic Elements
```python
# Problem: Too many random elements
# Reduce random connectivity
config.set("brain.connection_probability", 0.05)  # Less randomness

# Use deterministic initialization where possible
brain.setInitializationMode(DeterministicInitialization::Full);
```

#### 6.3 Check Results
```python
# Problem: Single run reported
print("WARNING: Reporting single run - consider multiple seeds")
print("Average over 5 seeds: ", calculate_average_over_seeds(5))
```

### Scenario 7: "CUDA not working"
**Problem**: GPU acceleration not working
**Symptoms**:
- `nlm train` shows CPU usage only
- No GPU metrics reported
- Performance lower than expected

**Root Causes**:
1. **No CUDA installation**: CUDA not installed
2. **Driver issues**: Outdated GPU drivers
3. **Compilation errors**: Not compiled with CUDA support
4. **Environment issues**: Wrong environment

**Solutions**:

#### 7.1 Check CUDA Installation
```bash
# Check CUDA installation
cuda --version

# Check CUDA toolkit
ls /usr/local/cuda

# Install CUDA if needed
# Follow NVIDIA installation instructions
```

#### 7.2 Check Environment
```bash
# Check GPU availability
nvidia-smi

# Check environment variables
echo $CUDA_HOME
echo $PATH | grep cuda
```

#### 7.3 Check Build
```bash
# Rebuild with CUDA support
mkdir build
cd build
cmake .. -DNLM_USE_CUDA=ON
make -j4
```

#### 7.4 Check Runtime
```python
# Check if CUDA is available
try:
    import pynlm
    print("CUDA available:", pynlm.cuda_available())
except ImportError:
    print("CUDA not available - using CPU")
```

## Advanced Troubleshooting Techniques

### 1. Debug Mode
```python
# Enable debug mode
import pynlm
pynlm.enable_debug_mode()

# Run with debug output
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    # Debug output
    if step % 10 == 0:
        print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, Active={brain.getActiveNeuronCount()}")
    
    brain.step(step)
```

### 2. State Inspection
```python
# Inspect brain state
brain_state = brain.save("debug_brain.bin")

# Load and inspect
brain2 = pynlm.createBrain(config)
brain2.load("debug_brain.bin")

# Check detailed state
def print_brain_state(brain):
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Check regions
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        print(f"Region {region_id}: {region.getNeuronCount()} neurons")
```

### 3. Performance Debugging
```python
# Performance debugging
import time
import cProfile
import pstats

def debug_performance(brain, world, agent, num_steps=100):
    """Debug performance with timing."""
    start_time = time.time()
    
    # Profile the simulation
    profiler = cProfile.Profile()
    profiler.enable()
    
    for step in range(num_steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        agent.applyRewardModulation(0.1, 0.0)
        
        # Update world
        world.update(0.1)
        agent.updateDevelopment(0.1)
    
    end_time = time.time()
    
    profiler.disable()
    
    # Generate performance report
    print(f"Performance: {num_steps} steps in {end_time - start_time:.2f}s")
    print(f"Steps per second: {num_steps / (end_time - start_time):.0f}")
    
    # Save profiling results
    profiler.dump_stats("performance.prof")
    
    # Generate stats
    stats = pstats.Stats("performance.prof")
    stats.sort_stats('cumulative')
    stats.print_stats(20)
```

### 4. Memory Debugging
```python
# Memory debugging
import tracemalloc

def debug_memory(brain, world, agent, num_steps=100):
    """Debug memory usage."""
    tracemalloc.start()
    
    # Run simulation
    for step in range(num_steps):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        world.update(0.1)
        agent.updateDevelopment(0.1)
    
    # Get memory usage
    current, peak = tracemalloc.get_traced_memory()
    print(f"Current memory usage: {current / 1024 / 1024:.2f} MB")
    print(f"Peak memory usage: {peak / 1024 / 1024:.2f} MB")
    
    # Get detailed statistics
    snapshot = tracemalloc.take_snapshot()
    top_stats = snapshot.statistics('lineno')
    
    print("\nTop memory allocations:")
    for stat in top_stats[:10]:
        print(f"{stat}")
    
    tracemalloc.stop()
```

## Automated Troubleshooting

### 1. Diagnostic Script
```python
#!/usr/bin/env python3
"""
Automated NLM troubleshooting script.
Automatically diagnoses and fixes common NLM issues.
"""

import pynlm
import numpy as np
from pathlib import Path

def check_basic_setup():
    """Check basic setup."""
    print("Checking basic setup...")
    
    # Check imports
    try:
        import pynlm
        print("✓ NLM imports successfully")
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    
    # Check configuration
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    # Check brain creation
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created: {brain.getTotalNeuronCount()} neurons")
    
    return True

def check_simulation(brain, world, agent):
    """Check simulation functionality."""
    print("\nChecking simulation...")
    
    # Run simple simulation
    num_steps = 10
    total_reward = 0.0
    
    for step in range(num_steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        if percept is None:
            print("✗ No sensory input")
            return False
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        if action is None:
            print(f"✗ Step {step}: No motor command")
            print(f"  Brain firing: {brain.getFiringNeuronCount()}")
            return False
        
        # Apply action to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        if result:
            total_reward += result.reward
        
        # Apply reward
        agent.applyRewardModulation(0.1, 0.0)
        
        # Update world
        world.update(0.1)
        agent.updateDevelopment(0.1)
    
    print(f"✓ Simulation completed: {num_steps} steps, total reward: {total_reward:.1f}")
    return True

def check_memory_usage():
    """Check memory usage."""
    print("\nChecking memory usage...")
    
    import tracemalloc
    tracemalloc.start()
    
    # Create and run simple simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimulationWorld(50, 50)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(50):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        world.update(0.1)
        agent.updateDevelopment(0.1)
    
    # Get memory usage
    current, peak = tracemalloc.get_traced_memory()
    
    print(f"Current memory: {current / 1024 / 1024:.2f} MB")
    print(f"Peak memory: {peak / 1024 / 1024:.2f} MB")
    
    if peak > 100 * 1024 * 1024:  # > 100 MB
        print("⚠ Warning: High memory usage")
    
    tracemalloc.stop()
    return True

def check_configuration(config):
    """Check configuration."""
    print("\nChecking configuration...")
    
    issues = []
    
    # Check critical parameters
    if not config.has("brain.neuron_count"):
        issues.append("Missing neuron_count")
    
    if not config.has("brain.connection_probability"):
        issues.append("Missing connection_probability")
    
    if not config.has("plasticity.stdp.enable"):
        issues.append("Missing STDP")
    
    if not config.has("plasticity.hebbian.enable"):
        issues.append("Missing Hebbian")
    
    if issues:
        print(f"✗ Configuration issues: {', '.join(issues)}")
        return False
    else:
        print("✓ Configuration looks good")
        return True

def run_troubleshooting():
    """Run complete troubleshooting."""
    print("NLM Troubleshooting Tool")
    print("=" * 40)
    
    all_passed = True
    
    # Check basic setup
    if not check_basic_setup():
        all_passed = False
    
    # Check configuration
    config = pynlm.createDefaultConfig()
    if not check_configuration(config):
        all_passed = False
    
    # Create full simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimulationWorld(100, 100)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Check simulation
    if not check_simulation(brain, world, agent):
        all_passed = False
    
    # Check memory
    check_memory_usage()
    
    # Summary
    print(f"\n{'=' * 40}")
    if all_passed:
        print("✓ Troubleshooting completed successfully")
    else:
        print("✗ Troubleshooting found issues that need fixing")
    
    return all_passed

if __name__ == "__main__":
    run_troubleshooting()
```

## Common Error Messages and Solutions

### Error: "ImportError: No module named 'pynlm'"
**Solution**:
```bash
# Install NLM
pip install -e .

# Or install from source
python setup.py install
```

### Error: "RuntimeError: Brain not initialized"
**Solution**:
```python
# Add initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Add this!
```

### Error: "Segmentation fault"
**Solution**:
```python
# Check configuration and parameters
brain = pynlm.createBrain(config)
if brain.getTotalNeuronCount() == 0:
    print("ERROR: No neurons created")
    config.set("brain.neuron_count", 100)

brain.initialize()
```

### Error: "High memory usage"
**Solution**:
```python
# Reduce memory usage
config.set("brain.neuron_count", 100)  # Fewer neurons
config.set("brain.region_count", 1)    # Fewer regions

# Use memory pools
config.set("memory.enable_pools", true)
```

### Error: "CUDA not available"
**Solution**:
```bash
# Check CUDA installation
cuda --version

# Rebuild with CUDA
mkdir build
cd build
cmake .. -DNLM_USE_CUDA=ON
make -j4
```

## Support Resources

### Online Resources
- [NLM Documentation](./README.md)
- [NLM GitHub Repository](https://github.com/nstarr/nlm)
- [NLM Mailing List](mailto:nlm-discuss@googlegroups.com)
- [NLM Discord Server](https://discord.gg/nlm)

### Local Support
- Check `docs/FAILURES.md` for known issues
- Look at `examples/` for working examples
- Check `logs/` for error logs
- Use `./nlm --help` for command reference

### Getting Help

#### 1. Search for Solutions
```bash
# Search documentation
grep -r "brain.initialize" docs/

# Search examples
grep -r "pynlm.createBrain" examples/
```

#### 2. Check Error Logs
```bash
# Check recent logs
ls -la logs/

# Check system logs
journalctl -u nlm
```

#### 3. Create Minimal Reproduction
```python
# Create minimal reproducible example
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10)

brain = pynlm.createBrain(config)
brain.initialize()

for step in range(5):
    brain.step(step)
```

#### 4. Report Issues
```bash
# Report issues with logs
nlm --log-level debug

# Create bug report
# Include:
# - Error messages
# - Configuration
# - Minimal reproduction
# - System information
```

## Conclusion

This troubleshooting guide provides comprehensive solutions for NLM issues, from basic setup to advanced debugging. The key to effective troubleshooting is:

1. **Start simple**: Check basic setup and configuration
2. **Isolate the problem**: Reproduce the issue with minimal code
3. **Check logs**: Use debugging information and error messages
4. **Compare with known issues**: Check documentation for similar problems
5. **Systematically test**: Test each component individually

The NLM community is here to help. If you encounter issues that cannot be resolved with this guide, please:

1. Check for updated documentation
2. Search for similar issues in the bug tracker
3. Report new issues with complete information
4. Share your solutions with the community

Happy debugging and may your NLM simulations run smoothly! 

---

**Quick Reference Commands:**

```bash
# Basic troubleshooting
nlm --help                    # Show help
nlm --version                 # Show version
nlm --log-level debug         # Enable debug logging

# Performance testing
nlm benchmark --suite scaling

# Configuration validation
nlm validate --config configs/default.cfg

# Basic simulation
nlm demo phase3 --episodes 5 --steps 100
```

This completes the Troubleshooting Documentation. The guide provides comprehensive solutions for NLM issues, from basic setup to advanced debugging, with practical examples and step-by-step instructions.