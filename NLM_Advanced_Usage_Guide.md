# NLM Advanced Usage Guide

This guide provides advanced usage patterns and examples for the NLM (Neural Learning Machine) framework.

## Advanced Brain Configuration

### Custom Brain Architectures

```python
import pynlm

# Create a specialized configuration for specific brain regions
config = pynlm.createDefaultConfig()

# Configure sensory and motor regions separately
config.set("brain.region_count", 4)
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Sensory region parameters
config.set("region.cortex.excitatory_neurons", 300)
config.set("region.cortex.inhibitory_neurons", 80)
config.set("region.cortex.plasticity_rate", 0.03)

# Motor region parameters
config.set("region.motor.excitatory_neurons", 200)
config.set("region.motor.inhibitory_neurons", 50)
config.set("region.motor.plasticity_rate", 0.02)

# Development parameters
config.set("development.enable", True)
config.set("development.critical_period_length", 1000)
config.set("development.plasticity_decay_rate", 0.99)

# Neuromodulation parameters
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.prediction_error.scale", 1.5)

# Memory parameters
config.set("memory.working_memory.capacity", 100)
config.set("memory.episodic_memory.capacity", 50)
config.set("memory.associative_memory.similarity_threshold", 0.7)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Simulation Control

```python
import pynlm
import time
import numpy as np

class AdvancedSimulation:
    def __init__(self):
        self.config = self.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.setupWorld()
        self.setupAgent()
        
    def createAdvancedConfig(self):
        config = pynlm.createDefaultConfig()
        
        # High-performance configuration
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Advanced neuromodulation
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.base_rate", 0.1)
        config.set("neuromod.curiosity.learning_rate", 0.02)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.8)
        config.set("neuromod.novelty.decay_rate", 0.95)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.dopamine.baseline", 0.1)
        config.set("neuromod.dopamine.peak", 2.0)
        config.set("neuromod.prediction_error.scale", 1.2)
        config.set("neuromod.prediction_error.decay_rate", 0.9)
        
        # Advanced development
        config.set("development.enable", True)
        config.set("development.stage_duration", 500)
        config.set("development.maturation_rate", 0.95)
        config.set("development.pruning_strength", 0.5)
        config.set("development.critical_period_length", 2000)
        
        # Memory systems
        config.set("memory.working_memory.capacity", 300)
        config.set("memory.episodic_memory.capacity", 150)
        config.set("memory.episodic_memory.encoding_strength", 0.8)
        
        return config
        
    def setupWorld(self):
        self.world.configure(
            width=50,
            height=50,
            visionWidth=8,
            visionHeight=8
        )
        self.world.reset()
        self.world.setRandomSeed(12345)
        
        # Add complex environment objects
        self.addResources()
        self.addHazards()
        self.addLandmarks()
        
    def addResources(self):
        # Add scattered resources throughout the world
        for x in range(10, 40, 5):
            for y in range(10, 40, 5):
                self.world.addObject(pynlm.WorldObject(
                    x, y, pynlm.WorldObjectType.Resource, value=1.0 + np.random.random()
                ))
                
    def addHazards(self):
        # Add hazards that require careful navigation
        for x in range(20, 30, 7):
            for y in range(20, 30, 7):
                self.world.addObject(pynlm.WorldObject(
                    x, y, pynlm.WorldObjectType.Hazard, value=2.0
                ))
                
    def addLandmarks(self):
        # Add landmarks for spatial navigation
        landmarks = [(15, 15), (35, 15), (15, 35), (35, 35)]
        for x, y in landmarks:
            self.world.addObject(pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Marker, value=3.0
            ))
            
    def setupAgent(self):
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
    def run(self, num_episodes=10, steps_per_episode=1000):
        results = []
        
        for episode in range(num_episodes):
            print(f"Episode {episode + 1}/{num_episodes}")
            
            # Reset environment and agent
            self.world.reset()
            self.agent.reset()
            self.agent.updateDevelopment(0.1)
            
            episode_reward = 0.0
            episode_steps = 0
            curiosity_trajectory = []
            novelty_trajectory = []
            prediction_error_trajectory = []
            
            for step in range(steps_per_episode):
                # Update world
                self.world.update(0.1)
                
                # Get and process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step, self.world.getSimulationTime())
                
                # Decode and apply action
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Calculate reward
                reward = self.calculateReward(percept)
                predicted_reward = self.agent.getPredictionError()
                self.agent.applyRewardModulation(reward, predicted_reward)
                
                # Track metrics
                episode_reward += reward
                curiosity_trajectory.append(self.agent.getCuriosityLevel())
                novelty_trajectory.append(self.agent.getNoveltyLevel())
                prediction_error_trajectory.append(self.agent.getPredictionError())
                
                episode_steps += 1
                
                # Print progress periodically
                if step % 200 == 0:
                    print(f"  Step {step}: Reward={reward:.3f}, "
                          f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
                          f"Novelty={self.agent.getNoveltyLevel():.3f}")
                    
            # Store episode results
            episode_result = {
                'episode': episode + 1,
                'total_reward': episode_reward,
                'steps': episode_steps,
                'avg_firing_rate': self.brain.getAverageFiringRate(),
                'final_curiosity': self.agent.getCuriosityLevel(),
                'final_novelty': self.agent.getNoveltyLevel(),
                'final_prediction_error': self.agent.getPredictionError(),
                'curiosity_trajectory': curiosity_trajectory,
                'novelty_trajectory': novelty_trajectory,
                'prediction_error_trajectory': prediction_error_trajectory,
                'development_stage': self.brain.getDevelopmentalStage(),
                'plasticity_factor': self.agent.getNeuromodulationLevel()
            }
            
            results.append(episode_result)
            print(f"Episode {episode + 1} completed: "
                  f"Total Reward={episode_reward:.2f}, "
                  f"Final Curiosity={self.agent.getCuriosityLevel():.3f}")
                    
        return self.analyzeResults(results)
        
    def calculateReward(self, percept):
        # Advanced reward calculation
        reward = 0.0
        
        # Energy reward (based on internal signals)
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            energy = internal[0]  # Assuming first signal is energy
            reward += energy * 0.1
            
        # Novelty reward (exploration bonus)
        reward += self.agent.getNoveltyLevel() * 0.5
        
        # Curiosity reward (exploration motivation)
        reward += self.agent.getCuriosityLevel() * 0.3
        
        return reward
        
    def analyzeResults(self, results):
        print("\n" + "="*60)
        print("SIMULATION RESULTS ANALYSIS")
        print("="*60)
        
        total_reward = sum(r['total_reward'] for r in results)
        avg_reward = total_reward / len(results)
        
        print(f"Total Episodes: {len(results)}")
        print(f"Average Reward per Episode: {avg_reward:.3f}")
        print(f"Total Reward: {total_reward:.3f}")
        
        # Development analysis
        final_stage = results[-1]['development_stage']
        print(f"Final Developmental Stage: {final_stage}")
        
        # Neuromodulation analysis
        avg_curiosity = sum(r['final_curiosity'] for r in results) / len(results)
        avg_novelty = sum(r['final_novelty'] for r in results) / len(results)
        avg_prediction_error = sum(r['final_prediction_error'] for r in results) / len(results)
        
        print(f"Average Curiosity Level: {avg_curiosity:.3f}")
        print(f"Average Novelty Level: {avg_novelty:.3f}")
        print(f"Average Prediction Error: {avg_prediction_error:.3f}")
        
        # Learning progress analysis
        curiosity_trend = []
        for r in results:
            curiosity_trend.append(r['final_curiosity'])
            
        if len(curiosity_trend) > 1:
            curiosity_change = curiosity_trend[-1] - curiosity_trend[0]
            print(f"Curiosity Trend: {curiosity_change:+.3f} ({'increasing' if curiosity_change > 0 else 'decreasing'})")
            
        return {
            'summary': {
                'total_episodes': len(results),
                'average_reward': avg_reward,
                'total_reward': total_reward,
                'final_development_stage': final_stage,
                'average_curiosity': avg_curiosity,
                'average_novelty': avg_novelty,
                'average_prediction_error': avg_prediction_error
            },
            'detailed_results': results,
            'learning_metrics': {
                'curiosity_trend': curiosity_trend,
                'novelty_trend': [r['final_novelty'] for r in results],
                'prediction_error_trend': [r['final_prediction_error'] for r in results]
            }
        }

