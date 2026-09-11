# Advanced NLM Python Examples

This document provides advanced usage patterns and examples for the NLM Python API, going beyond the basic examples found in `easy_usage.md`.

## Example 1: Advanced Agent with Full Learning Capabilities

This example demonstrates an agent that uses all available learning systems:

```python
import pynlm
import time

def advanced_agent_simulation(steps=5000):
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration options
    config.set("random_seed", 42)
    config.set("neuron_count", 2000)  # Larger brain
    config.set("region_count", 3)     # Multiple regions
    config.set("connection_probability", 0.15f)
    config.set("simulation_timestep", 0.001f)
    config.set("stdp_ltp_weight", 0.02f)
    config.set("stdp_ltd_weight", 0.015f)
    config.set("synaptogenesis_rate", 0.00005f)
    config.set("pruning_rate", 0.000005f)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting advanced simulation with all learning systems...")
    print(f"Initial configuration: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Statistics tracking
    stats = {
        'steps': 0,
        'total_spikes': 0,
        'active_neurons': 0,
        'reward_sum': 0.0,
        'curiosity_sum': 0.0,
        'development_stages': {},
        'spike_history': []
    }
    
    # Run simulation
    for step in range(steps):
        # World update
        world.update(0.1)  # 100ms per step
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes (with learning)
        brain.step(step, step * 0.1)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(action_result.reward, 0.0f)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Track statistics
        stats['steps'] += 1
        stats['total_spikes'] += brain.getTotalSpikeCount()
        stats['active_neurons'] += brain.getActiveNeuronCount()
        stats['reward_sum'] += action_result.reward
        stats['curiosity_sum'] += agent.getCuriosityLevel()
        
        # Track development stage
        stage = str(agent.getDevelopmentalStage())
        stats['development_stages'][stage] = stats['development_stages'].get(stage, 0) + 1
        
        # Record spike rate for analysis
        if step % 100 == 0:
            stats['spike_history'].append(brain.getTotalSpikeCount())
        
        # Print progress
        if step % 1000 == 0:
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()} neurons, "
                  f"Avg rate: {firing_rate:.2f} Hz, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Dev stage: {agent.getDevelopmentalStage()}")
    
    print("\n=== Simulation Results ===")
    print(f"Total simulation steps: {stats['steps']}")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Average firing rate: {stats['total_spikes']/steps:.2f}")
    print(f"Average active neurons: {stats['active_neurons']/stats['steps']:.1f}")
    print(f"Total reward accumulated: {stats['reward_sum']:.2f}")
    print(f"Average curiosity level: {stats['curiosity_sum']/stats['steps']:.3f}")
    print(f"Development stages distribution:")
    for stage, count in stats['development_stages'].items():
        print(f"  {stage}: {count} steps ({count/stats['steps']*100:.1f}%)")
    
    # Save brain state for later analysis
    brain.save("advanced_brain_checkpoint.nlm")
    print(f"\nBrain state saved to 'advanced_brain_checkpoint.nlm'")
    
    return stats

if __name__ == "__main__":
    # Run the advanced simulation
    results = advanced_agent_simulation(steps=5000)
```

## Example 2: Multi-Agent Coordination

This example demonstrates multiple agents working in the same world:

```python
import pynlm
import threading
import time

class MultiAgentEnvironment:
    def __init__(self, num_agents=3, world_size=30):
        self.num_agents = num_agents
        self.world_size = world_size
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=world_size, height=world_size, 
                           visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agents = []
        self.brains = []
        
        for i in range(num_agents):
            # Each agent gets a different brain configuration
            config = pynlm.createDefaultConfig()
            config.set("random_seed", 42 + i)
            config.set("neuron_count", 500 + i * 200)
            config.set("region_count", 1 + i)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Enable different learning combinations for each agent
            if i % 3 == 0:
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
            elif i % 3 == 1:
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
            else:
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
                agent.enableCuriosity(True)
            
            self.agents.append(agent)
            self.brains.append(brain)
    
    def run_step(self, step):
        """Run one simulation step for all agents"""
        # Update world (only once per step)
        self.world.update(0.1)
        
        # Get sensory input for all agents
        percept = self.world.getSensoryPercept()
        
        # Process sensory input and get actions for all agents
        actions = []
        for agent in self.agents:
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            actions.append(action)
        
        # Apply all actions to world
        for i, action in enumerate(actions):
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation for agents that use it
            if self.agents[i].isRewardModulationEnabled():
                action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                self.agents[i].applyRewardModulation(action_result.reward, 0.0f)
            
            # Update development for agents that use it
            if self.agents[i].isDevelopmentEnabled():
                self.agents[i].updateDevelopment(0.1)
        
        # Brain updates for all agents
        for i, brain in enumerate(self.brains):
            brain.step(step)
        
        return actions
    
    def run_simulation(self, steps=2000):
        """Run complete simulation"""
        print(f"Starting multi-agent simulation with {self.num_agents} agents...")
        print(f"World size: {self.world_size}x{self.world_size}")
        
        # Print initial statistics
        total_neurons = sum(brain.getTotalNeuronCount() for brain in self.brains)
        total_synapses = sum(brain.getTotalSynapseCount() for brain in self.brains)
        print(f"Total neurons across all agents: {total_neurons}")
        print(f"Total synapses across all agents: {total_synapses}")
        
        # Simulation statistics
        step_stats = []
        
        for step in range(steps):
            actions = self.run_step(step)
            
            # Record statistics every 100 steps
            if step % 100 == 0:
                total_spikes = sum(brain.getTotalSpikeCount() for brain in self.brains)
                total_firing = sum(brain.getFiringNeuronCount() for brain in self.brains)
                
                stats = {
                    'step': step,
                    'total_spikes': total_spikes,
                    'total_firing': total_firing,
                    'actions': actions.copy()
                }
                step_stats.append(stats)
                
                print(f"Step {step}: "
                      f"Spikes: {total_spikes}, "
                      f"Firing: {total_firing} neurons, "
                      f"Actions: {set(actions)}")
        
        print(f"\n=== Multi-Agent Simulation Complete ===")
        print(f"Total steps: {steps}")
        print(f"Average spikes per step: {sum(s['total_spikes'] for s in step_stats)/len(step_stats):.1f}")
        print(f"Average firing per step: {sum(s['total_firing'] for s in step_stats)/len(step_stats):.1f}")
        print(f"Most common actions: {max([(actions.count(a), a) for a in set(sum([s['actions'] for s in step_stats], []))])[1]}")
        
        return step_stats

if __name__ == "__main__":
    # Create and run multi-agent environment
    env = MultiAgentEnvironment(num_agents=3, world_size=30)
    results = env.run_simulation(steps=2000)
```

## Example 3: Brain State Management and Transfer

This example demonstrates saving and loading brain states, and transferring between different configurations:

```python
import pynlm
import os
import json

def brain_state_transfer_demo():
    print("=== Brain State Transfer Demo ===")
    
    # Create initial brain
    config1 = pynlm.createDefaultConfig()
    config1.set("random_seed", 123)
    config1.set("neuron_count", 1000)
    config1.set("region_count", 2)
    
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    # Run some simulation to create interesting state
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent1 = pynlm.createAgentBrain(brain1)
    agent1.initialize(world)
    agent1.enableRewardModulation(True)
    agent1.enableCuriosity(True)
    
    # Simulate for a while
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent1.processSensoryInput(percept)
        brain1.step(step)
        action = agent1.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent1.applyRewardModulation(0.1, 0.0)  # Small positive reward
    
    print(f"Initial brain created: {brain1.getTotalNeuronCount()} neurons, {brain1.getTotalSynapseCount()} synapses")
    print(f"Initial firing rate: {brain1.getAverageFiringRate():.2f} Hz")
    
    # Save brain state
    brain1.save("brain1_checkpoint.nlm")
    print("Brain state saved to 'brain1_checkpoint.nlm'")
    
    # Get configuration from saved brain
    saved_config = brain1.getConfig()
    saved_config_str = saved_config.summary()
    print(f"\nConfiguration from brain: {saved_config_str[:200]}...")
    
    # Create new brain with similar configuration
    config2 = pynlm.createDefaultConfig()
    config2.set("random_seed", 456)  # Different seed
    config2.set("neuron_count", 1500)  # Different size
    config2.set("region_count", 3)     # Different regions
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    print(f"\nNew brain created: {brain2.getTotalNeuronCount()} neurons, {brain2.getTotalSynapseCount()} synapses")
    
    # Create agent for new brain
    agent2 = pynlm.createAgentBrain(brain2)
    agent2.initialize(world)
    agent2.enableRewardModulation(True)
    agent2.enableCuriosity(True)
    
    # Load brain1's state into brain2
    brain2.load("brain1_checkpoint.nlm")
    print("Loaded brain1 state into brain2")
    
    # Verify state transfer
    print(f"\nBrain state verification:")
    print(f"Brain2 neuron count after load: {brain2.getTotalNeuronCount()} (original: {brain2.getTotalNeuronCount()})")
    print(f"Brain2 synapse count after load: {brain2.getTotalSynapseCount()} (original: {brain2.getTotalSynapseCount()})")
    
    # Run both brains to compare
    print("\n=== Running Comparison Simulation ===")
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process with agent1
        agent1.processSensoryInput(percept)
        brain1.step(step)
        action1 = agent1.decodeMotorCommand()
        
        # Process with agent2 (from loaded state)
        agent2.processSensoryInput(percept)
        brain2.step(step)
        action2 = agent2.decodeMotorCommand()
        
        # Apply actions
        world.applyMotorCommand(action1, world.getSimulationTime())
        world.applyMotorCommand(action2, world.getSimulationTime())
        
        # Apply rewards
        agent1.applyRewardModulation(0.05, 0.0)
        agent2.applyRewardModulation(0.05, 0.0)
    
    print(f"\nBrain1 final statistics:")
    print(f"  Total spikes: {brain1.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain1.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain1.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain1.getDevelopmentalStage()}")
    
    print(f"\nBrain2 final statistics:")
    print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain2.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain2.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain2.getDevelopmentalStage()}")
    
    # Clean up
    if os.path.exists("brain1_checkpoint.nlm"):
        os.remove("brain1_checkpoint.nlm")
        print(f"\nCleaned up temporary file 'brain1_checkpoint.nlm'")
    
    return brain1, brain2

if __name__ == "__main__":
    brain1, brain2 = brain_state_transfer_demo()
```

## Example 4: Real-time Brain Monitoring and Debugging

This example demonstrates real-time monitoring of brain activity:

```python
import pynlm
import time
import threading

class BrainMonitor:
    def __init__(self, brain, world, agent, update_interval=1.0):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.update_interval = update_interval
        self.running = False
        self.monitor_thread = None
        
        # Monitor data
        self.history = {
            'step': [],
            'firing_rate': [],
            'total_spikes': [],
            'reward': [],
            'curiosity': [],
            'development': [],
            'active_neurons': [],
            'e_i_ratio': [],
            'time': []
        }
    
    def start_monitoring(self):
        """Start background monitoring thread"""
        self.running = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
        print("Brain monitoring started (will run in background)")
    
    def stop_monitoring(self):
        """Stop background monitoring"""
        self.running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=2.0)
        print("Brain monitoring stopped")
    
    def _monitor_loop(self):
        """Background monitoring loop"""
        step = 0
        last_time = time.time()
        
        while self.running:
            try:
                current_time = time.time()
                elapsed = current_time - last_time
                
                if elapsed >= self.update_interval:
                    # Record current state
                    self.history['step'].append(step)
                    self.history['firing_rate'].append(self.brain.getFiringNeuronCount())
                    self.history['total_spikes'].append(self.brain.getTotalSpikeCount())
                    self.history['active_neurons'].append(self.brain.getActiveNeuronCount())
                    self.history['e_i_ratio'].append(self.brain.getExcitationInhibitionRatio())
                    self.history['time'].append(current_time - self.history['time'][0] if self.history['time'] else 0.0)
                    
                    # Get agent state if available
                    if self.agent:
                        self.history['reward'].append(self.agent.getNeuromodulationLevel())
                        self.history['curiosity'].append(self.agent.getCuriosityLevel())
                        self.history['development'].append(self.agent.getDevelopmentalStage())
                    
                    step += 1
                    last_time = current_time
                
                time.sleep(0.1)  # Check every 100ms
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(1.0)
    
    def get_statistics(self):
        """Get summary statistics from monitoring data"""
        if not self.history['step']:
            return None
        
        stats = {
            'steps_recorded': len(self.history['step']),
            'time_span': self.history['time'][-1] if self.history['time'] else 0.0,
            'avg_firing_rate': sum(self.history['firing_rate']) / len(self.history['firing_rate']),
            'max_firing_rate': max(self.history['firing_rate']),
            'avg_total_spikes': sum(self.history['total_spikes']) / len(self.history['total_spikes']),
            'max_total_spikes': max(self.history['total_spikes']),
            'avg_active_neurons': sum(self.history['active_neurons']) / len(self.history['active_neurons']),
            'avg_e_i_ratio': sum(self.history['e_i_ratio']) / len(self.history['e_i_ratio']),
            'avg_reward': sum(self.history['reward']) / len(self.history['reward']) if self.history['reward'] else 0.0,
            'avg_curiosity': sum(self.history['curiosity']) / len(self.history['curiosity']) if self.history['curiosity'] else 0.0,
            'development_distribution': {}
        }
        
        # Count development stages
        if self.history['development']:
            for stage in self.history['development']:
                stage_str = str(stage)
                stats['development_distribution'][stage_str] = stats['development_distribution'].get(stage_str, 0) + 1
        
        return stats
    
    def print_live_stats(self, max_points=20):
        """Print live statistics"""
        if not self.history['step']:
            print("No monitoring data available")
            return
        
        # Show last N points
        start_idx = max(0, len(self.history['step']) - max_points)
        
        print("\n=== Live Brain Statistics ===")
        print(f"Step range: {self.history['step'][start_idx]} to {self.history['step'][-1]}")
        print(f"Time span: {self.history['time'][-1]:.1f} seconds")
        print(f"Current firing rate: {self.history['firing_rate'][-1]:.1f}")
        print(f"Total spikes: {self.history['total_spikes'][-1]}")
        print(f"Average firing rate (last {max_points} points): {stats['avg_firing_rate']:.1f}")
        print(f"Average E/I ratio: {stats['avg_e_i_ratio']:.2f}")
        print(f"Average reward: {stats['avg_reward']:.3f}")
        print(f"Average curiosity: {stats['avg_curiosity']:.3f}")
        
        if stats['development_distribution']:
            print("Development stages:")
            for stage, count in stats['development_distribution'].items():
                print(f"  {stage}: {count} samples ({count/sum(stats['development_distribution'].values())*100:.1f}%)")

def real_time_monitoring_demo():
    print("=== Real-time Brain Monitoring Demo ===")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 789)
    config.set("neuron_count", 800)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Monitoring started with {brain.getTotalNeuronCount()} neurons")
    print("Monitoring will record:")
    print("  - Firing rate (neurons per step)")
    print("  - Total spike count")
    print("  - Active neuron count")
    print("  - Excitation/Inhibition ratio")
    print("  - Neuromodulation level (reward)")
    print("  - Curiosity level")
    print("  - Developmental stage")
    print("\nPress Ctrl+C to stop monitoring\n")
    
    # Create and start monitor
    monitor = BrainMonitor(brain, world, agent, update_interval=0.5)
    
    try:
        # Start monitoring
        monitor.start_monitoring()
        
        # Run simulation while monitoring
        for step in range(1000):
            # Update world
            world.update(0.1)
            
            # Process input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            agent.applyRewardModulation(action_result.reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Print stats every 50 steps
            if step % 50 == 0:
                firing_rate = brain.getFiringNeuronCount()
                curiosity = agent.getCuriosityLevel()
                dev_stage = agent.getDevelopmentalStage()
                print(f"Step {step:4d} | Firing: {firing_rate:4d} | "
                      f"Curiosity: {curiosity:.3f} | "
                      f"Dev: {dev_stage}")
                
                # Print live stats every 200 steps
                if step % 200 == 0:
                    monitor.print_live_stats(max_points=10)
                    
    except KeyboardInterrupt:
        print("\n\nMonitoring interrupted by user")
    
    finally:
        # Stop monitoring
        monitor.stop_monitoring()
        
        # Get and print final statistics
        stats = monitor.get_statistics()
        if stats:
            print(f"\n=== Final Monitoring Statistics ===")
            print(f"Total simulation time: {stats['time_span']:.1f} seconds")
            print(f"Average firing rate: {stats['avg_firing_rate']:.1f}")
            print(f"Maximum firing rate: {stats['max_firing_rate']}")
            print(f"Average total spikes: {stats['avg_total_spikes']:.1f}")
            print(f"Average active neurons: {stats['avg_active_neurons']:.1f}")
            print(f"Average E/I ratio: {stats['avg_e_i_ratio']:.2f}")
            print(f"Average reward level: {stats['avg_reward']:.3f}")
            print(f"Average curiosity level: {stats['avg_curiosity']:.3f}")
            
            if stats['development_distribution']:
                print("Development stage distribution:")
                for stage, count in stats['development_distribution'].items():
                    percentage = count / sum(stats['development_distribution'].values()) * 100
                    print(f"  {stage}: {count} ({percentage:.1f}%)")

if __name__ == "__main__":
    real_time_monitoring_demo()
```

