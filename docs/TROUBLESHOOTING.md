# NLM Troubleshooting Guide

## Overview

This document provides comprehensive troubleshooting guidance for NLM users, covering common issues, their causes, and step-by-step solutions. It includes error messages, debugging techniques, and workarounds for known problems.

---

## Quick Reference

### Immediate Solutions

#### Most Common Issues

| Symptom | Likely Cause | Quick Fix |
|---------|-------------|-----------|
| "Brain not responding" | Missing `initialize()` call | Call `brain.initialize()` before `brain.step()` |
| "Nothing happens" | No sensory input | Connect brain to world or provide input manually |
| "Agent not moving" | Motor command not applied | Call `world.applyMotorCommand()` |
| "Simulation too slow" | Large brain size | Use smaller configuration or performance preset |
| "Memory full" | Memory systems enabled | Disable unused memory systems |

#### Error Messages and Fixes

**Import/Load Errors:**
```
Error: No module named 'pynlm'
Fix: pip install pynlm
Fix: python -m pip install --force-reinstall .
```

**Initialization Errors:**
```
Error: Brain not initialized
Fix: Call brain.initialize() before any simulation steps
```

**Simulation Errors:**
```
Error: Invalid step number
Fix: Ensure step numbers are sequential and non-negative
```

**Memory Errors:**
```
Error: Memory allocation failed
Fix: Use smaller brain configuration or increase system memory
```

---

## Problem-Solving Workflow

### 1. Identify the Problem

#### Step-by-Step Diagnosis

1. **Check Error Messages**
   - Copy exact error message
   - Note stack trace if available
   - Check when the error occurs (step number, time)

2. **Reproduce the Issue**
   - Create minimal reproduction case
   - Test with known working configuration
   - Remove complexity until issue is reproduced

3. **Gather Information**
   - NLM version
   - Configuration used
   - System resources
   - Error logs

4. **Form Hypothesis**
   - What could cause this error?
   - When does it happen?
   - How can I test the hypothesis?

### 2. Use Debugging Tools

#### NLM Debugging Utilities

```python
import pynlm

# Debug brain state
brain = pynlm.createBrain(config)
brain.initialize()

# Check brain state
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Initialized: {brain.isInitialized()}")
print(f"Current step: {brain.getCurrentStep()}")

# Debug memory
print(f"Working memory load: {brain.getWorkingMemoryLoad()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Debug performance
brain.startProfiling()
for step in range(100):
    brain.step(step)
profile = brain.stopProfiling()
print(f"Performance: {profile.steps_per_second} steps/sec")
```

#### Logging and Monitoring

```python
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger('nlm')
logger.setLevel(logging.DEBUG)

# Add custom handler
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
console_handler.setFormatter(formatter)
logger.addHandler(console_handler)

# Log brain events
brain.setLogger(logger)
```

### 3. Apply Solutions

#### Systematic Troubleshooting

1. **For Initialization Issues**
   ```python
   # Problem: Brain not responding
   # Solution: Check initialization
   if not brain.isInitialized():
       brain.initialize()
       print("Brain initialized successfully")
   ```

2. **For Performance Issues**
   ```python
   # Problem: Simulation too slow
   # Solution: Use performance preset or reduce complexity
   if simulation_time > threshold:
       config = pynlm.createFastConfig()
       brain = pynlm.createBrain(config)
   ```

3. **For Memory Issues**
   ```python
   # Problem: Memory systems consuming too much
   # Solution: Disable unused systems
   if memory_usage > limit:
       config.set("memory.enable_working_memory", False)
       config.set("memory.enable_episodic_memory", False)
   ```

---

## Common Issues and Solutions

### 1. Initialization and Setup Issues

#### Problem: Brain Not Initializing

**Symptoms:**
- Brain appears in IDLE state
- No spikes generated even after many steps
- `brain.getFiringNeuronCount()` always returns 0

**Possible Causes:**
1. `brain.initialize()` not called
2. Invalid configuration
3. Memory allocation failure
4. Circular dependencies

**Solutions:**

**Check 1: Initialization Call**
```python
def debug_initialization(config):
    brain = pynlm.createBrain(config)
    
    # Check if initialized
    if brain.isInitialized():
        print("✓ Brain is initialized")
    else:
        print("✗ Brain is not initialized")
        brain.initialize()
        print("✓ Brain initialized after call")
    
    return brain

# Test
brain = debug_initialization(pynlm.createDefaultConfig())
```

