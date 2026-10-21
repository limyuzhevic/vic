# NLM Python API Enhancement Demonstration

This script demonstrates the enhanced Python API features available in NLM.

```python
"""Demonstration of Enhanced NLM Python API Features"""

import pynlm
import tempfile
import os
import json

def demonstrate_basic_enhanced_usage():
    """Demonstrate enhanced basic usage patterns"""
    print("=== Enhanced Basic Usage ===")
    
    # 1. Create brain with custom configuration
    print("\n1. Creating brain with custom configuration...")
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(500)  # Enhanced configuration method
    config.setRegionCount(2)
    config.setConnectionProbability(0.15)
    config.setSimulationTimestep(0.001)
    config.setRandomSeed(123)
    
    brain = pynlm.createBrain(config)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Enhanced initialization with error handling
    print("\n2. Initializing brain with error handling...")
    try:
        brain.initialize()
        print("   ✓ Brain initialized successfully")
    except pynlm.NLMBrainError as e:
        print(f"   ✗ Brain initialization failed: {e}")
    
    # 3. Enhanced simulation with statistics collection
    print("\n3. Running simulation with enhanced statistics...")
    stats = {
        'total_spikes': [],
        'firing_rates': [],
        'step_times': []
    }
    
    import time
    start_time = time.time()
    
    for step in range(10):
        step_start = time.time()
        
        brain.step(step)
        
        # Collect enhanced statistics
        stats['total_spikes'].append(brain.getTotalSpikeCount())
        stats['firing_rates'].append(brain.getAverageFiringRate())
        stats['step_times'].append(time.time() - step_start)
        
        if step % 5 == 0:
            print(f"   Step {step}: {brain.getTotalSpikeCount()} spikes, "
                  f"{brain.getAverageFiringRate():.2f} Hz firing rate")
    
    end_time = time.time()
    print(f"   Simulation completed in {end_time - start_time:.3f}s")
    print(f"   Average step time: {sum(stats['step_times']) / len(stats['step_times']):.5f}s")
    
    return brain, stats

def demonstrate_enhanced_configuration():
    """Demonstrate enhanced configuration features"""
    print("\n=== Enhanced Configuration Features ===")
    
    # 1. Validation
    print("\n1. Configuration validation...")
    config = pynlm.createDefaultConfig()
    
    if config.validate():
        print("   ✓ Default configuration is valid")
    else:
        print(f"   ✗ Configuration validation failed: {config.getValidationError()}")
    
    # 2. Preset configurations
    print("\n2. Preset configuration examples...")
    
    # Development configuration
    dev_config = pynlm.Config.createDevelopmentConfig()
    print(f"   Development config: {dev_config.getStringSafe('neuron_count')} neurons")
    
    # Minimal configuration
    min_config = pynlm.Config.createMinimalConfig()
    print(f"   Minimal config: {min_config.getStringSafe('neuron_count')} neurons")
    
    # 3. Configuration modification
    print("\n3. Configuration modification and validation...")
    config.setNeuronCount(2000)
    config.setRegionCount(4)
    
    try:
        config.set("invalid_key", "very_long_string_that_should_not_pass_validation_anywhere_in_the_system_" * 10)
        print("   ✗ Should have rejected invalid configuration")
    except pynlm.ConfigValidationError as e:
        print(f"   ✓ Correctly rejected invalid configuration: {e}")
    
    return config

def demonstrate_file_operations():
    """Demonstrate enhanced file operations"""
    print("\n=== Enhanced File Operations ===")
    
    # 1. Save brain state
    print("\n1. Saving brain state...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation steps
    for i in range(5):
        brain.step(i)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        filepath = f.name
    
    try:
        success = brain.save(filepath)
        if success:
            print(f"   ✓ Brain state saved to {filepath}")
            
            # Check file size
            file_size = os.path.getsize(filepath)
            print(f"   File size: {file_size} bytes")
            
            # Load brain state
            brain2 = pynlm.createBrain(config)
            success = brain2.load(filepath)
            if success:
                print(f"   ✓ Brain state loaded successfully")
                print(f"   Loaded brain has {brain2.getTotalNeuronCount()} neurons")
            else:
                print("   ✗ Failed to load brain state")
        else:
            print("   ✗ Failed to save brain state")
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
            print(f"   Cleaned up temporary file")

def demonstrate_agent_simulation():
    """Demonstrate enhanced agent simulation"""
    print("\n=== Enhanced Agent Simulation ===")
    
    # 1. Create complete agent simulation
    print("\n1. Creating enhanced agent simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"   Agent initialized with {agent.getSensoryInputSize()} sensory inputs")
    print(f"   Agent has {agent.getMotorOutputSize()} motor outputs")
    
    # 2. Run enhanced simulation
    print("\n2. Running enhanced agent simulation...")
    episode_length = 20
    
    for step in range(episode_length):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSimulationTime() % 5.0 - 2.5  # Simple reward function
        predicted_reward = agent.getCuriosityLevel()
        agent.applyRewardModulation(reward, predicted_reward)
        
        if step % 5 == 0:
            print(f"   Step {step}: "
                  f"Action={action}, "
                  f"Reward={reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Development={agent.getDevelopmentalStage()}")
    
    # 3. Analyze results
    print("\n3. Simulation results...")
    print(f"   Total simulation time: {world.getSimulationTime():.2f}s")
    print(f"   Final neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"   Final prediction error: {agent.getPredictionError():.2f}")
    print(f"   Final developmental stage: {agent.getDevelopmentalStage()}")

def demonstrate_error_handling():
    """Demonstrate enhanced error handling"""
    print("\n=== Enhanced Error Handling ===")
    
    # 1. Configuration validation errors
    print("\n1. Configuration validation errors...")
    try:
        config = pynlm.Config()
        config.setNeuronCount(-10)  # Invalid: negative neuron count
        if not config.validate():
            print(f"   ✓ Configuration validation caught error: {config.getValidationError()}")
    except Exception as e:
        print(f"   ✓ Exception caught: {type(e).__name__}: {e}")
    
    # 2. Brain operation errors
    print("\n2. Brain operation errors...")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        # Try to step without initialization (should fail)
        brain.step(0)
        print("   ✗ Should have raised error for uninitialized brain")
    except pynlm.NLMBrainError as e:
        print(f"   ✓ Brain operation correctly caught: {e}")
    
    # 3. File operation errors
    print("\n3. File operation errors...")
    try:
        # Try to load from non-existent file
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        success = brain.load("/non/existent/file.nlm")
        if not success:
            print("   ✓ File operation correctly failed for non-existent file")
    except Exception as e:
        print(f"   ✓ File operation exception: {type(e).__name__}: {e}")

def demonstrate_performance_monitoring():
    """Demonstrate enhanced performance monitoring"""
    print("\n=== Enhanced Performance Monitoring ===")
    
    import time
    
    # 1. Benchmark simulation performance
    print("\n1. Performance benchmarking...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm-up run
    for i in range(10):
        brain.step(i)
    
    # Benchmark run
    num_steps = 100
    start_time = time.time()
    
    for i in range(num_steps):
        brain.step(i)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"   Performance metrics:")
    print(f"   - Steps per second: {num_steps / elapsed:.1f}")
    print(f"   - Average step time: {elapsed / num_steps * 1000:.2f} ms")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   - Neurons processed: {brain.getTotalNeuronCount()}")

if __name__ == "__main__":
    print("NLM Python API Enhancement Demonstration")
    print("=" * 50)
    
    # Run all demonstrations
    demonstrate_basic_enhanced_usage()
    demonstrate_enhanced_configuration()
    demonstrate_file_operations()
    demonstrate_agent_simulation()
    demonstrate_error_handling()
    demonstrate_performance_monitoring()
    
    print("\n" + "=" * 50)
    print("Demonstration completed successfully!")
    print("\nKey enhancements demonstrated:")
    print("✓ Enhanced configuration validation and error handling")
    print("✓ Improved Python API with convenience methods")
    print("✓ Better error reporting and type safety")
    print("✓ Performance monitoring and statistics collection")
    print("✓ Advanced agent simulation capabilities")
    print("✓ Robust file operations with error handling")

```

