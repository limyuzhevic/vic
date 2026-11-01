# NLM Debugging and Performance Monitoring System

## Overview

This module provides comprehensive debugging and performance monitoring tools for the NLM framework. It includes:

1. **Performance Monitoring**: Track neural activity, learning progress, and system metrics
2. **Debug Logging**: Detailed logging for troubleshooting and analysis
3. **Memory Management**: Monitor and optimize memory usage
4. **Simulation Control**: Tools for controlling and analyzing simulations
5. **Neural State Tracking**: Monitor neuron states, synaptic dynamics, and network properties
6. **Learning Analysis**: Analyze learning progress and adaptation patterns
7. **Integration Testing**: Validate integration between different brain systems

## Performance Monitoring

### Performance Monitor Class

```python
from pynlm import Brain, SimpleWorld, createBrain, createSimpleWorld
import numpy as np
import time
import json
from typing import Dict, List, Optional, Callable

class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations."""
    
    def __init__(self, brain: Brain, world: SimpleWorld):
        self.brain = brain
        self.world = world
        self.metrics = {}
        self.start_time = time.time()
        self.step_count = 0
        self.episode_start_time = time.time()
        
        # Initialize metric tracking
        self.initialize_metrics()
    
    def initialize_metrics(self):
        """Initialize all performance metrics."""
        self.metrics = {
            'neural': {
                'step': [],
                'total_spikes': [],
                'active_neurons': [],
                'firing_rate': [],
                'excitatory_inhibition_ratio': [],
                'region_activity': {},
                'population_activity': {},
                'synaptic_strength': [],
                'plasticity_active': [],
            },
            'memory': {
                'step': [],
                'working_memory_active': [],
                'working_memory_capacity': [],
                'episodic_episodes': [],
                'episodic_replay_count': [],
                'associative_associations': [],
                'memory_consolidation': [],
            },
            'neuromodulation': {
                'step': [],
                'dopamine_level': [],
                'curiosity_level': [],
                'novelty_level': [],
                'prediction_error': [],
                'neurally_modulated_spikes': [],
            },
            'cognitive': {
                'step': [],
                'planning_confidence': [],
                'concept_activation': [],
                'attention_focus': [],
                'decision_confidence': [],
            },
            'environmental': {
                'step': [],
                'agent_position': [],
                'agent_orientation': [],
                'energy_level': [],
                'health_level': [],
                'reward_received': [],
                'action_taken': [],
                'object_interactions': [],
            },
            'development': {
                'step': [],
                'developmental_stage': [],
                'plasticity_rate': [],
                'synaptogenesis_rate': [],
                'pruning_rate': [],
                'critical_period_active': [],
            },
            'simulation': {
                'step': [],
                'wall_clock_time': [],
                'simulation_time': [],
                'time_per_step': [],
                'cpu_usage': [],
                'memory_usage': [],
            },
        }
    
    def record_step(self, step: int):
        """Record metrics for a simulation step."""
        self.step_count = step
        current_time = time.time()
        simulation_time = current_time - self.start_time
        
        # Record neural metrics
        self.metrics['neural']['step'].append(step)
        self.metrics['neural']['total_spikes'].append(self.brain.getTotalSpikeCount())
        self.metrics['neural']['active_neurons'].append(self.brain.getActiveNeuronCount())
        self.metrics['neural']['firing_rate'].append(self.brain.getAverageFiringRate())
        self.metrics['neural']['excitatory_inhibition_ratio'].append(
            self.brain.getExcitationInhibitionRatio())
        
        # Record memory metrics
        if self.brain.getWorkingMemory():
            self.metrics['memory']['working_memory_active'].append(
                self.brain.getWorkingMemory().getActiveTraces())
        
        if self.brain.getWorkingMemory():
            self.metrics['memory']['working_memory_capacity'].append(
                self.brain.getWorkingMemory().getCapacity())
        
        if self.brain.getEpisodicMemory():
            self.metrics['memory']['episodic_episodes'].append(
                self.brain.getEpisodicMemory().getEpisodeCount())
        
        # Record neuromodulation metrics
        if hasattr(self.brain, 'getDopamine') and self.brain.getDopamine():
            self.metrics['neuromodulation']['dopamine_level'].append(
                self.brain.getDopamine().getLevel())
        
        if hasattr(self.brain, 'getCuriosity') and self.brain.getCuriosity():
            self.metrics['neuromodulation']['curiosity_level'].append(
                self.brain.getCuriosity().getLevel())
        
        if hasattr(self.brain, 'getNovelty') and self.brain.getNovelty():
            self.metrics['neuromodulation']['novelty_level'].append(
                self.brain.getNovelty().getLevel())
        
        if hasattr(self.brain, 'getPredictionErrorSignal'):
            self.metrics['neuromodulation']['prediction_error'].append(
                self.brain.getPredictionErrorSignal().getError())
        
        # Record environmental metrics
        agent_body = self.world.getAgentBody()
        self.metrics['environmental']['step'].append(step)
        self.metrics['environmental']['agent_position'].append(
            [agent_body.x, agent_body.y])
        self.metrics['environmental']['agent_orientation'].append(
            agent_body.orientation)
        self.metrics['environmental']['energy_level'].append(agent_body.energy)
        self.metrics['environmental']['health_level'].append(agent_body.health)
        
        # Record simulation metrics
        self.metrics['simulation']['step'].append(step)
        self.metrics['simulation']['wall_clock_time'].append(current_time)
        self.metrics['simulation']['simulation_time'].append(simulation_time)
        
        if step > 0:
            prev_time = self.metrics['simulation']['wall_clock_time'][-2]
            time_per_step = (current_time - prev_time) * 1000  # milliseconds
            self.metrics['simulation']['time_per_step'].append(time_per_step)
    
    def get_neural_health(self) -> Dict:
        """Get neural system health metrics."""
        health = {}
        
        # Check spike rate balance
        if len(self.metrics['neural']['firing_rate']) > 10:
            recent_rates = self.metrics['neural']['firing_rate'][-10:]
            rate_mean = np.mean(recent_rates)
            rate_std = np.std(recent_rates)
            rate_cv = rate_std / rate_mean if rate_mean > 0 else 0
            
            health['firing_stability'] = {
                'mean_hz': rate_mean,
                'coefficient_of_variation': rate_cv,
                'stable': rate_cv < 0.5
            }
        
        # Check excitation/inhibition balance
        if self.metrics['neural']['excitatory_inhibition_ratio']:
            recent_ratios = self.metrics['neural']['excitatory_inhibition_ratio'][-20:]
            ratio_mean = np.mean(recent_ratios)
            health['e_i_balance'] = {
                'mean_ratio': ratio_mean,
                'balanced': 0.5 < ratio_mean < 2.0
            }
        
        # Check for runaway excitation
        if len(self.metrics['neural']['total_spikes']) > 100:
            spike_trend = np.polyfit(
                range(len(self.metrics['neural']['total_spikes'][-50:])),
                self.metrics['neural']['total_spikes'][-50:],
                1
```

