# NLM - Advanced Usage Guide

## Master Level: Advanced NLM Features

Welcome to the advanced usage guide! You now have a solid foundation in NLM and are ready to explore its full power. This guide covers expert-level features, advanced neural architecture design, performance optimization, and custom implementations.

## 1. Advanced Neural Architecture Design

### Custom Neural Types and Connectivity

```python
import pynlm
import numpy as np

class AdvancedNeuralArchitecture:
    def __init__(self, config):
        self.config = config
        self.create_special_neuron_types()
        self.build_custom_connectivity()
        self.setup_hierarchical_structure()
    
    def create_special_neuron_types(self):
        """Create custom neuron types with specialized functions"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create specialized neuron populations
        self.create_attention_neurons()
        self.create_memory_neurons()
        self.create_plasticity_neurons()
        self.create_modulation_neurons()
    
    def create_attention_neurons(self):
        """Create attention mechanism neurons"""
        # Create high-density attention region
        attention_region_id = self.brain.addRegion("attention")
        
        # Add many neurons with specific properties
        for i in range(200):
            # We can't directly add neurons, but we can configure regions
            pass
        
        # Configure attention region parameters
        # (This would require accessing internal brain state)
    
    def create_memory_neurons(self):
        """Create memory system neurons"""
        # Working memory neurons with persistent activity
        working_memory_id = self.brain.addRegion("working_memory")
        
        # Episodic memory neurons with associative properties
        episodic_memory_id = self.brain.addRegion("episodic_memory")
        
        # Semantic memory neurons with pattern completion
        semantic_memory_id = self.brain.addRegion("semantic_memory")
    
    def create_plasticity_neurons(self):
        """Create plasticity regulation neurons"""
        # STDP regulation neurons
        stdp_reg_id = self.brain.addRegion("stdp_regulation")
        
        # Hebbian regulation neurons
        hebbian_reg_id = self.brain.addRegion("hebbian_regulation")
        
        # Structural plasticity neurons
        structural_reg_id = self.brain.addRegion("structural_regulation")
    
    def create_modulation_neurons(self):
        """Create neuromodulation neurons"""
        # Dopamine neurons
        dopamine_id = self.brain.addRegion("dopamine")
        
        # Serotonin neurons
        serotonin_id = self.brain.addRegion("serotonin")
        
        # Noradrenaline neurons
        noradrenaline_id = self.brain.addRegion("noradrenaline")
    
    def build_custom_connectivity(self):
        """Build sophisticated connectivity patterns"""
        # We can't directly manipulate synaptic connections through Python API
        # This would require accessing internal brain state or C++ extensions
        
        # Instead, we can:
        # 1. Configure regional connectivity through brain parameters
        # 2. Use neuromodulation to influence connection strengths
        # 3. Leverage structural plasticity over time
        
        print("Custom connectivity requires access to internal brain state")
        print("Consider extending pybind11 bindings for direct manipulation")
    
    def setup_hierarchical_structure(self):
        """Set up hierarchical neural organization"""
        regions = [
            ("sensorimotor", "Basic action selection"),
            ("perceptual", "Sensory processing"),
            ("cognitive", "Higher cognition"),
            ("meta_cognitive", "Thinking about thinking")
        ]
        
        for region_name, description in regions:
            region_id = self.brain.addRegion(region_name)
            print(f"Created {region_name} region ({description})")
    
    def advanced_simulation_loop(self, custom_params):
        """Run advanced simulation with custom control"""
        print("Starting advanced simulation...")
        
        # Phase 1: Sensory integration and perception
        for step in range(custom_params['perception_phase']):
            self.run_perception_phase(step)
        
        # Phase 2: Action selection and planning
        for step in range(custom_params['planning_phase']):
            self.run_planning_phase(step)
        
        # Phase 3: Learning and development
        for step in range(custom_params['learning_phase']):
            self.run_learning_phase(step)
        
        # Phase 4: Consolidation and integration
        for step in range(custom_params['integration_phase']):
            self.run_integration_phase(step)
    
    def run_perception_phase(self, step):
        """Advanced perception processing"""
        # Enhanced sensory processing with attention
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Apply attention mechanisms
        attended_percept = self.apply_attention(percept)
        agent.processSensoryInput(attended_percept)
        
        # Advanced feature extraction
        features = self.extract_advanced_features(percept)
        brain.receiveSensoryInput(features)
    
    def run_planning_phase(self, step):
        """Advanced action planning"""
        # Multi-step planning
        brain.step(step)
        
        # Look ahead simulation
        predicted_actions = self.look_ahead_prediction(step)
        selected_action = self.select_optimal_action(predicted_actions)
        
        # Apply motor command with fine control
        world.applyMotorCommand(selected_action, world.getSimulationTime())
    
    def run_learning_phase(self, step):
        """Advanced learning mechanisms"""
        # Error-based learning
        prediction_error = self.calculate_prediction_error(step)
        agent.applyRewardModulation(0.0, prediction_error)
        
        # Curiosity-driven exploration
        novelty = self.detect_novelty(step)
        self.adapt_based_on_novelty(novelty)
        
        # Metaplastic changes
        self.metaplastic_adaptation(step)
        
        # Development progression
        agent.updateDevelopment(0.1)
    
    def run_integration_phase(self, step):
        """Consolidation and memory integration"""
        # Working memory consolidation
        self.consolidate_working_memory()
        
        # Episodic memory formation
        self.form_episodic_memory(step)
        
        # Semantic memory updating
        self.update_semantic_memory(step)
        
        # Structural reorganization
        self.reorganize_structure()
```

### Custom Learning Rules Implementation

```python
class CustomLearningRule:
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        self.eligibility_traces = {}
        self.metaplastic_parameters = {}
    
    def implement_custom_stdp(self, neuron_a, neuron_b, spike_time_diff):
        """Custom STDP rule with additional factors"""
        # Standard STDP
        if spike_time_diff > 0:
            # Post before pre
            weight_change = self.positive_stdp_kernel(spike_time_diff)
        else:
            # Pre before post
            weight_change = self.negative_stdp_kernel(-spike_time_diff)
        
        # Add custom modulation factors
        neuromodulation_factor = self.get_neuromodulation_factor()
        developmental_factor = self.get_developmental_factor()
        
        total_weight_change = weight_change * neuromodulation_factor * developmental_factor
        
        # Apply weight change
        self.apply_weight_change(neuron_a, neuron_b, total_weight_change)
    
    def positive_stdp_kernel(self, dt):
        """Custom positive STDP kernel"""
        return 0.01 * np.exp(-dt / 20.0)  # Custom time constant
    
    def negative_stdp_kernel(self, dt):
        """Custom negative STDP kernel"""
        return -0.012 * np.exp(-dt / 25.0)  # Different magnitude
    
    def get_neuromodulation_factor(self):
        """Get neuromodulation influence on plasticity"""
        # Get current neuromodulation level
        neuromod_level = self.agent.getNeuromodulationLevel()
        
        # Scale neuromodulation effect
        if neuromod_level > 0.5:
            return 1.5  # Enhanced plasticity
        elif neuromod_level < 0.2:
            return 0.5  # Reduced plasticity
        else:
            return 1.0  # Normal plasticity
    
    def get_developmental_factor(self):
        """Get developmental influence on plasticity"""
        stage = self.brain.getDevelopmentalStage()
        
        # Critical period enhancement
        if stage == pynlm.DevelopmentalStage.CriticalPeriod:
            return 2.0  # Enhanced plasticity during critical period
        
        # Adult stability
        elif stage == pynlm.DevelopmentalStage.Adult:
            return 0.8  # Slightly reduced plasticity
        
        # Other stages
        else:
            return 1.0
    
    def metaplastic_adaptation(self, step):
        """Metaplastic changes for stability"""
        # Track recent reward history
        recent_rewards = self.get_recent_rewards(step - 100, step)
        
        if len(recent_rewards) > 0:
            avg_reward = np.mean(recent_rewards)
            
            # Adjust learning rates based on reward history
            if avg_reward > 0.5:  # Consistently positive
                # Reduce plasticity to prevent runaway excitation
                self.metaplastic_parameters['learning_rate'] *= 0.95
            elif avg_reward < -0.5:  # Consistently negative
                # Reduce plasticity to prevent runaway inhibition
                self.metaplastic_parameters['learning_rate'] *= 0.95
            else:
                # Maintain moderate plasticity
                self.metaplastic_parameters['learning_rate'] *= 1.02
    
    def get_recent_rewards(self, start_step, end_step):
        """Get rewards from recent simulation steps"""
        # This would require tracking reward history
        # Implementation depends on how rewards are stored
        return []
    
    def apply_weight_change(self, neuron_a, neuron_b, change):
        """Apply weight change to synaptic connection"""
        # This requires direct access to synaptic connections
        # May require extending pybind11 bindings
        print(f"Applying weight change: {change:.6f} to connection {neuron_a} -> {neuron_b}")
```