if __name__ == "__main__":
    # Run advanced simulation
    simulation = AdvancedSimulation()
    results = simulation.run(num_episodes=5, steps_per_episode=500)
    
    # Save results
    import json
    with open("simulation_results.json", "w") as f:
        json.dump(results, f, indent=2)
        
    print("\nAdvanced simulation completed successfully!")
    print("Results saved to simulation_results.json")
```

## Performance Optimization Techniques

### Brain Performance Tuning

```python
import pynlm

class OptimizedBrain:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1000)  # Start small
        self.config.set("plasticity.structural.enable", True)
        self.config.set("development.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
    def warmUp(self, warmup_steps=100):
        """Warm up the brain to stabilize dynamics"""
        print(f"Warming up brain for {warmup_steps} steps...")
        for step in range(warmup_steps):
            # Inject some baseline current to help neurons stabilize
            self.brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 0.1)
            self.brain.step(step)
            
        print(f"Warm-up complete. Firing rate: {self.brain.getAverageFiringRate():.3f}")
        
    def optimizedStep(self, step_num, use_temporal=True):
        """Optimized simulation step with adaptive timing"""
        
        # Adaptive timing based on current brain state
        if use_temporal:
            # Time based on brain activity
            current_time = self.brain.getTotalSpikeCount() * 0.1
            self.brain.step(step_num, current_time)
        else:
            # Simple step
            self.brain.step(step_num)
            
        return self.getPerformanceStats(step_num)
        
    def getPerformanceStats(self, step_num):
        """Get performance statistics for monitoring"""
        stats = {
            'step': step_num,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'plasticity_factor': self.brain.getNeuromodulationLevel(),
            'development_stage': self.brain.getDevelopmentalStage()
        }
        
        return stats

# Usage example
def demonstrateOptimization():
    print("=== Brain Performance Optimization Demo ===\n")
    
    # Create optimized brain
    optimized_brain = OptimizedBrain()
    
    # Warm up
    optimized_brain.warmUp(200)
    
    # Run optimized simulation
    performance_history = []
    
    for step in range(500):
        # Use optimized stepping
        stats = optimized_brain.optimizedStep(step, use_temporal=True)
        performance_history.append(stats)
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing rate: {stats['average_firing_rate']:.3f}")
            print(f"  Plasticity factor: {stats['plasticity_factor']:.3f}")
            print(f"  Development stage: {stats['development_stage']}")
    
    # Analyze performance
    print("\n=== Performance Analysis ===")
    avg_firing_rate = sum(s['average_firing_rate'] for s in performance_history) / len(performance_history)
    print(f"Average firing rate: {avg_firing_rate:.3f}")
    
    # Check for performance issues
    if avg_firing_rate > 5.0:
        print("WARNING: High firing rate detected. Consider reducing brain excitability.")
    elif avg_firing_rate < 0.1:
        print("WARNING: Low firing rate detected. Consider increasing input current.")
        
    return performance_history

