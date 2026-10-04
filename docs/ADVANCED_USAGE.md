# NLM - Advanced Usage Guide for Experienced Users

**For developers who want to go beyond the basics and customize the NLM framework**

## Part 1: Advanced Brain Configuration

### 1.1 Custom Brain Architectures

Create brains with specific regional configurations:

```python
import pynlm

# Advanced configuration with multiple regions
config = pynlm.createDefaultConfig()

# Configure brain architecture
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.15)

# Set regional properties
config.set("brain.region.0.population.sensory", 500)
config.set("brain.region.0.population.internal", 2000)
config.set("brain.region.0.population.motor", 500)
config.set("brain.region.1.population.sensory", 300)
config.set("brain.region.1.population.internal", 1500)
config.set("brain.region.1.population.motor", 300)
config.set("brain.region.2.population.sensory", 200)
config.set("brain.region.2.population.internal", 1000)
config.set("brain.region.2.population.motor", 200)

# Set specific neuron parameters
config.set("brain.v_thresh", -45.0)      # Higher threshold for faster firing
config.set("brain.v_reset", -60.0)      # Reset potential
config.set("brain.tau_mem", 15.0)       # Faster membrane time constant
config.set("brain.tau_ref", 3.0)        # Shorter refractory period

brain = pynlm.createBrain(config)
brain.initialize()
```

### 1.2 Advanced Plasticity Configuration

Configure specific plasticity rules and parameters:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Enable/disable plasticity systems
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.stdp.tau_plus", 30.0)
config.set("plasticity.stdp.tau_minus", 30.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.01)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
config.set("plasticity.structural.pruning_rate", 0.00005)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Part 2: Memory System Control

### 2.1 Working Memory Configuration

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Get working memory reference
working_memory = brain.getWorkingMemory()

# Configure working memory
working_memory.setCapacity(1000)           # Larger capacity for longer-term storage
working_memory.setDecayRate(0.95)         # Slower decay
working_memory.setCompetitionStrength(0.7) # Stronger competition
working_memory.enableReplay(true)        # Enable memory replay
working_memory.setReplayFrequency(100)    # Replay every 100 steps

# Monitor working memory
print(f"Working memory capacity: {working_memory.getCapacity()}")
print(f"Current active traces: {working_memory.getActiveTraces()}")
```

### 2.2 Episodic Memory Management

```python
# Configure episodic memory
episodic_memory = brain.getEpisodicMemory()

# Set memory parameters
episodic_memory.setMaxEpisodes(5000)       # Store more episodes
episodic_memory.setMemoryPruningThreshold(0.1)  # Remove weaker memories
episodic_memory.setReplayRatio(0.3)       # Replay 30% of memories
episodic_memory.setConsolidationInterval(500)  # More frequent consolidation

# Access stored episodes
episodes = episodic_memory.getRecentEpisodes(10)
for episode in episodes:
    print(f"Episode {episode.timestamp}: {len(episode.activeNeurons)} active neurons")
```

## Part 3: Neuromodulation Control

### 3.1 Advanced Neuromodulation

```python
agent = pynlm.createAgentBrain(brain)

# Configure neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Set neuromodulation parameters
agent.setRewardScale(2.0)                # Amplify reward signals
agent.setCuriositySensitivity(0.5)      # Moderate curiosity
agent.setNoveltyThreshold(0.15)          # Lower novelty detection threshold

