# NLM Demo Script Collection
# Various demo scripts for different scenarios and use cases

# demo_1_basic_brain.py
"""
Basic Brain Demo - Simple standalone brain without world interaction.
This demonstrates the core neural computation capabilities of NLM.
"""

import pynlm

def demo_basic_brain():
    """Create and run a simple brain."""
    print("=== Basic Brain Demo ===")
    
    # Create brain with default config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Expected {brain.getTotalSynapseCount()} synapses")
    
    # Run a simulation
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            print(f"Step {step}: {firing} firing neurons, {spikes} total spikes")
    
    # Print final statistics
    print(f"\nFinal statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    print("\nBasic brain demo complete!")

# demo_2_agent_interaction.py
"""
Agent-World Interaction Demo - Shows how an agent interacts with a world.
This demonstrates the full perception-action loop.
"""

import pynlm

def demo_agent_interaction():
    """Run an agent interacting with a world."""
    print("=== Agent-World Interaction Demo ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Setup complete:")
    print(f"  World: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  Agent start: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    
    # Run simulation
    total_reward = 0.0
    steps = 200
    
    for step in range(steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Run brain
        brain.step(step)
        
        # 5. Decode motor command
        action = agent.decodeMotorCommand()
        
        # 6. Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Track rewards
        total_reward += result.reward
        
        # Print progress every 40 steps
        if step % 40 == 0:
            print(f"\nStep {step}:")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Action: {pynlm.actionTypeToString(action)}")
            print(f"  Reward: {result.reward:.1f}")
            print(f"  Total reward so far: {total_reward:.1f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Print final summary
    print(f"\n=== Simulation Complete ===")
    print(f"Total steps: {steps}")
    print(f"Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"Final energy: {world.getAgentBody().energy:.1f}")
    print(f"Total reward: {total_reward:.1f}")
    print(f"Average reward: {total_reward/steps:.3f}")
    print(f"Final neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("\nAgent-world interaction demo complete!")

# demo_3_learning_progress.py
"""
Learning Progress Demo - Shows how the brain learns and adapts over time.
This demonstrates plasticity and development.
"""

import pynlm

def demo_learning_progress():
    """Run a simulation showing learning progress over time."""
    print("=== Learning Progress Demo ===")
    
    # Create configuration for learning
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Phase-based learning demonstration
    phase_length = 100
    num_phases = 3
    
    print("Phase-based learning demonstration:")
    print("Phase 1: Exploration (high curiosity)")
    print("Phase 2: Exploitation (learning from rewards)")
    print("Phase 3: Integration (combined exploration/exploitation)")
    
    # Track learning metrics
    learning_metrics = []
    
    for phase in range(num_phases):
        print(f"\n--- Phase {phase + 1} ---")
        
        # Phase-specific settings
        if phase == 0:  # Exploration
            agent.enableCuriosity(True)
            agent.enableDevelopment(False)
        elif phase == 1:  # Exploitation
            agent.enableCuriosity(False)
            agent.enableDevelopment(True)
        else:  # Integration
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        phase_metrics = {
            'phase': phase + 1,
            'start_neurons': brain.getTotalNeuronCount(),
            'start_synapses': brain.getTotalSynapseCount(),
            'start_firing': brain.getFiringNeuronCount(),
            'start_reward': 0.0,
            'end_neurons': 0,
            'end_synapses': 0,
            'end_firing': 0,
            'end_reward': 0.0,
            'novelty': 0.0,
            'curiosity': 0.0,
            'development_stage': 0
        }
        
        # Run phase
        for step in range(phase_length):
            # Update world
            world.update(0.1)
            
            # Get percept
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track phase metrics
            phase_metrics['end_reward'] += result.reward
        
        # Record end metrics
        phase_metrics['end_neurons'] = brain.getTotalNeuronCount()
        phase_metrics['end_synapses'] = brain.getTotalSynapseCount()
        phase_metrics['end_firing'] = brain.getFiringNeuronCount()
        phase_metrics['novelty'] = agent.getNoveltyLevel()
        phase_metrics['curiosity'] = agent.getCuriosityLevel()
        phase_metrics['development_stage'] = agent.getDevelopmentalStage()
        
        learning_metrics.append(phase_metrics)
        
        # Print phase summary
        print(f"Phase {phase + 1} summary:")
        print(f"  Synapses: {phase_metrics['start_synapses']} -> {phase_metrics['end_synapses']} (change: {phase_metrics['end_synapses'] - phase_metrics['start_synapses']:+d})")
        print(f"  Total reward: {phase_metrics['end_reward']:.1f}")
        print(f"  Novelty level: {phase_metrics['novelty']:.3f}")
        print(f"  Curiosity level: {phase_metrics['curiosity']:.3f}")
        print(f"  Developmental stage: {phase_metrics['development_stage']}")
    
    # Print overall learning analysis
    print(f"\n=== Learning Analysis ===")
    
    for i, metric in enumerate(learning_metrics):
        neuron_change = metric['end_neurons'] - metric['start_neurons']
        synapse_change = metric['end_synapses'] - metric['start_synapses']
        
        print(f"Phase {metric['phase']}:")
        print(f"  Neuron count: {metric['start_neurons']} -> {metric['end_neurons']} (change: {neuron_change:+d})")
        print(f"  Synapse count: {metric['start_synapses']} -> {metric['end_synapses']} (change: {synapse_change:+d})")
        print(f"  Total reward earned: {metric['end_reward']:.1f}")
        
        if synapse_change > 0:
            print(f"  → Plasticity: Increased synaptic connections")
        elif synapse_change < 0:
            print(f"  → Plasticity: Pruned synaptic connections")
        else:
            print(f"  → Plasticity: No significant change")
    
    print("\nLearning progress demo complete!")

# demo_4_advanced_configuration.py
"""
Advanced Configuration Demo - Shows how to configure NLM for specific use cases.
This demonstrates advanced configuration options and custom setups.
"""

import pynlm
import json

def demo_advanced_configuration():
    """Demonstrate advanced configuration options."""
    print("=== Advanced Configuration Demo ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration settings
    print("Configuring brain for advanced simulation...")
    
    # Learning-related settings
    config.set("stdp_ltp_weight", 0.005)  # Learning rate
    config.set("stdp_ltd_weight", 0.006)  # Forgetting rate
    config.set("plasticity.stdp.enable", True)  # Enable STDP
    config.set("plasticity.hebbian.enable", True)  # Enable Hebbian
    config.set("plasticity.structural.enable", True)  # Enable structural
    
    # Brain architecture
    config.set("neuron_count", 2000)  # Larger brain
    config.set("region_count", 4)  # Multiple regions
    config.set("connection_probability", 0.05)  # Moderate connectivity
    config.set("random_seed", 12345)  # Reproducible results
    
    # Development settings
    config.set("development.enable", True)
    config.set("synaptogenesis_rate", 0.0002)  # Growth rate
    config.set("pruning_rate", 0.00005)  # Pruning rate
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)  # Reward sensitivity
    config.set("neuromod.curiosity.enable", True)  # Curiosity-driven exploration
    config.set("neuromod.novelty.enable", True)  # Novelty detection
    
    # Simulation
    config.set("simulation_timestep", 0.001)  # Fine time resolution
    config.set("max_steps", 10000)  # Longer simulation
    
    # Checkpointing
    config.set("checkpoint_dir", "./checkpoints")
    config.set("checkpoint_interval", 1000)  # Save every 1000 steps
    config.set("checkpoint_compression", "balanced")
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Configuration complete:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent.initialize(world)
    
    print("\nAdvanced setup complete:")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision range: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  Agent start: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Save configuration to file
    config.saveToFile("advanced_config.json")
    print(f"\nConfiguration saved to 'advanced_config.json'")
    
    # Load configuration from file
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("advanced_config.json")
    
    print("\nConfiguration can be loaded from file and reused")
    
    print("\nAdvanced configuration demo complete!")

# demo_5_performance_monitor.py
"""
Performance Monitoring Demo - Shows how to monitor and optimize NLM performance.
This demonstrates performance monitoring and profiling tools.
"""

import pynlm
import time
import psutil
import os

def demo_performance_monitor():
    """Demonstrate performance monitoring capabilities."""
    print("=== Performance Monitoring Demo ===")
    
    # Get current process
    process = psutil.Process(os.getpid())
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Performance tracking
    performance_data = []
    start_time = time.time()
    start_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    steps = 500
    print(f"Running {steps} simulation steps with performance monitoring...")
    
    for step in range(steps):
        # Record performance before step
        mem_before = process.memory_info().rss / 1024 / 1024
        cpu_percent = process.cpu_percent()
        
        # Update world
        world.update(0.1)
        
        # Get and process percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Record performance after step
        mem_after = process.memory_info().rss / 1024 / 1024
        
        # Store performance data
        perf_record = {
            'step': step,
            'memory_before': mem_before,
            'memory_after': mem_after,
            'memory_delta': mem_after - mem_before,
            'cpu_percent': cpu_percent,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'reward': result.reward,
            'novelty': agent.getNoveltyLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'neuromodulation': agent.getNeuromodulationLevel()
        }
        
        performance_data.append(perf_record)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Memory: {mem_after:.1f}MB ({perf_record['memory_delta']:+.1f}), "
                  f"CPU: {cpu_percent:.1f}%, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Reward: {result.reward:.1f}")
    
    # Calculate performance statistics
    end_time = time.time()
    end_memory = process.memory_info().rss / 1024 / 1024
    
    total_time = end_time - start_time
    memory_change = end_memory - start_memory
    
    # Analyze performance data
    memory_values = [d['memory_after'] for d in performance_data]
    cpu_values = [d['cpu_percent'] for d in performance_data]
    spike_values = [d['total_spikes'] for d in performance_data]
    reward_values = [d['reward'] for d in performance_data]
    
    max_memory = max(memory_values)
    min_memory = min(memory_values)
    avg_cpu = sum(cpu_values) / len(cpu_values)
    max_cpu = max(cpu_values)
    total_spikes = spike_values[-1]
    total_reward = sum(reward_values)
    
    # Print performance summary
    print(f"\n=== Performance Summary ===")
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Average steps per second: {steps/total_time:.1f}")
    print(f"Memory usage: {start_memory:.1f}MB -> {end_memory:.1f}MB ({memory_change:+.1f}MB)")
    print(f"Memory range: {min_memory:.1f}MB - {max_memory:.1f}MB")
    print(f"CPU usage: Avg {avg_cpu:.1f}%, Max {max_cpu:.1f}%")
    print(f"Total spikes generated: {total_spikes}")
    print(f"Total reward accumulated: {total_reward:.1f}")
    print(f"Average reward per step: {total_reward/steps:.3f}")
    
    # Memory efficiency
    if memory_change > 0:
        print(f"\nMemory efficiency: {total_reward/memory_change:.1f} reward units per MB")
    else:
        print(f"\nMemory efficiency: Stable (minimal memory growth)")
    
    print("\nPerformance monitoring demo complete!")

# demo_6_visualization_demo.py
"""
Visualization Demo - Shows how to visualize brain activity and agent behavior.
This demonstrates visualization and debugging capabilities.
"""

import pynlm

def demo_visualization():
    """Demonstrate visualization and debugging capabilities."""
    print("=== Visualization Demo ===")
    
    # Create simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    print("Visualization capabilities available:")
    print("1. Brain state visualization")
    print("2. World/Environment visualization")
    print("3. Agent behavior visualization")
    print("4. Activity pattern analysis")
    print("5. Developmental stage visualization")
    
    # Run a short simulation
    steps = 50
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Print visualization data
        if step % 10 == 0:
            print(f"\nStep {step} - Visualization Data:")
            print(f"  World: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Brain: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getAverageFiringRate():.3f} avg firing rate")
            print(f"  Agent: Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Neuromod={agent.getNeuromodulationLevel():.3f}")
            print(f"  Environment: {world.getAgentBody().energy:.1f}/100 energy, "
                  f"{agent.getDevelopmentalStage()} developmental stage")
            
            # Show action in human-readable form
            action_str = pynlm.actionTypeToString(action)
            print(f"  Action: {action_str} (reward: {result.reward:.1f})")
    
    print(f"\n=== Visualization Summary ===")
    print(f"Simulation completed: {steps} steps")
    print(f"Final brain state: {brain.getFiringNeuronCount()} firing neurons")
    print(f"Final agent state: Energy={world.getAgentBody().energy:.1f}, "
          f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print("\nVisualization demo complete!")

# Main execution function
def run_all_demos():
    """Run all demo scripts."""
    print("=" * 60)
    print("NLM Demo Script Collection")
    print("=" * 60)
    
    demos = [
        ("Basic Brain Demo", demo_basic_brain),
        ("Agent-World Interaction Demo", demo_agent_interaction),
        ("Learning Progress Demo", demo_learning_progress),
        ("Advanced Configuration Demo", demo_advanced_configuration),
        ("Performance Monitor Demo", demo_performance_monitor),
        ("Visualization Demo", demo_visualization)
    ]
    
    for demo_name, demo_func in demos:
        print(f"\n{'=' * 60}")
        print(f"Running: {demo_name}")
        print(f"{'=' * 60}")
        
        try:
            demo_func()
            print(f"\n✓ {demo_name} completed successfully!")
        except Exception as e:
            print(f"\n✗ {demo_name} failed with error: {str(e)}")
    
    print(f"\n{'=' * 60}")
    print("All demos completed!")
    print(f"{'=' * 60}")

if __name__ == "__main__":
    run_all_demos()