**Check 2: Configuration Validity**
```python
def debug_configuration(config):
    brain = pynlm.createBrain(config)
    
    # Validate configuration
    if not config.validate():
        print("Configuration invalid:")
        for error in config.getValidationErrors():
            print(f"  - {error}")
        
        # Try to fix
        config = fix_configuration(config)
    
    # Try initialization
    try:
        brain.initialize()
        print("✓ Configuration valid, initialization successful")
    except Exception as e:
        print(f"✗ Initialization failed: {e}")
    
    return brain

# Test
def fix_configuration(config):
    """Attempt to fix common configuration issues"""
    # Ensure required fields are set
    if not config.has("brain.neuron_count"):
        config.set("brain.neuron_count", 1000)
    
    if not config.has("plasticity.stdp.enable"):
        config.set("plasticity.stdp.enable", True)
    
    return config
```

**Check 3: System Resources**
```python
import psutil
import os

def debug_system_resources():
    """Check if system has enough resources"""
    
    # Memory check
    memory = psutil.virtual_memory()
    print(f"Available memory: {memory.available / (1024**3):.1f} GB")
    
    if memory.available < 2 * 1024**3:  # Less than 2GB
        print("⚠️  Low memory - consider smaller brain configuration")
    
    # CPU check
    cpu_count = os.cpu_count()
    print(f"CPU cores: {cpu_count}")
    
    # Disk space
    disk = psutil.disk_usage('/')
    print(f"Available disk space: {disk.free / (1024**3):.1f} GB")
    
    return memory.available > 2 * 1024**3 and cpu_count >= 2
```

#### Problem: Configuration Not Working

**Symptoms:**
- Configuration changes not applied
- Using default values instead of custom ones
- Settings ignored silently

**Solutions:**

**Method 1: Verify Configuration**
```python
def verify_configuration(config):
    """Verify that configuration changes are applied"""
    
    print("Configuration verification:")
    
    # Test key settings
    test_settings = [
        ("brain.neuron_count", 1000),
        ("plasticity.stdp.enable", True),
        ("neuromod.dopamine.scale", 1.0),
    ]
    
    for setting_path, expected_value in test_settings:
        if config.has(setting_path):
            actual_value = config.getInt(setting_path) if isinstance(expected_value, int) else \
                          config.getFloat(setting_path) if isinstance(expected_value, float) else \
                          config.getBool(setting_path)
            
            if actual_value == expected_value:
                print(f"✓ {setting_path}: {actual_value} (expected: {expected_value})")
            else:
                print(f"✗ {setting_path}: {actual_value} (expected: {expected_value})")
        else:
            print(f"✗ {setting_path}: not set")
    
    return config

# Test
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
verified_config = verify_configuration(config)
```

**Method 2: Configuration File Issues**
```python
def debug_config_file(filename):
    """Debug configuration file loading"""
    
    # Try to load config file
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile(filename):
        print(f"✗ Failed to load config file: {filename}")
        print("Creating default configuration...")
        config = pynlm.createDefaultConfig()
        
        # Save default for future use
        config.saveToFile(f"default_{filename}")
        print(f"Saved default config to: default_{filename}")
    
    return config

# Test
config = debug_config_file("my_config.cfg")
```

### 2. Simulation and Runtime Issues

#### Problem: Agent Not Interacting with World

**Symptoms:**
- Agent doesn't move
- No changes to world state
- Actions not being applied

**Solutions:**

**Check 1: Action Pipeline**
```python
def debug_agent_world_interaction(brain, world, agent, steps=10):
    """Debug agent-world interaction"""
    
    print("=== Agent-World Interaction Debug ===")
    
    for step in range(steps):
        print(f"\nStep {step}:")
        
        # 1. Update world
        world.update(0.1)
        print(f"  World updated: {world.getSimulationTime()}")
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        print(f"  Sensory input received: {len(percept.getData())} elements")
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        print(f"  Agent processed sensory input")
        
        # 4. Run brain step
        brain.step(step)
        print(f"  Brain stepped: {brain.getFiringNeuronCount()} firing neurons")
        
        # 5. Decode motor command
        action = agent.decodeMotorCommand()
        print(f"  Motor command decoded: {action.type}")
        
        # 6. Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        print(f"  Action applied")
        
        # 7. Check results
        print(f"  World state changed: {check_world_changed()}")
    
    return brain, world, agent

# Test
brain, world, agent = debug_agent_world_interaction(brain, world, agent)
```