if __name__ == "__main__":
    history = demonstrateOptimization()
```

## Advanced Memory Management

### Working with Brain States

```python
import pynlm
import os
import json
import time

class BrainManager:
    def __init__(self):
        self.checkpoint_dir = "brain_checkpoints"
        self.metadata_file = "brain_metadata.json"
        self.ensureDirectory(self.checkpoint_dir)
        self.loadMetadata()
        
    def ensureDirectory(self, path):
        if not os.path.exists(path):
            os.makedirs(path)
            
    def loadMetadata(self):
        if os.path.exists(self.metadata_file):
            with open(self.metadata_file, 'r') as f:
                self.metadata = json.load(f)
        else:
            self.metadata = {}
            
    def saveMetadata(self):
        with open(self.metadata_file, 'w') as f:
            json.dump(self.metadata, f, indent=2)
            
    def createBrainFromConfigFile(self, config_file):
        """Create a brain from a configuration file"""
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
        return config
        
    def saveBrainWithMetadata(self, brain, episode, reward, additional_info=None):
        """Save brain state with metadata"""
        
        # Create checkpoint filename
        checkpoint_id = f"brain_checkpoint_ep{episode:04d}_r{reward:06.2f}"
        checkpoint_path = os.path.join(self.checkpoint_dir, checkpoint_id + ".bin")
        
        # Save brain state
        brain.save(checkpoint_path)
        
        # Create metadata entry
        metadata_entry = {
            'checkpoint_id': checkpoint_id,
            'episode': episode,
            'reward': reward,
            'timestamp': time.time(),
            'total_spikes': brain.getTotalSpikeCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage().value
        }
        
        if additional_info:
            metadata_entry.update(additional_info)
            
        self.metadata[checkpoint_id] = metadata_entry
        self.saveMetadata()
        
        return checkpoint_path
        
    def loadBestBrain(self, min_episodes=100):
        """Load the brain with the best performance"""
        
        if not self.metadata:
            return None
            
        # Find best performing checkpoint
        best_checkpoint = None
        best_reward = float('-inf')
        
        for checkpoint_id, metadata in self.metadata.items():
            if metadata['reward'] > best_reward and metadata['episode'] >= min_episodes:
                best_reward = metadata['reward']
                best_checkpoint = checkpoint_id
                
        if best_checkpoint:
            checkpoint_path = os.path.join(self.checkpoint_dir, best_checkpoint + ".bin")
            
            # Create new brain and load state
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.load(checkpoint_path)
            
            print(f"Loaded best brain from episode {metadata['episode']}")
            print(f"Reward: {metadata['reward']:.2f}, "
                  f"Firing rate: {metadata['firing_rate']:.3f}")
                  
            return brain
            
        return None
        
    def listCheckpoints(self, sort_by='reward', limit=10):
        """List available checkpoints"""
        
        if not self.metadata:
            print("No checkpoints available.")
            return
            
        # Sort checkpoints
        items = list(self.metadata.items())
        
        if sort_by == 'reward':
            items.sort(key=lambda x: x[1]['reward'], reverse=True)
        elif sort_by == 'episode':
            items.sort(key=lambda x: x[1]['episode'])
        elif sort_by == 'timestamp':
            items.sort(key=lambda x: x[1]['timestamp'], reverse=True)
            
        print(f"\nTop {min(limit, len(items))} checkpoints:")
        print("=" * 80)
        
        for i, (checkpoint_id, metadata) in enumerate(items[:limit]):
            print(f"{i+1:2d}. {checkpoint_id}")
            print(f"    Episode: {metadata['episode']}, Reward: {metadata['reward']:8.2f}, "
                  f"Firing Rate: {metadata['firing_rate']:6.3f}, "
                  f"Stage: {metadata['development_stage']}")
            print()

# Usage example
def demonstrateBrainManagement():
    print("=== Advanced Brain Management Demo ===\n")
    
    # Create brain manager
    manager = BrainManager()
    
    # Create and train a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Train for several episodes
    best_reward = 0.0
    
    for episode in range(1, 51):
        # Reset and run episode
        world.reset()
        agent.reset()
        
        episode_reward = 0.0
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = 0.1  # Simple reward for demonstration
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
            
            episode_reward += reward
            
        # Save checkpoint if performance is good
        if episode_reward > best_reward:
            best_reward = episode_reward
            manager.saveBrainWithMetadata(
                brain, 
                episode, 
                episode_reward,
                {'firing_rate': brain.getAverageFiringRate(),
                 'curiosity': agent.getCuriosityLevel()}
            )
        
        if episode % 10 == 0:
            print(f"Episode {episode}: Reward = {episode_reward:.2f}")
            
    # List checkpoints
    manager.listCheckpoints(sort_by='reward', limit=5)
    
    # Load best brain
    best_brain = manager.loadBestBrain(min_episodes=50)
    
    if best_brain:
        print("\nSuccessfully loaded best performing brain!")
        print(f"Best brain achieved: {best_reward:.2f} reward")
        print(f"Best brain firing rate: {best_brain.getAverageFiringRate():.3f}")
        
        # Test the loaded brain
        test_brain = best_brain
        test_brain.initialize()
        
        # Run a quick test
        for step in range(50):
            test_brain.step(step)
            
        print(f"Test brain firing rate after 50 steps: {test_brain.getAverageFiringRate():.3f}")
    
    return manager