## 2. Performance Optimization and Parallel Processing

### Advanced Performance Tuning

```python
import time
import threading
import multiprocessing
import numpy as np
import pynlm

class AdvancedPerformanceOptimizer:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.performance_metrics = {}
        self.optimization_history = []
    
    def parallel_step_execution(self, num_steps, num_workers=None):
        """Execute simulation steps in parallel"""
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        
        print(f"Running {num_steps} steps with {num_workers} workers...")
        
        # Divide work among workers
        steps_per_worker = num_steps // num_workers
        worker_ranges = []
        
        for i in range(num_workers):
            start = i * steps_per_worker
            end = (i + 1) * steps_per_worker if i < num_workers - 1 else num_steps
            worker_ranges.append((start, end))
        
        # Create worker processes
        workers = []
        for start, end in worker_ranges:
            worker = multiprocessing.Process(
                target=self.worker_simulation_step,
                args=(start, end)
            )
            workers.append(worker)
        
        # Start all workers
        start_time = time.time()
        for worker in workers:
            worker.start()
        
        # Wait for all workers to complete
        for worker in workers:
            worker.join()
        
        end_time = time.time()
        total_time = end_time - start_time
        
        print(f"Parallel execution completed in {total_time:.2f} seconds")
        print(f"Speedup: {num_steps / total_time:.1f}x")
        
        return {
            'total_time': total_time,
            'steps_per_second': num_steps / total_time,
            'num_workers': num_workers
        }
    
    def worker_simulation_step(self, start_step, end_step):
        """Worker function for parallel simulation"""
        # Each worker needs its own brain/world/agent instances
        # This is complex due to shared state requirements
        # This is a simplified version
        
        worker_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        worker_brain.initialize()
        
        worker_world = pynlm.createSimpleWorld()
        worker_world.configure(
            width=self.world.getWidth(),
            height=self.world.getHeight(),
            visionWidth=self.world.getVisionWidth(),
            visionHeight=self.world.getVisionHeight()
        )
        worker_world.reset()
        
        worker_agent = pynlm.createAgentBrain(worker_brain)
        worker_agent.initialize(worker_world)
        
        # Run assigned steps
        for step in range(start_step, end_step):
            worker_world.update(0.1)
            percept = worker_world.getSensoryPercept()
            worker_agent.processSensoryInput(percept)
            worker_brain.step(step)
            action = worker_agent.decodeMotorCommand()
            worker_world.applyMotorCommand(action, worker_world.getSimulationTime())
    
    def optimized_brain_parameters(self):
        """Automatically optimize brain parameters for performance"""
        print("Optimizing brain parameters for performance...")
        
        # Test different neuron counts
        neuron_counts = [500, 1000, 2000, 5000, 10000]
        performance_results = []
        
        for neuron_count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            # Create brain and measure performance
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure warm-up time
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            warm_up_time = time.time() - start_time
            
            # Measure steady-state performance
            start_time = time.time()
            for step in range(1000):
                brain.step(step)
            steady_time = time.time() - start_time
            
            performance_results.append({
                'neuron_count': neuron_count,
                'warm_up_time': warm_up_time,
                'steady_time': steady_time,
                'steps_per_second': 1000 / steady_time
            })
        
        # Find optimal configuration
        best_result = max(performance_results, key=lambda x: x['steps_per_second'])
        
        print(f"\nPerformance optimization results:")
        for result in performance_results:
            print(f"  Neurons: {result['neuron_count']:5d} | "
                  f"Warm-up: {result['warm_up_time']:.3f}s | "
                  f"Steady: {result['steps_per_second']:8.1f} steps/s")
        
        print(f"\nOptimal configuration:")
        print(f"  Neuron count: {best_result['neuron_count']}")
        print(f"  Performance: {best_result['steps_per_second']:.1f} steps/second")
        
        return best_result
    
    def memory_efficiency_optimization(self):
        """Optimize memory usage"""
        print("Optimizing memory efficiency...")
        
        # Monitor memory usage during simulation
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        memory_samples = []
        
        # Run simulation and collect memory samples
        for step in range(100):
            self.brain.step(step)
            
            if step % 10 == 0:
                memory_info = process.memory_info()
                memory_samples.append({
                    'step': step,
                    'rss': memory_info.rss / 1024 / 1024,  # MB
                    'vms': memory_info.vms / 1024 / 1024,  # MB
                })
        
        # Analyze memory usage patterns
        avg_memory = np.mean([m['rss'] for m in memory_samples])
        memory_growth = memory_samples[-1]['rss'] - memory_samples[0]['rss']
        
        print(f"\nMemory efficiency analysis:")
        print(f"  Average memory usage: {avg_memory:.1f} MB")
        print(f"  Memory growth: {memory_growth:.1f} MB")
        print(f"  Memory per neuron: {avg_memory / self.brain.getTotalNeuronCount():.3f} MB")
        
        return {
            'avg_memory': avg_memory,
            'memory_growth': memory_growth,
            'memory_per_neuron': avg_memory / self.brain.getTotalNeuronCount()
        }
    
    def benchmark_architecture_scenarios(self):
        """Benchmark different architectural scenarios"""
        print("Benchmarking different architectural scenarios...")
        
        scenarios = [
            {
                'name': 'Minimal',
                'config': {'brain': {'neuron_count': 100}, 'world': {'width': 5, 'height': 5}}
            },
            {
                'name': 'Standard',
                'config': {'brain': {'neuron_count': 1000}, 'world': {'width': 10, 'height': 10}}
            },
            {
                'name': 'Complex',
                'config': {'brain': {'neuron_count': 5000}, 'world': {'width': 20, 'height': 20}}
            },
            {
                'name': 'Dense',
                'config': {'brain': {'neuron_count': 10000}, 'world': {'width': 30, 'height': 30}}
            }
        ]
        
        benchmark_results = []
        
        for scenario in scenarios:
            print(f"\nBenchmarking {scenario['name']} scenario...")
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", scenario['config']['brain']['neuron_count'])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**scenario['config']['world'])
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run benchmark
            start_time = time.time()
            num_steps = min(200, 1000 // scenario['config']['brain']['neuron_count'] * 10)
            
            for step in range(num_steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            end_time = time.time()
            
            benchmark_results.append({
                'scenario': scenario['name'],
                'neuron_count': scenario['config']['brain']['neuron_count'],
                'world_size': f"{scenario['config']['world']['width']}x{scenario['config']['world']['height']}",
                'steps': num_steps,
                'time': end_time - start_time,
                'steps_per_second': num_steps / (end_time - start_time)
            })
        
        # Display results
        print(f"\n{'Scenario':<10} {'Neurons':<10} {'World Size':<12} {'Steps':<6} {'Time':<8} {'Steps/s':<10}")
        print("-" * 70)
        
        for result in benchmark_results:
            print(f"{result['scenario']:<10} {result['neuron_count']:<10} {result['world_size']:<12} "
                  f"{result['steps']:<6} {result['time']:<8.2f} {result['steps_per_second']:<10.1f}")
        
        return benchmark_results
```

## 3. Advanced Visualization and Analysis

### Sophisticated Visualization Tools

