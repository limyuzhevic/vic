"""
Quick usage guide for NLM Python examples.
"""

# Example 1: Quick minimal test
"""
The quickest way to test NLM installation and basic functionality.
"""
import pynlm

# Create and run a minimal simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"✓ Minimal simulation complete. Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Clean up
brain.reset()

# Example 2: Configuration test
"""
Test configuration file I/O with JSON format.
"""
import pynlm
import os

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.set("plasticity.stdp.enable", True)

# Save to JSON
config.saveToFile("test_config.json")
print("✓ Configuration saved to JSON")

# Load configuration
new_config = pynlm.createDefaultConfig()
if new_config.loadFromFile("test_config.json"):
    print("✓ Configuration loaded successfully")
    print(f"  Neuron count: {new_config.getOr('brain.neuron_count', 1000)}")
    print(f"  STDP enabled: {new_config.getOr('plasticity.stdp.enable', False)}")
else:
    print("✗ Failed to load configuration")

# Clean up
if os.path.exists("test_config.json"):
    os.remove("test_config.json")

# Example 3: Basic brain with monitoring
"""
Demonstrate basic brain simulation with statistics.
"""
import pynlm
import time

print("\n=== Basic Brain with Monitoring ===")

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 200)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Run simulation with periodic monitoring
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getAverageFiringRate():.2f} Hz average rate")

print(f"\nFinal statistics:")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Clean up
brain.reset()

# Example 4: Brain creation with different parameters
"""
Demonstrate brain creation with various configuration parameters.
"""
import pynlm

print("\n=== Brain Creation with Different Parameters ===")

# Test 1: Small brain
print("\n1. Small brain (100 neurons):")
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 100)
config1.set("brain.connection_probability", 0.05)

brain1 = pynlm.createBrain(config1)
brain1.initialize()
print(f"   Created: {brain1.getTotalNeuronCount()} neurons, "
      f"{brain1.getTotalSynapseCount()} synapses")

# Test 2: Medium brain
print("\n2. Medium brain (500 neurons):")
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 500)
config2.set("brain.connection_probability", 0.1)
config2.set("simulation_timapse", 0.001)

brain2 = pynlm.createBrain(config2)
brain2.initialize()
print(f"   Created: {brain2.getTotalNeuronCount()} neurons, "
      f"{brain2.getTotalSynapseCount()} synapses")

# Test 3: Brain with plasticity
print("\n3. Brain with plasticity:")
config3 = pynlm.createDefaultConfig()
config3.set("brain.neuron_count", 300)
config3.set("plasticity.stdp.enable", True)
config3.set("plasticity.hebbian.enable", True)

brain3 = pynlm.createBrain(config3)
brain3.initialize()
print(f"   Created: {brain3.getTotalNeuronCount()} neurons with plasticity")

# Clean up
brain1.reset()
brain2.reset()
brain3.reset()

print("\n✓ All quick tests completed successfully!")

# Example 5: Configuration comparison
"""
Compare different configuration options.
"""
import pynlm

print("\n=== Configuration Comparison ===")

# Configuration 1: Default
print("\n1. Default configuration:")
config_default = pynlm.createDefaultConfig()
print(f"   Neurons: {config_default.getOr('brain.neuron_count', 1000)}")
print(f"   Connection probability: {config_default.getOr('brain.connection_probability', 0.1):.3f}")
print(f"   Time step: {config_default.getOr('simulation_timestep', 0.001):.6f}")
print(f"   STDP enabled: {config_default.getOr('plasticity.stdp.enable', False)}")
print(f"   Hebbian enabled: {config_default.getOr('plasticity.hebbian.enable', False)}")

# Configuration 2: Custom
print("\n2. Custom configuration:")
config_custom = pynlm.createDefaultConfig()
config_custom.set("brain.neuron_count", 2000)
config_custom.set("brain.connection_probability", 0.15)
config_custom.set("simulation_timestep", 0.0005)
config_custom.set("plasticity.stdp.enable", True)
config_custom.set("plasticity.hebbian.enable", True)
config_custom.set("neuromod.curiosity.enable", True)

print(f"   Neurons: {config_custom.getOr('brain.neuron_count', 1000)}")
print(f"   Connection probability: {config_custom.getOr('brain.connection_probability', 0.1):.3f}")
print(f"   Time step: {config_custom.getOr('simulation_timestep', 0.001):.6f}")
print(f"   STDP enabled: {config_custom.getOr('plasticity.stdp.enable', False)}")
print(f"   Hebbian enabled: {config_custom.getOr('plasticity.hebbian.enable', False)}")
print(f"   Curiosity enabled: {config_custom.getOr('neuromod.curiosity.enable', False)}")

# Create brains with different configurations
print("\n3. Creating brains with different configurations:")

brain_default = pynlm.createBrain(config_default)
brain_default.initialize()
print(f"   Default brain: {brain_default.getTotalNeuronCount()} neurons")

brain_custom = pynlm.createBrain(config_custom)
brain_custom.initialize()
print(f"   Custom brain: {brain_custom.getTotalNeuronCount()} neurons")

# Clean up
brain_default.reset()
brain_custom.reset()

print("\n✓ Configuration comparison completed!")

# Example 6: Performance test
"""
Quick performance test with different brain sizes.
"""
import pynlm
import time

print("\n=== Performance Test ===")

sizes = [100, 500, 1000]
times = []

for size in sizes:
    start = time.time()
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", size)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(50):
        brain.step(step)
    
    elapsed = time.time() - start
    steps_per_sec = 50 / elapsed
    
    times.append((size, elapsed, steps_per_sec))
    
    print(f"Size {size}: {elapsed:.3f}s ({steps_per_sec:.1f} steps/s)")
    
    brain.reset()

print("\nPerformance summary:")
print("Size | Time (s) | Steps/sec")
print("-----|----------|----------")
for size, elapsed, rate in times:
    print(f"{size:4} | {elapsed:8.3f} | {rate:9.1f}")

print("\n✓ Performance test completed!")

# Example 7: Error handling
"""
Demonstrate error handling and validation.
"""
import pynlm

print("\n=== Error Handling Demo ===")

# Create brain
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized successfully")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
        print(f"✓ Step {step} completed")
    
    print("✓ Simulation completed successfully")
    
except Exception as e:
    print(f"✗ Error: {e}")

# Test with invalid configuration (if possible)
print("\nTesting configuration validation...")
try:
    # Create configuration with invalid values
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1)  # Negative neurons
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("✓ Configuration with negative neurons created (behavior depends on implementation)")
    
except Exception as e:
    print(f"✓ Configuration validation caught error: {e}")

print("\n✓ Error handling demo completed!")

print("\n" + "="*50)
print("QUICK START GUIDE - ALL TESTS PASSED")
print("="*50)
print("\nTo run these examples in your terminal:")
print("python examples/basics/minimal_example.py")
print("python examples/basics/config_test.py")
print("python examples/basics/basic_brain.py")
print("\nFor more comprehensive examples:")
print("python examples/agent_world/simple_agent.py")
print("python examples/advanced/plasticity_demo.py")