if __name__ == "__main__":
    manager = demonstrateBrainManagement()
```

## Performance Monitoring Tools

### Real-time Monitoring

```python
import pynlm
import time
import threading
from typing import Dict, List, Callable, Any

class PerformanceMonitor:
    def __init__(self, brain: pynlm.Brain, interval: float = 0.1):
        self.brain = brain
        self.interval = interval
        self.monitor_thread = None
        self.stop_event = threading.Event()
        self.metrics = []
        self.callbacks = []
        
    def start(self):
        """Start performance monitoring"""
        self.stop_event.clear()
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print("Performance monitoring started")
        
    def stop(self):
        """Stop performance monitoring"""
        self.stop_event.set()
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
        print("Performance monitoring stopped")
        
    def _monitor_loop(self):
        """Main monitoring loop"""
        while not self.stop_event.is_set():
            try:
                metrics = self._collect_metrics()
                self.metrics.append(metrics)
                
                # Trigger callbacks
                for callback in self.callbacks:
                    callback(metrics)
                    
                time.sleep(self.interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self.interval)
                
    def _collect_metrics(self) -> Dict[str, Any]:
        """Collect current performance metrics"""
        timestamp = time.time()
        
        metrics = {
            'timestamp': timestamp,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_synapse_count': self.brain.getTotalSynapseCount(),
            'developmental_stage': self.brain.getDevelopmentalStage().value,
            'plasticity_factor': self.brain.getNeuromodulationLevel()
        }
        
        return metrics
        
    def add_callback(self, callback: Callable[[Dict[str, Any]], None]):
        """Add a callback function for metrics"""
        self.callbacks.append(callback)
        
    def getMetrics(self) -> List[Dict[str, Any]]:
        """Get collected metrics"""
        return self.metrics.copy()
        
    def clearMetrics(self):
        """Clear collected metrics"""
        self.metrics.clear()
        
    def getStatistics(self) -> Dict[str, Any]:
        """Get summary statistics of collected metrics"""
        if not self.metrics:
            return {}
            
        total_spikes = [m['total_spikes'] for m in self.metrics]
        firing_rates = [m['average_firing_rate'] for m in self.metrics]
        
        return {
            'duration_seconds': self.metrics[-1]['timestamp'] - self.metrics[0]['timestamp'],
            'total_spikes': total_spikes[-1],
            'spike_rate_hz': (total_spikes[-1] - total_spikes[0]) / (self.metrics[-1]['timestamp'] - self.metrics[0]['timestamp']),
            'average_firing_rate': sum(firing_rates) / len(firing_rates),
            'max_firing_rate': max(firing_rates),
            'min_firing_rate': min(firing_rates),
            'metric_points': len(self.metrics)
        }

# Example usage
class RealTimeDashboard:
    def __init__(self, brain: pynlm.Brain):
        self.monitor = PerformanceMonitor(brain, interval=0.5)
        self.setupCallbacks()
        
    def setupCallbacks(self):
        """Setup dashboard update callbacks"""
        
        def updateMetrics(metrics):
            # Update dashboard with new metrics
            print(f"[{metrics['timestamp']:.2f}] "
                  f"Firing Rate: {metrics['average_firing_rate']:.3f}, "
                  f"Neurons: {metrics['firing_neuron_count']}, "
                  f"Spikes: {metrics['total_spikes']}, "
                  f"Stage: {metrics['developmental_stage']}")
                  
        self.monitor.add_callback(updateMetrics)
        
    def startMonitoring(self):
        """Start real-time monitoring"""
        self.monitor.start()
        
    def stopMonitoring(self):
        """Stop real-time monitoring"""
        self.monitor.stop()
        
    def getCurrentStats(self) -> Dict[str, Any]:
        """Get current statistics"""
        return self.monitor.getStatistics()

# Usage example
class RealTimeDemo:
    def __init__(self):
        print("=== Real-time Performance Monitoring Demo ===\n")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create dashboard
        self.dashboard = RealTimeDashboard(brain)
        
        print("Starting real-time monitoring...")
        print("Press Ctrl+C to stop")
        print("-" * 50)
        
    def start(self):
        """Start the demo"""
        self.dashboard.startMonitoring()
        
        # Run simulation while monitoring
        for step in range(100):
            self.dashboard.brain.step(step)
            time.sleep(0.1)
            
            if step % 10 == 0:
                stats = self.dashboard.getCurrentStats()
                if stats:
                    print(f"Step {step}: "
                          f"Firing Rate: {stats['average_firing_rate']:.3f}, "
                          f"Spike Rate: {stats['spike_rate_hz']:.1f} Hz")
                          
    def stop(self):
        """Stop the demo"""
        self.dashboard.stopMonitoring()

# Usage
def runRealTimeDemo():
    demo = RealTimeDemo()
    
    try:
        demo.start()
    except KeyboardInterrupt:
        print("\nStopping monitoring...")
        demo.stop()
        
    print("\nReal-time monitoring completed!")

if __name__ == "__main__":
    runRealTimeDemo()
```

## Error Handling and Debugging

### Comprehensive Error Management

```python
import pynlm
import traceback
import time
import json
from typing import Dict, Any, Optional, Callable