# Monitor neuromodulation levels
print(f"Current dopamine level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
```

## Part 4: Prediction and Planning Systems

### 4.1 Prediction System Configuration

```python
# Configure prediction system
prediction_system = brain.getPredictionSystem()

# Set prediction parameters
prediction_system.setPredictionHorizon(20)     # Predict 20 steps ahead
prediction_system.setPredictionAccuracy(0.8)    # Target accuracy
prediction_system.setPredictionUpdateInterval(5) # Update predictions every 5 steps
```

### 4.2 Planning System Control

```python
# Configure planning system
planner = brain.getPlanner()

# Set planning parameters
planner.setPlanningDepth(10)           # Plan 10 steps ahead
planner.setPlanningHorizon(0.5)       # 50% of available time
planner.setEvaluationComplexity("high")  # More sophisticated evaluation
```

## Part 5: Development System Control

### 5.1 Custom Development Stages

```python
# Configure development system
development_system = brain.getDevelopmentSystem()

# Set developmental stages
development_system.setStageDuration("initial", 60.0)
development_system.setStageDuration("critical", 120.0)
development_system.setStageDuration("maturation", 300.0)

# Configure development effects
development_system.setPlasticityModulation("initial", 1.0)
development_system.setPlasticityModulation("critical", 0.8)
development_system.setPlasticityModulation("maturation", 0.5)
```

## Part 6: Advanced World Control

### 6.1 Custom World Configuration

```python
world = pynlm.createSimpleWorld()

# Advanced world configuration
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    timeStep=0.05,
    maxEnergy=200.0,
    energyDecayRate=0.99,
    gravityEnabled=True,
    frictionEnabled=True
)

# Configure world objects
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, value=50.0))
world.addObject(pynlm.WorldObject(30.0, 30.0, pynlm.WorldObjectType.Hazard, radius=2.0))
```

## Part 7: Advanced Agent Configuration

### 7.1 Custom Agent Brain Configuration

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure agent behavior
agent.enableAdvancedMotorControl(True)
agent.setMotorNoiseLevel(0.1)
agent.setSensoryNoiseLevel(0.05)
agent.setActionSelectionPolicy("epsilon-greedy")
agent.setExplorationRate(0.1)

# Configure agent learning
agent.enableTransferLearning(True)
agent.setTransferStrength(0.3)
agent.enableMetaLearning(True)
```

## Part 8: Performance Optimization

### 8.1 Memory and Performance Monitoring

```python
import time
import psutil
import os

class PerformanceMonitor:
    def __init__(self):
        self.start_time = time.time()
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
    
    def record_step(self, step):
        current_time = time.time()
        step_time = current_time - self.start_time
        self.step_times.append(step_time)
        
        process = psutil.Process(os.getpid())
        self.memory_usage.append(process.memory_info().rss / 1024 / 1024)
        self.cpu_usage.append(process.cpu_percent())
        
        self.start_time = current_time
    
    def get_performance_report(self):
        report = {
            'total_steps': len(self.step_times),
            'avg_step_time': sum(self.step_times) / len(self.step_times) if self.step_times else 0,
            'max_step_time': max(self.step_times) if self.step_times else 0,
            'min_step_time': min(self.step_times) if self.step_times else 0,
            'avg_memory_usage': sum(self.memory_usage) / len(self.memory_usage) if self.memory_usage else 0,
            'max_memory_usage': max(self.memory_usage) if self.memory_usage else 0,
            'avg_cpu_usage': sum(self.cpu_usage) / len(self.cpu_usage) if self.cpu_usage else 0,
        }
        return report

monitor = PerformanceMonitor()

# Run simulation with monitoring
for step in range(1000):
    monitor.record_step(step)

monitor.print_report()
```

## Part 9: Advanced Simulation Control

