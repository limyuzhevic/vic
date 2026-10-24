#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM Python API.

This script showcases all the new convenience methods, presets, and
analysis tools added to the NLM Python bindings.
"""

import pynlm

print("=== Enhanced NLM Python API Examples ===\n")

# Example 1: Using preset configurations
print("1. Creating brains with preset configurations:")
print("-" * 50)

# Using different presets
presets = ["simple_learning", "complex_cognition", "development", "fast_simulation"]

for preset in presets:
    print(f"\nCreating brain with '{preset}' preset:")
    brain = pynlm.createBrainWithPreset(preset)
    brain.initialize()
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Development Stage: {brain.getDevelopmentalStage()}")
    print(f"   Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average Firing Rate: {brain.getAverageFiringRate():.1f} Hz")

print("\n" + "=" * 60 + "\n")

# Example 2: Using the high-level simulation helpers
print("2. Using high-level simulation helpers:")
print("-" * 50)

# Create a simple agent simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

print("Running agent episode with 100 steps:")
brain, agent, world = pynlm.run_agent_episode(brain, world, agent, 100)

print(f"   Total spikes: {brain.getTotalSpikeCount()}")
print(f"   Neurons firing: {brain.getFiringNeuronCount()}")
print(f"   Average firing rate: {brain.getAverageFiringRate():.1f} Hz")
print(f"   Development stage: {brain.getDevelopmentalStage()}")
print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")

print("\n" + "=" * 60 + "\n")

# Example 3: Using format_stats and export_data
print("3. Using formatted statistics and data export:")
print("-" * 50)

print("Formatted statistics:")
print(brain.format_stats())

print("\nCSV data export (first 500 chars):")
csv_data = brain.export_data("csv")
print(csv_data[:500] + "...")

print("\nJSON data export (first 300 chars):")
json_data = brain.export_data("json")
print(json_data[:300] + "...")

print("\n" + "=" * 60 + "\n")

# Example 4: Using safe_step with validation
print("4. Using safe_step with input validation:")
print("-" * 50)

brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

print("Running safe simulation steps:")
for i in range(5):
    pynlm.safe_step(brain2, i)
    print(f"   Step {i}: {brain2.getFiringNeuronCount()} neurons firing")

print("\n" + "=" * 60 + "\n")

# Example 5: Creating a complete learning simulation
print("5. Complete learning simulation example:")
print("-" * 50)

print("Setting up a learning environment:")

# Use the simple_learning preset for this example
config = pynlm.get_config_preset("simple_learning")
brain3 = pynlm.createBrain(config)
brain3.initialize()

world3 = pynlm.createSimpleWorld()
agent3 = pynlm.createAgentBrain(brain3)

print("Running training simulation (1000 steps):")
for step in range(1000):
    world3.update(0.1)
    agent3.processSensoryInput(world3.getSensoryPercept())
    brain3.step(step)
    action = agent3.decodeMotorCommand()
    world3.applyMotorCommand(action, world3.getSimulationTime())
    
    # Log progress every 200 steps
    if step % 200 == 0 and step > 0:
        print(f"   Step {step}:")
        print(f"     Firing neurons: {brain3.getFiringNeuronCount()}")
        print(f"     Average firing rate: {brain3.getAverageFiringRate():.1f} Hz")
        print(f"     Curiosity: {agent3.getCuriosityLevel():.3f}")
        print(f"     Novelty: {agent3.getNoveltyLevel():.3f}")
        print(f"     Development stage: {brain3.getDevelopmentalStage()}")

print("\nFinal statistics:")
print(brain3.format_stats())

print("\n" + "=" * 60 + "\n")

# Example 6: Comparison between presets
print("6. Comparing different presets:")
print("-" * 50)

print("Testing all presets for performance comparison:")

results = {}
for preset in presets:
    print(f"\nTesting '{preset}' preset:")
    brain_test = pynlm.createBrainWithPreset(preset)
    brain_test.initialize()
    
    # Run a quick simulation
    for step in range(100):
        brain_test.step(step)
    
    # Collect statistics
    stats = {
        "neurons": brain_test.getTotalNeuronCount(),
        "firing": brain_test.getFiringNeuronCount(),
        "rate": brain_test.getAverageFiringRate(),
        "spikes": brain_test.getTotalSpikeCount(),
        "e_i_ratio": brain_test.getExcitationInhibitionRatio(),
        "stage": str(brain_test.getDevelopmentalStage())
    }
    
    results[preset] = stats
    
    print(f"   Neurons: {stats['neurons']}")
    print(f"   Firing: {stats['firing']}")
    print(f"   Avg Rate: {stats['rate']:.1f} Hz")
    print(f"   E/I Ratio: {stats['e_i_ratio']:.3f}")
    print(f"   Development Stage: {stats['stage']}")

print("\nComparison summary:")
print("-" * 30)
print(f"{'Preset':<20} {'Neurons':<10} {'Firing':<10} {'Avg Rate':<10} {'E/I Ratio':<10}")
print("-" * 30)
for preset, stats in results.items():
    print(f"{preset:<20} {stats['neurons']:<10} {stats['firing']:<10} "
           f"{stats['rate']:<10.1f} {stats['e_i_ratio']:<10.3f}")

print("\n" + "=" * 60 + "\n")

print("All examples completed successfully!")
print("\nKey improvements demonstrated:")
print("✓ Preset configurations for different use cases")
print("✓ High-level simulation helpers")
print("✓ Formatted statistics output")
print("✓ Data export in multiple formats")
print("✓ Safe step execution with validation")
print("✓ Complete learning simulations")
print("✓ Preset comparison and analysis")
print("\nThe enhanced NLM Python API provides powerful tools for both")
print("beginners and advanced users while maintaining backward compatibility.")