```python
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import pynlm

class AdvancedVisualizer:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.visualization_style = 'default'
    
    def create_comprehensive_dashboard(self, num_steps=500):
        """Create a comprehensive visualization dashboard"""
        # Create subplot grid
        fig, axes = plt.subplots(3, 4, figsize=(20, 15))
        fig.suptitle('NLM Comprehensive Brain Dashboard', fontsize=16, fontweight='bold')
        
        # Collect data over simulation
        data = self.collect_simulation_data(num_steps)
        
        # Plot 1: Neural activity heatmap
        self.plot_neural_activity_heatmap(axes[0, 0], data)
        
        # Plot 2: Development stages
        self.plot_development_stages(axes[0, 1], data)
        
        # Plot 3: Neuromodulation levels
        self.plot_neuromodulation(axes[0, 2], data)
        
        # Plot 4: Curiosity vs Novelty
        self.plot_curiosity_novelty(axes[0, 3], data)
        
        # Plot 5: Spike raster plot
        self.plot_spike_raster(axes[1, 0], data)
        
        # Plot 6: Learning curve
        self.plot_learning_curve(axes[1, 1], data)
        
        # Plot 7: Action distribution
        self.plot_action_distribution(axes[1, 2], data)
        
        # Plot 8: Environment interaction
        self.plot_environment_interaction(axes[1, 3], data)
        
        # Plot 9: Memory formation
        self.plot_memory_formation(axes[2, 0], data)
        
        # Plot 10: Performance metrics
        self.plot_performance_metrics(axes[2, 1], data)
        
        # Plot 11: Neural connectivity
        self.plot_neural_connectivity(axes[2, 2], data)
        
        # Plot 12: System balance
        self.plot_system_balance(axes[2, 3], data)
        
        plt.tight_layout()
        plt.show()
        
        return fig, axes
    
    def collect_simulation_data(self, num_steps):
        """Collect comprehensive simulation data"""
        data = {
            'steps': list(range(num_steps)),
            'firing_neurons': [],
            'total_spikes': [],
            'avg_firing_rate': [],
            'curiosity': [],
            'novelty': [],
            'neuromodulation': [],
            'dev_stage': [],
            'simulation_time': [],
            'action_types': {},
            'reward_history': [],
            'spike_times': []
        }
        
        for step in range(num_steps):
            # Run one simulation step
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect data
            data['firing_neurons'].append(self.brain.getFiringNeuronCount())
            data['total_spikes'].append(self.brain.getTotalSpikeCount())
            data['avg_firing_rate'].append(self.brain.getAverageFiringRate())
            data['curiosity'].append(self.agent.getCuriosityLevel())
            data['novelty'].append(self.agent.getNoveltyLevel())
            data['neuromodulation'].append(self.agent.getNeuromodulationLevel())
            data['dev_stage'].append(self.brain.getDevelopmentalStage())
            data['simulation_time'].append(self.world.getSimulationTime())
            
            # Track action types
            action_name = self.get_action_name(action)
            data['action_types'][action_name] = data['action_types'].get(action_name, 0) + 1
            
            # Extract spike times (simplified)
            if step % 10 == 0:  # Record every 10th step
                data['spike_times'].append(step)
        
        return data
    
    def plot_neural_activity_heatmap(self, ax, data):
        """Plot neural activity heatmap over time"""
        # Create a heatmap of neural activity
        activity_matrix = np.zeros((len(data['steps']), 5))
        
        for i, step in enumerate(data['steps']):
            firing_rate = data['avg_firing_rate'][i]
            curiosity = data['curiosity'][i]
            novelty = data['novelty'][i]
            neuromod = data['neuromodulation'][i]
            
            activity_matrix[i] = [
                firing_rate / 100,  # Normalize
                curiosity,
                novelty,
                neuromod,
                data['firing_neurons'][i] / 1000  # Neurons relative to max
            ]
        
        # Create heatmap
        sns.heatmap(activity_matrix.T, ax=ax, cmap='hot', cbar=True,
                   xticks=[0, 100, 250, 400, 499],
                   yticklabels=['Firing Rate', 'Curiosity', 'Novelty', 
                              'Neuromod', 'Neuron Count'])
        ax.set_title('Neural Activity Heatmap')
    
    def plot_development_stages(self, ax, data):
        """Plot development stage progression"""
        stage_colors = {
            pynlm.DevelopmentalStage.Initial: 'blue',
            pynlm.DevelopmentalStage.CriticalPeriod: 'green',
            pynlm.DevelopmentalStage.Maturation: 'orange',
            pynlm.DevelopmentalStage.Adult: 'red',
            pynlm.DevelopmentalStage.Aging: 'purple'
        }
        
        stages = data['dev_stage']
        stage_values = []
        
        for stage in stages:
            # Map stage to numeric value for plotting
            if stage == pynlm.DevelopmentalStage.Initial:
                stage_values.append(0)
            elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
                stage_values.append(1)
            elif stage == pynlm.DevelopmentalStage.Maturation:
                stage_values.append(2)
            elif stage == pynlm.DevelopmentalStage.Adult:
                stage_values.append(3)
            else:
                stage_values.append(4)
        
        colors = [stage_colors[stage] for stage in stages]
        
        scatter = ax.scatter(data['steps'], stage_values, c=colors, s=10, alpha=0.7)
        ax.set_yticks([0, 1, 2, 3, 4])
        ax.set_yticklabels(['Initial', 'Critical', 'Mature', 'Adult', 'Aging'])
        ax.set_xlabel('Step')
        ax.set_title('Development Stage Progression')
        ax.grid(True, alpha=0.3)
    
    def plot_neuromodulation(self, ax, data):
        """Plot neuromodulation levels over time"""
        steps = data['steps']
        neuromod = data['neuromodulation']
        
        ax.plot(steps, neuromod, linewidth=2)
        ax.fill_between(steps, 0, neuromod, alpha=0.3)
        ax.set_xlabel('Step')
        ax.set_ylabel('Neuromodulation Level')
        ax.set_title('Neuromodulation Dynamics')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def plot_curiosity_novelty(self, ax, data):
        """Plot curiosity and novelty relationship"""
        curiosity = data['curiosity']
        novelty = data['novelty']
        
        ax.scatter(curiosity, novelty, c=data['steps'], cmap='viridis', s=10, alpha=0.7)
        ax.set_xlabel('Curiosity Level')
        ax.set_ylabel('Novelty Level')
        ax.set_title('Curiosity-Novelty Relationship')
        ax.grid(True, alpha=0.3)
    
    def plot_spike_raster(self, ax, data):
        """Plot spike raster over time"""
        # This would require actual spike timing data
        # For now, create a simplified representation
        num_neurons = 50  # Simplified
        time_window = 500
        
        spike_matrix = np.random.binomial(1, 0.02, (num_neurons, time_window))
        
        # Transpose for seaborn
        spike_matrix_t = spike_matrix.T
        
        # Create spike plot
        for i in range(num_neurons):
            spikes = np.where(spike_matrix_t[i] == 1)[0]
            if len(spikes) > 0:
                ax.plot(spikes, [i] * len(spikes), 'k.', markersize=1)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Neuron Index')
        ax.set_title('Spike Raster Plot')
        ax.set_xlim(0, time_window)
        ax.set_ylim(0, num_neurons)
        ax.grid(True, alpha=0.3)
    
    def plot_learning_curve(self, ax, data):
        """Plot learning progress over time"""
        steps = data['steps']
        firing_rates = data['avg_firing_rate']
        
        # Smooth the curve
        window_size = 20
        smoothed = np.convolve(firing_rates, np.ones(window_size)/window_size, mode='same')
        
        ax.plot(steps, smoothed, linewidth=2, label='Smoothed')
        ax.plot(steps, firing_rates, alpha=0.3, linewidth=0.5, label='Raw')
        
        ax.set_xlabel('Step')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Learning Curve')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_action_distribution(self, ax, data):
        """Plot action distribution over simulation"""
        action_types = data['action_types']
        
        if action_types:
            actions = list(action_types.keys())
            counts = list(action_types.values())
            
            bars = ax.bar(range(len(actions)), counts, alpha=0.7)
            ax.set_xticks(range(len(actions)))
            ax.set_xticklabels(actions)
            ax.set_ylabel('Frequency')
            ax.set_title('Action Distribution')
            ax.grid(True, alpha=0.3)
            
            # Add value labels on bars
            for bar, count in zip(bars, counts):
                ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                       str(count), ha='center', va='bottom')
    
    def plot_environment_interaction(self, ax, data):
        """Plot environment interaction patterns"""
        # Create a simple representation
        steps = data['steps']
        simulation_time = data['simulation_time']
        
        ax.plot(steps, simulation_time, linewidth=2)
        ax.set_xlabel('Step')
        ax.set_ylabel('Simulation Time (s)')
        ax.set_title('Environment Simulation Time')
        ax.grid(True, alpha=0.3)
    
    def plot_memory_formation(self, ax, data):
        """Plot memory formation over time"""
        # Simplified memory formation curve
        steps = data['steps']
        
        # Create a sigmoid-like memory formation curve
        time_normalized = np.array(steps) / max(steps)
        memory_formation = 1 / (1 + np.exp(-10 * (time_normalized - 0.5)))
        
        ax.plot(steps, memory_formation, linewidth=3)
        ax.set_xlabel('Step')
        ax.set_ylabel('Memory Strength')
        ax.set_title('Memory Formation Dynamics')
        ax.grid(True, alpha=0.3)
    
    def plot_performance_metrics(self, ax, data):
        """Plot performance metrics over time"""
        steps = data['steps']
        firing_rates = data['avg_firing_rate']
        curiosity = data['curiosity']
        novelty = data['novelty']
        
        # Calculate performance metrics
        performance = (np.array(firing_rates) / 100) * 0.4 + \
                     (np.array(curiosity)) * 0.3 + \
                     (np.array(novelty)) * 0.3
        
        ax.plot(steps, performance, linewidth=2)
        ax.set_xlabel('Step')
        ax.set_ylabel('Performance Score')
        ax.set_title('System Performance Metrics')
        ax.set_ylim(0, 1.2)
        ax.grid(True, alpha=0.3)
    
    def plot_neural_connectivity(self, ax, data):
        """Plot neural connectivity patterns"""
        # Simplified connectivity visualization
        steps = data['steps']
        connectivity = np.random.rand(len(steps)) * 0.8 + 0.1
        
        ax.plot(steps, connectivity, linewidth=2, alpha=0.7)
        ax.set_xlabel('Step')
        ax.set_ylabel('Connectivity Strength')
        ax.set_title('Neural Connectivity Dynamics')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def plot_system_balance(self, ax, data):
        """Plot system balance indicators"""
        steps = data['steps']
        firing_rates = np.array(data['avg_firing_rate'])
        neuromod = np.array(data['neuromodulation'])
        
        # Calculate E/I balance (simplified)
        e_i_balance = (firing_rates / 100) * (1 - neuromod) + 0.1
        
        ax.plot(steps, e_i_balance, linewidth=2)
        ax.set_xlabel('Step')
        ax.set_ylabel('E/I Balance')
        ax.set_title('System Balance Dynamics')
        ax.set_ylim(0, 2)
        ax.grid(True, alpha=0.3)
    
    def get_action_name(self, action):
        """Convert action to readable name"""
        action_names = {
            pynlm.ActionType.MoveForward: 'Move Forward',
            pynlm.ActionType.MoveBackward: 'Move Backward',
            pynlm.ActionType.TurnLeft: 'Turn Left',
            pynlm.ActionType.TurnRight: 'Turn Right',
            pynlm.ActionType.Look: 'Look',
            pynlm.ActionType.Interact: 'Interact',
            pynlm.ActionType.Eat: 'Eat',
            pynlm.ActionType.Drink: 'Drink',
            pynlm.ActionType.Rest: 'Rest',
            pynlm.ActionType.Wait: 'Wait',
            pynlm.ActionType.Custom: 'Custom'
        }
        
        return action_names.get(action, f'Unknown ({action})')
```

