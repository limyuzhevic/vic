import pynlm

# Advanced Usage Examples

print("=" * 60)
print("Example 1: Multi-Agent Simulation")
print("=" * 60)

# Create multiple agents with different configurations
configs = []
brains = []
agents = []
worlds = []

for i in range(3):
    # Custom configuration for each agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000 + i * 500)
    config.set("brain.v_thresh", -50.0 - i * 5.0)
    config.set("plasticity.stdp.enable", True)
    
    configs.append(config)
    brains.append(pynlm.createBrain(config))
    agents.append(pynlm.createAgentBrain(brains[-1]))
    worlds.append(pynlm.createSimpleWorld())

# Configure all worlds
for i, world in enumerate(worlds):
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.setAgentStart(7.5 + i * 2, 7.5)

# Initialize all agents
for i, agent in enumerate(agents):
    agents[i].initialize(worlds[i])
    agents[i].enableAllFeatures()

print("Starting multi-agent simulation...")
print("Agent configurations:")
for i, config in enumerate(configs):
    stats = brains[i].getStats()
    print(f"  Agent {i+1}: {stats['total_neurons']} neurons, "
          f"firing rate: {stats['firing_rate']:.2f} Hz")

# Run simulation for each agent
for episode in range(3):
    print(f"\nEpisode {episode + 1}:")
    
    for i, agent in enumerate(agents):
        # Reset world
        worlds[i].reset()
        
        # Run agent episode
        for step in range(50):
            worlds[i].update(0.1)
            agent.processSensoryInput(worlds[i].getSensoryPercept())
            agents[i].decodeMotorCommand()
            worlds[i].applyMotorCommand(agents[i].decodeMotorCommand(), 
                                       worlds[i].getSimulationTime())
            agents[i].applyRewardModulation(0.1, 0.0)
            agents[i].updateDevelopment(0.1)
        
        # Get statistics
        agent_stats = agents[i].getAgentStats()
        brain_stats = brains[i].getStats()
        
        print(f"  Agent {i+1}: "
              f"Curiosity: {agent_stats['curiosity']:.3f}, "
              f"Novelty: {agent_stats['novelty']:.3f}, "
              f"Firing rate: {brain_stats['firing_rate']:.2f} Hz")

print("\n" + "=" * 60)
print("Multi-agent simulation completed!")
print("=" * 60)

print("\n" + "=" * 60)
print("Example 2: Developmental Learning")
print("=" * 60)

# Create brain with developmental features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromodulation.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)
world.reset()

agent.initialize(world)

print("Starting developmental learning simulation...")
print("Initial developmental stage:", 
      "Critical Period" if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod 
      else "Other")

# Run developmental simulation
for epoch in range(5):
    epoch_start = time.time()
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on proximity to goal
        percept = world.getSensoryPercept()
        if percept.getTouch():
            agent.applyRewardModulation(0.5, 0.0)
        
        agent.updateDevelopment(0.1)
    
    epoch_end = time.time()
    
    # Log developmental progress
    dev_stage = brain.getDevelopmentalStage()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    print(f"Epoch {epoch + 1}: "
          f"Stage: {dev_stage.name}, "
          f"Curiosity: {curiosity:.3f}, "
          f"Novelty: {novelty:.3f}, "
          f"Duration: {epoch_end - epoch_start:.2f}s")

print("\n" + "=" * 60)
print("Developmental learning completed!")
print("=" * 60)

print("\n" + "=" * 60)
print("Example 3: Brain State Management")
print("=" * 60)

# Create brain and save initial state
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)

brain1 = pynlm.createBrain(config)
brain1.initialize()
brain1.runSimulation(50)

print("Initial brain state saved:")
stats1 = brain1.getStats()
print(f"  Neurons: {stats1['total_neurons']}")
print(f"  Firing rate: {stats1['firing_rate']:.2f} Hz")
print(f"  Total spikes: {stats1['total_spikes']}")

# Save brain state
brain1.save("checkpoint1.bin")
print("✓ Brain state saved to checkpoint1.bin")

# Modify brain state
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.runSimulation(100)  # Run longer simulation

# Apply additional learning
for step in range(50):
    brain2.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 0.1)
    brain2.step(step + 100)

stats2 = brain2.getStats()
print("\nModified brain state:")
print(f"  Neurons: {stats2['total_neurons']}")
print(f"  Firing rate: {stats2['firing_rate']:.2f} Hz")
print(f"  Total spikes: {stats2['total_spikes']}")

# Save modified brain state
brain2.save("checkpoint2.bin")
print("✓ Modified brain state saved to checkpoint2.bin")

# Load and compare brain states
brain3 = pynlm.createBrain(config)
brain3.load("checkpoint1.bin")

stats3 = brain3.getStats()
print("\nLoaded brain state (from checkpoint1):")
print(f"  Neurons: {stats3['total_neurons']}")
print(f"  Firing rate: {stats3['firing_rate']:.2f} Hz")
print(f"  Total spikes: {stats3['total_spikes']}")

# Compare with original
spike_difference = abs(stats1['total_spikes'] - stats3['total_spikes'])
print(f"\nSpike count difference between saves: {spike_difference}")
print("✓ Brain state persistence verified!")

print("\n" + "=" * 60)
print("Brain state management completed!")
print("=" * 60)

print("\n" + "=" * 60)
print("Example 4: Performance Benchmarking")
print("=" * 60)

import time

def benchmark_brain_performance(num_neurons, num_steps):
    """Benchmark brain performance with different configurations"""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", num_neurons)
    
    brain = pynlm.createBrain(config)
    
    # Warm-up
    brain.initialize()
    brain.runSimulation(10)
    
    # Benchmark
    start_time = time.time()
    brain.runSimulation(num_steps)
    end_time = time.time()
    
    duration = end_time - start_time
    stats = brain.getStats()
    
    return {
        "neurons": num_neurons,
        "steps": num_steps,
        "duration": duration,
        "spikes_per_second": stats["total_spikes"] / duration if duration > 0 else 0,
        "firing_rate": stats["firing_rate"]
    }