**Check 2: Action Validation**
```python
def validate_actions(world, agent, action_types):
    """Validate that actions are being applied correctly"""
    
    print(f"\n=== Action Validation ===")
    
    action_counts = {action_type: 0 for action_type in action_types}
    
    for _ in range(100):  # Run multiple steps
        # Get action from agent
        action = agent.decodeMotorCommand()
        
        # Validate action
        if action.type in action_counts:
            action_counts[action.type] += 1
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Action counts:")
    for action_type, count in action_counts.items():
        if count > 0:
            print(f"  {action_type}: {count} times")
        else:
            print(f"  {action_type}: never used")
    
    return action_counts

# Test
action_types = ['MoveForward', 'TurnLeft', 'TurnRight', 'Interact', 'Wait']
action_counts = validate_actions(world, agent, action_types)
```

#### Problem: Learning Not Working

**Symptoms:**
- Synaptic weights not changing
- No learning progress
- Plasticity rules not applying

**Solutions:**

**Check 1: Plasticity Status**
```python
def debug_plasticity(brain, agent, steps=100):
    """Debug plasticity mechanisms"""
    
    print("\n=== Plasticity Debug ===")
    
    # Track synaptic changes
    initial_weights = {}
    weight_changes = []
    
    for step in range(steps):
        # Record initial weights (first step)
        if step == 0:
            brain.recordInitialWeights()
        
        # Run simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(world.getReward(), agent.getPredictionError())
        agent.updateDevelopment(0.1)
        
        # Record weight changes
        if step % 10 == 0:
            current_weights = brain.getCurrentWeights()
            weight_change = calculate_weight_change(initial_weights, current_weights)
            weight_changes.append(weight_change)
            
            print(f"  Step {step}: Weight change = {weight_change:.6f}")
    
    # Analyze plasticity
    avg_weight_change = np.mean([change for change in weight_changes if change is not None])
    print(f"\nAverage weight change: {avg_weight_change:.6f}")
    
    if abs(avg_weight_change) < 0.001:
        print("⚠️  Minimal weight change - plasticity may not be working")
    else:
        print("✓ Significant weight change detected")
    
    return weight_changes

# Test
weight_changes = debug_plasticity(brain, agent)
```

**Check 2: Neuromodulation**
```python
def debug_neuromodulation(agent, steps=50):
    """Debug neuromodulation systems"""
    
    print(f"\n=== Neuromodulation Debug ===")
    
    neuromod_data = {
        'dopamine': [],
        'curiosity': [],
        'novelty': [],
        'prediction_error': []
    }
    
    for step in range(steps):
        # Get neuromodulation levels
        neuromod_data['dopamine'].append(agent.getNeuromodulationLevel('dopamine'))
        neuromod_data['curiosity'].append(agent.getCuriosityLevel())
        neuromod_data['novelty'].append(agent.getNoveltyLevel())
        neuromod_data['prediction_error'].append(agent.getPredictionError())
    
    # Analyze patterns
    print("Neuromodulation levels (last 10 steps):")
    for neuromod_type, values in neuromod_data.items():
        last_values = values[-10:]
        avg_value = np.mean(last_values)
        print(f"  {neuromod_type}: {avg_value:.3f} (min: {min(last_values):.3f}, max: {max(last_values):.3f})")
    
    # Check if neuromodulation is active
    if all(np.mean(values) == 0.0 for values in neuromod_data.values()):
        print("⚠️  Neuromodulation appears to be inactive")
    else:
        print("✓ Neuromodulation systems are active")
    
    return neuromod_data

# Test
neuromod_data = debug_neuromodulation(agent)
```

#### Problem: Memory Systems Not Working

**Symptoms:**
- Memory capacity not being used
- No pattern encoding/decoding
- Memory load always zero

**Solutions:**