## 4. Custom Extensions and Integrations

### Extending the Python API

```python
import pybind11/pybind11.h
#include <pybind11/functional.h>
#include <pybind11/stl.h>

// Example: Extending pynlm with custom functions

namespace py = pybind11;
namespace nlm {

// Custom exception classes
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Add custom functions
    m.def("validate_config", &validate_config, py::arg("config"),
          "Validate a configuration object");

    m.def("run_simulation_batch", &run_simulation_batch,
          py::arg("configs"), py::arg("steps"), py::arg("repeats") = 1,
          "Run multiple simulations in batch");

    m.def("analyze_spike_patterns", &analyze_spike_patterns,
          py::arg("brain"), py::arg("window_size"), py::arg("threshold") = 0.5,
          "Analyze spike patterns in a brain object");

    // Add custom classes
    py::class_<AdvancedAgentBrain, std::shared_ptr<AdvancedAgentBrain>>(m, "AdvancedAgentBrain")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("set_learning_strategy", &AdvancedAgentBrain::setLearningStrategy)
        .def("get_learning_stats", &AdvancedAgentBrain::getLearningStats)
        .def("export_brain_state", &AdvancedAgentBrain::exportBrainState)
        .def("import_brain_state", &AdvancedAgentBrain::importBrainState);

    // Add constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1;
    m.attr("DEFAULT_PLATONIC_DIMENSION") = 2;
    m.attr("MAX_SIMULATION_STEPS") = 1000000;

    // Add type conversions
    py::implicitly_convertible<std::string, pynlm::Config>();
}

// Implementation of custom functions
bool validate_config(const Config& config) {
    try {
        // Check required fields
        if (!config.has("brain.neuron_count")) {
            throw ConfigurationError("Missing brain.neuron_count");
        }
        
        // Validate values
        int neuron_count = config.get<int>("brain.neuron_count");
        if (neuron_count <= 0) {
            throw ConfigurationError("Neuron count must be positive");
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Configuration validation failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::vector<double>> run_simulation_batch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int steps_per_simulation,
    int num_repeats) {
    
    std::vector<std::vector<double>> results;
    
    for (const auto& config : configs) {
        for (int repeat = 0; repeat < num_repeats; ++repeat) {
            auto brain = createBrain(*config);
            brain->initialize();
            
            std::vector<double> simulation_results;
            
            for (int step = 0; step < steps_per_simulation; ++step) {
                brain->step(step);
                simulation_results.push_back(brain->getAverageFiringRate());
            }
            
            results.push_back(simulation_results);
        }
    }
    
    return results;
}

std::vector<std::vector<double>> analyze_spike_patterns(
    const Brain& brain,
    int window_size,
    double threshold) {
    
    std::vector<std::vector<double>> pattern_analysis;
    
    // Analyze spike patterns in different time windows
    for (int window_start = 0; window_start < 1000; window_start += window_size) {
        std::vector<double> window_patterns;
        
        // Extract spike data (simplified)
        for (int neuron = 0; neuron < brain.getTotalNeuronCount(); ++neuron) {
            // Get spike data for this neuron in current window
            // This would require access to internal spike recording
            double spike_pattern = 0.0;  // Placeholder
            window_patterns.push_back(spike_pattern);
        }
        
        pattern_analysis.push_back(window_patterns);
    }
    
    return pattern_analysis;
}
```

### Advanced Neural Recording and Analysis