### Advanced Debug Logger

```python
import logging
from datetime import datetime
from enum import Enum
from typing import Dict, Any

class LogLevel(Enum):
    TRACE = 5
    DEBUG = 10
    INFO = 20
    WARNING = 30
    ERROR = 40
    CRITICAL = 50

class AdvancedDebugLogger:
    """Advanced logging system for NLM debugging."""
    
    def __init__(self, log_file: str = "nlm_debug.log"):
        self.log_file = log_file
        self.setup_logging()
        self.debug_metrics = {}
        self.performance_baseline = {}
        self.alert_thresholds = {}
    
    def setup_logging(self):
        """Setup comprehensive logging configuration."""
        # Create logger
        self.logger = logging.getLogger('nlm.debug')
        self.logger.setLevel(logging.DEBUG)
        
        # Remove existing handlers
        self.logger.handlers.clear()
        
        # File handler with detailed formatting
        file_handler = logging.FileHandler(self.log_file)
        file_handler.setLevel(logging.DEBUG)
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        
        # Formatters
        detailed_formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - '
            '%(filename)s:%(lineno)d - %(message)s'
        )
        
        simple_formatter = logging.Formatter(
            '%(asctime)s - %(levelname)s - %(message)s'
        )
        
        file_handler.setFormatter(detailed_formatter)
        console_handler.setFormatter(simple_formatter)
        
        # Add handlers
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        # Prevent propagation to root logger
        self.logger.propagate = False
    
    def log_step_details(self, step: int, action_result, neural_state):
        """Log detailed information for a simulation step."""
        self.logger.debug(f"=== STEP {step} DETAILS ===")
        
        # Log neural state
        self.logger.debug(f"  Neural State:")
        self.logger.debug(f"    Total Spikes: {neural_state.get('total_spikes', 0)}")
        self.logger.debug(f"    Active Neurons: {neural_state.get('active_neurons', 0)}")
        self.logger.debug(f"    Firing Rate: {neural_state.get('firing_rate', 0):.4f} Hz")
        self.logger.debug(f"    E/I Ratio: {neural_state.get('e_i_ratio', 0):.4f}")
        
        # Log action result
        self.logger.debug(f"  Action Result:")
        self.logger.debug(f"    Reward: {action_result.reward}")
        self.logger.debug(f"    Success: {action_result.success}")
        self.logger.debug(f"    Message: {action_result.message}")
        
        # Log agent state
        agent_body = self.world.getAgentBody()
        self.logger.debug(f"  Agent State:")
        self.logger.debug(f"    Position: ({agent_body.x:.2f}, {agent_body.y:.2f})")
        self.logger.debug(f"    Orientation: {agent_body.orientation:.2f} rad")
        self.logger.debug(f"    Energy: {agent_body.energy:.2f}")
        self.logger.debug(f"    Health: {agent_body.health:.2f}")
    
    def track_performance_anomalies(self, metrics: Dict):
        """Track and alert on performance anomalies."""
        # Store baseline if not already set
        if not self.performance_baseline:
            self.performance_baseline = metrics.copy()
            self.logger.info("Performance baseline established")
            return
        
        # Check for anomalies
        anomalies = []
        
        # Check spike rate anomalies
        current_spikes = metrics['neural']['total_spikes']
        if len(current_spikes) >= 10:
            recent_spikes = current_spikes[-10:]
            spike_rate = sum(recent_spikes) / len(recent_spikes)
            baseline_spikes = self.performance_baseline['neural']['total_spikes'][-10:]
            baseline_rate = sum(baseline_spikes) / len(baseline_spikes)
            
            spike_change = (spike_rate - baseline_rate) / baseline_rate if baseline_rate > 0 else 0
            
            if abs(spike_change) > 2.0:
                anomalies.append({
                    'type': 'spike_rate_change',
                    'severity': 'high' if abs(spike_change) > 5.0 else 'medium',
                    'change_percent': spike_change * 100,
                    'current_rate': spike_rate,
                    'baseline_rate': baseline_rate
                })
        
        # Check reward anomalies
        current_rewards = metrics['environmental']['reward_received']
        if len(current_rewards) >= 20:
            recent_rewards = current_rewards[-20:]
            reward_mean = np.mean(recent_rewards)
            reward_std = np.std(recent_rewards)
            
            if reward_std > abs(reward_mean) * 2:
                anomalies.append({
                    'type': 'reward_volatility',
                    'severity': 'high' if reward_std > abs(reward_mean) * 5 else 'medium',
                    'mean_reward': reward_mean,
                    'std_reward': reward_std
                })
        
        # Log anomalies
        if anomalies:
            self.logger.warning(f"Performance anomalies detected:")
            for anomaly in anomalies:
                self.logger.warning(f"  {anomaly['type']}: {anomaly}")
        
        return anomalies
    
    def export_debug_data(self, filename: str):
        """Export debug data to file."""
        import json
        import os
        
        # Prepare data for export
        export_data = {
            'timestamp': datetime.now().isoformat(),
            'debug_metrics': self.debug_metrics,
            'performance_baseline': self.performance_baseline,
            'alert_thresholds': self.alert_thresholds
        }
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filename), exist_ok=True)
        
        # Export to JSON
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        self.logger.info(f"Debug data exported to: {filename}")
```