**Check 1: Memory System Status**
```python
def debug_memory_systems(brain, steps=50):
    """Debug memory systems"""
    
    print(f"\n=== Memory Systems Debug ===")
    
    # Test working memory
    print("Testing Working Memory:")
    
    test_pattern = [1.0, 0.5, 0.2, 0.1, 0.0]
    brain.receiveWorkingMemory(test_pattern, len(test_pattern))
    print(f"  Pattern encoded (length: {len(test_pattern)})")
    
    working_memory_load = brain.getWorkingMemoryLoad()
    print(f"  Working memory load: {working_memory_load:.3f}")
    
    if working_memory_load == 0:
        print("  ⚠️  Working memory not active")
    
    # Test episodic memory
    print("\nTesting Episodic Memory:")
    
    for episode in range(3):
        # Create experience pattern
        experience_pattern = np.random.rand(20)
        
        # Encode
        brain.encodeEpisodicMemory(experience_pattern, episode)
        print(f"  Episode {episode}: Encoded")
        
        # Simulate time passing
        for _ in range(10):
            brain.step(0)  # Use step 0 for timing
        
        # Try to recall
        cue_pattern = experience_pattern * 0.5  # Partial cue
        recalled = []
        brain.recallEpisodicMemory(recalled, cue_pattern)
        
        if recalled:
            similarity = np.dot(experience_pattern, recalled) / (
                np.linalg.norm(experience_pattern) * np.linalg.norm(recalled)
            )
            print(f"  Episode {episode}: Recalled (similarity: {similarity:.3f})")
        else:
            print(f"  Episode {episode}: Not recalled")
    
    # Test associative memory
    print("\nTesting Associative Memory:")
    
    stimulus = np.array([1.0, 0.0, 0.0, 1.0, 0.0])
    response = np.array([0.0, 1.0, 1.0, 0.0, 1.0])
    
    brain.associate(stimulus, response)
    print(f"  Association formed: {np.dot(stimulus, response):.3f}")
    
    # Test recall
    cue = stimulus * 0.7
    retrieved = []
    brain.retrieveAssociation(retrieved, cue)
    
    if retrieved:
        accuracy = np.dot(response, retrieved) / (
            np.linalg.norm(response) * np.linalg.norm(retrieved)
        )
        print(f"  Recall accuracy: {accuracy:.3f}")
    else:
        print("  ⚠️  Association not recalled")
    
    return {
        'working_memory_active': working_memory_load > 0,
        'episodic_memory_tested': True,
        'associative_memory_tested': True
    }

# Test
memory_status = debug_memory_systems(brain)
```

### 3. Performance and Resource Issues

#### Problem: Simulation Too Slow

**Symptoms:**
- Long execution times
- Low steps per second
- Poor performance compared to expectations

**Solutions:**

**Check 1: Performance Profiling**
```python
def profile_performance(config, steps=1000):
    """Profile simulation performance"""
    
    print(f"\n=== Performance Profiling ({steps} steps) ===")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Start profiling
    start_time = time.time()
    tracemalloc.start()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        if step % 200 == 0:
            print(f"  Step {step}/{steps}")
    
    end_time = time.time()
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    # Calculate metrics
    simulation_time = end_time - start_time
    steps_per_second = steps / simulation_time
    memory_used_mb = peak / (1024 * 1024)
    
    print(f"\nPerformance Results:")
    print(f"  Simulation time: {simulation_time:.2f} seconds")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Memory usage: {memory_used_mb:.1f} MB")
    print(f"  Time per step: {simulation_time/steps*1000:.2f} ms")
    
    # Compare with expected performance
    expected_sps = get_expected_steps_per_second(config)
    if steps_per_second < expected_sps * 0.5:
        print(f"  ⚠️  Performance {steps_per_second:.1f} SPS is below expected {expected_sps:.1f} SPS")
        return False
    else:
        print(f"  ✓ Performance meets expectations")
        return True

# Expected performance by configuration
performance_targets = {
    'minimal': 1000,
    'fast': 500,
    'learning': 200,
    'development': 150,
    'performance': 100,
    'research': 300
}

def get_expected_steps_per_second(config_name):
    return performance_targets.get(config_name, 100)
```