```python
class AdvancedSimulationController:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.checkpoint_interval = 100
        self.simulation_time = 0.0
    
    def run_with_control(self, total_steps, control_events):
        for step in range(total_steps):
            event = self.check_control_events(control_events)
            if event:
                self.handle_control_event(event, step)
            
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step, self.simulation_time)
            
            if step % self.checkpoint_interval == 0:
                self.save_checkpoint(step)
            
            self.simulation_time += 0.1
        
        return self.get_final_state()
    
    def check_control_events(self, control_events):
        current_time = self.simulation_time
        for event in control_events:
            if event['time'] <= current_time and not event['handled']:
                event['handled'] = True
                return event
        return None
    
    def handle_control_event(self, event, step):
        event_type = event['type']
        
        if event_type == 'pause':
            print(f"Simulation paused at step {step}")
        elif event_type == 'speed_up':
            self.world.setTimeStep(0.01)
            print(f"Simulation speed increased at step {step}")
        elif event_type == 'slow_down':
            self.world.setTimeStep(0.5)
            print(f"Simulation speed decreased at step {step}")
        elif event_type == 'reset_brain':
            self.brain.reset()
            self.brain.initialize(self.brain.getConfig())
            print(f"Brain reset at step {step}")
        elif event_type == 'change_environment':
            self.change_environment(event['parameters'])
            print(f"Environment changed at step {step}")
        elif event_type == 'apply_stimulus':
            self.apply_stimulus(event['parameters'])
            print(f"Stimulus applied at step {step}")
    
    def change_environment(self, parameters):
        if 'new_objects' in parameters:
            for obj_params in parameters['new_objects']:
                obj = pynlm.WorldObject(
                    obj_params['x'], obj_params['y'],
                    obj_params['type'], obj_params['value']
                )
                self.world.addObject(obj)
        
        if 'world_size' in parameters:
            self.world.configure(
                parameters['world_size']['width'],
                parameters['world_size']['height'],
                self.world.getVisionWidth(),
                self.world.getVisionHeight()
            )
    
    def apply_stimulus(self, parameters):
        stimulus_type = parameters.get('type', 'current')
        target_neurons = parameters.get('target_neurons', 'sensory')
        
        if stimulus_type == 'current':
            intensity = parameters.get('intensity', 10.0)
            if target_neurons == 'sensory':
                for neuron in self.agent.getBrain().getSensoryNeurons():
                    neuron.injectCurrent(intensity)
            elif target_neurons == 'all':
                for region in self.brain.getRegions():
                    for pop in region.getPopulations():
                        for neuron in pop.getNeurons():
                            neuron.injectCurrent(intensity * 0.1)

# Use advanced simulation controller
controller = AdvancedSimulationController(brain, agent, world)

control_events = [
    {'type': 'pause', 'time': 200.0, 'handled': False},
    {'type': 'speed_up', 'time': 400.0, 'handled': False},
    {'type': 'slow_down', 'time': 600.0, 'handled': False},
    {'type': 'reset_brain', 'time': 800.0, 'handled': False},
]

final_state = controller.run_with_control(1000, control_events)
print(f"Final simulation state: {final_state}")
```

## Part 10: Advanced Experiment Framework

```python
class AdvancedExperiment:
    def __init__(self, name, brain, agent, world):
        self.name = name
        self.brain = brain
        self.agent = agent
        self.world = world
        self.metrics = {}
        self.checkpoint_dir = f"experiments/{name}"
        os.makedirs(self.checkpoint_dir, exist_ok=True)
    
    def run_experiment(self, parameters):
        print(f"Starting experiment: {self.name}")
        
        self.initialize_experiment(parameters)
        results = self.run_simulation(parameters['num_steps'])
        analysis = self.analyze_results(results)
        self.save_results(results, analysis)
        
        return analysis
    
    def initialize_experiment(self, parameters):
        self.brain.reset()
        self.brain.initialize(self.brain.getConfig())
        self.agent.reset()
        
        if 'brain_config' in parameters:
            for key, value in parameters['brain_config'].items():
                self.brain.getConfig().set(key, value)
    
    def run_simulation(self, num_steps):
        for step in range(num_steps):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            self.collect_metrics(step)
            
            if step % 100 == 0:
                self.save_checkpoint(step)
        
        return self.get_experiment_results()
    
    def collect_metrics(self, step):
        self.metrics['steps'] += 1
        current_reward = self.calculate_current_reward()
        self.metrics['total_reward'] += current_reward
        
        firing_rate = self.brain.getAverageFiringRate()
        self.metrics['learning_curve'].append(firing_rate)
    
    def calculate_current_reward(self):
        agent_body = self.agent.getBrain().getAgentBody()
        if agent_body.energy > 0.8:
            return 1.0
        elif agent_body.energy < 0.2:
            return -1.0
        else:
            return 0.0
    
    def analyze_results(self, results):
        analysis = {}
        
        if self.metrics['steps'] > 0:
            analysis['success_rate'] = self.metrics['total_reward'] / self.metrics['steps']
        else:
            analysis['success_rate'] = 0.0
        
        return analysis
    
    def save_results(self, results, analysis):
        import json
        
        experiment_data = {
            'name': self.name,
            'parameters': self.metrics.get('parameters', {}),
            'results': results,
            'analysis': analysis,
            'timestamp': time.time()
        }
        
        filename = f"{self.checkpoint_dir}/experiment_data.json"
        with open(filename, 'w') as f:
            json.dump(experiment_data, f, indent=2)
        
        print(f"Results saved: {filename}")
    
    def save_checkpoint(self, step):
        filename = f"{self.checkpoint_dir}/checkpoint_step_{step}.nml"
        if self.brain.save(filename):
            print(f"Checkpoint saved: {filename}")

# Use advanced experiment framework
experiment = AdvancedExperiment("advanced_behavior_test", brain, agent, world)

experiment_parameters = {
    'num_steps': 2000,
    'brain_config': {
        'brain.neuron_count': 3000,
        'brain.connection_probability': 0.12,
        'plasticity.stdp.learning_rate': 0.008,
    }
}

results = experiment.run_experiment(experiment_parameters)
print(f"Experiment results: {results}")
```