class NLMError(Exception):
    """Base exception for NLM errors"""
    def __init__(self, message: str, error_code: str = "UNKNOWN", details: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        
    def __str__(self):
        return f"NLM Error [{self.error_code}]: {self.message}"

class ErrorHandler:
    def __init__(self):
        self.error_handlers = {}
        self.error_log = []
        
    def addHandler(self, error_code: str, handler: Callable[[NLMError], Any]):
        """Add an error handler for a specific error code"""
        self.error_handlers[error_code] = handler
        
    def handleError(self, error: Exception) -> Any:
        """Handle an error using registered handlers"""
        
        # Convert to NLMError if not already
        if not isinstance(error, NLMError):
            nlm_error = NLMError(
                str(error),
                error_code="RUNTIME_ERROR",
                details={"stack_trace": traceback.format_exc()}
            )
        else:
            nlm_error = error
            
        # Log the error
        self.error_log.append({
            'timestamp': time.time(),
            'error': nlm_error,
            'stack_trace': traceback.format_exc() if not isinstance(error, NLMError) else None
        })
        
        # Try registered handlers
        if nlm_error.error_code in self.error_handlers:
            try:
                return self.error_handlers[nlm_error.error_code](nlm_error)
            except Exception as handler_error:
                print(f"Error in handler for {nlm_error.error_code}: {handler_error}")
                
        # Default error handling
        print(f"Unhandled error: {nlm_error}")
        return None
        
    def getErrorLog(self) -> List[Dict[str, Any]]:
        """Get error log"""
        return self.error_log.copy()
        
    def clearErrorLog(self):
        """Clear error log"""
        self.error_log.clear()

class DebugMode:
    def __init__(self):
        self.debug_enabled = False
        self.breakpoints = {}
        self.watch_variables = {}
        self.debug_log = []
        
    def enableDebugMode(self):
        """Enable debug mode"""
        self.debug_enabled = True
        print("Debug mode enabled")
        
    def disableDebugMode(self):
        """Disable debug mode"""
        self.debug_enabled = False
        print("Debug mode disabled")
        
    def setBreakpoint(self, step: int, condition: Optional[str] = None):
        """Set a breakpoint at a specific step"""
        self.breakpoints[step] = condition
        print(f"Breakpoint set at step {step} (condition: {condition})")
        
    def addWatchVariable(self, name: str, getter: Callable[[], Any]):
        """Add a watch variable to monitor"""
        self.watch_variables[name] = getter
        print(f"Watch variable '{name}' added")
        
    def logDebug(self, message: str, level: str = "INFO"):
        """Log debug message"""
        debug_entry = {
            'timestamp': time.time(),
            'level': level,
            'message': message,
            'watch_variables': {name: getter() for name, getter in self.watch_variables.items()}
        }
        
        self.debug_log.append(debug_entry)
        
        if self.debug_enabled:
            print(f"[{level}] {message}")
            
    def getDebugLog(self, level: Optional[str] = None) -> List[Dict[str, Any]]:
        """Get debug log, optionally filtered by level"""
        if level:
            return [entry for entry in self.debug_log if entry['level'] == level]
        return self.debug_log.copy()
        
    def saveDebugLog(self, filepath: str):
        """Save debug log to file"""
        with open(filepath, 'w') as f:
            json.dump(self.debug_log, f, indent=2, default=str)
            
    def loadDebugLog(self, filepath: str):
        """Load debug log from file"""
        if os.path.exists(filepath):
            with open(filepath, 'r') as f:
                self.debug_log = json.load(f)
        else:
            print(f"Debug log file not found: {filepath}")

class SafeBrain:
    def __init__(self, config):
        self.config = config
        self.error_handler = ErrorHandler()
        self.debug_mode = DebugMode()
        self.setupErrorHandlers()
        
        try:
            self.brain = pynlm.createBrain(config)
            self.brain.initialize()
            self.logOperation("Brain created successfully")
        except Exception as e:
            self.error_handler.handleError(e)
            raise NLMError("Failed to create brain", "BRAIN_CREATION_ERROR", {"config": str(config)})
            
    def setupErrorHandlers(self):
        """Setup error handlers for common errors"""
        
        def handleConfigError(error: NLMError):
            print(f"Configuration error: {error.message}")
            # Attempt to create a default configuration
            default_config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(default_config)
            self.brain.initialize()
            return True
            
        def handleBrainError(error: NLMError):
            print(f"Brain operation error: {error.message}")
            # Attempt to reset and reinitialize
            if self.brain:
                try:
                    self.brain.reset()
                    self.brain.initialize()
                    return True
                except Exception as reset_error:
                    print(f"Failed to reset brain: {reset_error}")
            return False
            
        self.error_handler.addHandler("CONFIGURATION_ERROR", handleConfigError)
        self.error_handler.addHandler("BRAIN_OPERATION_ERROR", handleBrainError)
        
    def safeStep(self, step: int, time: Optional[float] = None):
        """Safely perform a brain step"""
        try:
            if time is not None:
                self.brain.step(step, time)
            else:
                self.brain.step(step)
            return True
        except Exception as e:
            self.error_handler.handleError(
                NLMError(f"Brain step failed at step {step}", "BRAIN_STEP_ERROR", {"step": step, "time": time})
            )
            return False
            
    def safeInjectCurrent(self, neuron_id, current):
        """Safely inject current into a neuron"""
        try:
            self.brain.injectCurrent(neuron_id, current)
            return True
        except Exception as e:
            self.error_handler.handleError(
                NLMError(f"Current injection failed", "CURRENT_INJECTION_ERROR", 
                        {"neuron_id": neuron_id, "current": current})
            )
            return False
            
    def logOperation(self, message: str):
        """Log an operation"""
        self.debug_mode.logDebug(f"Operation: {message}", "INFO")
        
    def getBrain(self):
        """Get the brain object"""
        return self.brain
        
    def getErrorHandler(self):
        """Get the error handler"""
        return self.error_handler
        
    def getDebugMode(self):
        """Get the debug mode"""
        return self.debug_mode

# Usage example
class DebugBrainSimulation:
    def __init__(self):
        print("=== Debug Mode Brain Simulation ===\n")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        
        # Create safe brain
        self.safe_brain = SafeBrain(config)
        self.brain = self.safe_brain.getBrain()
        
        # Setup debug mode
        debug_mode = self.safe_brain.getDebugMode()
        debug_mode.enableDebugMode()
        
        # Setup watch variables
        self.setupWatchVariables(debug_mode)
        
        # Setup breakpoints
        self.setupBreakpoints(debug_mode)
        
        # Create agent and world for simulation
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable features
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        print("Debug simulation initialized")
        print("Press Ctrl+C to stop and save debug logs")
        
    def setupWatchVariables(self, debug_mode):
        """Setup watch variables for debugging"""
        
        def getFiringRate():
            return self.brain.getAverageFiringRate()
            
        def getTotalSpikes():
            return self.brain.getTotalSpikeCount()
            
        def getActiveNeurons():
            return self.brain.getFiringNeuronCount()
            
        def getCuriosity():
            return self.agent.getCuriosityLevel()
            
        def getDevelopmentStage():
            return self.brain.getDevelopmentalStage().value
            
        debug_mode.addWatchVariable("AverageFiringRate", getFiringRate)
        debug_mode.addWatchVariable("TotalSpikes", getTotalSpikes)
        debug_mode.addWatchVariable("ActiveNeurons", getActiveNeurons)
        debug_mode.addWatchVariable("Curiosity", getCuriosity)
        debug_mode.addWatchVariable("DevelopmentStage", getDevelopmentStage)
        
    def setupBreakpoints(self, debug_mode):
        """Setup breakpoints for debugging"""
        
        # Breakpoint at specific steps
        debug_mode.setBreakpoint(50, "high_curiosity")
        debug_mode.setBreakpoint(100, "low_firing_rate")
        debug_mode.setBreakpoint(200, "development_change")
        
    def runSimulation(self, num_steps: int = 300):
        """Run simulation with debugging"""
        
        try:
            for step in range(num_steps):
                # Check breakpoints
                if step in self.safe_brain.getDebugMode().breakpoints:
                    condition = self.safe_brain.getDebugMode().breakpoints[step]
                    
                    # Evaluate condition
                    if condition == "high_curiosity":
                        if self.agent.getCuriosityLevel() > 0.7:
                            print(f"\nBREAKPOINT AT STEP {step}: High curiosity ({self.agent.getCuriosityLevel():.3f})")
                            debug_mode = self.safe_brain.getDebugMode()
                            debug_mode.logDebug(f"High curiosity detected: {self.agent.getCuriosityLevel():.3f}", "WARNING")
                    
                    elif condition == "low_firing_rate":
                        if self.brain.getAverageFiringRate() < 0.5:
                            print(f"\nBREAKPOINT AT STEP {step}: Low firing rate ({self.brain.getAverageFiringRate():.3f})")
                            debug_mode = self.safe_brain.getDebugMode()
                            debug_mode.logDebug(f"Low firing rate detected: {self.brain.getAverageFiringRate():.3f}", "WARNING")
                    
                    elif condition == "development_change":
                        if self.brain.getDevelopmentalStage().value > 2:
                            print(f"\nBREAKPOINT AT STEP {step}: Development stage change")
                            debug_mode = self.safe_brain.getDebugMode()
                            debug_mode.logDebug(f"Development stage changed to {self.brain.getDevelopmentalStage().value}", "INFO")
                
                # Safe simulation step
                if not self.safe_brain.safeStep(step):
                    print(f"Simulation failed at step {step}")
                    break
                    
                # Run agent step
                self.world.update(0.1)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Log every 10 steps
                if step % 10 == 0:
                    self.safe_brain.logOperation(f"Step {step} completed")
                    
                # Small delay for real-time monitoring
                time.sleep(0.01)
                
        except KeyboardInterrupt:
            print("\nSimulation interrupted by user")
        except Exception as e:
            self.safe_brain.getErrorHandler().handleError(e)
            
        # Save debug logs
        debug_mode = self.safe_brain.getDebugMode()
        debug_mode.saveDebugLog("brain_simulation_debug.json")
        print(f"Debug logs saved to brain_simulation_debug.json")
        
    def getSimulationReport(self) -> Dict[str, Any]:
        """Get simulation report"""
        
        debug_mode = self.safe_brain.getDebugMode()
        error_handler = self.safe_brain.getErrorHandler()
        
        report = {
            'simulation_summary': {
                'total_steps': len([d for d in debug_mode.getDebugLog() if 'Operation' in d['message']]),
                'total_errors': len(error_handler.getErrorLog()),
                'debug_entries': len(debug_mode.getDebugLog())
            },
            'error_log': error_handler.getErrorLog(),
            'debug_log': debug_mode.getDebugLog(),
            'final_state': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage().value,
                'curiosity': self.agent.getCuriosityLevel()
            }
        }
        
        return report