```python
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import pynlm

class AdvancedNeuralRecorder:
    def __init__(self, brain, agent, world, recording_duration=1000):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.recording_duration = recording_duration
        
        # Recording buffers
        self.spike_times = []
        self.firing_rates = []
        self.neuromodulation_levels = []
        self.curiosity_levels = []
        self.novelty_levels = []
        self.action_history = []
        self.reward_history = []
        
        # Recording state
        self.recording = False
        self.current_step = 0
    
    def start_recording(self):
        """Start recording neural activity"""
        self.recording = True
        self.spike_times = []
        self.firing_rates = []
        self.neuromodulation_levels = []
        self.curiosity_levels = []
        self.novelty_levels = []
        self.action_history = []
        self.reward_history = []
        
        print("Started recording neural activity...")
    
    def stop_recording(self):
        """Stop recording neural activity"""
        self.recording = False
        print(f"Recording stopped. Recorded {len(self.spike_times)} spike events.")
    
    def record_step(self):
        """Record data for current simulation step"""
        if not self.recording:
            return
        
        # Record spike information
        self.spike_times.append(self.current_step)
        
        # Record firing statistics
        self.firing_rates.append(self.brain.getAverageFiringRate())
        self.neuromodulation_levels.append(self.agent.getNeuromodulationLevel())
        self.curiosity_levels.append(self.agent.getCuriosityLevel())
        self.novelty_levels.append(self.agent.getNoveltyLevel())
        
        # Record actions
        action = self.agent.decodeMotorCommand()
        self.action_history.append(action)
        
        # Record rewards (simplified)
        # This would depend on how rewards are accessed in the world
        self.reward_history.append(0.0)  # Placeholder
        
        self.current_step += 1
    
    def analyze_spike_patterns(self):
        """Analyze spike patterns using advanced signal processing"""
        if not self.spike_times:
            return None
        
        # Convert spike times to spike train
        spike_train = np.zeros(self.recording_duration)
        for spike_time in self.spike_times:
            if spike_time < self.recording_duration:
                spike_train[spike_time] = 1
        
        analysis_results = {
            'spike_count': len(self.spike_times),
            'spike_rate': len(self.spike_times) / self.recording_duration,
            'firing_rate_mean': np.mean(self.firing_rates),
            'firing_rate_std': np.std(self.firing_rates),
            'firing_rate_cv': np.std(self.firing_rates) / np.mean(self.firing_rates) if np.mean(self.firing_rates) > 0 else 0,
            'burst_frequency': self.detect_bursts(spike_train),
            'regularity': self.calculate_spike_irregularity(spike_train),
            'synchrony': self.calculate_neural_synchrony()
        }
        
        return analysis_results
    
    def detect_bursts(self, spike_train, min_burst_length=5, min_inter_burst_interval=20):
        """Detect spike bursts using threshold crossing"""
        # Find peaks in spike train
        smoothed = signal.savgol_filter(spike_train, 5, 1)
        threshold = np.mean(smoothed) + 2 * np.std(smoothed)
        
        # Find threshold crossings
        above_threshold = smoothed > threshold
        
        # Find burst boundaries
        diff = np.diff(above_threshold.astype(int))
        starts = np.where(diff == 1)[0] + 1
        ends = np.where(diff == -1)[0]
        
        # Handle edge cases
        if above_threshold[0]:
            starts = np.insert(starts, 0, 0)
        if above_threshold[-1]:
            ends = np.append(ends, len(above_threshold) - 1)
        
        # Filter bursts by minimum length
        bursts = []
        for start, end in zip(starts, ends):
            if end - start + 1 >= min_burst_length:
                bursts.append((start, end))
        
        # Filter by minimum inter-burst interval
        filtered_bursts = []
        for i, (start, end) in enumerate(bursts):
            if i == 0 or start - ends[i-1] >= min_inter_burst_interval:
                filtered_bursts.append((start, end))
        
        return len(filtered_bursts)
    
    def calculate_spike_irregularity(self, spike_train):
        """Calculate spike train irregularity using coefficient of variation"""
        # Calculate interspike intervals
        spike_indices = np.where(spike_train == 1)[0]
        if len(spike_indices) < 2:
            return 0.0
        
        isi = np.diff(spike_indices)
        return np.std(isi) / np.mean(isi)
    
    def calculate_neural_synchrony(self):
        """Calculate neural synchrony across the population"""
        # Simplified synchrony calculation
        # In a real implementation, this would use actual spike timing data
        if len(self.spike_times) < 2:
            return 0.0
        
        # Calculate cross-correlation between firing rate time series
        # and neuromodulation levels
        synchrony = np.corrcoef(
            np.array(self.firing_rates),
            np.array(self.neuromodulation_levels)
        )[0, 1]
        
        return abs(synchrony)
    
    def visualize_advanced_recording(self):
        """Create advanced visualization of recorded data"""
        if not self.spike_times:
            print("No recording data available.")
            return
        
        # Create comprehensive visualization
        fig, axes = plt.subplots(4, 3, figsize=(18, 12))
        fig.suptitle('Advanced Neural Recording Analysis', fontsize=16, fontweight='bold')
        
        time_steps = len(self.spike_times)
        
        # Plot 1: Spike raster
        ax = axes[0, 0]
        self.plot_spike_raster(ax, self.spike_times)
        
        # Plot 2: Firing rate dynamics
        ax = axes[0, 1]
        self.plot_firing_rate_dynamics(ax, self.firing_rates)
        
        # Plot 3: Neuromodulation
        ax = axes[0, 2]
        self.plot_neuromodulation(ax, self.neuromodulation_levels)
        
        # Plot 4: Curiosity evolution
        ax = axes[1, 0]
        self.plot_curiosity_evolution(ax, self.curiosity_levels)
        
        # Plot 5: Novelty detection
        ax = axes[1, 1]
        self.plot_novelty_detection(ax, self.novelty_levels)
        
        # Plot 6: Action selection
        ax = axes[1, 2]
        self.plot_action_selection(ax, self.action_history)
        
        # Plot 7: Reward history
        ax = axes[2, 0]
        self.plot_reward_history(ax, self.reward_history)
        
        # Plot 8: Spike statistics
        ax = axes[2, 1]
        self.plot_spike_statistics(ax, self.spike_times)
        
        # Plot 9: Performance metrics
        ax = axes[2, 2]
        self.plot_performance_metrics(ax, self.firing_rates, 
                                      self.curiosity_levels, 
                                      self.neuromodulation_levels)
        
        # Plot 10: Burst analysis
        ax = axes[0, 1]  # Replace existing firing rate plot
        self.plot_burst_analysis(ax, self.spike_times)
        
        plt.tight_layout()
        plt.show()
    
    def plot_spike_raster(self, ax, spike_times):
        """Plot spike raster"""
        if len(spike_times) == 0:
            return
        
        # Create spike raster
        for i, spike_time in enumerate(spike_times):
            ax.plot(spike_time, i, 'k.', markersize=1)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Spike Index')
        ax.set_title('Spike Raster')
        ax.set_xlim(0, max(spike_times) if spike_times else 100)
        ax.grid(True, alpha=0.3)
    
    def plot_firing_rate_dynamics(self, ax, firing_rates):
        """Plot firing rate dynamics"""
        time_steps = range(len(firing_rates))
        
        # Apply smoothing
        window_size = 10
        if len(firing_rates) >= window_size:
            smoothed = np.convolve(firing_rates, 
                                  np.ones(window_size)/window_size, 
                                  mode='valid')
            ax.plot(time_steps[window_size-1:], smoothed, linewidth=2)
        
        ax.plot(time_steps, firing_rates, alpha=0.3, linewidth=0.5)
        ax.set_xlabel('Step')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Firing Rate Dynamics')
        ax.grid(True, alpha=0.3)
    
    def plot_neuromodulation(self, ax, neuromodulation_levels):
        """Plot neuromodulation levels"""
        time_steps = range(len(neuromodulation_levels))
        
        ax.plot(time_steps, neuromodulation_levels, linewidth=2, color='red')
        ax.fill_between(time_steps, 0, neuromodulation_levels, alpha=0.3, color='red')
        ax.set_xlabel('Step')
        ax.set_ylabel('Neuromodulation Level')
        ax.set_title('Neuromodulation Dynamics')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def plot_curiosity_evolution(self, ax, curiosity_levels):
        """Plot curiosity evolution"""
        time_steps = range(len(curiosity_levels))
        
        ax.plot(time_steps, curiosity_levels, linewidth=2, color='green')
        ax.fill_between(time_steps, 0, curiosity_levels, alpha=0.3, color='green')
        ax.set_xlabel('Step')
        ax.set_ylabel('Curiosity Level')
        ax.set_title('Curiosity Evolution')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def plot_novelty_detection(self, ax, novelty_levels):
        """Plot novelty detection"""
        time_steps = range(len(novelty_levels))
        
        ax.plot(time_steps, novelty_levels, linewidth=2, color='blue')
        ax.fill_between(time_steps, 0, novelty_levels, alpha=0.3, color='blue')
        ax.set_xlabel('Step')
        ax.set_ylabel('Novelty Level')
        ax.set_title('Novelty Detection')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def plot_action_selection(self, ax, action_history):
        """Plot action selection over time"""
        if not action_history:
            return
        
        # Count action frequencies
        action_counts = {}
        for action in action_history:
            action_name = self.get_action_name(action)
            action_counts[action_name] = action_counts.get(action_name, 0) + 1
        
        # Create bar chart
        actions = list(action_counts.keys())
        counts = list(action_counts.values())
        
        bars = ax.bar(range(len(actions)), counts, alpha=0.7)
        ax.set_xticks(range(len(actions)))
        ax.set_xticklabels(actions, rotation=45)
        ax.set_ylabel('Frequency')
        ax.set_title('Action Selection Distribution')
        ax.grid(True, alpha=0.3)
        
        # Add value labels
        for bar, count in zip(bars, counts):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                   str(count), ha='center', va='bottom')
    
    def plot_reward_history(self, ax, reward_history):
        """Plot reward history"""
        time_steps = range(len(reward_history))
        
        ax.plot(time_steps, reward_history, linewidth=2, color='orange')
        ax.fill_between(time_steps, 0, reward_history, alpha=0.3, color='orange')
        ax.set_xlabel('Step')
        ax.set_ylabel('Reward')
        ax.set_title('Reward History')
        ax.grid(True, alpha=0.3)
    
    def plot_spike_statistics(self, ax, spike_times):
        """Plot spike statistics"""
        if len(spike_times) < 2:
            return
        
        isi = np.diff(spike_times)
        
        # ISI distribution
        hist, bins, patches = ax.hist(isi, bins=30, alpha=0.7, density=True)
        ax.set_xlabel('Interspike Interval')
        ax.set_ylabel('Density')
        ax.set_title('Interspike Interval Distribution')
        ax.grid(True, alpha=0.3)
    
    def plot_performance_metrics(self, ax, firing_rates, curiosity_levels, neuromodulation_levels):
        """Plot comprehensive performance metrics"""
        time_steps = range(len(firing_rates))
        
        # Calculate composite performance metric
        perf_firing = np.array(firing_rates) / 100
        perf_curiosity = np.array(curiosity_levels)
        perf_neuromod = np.array(neuromodulation_levels)
        
        performance = perf_firing * 0.4 + perf_curiosity * 0.3 + perf_neuromod * 0.3
        
        ax.plot(time_steps, performance, linewidth=3, color='purple')
        ax.set_xlabel('Step')
        ax.set_ylabel('Performance Score')
        ax.set_title('System Performance Metrics')
        ax.set_ylim(0, 1.2)
        ax.grid(True, alpha=0.3)
    
    def plot_burst_analysis(self, ax, spike_times):
        """Plot burst analysis"""
        if len(spike_times) < 2:
            return
        
        isi = np.diff(spike_times)
        
        # Create scatter plot of ISI vs next ISI
        ax.scatter(isi[:-1], isi[1:], alpha=0.5, s=10)
        ax.set_xlabel('Current ISI')
        ax.set_ylabel('Next ISI')
        ax.set_title('Burst Analysis (ISI Correlation)')
        ax.grid(True, alpha=0.3)
    
    def get_action_name(self, action):
        """Convert action to readable name"""
        action_names = {
            pynlm.ActionType.MoveForward: 'Move Forward',
            pynlm.ActionType.MoveBackward: 'Move Backward',
            pynlm.ActionType.TurnLeft: 'Turn Left',
            pynlm.ActionType.TurnRight: 'Turn Right',
            pynlm.ActionType.Look: 'Look',
            pynlm.ActionType.Interact: 'Interact',
            pynlm.ActionType.Eat: 'Eat',
            pynlm.ActionType.Drink: 'Drink',
            pynlm.ActionType.Rest: 'Rest',
            pynlm.ActionType.Wait: 'Wait',
            pynlm.ActionType.Custom: 'Custom'
        }
        
        return action_names.get(action, f'Unknown ({action})')
```