## Part 11: Advanced Visualization and Debugging

```python
import matplotlib.pyplot as plt

class AdvancedBrainVisualizer:
    def __init__(self, brain):
        self.brain = brain
        self.fig, ((self.ax1, self.ax2), (self.ax3, self.ax4)) = plt.subplots(2, 2, figsize=(15, 12))
        self.setup_plots()
    
    def setup_plots(self):
        plt.style.use('seaborn-v0_8')
        self.ax1.set_title('Neural Activity Heatmap')
        self.ax2.set_title('Learning Progress')
        self.ax3.set_title('Memory Usage')
        self.ax4.set_title('Development Progress')
        self.activity_data = []
        self.learning_curve = []
        self.memory_usage = []
        self.development_progress = []
    
    def update_visualization(self, step):
        active_neurons = self.brain.getActiveNeuronCount()
        firing_rate = self.brain.getAverageFiringRate()
        
        self.update_activity_heatmap()
        self.learning_curve.append(firing_rate)
        self.update_learning_curve()
        
        if hasattr(self.brain, 'getWorkingMemory'):
            working_memory = self.brain.getWorkingMemory()
            self.memory_usage.append(working_memory.getActiveTraces())
            self.update_memory_usage()
        
        self.development_progress.append(self.brain.getDevelopmentalStage())
        self.update_development_progress()
        
        if step % 100 == 0:
            self.ax1.text(0.02, 0.98, f'Step: {step}',
                          transform=self.ax1.transAxes, fontsize=10,
                          verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        plt.pause(0.001)
    
    def update_activity_heatmap(self):
        self.ax1.clear()
        regions = self.brain.getRegions()
        if not regions:
            return
        
        grid_size = 20
        activity_grid = np.zeros((grid_size, grid_size))
        
        for region in regions:
            for pop in region.getPopulations():
                neurons = pop.getNeurons()
                if neurons:
                    region_idx = region.getId().index() % grid_size
                    
                    for neuron in neurons[:10]:
                        if neuron.isFiring():
                            pos_in_region = len(neurons) > 1 ? (neurons.index(neuron) % 10) / 10.0 : 0.5
                            grid_x = region_idx
                            grid_y = int(pos_in_region * (grid_size - 1))
                            
                            if grid_x < grid_size and grid_y < grid_size:
                                activity_grid[grid_x, grid_y] += 1
        
        im = self.ax1.imshow(activity_grid.T, cmap='hot', interpolation='bilinear')
        self.ax1.set_xlim(0, grid_size - 1)
        self.ax1.set_ylim(0, grid_size - 1)
        plt.colorbar(im, ax=self.ax1)
    
    def update_learning_curve(self):
        self.ax2.clear()
        
        if self.learning_curve:
            x = list(range(len(self.learning_curve)))
            y = self.learning_curve
            
            self.ax2.plot(x, y, 'b-', linewidth=2, label='Firing Rate')
            
            if len(self.learning_curve) > 10:
                z = np.polyfit(x[-10:], y[-10:], 1)
                p = np.poly1d(z)
                self.ax2.plot(x[-10:], p(x[-10:]), 'r--', linewidth=2, label='Trend')
            
            if len(self.learning_curve) > 0:
                self.ax2.axvline(x=len(self.learning_curve)//2, color='g', linestyle=':', alpha=0.7, label='Midpoint')
                self.ax2.axvline(x=len(self.learning_curve)//4, color='orange', linestyle=':', alpha=0.7, label='25%')
                self.ax2.axvline(x=3*len(self.learning_curve)//4, color='purple', linestyle=':', alpha=0.7, label='75%')
            
            self.ax2.set_xlabel('Simulation Steps')
            self.ax2.set_ylabel('Average Firing Rate')
            self.ax2.legend()
            self.ax2.grid(True, alpha=0.3)
    
    def update_memory_usage(self):
        self.ax3.clear()
        
        if self.memory_usage:
            x = list(range(len(self.memory_usage)))
            y = self.memory_usage
            
            self.ax3.plot(x, y, 'g-', linewidth=2, label='Active Traces')
            
            max_memory = max(self.memory_usage) if self.memory_usage else 0
            self.ax3.axhline(y=max_memory * 0.8, color='orange', linestyle='--', alpha=0.7, label='80% Capacity')
            self.ax3.axhline(y=max_memory * 0.9, color='red', linestyle='--', alpha=0.7, label='90% Capacity')
            
            self.ax3.set_xlabel('Simulation Steps')
            self.ax3.set_ylabel('Active Memory Traces')
            self.ax3.legend()
            self.ax3.grid(True, alpha=0.3)
    
    def update_development_progress(self):
        self.ax4.clear()
        
        if self.development_progress:
            x = list(range(len(self.development_progress)))
            y = self.development_progress
            
            stage_values = []
            for stage in y:
                if stage == 0: stage_values.append(0)
                elif stage == 1: stage_values.append(1)
                elif stage == 2: stage_values.append(2)
                elif stage == 3: stage_values.append(3)
                elif stage == 4: stage_values.append(4)
                else: stage_values.append(0)
            
            self.ax4.plot(x, stage_values, 'b-', linewidth=2, marker='o', markersize=3, label='Development Stage')
            
            stages = [0, 1, 2, 3, 4]
            stage_names = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
            for stage, name in zip(stages, stage_names):
                self.ax4.axhline(y=stage + 0.5, color='gray', linestyle='-', alpha=0.3)
                self.ax4.text(x[-1] * 1.02, stage + 0.5, name, 
                             verticalalignment='center', fontsize=8)
            
            self.ax4.set_xlabel('Simulation Steps')
            self.ax4.set_ylabel('Development Stage')
            self.ax4.set_yticks(stages)
            self.ax4.set_yticklabels(stage_names)
            self.ax4.legend()
            self.ax4.grid(True, alpha=0.3)

# Use advanced brain visualization
visualizer = AdvancedBrainVisualizer(brain)

for step in range(500):
    visualizer.update_visualization(step)

visualizer.save_visualization()
visualizer.close()
```