if __name__ == "__main__":
    # Run debug simulation
    simulation = DebugBrainSimulation()
    
    print("Starting debug simulation...")
    simulation.runSimulation(100)
    
    # Get simulation report
    report = simulation.getSimulationReport()
    
    print("\n=== SIMULATION REPORT ===")
    print(f"Total steps: {report['simulation_summary']['total_steps']}")
    print(f"Total errors: {report['simulation_summary']['total_errors']}")
    print(f"Debug entries: {report['simulation_summary']['debug_entries']}")
    
    if report['simulation_summary']['total_errors'] > 0:
        print(f"\nErrors encountered:")
        for error_info in report['error_log']:
            print(f"  - {error_info['error']}")
    
    print(f"\nFinal state:")
    print(f"  Total spikes: {report['final_state']['total_spikes']}")
    print(f"  Firing rate: {report['final_state']['firing_rate']:.3f}")
    print(f"  Development stage: {report['final_state']['development_stage']}")
    print(f"  Curiosity level: {report['final_state']['curiosity']:.3f}")
```

## Advanced Configuration Examples

### High-Performance Configuration

```python
import pynlm

def createHighPerformanceConfig():
    """Create configuration optimized for performance"""
    
    config = pynlm.createDefaultConfig()
    
    # Optimize for performance
    config.set("brain.neuron_count", 2000)  # Larger network
    config.set("brain.synapse_density", 0.2)  # More connections
    
    # Enable advanced features
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.02)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation for adaptive behavior
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.base_rate", 0.2)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.8)
    
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.dopamine.baseline", 0.1)
    
    config.set("neuromod.prediction_error.scale", 1.2)
    
    # Development for long-term learning
    config.set("development.enable", True)
    config.set("development.stage_duration", 1000)
    config.set("development.maturation_rate", 0.95)
    
    # Memory systems
    config.set("memory.working_memory.capacity", 300)
    config.set("memory.episodic_memory.capacity", 150)
    
    # Simulation parameters
    config.set("simulation.steps_per_episode", 2000)
    config.set("simulation.episodes", 10)
    
    return config