**Check 2: Memory Optimization**
```python
def optimize_memory_usage(config):
    """Optimize memory usage for better performance"""
    
    print(f"\n=== Memory Optimization ===")
    
    # Get current memory usage
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    initial_memory = brain.getMemoryUsage()
    print(f"Initial memory usage: {initial_memory} MB")
    
    # Try memory optimization
    optimizations = [
        ("neuron_pooling", lambda c: c.set("memory.pools.enable_neuron_pool", True)),
        ("sparse_connectivity", lambda c: c.set("connectivity.type", "adjacency_list")),
        ("event_driven", lambda c: c.set("plasticity.update_frequency", "event_driven")),
        ("vectorization", lambda c: c.set("processing.enable_vectorization", True)),
    ]
    
    for optimization_name, optimization_func in optimizations:
        print(f"\nTrying {optimization_name}...")
        
        # Create copy with optimization
        optimized_config = pynlm.createDefaultConfig()
        optimized_config.set_from_config(config)  # Copy all settings
        optimization_func(optimized_config)
        
        # Test performance
        brain_optimized = pynlm.createBrain(optimized_config)
        brain_optimized.initialize()
        
        # Compare memory usage
        optimized_memory = brain_optimized.getMemoryUsage()
        
        if optimized_memory < initial_memory * 0.8:  # 20% improvement
            print(f"✓ {optimization_name} reduced memory usage by {((initial_memory - optimized_memory) / initial_memory * 100):.1f}%")
            config = optimized_config
        else:
            print(f"✗ {optimization_name} did not improve memory usage")
    
    print(f"\nFinal memory usage: {config.getInt('brain.neuron_count')} neurons")
    return config

# Test
config = pynlm.createLearningConfig()
optimized_config = optimize_memory_usage(config)
```

**Check 3: Configuration-Based Performance**
```python
def get_performance_config(neuron_count):
    """Get appropriate configuration for desired neuron count"""
    
    if neuron_count <= 1000:
        return pynlm.createMinimalConfig()
    elif neuron_count <= 5000:
        return pynlm.createFastConfig()
    elif neuron_count <= 10000:
        return pynlm.createLearningConfig()
    elif neuron_count <= 50000:
        return pynlm.createPerformanceConfig()
    else:
        return pynlm.createResearchConfig()

# Usage
brain = pynlm.createBrain(pynlm.createPerformanceConfig())
```

### 4. Debugging Techniques

#### Problem: Hard to Debug Complex Issues

**Symptoms:**
- Intermittent failures
- Difficult to reproduce
- Complex interactions between systems

**Solutions:**

**Method 1: Systematic Logging**
```python
def enable_systematic_logging(brain, world, agent, log_file="debug.log"):
    """Enable comprehensive logging for debugging"""
    
    import logging
    from datetime import datetime
    
    # Create logger
    logger = logging.getLogger('nlm_debug')
    logger.setLevel(logging.DEBUG)
    
    # Remove existing handlers
    logger.handlers.clear()
    
    # File handler
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    file_handler = logging.FileHandler(f"{log_file}_{timestamp}.log")
    file_handler.setLevel(logging.DEBUG)
    
    # Console handler
    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.INFO)
    
    # Formatters
    formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    file_handler.setFormatter(formatter)
    console_handler.setFormatter(formatter)
    
    # Add handlers
    logger.addHandler(file_handler)
    logger.addHandler(console_handler)
    
    # Log brain events
    brain.setLogger(logger, "brain")
    world.setLogger(logger, "world")
    agent.setLogger(logger, "agent")
    
    print(f"Logging enabled. Debug file: {log_file}_{timestamp}.log")
    
    return logger

# Enable logging
logger = enable_systematic_logging(brain, world, agent)

# Log important events
logger.info("Simulation started")
logger.debug(f"Brain created with {brain.getTotalNeuronCount()} neurons")
logger.debug(f"World configured: {world.getWidth()}x{world.getHeight()}")
```

**Method 2: State Snapshot**
```python
def take_state_snapshot(brain, world, agent, snapshot_file="snapshot.pkl"):
    """Take a snapshot of current state for debugging"""
    
    import pickle
    from datetime import datetime
    
    # Create snapshot data
    snapshot = {
        'timestamp': datetime.now().isoformat(),
        'brain_state': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_count': brain.getFiringNeuronCount(),
            'stage': brain.getDevelopmentalStage().value,
            'time': brain.getCurrentTime()
        },
        'world_state': {
            'position': world.getAgentPosition(),
            'energy': world.getAgentEnergy(),
            'simulation_time': world.getSimulationTime()
        },
        'agent_state': {
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'reward': agent.getCurrentReward(),
            'error': agent.getPredictionError()
        }
    }
    
    # Save snapshot
    filename = f"{snapshot_file}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.pkl"
    with open(filename, 'wb') as f:
        pickle.dump(snapshot, f)
    
    print(f"State snapshot saved to: {filename}")
    
    return snapshot

# Take snapshot
snapshot = take_state_snapshot(brain, world, agent)
```