## Part 12: Advanced Command Line Interface and Utilities

### 12.1 Advanced Command Line Interface

```bash
# Basic usage
./nlm --help
./nlm_phase6_demo --help

# Configuration options
./nlm --config=config.json --brain.neuron_count=10000 --brain.region_count=5
./nlm --stdp_ltp_weight=0.02 --stdp_ltd_weight=0.018 --plasticity.structural=true

# Output options
./nlm --output=brain_state.nml --log_level=debug
./nlm_phase3_demo --output=experiment_results.json --record_video

# Performance options
./nlm --threads=4 --simd
./nlm_phase4_demo --checkpoint_interval=5000 --memory_pool

# Advanced options
./nlm --debug --profile --validate --benchmark
```

### 12.2 Python Utilities

```python
import pynlm
import time
import json
import os

class NLMUtilities:
    @staticmethod
    def create_batch_simulation(config, num_brains, steps_per_brain):
        brains = []
        for i in range(num_brains):
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.neuron_count", config.get("brain.neuron_count", 1000))
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            brains.append(brain)
        
        return brains
    
    @staticmethod
    def run_parallel_simulation(brains, world, agent, steps):
        import threading
        
        results = []
        threads = []
        
        def run_brain(brain, world, agent, step_results):
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
            step_results.append(brain)
        
        for brain in brains:
            step_results = []
            thread = threading.Thread(target=run_brain, args=(brain, world, agent, step_results))
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        return brains
    
    @staticmethod
    def benchmark_configuration(config_file, iterations=10):
        configurations = [
            {"name": "small", "neuron_count": 1000},
            {"name": "medium", "neuron_count": 5000},
            {"name": "large", "neuron_count": 10000},
            {"name": "very_large", "neuron_count": 50000},
        ]
        
        results = {}
        
        for config_params in configurations:
            times = []
            
            for i in range(iterations):
                config = pynlm.createDefaultConfig()
                for key, value in config_params.items():
                    if key != "name":
                        config.set(key, value)
                
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                start_time = time.time()
                for step in range(100):
                    brain.step(step)
                end_time = time.time()
                
                times.append(end_time - start_time)
            
            avg_time = sum(times) / len(times)
            results[config_params["name"]] = {
                "avg_time": avg_time,
                "min_time": min(times),
                "max_time": max(times),
                "std_time": (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5
            }
        
        return results

# Use NLM utilities
utilities = NLMUtilities()
benchmark_results = utilities.benchmark_configuration({"brain.neuron_count": 5000}, iterations=5)
print(f"Benchmark results: {benchmark_results}")
```