## Example 5: Configuration-Driven Brain Experiments

This example demonstrates systematic experimentation with different brain configurations:

```python
import pynlm
import numpy as np
import json
import os

class BrainExperiment:
    def __init__(self, base_config=None):
        self.base_config = base_config or pynlm.createDefaultConfig()
        self.results = {}
    
    def create_brain_with_config(self, config_overrides):
        """Create a brain with specific configuration overrides"""
        config = pynlm.createDefaultConfig()
        
        # Apply base configuration
        for key, value in self.base_config.getKeys():
            # Get value and apply to new config
            original_value = self.base_config.get(config_overrides.get(key, key))
            if original_value:
                config.set(key, original_value)
        
        # Apply experimental overrides
        for key, value in config_overrides.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain, config
    
    def run_experiment(self, param_name, param_values, fixed_params=None):
        """Run experiment with varying parameter"""
        print(f"Running experiment: {param_name} with values {param_values}")
        
        if fixed_params is None:
            fixed_params = {}
        
        # Prepare parameter combinations
        param_combinations = []
        for value in param_values:
            combination = fixed_params.copy()
            combination[param_name] = value
            param_combinations.append(combination)
        
        # Run experiments
        experiment_results = []
        for i, params in enumerate(param_combinations):
            print(f"  Experiment {i+1}/{len(param_combinations)}: {params}")
            
            # Create brain with these parameters
            brain, config = self.create_brain_with_config(params)
            
            # Create agent and run simulation
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            steps = 500
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(0.05, 0.0)
                agent.updateDevelopment(0.1)
            
            # Record results
            result = {
                'parameters': params.copy(),
                'config': config.summary(),
                'final_neuron_count': brain.getTotalNeuronCount(),
                'final_synapse_count': brain.getTotalSynapseCount(),
                'final_spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'final_development_stage': brain.getDevelopmentalStage(),
                'final_curiosity_level': agent.getCuriosityLevel(),
                'final_reward_level': agent.getNeuromodulationLevel()
            }
            
            experiment_results.append(result)
            
            # Save brain state
            brain.save(f"exp_{param_name}_{i}.nlm")
        
        # Store results
        self.results[param_name] = experiment_results
        
        return experiment_results
    
    def analyze_results(self, param_name):
        """Analyze experiment results and generate statistics"""
        if param_name not in self.results:
            print(f"No results found for parameter: {param_name}")
            return
        
        results = self.results[param_name]
        
        print(f"\n=== Analysis of {param_name} Experiment ===")
        print(f"Number of trials: {len(results)}")
        
        # Extract parameter values and results
        param_values = []
        spike_counts = []
        firing_rates = []
        e_i_ratios = []
        
        for result in results:
            param_values.append(result['parameters'][param_name])
            spike_counts.append(result['final_spike_count'])
            firing_rates.append(result['average_firing_rate'])
            e_i_ratios.append(result['excitation_inhibition_ratio'])
        
        # Calculate statistics
        print(f"\nParameter {param_name} values: {param_values}")
        print(f"Spike count statistics:")
        print(f"  Mean: {np.mean(spike_counts):.1f}")
        print(f"  Std: {np.std(spike_counts):.1f}")
        print(f"  Min: {np.min(spike_counts)}")
        print(f"  Max: {np.max(spike_counts)}")
        
        print(f"\nFiring rate statistics:")
        print(f"  Mean: {np.mean(firing_rates):.2f}")
        print(f"  Std: {np.std(firing_rates):.2f}")
        
        print(f"\nE/I ratio statistics:")
        print(f"  Mean: {np.mean(e_i_ratios):.2f}")
        print(f"  Std: {np.std(e_i_ratios):.2f}")
        
        # Generate correlation plots
        print(f"\n=== Correlation Analysis ===")
        if len(param_values) > 1:
            spike_corr = np.corrcoef(param_values, spike_counts)[0, 1]
            firing_corr = np.corrcoef(param_values, firing_rates)[0, 1]
            ei_corr = np.corrcoef(param_values, e_i_ratios)[0, 1]
            
            print(f"Correlation with spike count: {spike_corr:.3f}")
            print(f"Correlation with firing rate: {firing_corr:.3f}")
            print(f"Correlation with E/I ratio: {ei_corr:.3f}")
            
            # Determine best parameter value
            best_idx = np.argmax(spike_counts)
            print(f"\nBest parameter value for spike production: {param_values[best_idx]}")
            print(f"Resulting spike count: {spike_counts[best_idx]}")
    
    def save_results(self, filename="experiment_results.json"):
        """Save experiment results to JSON file"""
        # Convert results to JSON-serializable format
        json_results = {}
        
        for param_name, results in self.results.items():
            json_results[param_name] = []
            for result in results:
                json_result = {
                    'parameters': result['parameters'],
                    'final_neuron_count': result['final_neuron_count'],
                    'final_synapse_count': result['final_synapse_count'],
                    'final_spike_count': result['final_spike_count'],
                    'average_firing_rate': result['average_firing_rate'],
                    'excitation_inhibition_ratio': result['excitation_inhibition_ratio'],
                    'final_development_stage': str(result['final_development_stage']),
                    'final_curiosity_level': result['final_curiosity_level'],
                    'final_reward_level': result['final_reward_level']
                }
                json_results[param_name].append(json_result)
        
        with open(filename, 'w') as f:
            json.dump(json_results, f, indent=2)
        
        print(f"\nResults saved to {filename}")

def systematic_experiment_demo():
    print("=== Systematic Brain Configuration Experiment ===")
    
    # Create base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.set("random_seed", 1000)
    base_config.set("region_count", 2)
    
    # Create experiment object
    experiment = BrainExperiment(base_config)
    
    # Experiment 1: Vary neuron count
    print("\n--- Experiment 1: Varying Neuron Count ---")
    neuron_counts = [500, 1000, 2000, 5000]
    results1 = experiment.run_experiment(
        param_name="neuron_count",
        param_values=neuron_counts,
        fixed_params={"random_seed": 1000, "region_count": 2}
    )
    
    # Experiment 2: Vary connection probability
    print("\n--- Experiment 2: Varying Connection Probability ---")
    connection_probs = [0.05, 0.1, 0.2, 0.3]
    results2 = experiment.run_experiment(
        param_name="connection_probability",
        param_values=connection_probs,
        fixed_params={"random_seed": 2000, "neuron_count": 1000, "region_count": 2}
    )
    
    # Experiment 3: Vary STDP weights
    print("\n--- Experiment 3: Varying STDP Weights ---")
    # Create custom STDP configuration for each experiment
    stdp_params = [
        {"stdp_ltp_weight": 0.01, "stdp_ltd_weight": 0.012, "stdp_tau": 10},
        {"stdp_ltp_weight": 0.02, "stdp_ltd_weight": 0.015, "stdp_tau": 20},
        {"stdp_ltp_weight": 0.03, "stdp_ltd_weight": 0.018, "stdp_tau": 30},
        {"stdp_ltp_weight": 0.04, "stdp_ltd_weight": 0.021, "stdp_tau": 40}
    ]
    
    # For STDP weights, we need to create separate experiments
    for i, params in enumerate(stdp_params):
        print(f"\n  Sub-experiment {i+1}/{len(stdp_params)}")
        results_sub = experiment.run_experiment(
            param_name="stdp_ltp_weight",
            param_values=[params["stdp_ltp_weight"]],
            fixed_params={
                "random_seed": 3000 + i,
                "neuron_count": 1000,
                "region_count": 2,
                "stdp_ltd_weight": params["stdp_ltd_weight"],
                "stdp_tau": params["stdp_tau"]
            }
        )
    
    # Analyze results
    print("\n=== Experiment Analysis ===")
    experiment.analyze_results("neuron_count")
    experiment.analyze_results("connection_probability")
    
    # Save results
    experiment.save_results("brain_experiment_results.json")
    
    # Clean up temporary files
    for i in range(len(neuron_counts)):
        if os.path.exists(f"exp_neuron_count_{i}.nlm"):
            os.remove(f"exp_neuron_count_{i}.nlm")
    
    for i in range(len(connection_probs)):
        if os.path.exists(f"exp_connection_probability_{i}.nlm"):
            os.remove(f"exp_connection_probability_{i}.nlm")
    
    for i in range(len(stdp_params)):
        if os.path.exists(f"exp_stdp_ltp_weight_{i}.nlm"):
            os.remove(f"exp_stdp_ltp_weight_{i}.nlm")
    
    print("\n=== Experiment Complete ===")
    print("Results have been saved to 'brain_experiment_results.json'")

if __name__ == "__main__":
    systematic_experiment_demo()
```

## Usage Summary

These advanced examples demonstrate:

1. **Complete Learning Systems**: Using reward modulation, curiosity, development, and structural plasticity together
2. **Multi-Agent Coordination**: Multiple agents learning in shared environments
3. **Brain State Management**: Saving, loading, and transferring brain states between sessions
4. **Real-time Monitoring**: Continuous monitoring and analysis of brain activity
5. **Systematic Experimentation**: Running controlled experiments with different configurations

These examples showcase the flexibility and power of the NLM Python API for advanced neural simulation research and experimentation.

For more examples or specific use cases, refer to the NLM documentation or the official repository examples.
```