**Method 3: Step-by-Step Replay**
```python
class DebugReplay:
    """Replay simulation step-by-step for debugging"""
    
    def __init__(self, brain, world, agent, steps=100):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.steps = steps
        self.step_data = []
    
    def record_step(self, step_number):
        """Record data for a single step"""
        
        step_record = {
            'step': step_number,
            'brain_state': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'firing_count': self.brain.getFiringNeuronCount(),
                'stage': self.brain.getDevelopmentalStage().value,
                'time': self.brain.getCurrentTime()
            },
            'world_state': {
                'position': self.world.getAgentPosition(),
                'energy': self.world.getAgentEnergy(),
                'simulation_time': self.world.getSimulationTime()
            },
            'agent_state': {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'reward': self.agent.getCurrentReward(),
                'error': self.agent.getPredictionError()
            }
        }
        
        self.step_data.append(step_record)
        return step_record
    
    def replay_step(self, step_number):
        """Replay a specific step"""
        
        # Find recorded step data
        step_record = next((record for record in self.step_data 
                           if record['step'] == step_number), None)
        
        if step_record:
            print(f"Replaying step {step_number}:")
            print(f"  Brain: {step_record['brain_state']['firing_count']} neurons firing")
            print(f"  World: Agent at {step_record['world_state']['position']}")
            print(f"  Agent: Curiosity {step_record['agent_state']['curiosity']:.3f}, "
                  f"Novelty {step_record['agent_state']['novelty']:.3f}")
        
        return step_record

# Record simulation for debugging
replay = DebugReplay(brain, world, agent, 100)
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    replay.record_step(step)
```

---

## Error Code Reference

### Brain Errors (1000-1999)

| Error Code | Description | Solution |
|------------|-------------|----------|
| 1000 | Brain not initialized | Call brain.initialize() before simulation |
| 1001 | Invalid configuration | Check configuration file |
| 1002 | Memory allocation failed | Increase system memory or reduce brain size |
| 1003 | Invalid step number | Use sequential step numbers starting from 0 |
| 1004 | Plasticity rule not found | Enable required plasticity mechanisms |
| 1005 | Memory system not available | Check memory system configuration |

### World Errors (2000-2999)

| Error Code | Description | Solution |
|------------|-------------|----------|
| 2000 | World not configured | Call world.configure() before use |
| 2001 | Invalid action | Use valid ActionType enum value |
| 2002 | Position out of bounds | Check world dimensions |
| 2003 | Entity already exists | Check for duplicates |
| 2004 | Invalid reward calculation | Check reward calculation logic |
| 2005 | Simulation time inconsistency | Check time step consistency |

### Agent Errors (3000-3999)

| Error Code | Description | Solution |
|------------|-------------|----------|
| 3000 | Agent not initialized | Call agent.initialize() after world setup |
| 3001 | Sensory processing failed | Check sensory input format |
| 3002 | Motor decoding failed | Check brain activity for motor output |
| 3003 | Neuromodulation not applied | Enable required neuromodulation systems |
| 3004 | Development update failed | Check development system state |
| 3005 | Learning rule not working | Check plasticity rule configuration |

### Configuration Errors (4000-4999)

| Error Code | Description | Solution |
|------------|-------------|----------|
| 4000 | Invalid preset name | Use valid preset: minimal, fast, learning, etc. |
| 4001 | Configuration file not found | Check file path and permissions |
| 4002 | Invalid configuration value | Check value ranges and types |
| 4003 | Missing required configuration | Add required configuration keys |
| 4004 | Configuration validation failed | Check configuration schema |
| 4005 | Preset not available | Use available preset from list |

---

## Common Workflows for Debugging

### Workflow 1: Initialization Debugging