## 5. Custom Extensions and Integrations

### Extending the Pybind11 Module

The most powerful feature of NLM is its extensibility through C++ extensions. Here's how to add custom functionality:

```python
# Example: Extending pynlm with custom neural operations

import pybind11/pybind11.h
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace py = pybind11;
namespace nlm {

// Custom neural operations
class CustomNeuralOps {
public:
    static std::vector<double> apply_custom_plasticity(
        const Brain& brain,
        const std::vector<double>& weights,
        const std::string& rule_name) {
        
        std::vector<double> modified_weights = weights;
        
        if (rule_name == "adaptive_stdp") {
            // Implement custom STDP with adaptive learning rate
            double learning_rate = 0.001;
            // Custom STDP implementation
        } else if (rule_name == "hebbian_with_noise") {
            // Implement Hebbian learning with noise
            double noise_level = 0.01;
            // Add noise to weight updates
        } else if (rule_name == "reward_based") {
            // Implement reward-based plasticity
            // Use reward signals to guide learning
        }
        
        return modified_weights;
    }
    
    static std::vector<double> calculate_neural_complexity(
        const Brain& brain,
        const std::vector<double>& spike_patterns) {
        
        // Calculate various complexity metrics
        double mean_spike_rate = std::accumulate(spike_patterns.begin(), 
                                                spike_patterns.end(), 0.0) / spike_patterns.size();
        
        double variance = 0.0;
        for (double rate : spike_patterns) {
            variance += (rate - mean_spike_rate) * (rate - mean_spike_rate);
        }
        variance /= spike_patterns.size();
        
        double coefficient_of_variation = std::sqrt(variance) / mean_spike_rate;
        
        return {mean_spike_rate, variance, coefficient_of_variation};
    }
};

PYBIND11_MODULE(pynlm, m) {
    // Add custom functions
    m.def("apply_custom_plasticity", &CustomNeuralOps::apply_custom_plasticity,
          py::arg("brain"), py::arg("weights"), py::arg("rule_name"),
          "Apply custom plasticity rules to synaptic weights");
    
    m.def("calculate_neural_complexity", &CustomNeuralOps::calculate_neural_complexity,
          py::arg("brain"), py::arg("spike_patterns"),
          "Calculate complexity metrics for neural activity");
    
    // Add custom exception handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
}
```

### Advanced Integration Examples

```python
# Integration example: Custom world simulation
import pynlm

class PhysicsBasedWorld(pynlm.SimpleWorld):
    def __init__(self):
        super().__init__()
        self.physics_engine = PhysicsEngine()
        self.gravity = 9.81
        self.friction = 0.1
    
    def update_physics(self, timestep):
        """Update world physics based on simulation timestep"""
        # Get current agent state
        agent_body = self.getAgentBody()
        
        # Apply gravity
        agent_body.velocityY -= self.gravity * timestep
        
        # Apply friction
        agent_body.velocityX *= (1 - self.friction * timestep)
        agent_body.velocityY *= (1 - self.friction * timestep)
        
        # Update position
        agent_body.x += agent_body.velocityX * timestep
        agent_body.y += agent_body.velocityY * timestep
        
        # Boundary conditions
        self.apply_boundaries(agent_body)
    
    def apply_boundaries(self, body):
        """Apply boundary conditions to agent"""
        width, height = self.getWidth(), self.getHeight()
        
        # Wall collisions
        if body.x < 0:
            body.x = 0
            body.velocityX = -body.velocityX * 0.8
        elif body.x > width:
            body.x = width
            body.velocityX = -body.velocityX * 0.8
        
        if body.y < 0:
            body.y = 0
            body.velocityY = -body.velocityY * 0.8
        elif body.y > height:
            body.y = height
            body.velocityY = -body.velocityY * 0.8

# Integration example: Custom agent with goal system
class GoalDrivenAgent(pynlm.AgentBrain):
    def __init__(self, brain):
        super().__init__(brain)
        self.goals = []
        self.goal_weights = {}
        self.action_planner = ActionPlanner()
    
    def add_goal(self, goal_name, priority, target_value):
        """Add a goal to the agent's goal system"""
        self.goals.append({
            'name': goal_name,
            'priority': priority,
            'target_value': target_value,
            'current_value': 0.0,
            'achievement_history': []
        })
        self.goal_weights[goal_name] = priority
    
    def evaluate_goals(self):
        """Evaluate progress toward current goals"""
        goal_progress = {}
        
        for goal in self.goals:
            current_value = self.calculate_goal_value(goal['name'])
            progress = (current_value - goal['current_value']) / goal['target_value']
            goal_progress[goal['name']] = progress
            
            goal['current_value'] = current_value
            goal['achievement_history'].append(progress)
        
        return goal_progress
    
    def calculate_goal_value(self, goal_name):
        """Calculate current value of a goal based on perception"""
        # This would depend on the specific goal
        # Examples:
        # - Energy goal: based on agent's energy level
        # - Exploration goal: based on novel experiences
        # - Social goal: based on interactions with other agents
        
        if goal_name == "energy":
            return self.brain.getAgentBody().energy
        elif goal_name == "curiosity":
            return self.getCuriosityLevel()
        elif goal_name == "novelty":
            return self.getNoveltyLevel()
        else:
            return 0.0
    
    def plan_action_based_on_goals(self):
        """Plan actions based on goal achievement"""
        goal_progress = self.evaluate_goals()
        
        # Sort goals by priority and progress
        sorted_goals = sorted(
            goal_progress.items(),
            key=lambda x: (x[1], self.goal_weights[x[0]]),
            reverse=True
        )
        
        # Select action based on highest priority, least achieved goal
        if sorted_goals:
            least_achieved = sorted_goals[0]
            if least_achieved[1] < 0.8:  # If goal is not 80% achieved
                return self.action_planner.plan_action_for_goal(least_achieved[0])
        
        # Default to curiosity-driven exploration if goals met
        return self.decodeMotorCommand()
```