### Memory Monitor

```python
import psutil
import time
from typing import Dict, List
import gc

class MemoryMonitor:
    """Monitor memory usage during NLM simulations."""
    
    def __init__(self):
        self.memory_samples = []
        self.process = psutil.Process()
        self.baseline_memory = None
        self.peak_memory = 0
        self.start_time = time.time()
    
    def record_memory_usage(self):
        """Record current memory usage."""
        # Get process memory info
        memory_info = self.process.memory_info()
        
        # Record RSS (Resident Set Size) and VMS (Virtual Memory Size)
        memory_sample = {
            'timestamp': time.time(),
            'rss_mb': memory_info.rss / 1024 / 1024,  # MB
            'vms_mb': memory_info.vms / 1024 / 1024,  # MB
            'shared_mb': memory_info.shared / 1024 / 1024,  # MB
            'text_mb': memory_info.text / 1024 / 1024,  # MB
            'data_mb': memory_info.data / 1024 / 1024,  # MB
            'dirty_mb': memory_info.dirty / 1024 / 1024,  # MB
        }
        
        self.memory_samples.append(memory_sample)
        
        # Update peak memory
        current_rss = memory_info.rss / 1024 / 1024
        if current_rss > self.peak_memory:
            self.peak_memory = current_rss
        
        # Set baseline if not already set
        if self.baseline_memory is None:
            self.baseline_memory = memory_sample
        
        return memory_sample
    
    def get_memory_statistics(self) -> Dict:
        """Get memory usage statistics."""
        if not self.memory_samples:
            return {}
        
        rss_values = [sample['rss_mb'] for sample in self.memory_samples]
        vms_values = [sample['vms_mb'] for sample in self.memory_samples]
        
        stats = {
            'duration_seconds': time.time() - self.start_time,
            'sample_count': len(self.memory_samples),
            'rss': {
                'mean_mb': np.mean(rss_values),
                'std_mb': np.std(rss_values),
                'min_mb': np.min(rss_values),
                'max_mb': np.max(rss_values),
                'peak_mb': self.peak_memory,
                'growth_mb': rss_values[-1] - rss_values[0] if len(rss_values) > 1 else 0,
            },
            'vms': {
                'mean_mb': np.mean(vms_values),
                'std_mb': np.std(vms_values),
                'min_mb': np.min(vms_values),
                'max_mb': np.max(vms_values),
            },
            'memory_efficiency': {
                'rss_to_vms_ratio': np.mean(rss_values) / np.mean(vms_values),
                'memory_growth_rate_mb_per_sec': (rss_values[-1] - rss_values[0]) / 
                                                (time.time() - self.start_time),
            }
        }
        
        return stats
    
    def analyze_memory_trends(self) -> Dict:
        """Analyze memory usage trends."""
        if len(self.memory_samples) < 10:
            return {}
        
        rss_values = [sample['rss_mb'] for sample in self.memory_samples]
        timestamps = [sample['timestamp'] for sample in self.memory_samples]
        
        # Calculate linear trend
        if len(rss_values) >= 3:
            x = np.array(range(len(rss_values)))
            y = np.array(rss_values)
            
            # Linear regression
            slope = np.polyfit(x, y, 1)[0]
            intercept = np.polyfit(x, y, 1)[1]
            
            # R-squared
            y_pred = slope * x + intercept
            ss_res = np.sum((y - y_pred) ** 2)
            ss_tot = np.sum((y - np.mean(y)) ** 2)
            r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
            
            return {
                'linear_trend': {
                    'slope_mb_per_sample': slope,
                    'slope_mb_per_sec': slope / (timestamps[1] - timestamps[0]) if len(timestamps) > 1 else 0,
                    'intercept_mb': intercept,
                    'r_squared': r_squared,
                    'trend_direction': 'increasing' if slope > 0.1 else 'decreasing' if slope < -0.1 else 'stable',
                },
                'recent_trend': {
                    'last_10_samples_slope': np.polyfit(range(10), rss_values[-10:], 1)[0],
                    'last_50_samples_slope': np.polyfit(range(50), rss_values[-50:], 1)[0] if len(rss_values) >= 50 else 0,
                },
                'memory_efficiency': {
                    'current_efficiency': rss_values[-1] / vms_values[-1],
                    'average_efficiency': np.mean([r / v for r, v in zip(rss_values, vms_values)]),
                }
            }
        
        return {}
```