```python
# Step 1: Create minimal configuration
config = pynlm.createMinimalConfig()

# Step 2: Check initialization
brain = pynlm.createBrain(config)
if not brain.isInitialized():
    print("Brain not initialized")
    brain.initialize()

# Step 3: Test basic functionality
brain.step(0)
if brain.getFiringNeuronCount() == 0:
    print("No neurons firing - check configuration")
else:
    print("✓ Basic brain functionality working")
```

### Workflow 2: Agent-World Interaction Debugging

```python
# Step 1: Create complete setup
brain = pynlm.createBrain(pynlm.createLearningConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 2: Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Step 3: Test interaction
for step in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Debug action
    if action.type == pynlm.ActionType.MoveForward:
        print("Agent moving forward")
    else:
        print(f"Agent action: {action.type}")
    
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Workflow 3: Performance Debugging

```python
# Step 1: Profile current performance
config = pynlm.createLearningConfig()
performance = pynlm.profileSimulation(config, steps=100)

# Step 2: Identify bottlenecks
if performance['steps_per_second'] < 100:
    print("Performance bottleneck detected")
    
    # Try performance optimization
    optimized_config = pynlm.createFastConfig()
    optimized_config.set("brain.neuron_count", config.getInt("brain.neuron_count"))
    
    # Re-test
    optimized_performance = pynlm.profileSimulation(optimized_config, steps=100)
    
    if optimized_performance['steps_per_second'] > performance['steps_per_second']:
        print("Performance improved with fast configuration")