This demonstration shows the comprehensive improvements made to the NLM Python API:

## Key Features Demonstrated:

### 1. **Enhanced Configuration Management**
- Configuration validation with detailed error messages
- Preset configuration factory methods
- Type-safe configuration getters
- Automatic validation during configuration changes

### 2. **Robust Error Handling**
- Comprehensive exception handling for all operations
- Detailed error messages for debugging
- Graceful handling of invalid operations
- Configuration validation with meaningful error messages

### 3. **Advanced Python API**
- Convenience methods for common operations
- Type-safe accessors with validation
- Better integration with Python patterns
- Enhanced file I/O operations

### 4. **Performance Monitoring**
- Benchmark capabilities
- Performance statistics collection
- Step timing and optimization insights

### 5. **Agent Simulation**
- Complete agent-environment integration
- Neuromodulation and learning features
- Enhanced action selection and reward processing
- Development system integration

### 6. **Code Quality Improvements**
- Better separation of concerns
- Modular architecture with clear interfaces
- Comprehensive documentation
- Extensive testing and validation

## Usage Examples:

```python
# Enhanced configuration with validation
config = pynlm.createDefaultConfig()
config.setNeuronCount(1000)
config.setRegionCount(2)
if not config.validate():
    print(f"Config error: {config.getValidationError()}")

# Robust error handling
brain = pynlm.createBrain(config)
try:
    brain.initialize()
    brain.step(0)
except pynlm.NLMBrainError as e:
    print(f"Brain operation failed: {e}")

# Performance monitoring
import time
start = time.time()
for i in range(1000):
    brain.step(i)
elapsed = time.time() - start
print(f"Performance: {1000/elapsed:.1f} steps/second")
```

This demonstrates how the enhanced NLM Python API provides a more robust, user-friendly, and feature-rich experience for neural simulation and experimentation.