# Usage
config = createHighPerformanceConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Minimal Configuration

```python
import pynlm

def createMinimalConfig():
    """Create minimal configuration for basic simulation"""
    
    config = pynlm.createDefaultConfig()
    
    # Basic brain
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.05)
    
    # No plasticity for performance
    config.set("plasticity.stdp.enable", False)
    config.set("plasticity.hebbian.enable", False)
    config.set("plasticity.structural.enable", False)
    
    # No neuromodulation
    config.set("neuromod.curiosity.enable", False)
    config.set("neuromod.novelty.enable", False)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.prediction_error.scale", 1.0)
    
    # No development
    config.set("development.enable", False)
    
    # Small memory systems
    config.set("memory.working_memory.capacity", 10)
    config.set("memory.episodic_memory.capacity", 5)
    
    # Short simulation
    config.set("simulation.steps_per_episode", 50)
    config.set("simulation.episodes", 1)
    
    return config

# Usage
config = createMinimalConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Research Configuration

```python
import pynlm

def createResearchConfig():
    """Create configuration for research experiments"""
    
    config = pynlm.createDefaultConfig()
    
    # Large-scale brain
    config.set("brain.neuron_count", 10000)
    config.set("brain.synapse_density", 0.1)
    
    # Advanced plasticity rules
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.formation_rate", 0.001)
    config.set("plasticity.structural.pruning_threshold", 0.1)
    
    # Sophisticated neuromodulation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.base_rate", 0.05)
    config.set("neuromod.curiosity.learning_rate", 0.01)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.9)
    config.set("neuromod.novelty.decay_rate", 0.99)
    
    # Complex development
    config.set("development.enable", True)
    config.set("development.stage_duration", 2000)
    config.set("development.maturation_rate", 0.98)
    config.set("development.pruning_strength", 0.8)
    config.set("development.critical_period_length", 5000)
    
    # Large memory systems
    config.set("memory.working_memory.capacity", 1000)
    config.set("memory.episodic_memory.capacity", 500)
    config.set("memory.episodic_memory.encoding_strength", 0.95)
    
    # Extensive simulation
    config.set("simulation.steps_per_episode", 10000)
    config.set("simulation.episodes", 100)
    
    return config

# Usage
config = createResearchConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

## Example Scripts

### Complete Brain Training Script