## 6. Contributing to NLM

### Developing Extensions

#### Step 1: Set Up Development Environment

```bash
# Clone the repository
cd /path/to/nlm

# Create development virtual environment
python -m venv nlm_dev
source nlm_dev/bin/activate

# Install development dependencies
pip install -e .[dev]

# Install additional tools
pip install pytest pytest-cov coverage black flake8 mypy pre-commit
```

#### Step 2: Understanding the Extension System

NLM's extension system is based on:

1. **Pybind11 Extensions**: Add custom Python functions and classes
2. **Plugin Architecture**: Modular system for adding new functionality
3. **Configuration System**: Extend configuration options
4. **World Extensions**: Create custom simulation environments
5. **Neural Extensions**: Add custom neuron types and dynamics

#### Step 3: Creating Custom Extensions

```python
# Example: Creating a custom extension module

# 1. Create a new module in the python directory
# custom_extensions.py

# 2. Extend pybind11 bindings
# python/bindings.cpp

#include <pybind11/pybind11.h>
#include "CustomExtensions.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm_custom, m) {
    py::class_<CustomExtensions>(m, "CustomExtensions")
        .def(py::init<>())
        .def("add_custom_neuron", &CustomExtensions::addCustomNeuron)
        .def("configure_custom_plasticity", &CustomExtensions::configureCustomPlasticity)
        .def("run_custom_simulation", &CustomExtensions::runCustomSimulation);
    
    // Register custom exceptions
    py::register_exception<CustomException>(m, "CustomException");
}
```

#### Step 4: Testing and Validation

```python
# Create comprehensive test suite for custom extensions
import pytest
import pynlm

class TestCustomExtensions:
    def setup_method(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
    def test_custom_neuron_creation(self):
        # Test custom neuron creation
        result = pynlm.create_custom_neuron(...)
        assert result is not None
        
    def test_custom_plasticity(self):
        # Test custom plasticity rules
        result = pynlm.apply_custom_plasticity(...)
        assert result is not None
        
    def test_custom_simulation(self):
        # Test custom simulation
        result = pynlm.run_custom_simulation(...)
        assert result is not None
```

#### Step 5: Documentation and Examples

```python
# Create comprehensive documentation for extensions
# extension_documentation.md

# Extension Documentation Template

## Extension: [Extension Name]

### Overview
[Brief description of the extension]

### Features
- [Feature 1]
- [Feature 2]
- [Feature 3]

### Installation
```bash
pip install pynlm[extensions]
```

### Usage
```python
import pynlm

# Basic usage
result = pynlm.extension_function(...)

# Advanced usage
advanced_result = pynlm.extension_function(advanced_params...)
```

### Examples
[Code examples showing different use cases]

### API Reference
[Detailed API documentation]

### Limitations
[Any limitations or considerations]

### Contributing
[How to contribute to the extension]
```

#### Step 6: Submission and Integration

```python
# Create a pull request with your extension
# Include:

1. **Code Changes**: Custom extensions
2. **Documentation**: Usage guides and examples
3. **Tests**: Test suite for the extension
4. **README Updates**: Updated project documentation
5. **Changelog**: Note changes in the project changelog

# Example pull request template

## Summary
Brief description of the extension and its purpose

## Changes Made
1. Added [Feature 1] in [file]
2. Added [Feature 2] in [file]
3. Updated documentation in [file]

## Testing
- Added unit tests in [test_file]
- Verified compatibility with existing API
- Performance benchmarks included

## Documentation
- Updated API reference
- Added usage examples
- Created user guide

## Impact
- Improves NLM functionality by [specific improvement]
- Enables new use cases
- Maintains backward compatibility
```

## 7. Advanced Research and Development

### Research Applications

```python
# Research application: Studying neural development
class NeuralDevelopmentResearch:
    def __init__(self):
        self.research_parameters = {
            'initial_neuron_count': 100,
            'connectivity_probability': 0.05,
            'plasticity_rate': 0.001,
            'environmental_complexity': 0.5
        }
    
    def run_development_experiment(self, num_generations=10):
        """Run multiple generations of neural development"""
        for generation in range(num_generations):
            print(f"Running generation {generation + 1}/{num_generations}")
            
            # Create initial population
            population = self.create_initial_population(generation)
            
            # Evolve population
            evolved_population = self.evolve_population(population, generation)
            
            # Analyze results
            self.analyze_generation(evolved_population, generation)
    
    def create_initial_population(self, generation):
        """Create initial population of neural systems"""
        population = []
        
        for individual in range(10):  # 10 individuals per generation
            # Create random brain configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 
                      int(self.research_parameters['initial_neuron_count'] * 
                          (1 + np.random.normal(0, 0.1))))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**self.get_environment_for_generation(generation))
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning based on research parameters
            self.configure_learning_for_generation(agent)
            
            population.append({
                'brain': brain,
                'world': world,
                'agent': agent,
                'generation': generation,
                'fitness': self.evaluate_fitness(brain, world, agent, generation)
            })
        
        return population
    
    def evolve_population(self, population, generation):
        """Evolve population using selection and mutation"""
        # Select parents based on fitness
        parents = self.select_parents(population)
        
        # Create offspring through mutation and crossover
        offspring = self.create_offspring(parents, generation)
        
        return offspring
    
    def evaluate_fitness(self, brain, world, agent, generation):
        """Evaluate fitness of an individual"""
        # Run simulation and evaluate performance
        fitness_metrics = {
            'survival_time': self.run_fitness_simulation(brain, world, agent),
            'learning_efficiency': self.calculate_learning_efficiency(brain, agent),
            'adaptation_ability': self.calculate_adaptation_ability(agent, generation)
        }
        
        # Combine metrics into overall fitness score
        overall_fitness = (
            fitness_metrics['survival_time'] * 0.5 +
            fitness_metrics['learning_efficiency'] * 0.3 +
            fitness_metrics['adaptation_ability'] * 0.2
        )
        
        return overall_fitness
    
    def run_fitness_simulation(self, brain, world, agent, max_steps=100):
        """Run simulation to evaluate fitness"""
        for step in range(max_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check for termination conditions
            if self.check_termination_condition(brain, world, agent):
                break
        
        return step + 1  # Total steps survived
```

### Advanced Analysis Tools