## Part 13: API Reference and Extensions

### 13.1 Core Classes Reference

#### Config Class
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.loadFromArgs(sys.argv)
config.saveToFile("output.json")
config.set("brain.neuron_count", 5000)
config.get("brain.neuron_count")
config.getOr("brain.connection_probability", 0.1)
config.has("brain.v_thresh")
config.getKeys()
config.clear()
config.summary()
```

#### Brain Class
```python
brain = pynlm.createBrain(config)
brain.initialize()

brain.step(step_number)
brain.step(step_number, time)
brain.receiveSensoryInput(input)
brain.produceAction()
brain.reset()
brain.save("brain_state.nml")
brain.load("brain_state.nml")
brain.addRegion("cortex")
brain.getRegion(region_id)
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()
```

### 13.2 Enumerations

#### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

#### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Rest
pynlm.ActionType.Wait
```

## Part 14: Frequently Asked Questions

### Q: What's the difference between Brain and AgentBrain?
A: Brain is the core neural computation engine, while AgentBrain connects the brain to the world by handling sensory processing and motor decoding.

### Q: How do I start from scratch?
A: Use `pynlm.createDefaultConfig()` to create a config, then `pynlm.createBrain(config)` to create a brain, and `brain.initialize()` to start it.

### Q: What's the difference between step() and step(time)?
A: `step(step_number)` uses current simulation time, while `step(step_number, time)` allows you to specify a specific timestamp.

### Q: How do I enable learning?
A: Call `agent.enableRewardModulation(True)`, `agent.enableCuriosity(True)`, and `agent.enableDevelopment(True)` to enable different learning mechanisms.

## Part 15: Conclusion

This Advanced Usage Guide provides comprehensive knowledge for experienced NLM users who want to go beyond basic functionality. The NLM framework offers a powerful platform for creating artificial developmental brains with complex cognition, learning, and behavior.

The key principles of NLM are:

1. **Modularity**: Clear interfaces between all components
2. **Extensibility**: Easy to add new components and functionality
3. **Performance**: Optimized for large-scale neural simulations
4. **Determinism**: Reproducible experiments with explicit random seeds
5. **Biological Inspiration**: Models of brain function and development

Use this guide as a foundation for building complex, sophisticated neural systems that advance our understanding of artificial intelligence and brain-inspired computation.