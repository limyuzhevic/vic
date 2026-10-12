# Migration Guide: NLM Python Package Reorganization

## Overview

This document describes the changes made to the NLM (Neural Learning Machine) Python bindings and provides guidance for migration.

## Summary of Changes

### 1. New Package Structure
**Before:** Simple `python/` directory with `bindings.cpp` only
**After:** Modern `pynlm/` Python package with organized modules

### 2. Enhanced Documentation
**Before:** Fragmented documentation (`easy_usage.md`, `HOW_TO_USE.md`, `README.md`)
**After:** Unified documentation hub (`NLM_Documentation.md`)

### 3. New Features
- Performance monitoring utilities
- Progress tracking for simulations
- Checkpoint management with metadata
- Benchmarking tools
- Enhanced error handling

## Migration Steps

### Step 1: Package Installation

**Old way:**
```bash
# After building C++ code
pip install -e .  # or pip install .
```

**New way (same command, but enhanced):**
```bash
pip install -e .  # Now installs the full pynlm package
```

### Step 2: Import Updates

**No changes needed!** All existing imports continue to work:

```python
# These all still work exactly the same
import pynlm
from pynlm import Brain, Config, createBrain, createDefaultConfig
from pynlm import AgentBrain, SimpleWorld
from pynlm import Vision, Audio, SensoryInput
from pynlm import Action, ActionType
```

### Step 3: Access New Features

**New utilities available:**

```python
import pynlm

# Progress tracking with statistics
stats = pynlm.run_with_progress(brain, world, agent, num_steps=1000)

# Save checkpoints with metadata
pynlm.save_checkpoint(brain, "checkpoint.bin", "Simulation at step 100")

# Validate simulation setup
pynlm.validate_simulation_setup(brain, world, agent)

# Benchmark performance
benchmark_stats = pynlm.benchmark_simulation(brain, world, agent)

# Get quick reference
quick_ref = pynlm.get_quick_reference()

# Get example patterns
examples = pynlm.get_example_patterns()
```

## Examples Migration

### Example 1: Simple Brain Simulation

**Old documentation (easy_usage.md):**
```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

**New documentation (NLM_Documentation.md):**
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 2: Complete Agent Simulation

**Old documentation (easy_usage.md):**
```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

**New documentation (NLM_Documentation.md):**
```python
import pynlm

def run_agent_simulation(world_size=20, num_steps=500):
    """Run a complete agent simulation with learning and development."""
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Run simulation with progress tracking
    pynlm.run_with_progress(brain, world, agent, num_steps)
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

## Advanced Features Migration

### Progress Tracking

**New feature - previously not available:**

```python
# Old way: Basic simulation loop
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons")

# New way: With progress tracking and statistics
stats = pynlm.run_with_progress(brain, world, agent, num_steps=1000)
print(f"Simulation completed in {stats['elapsed_time']:.2f} seconds")
print(f"Average firing rate: {stats['average_firing_rate']:.3f} Hz")
```

### Checkpoint Management

**New feature - previously only `brain.save()`:**

```python
# Old way: Basic checkpointing
brain.save("checkpoint.bin")

# New way: Enhanced checkpointing with metadata
pynlm.save_checkpoint(brain, "checkpoint_step_100.bin", "Simulation at step 100")

# Or using the high-level interface
checkpoint_data = pynlm.save_checkpoint(brain, "checkpoint.bin", "My simulation")
print(f"Checkpoint created: {checkpoint_data['timestamp']}")
```

### Benchmarking

**New feature - performance benchmarking:**

```python
# Old way: Manual timing
import time
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()
print(f"Manual timing: {end_time - start_time:.3f}s")

# New way: Built-in benchmarking
benchmark_stats = pynlm.benchmark_simulation(brain, world, agent, num_steps=1000)
print(f"Average time: {benchmark_stats['average_time']:.3f}s")
print(f"Steps per second: {benchmark_stats['steps_per_second']:.1f}")
```

### Validation

**New feature - simulation validation:**

```python
# Old way: Manual validation
def validate_manual(brain, world, agent):
    errors = []
    if not brain:
        errors.append("Brain not initialized")
    if world and not hasattr(world, 'getSensoryPercept'):
        errors.append("World missing getSensoryPercept")
    if agent and not hasattr(agent, 'processSensoryInput'):
        errors.append("Agent missing processSensoryInput")
    return errors