```python
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import pynlm

class AdvancedAnalysisSuite:
    def __init__(self, data_directory="analysis_results"):
        self.data_directory = data_directory
        self.results = []
    
    def collect_comprehensive_data(self, simulation_runs):
        """Collect comprehensive data from multiple simulation runs"""
        for run_id in range(simulation_runs):
            print(f"Running simulation {run_id + 1}/{simulation_runs}")
            
            # Create simulation
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run simulation and collect data
            run_data = self.run_and_collect_data(brain, world, agent, run_id)
            self.results.append(run_data)
    
    def run_and_collect_data(self, brain, world, agent, run_id):
        """Run simulation and collect detailed data"""
        data = {
            'run_id': run_id,
            'steps': list(range(500)),
            'firing_neurons': [],
            'total_spikes': [],
            'avg_firing_rate': [],
            'curiosity': [],
            'novelty': [],
            'neuromodulation': [],
            'dev_stage': [],
            'simulation_time': [],
            'action_types': {},
            'spike_patterns': []
        }
        
        for step in range(500):
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect data
            data['firing_neurons'].append(brain.getFiringNeuronCount())
            data['total_spikes'].append(brain.getTotalSpikeCount())
            data['avg_firing_rate'].append(brain.getAverageFiringRate())
            data['curiosity'].append(agent.getCuriosityLevel())
            data['novelty'].append(agent.getNoveltyLevel())
            data['neuromodulation'].append(agent.getNeuromodulationLevel())
            data['dev_stage'].append(brain.getDevelopmentalStage())
            data['simulation_time'].append(world.getSimulationTime())
            
            # Track actions
            action_name = self.get_action_name(action)
            data['action_types'][action_name] = data['action_types'].get(action_name, 0) + 1
            
            # Record spike patterns periodically
            if step % 50 == 0:
                spike_pattern = self.extract_spike_pattern(brain, step)
                data['spike_patterns'].append(spike_pattern)
        
        return data
    
    def extract_spike_pattern(self, brain, step):
        """Extract spike pattern from brain"""
        # This would require access to internal spike recording
        # For now, return a simplified pattern
        pattern = {
            'step': step,
            'neuron_count': brain.getTotalNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'firing_neurons': brain.getFiringNeuronCount()
        }
        
        return pattern
    
    def statistical_analysis(self):
        """Perform comprehensive statistical analysis"""
        # Convert results to DataFrame for analysis
        df_data = []
        
        for run_data in self.results:
            for i, step in enumerate(run_data['steps']):
                df_data.append({
                    'run_id': run_data['run_id'],
                    'step': step,
                    'firing_neurons': run_data['firing_neurons'][i],
                    'avg_firing_rate': run_data['avg_firing_rate'][i],
                    'curiosity': run_data['curiosity'][i],
                    'novelty': run_data['novelty'][i],
                    'neuromodulation': run_data['neuromodulation'][i],
                    'dev_stage': run_data['dev_stage'][i]
                })
        
        df = pd.DataFrame(df_data)
        
        # Perform statistical tests
        analysis_results = {
            'firing_rate_statistics': self.analyze_time_series(df, 'avg_firing_rate'),
            'curiosity_statistics': self.analyze_time_series(df, 'curiosity'),
            'novelty_statistics': self.analyze_time_series(df, 'novelty'),
            'neuromodulation_statistics': self.analyze_time_series(df, 'neuromodulation'),
            'cross_correlations': self.calculate_cross_correlations(df),
            'developmental_transitions': self.analyze_developmental_transitions(df)
        }
        
        return analysis_results
    
    def analyze_time_series(self, df, column):
        """Analyze time series data"""
        # Remove run_id grouping for overall statistics
        values = df[column].dropna()
        
        analysis = {
            'mean': values.mean(),
            'std': values.std(),
            'min': values.min(),
            'max': values.max(),
            'median': values.median(),
            'q25': values.quantile(0.25),
            'q75': values.quantile(0.75),
            'skewness': stats.skew(values),
            'kurtosis': stats.kurtosis(values),
            'stationarity': self.check_stationarity(values)
        }
        
        return analysis
    
    def calculate_cross_correlations(self, df):
        """Calculate cross-correlations between variables"""
        correlations = {}
        
        variables = ['avg_firing_rate', 'curiosity', 'novelty', 'neuromodulation']
        
        for i, var1 in enumerate(variables):
            for j, var2 in enumerate(variables[i+1:], i+1):
                correlation = df[var1].corr(df[var2])
                correlations[f"{var1}_vs_{var2}"] = {
                    'correlation': correlation,
                    'p_value': stats.pearsonr(df[var1], df[var2])[1]
                }
        
        return correlations
    
    def analyze_developmental_transitions(self, df):
        """Analyze developmental stage transitions"""
        dev_transitions = {}
        
        for run_id in df['run_id'].unique():
            run_data = df[df['run_id'] == run_id]
            
            # Find stage transitions
            stages = run_data['dev_stage'].unique()
            transitions = []
            
            for i in range(len(stages) - 1):
                current_stage = stages[i]
                next_stage = stages[i + 1]
                transition_step = run_data[run_data['dev_stage'] == next_stage]['step'].min()
                
                transitions.append({
                    'from_stage': current_stage,
                    'to_stage': next_stage,
                    'transition_step': transition_step
                })
            
            dev_transitions[run_id] = transitions
        
        return dev_transitions
    
    def create_advanced_visualizations(self, analysis_results):
        """Create advanced visualizations of analysis results"""
        # This would create comprehensive plots and charts
        # Similar to AdvancedVisualizer but for analysis results
        pass
```

## 8. Project Future Directions

### Research Roadmap

#### Short-term (Next 6 months)
1. **Complete Memory Integration**: Connect working memory to brain loop
2. **Implement Prediction System**: Make prediction error computation operational
3. **Enhance Save/Load**: Implement real checkpoint system
4. **Develop Advanced Extensions**: Create comprehensive plugin system

#### Medium-term (6-18 months)
1. **Advanced Learning Algorithms**: Implement novel plasticity rules
2. **Neural Architecture Search**: Automated design of optimal brain architectures
3. **Multi-Modal Perception**: Integrate visual, auditory, and internal signals
4. **Social Learning**: Enable agent-to-agent knowledge transfer

#### Long-term (18+ months)
1. **Biological Plausibility**: Validate against biological brain data
2. **Complex Task Solving**: Demonstrate human-level problem solving
3. **Transfer Learning**: Enable knowledge transfer across tasks
4. **Self-Awareness**: Develop mechanisms for self-monitoring and reflection

### Community and Collaboration

#### Open Source Development
```bash
# How to contribute to NLM development

# 1. Fork the repository
git clone https://github.com/nlm-project/nlm.git

# 2. Create a feature branch
git checkout -b feature/improved-python-api

# 3. Make your changes
# ... (implement your changes)

# 4. Test thoroughly
pytest tests/
python examples/example.py

# 5. Commit changes
git add .
git commit -m "Add improved Python API with error handling"

# 6. Push to your fork
git push origin feature/improved-python-api

# 7. Create a pull request
# ... (create PR with detailed description)
```

#### Research Collaboration
NLM is designed for collaborative research:

1. **Shared Repositories**: Store simulation results and models
2. **Standardized Formats**: Common data exchange formats
3. **Reproducible Research**: Enable replication of experiments
4. **Open Extensions**: Allow community contributions

### Support and Resources

#### Getting Help
- **Documentation**: Comprehensive guides and API references
- **Examples**: Working examples for different use cases
- **Community**: Forum for questions and discussions
- **Support**: Issue tracking for bug reports and feature requests

#### Development Resources
- **Source Code**: Full access to implementation
- **Build Tools**: Automated build and testing scripts
- **Performance Tools**: Benchmarking and optimization utilities
- **Extension Framework**: Extensible architecture for custom functionality

#### Research Resources
- **Scientific Background**: Research papers and theoretical foundations
- **Experiment Templates**: Pre-built experimental setups
- **Analysis Tools**: Statistical analysis and visualization
- **Data Collection**: Standardized data recording and analysis

## Quick Reference for Advanced Users

### Key Extensions and Customizations
```python
# Advanced brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Custom world setup
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

# Advanced agent setup
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_all_learning()  # Enable all learning systems
```

### Performance Optimization
```python
# Profile simulation performance
monitor = AdvancedPerformanceOptimizer(brain, world, agent)
optimized_config = monitor.optimized_brain_parameters()

# Memory efficiency
memory_stats = monitor.memory_efficiency_optimization()

# Architecture benchmarking
benchmark_results = monitor.benchmark_architecture_scenarios()
```

### Advanced Analysis
```python
# Comprehensive analysis
analyzer = AdvancedAnalysisSuite()
analyzer.collect_comprehensive_data(50)
results = analyzer.statistical_analysis()

# Advanced visualization
visualizer = AdvancedVisualizer(brain, world, agent)
visualizer.create_comprehensive_dashboard(1000)
```

### Custom Extensions
```python
# Create custom extensions
ext = CustomExtensions()
custom_neuron = ext.add_custom_neuron(type='adaptive', params={...})
custom_plasticity = ext.configure_custom_plasticity(rule='adaptive_stdp', params={...})
custom_simulation = ext.run_custom_simulation(duration=1000)
```

### Research Tools
```python
# Research tools
researcher = NeuralDevelopmentResearch()
population = researcher.create_initial_population(0)
evolved = researcher.evolve_population(population, 0)

# Advanced analysis
analysis_suite = AdvancedAnalysisSuite()
analysis_suite.collect_comprehensive_data(100)
stats = analysis_suite.statistical_analysis()
```

## Final Thoughts

NLM represents a significant step forward in brain-inspired artificial intelligence. Its modular architecture, comprehensive documentation, and extensibility make it suitable for:

- **Educational use**: Teaching neural computation and AI
- **Research**: Testing hypotheses about brain function
- **Development**: Building applications requiring adaptive intelligence
- **Engineering**: Creating robust, learning systems

The future of NLM lies in continued development of its extension system, refinement of cognitive integration, and validation against biological and behavioral data. Your contributions and innovations will help shape the next generation of brain-inspired AI systems.

Welcome to the advanced world of NLM!
