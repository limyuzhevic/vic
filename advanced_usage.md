# NLM - Advanced Usage Guide
# Comprehensive examples and best practices for experienced users

import pynlm

print("=== Advanced NLM Usage Examples ===\n")

# Example 1: Custom configuration with development stages
print("Example 1: Brain with custom development stages")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.initial_plasticity", 0.8)
config.set("development.critical_period_length", 60.0)

brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial development stage: {brain.getDevelopmentalStage()}")
print()

# Example 2: Complex simulation with memory systems
print("Example 2: Simulation with memory tracking")
import time as sim_time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable all memory systems
config = brain.getConfig()
config.set("memory.working_memory.enable", True)
config.set("memory.episodic_memory.enable", True)
config.set("memory.associative_memory.enable", True)

# Run simulation with statistics collection
num_steps = 1000
start_time = sim_time.time()

for step in range(num_steps):
    # Inject different types of input based on step
    if step % 100 == 0:
        # Novelty-driven exploration
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0)
    elif step % 50 == 0:
        # Reward prediction error simulation
        reward = 0.8 if step % 200 == 0 else 0.2
        brain.applyRewardModulation(reward, reward * 0.5)
    elif step % 75 == 0:
        # Development simulation
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    
    brain.step(step)

end_time = sim_time.time()
duration = end_time - start_time

print(f"Simulation completed in {duration:.2f} seconds")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Current development stage: {brain.getDevelopmentalStage()}")
print()

# Example 3: Agent with all subsystems enabled
print("Example 3: Full-featured agent simulation")

# Create complete agent ecosystem
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world with obstacles
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
world.setAgentStart(15.0, 15.0)

# Enable all agent features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

agent.initialize(world)

# Store initial state for comparison
initial_curiosity = agent.getCuriosityLevel()
initial_novelty = agent.getNoveltyLevel()
initial_spikes = brain.getTotalSpikeCount()

# Run exploratory episode
for step in range(500):
    # Update world
    world.update(0.05)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = 1.0 if step % 100 == 0 else 0.1
    agent.applyRewardModulation(reward, reward * 0.3)
    
    # Update development
    agent.updateDevelopment(0.05)
    
    # Optional: Log progress every 50 steps
    if step % 50 == 0:
        print(f"  Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}, "
              f"Firing neurons={brain.getFiringNeuronCount()}")

# Compare final vs initial state
final_curiosity = agent.getCuriosityLevel()
final_novelty = agent.getNoveltyLevel()
final_spikes = brain.getTotalSpikeCount()

print(f"Episode completed:")
print(f"  Curiosity change: {final_curiosity - initial_curiosity:.3f}")
print(f"  Novelty change: {final_novelty - initial_novelty:.3f}")
print(f"  Spike generation: {final_spikes - initial_spikes} additional spikes")
print(f"  Final development stage: {brain.getDevelopmentalStage()}")
print()

# Example 4: Checkpoints and persistence
print("Example 4: Brain state management")

# Create and initialize brain
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain1.initialize()

# Run some simulation steps
for i in range(200):
    brain1.step(i)
    if i % 50 == 0:
        brain1.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)

print(f"Brain1 state before save: {brain1.getTotalSpikeCount()} spikes")

# Save brain state
brain1.save("checkpoint1.bin")
print("Brain state saved to checkpoint1.bin")

# Modify brain state
for i in range(200, 400):
    brain1.step(i)
    if i % 50 == 0:
        brain1.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)

print(f"Brain1 state after modifications: {brain1.getTotalSpikeCount()} spikes")

# Create new brain and load saved state
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
brain2.load("checkpoint1.bin")

print(f"Brain2 (loaded): {brain2.getTotalSpikeCount()} spikes")
print(f"Brains are equivalent: {brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount()}")
print()

# Example 5: Statistical analysis and monitoring
print("Example 5: Brain monitoring and analysis")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Collect statistics over time
stats_log = []

for step in range(500):
    # Record baseline state
    baseline_spikes = brain.getTotalSpikeCount()
    baseline_firing = brain.getFiringNeuronCount()
    baseline_rate = brain.getAverageFiringRate()
    baseline_ei_ratio = brain.getExcitationInhibitionRatio()
    baseline_dev_stage = brain.getDevelopmentalStage()
    
    # Apply stimulation
    if step % 100 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 50.0)
    elif step % 75 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 30.0)
    
    # Advance simulation
    brain.step(step)
    
    # Record post-stimulation state
    post_spikes = brain.getTotalSpikeCount()
    post_firing = brain.getFiringNeuronCount()
    post_rate = brain.getAverageFiringRate()
    post_ei_ratio = brain.getExcitationInhibitionRatio()
    post_dev_stage = brain.getDevelopmentalStage()
    
    # Calculate changes
    spike_change = post_spikes - baseline_spikes
    firing_change = post_firing - baseline_firing
    rate_change = post_rate - baseline_rate
    
    # Store statistics
    stats_log.append({
        'step': step,
        'spike_change': spike_change,
        'firing_change': firing_change,
        'rate_change': rate_change,
        'ei_ratio': post_ei_ratio,
        'dev_stage': str(post_dev_stage)
    })

# Analyze statistics
spike_changes = [s['spike_change'] for s in stats_log]
max_positive_spike_change = max(spike_changes)
max_negative_spike_change = min(spike_changes)

print(f"Statistics collected over {len(stats_log)} steps")
print(f"Max positive spike change: {max_positive_spike_change}")
print(f"Max negative spike change: {max_negative_spike_change}")
print(f"Average E/I ratio: {sum(s['ei_ratio'] for s in stats_log) / len(stats_log):.3f}")
print(f"Development stage progression: {[s['dev_stage'] for s in stats_log]}")
print()

# Example 6: Error handling and validation
print("Example 6: Robust error handling")

try:
    # Test error handling for invalid operations
    print("Testing error handling...")
    
    # Create brain and test various error conditions
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Test uninitialized brain error
    try:
        brain.step(0)
        print("  WARNING: Step succeeded on uninitialized brain (may be expected)")
    except Exception as e:
        print(f"  Caught expected error: {type(e).__name__}")
    
    # Test valid initialization and operation
    brain.initialize()
    brain.step(0)
    print("  ✓ Valid operations work correctly")
    
    # Test configuration validation
    config = brain.getConfig()
    print(f"  Config has key 'brain.neuron_count': {config.has('brain.neuron_count')}")
    
    print("  ✓ Error handling tests completed")
    
except Exception as e:
    print(f"  ✗ Unexpected error: {e}")

print("\n=== Advanced examples completed successfully! ===\n")

# Summary of advanced features demonstrated:
print("Advanced features demonstrated:")
print("  ✓ Custom configuration with development stages")
print("  ✓ Memory system integration and tracking")
print("  ✓ Full agent ecosystem (brain, agent, world)")
print("  ✓ Checkpoint save/load functionality")
print("  ✓ Statistical analysis and monitoring")
print("  ✓ Error handling and validation")
print("  ✓ Neuromodulation and plasticity control")
print("\nFor more information, see HOW_TO_USE.md and easy_usage.md")