```

### Workflow 4: Memory System Debugging

```python
# Step 1: Test memory systems
config = pynlm.createLearningConfig()
config.set("memory.enable_working_memory", True)
config.set("memory.enable_episodic_memory", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Test working memory
pattern = [1.0, 0.5, 0.2]
brain.receiveWorkingMemory(pattern, len(pattern))

# Step 3: Wait and check retention
for step in range(50):
    brain.step(step)

retrieved = []
brain.getWorkingMemory(retrieved, len(pattern))

if len(retrieved) == len(pattern):
    print("✓ Working memory functional")
else:
    print("✗ Working memory not working")
```

### Workflow 5: Neuromodulation Debugging

```python
# Step 1: Enable neuromodulation
config = pynlm.createCuriosityConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Step 2: Monitor neuromodulation
for step in range(20):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check neuromodulation levels
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    reward = world.getReward()
    
    print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, Reward={reward:.2f}")
    
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Advanced Debugging Techniques

### 1. Automated Error Detection

```python
def automated_debugging(brain, world, agent, max_steps=1000):
    """Automated debugging that detects and fixes common issues"""
    
    issues_found = []
    fixes_applied = []
    
    for step in range(max_steps):
        # Check for common issues
        if brain.getFiringNeuronCount() == 0 and step > 100:
            issues_found.append("No neurons firing after initialization")
            # Apply fix: Check configuration
            if brain.getTotalNeuronCount() == 0:
                fixes_applied.append("Reset brain with valid configuration")
                brain.initialize()
        
        if agent.getCuriosityLevel() == 0 and step > 50:
            issues_found.append("Curiosity system not active")
            # Apply fix: Enable curiosity
            agent.enableCuriosity(True)
            fixes_applied.append("Enabled curiosity system")
        
        if world.getAgentEnergy() <= 0 and step > 10:
            issues_found.append("Agent energy depleted")
            # Apply fix: Reset environment
            world.reset()
            fixes_applied.append("Reset world environment")
        
        # Continue simulation
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Check for convergence
        if step % 100 == 0:
            print(f"Step {step}: Issues {len(issues_found)}, Fixes {len(fixes_applied)}")
    
    return issues_found, fixes_applied
```

### 2. Machine Learning-Based Debugging

```python
def ml_debugging(brain, world, agent, training_data=None):
    """Use machine learning to identify and fix debugging issues"""
    
    # Collect debugging data
    debug_features = []
    debug_targets = []
    
    for episode in range(10):
        episode_features = extract_debug_features(brain, world, agent)
        episode_target = detect_debug_issues(brain, world, agent)
        
        debug_features.append(episode_features)
        debug_targets.append(episode_target)
    
    # Train debugging model
    model = train_debugging_model(debug_features, debug_targets)
    
    # Apply debugging recommendations
    recommendations = model.predict(debug_features[-1])
    
    for recommendation in recommendations:
        apply_debug_recommendation(recommendation, brain, world, agent)
    
    return recommendations
```

### 3. Integration Testing

```python
def integration_test_suite(brain, world, agent):
    """Run comprehensive integration tests"""
    
    test_results = {
        'initialization': False,
        'sensory_processing': False,
        'motor_control': False,
        'learning': False,
        'development': False,
        'memory': False,
        'neuromodulation': False,
        'performance': False
    }
    
    # Test initialization
    try:
        brain.initialize()
        test_results['initialization'] = True
        print("✓ Initialization test passed")
    except Exception as e:
        print(f"✗ Initialization test failed: {e}")
    
    # Test sensory processing
    try:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        test_results['sensory_processing'] = True
        print("✓ Sensory processing test passed")
    except Exception as e:
        print(f"✗ Sensory processing test failed: {e}")
    
    # Test motor control
    try:
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        test_results['motor_control'] = True
        print("✓ Motor control test passed")
    except Exception as e:
        print(f"✗ Motor control test failed: {e}")
    
    # Test learning
    try:
        agent.enableRewardModulation(True)
        agent.applyRewardModulation(1.0, 0.0)
        test_results['learning'] = True
        print("✓ Learning test passed")
    except Exception as e:
        print(f"✗ Learning test failed: {e}")
    
    # Continue with other tests...
    
    # Print summary
    print(f"\nIntegration Test Results: {sum(test_results.values())}/{len(test_results)} passed")
    
    for test_name, passed in test_results.items():
        status = "✓" if passed else "✗"
        print(f"{status} {test_name}")
    
    return test_results
```

---

## Documentation and Support

### Getting Help

#### Online Resources
- **NLM Documentation**: https://docs.nlm.org
- **GitHub Issues**: https://github.com/nlmlab/nlm/issues
- **Discussion Forum**: https://forum.nlm.org
- **Tutorial Videos**: https://youtube.com/nlmlab/tutorials

#### Support Channels

| Channel | Type | Response Time | Description |
|---------|------|---------------|-------------|
| GitHub Issues | Bug Reports | 24-72 hours | Technical issues and bug reports |
| Forum | General Discussion | 48 hours | Questions and discussions |
| Email | Technical Support | 24 hours | Priority support |
| Slack | Real-time Chat | Immediate | Quick questions and code review |

### Reporting Issues

#### Bug Report Template

```
## Bug Report

### Description
[Clear description of the bug]

### Steps to Reproduce
1. [Step 1]
2. [Step 2]
3. [Step 3]

### Expected Behavior
[What should happen]

### Actual Behavior
[What actually happens]

### Environment
- NLM Version: [version]
- Configuration: [config description]
- System: [OS, CPU, memory]
- Simulation steps: [number]

### Error Messages
[Copy exact error messages]

### Logs
[Attach relevant log files]

### Reproduction Script
[Provide minimal script to reproduce the issue]
```

### Best Practices

#### Configuration Management
1. **Always validate configurations** before using them
2. **Use presets** as starting points for experimentation
3. **Document custom configurations** for future reference
4. **Version control configurations** along with code

#### Simulation Best Practices
1. **Always call `initialize()` before `step()`**
2. **Use consistent step numbering** (sequential, starting from 0)
3. **Monitor performance** during long simulations
4. **Save checkpoints** periodically for long-running experiments
5. **Validate input parameters** before simulation

#### Debugging Best Practices
1. **Start simple** and add complexity gradually
2. **Use logging** to track simulation state
3. **Isolate issues** by removing components
4. **Use version control** to track debugging changes
5. **Document fixes** for future reference

#### Performance Optimization
1. **Profile before optimizing** - know what needs improvement
2. **Use appropriate presets** for simulation scale
3. **Enable/disable systems** based on requirements
4. **Monitor resource usage** during long simulations
5. **Use benchmarking** to measure improvements

---

## Conclusion

Effective NLM debugging requires:

1. **Systematic Approach**: Follow step-by-step diagnosis process
2. **Comprehensive Tools**: Use logging, profiling, and state snapshots
3. **Community Support**: Utilize online resources and support channels
4. **Documentation**: Document issues and fixes for future reference
5. **Performance Awareness**: Balance functionality with computational resources

The troubleshooting guide provides comprehensive support for NLM users, from simple setup issues to complex debugging scenarios. Use the workflow patterns, debugging techniques, and support resources to effectively diagnose and resolve NLM-related issues.