```python
#!/usr/bin/env python3
"""
Complete brain training script with monitoring and checkpointing.
"""

import pynlm
import time
import json
import os
from pathlib import Path

def createTrainingConfig():
    """Create training configuration"""
    config = pynlm.createDefaultConfig()
    
    # Training configuration
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.15)
    
    # Enable all features for training
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.002)
    config.set("plasticity.structural.enable", True)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.base_rate", 0.1)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.7)
    config.set("neuromod.dopamine.scale", 1.2)
    
    config.set("development.enable", True)
    config.set("development.stage_duration", 800)
    
    config.set("memory.working_memory.capacity", 200)
    config.set("memory.episodic_memory.capacity", 100)
    
    return config

def createWorld(width=30, height=30):
    """Create training world"""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 8, 8)
    
    # Add resources for learning
    for x in range(5, width-5, 6):
        for y in range(5, height-5, 6):
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    # Add hazards for challenge
    for x in range(width//3, width//3 + 15, 4):
        for y in range(height//3, height//3 + 15, 4):
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=2.0))
    
    return world

def runTrainingSession(brain, agent, world, episode_id):
    """Run a single training session"""
    
    world.reset()
    agent.reset()
    agent.updateDevelopment(0.1)
    
    episode_reward = 0.0
    steps_completed = 0
    max_curiosity = 0.0
    
    for step in range(500):  # 500 steps per episode
        # World update
        world.update(0.1)
        
        # Sensorimotor loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = 0.0
        if percept.getInternal() and len(percept.getInternal()) > 0:
            energy = percept.getInternal()[0]
            reward += energy * 0.1
            
        reward += agent.getNoveltyLevel() * 0.3  # Exploration bonus
        
        # Neuromodulation
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        episode_reward += reward
        max_curiosity = max(max_curiosity, agent.getCuriosityLevel())
        steps_completed += 1
        
    return {
        'episode': episode_id,
        'total_reward': episode_reward,
        'max_curiosity': max_curiosity,
        'steps_completed': steps_completed,
        'avg_firing_rate': brain.getAverageFiringRate(),
        'final_development_stage': brain.getDevelopmentalStage().value,
        'final_plasticity_factor': agent.getNeuromodulationLevel()
    }

def main():
    print("=== NLM Brain Training Session ===\n")
    
    # Setup output directory
    output_dir = Path("training_results")
    output_dir.mkdir(exist_ok=True)
    
    # Create configuration
    config = createTrainingConfig()
    print(f"Created training configuration with {config.get('brain.neuron_count', 'default')} neurons")
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Setup world
    world = createWorld()
    agent.initialize(world)
    
    # Enable training features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Started training session")
    print("=" * 50)
    
    # Training loop
    training_results = []
    start_time = time.time()
    
    for episode in range(1, 21):  # 20 episodes
        episode_start = time.time()
        
        print(f"Episode {episode:2d}/20 ", end="")
        
        result = runTrainingSession(brain, agent, world, episode)
        
        episode_time = time.time() - episode_start
        
        print(f"completed in {episode_time:.1f}s: "
              f"Reward={result['total_reward']:6.2f}, "
              f"Curiosity={result['max_curiosity']:5.3f}, "
              f"Firing={result['avg_firing_rate']:5.3f}")
        
        training_results.append(result)
        
        # Save checkpoint every 5 episodes
        if episode % 5 == 0:
            checkpoint_file = output_dir / f"checkpoint_ep{episode:03d}.bin"
            brain.save(str(checkpoint_file))
            print(f"  Checkpoint saved: {checkpoint_file.name}")
            
            # Save session results
            session_file = output_dir / f"session_{episode:03d}_results.json"
            with open(session_file, 'w') as f:
                json.dump({
                    'episode': episode,
                    'result': result,
                    'timestamp': time.time(),
                    'config_summary': {
                        'neuron_count': config.get('brain.neuron_count', 0),
                        'synapse_density': config.get('brain.synapse_density', 0)
                    }
                }, f, indent=2)
    
    total_time = time.time() - start_time
    
    # Save final results
    final_results = {
        'total_episodes': len(training_results),
        'total_training_time': total_time,
        'average_reward_per_episode': sum(r['total_reward'] for r in training_results) / len(training_results),
        'max_curiosity_achieved': max(r['max_curiosity'] for r in training_results),
        'final_development_stage': training_results[-1]['final_development_stage'],
        'final_plasticity_factor': training_results[-1]['final_plasticity_factor'],
        'episode_results': training_results
    }
    
    with open(output_dir / "final_training_results.json", 'w') as f:
        json.dump(final_results, f, indent=2)
    
    # Create summary
    print("\n" + "=" * 50)
    print("TRAINING SUMMARY")
    print("=" * 50)
    print(f"Total Episodes: {final_results['total_episodes']}")
    print(f"Total Training Time: {final_results['total_training_time']:.1f}s")
    print(f"Average Reward per Episode: {final_results['average_reward_per_episode']:.3f}")
    print(f"Max Curiosity Achieved: {final_results['max_curiosity_achieved']:.3f}")
    print(f"Final Development Stage: {final_results['final_development_stage']}")
    print(f"Final Plasticity Factor: {final_results['final_plasticity_factor']:.3f}")
    
    print(f"\nResults saved to: {output_dir}/")
    print(f"  - final_training_results.json")
    print(f"  - checkpoint_*.bin (checkpoints)")
    print(f"  - session_*.json (session results)")
    
    # Create performance plot data
    plot_data = {
        'episodes': [r['episode'] for r in training_results],
        'rewards': [r['total_reward'] for r in training_results],
        'curiosity': [r['max_curiosity'] for r in training_results],
        'firing_rates': [r['avg_firing_rate'] for r in training_results]
    }
    
    with open(output_dir / "plot_data.json", 'w') as f:
        json.dump(plot_data, f, indent=2)
    
    print(f"\nPlot data saved to: plot_data.json")
    
    return final_results

if __name__ == "__main__":
    try:
        results = main()
        print("\nTraining completed successfully!")
    except Exception as e:
        print(f"\nTraining failed with error: {e}")
        import traceback
        traceback.print_exc()
```

## Conclusion

This advanced usage guide provides comprehensive examples and techniques for working with the NLM framework:

1. **Advanced Brain Configuration** - Custom brain architectures and parameters
2. **Performance Optimization** - Techniques for efficient simulation
3. **Memory Management** - Checkpointing and state management
4. **Batch Processing** - Automated simulation runs
5. **Performance Monitoring** - Real-time metrics and statistics
6. **Error Handling** - Robust error management and debugging
7. **Configuration Examples** - Various configuration presets
8. **Example Scripts** - Complete working examples

These examples demonstrate how to leverage the full power of NLM for advanced neural simulation research and applications.

---

*This guide is part of the NLM framework documentation. For more information, see:
- `README.md` - Basic usage and installation
- `HOW_TO_USE.md` - Detailed usage instructions
- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific background*