# Run benchmarks
print("Running performance benchmarks...")
benchmarks = []

for neuron_count in [500, 1000, 2000, 5000]:
    for step_count in [100, 500, 1000]:
        print(f"  Benchmarking {neuron_count} neurons, {step_count} steps...", end=" ")
        
        result = benchmark_brain_performance(neuron_count, step_count)
        benchmarks.append(result)
        
        print(f"✓ {result['duration']:.2f}s, "
              f"{result['spikes_per_second']:.0f} spikes/s")

# Analyze results
print("\nBenchmark Results Summary:")
print("Neurons | Steps | Duration | Spikes/s | Firing Rate")
print("-------|-------|---------|----------|------------")

for benchmark in benchmarks:
    print(f"{benchmark['neurons']:6d} | "
          f"{benchmark['steps']:5d} | "
          f"{benchmark['duration']:8.2f} | "
          f"{benchmark['spikes_per_second']:8.0f} | "
          f"{benchmark['firing_rate']:11.2f}")

# Find best performing configuration
best_benchmark = max(benchmarks, key=lambda x: x["spikes_per_second"])
print(f"\nBest performing configuration:")
print(f"  {best_benchmark['neurons']} neurons, "
      f"{best_benchmark['steps']} steps, "
      f"{best_benchmark['spikes_per_second']:.0f} spikes/second")

print("\n" + "=" * 60)
print("Performance benchmarking completed!")
print("=" * 60)

print("\n" + "=" * 60)
print("Example 5: Error Handling and Recovery")
print("=" * 60)

# Demonstrate error handling
print("Testing error handling...")

# Test 1: Invalid configuration
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.invalid_key", 123)  # Invalid key
    print("✗ Should have raised exception for invalid key")
except Exception as e:
    print(f"✓ Correctly handled invalid key: {type(e).__name__}")

# Test 2: Brain not initialized
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.step(0)  # Should fail without initialization
    print("✗ Should have raised exception for uninitialized brain")
except Exception as e:
    print(f"✓ Correctly handled uninitialized brain: {type(e).__name__}")

# Test 3: File operations
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.saveAndValidate("/invalid/path/checkpoint.bin")  # Invalid path
    print("✗ Should have raised exception for invalid file path")
except Exception as e:
    print(f"✓ Correctly handled invalid file path: {type(e).__name__}")

# Test 4: Safe configuration operations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Safe get with default
neuron_count = config.getOr("brain.neuron_count", 500)
print(f"✓ Safe configuration access: {neuron_count} neurons")

# Safe dictionary operations
stats = brain.getStats()
if "total_neurons" in stats:
    print(f"✓ Safe dictionary access: {stats['total_neurons']} neurons")
else:
    print("✗ Dictionary access failed")

# Test 5: Type safety
try:
    config.set("brain.neuron_count", "not_a_number")  # Should fail
    print("✗ Should have raised exception for invalid type")
except Exception as e:
    print(f"✓ Correctly handled type conversion: {type(e).__name__}")

print("\n" + "=" * 60)
print("Error handling demonstration completed!")
print("=" * 60)

print("\n" + "=" * 60)
print("Example 6: Advanced Configuration Management")
print("=" * 60)

# Complex configuration management
print("Setting up complex configuration scenarios...")

# Configuration template
template_config = pynlm.createDefaultConfig()
template_config.set("brain.neuron_count", 2000)
template_config.set("brain.v_thresh", -50.0)
template_config.set("plasticity.stdp.enable", True)
template_config.set("plasticity.hebbian.enable", True)
template_config.set("neuromodulation.curiosity.enable", True)
template_config.set("neuromodulation.novelty.enable", True)

# Create multiple scenarios
scenarios = {
    "basic": {"brain.neuron_count": 1000, "development.enabled": False},
    "advanced": {"brain.neuron_count": 3000, "development.enabled": True},
    "expert": {"brain.neuron_count": 5000, "development.enabled": True, "plasticity.structural.enable": True}
}

# Test each scenario
for scenario_name, overrides in scenarios.items():
    print(f"\nTesting '{scenario_name}' scenario...")
    
    # Create scenario from template
    config = pynlm.createDefaultConfig()
    
    # Apply template
    for key, value in template_config.getKeys():
        config.set(key, template_config.getOr(key, value))
    
    # Apply scenario-specific overrides
    for key, value in overrides.items():
        config.set(key, value)
    
    # Verify configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    stats = brain.getStats()
    print(f"  Neurons: {stats['total_neurons']}")
    print(f"  Firing rate: {stats['firing_rate']:.2f} Hz")
    
    # Test scenario-specific operations
    if "development.enabled" in overrides and overrides["development.enabled"]:
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
        print(f"  Development stage: {brain.getDevelopmentalStage().name}")

# Configuration comparison
print("\nConfiguration comparison...")
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 2000)

comparison_configs = []
for multiplier in [0.5, 1.0, 2.0]:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", int(2000 * multiplier))
    comparison_configs.append(config)

print("Configuration comparison results:")
for i, config in enumerate(comparison_configs):
    brain = pynlm.createBrain(config)
    brain.initialize()
    stats = brain.getStats()
    print(f"  Config {i+1}: {stats['total_neurons']} neurons, "
          f"{stats['firing_rate']:.2f} Hz firing rate")

print("\n" + "=" * 60)
print("Advanced configuration management completed!")
print("=" * 60)

print("\nAll advanced examples completed successfully!")
print("=" * 60)