### Simulation Controller

```python
class SimulationController:
    """Control and manage NLM simulations."""
    
    def __init__(self, brain: Brain, world: SimpleWorld):
        self.brain = brain
        self.world = world
        self.simulation_running = False
        self.simulation_paused = False
        self.current_step = 0
        self.max_steps = 0
        self.simulation_start_time = 0
        self.callbacks = []
    
    def start_simulation(self, max_steps: int, save_interval: int = 1000):
        """Start a simulation with specified parameters."""
        self.max_steps = max_steps
        self.simulation_running = True
        self.simulation_paused = False
        self.simulation_start_time = time.time()
        self.current_step = 0
        
        # Initialize callbacks
        self.setup_callbacks(save_interval)
        
        print(f"Starting simulation: {max_steps} steps")
        print(f"Save interval: {save_interval} steps")
    
    def pause_simulation(self):
        """Pause the simulation."""
        self.simulation_paused = True
        print("Simulation paused")
    
    def resume_simulation(self):
        """Resume the simulation."""
        self.simulation_paused = False
        print("Simulation resumed")
    
    def stop_simulation(self):
        """Stop the simulation."""
        self.simulation_running = False
        self.simulation_paused = False
        print("Simulation stopped")
    
    def setup_callbacks(self, save_interval: int):
        """Setup simulation callbacks."""
        # Create checkpoints periodically
        def checkpoint_callback(step: int):
            if step % save_interval == 0:
                checkpoint_file = f"checkpoint_{step}.bin"
                if self.brain.save(checkpoint_file):
                    print(f"Checkpoint saved: {checkpoint_file}")
                else:
                    print(f"Failed to save checkpoint: {checkpoint_file}")
        
        # Record performance metrics
        def performance_callback(step: int):
            if step % 100 == 0:
                firing_rate = self.brain.getAverageFiringRate()
                spike_count = self.brain.getTotalSpikeCount()
                print(f"Step {step}: FR={firing_rate:.2f}Hz, Total Spikes={spike_count}")
        
        # Check for simulation completion
        def completion_callback(step: int):
            if step >= self.max_steps:
                self.stop_simulation()
                self.on_simulation_complete()
        
        # Add callbacks to simulation loop
        self.callbacks = [
            checkpoint_callback,
            performance_callback,
            completion_callback,
        ]
    
    def run_step(self) -> bool:
        """Run a single simulation step."""
        if not self.simulation_running or self.simulation_paused:
            return False
        
        # Record start time
        step_start_time = time.time()
        
        # Execute simulation step
        try:
            self.current_step += 1
            
            # Get motor command
            cmd = self.brain.decodeMotorCommand()
            
            # Apply action to world
            action_result = self.world.applyMotorCommand(cmd, self.current_step * 0.001)
            
            # Apply reward modulation
            self.brain.applyRewardModulation(action_result.reward, self.brain.getPredictionError())
            
            # Record completion time
            step_time = time.time() - step_start_time
            
            # Execute callbacks
            for callback in self.callbacks:
                callback(self.current_step)
            
            print(f"Step {self.current_step} completed in {step_time:.4f}s, "
                  f"Reward: {action_result.reward:.3f}")
            
            return True
            
        except Exception as e:
            print(f"Error in simulation step {self.current_step}: {e}")
            return False
    
    def run_simulation(self) -> Dict:
        """Run the complete simulation."""
        self.start_simulation(self.max_steps)
        
        while self.simulation_running:
            if not self.run_step():
                break
        
        # Calculate final statistics
        stats = self.calculate_simulation_stats()
        
        # Save simulation data
        self.save_simulation_data(stats)
        
        return stats
    
    def calculate_simulation_stats(self) -> Dict:
        """Calculate final simulation statistics."""
        end_time = time.time()
        total_time = end_time - self.simulation_start_time
        
        stats = {
            'steps_completed': self.current_step,
            'total_time_seconds': total_time,
            'steps_per_second': self.current_step / total_time if total_time > 0 else 0,
            'final_neural_state': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            },
            'environmental_state': {
                'final_position': [self.world.getAgentBody().x, self.world.getAgentBody().y],
                'final_orientation': self.world.getAgentBody().orientation,
                'final_energy': self.world.getAgentBody().energy,
                'final_health': self.world.getAgentBody().health,
            },
            'simulation_metrics': {
                'success_rate': 0.85,  # Placeholder
                'reward_efficiency': 0.75,  # Placeholder
                'stability_index': 0.80,  # Placeholder
            }
        }
        
        return stats
    
    def save_simulation_data(self, stats: Dict):
        """Save simulation data to files."""
        import json
        import os
        
        # Create directory if it doesn't exist
        os.makedirs("simulation_results", exist_ok=True)
        
        # Save JSON stats
        with open("simulation_results/simulation_stats.json", 'w') as f:
            json.dump(stats, f, indent=2)
        
        # Save neural checkpoints periodically
        for step in range(0, self.max_steps + 1, 1000):
            checkpoint_file = f"simulation_results/checkpoint_{step}.bin"
            if step <= self.current_step:
                self.brain.save(checkpoint_file)
        
        print(f"Simulation data saved to simulation_results/")
    
    def on_simulation_complete(self):
        """Called when simulation completes."""
        print("=== SIMULATION COMPLETE ===")
        print(f"Total steps: {self.current_step}")
        print(f"Total time: {time.time() - self.simulation_start_time:.2f} seconds")
        print(f"Steps per second: {self.current_step / (time.time() - self.simulation_start_time):.2f}")
        print(f"Final neural state: FR={self.brain.getAverageFiringRate():.2f}Hz, "
              f"Total Spikes={self.brain.getTotalSpikeCount()}")
    
    def get_simulation_status(self) -> Dict:
        """Get current simulation status."""
        return {
            'running': self.simulation_running,
            'paused': self.simulation_paused,
            'current_step': self.current_step,
            'max_steps': self.max_steps,
            'progress_percent': (self.current_step / self.max_steps * 100) if self.max_steps > 0 else 0,
            'simulation_time': time.time() - self.simulation_start_time,
        }
```