# New way: Built-in validation
errors = pynlm.validate_simulation_setup(brain, world, agent)
if errors:
    print("Validation errors:", errors)
else:
    print("Simulation setup is valid")
```

## Error Handling Improvements

### Custom Exception Classes

**New feature:**

```python
import pynlm
from pynlm.exceptions import NLMError, ConfigurationError, SimulationError, ValidationError

# Old way: Basic exceptions
try:
    brain.step(step)
except RuntimeError as e:
    print(f"Error: {e}")

# New way: Specific exception types
try:
    pynlm.validate_simulation_setup(brain, world, agent)
except ValidationError as e:
    print(f"Validation failed: {e}")
except SimulationError as e:
    print(f"Simulation error: {e}")
```

## Documentation Navigation

### Quick Reference

**New utility:**

```python
import pynlm

# Get quick reference for interactive use
quick_ref = pynlm.get_quick_reference()
print("Brain operations:", quick_ref['brain.step()'])
print("World operations:", quick_ref['world.update()'])
```

### Example Patterns

**New utility:**

```python
import pynlm

# Get example patterns for common use cases
examples = pynlm.get_example_patterns()
print("=== Simple Brain Example ===")
print(examples['simple_brain'])

print("\n=== Agent Simulation Example ===")
print(examples['agent_world'])

print("\n=== Brain Statistics Example ===")
print(examples['brain_stats'])
```

## Backward Compatibility

### What Hasn't Changed
- **API:** All existing class methods and functions work identically
- **Imports:** All import statements continue to work
- **Functionality:** Core simulation behavior is unchanged
- **Examples:** Existing examples continue to work

### What Has Changed
- **Package Structure:** Now organized into `pynlm/` package
- **Documentation:** Unified into `NLM_Documentation.md`
- **New Features:** Added utilities and enhanced functionality
- **Installation:** Same command, but enhanced package

### Migration Checklist

☐ **Code Review:** Check that all imports still work
☐ **Documentation:** Update references to point to `NLM_Documentation.md`
☐ **Testing:** Run existing tests to ensure compatibility
☐ **New Features:** Explore and adopt new utilities where beneficial
☐ **Performance:** Test enhanced features for performance impact

## Troubleshooting

### Common Issues and Solutions

#### Issue: Import errors
**Symptom:** `ImportError: cannot import name 'pynlm'`
**Solution:** 
```bash
pip install -e .
```

#### Issue: Documentation not found
**Symptom:** References to old documentation files
**Solution:** Use `NLM_Documentation.md` for comprehensive documentation

#### Issue: New features not working
**Symptom:** Utilities not available
**Solution:** Ensure package is properly installed and up-to-date

## Next Steps

### For Users
1. **Update Documentation References:** Point to `NLM_Documentation.md` instead of individual files
2. **Explore New Features:** Try the progress tracking, checkpointing, and benchmarking utilities
3. **Performance Testing:** Use the built-in benchmarking tools to compare performance

### For Developers
1. **Contribute to Documentation:** Help maintain `NLM_Documentation.md`
2. **Add Examples:** Contribute new example patterns to the utilities
3. **Enhance Features:** Add more validation or utility functions
4. **Test Thoroughly:** Ensure backward compatibility is maintained

### For Research
1. **Performance Monitoring:** Use benchmarking tools for research comparisons
2. **Simulation Tracking:** Leverage progress tracking for long experiments
3. **Checkpoint Management:** Save and restore complex simulations
4. **Validation:** Ensure experimental setups are properly configured

## Conclusion

This reorganization modernizes the NLM Python bindings while maintaining full backward compatibility. The enhanced package structure, unified documentation, and new utilities significantly improve the user experience without breaking existing code. The migration path is straightforward, with most existing code continuing to work unchanged while users can gradually adopt the new features as they become available.

The key benefits are:
- **Better Organization:** Logical package structure makes code easier to navigate
- **Comprehensive Documentation:** Single source of truth for all documentation
- **Enhanced Functionality:** Performance monitoring, validation, and benchmarking tools
- **Improved Developer Experience:** Type hints, examples, and utilities
- **Maintained Compatibility:** No breaking changes to existing code

This transformation positions NLM for continued development while making it more accessible to new users and more powerful for existing users.
