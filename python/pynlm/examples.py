"""
Example demonstrations for NLM (Neural Learning Machine) Python module.

This module contains comprehensive examples demonstrating how to use
the NLM Python bindings for various simulation scenarios.
"""

import pynlm


examples = {
    "minimal": """
# NLM Minimal Example
import pynlm

# Create and initialize a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a short simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
""",

    "basic_brain": """
# NLM Basic Brain Example
import pynlm

# Create a brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation and collect metrics
metrics_history = []
for step in range(200):
    brain.step(step)
    
    # Collect metrics every 50 steps
    if step % 50 == 0:
        metrics = pynlm.SimulationMetrics.collect(brain)
        metrics['step'] = step
        metrics_history.append(metrics)
        
        print(f"Step {step}: {metrics['firing_neuron_count']} firing neurons, "
              f"{metrics['average_firing_rate']:.2f} Hz average rate")

# Generate summary report
print("\nFinal Metrics:")
print(pynlm.SimulationMetrics.format(metrics_history[-1], precision=2))
""",

    "agent_world": """
# NLM Agent in World Example
import pynlm

# Setup complete agent simulation
def run_complete_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getAgentBody().energy / 100.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_agent_simulation(500)
print(f"\nSimulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
""",

    "brain_config_example": """
# NLM Brain Configuration Example
import pynlm

# Example 1: Create a small, fast brain
fast_brain_config = pynlm.BrainConfig.with_parameters(
    neuron_count=500,
    synapse_density=0.05,
    learning_rate=0.01,
    v_thresh=-45.0,
    tau_mem=15.0
)

# Example 2: Create a large, complex brain
complex_brain_config = pynlm.BrainConfig.with_parameters(
    neuron_count=5000,
    synapse_density=0.2,
    learning_rate=0.001,
    v_thresh=-50.0,
    tau_mem=25.0,
    stdp_enable=True,
    hebbian_enable=True,
    structural_enable=True,
    neuromod_curiosity_enable=True,
    neuromod_novelty_enable=True,
)

# Example 3: Load configuration from file
# config = pynlm.BrainConfig.from_file("my_config.json")
# brain = pynlm.createBrain(config)

print("Available configuration presets:")
print("1. Fast Brain: 500 neurons, high learning rate")
print("2. Complex Brain: 5000 neurons, full plasticity")
print("3. Default Configuration: Standard settings")

# Save configuration to file (if desired)
# pynlm.BrainConfig.save_to_file(complex_brain_config, "complex_config.json")
""",

    "metrics_example": """
# NLM Metrics Collection Example
import pynlm

# Create a brain and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# World and agent for metrics
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Collecting metrics over 300 steps...")

# Collect metrics at different intervals
metrics_samples = []
for step in range(300):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getAgentBody().energy / 100.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Collect metrics every 50 steps
    if step % 50 == 0:
        metrics = pynlm.SimulationMetrics.collect(brain, world, agent)
        metrics['step'] = step
        metrics_samples.append(metrics)
        
        print(f"Step {step}: {metrics['firing_neuron_count']} firing neurons, "
              f"{metrics['average_firing_rate']:.2f} Hz")

# Format and display metrics summary
print("\n" + "="*60)
print("METRICS SUMMARY")
print("="*60)

for i, metrics in enumerate(metrics_samples):
    print(f"\nSample {i + 1} (Step {metrics['step']}):")
    print(pynlm.SimulationMetrics.format(metrics, precision=2))

# Analyze trends
if len(metrics_samples) >= 3:
    trends = pynlm.SimulationMetrics.analyze_trends(metrics_samples)
    print(f"\n\nTrend Analysis:")
    print(f"Status: {trends['status']}")
    print(f"Number of metrics analyzed: {trends['num_trends']}")
    
    for metric, trend_info in trends['trends'].items():
        print(f"  {metric}: {trend_info['trend']} ({trend_info['strength']})")
""",

    "performance_monitoring_example": """
# NLM Performance Monitoring Example
import pynlm
import time

# Create a brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create performance monitor
monitor = pynlm.PerformanceMonitor(brain, world)

print("Starting performance monitoring...")
monitor.start()

# Run simulation with performance monitoring
num_steps = 200
for step in range(num_steps):
    # Record step start time
    step_start = time.time()
    
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action and apply it
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getAgentBody().energy / 100.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Record step time
    step_time = time.time() - step_start
    monitor.record_step(step_time)
    
    # Print progress
    if step % 50 == 0:
        print(f"Step {step} completed in {step_time*1000:.2f} ms")

# Stop monitoring and get results
results = monitor.stop()

print("\n" + "="*60)
print("PERFORMANCE MONITORING RESULTS")
print("="*60)
print(monitor.get_detailed_report())

# Print key metrics
print(f"\nKey Performance Metrics:")
print(f"  Total Steps: {results['steps']['steps']}")
print(f"  Total Time: {results['steps']['total_time']:.2f} seconds")
print(f"  Steps per Second: {results['steps']['steps_per_second']:.2f}")
print(f"  Average Step Time: {results['steps']['avg_step_time']*1000:.2f} ms")
print(f"  Min Step Time: {results['steps']['min_step_time']*1000:.2f} ms")
print(f"  Max Step Time: {results['steps']['max_step_time']*1000:.2f} ms")
print(f"  Step Time Standard Deviation: {results['steps']['std_step_time']*1000:.2f} ms")
print(f"  Efficiency Score: {results['efficiency']['efficiency_score']:.2f}")
print(f"  Memory Efficiency: {results['memory_efficiency']['memory_efficiency_score']:.2f}")
""",

    "benchmark_example": """
# NLM Benchmarking Example
import pynlm

print("Running NLM benchmarks...")

# Benchmark 1: Brain creation performance
print("\n1. Brain Creation Benchmark")
print("-" * 40)
brain_creation_results = pynlm.benchmark_brain_creation(
    num_brains=20,
    neuron_count=1000
)

print(f"Created 20 brains in {brain_creation_results['creation_time']:.2f} seconds")
print(f"Brains per second: {brain_creation_results['brains_per_second']:.2f}")
print(f"Average creation time: {brain_creation_results['avg_creation_time']:.4f} seconds")

# Benchmark 2: Simulation performance
print("\n2. Simulation Benchmark")
print("-" * 40)

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

simulation_results = pynlm.benchmark_simulation(brain, world, num_steps=500)

print(f"Simulated 500 steps in {simulation_results['simulation_time']:.2f} seconds")
print(f"Steps per second: {simulation_results['steps_per_second']:.2f}")
print(f"Average step time: {simulation_results['avg_step_time']*1000:.2f} ms")

# Benchmark 3: Complete experiment
print("\n3. Complete Experiment Benchmark")
print("-" * 40)

exp_results = pynlm.run_experiment(
    config=config,
    world=world,
    num_steps=300,
    enable_learning=True,
    enable_development=True,
    enable_curiosity=True,
    log_interval=100
)

print(f"Experiment completed in {exp_results['simulation_time']:.2f} seconds")
print(f"Steps per second: {exp_results['steps_per_second']:.2f}")
print(f"Average step time: {exp_results['avg_step_time']*1000:.2f} ms")
print(f"Final firing rate: {exp_results['final_firing_rate']:.2f} Hz")
print(f"Final curiosity: {exp_results['final_curiosity']:.3f}")
print(f"Final novelty: {exp_results['final_novelty']:.3f}")

print("\n" + "="*60)
print("BENCHMARKING COMPLETE")
print("="*60)
""",

    "registration_example": """
# NLM Brain Registry Example
import pynlm

print("Demonstrating NLM brain registry...")

# Create multiple brains
config = pynlm.createDefaultConfig()

brain1 = pynlm.createBrain(config)
brain1.initialize()

brain2 = pynlm.createBrain(config)
brain2.initialize()

brain3 = pynlm.createBrain(config)
brain3.initialize()

# Register brains
pynlm.register_brain("main_brain", brain1, {'type': 'main', 'description': 'Primary brain'})
pynlm.register_brain("secondary_brain", brain2, {'type': 'secondary', 'description': 'Secondary brain'})
pynlm.register_brain("test_brain", brain3, {'type': 'test', 'description': 'Test brain'})

print(f"Registered {len(pynlm.list_brains())} brains")
print("Available brains:", pynlm.list_brains())

# Retrieve brains
retrieved_brain = pynlm.get_brain("main_brain")
print(f"\nRetrieved brain: {type(retrieved_brain).__name__}")

# Get brain metadata
metadata = pynlm.get_brain_metadata("secondary_brain")
print(f"Secondary brain metadata: {metadata}")

# Update metadata
pynlm.update_brain_metadata("main_brain", {'last_used': time.time(), 'usage_count': 5})

# Use context manager for automatic cleanup
print("\nUsing BrainContext for automatic cleanup...")
with pynlm.BrainContext("temp_brain", brain1, {'temp': True}):
    temp_brain = pynlm.get_brain("temp_brain")
    print(f"Temp brain in context: {temp_brain is not None}")

# After context exit, temp_brain should be unregistered
print(f"After context exit - temp brain registered: {pynlm.has_brain('temp_brain')}")

# Get all metadata
all_metadata = pynlm.get_all_brain_metadata()
print(f"\nAll brain metadata:")
for name, meta in all_metadata.items():
    print(f"  {name}: {meta}")

# Clean up remaining brains
pynlm.unregister_brain("main_brain")
pynlm.unregister_brain("secondary_brain")
pynlm.unregister_brain("test_brain")

print(f"\nRemaining registered brains: {len(pynlm.list_brains())}")

# Clean up global registry
pynlm.clear_registry()
print("Global registry cleared")
""",

    "experiment_example": """
# NLM Experiment Framework Example
import pynlm

print("Demonstrating NLM experiment framework...")

# Create configuration
config = pynlm.createDefaultConfig()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(7.5, 7.5)

# Run single experiment
print("\n1. Running Single Experiment")
print("-" * 40)
results = pynlm.run_experiment(
    config=config,
    world=world,
    num_steps=500,
    enable_learning=True,
    enable_development=True,
    enable_curiosity=True,
    log_interval=100,
    verbose=True
)

print(f"\nExperiment completed: {results.success}")
print(f"Errors encountered: {len(results.errors)}")
print(f"Steps completed: {len(results.metrics)}")

# Get experiment summary
summary = results.get_summary()
print(f"\nExperiment Summary:")
for key, value in summary.items():
    if key not in ['errors', 'final_metrics', 'performance_profile']:
        print(f"  {key}: {value}")

# Run multiple experiments
print("\n2. Running Multiple Experiments")
print("-" * 40)

# Define experiment configurations
experiment_configs = [
    {'num_steps': 200, 'enable_learning': True, 'enable_development': False, 'enable_curiosity': False},
    {'num_steps': 500, 'enable_learning': True, 'enable_development': True, 'enable_curiosity': False},
    {'num_steps': 300, 'enable_learning': False, 'enable_development': True, 'enable_curiosity': True},
]

num_steps_list = [config['num_steps'] for config in experiment_configs]

multi_results = pynlm.run_multiple_experiments(
    config=config,
    world=world,
    num_steps_list=num_steps_list,
    experiment_configs=experiment_configs,
    verbose=True
)

# Compare experiments
print(f"\nComparing {len(multi_results)} experiments...")
comparison = pynlm.compare_experiments(multi_results)

print(f"\nComparison Summary:")
print(f"  Total experiments: {comparison['summary']['total_experiments']}")
print(f"  Successful experiments: {comparison['summary']['successful_experiments']}")
print(f"  Failed experiments: {comparison['summary']['failed_experiments']}")

# Export results
print(f"\nExporting experiment results...")
experiment_filename = "experiment_results.csv"
if pynlm.ExperimentHelper.export_experiment_results(results, experiment_filename, 'csv'):
    print(f"Successfully exported to {experiment_filename}")

# Import results (if file exists)
if os.path.exists(experiment_filename):
    imported_results = pynlm.ExperimentHelper.import_experiment_results(experiment_filename, 'csv')
    print(f"Successfully imported {len(imported_results.metrics)} steps from {experiment_filename}")

print("\n" + "="*60)
print("EXPERIMENT FRAMEWORK DEMONSTRATION COMPLETE")
print("="*60)
""",

    "visualization_example": """
# NLM Visualization Example
import pynlm
import matplotlib.pyplot as plt
import numpy as np

print("Demonstrating NLM visualization capabilities...")

# Create a simple visualization example
fig, axes = plt.subplots(2, 2, figsize=(12, 10))
fig.suptitle('NLM Visualization Example', fontsize=16, fontweight='bold')

# Example 1: Firing rate over time
steps = np.linspace(0, 100, 50)
firing_rates = np.random.randn(50) * 10 + 20  # Random firing rates
ax1 = axes[0, 0]
ax1.plot(steps, firing_rates, 'b-', linewidth=2, alpha=0.7)
ax1.fill_between(steps, firing_rates, alpha=0.3, color='blue')
ax1.set_title('Firing Rate Over Time')
ax1.set_xlabel('Simulation Step')
ax1.set_ylabel('Firing Rate (Hz)')
ax1.grid(True, alpha=0.3)

# Example 2: Neuron population distribution
ax2 = axes[0, 1]
populations = ['Sensory', 'Motor', 'Association', 'Memory']
neuron_counts = [1000, 800, 1200, 900]
colors = ['skyblue', 'lightgreen', 'lightcoral', 'gold']
ax2.bar(populations, neuron_counts, color=colors, alpha=0.8)
ax2.set_title('Neural Population Distribution')
ax2.set_ylabel('Neuron Count')
ax2.grid(True, alpha=0.3)

# Example 3: Agent movement in world (simplified)
ax3 = axes[1, 0]
# Create a simple world representation
world_width, world_height = 20, 20
ax3.set_xlim(0, world_width)
ax3.set_ylim(0, world_height)

# Draw world boundaries
ax3.plot([0, world_width, world_width, 0, 0], 
         [0, 0, world_height, world_height, 0], 'k-', linewidth=2)

# Draw some resources (green circles)
for i in range(8):
    x = np.random.uniform(3, world_width - 3)
    y = np.random.uniform(3, world_height - 3)
    radius = np.random.uniform(0.5, 1.5)
    ax3.add_patch(plt.Circle((x, y), radius, color='green', alpha=0.6))

# Draw agent
agent_x, agent_y = 10, 10
agent_radius = 0.8
ax3.add_patch(plt.Circle((agent_x, agent_y), agent_radius, color='red', alpha=0.8))
ax3.text(agent_x, agent_y + 1.5, 'AGENT', ha='center', va='bottom', 
         fontsize=10, fontweight='bold', color='red')

ax3.set_title('World Environment')
ax3.set_xlabel('X Position')
ax3.set_ylabel('Y Position')
ax3.grid(True, alpha=0.3)

# Example 4: Metrics comparison
ax4 = axes[1, 1]
# Simulate metrics over time
steps = np.linspace(0, 100, 50)
curiosity = 0.1 + 0.8 * np.sin(steps / 20)  # Oscillating curiosity
novelty = 0.2 + 0.6 * np.cos(steps / 15)    # Oscillating novelty
development = np.minimum(1.0, steps / 50)    # Growing development

ax4.plot(steps, curiosity, 'r-', linewidth=2, label='Curiosity')
ax4.plot(steps, novelty, 'g-', linewidth=2, label='Novelty')
ax4.plot(steps, development, 'b-', linewidth=2, label='Development Stage')

ax4.set_title('Subsystem Dynamics')
ax4.set_xlabel('Simulation Step')
ax4.set_ylabel('Level')
ax4.legend()
ax4.grid(True, alpha=0.3)
ax4.set_ylim(0, 1.1)

plt.tight_layout()
plt.savefig('nlm_visualization_example.png', dpi=150, bbox_inches='tight')
print("Saved visualization to 'nlm_visualization_example.png'")

# Show plot (if in interactive environment)
try:
    plt.show()
except:
    print("(Plot would be displayed in interactive environment)")

print("\n" + "="*60)
print("VISUALIZATION EXAMPLE COMPLETE")
print("="*60)
""",
}

# Export examples
__all__ = ['examples']

# Print examples summary when module is imported
if __name__ == "__main__":
    print("NLM Examples Module")
    print("Available examples:")
    for name in examples.keys():
        print(f"  - {name}")
    print(f"\nTotal examples: {len(examples)}")
    
    # Show a sample example
    print(f"\nSample 'minimal' example:")
    print("-" * 40)
    print(examples['minimal'].strip())