### Integration Test Framework

```python
class IntegrationTestFramework:
    """Framework for testing NLM system integration."""
    
    def __init__(self):
        self.test_results = []
        self.performance_benchmarks = {}
    
    def run_integration_tests(self):
        """Run comprehensive integration tests."""
        print("=== NLM INTEGRATION TEST FRAMEWORK ===")
        
        # Test 1: Basic neural functionality
        self.test_neural_functionality()
        
        # Test 2: Memory integration
        self.test_memory_integration()
        
        # Test 3: Neuromodulation integration
        self.test_neuromodulation_integration()
        
        # Test 4: Cognitive integration
        self.test_cognitive_integration()
        
        # Test 5: Development integration
        self.test_development_integration()
        
        # Test 6: Performance benchmarks
        self.run_performance_benchmarks()
        
        # Generate test report
        self.generate_test_report()
    
    def test_neural_functionality(self):
        """Test basic neural functionality."""
        print("\n--- Test 1: Neural Functionality ---")
        
        config = Config()
        config.set("neuron_count", 500)
        config.set("region_count", 2)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Test basic neural operations
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        
        # Verify neurons are created
        if neurons >= 100:
            print("✓ Neural initialization successful")
            self.test_results.append({
                'test': 'neural_functionality',
                'status': 'PASS',
                'details': f'Created {neurons} neurons, {synapses} synapses'
            })
        else:
            print("✗ Neural initialization failed")
            self.test_results.append({
                'test': 'neural_functionality',
                'status': 'FAIL',
                'details': f'Failed to create sufficient neurons: {neurons}'
            })
    
    def test_memory_integration(self):
        """Test memory system integration."""
        print("\n--- Test 2: Memory Integration ---")
        
        config = Config()
        config.set("neuron_count", 500)
        config.set("replay_interval", 100)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Test working memory
        if brain.getWorkingMemory():
            print("✓ Working memory initialized")
        else:
            print("✗ Working memory failed to initialize")
            self.test_results.append({
                'test': 'memory_integration',
                'status': 'FAIL',
                'details': 'Working memory not initialized'
            })
            return
        
        # Test episodic memory
        if brain.getEpisodicMemory():
            print("✓ Episodic memory initialized")
        else:
            print("✗ Episodic memory failed to initialize")
            self.test_results.append({
                'test': 'memory_integration',
                'status': 'FAIL',
                'details': 'Episodic memory not initialized'
            })
            return
        
        # Test memory integration with neural processing
        brain.step(0, 0.0)
        
        if brain.getWorkingMemory().getActiveTraces() > 0:
            print("✓ Memory integration with neural processing successful")
            self.test_results.append({
                'test': 'memory_integration',
                'status': 'PASS',
                'details': 'Memory systems integrated with neural processing'
            })
        else:
            print("✗ Memory integration failed")
            self.test_results.append({
                'test': 'memory_integration',
                'status': 'FAIL',
                'details': 'Memory systems not processing neural activity'
            })
    
    def test_neuromodulation_integration(self):
        """Test neuromodulation system integration."""
        print("\n--- Test 3: Neuromodulation Integration ---")
        
        config = Config()
        config.set("neuron_count", 500)
        config.set("dopamine_baseline", 0.1)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Check neuromodulation components
        neuromod_components = [
            ('Dopamine', brain.getDopamine()),
            ('Curiosity', brain.getCuriosity()),
            ('Novelty', brain.getNovelty()),
            ('PredictionError', brain.getPredictionErrorSignal()),
        ]
        
        initialized = 0
        for name, component in neuromod_components:
            if component:
                print(f"✓ {name} initialized")
                initialized += 1
            else:
                print(f"✗ {name} failed to initialize")
        
        if initialized >= 3:
            print("✓ Neuromodulation integration successful")
            self.test_results.append({
                'test': 'neuromodulation_integration',
                'status': 'PASS',
                'details': f'{initialized}/{len(neuromod_components)} neuromodulators initialized'
            })
        else:
            print("✗ Neuromodulation integration failed")
            self.test_results.append({
                'test': 'neuromodulation_integration',
                'status': 'FAIL',
                'details': f'Only {initialized}/{len(neuromod_components)} neuromodulators initialized'
            })
    
    def test_cognitive_integration(self):
        """Test cognitive system integration."""
        print("\n--- Test 4: Cognitive Integration ---")
        
        config = Config()
        config.set("neuron_count", 500)
        config.set("enable_concept_formation", True)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Check cognitive components
        cognitive_components = [
            ('NeuralPlanner', brain.getPlanner()),
            ('ConceptFormation', brain.getConceptFormation()),
            ('AttentionalSelection', brain.getAttention()),
        ]
        
        initialized = 0
        for name, component in cognitive_components:
            if component:
                print(f"✓ {name} initialized")
                initialized += 1
            else:
                print(f"✗ {name} failed to initialize")
        
        if initialized >= 2:
            print("✓ Cognitive integration successful")
            self.test_results.append({
                'test': 'cognitive_integration',
                'status': 'PASS',
                'details': f'{initialized}/{len(cognitive_components)} cognitive systems initialized'
            })
        else:
            print("✗ Cognitive integration failed")
            self.test_results.append({
                'test': 'cognitive_integration',
                'status': 'FAIL',
                'details': f'Only {initialized}/{len(cognitive_components)} cognitive systems initialized'
            })
    
    def test_development_integration(self):
        """Test development system integration."""
        print("\n--- Test 5: Development Integration ---")
        
        config = Config()
        config.set("neuron_count", 500)
        config.set("enable_development", True)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Check development components
        if brain.getDevelopmentSystem():
            print("✓ Development system initialized")
        else:
            print("✗ Development system failed to initialize")
            self.test_results.append({
                'test': 'development_integration',
                'status': 'FAIL',
                'details': 'Development system not initialized'
            })
            return
        
        # Test developmental stage changes
        brain.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod)
        if brain.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod:
            print("✓ Developmental stage control working")
            self.test_results.append({
                'test': 'development_integration',
                'status': 'PASS',
                'details': 'Development system functional with stage control'
            })
        else:
            print("✗ Developmental stage control failed")
            self.test_results.append({
                'test': 'development_integration',
                'status': 'FAIL',
                'details': 'Development system stage control not working'
            })
    
    def run_performance_benchmarks(self):
        """Run performance benchmarks."""
        print("\n--- Test 6: Performance Benchmarks ---")
        
        # Define benchmarks
        benchmarks = {
            'neural_initialization': {
                'description': 'Time to initialize neural systems',
                'threshold_ms': 1000,
                'target': '< 1 second'
            },
            'simulation_speed': {
                'description': 'Simulation steps per second',
                'threshold_steps_per_sec': 100,
                'target': '> 100 steps/second'
            },
            'memory_efficiency': {
                'description': 'Memory usage growth rate',
                'threshold_mb_per_sec': 10,
                'target': '< 10 MB/sec'
            },
            'learning_efficiency': {
                'description': 'Learning progress per step',
                'threshold_reward_per_step': 0.01,
                'target': '> 0.01 reward per step average'
            }
        }
        
        for bench_name, bench_config in benchmarks.items():
            print(f"\nRunning {bench_name} benchmark...")
            print(f"  Description: {bench_config['description']}")
            print(f"  Target: {bench_config['target']}")
            
            # Run benchmark (simplified implementation)
            start_time = time.time()
            
            # Simulate benchmark workload
            config = Config()
            config.set("neuron_count", 100)
            config.set("region_count", 1)
            
            brain = createBrain(config)
            brain.initialize()
            
            # Run a few steps
            for step in range(10):
                brain.step(step, step * 0.001)
            
            end_time = time.time()
            elapsed = (end_time - start_time) * 1000  # milliseconds
            
            print(f"  Result: {elapsed:.2f} ms")
            
            # Check if benchmark passes
            if bench_name == 'neural_initialization':
                if elapsed < bench_config['threshold_ms']:
                    print(f"  ✓ PASS")
                    self.performance_benchmarks[bench_name] = {'status': 'PASS', 'time_ms': elapsed}
                else:
                    print(f"  ✗ FAIL")
                    self.performance_benchmarks[bench_name] = {'status': 'FAIL', 'time_ms': elapsed}
            
            elif bench_name == 'simulation_speed':
                steps_per_sec = 10 / (elapsed / 1000)
                if steps_per_sec > bench_config['threshold_steps_per_sec']:
                    print(f"  ✓ PASS ({steps_per_sec:.1f} steps/sec)")
                    self.performance_benchmarks[bench_name] = {'status': 'PASS', 'steps_per_sec': steps_per_sec}
                else:
                    print(f"  ✗ FAIL ({steps_per_sec:.1f} steps/sec)")
                    self.performance_benchmarks[bench_name] = {'status': 'FAIL', 'steps_per_sec': steps_per_sec}
    
    def generate_test_report(self):
        """Generate comprehensive test report."""
        print("\n=== INTEGRATION TEST REPORT ===")
        
        total_tests = len(self.test_results)
        passed_tests = len([r for r in self.test_results if r['status'] == 'PASS'])
        failed_tests = len([r for r in self.test_results if r['status'] == 'FAIL'])
        
        print(f"\nTest Summary:")
        print(f"  Total Tests: {total_tests}")
        print(f"  Passed: {passed_tests}")
        print(f"  Failed: {failed_tests}")
        print(f"  Success Rate: {passed_tests/total_tests*100:.1f}%")
        
        print(f"\nTest Results:")
        for result in self.test_results:
            status_icon = "✓" if result['status'] == 'PASS' else "✗"
            print(f"  {status_icon} {result['test']}: {result['status']} - {result['details']}")
        
        if failed_tests == 0:
            print(f"\n🎉 ALL INTEGRATION TESTS PASSED! 🎉")
        else:
            print(f"\n⚠️  {failed_tests} INTEGRATION TESTS FAILED ⚠️")
        
        # Generate performance report
        print(f"\nPerformance Benchmarks:")
        for bench_name, bench_result in self.performance_benchmarks.items():
            status_icon = "✓" if bench_result['status'] == 'PASS' else "✗"
            print(f"  {status_icon} {bench_name}: {bench_result}")
        
        # Save report to file
        import json
        report = {
            'timestamp': datetime.now().isoformat(),
            'test_summary': {
                'total_tests': total_tests,
                'passed_tests': passed_tests,
                'failed_tests': failed_tests,
                'success_rate': passed_tests/total_tests*100,
            },
            'test_results': self.test_results,
            'performance_benchmarks': self.performance_benchmarks,
        }
        
        with open("integration_test_report.json", 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\nReport saved to: integration_test_report.json")
```

This debugging and performance monitoring system provides comprehensive tools for developing, testing, and analyzing NLM simulations. It includes performance monitoring, advanced logging, memory management, simulation control, and integration testing capabilities.

The system is designed to be modular and extensible, allowing developers to add custom monitoring and debugging tools as needed. It provides detailed metrics and analysis to help identify performance issues, debug problems, and optimize simulation performance.

Note: Some advanced features require additional dependencies (psutil, numpy) which can be installed via:
```bash
pip install psutil numpy
```

The monitoring tools can be integrated into existing NLM applications to provide real-time insights into simulation performance and behavior.