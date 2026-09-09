# Advanced NLM Features Guide

## Overview

This guide covers advanced features and capabilities for power users of the NLM (Neural Learning Machine) framework. These features are designed for developers and researchers who want to customize, extend, and optimize the neural simulation system for their specific use cases.

## Table of Contents

1. [Plugin Architecture](#plugin-architecture)
2. [Advanced Configuration Management](#advanced-configuration-management)
3. [Performance Monitoring and Optimization](#performance-monitoring-and-optimization)
4. [Advanced Learning Algorithms](#advanced-learning-algorithms)
5. [Multi-Agent Systems](#multi-agent-systems)
6. [Debugging and Visualization Tools](#debugging-and-visualization-tools)
7. [Hardware Acceleration](#hardware-acceleration)
8. [Research and Development Features](#research-and-development-features)

---

# 1. Plugin Architecture

## 1.1 Neuron Plugin System

Create custom neuron types that can be dynamically loaded and used in simulations.

```python
import pynlm
from pynlm.plugin import NeuronPlugin, SynapsePlugin

class CustomNeuronPlugin(NeuronPlugin):
    """Example: Custom neuron with unique dynamics"""
    
    def getNeuronType(self):
        return "CustomLIF"
    
    def createNeuron(self, neuron_id):
        # Create neuron with custom properties
        neuron = pynlm.createNeuron()
        neuron.setType(pynlm.NeuronType.Custom)
        neuron.setMembranePotential(-65.0f)
        neuron.setThreshold(-50.0f)
        # Custom dynamics
        neuron.setAdaptationVariable(0.01f)
        return neuron
    
    def stepNeuron(self, neuron, current_time, dt):
        # Implement custom stepping logic
        state = neuron.getState()
        
        # Custom LIF with adaptation
        v = state.membranePotential
        v += current_time * state.leakConductance * (state.restingPotential - v)
        v += state.synapseConductance * (current_time * state.totalCurrent)
        
        # Add adaptation current
        v += current_time * 0.1f * state.adaptationVariable
        
        # Reset if threshold crossed
        if v > state.threshold:
            v = state.resetPotential
            # Increase adaptation
            state.adaptationVariable += 0.1f
        
        neuron.setMembranePotential(v)
        return neuron

# Register plugin
pynlm.registerNeuronPlugin(CustomNeuronPlugin())
```

## 1.2 Synapse Plugin System

Create custom synaptic dynamics and learning rules.

```python
class CustomSynapsePlugin(SynapsePlugin):
    """Example: Custom synapse with unique properties"""
    
    def getSynapseType(self):
        return "CustomPlastic"
    
    def createSynapse(self, source_id, dest_id):
        synapse = pynlm.createSynapse(source_id, dest_id)
        
        # Custom properties
        synapse.setType(pynlm.SynapseType.Electrical)
        synapse.setWeight(0.5f)
        synapse.setDelay(2)
        
        # Custom plasticity rules
        flags = pynlm.PlasticityFlags()
        flags.hebbian = True
        flags.stdp = True
        flags.reward_modulated = True
        synapse.enablePlasticity(flags)
        
        return synapse
    
    def applyPlasticity(self, synapse, pre_spike, post_spike):
        # Custom STDP with additional factors
        weight = synapse.getWeight()
        
        # Standard STDP
        if pre_spike and post_spike:
            dt = post_spike - pre_spike
            if dt > 0:
                weight += 0.01f * (1.0f - dt / 50.0f)  # Decay with time difference
            else:
                weight -= 0.02f * (1.0f + dt / 50.0f)  # Negative for post before pre
        
        # Additional custom rule
        time_since_pre = self.getCurrentTime() - pre_spike
        if time_since_pre < 10.0f:  # Strong early potentiation
            weight += 0.05f
        
        # Clamp weight
        weight = max(-1.0f, min(1.0f, weight))
        synapse.setWeight(weight)

# Register plugin
pynlm.registerSynapsePlugin(CustomSynapsePlugin())
```

## 1.3 Module Loading System

```python
class ModuleManager:
    """Manage loading and unloading of plugins"""
    
    def __init__(self):
        self.neuron_plugins = {}
        self.synapse_plugins = {}
        self.loaded_modules = set()
    
    def load_module(self, module_path):
        # Dynamically load Python modules
        import importlib.util
        spec = importlib.util.spec_from_file_location("module", module_path)
        module = importlib.util.module_from_spec(spec)
        
        # Execute module to register plugins
        spec.loader.exec_module(module)
        self.loaded_modules.add(module_path)
    
    def unload_module(self, module_path):
        # Remove plugin references
        if module_path in self.loaded_modules:
            self.loaded_modules.remove(module_path)
    
    def get_neuron_plugin(self, name):
        return self.neuron_plugins.get(name)
    
    def get_synapse_plugin(self, name):
        return self.synapse_plugins.get(name)
```

---

# 2. Advanced Configuration Management

## 2.1 Configuration Templates

Create and manage configuration templates for different simulation scenarios.

```python
import json
import os
from pathlib import Path

class ConfigTemplateManager:
    """Manage configuration templates for different scenarios"""
    
    def __init__(self, template_dir="templates"):
        self.template_dir = Path(template_dir)
        self.template_dir.mkdir(exist_ok=True)
        
        # Built-in templates
        self.templates = {
            "basic": self._get_basic_template(),
            "learning": self._get_learning_template(),
            "cognitive": self._get_cognitive_template(),
            "production": self._get_production_template(),
        }
    
    def _get_basic_template(self):
        return {
            "brain": {
                "neuron_count": 1000,
                "synapse_density": 0.1,
                "connection_probability": 0.05,
            },
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": 0.01},
                "hebbian": {"enable": True, "learning_rate": 0.005},
                "structural": {"enable": False},
            },
            "neuromodulation": {
                "dopamine": {"scale": 1.0},
                "curiosity": {"enable": True},
                "novelty": {"enable": True},
            }
        }
    
    def get_template(self, name):
        return self.templates.get(name, self.templates["basic"])
    
    def save_template(self, name, config_dict):
        """Save a new configuration template"""
        self.templates[name] = config_dict
        
        # Save to file
        filepath = self.template_dir / f"{name}.json"
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
    
    def load_template(self, name):
        """Load a template from file"""
        filepath = self.template_dir / f"{name}.json"
        if filepath.exists():
            with open(filepath, 'r') as f:
                file_template = json.load(f)
                self.templates[name] = file_template
        
        return self.get_template(name)
    
    def create_config_from_template(self, template_name, overrides=None):
        """Create a NLM config from a template with optional overrides"""
        config_dict = self.get_template(template_name)
        
        if overrides:
            self._apply_overrides(config_dict, overrides)
        
        return self._dict_to_config(config_dict)
    
    def _apply_overrides(self, config_dict, overrides):
        """Recursively apply overrides to nested dictionary"""
        for key, value in overrides.items():
            if key in config_dict and isinstance(config_dict[key], dict):
                if isinstance(value, dict):
                    self._apply_overrides(config_dict[key], value)
                else:
                    config_dict[key] = value
            else:
                config_dict[key] = value
    
    def _dict_to_config(self, config_dict):
        """Convert dictionary to NLM configuration"""
        config = pynlm.createDefaultConfig()
        
        # Map dictionary keys to NLM config
        if "brain" in config_dict:
            brain_section = config_dict["brain"]
            if "neuron_count" in brain_section:
                config.set("brain.neuron_count", brain_section["neuron_count"])
            if "synapse_density" in brain_section:
                config.set("brain.synapse_density", brain_section["synapse_density"])
        
        if "plasticity" in config_dict:
            plasticity = config_dict["plasticity"]
            if "stdp" in plasticity and "learning_rate" in plasticity["stdp"]:
                config.set("plasticity.stdp.learning_rate", 
                          plasticity["stdp"]["learning_rate"])
        
        return config

# Usage example
manager = ConfigTemplateManager()

# Create config from learning template
learning_config = manager.create_config_from_template(
    "learning",
    {"brain.neuron_count": 5000, "plasticity.stdp.learning_rate": 0.05}
)

brain = pynlm.createBrain(learning_config)
```

## 2.2 Configuration Validation

```python
class ConfigValidator:
    """Validate NLM configurations for correctness and completeness"""
    
    def __init__(self):
        self.validation_rules = [
            self._validate_neuron_count,
            self._validate_plasticity_balance,
            self._validate_memory_requirements,
            self._validate_performance_parameters,
        ]
    
    def validate(self, config):
        """Validate a configuration and return issues"""
        issues = []
        
        for rule in self.validation_rules:
            rule_issues = rule(config)
            issues.extend(rule_issues)
        
        return issues
    
    def _validate_neuron_count(self, config):
        issues = []
        
        try:
            neuron_count = config.get("brain.neuron_count").value
            if neuron_count <= 0:
                issues.append("Neuron count must be positive")
            elif neuron_count > 1000000:
                issues.append("Neuron count too large - may cause memory issues")
        except:
            issues.append("Invalid or missing brain.neuron_count")
        
        return issues
    
    def _validate_plasticity_balance(self, config):
        issues = []
        
        # Check if any plasticity rules are enabled
        try:
            # Default all false if not present
            stdp_enabled = config.get("plasticity.stdp.enable").value if config.has("plasticity.stdp.enable") else False
            hebbian_enabled = config.get("plasticity.hebbian.enable").value if config.has("plasticity.hebbian.enable") else False
            structural_enabled = config.get("plasticity.structural.enable").value if config.has("plasticity.structural.enable") else False
            
            if not (stdp_enabled or hebbian_enabled or structural_enabled):
                issues.append("No plasticity rules enabled - brain won't learn")
        except:
            issues.append("Error checking plasticity settings")
        
        return issues
    
    def _validate_memory_requirements(self, config):
        issues = []
        
        try:
            neuron_count = config.get("brain.neuron_count").value
            
            # Estimate memory usage
            memory_estimate_mb = neuron_count * 100 / (1024 * 1024)  # Rough estimate
            
            if memory_estimate_mb > 2000:
                issues.append(f"High memory usage estimated: {memory_estimate_mb:.1f}MB")
        except:
            pass
        
        return issues
    
    def _validate_performance_parameters(self, config):
        issues = []
        
        try:
            # Check timestep compatibility
            timestep = config.get("simulation.timestep").value if config.has("simulation.timestep") else 0.001
            
            if timestep <= 0:
                issues.append("Timestep must be positive")
            elif timestep > 0.1:
                issues.append("Timestep too large - may affect accuracy")
        except:
            pass
        
        return issues

# Usage
validator = ConfigValidator()
config = pynlm.createDefaultConfig()
issues = validator.validate(config)

if issues:
    print("Configuration issues found:")
    for issue in issues:
        print(f"  - {issue}")
```

---

# 3. Performance Monitoring and Optimization

## 3.1 Real-time Performance Metrics

```python
import time
import threading
from typing import Dict, List, Callable
import statistics

class PerformanceMonitor:
    """Monitor and analyze NLM simulation performance"""
    
    def __init__(self, sampling_interval=0.1):
        self.sampling_interval = sampling_interval
        self.metrics = {
            'time': [],
            'neuron_count': [],
            'spike_rate': [],
            'memory_usage': [],
            'cpu_usage': [],
        }
        self.monitoring = False
        self.monitor_thread = None
        self.callbacks = []
    
    def start_monitoring(self, brain, world, agent=None):
        """Start real-time performance monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop, 
            args=(brain, world, agent),
            daemon=True
        )
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join()
            self.monitor_thread = None
    
    def _monitor_loop(self, brain, world, agent):
        """Main monitoring loop"""
        while self.monitoring:
            try:
                metrics = self._collect_metrics(brain, world, agent)
                self._record_metrics(metrics)
                self._trigger_callbacks(metrics)
                
                time.sleep(self.sampling_interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                break
    
    def _collect_metrics(self, brain, world, agent):
        """Collect performance metrics from NLM systems"""
        metrics = {}
        
        # Brain performance
        metrics['time'] = time.time()
        metrics['neuron_count'] = brain.getTotalNeuronCount()
        metrics['spike_rate'] = brain.getAverageFiringRate()
        metrics['active_neurons'] = brain.getActiveNeuronCount()
        metrics['total_spikes'] = brain.getTotalSpikeCount()
        metrics['e_i_ratio'] = brain.getExcitationInhibitionRatio()
        
        # World performance
        metrics['world_time'] = world.getSimulationTime()
        metrics['world_update_rate'] = 1.0 / 0.1  # Assuming 0.1s timesteps
        
        # Agent performance
        if agent:
            metrics['curiosity'] = agent.getCuriosityLevel()
            metrics['novelty'] = agent.getNoveltyLevel()
            metrics['prediction_error'] = agent.getPredictionError()
            metrics['neuromodulation_level'] = agent.getNeuromodulationLevel()
        
        # Memory estimation (simplified)
        metrics['memory_estimate_mb'] = (
            metrics['neuron_count'] * 0.1 +  # Neurons
            brain.getTotalSynapseCount() * 0.05 +  # Synapses
            world.getHeight() * world.getWidth() * 0.01  # World grid
        )
        
        return metrics
    
    def _record_metrics(self, metrics):
        """Record metrics for analysis"""
        for key, value in metrics.items():
            if key in self.metrics:
                self.metrics[key].append(value)
                
                # Keep only last hour of data
                max_points = int(3600 / self.sampling_interval)
                if len(self.metrics[key]) > max_points:
                    self.metrics[key] = self.metrics[key][-max_points:]
    
    def _trigger_callbacks(self, metrics):
        """Trigger registered callbacks with new metrics"""
        for callback in self.callbacks:
            try:
                callback(metrics)
            except Exception as e:
                print(f"Callback error: {e}")
    
    def add_callback(self, callback: Callable):
        """Add a callback function for metrics"""
        self.callbacks.append(callback)
    
    def get_statistics(self, metric_name, duration=None):
        """Get statistics for a metric"""
        if metric_name not in self.metrics or not self.metrics[metric_name]:
            return None
        
        data = self.metrics[metric_name]
        
        if duration is not None:
            # Filter by time
            cutoff_time = data[-1]['time'] - duration
            data = [d for d in data if d['time'] >= cutoff_time]
        
        values = [d['value'] if isinstance(d, dict) else d for d in data]
        
        if not values:
            return None
        
        return {
            'mean': statistics.mean(values),
            'median': statistics.median(values),
            'stdev': statistics.stdev(values) if len(values) > 1 else 0,
            'min': min(values),
            'max': max(values),
            'count': len(values),
        }
    
    def get_performance_report(self):
        """Generate a comprehensive performance report"""
        report = {
            'timestamp': time.time(),
            'metrics': {},
            'health_status': self._assess_health(),
            'recommendations': self._generate_recommendations(),
        }
        
        for metric_name in self.metrics:
            stats = self.get_statistics(metric_name)
            if stats:
                report['metrics'][metric_name] = stats
        
        return report
    
    def _assess_health(self):
        """Assess overall system health"""
        health = 'healthy'
        issues = []
        
        # Check for performance anomalies
        if 'spike_rate' in self.metrics:
            stats = self.get_statistics('spike_rate')
            if stats and (stats['mean'] < 0.01 or stats['stdev'] > stats['mean'] * 10):
                issues.append("Unstable spike rate")
        
        if 'memory_estimate_mb' in self.metrics:
            stats = self.get_statistics('memory_estimate_mb')
            if stats and stats['mean'] > 1000:
                issues.append("High memory usage")
        
        if issues:
            health = 'warning'
            if any("High memory" in issue for issue in issues):
                health = 'critical'
        
        return {
            'status': health,
            'issues': issues,
        }
    
    def _generate_recommendations(self):
        """Generate performance recommendations"""
        recommendations = []
        
        stats = self.get_statistics('memory_estimate_mb')
        if stats and stats['mean'] > 500:
            recommendations.append(
                "Consider reducing neuron count or using memory-efficient representations"
            )
        
        stats = self.get_statistics('spike_rate')
        if stats and stats['mean'] < 0.01:
            recommendations.append(
                "Brain activity is very low - consider increasing input or adjusting thresholds"
            )
        
        stats = self.get_statistics('e_i_ratio')
        if stats and (stats['mean'] < 0.5 or stats['mean'] > 2.0):
            recommendations.append(
                "Excitation-inhibition balance is off - may affect stability"
            )
        
        return recommendations

# Usage example
monitor = PerformanceMonitor(sampling_interval=0.05)

# Add custom callback
def on_performance_threshold(metrics):
    if metrics.get('memory_estimate_mb', 0) > 800:
        print("WARNING: High memory usage detected!")

monitor.add_callback(on_performance_threshold)

# Start monitoring (example)
# monitor.start_monitoring(brain, world, agent)

# Get current performance
# report = monitor.get_performance_report()
```

## 3.2 Simulation Optimization

```python
class SimulationOptimizer:
    """Optimize NLM simulations for performance and accuracy"""
    
    def __init__(self):
        self.optimization_rules = []
        self.performance_history = []
    
    def optimize(self, brain, world, agent, target_performance):
        """Optimize simulation based on target performance"""
        recommendations = []
        
        # Analyze current performance
        current_perf = self._analyze_performance(brain, world, agent)
        
        # Apply optimization rules
        for rule in self.optimization_rules:
            rule_recommendations = rule(current_perf, target_performance)
            recommendations.extend(rule_recommendations)
        
        # Apply optimizations
        for recommendation in recommendations:
            self._apply_recommendation(recommendation, brain, world, agent)
        
        return recommendations
    
    def _analyze_performance(self, brain, world, agent):
        """Analyze current simulation performance"""
        perf = {}
        
        # Speed analysis
        perf['speed'] = self._measure_speed(world)
        
        # Accuracy analysis
        perf['accuracy'] = self._measure_accuracy(brain, world, agent)
        
        # Resource usage
        perf['memory'] = self._estimate_memory(brain, world)
        perf['cpu'] = self._estimate_cpu(brain)
        
        # Stability analysis
        perf['stability'] = self._measure_stability(brain)
        
        return perf
    
    def _measure_speed(self, world):
        """Measure simulation speed"""
        # Implement speed measurement
        pass
    
    def _measure_accuracy(self, brain, world, agent):
        """Measure simulation accuracy"""
        pass
    
    def _estimate_memory(self, brain, world):
        """Estimate memory usage"""
        pass
    
    def _estimate_cpu(self, brain):
        """Estimate CPU usage"""
        pass
    
    def _measure_stability(self, brain):
        """Measure simulation stability"""
        pass
    
    def add_optimization_rule(self, rule):
        """Add an optimization rule"""
        self.optimization_rules.append(rule)
    
    def remove_optimization_rule(self, rule):
        """Remove an optimization rule"""
        if rule in self.optimization_rules:
            self.optimization_rules.remove(rule)
```

---

# 4. Advanced Learning Algorithms

## 4.1 Custom Plasticity Rules

```python
import math

class CustomPlasticityManager:
    """Manage advanced plasticity algorithms"""
    
    def __init__(self):
        self.plasticity_algorithms = {
            'stdp': self._stdp_algorithm,
            'hebbian': self._hebbian_algorithm,
            'reward': self._reward_modulated_algorithm,
            'adaptive': self._adaptive_plasticity_algorithm,
            'homeostatic': self._homeostatic_plasticity_algorithm,
        }
    
    def apply_custom_plasticity(self, synapse, pre_spike_time, post_spike_time, reward=0.0):
        """Apply multiple plasticity rules to a synapse"""
        weight = synapse.getWeight()
        
        # Apply STDP
        stdp_change = self._calculate_stdp(pre_spike_time, post_spike_time)
        weight += stdp_change * synapse.getWeight() * 0.01
        
        # Apply Hebbian learning (if both spikes occurred)
        if pre_spike_time and post_spike_time:
            hebbian_change = self._calculate_hebbian(pre_spike_time, post_spike_time)
            weight += hebbian_change * 0.001
        
        # Apply reward modulation
        if reward != 0.0:
            reward_change = self._calculate_reward_modulation(synapse, reward)
            weight += reward_change
        
        # Apply homeostatic regulation
        homeostatic_change = self._calculate_homeostatic(weight, pre_spike_time)
        weight += homeostatic_change
        
        # Apply adaptive plasticity
        adaptive_change = self._calculate_adaptive_plasticity(synapse, weight)
        weight += adaptive_change
        
        # Clamp weight
        weight = self._clamp_weight(weight)
        
        synapse.setWeight(weight)
        return weight
    
    def _calculate_stdp(self, pre_time, post_time):
        """Spike-Timing-Dependent Plasticity"""
        if pre_time and post_time:
            dt = post_time - pre_time
            if dt > 0:  # Post after pre = LTP
                return -0.1 * math.exp(-dt / 20.0)
            else:  # Pre after post = LTD
                return 0.1 * math.exp(dt / 20.0)
        return 0.0
    
    def _calculate_hebbian(self, pre_time, post_time):
        """Hebbian learning"""
        return 0.05 * (1.0 - abs((pre_time - post_time) / 50.0))
    
    def _calculate_reward_modulation(self, synapse, reward):
        """Reward-modulated plasticity"""
        eligibility = synapse.getEligibilityTrace()
        
        # Update eligibility trace
        if reward > 0:
            eligibility += 0.5
        else:
            eligibility -= 0.1
        
        synapse.setEligibilityTrace(eligibility)
        
        # Modulate weight based on reward
        return eligibility * reward * 0.02
    
    def _calculate_homeostatic(self, weight, current_time):
        """Homeostatic plasticity to maintain stability"""
        target_weight = 0.1
        deviation = weight - target_weight
        
        # Weaker synapses grow, stronger synapses shrink
        if abs(deviation) < 0.05:
            return 0.0
        
        return -0.01 * deviation * math.exp(-current_time / 1000.0)
    
    def _calculate_adaptive_plasticity(self, synapse, current_weight):
        """Adaptive plasticity based on activity"""
        history = synapse.getPreSpikeHistory()
        if len(history) < 2:
            return 0.0
        
        # Calculate recent activity
        recent_activity = len([t for t in history if t > current_time - 100.0])
        
        # Adapt learning rate based on activity
        activity_factor = min(1.0, recent_activity / 10.0)
        
        # Change based on activity
        return activity_factor * 0.01 * (0.5 - current_weight)
    
    def _clamp_weight(self, weight):
        """Clamp weight to valid range"""
        return max(-1.0, min(1.0, weight))

# Usage
plasticity_manager = CustomPlasticityManager()

# In brain step function
def custom_step_with_plasticity(self, step, time):
    # ... brain step logic ...
    
    # Apply custom plasticity
    for region in self.pImpl->regions:
        for pop in region->getPopulations():
            for neuron in pop->getNeurons():
                # Get spike times
                spike_times = neuron->getSpikeHistory()
                
                # Apply custom plasticity to synapses
                for synapse in neuron->getOutgoingSynapses():
                    plasticity_manager.apply_custom_plasticity(
                        synapse,
                        spike_times[-1] if spike_times else None,
                        self.pImpl->last_post_spike_time,
                        self.pImpl->last_reward
                    )
```

## 4.2 Advanced Learning Algorithms

```python
class AdvancedLearningAlgorithms:
    """Implement advanced learning algorithms for NLM"""
    
    def __init__(self):
        self.algorithms = {
            'bayesian': self._bayesian_learning,
            'reinforcement': self._reinforcement_learning,
            'meta': self._meta_learning,
            'transfer': self._transfer_learning,
        }
    
    def apply_bayesian_learning(self, brain, world, agent, experience):
        """Apply Bayesian learning to update beliefs"""
        # Update belief states
        self._update_beliefs(brain, experience)
        
        # Apply Bayesian inference
        self._apply_bayesian_inference(agent, experience)
        
        # Update action policies
        self._update_action_policy(agent, experience)
    
    def apply_reinforcement_learning(self, brain, world, agent, state, action, reward, next_state):
        """Apply reinforcement learning algorithms"""
        # Q-learning approach
        self._q_learning_update(agent, state, action, reward, next_state)
        
        # Actor-critic approach
        self._actor_critic_update(agent, state, action, reward)
        
        # Policy gradient
        self._policy_gradient_update(agent, state, action, reward)
    
    def apply_meta_learning(self, brain, world, agent):
        """Apply meta-learning to adapt learning rate"""
        # Monitor learning progress
        learning_progress = self._monitor_learning_progress(agent)
        
        # Adapt learning rate
        optimal_lr = self._calculate_optimal_learning_rate(learning_progress)
        
        # Update agent parameters
        agent.setLearningRate(optimal_lr)
    
    def apply_transfer_learning(self, source_brain, target_brain, transfer_strength=0.5):
        """Apply transfer learning from source to target brain"""
        # Extract features from source
        source_features = self._extract_features(source_brain)
        
        # Transfer to target
        self._transfer_features(target_brain, source_features, transfer_strength)
        
        # Fine-tune target
        self._fine_tune_target(target_brain)
    
    def _bayesian_learning(self, brain, experience):
        """Bayesian learning implementation"""
        pass
    
    def _reinforcement_learning(self, brain, experience):
        """Reinforcement learning implementation"""
        pass
    
    def _meta_learning(self, brain):
        """Meta-learning implementation"""
        pass
    
    def _transfer_learning(self, source, target):
        """Transfer learning implementation"""
        pass
```

---

# 5. Multi-Agent Systems

## 5.1 Agent Communication Protocols

```python
import json
import time
import threading
from typing import Dict, List, Callable
import queue

class AgentCommunicationHub:
    """Hub for multi-agent communication and coordination"""
    
    def __init__(self, agent_id, agents=None):
        self.agent_id = agent_id
        self.agents = agents or {}
        self.message_queue = queue.Queue()
        self.subscriptions = {}
        self.publishers = {}
        self.communication_channels = {}
        self.communication_history = []
        
        # Background threads
        self.message_processor = None
        self.communication_monitor = None
        
        # Communication protocols
        self.protocols = {
            'direct': self._direct_communication,
            'publish_subscribe': self._publish_subscribe_communication,
            'message_passing': self._message_passing_communication,
            'gossip': self._gossip_communication,
            'consensus': self._consensus_communication,
        }
    
    def register_agent(self, agent_id, agent):
        """Register an agent with the hub"""
        self.agents[agent_id] = agent
        
        # Initialize agent's communication
        if hasattr(agent, 'set_communication_hub'):
            agent.set_communication_hub(self)
    
    def unregister_agent(self, agent_id):
        """Unregister an agent"""
        if agent_id in self.agents:
            del self.agents[agent_id]
    
    def send_message(self, recipient_id, message, protocol='direct', priority=1):
        """Send a message to another agent"""
        message_data = {
            'sender': self.agent_id,
            'recipient': recipient_id,
            'content': message,
            'timestamp': time.time(),
            'protocol': protocol,
            'priority': priority,
        }
        
        # Route through protocol
        if protocol in self.protocols:
            self.protocols[protocol](message_data)
        else:
            raise ValueError(f"Unknown protocol: {protocol}")
    
    def broadcast_message(self, message, protocol='direct', recipients=None):
        """Broadcast a message to all agents or specified recipients"""
        if recipients is None:
            recipients = list(self.agents.keys())
        
        for recipient_id in recipients:
            self.send_message(recipient_id, message, protocol)
    
    def subscribe(self, topic, agent_id, callback=None):
        """Subscribe an agent to a communication topic"""
        if topic not in self.subscriptions:
            self.subscriptions[topic] = {}
        
        self.subscriptions[topic][agent_id] = callback
    
    def unsubscribe(self, topic, agent_id):
        """Unsubscribe an agent from a topic"""
        if topic in self.subscriptions and agent_id in self.subscriptions[topic]:
            del self.subscriptions[topic][agent_id]
    
    def publish(self, topic, message):
        """Publish a message to all subscribers of a topic"""
        if topic in self.subscriptions:
            for agent_id, callback in self.subscriptions[topic].items():
                if agent_id in self.agents:
                    message_data = {
                        'topic': topic,
                        'sender': self.agent_id,
                        'content': message,
                        'timestamp': time.time(),
                    }
                    
                    if callback:
                        callback(message_data)
                    else:
                        # Default callback - store message
                        self._store_message(agent_id, message_data)
    
    def get_message(self, timeout=1.0):
        """Get a message from the queue"""
        try:
            message = self.message_queue.get(timeout=timeout)
            return message
        except queue.Empty:
            return None
    
    def get_message_history(self, agent_id, limit=100):
        """Get message history for an agent"""
        return self.communication_history[-limit:]
    
    def start_communication(self):
        """Start communication background threads"""
        self.message_processor = threading.Thread(
            target=self._process_messages,
            daemon=True
        )
        self.message_processor.start()
        
        self.communication_monitor = threading.Thread(
            target=self._monitor_communication,
            daemon=True
        )
        self.communication_monitor.start()
    
    def stop_communication(self):
        """Stop communication background threads"""
        if self.message_processor:
            self.message_processor.join()
        
        if self.communication_monitor:
            self.communication_monitor.join()
    
    def _direct_communication(self, message_data):
        """Direct communication protocol"""
        recipient_id = message_data['recipient']
        
        if recipient_id in self.agents:
            self._store_message(recipient_id, message_data)
            
            # Deliver message
            if hasattr(self.agents[recipient_id], 'receive_message'):
                self.agents[recipient_id].receive_message(message_data)
    
    def _publish_subscribe_communication(self, message_data):
        """Publish-subscribe communication protocol"""
        topic = message_data.get('topic')
        if topic:
            self.publish(topic, message_data)
        else:
            # Fallback to direct
            self._direct_communication(message_data)
    
    def _message_passing_communication(self, message_data):
        """Message passing communication protocol"""
        # Implement message passing with acknowledgments
        message_data['status'] = 'sent'
        
        # Store for delivery tracking
        self._store_message(message_data['recipient'], message_data)
        
        # Deliver asynchronously
        threading.Thread(
            target=self._deliver_message_async,
            args=(message_data,)
        ).start()
    
    def _gossip_communication(self, message_data):
        """Gossip communication protocol for decentralized networks"""
        # Select random agents to forward message
        import random
        num_hops = min(3, len(self.agents) - 1)
        
        current_agent = self.agent_id
        for hop in range(num_hops):
            # Select random recipient from remaining agents
            possible_recipients = [
                aid for aid in self.agents.keys() if aid != current_agent
            ]
            
            if possible_recipients:
                next_recipient = random.choice(possible_recipients)
                self._direct_communication({
                    'sender': current_agent,
                    'recipient': next_recipient,
                    'content': message_data['content'],
                    'timestamp': message_data['timestamp'],
                    'gossip_hop': hop,
                })
                
                current_agent = next_recipient
    
    def _consensus_communication(self, message_data):
        """Consensus communication protocol for coordinated decisions"""
        # Collect opinions from agents
        opinions = {}
        for agent_id, agent in self.agents.items():
            if hasattr(agent, 'get_opinion_on'):
                opinions[agent_id] = agent.get_opinion_on(message_data)
        
        # Compute consensus
        consensus = self._compute_consensus(opinions)
        
        # Deliver consensus
        consensus_message = {
            'type': 'consensus',
            'topic': message_data.get('topic'),
            'content': consensus,
            'agreed_by': list(opinions.keys()),
            'timestamp': message_data['timestamp'],
        }
        
        self.broadcast_message(consensus_message)
    
    def _process_messages(self):
        """Background thread to process messages"""
        while True:
            try:
                message = self.get_message(timeout=0.1)
                if message:
                    self._handle_incoming_message(message)
            except Exception as e:
                print(f"Message processing error: {e}")
            
            time.sleep(0.01)
    
    def _monitor_communication(self):
        """Background thread to monitor communication quality"""
        while True:
            try:
                self._assess_communication_quality()
                time.sleep(5.0)  # Check every 5 seconds
            except Exception as e:
                print(f"Communication monitoring error: {e}")
    
    def _handle_incoming_message(self, message):
        """Handle an incoming message"""
        # Forward to agent's message handler
        agent_id = message.get('recipient')
        
        if agent_id in self.agents:
            agent = self.agents[agent_id]
            
            if hasattr(agent, 'on_message_received'):
                agent.on_message_received(message)
    
    def _store_message(self, agent_id, message_data):
        """Store message for delivery"""
        # Add delivery tracking
        message_data['delivered'] = False
        message_data['delivery_time'] = time.time()
        
        # Store in history
        self.communication_history.append(message_data)
        
        # Keep only recent messages
        if len(self.communication_history) > 10000:
            self.communication_history = self.communication_history[-5000:]
    
    def _deliver_message_async(self, message_data):
        """Asynchronously deliver a message"""
        recipient_id = message_data['recipient']
        
        if recipient_id in self.agents:
            agent = self.agents[recipient_id]
            
            if hasattr(agent, 'receive_message'):
                try:
                    agent.receive_message(message_data)
                    message_data['delivered'] = True
                except Exception as e:
                    print(f"Failed to deliver message to {recipient_id}: {e}")
    
    def _compute_consensus(self, opinions):
        """Compute consensus from agent opinions"""
        if not opinions:
            return None
        
        # Simple averaging for numeric opinions
        numeric_values = []
        for opinion in opinions.values():
            if isinstance(opinion, (int, float)):
                numeric_values.append(opinion)
        
        if numeric_values:
            return sum(numeric_values) / len(numeric_values)
        
        # Default to first opinion for non-numeric
        return list(opinions.values())[0]
    
    def _assess_communication_quality(self):
        """Assess communication quality metrics"""
        if not self.communication_history:
            return
        
        recent_messages = self.communication_history[-100:]
        
        metrics = {
            'delivery_rate': self._calculate_delivery_rate(recent_messages),
            'latency': self._calculate_average_latency(recent_messages),
            'message_volume': len(recent_messages),
        }
        
        # Store metrics
        if not hasattr(self, 'communication_metrics'):
            self.communication_metrics = []
        
        self.communication_metrics.append({
            'timestamp': time.time(),
            'metrics': metrics,
        })
        
        # Keep metrics manageable
        if len(self.communication_metrics) > 1000:
            self.communication_metrics = self.communication_metrics[-500:]
    
    def _calculate_delivery_rate(self, messages):
        """Calculate message delivery rate"""
        delivered = sum(1 for msg in messages if msg.get('delivered', False))
        total = len(messages)
        return delivered / total if total > 0 else 0.0
    
    def _calculate_average_latency(self, messages):
        """Calculate average message latency"""
        latencies = []
        
        for msg in messages:
            if 'delivery_time' in msg and 'timestamp' in msg:
                latency = msg['delivery_time'] - msg['timestamp']
                latencies.append(latency)
        
        return sum(latencies) / len(latencies) if latencies else 0.0

# Usage example
class SmartAgent:
    """Example agent that uses the communication hub"""
    
    def __init__(self, agent_id, nlm_brain):
        self.agent_id = agent_id
        self.brain = nlm_brain
        self.communication_hub = None
        self.knowledge = {}
    
    def set_communication_hub(self, hub):
        """Set the communication hub"""
        self.communication_hub = hub
    
    def receive_message(self, message):
        """Handle incoming message"""
        content = message.get('content', {})
        
        if isinstance(content, dict):
            if content.get('type') == 'consensus':
                # Process consensus decision
                self._process_consensus(content)
            else:
                # Process regular message
                self._process_message(message)
        else:
            # Process simple message
            self._process_message(message)
    
    def on_message_received(self, message):
        """Override this method for custom message handling"""
        print(f"Agent {self.agent_id} received message: {message}")
    
    def broadcast_knowledge(self, topic, knowledge):
        """Broadcast knowledge to other agents"""
        if self.communication_hub:
            self.communication_hub.publish(topic, knowledge)
    
    def request_consensus(self, topic, proposal):
        """Request consensus on a proposal"""
        if self.communication_hub:
            self.communication_hub.send_message(
                'consensus_hub',
                {
                    'type': 'proposal',
                    'topic': topic,
                    'proposal': proposal,
                    'proposer': self.agent_id,
                },
                protocol='consensus'
            )

# Create multi-agent system
class MultiAgentSystem:
    """Manage a system of interacting agents"""
    
    def __init__(self):
        self.agents = {}
        self.communication_hub = AgentCommunicationHub('system_hub')
    
    def add_agent(self, agent_id, agent):
        """Add an agent to the system"""
        self.agents[agent_id] = agent
        self.communication_hub.register_agent(agent_id, agent)
    
    def start_system(self):
        """Start the multi-agent system"""
        self.communication_hub.start_communication()
    
    def stop_system(self):
        """Stop the multi-agent system"""
        self.communication_hub.stop_communication()
    
    def run_simulation(self, steps):
        """Run the multi-agent simulation"""
        for step in range(steps):
            # Each agent performs its action
            for agent_id, agent in self.agents.items():
                self._step_agent(agent, step)
            
            # Agents communicate
            self._handle_communication(step)
    
    def _step_agent(self, agent, step):
        """Step a single agent"""
        # Get sensory input
        percept = agent.get_percept()
        
        # Process with brain
        agent.brain.step(step)
        
        # Decide action
        action = agent.decide_action()
        
        # Execute action
        agent.execute_action(action)
    
    def _handle_communication(self, step):
        """Handle agent communication for this step"""
        # Agents can exchange information
        for agent_id, agent in self.agents.items():
            if hasattr(agent, 'share_knowledge'):
                agent.share_knowledge(step)

# Usage example
# Create agents
agent1 = SmartAgent('agent1', pynlm.createBrain(pynlm.createDefaultConfig()))
agent2 = SmartAgent('agent2', pynlm.createBrain(pynlm.createDefaultConfig()))

# Create multi-agent system
mas = MultiAgentSystem()
mas.add_agent('agent1', agent1)
mas.add_agent('agent2', agent2)

# Start system
mas.start_system()

# Run simulation
mas.run_simulation(100)
```

---

# 6. Debugging and Visualization Tools

## 6.1 Advanced Debugging

```python
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict
import time

class NLMDebugger:
    """Advanced debugging tools for NLM simulations"""
    
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Debug data storage
        self.debug_data = {
            'steps': [],
            'neuron_states': [],
            'synapse_weights': [],
            'spike_patterns': [],
            'plastic_changes': [],
            'memory_states': [],
            'world_states': [],
            'agent_states': [],
        }
        
        # Debug visualizations
        self.visualizations = {
            'neuron_activity': None,
            'synapse_network': None,
            'spike_raster': None,
            'weight_distribution': None,
            'plastic_changes_over_time': None,
            'memory_traces': None,
            'world_dynamics': None,
            'agent_behavior': None,
        }
        
        # Debug helpers
        self.debug_helpers = {
            'anomaly_detector': self._detect_anomalies,
            'pattern_analyzer': self._analyze_patterns,
            'performance_profiler': self._profile_performance,
            'memory_tracker': self._track_memory_usage,
        }
    
    def step_debug(self, step, time, world_state=None, agent_state=None):
        """Record debug information for a simulation step"""
        # Record step information
        step_data = {
            'step': step,
            'time': time,
            'world_state': world_state,
            'agent_state': agent_state,
        }
        
        self.debug_data['steps'].append(step_data)
        
        # Record brain state
        self._record_brain_state(step)
        
        # Record synapse state
        self._record_synapse_state(step)
        
        # Record spike patterns
        self._record_spike_patterns(step)
        
        # Record plasticity changes
        self._record_plastic_changes(step)
        
        # Record memory states
        if self.agent:
            self._record_memory_states(step)
        
        # Record world state
        if self.world:
            self._record_world_state(step)
        
        # Record agent state
        if self.agent:
            self._record_agent_state(step)
    
    def _record_brain_state(self, step):
        """Record brain state information"""
        brain_state = {
            'step': step,
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'pending_spikes': self.brain.getPendingSpikeEventCount(),
        }
        
        self.debug_data['neuron_states'].append(brain_state)
    
    def _record_synapse_state(self, step):
        """Record synapse state information"""
        # Collect synapse statistics
        weights = []
        delays = []
        types = defaultdict(int)
        
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                weights.append(synapse.getWeight())
                delays.append(synapse.getDelay())
                types[synapse.getType()] += 1
        
        if weights:
            synapse_state = {
                'step': step,
                'mean_weight': np.mean(weights),
                'weight_std': np.std(weights),
                'weight_range': [np.min(weights), np.max(weights)],
                'mean_delay': np.mean(delays),
                'delay_std': np.std(delays),
                'type_distribution': dict(types),
            }
            
            self.debug_data['synapse_weights'].append(synapse_state)
    
    def _record_spike_patterns(self, step):
        """Record spike pattern information"""
        spike_patterns = {}
        
        for region in self.brain.getRegions():
            for population in region.getPopulations():
                # Record population spike activity
                neurons = population.getNeurons()
                firing_count = sum(1 for neuron in neurons if neuron.isFiring())
                
                if firing_count > 0:
                    population_id = population.getId()
                    spike_patterns[population_id] = {
                        'firing_count': firing_count,
                        'population_size': len(neurons),
                        'firing_rate': firing_count / len(neurons) if neurons else 0,
                    }
        
        self.debug_data['spike_patterns'].append({
            'step': step,
            'patterns': spike_patterns,
        })
    
    def _record_plastic_changes(self, step):
        """Record plasticity changes"""
        plastic_changes = []
        
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                # Record potential plasticity changes
                pre_history = synapse.getPreSpikeHistory()
                post_history = synapse.getPostSpikeHistory()
                
                if pre_history and post_history:
                    # Calculate recent plasticity
                    recent_pre = len([t for t in pre_history if t > step - 10])
                    recent_post = len([t for t in post_history if t > step - 10])
                    
                    if recent_pre > 0 or recent_post > 0:
                        change = {
                            'synapse_id': synapse.getId(),
                            'recent_pre_spikes': recent_pre,
                            'recent_post_spikes': recent_post,
                            'weight_change_potential': self._estimate_weight_change(synapse),
                        }
                        
                        plastic_changes.append(change)
        
        self.debug_data['plastic_changes'].append({
            'step': step,
            'changes': plastic_changes,
        })
    
    def _record_memory_states(self, step):
        """Record memory system states"""
        if self.agent and hasattr(self.agent, 'get_working_memory'):
            memory_state = {
                'step': step,
                'working_memory_content': self.agent.get_working_memory(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
            }
            
            self.debug_data['memory_states'].append(memory_state)
    
    def _record_world_state(self, step):
        """Record world state"""
        if self.world:
            world_state = {
                'step': step,
                'simulation_time': self.world.getSimulationTime(),
                'agent_position': self.world.getAgentBody() if hasattr(self.world, 'getAgentBody') else None,
                'object_count': len(self.world.getObjects()) if hasattr(self.world, 'getObjects') else 0,
            }
            
            self.debug_data['world_states'].append(world_state)
    
    def _record_agent_state(self, step):
        """Record agent state"""
        if self.agent:
            agent_state = {
                'step': step,
                'action_history': getattr(self.agent, 'get_action_history', lambda: [])(),
                'decision_patterns': self._analyze_decision_patterns(self.agent),
                'behavior_profile': self._profile_agent_behavior(self.agent),
            }
            
            self.debug_data['agent_states'].append(agent_state)
    
    def _analyze_anomalies(self):
        """Detect anomalies in simulation"""
        anomalies = []
        
        # Check for abnormal spike patterns
        if self.debug_data['spike_patterns']:
            for pattern in self.debug_data['spike_patterns']:
                for pop_id, data in pattern['patterns'].items():
                    if data['firing_rate'] > 0.5:  # Too many neurons firing
                        anomalies.append({
                            'type': 'high_firing_rate',
                            'population': pop_id,
                            'rate': data['firing_rate'],
                            'step': pattern['step'],
                        })
        
        # Check for weight explosions
        if self.debug_data['synapse_weights']:
            for synapse_state in self.debug_data['synapse_weights']:
                weight_range = synapse_state['weight_range']
                if weight_range[0] < -0.9 or weight_range[1] > 0.9:
                    anomalies.append({
                        'type': 'weight_extremes',
                        'weights': weight_range,
                        'step': synapse_state['step'],
                    })
        
        return anomalies
    
    def _analyze_patterns(self):
        """Analyze patterns in debug data"""
        patterns = {
            'firing_dynamics': self._analyze_firing_dynamics(),
            'weight_dynamics': self._analyze_weight_dynamics(),
            'spike_timing': self._analyze_spike_timing(),
        }
        
        return patterns
    
    def _profile_performance(self):
        """Profile simulation performance"""
        performance = {
            'speed': self._measure_speed(),
            'memory': self._measure_memory_usage(),
            'stability': self._measure_stability(),
        }
        
        return performance
    
    def _track_memory_usage(self):
        """Track memory usage over time"""
        # Implementation would track actual memory usage
        pass
    
    def create_debug_visualization(self, plot_type, **kwargs):
        """Create debug visualizations"""
        if plot_type == 'neuron_activity':
            self._create_neuron_activity_plot(**kwargs)
        elif plot_type == 'synapse_network':
            self._create_synapse_network_plot(**kwargs)
        elif plot_type == 'spike_raster':
            self._create_spike_raster_plot(**kwargs)
        elif plot_type == 'weight_distribution':
            self._create_weight_distribution_plot(**kwargs)
        elif plot_type == 'plastic_changes_over_time':
            self._create_plastic_changes_plot(**kwargs)
        
        return self.visualizations[plot_type]
    
    def _create_neuron_activity_plot(self, **kwargs):
        """Create neuron activity visualization"""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Total firing rate over time
        steps = [s['step'] for s in self.debug_data['neuron_states']]
        firing_rates = [s['average_firing_rate'] for s in self.debug_data['neuron_states']]
        
        axes[0, 0].plot(steps, firing_rates)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Rate (Hz)')
        axes[0, 0].set_title('Total Average Firing Rate')
        axes[0, 0].grid(True)
        
        # Plot 2: Excitation-inhibition ratio
        e_i_ratios = [s['e_i_ratio'] for s in self.debug_data['neuron_states']]
        
        axes[0, 1].plot(steps, e_i_ratios)
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('E/I Ratio')
        axes[0, 1].set_title('Excitation-Inhibition Balance')
        axes[0, 1].set_yscale('log')
        axes[0, 1].grid(True)
        
        # Plot 3: Active neurons over time
        active_neurons = [s['firing_neurons'] for s in self.debug_data['neuron_states']]
        
        axes[1, 0].plot(steps, active_neurons)
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Active Neurons')
        axes[1, 0].set_title('Currently Active Neurons')
        axes[1, 0].grid(True)
        
        # Plot 4: Total spikes over time
        total_spikes = [s['total_spikes'] for s in self.debug_data['neuron_states']]
        
        axes[1, 1].plot(steps, total_spikes)
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Total Spikes')
        axes[1, 1].set_title('Cumulative Spike Count')
        axes[1, 1].grid(True)
        
        plt.tight_layout()
        self.visualizations['neuron_activity'] = fig
        
        return fig
    
    def _create_synapse_network_plot(self, **kwargs):
        """Create synapse network visualization"""
        # This would create a network graph visualization
        pass
    
    def _create_spike_raster_plot(self, **kwargs):
        """Create spike raster visualization"""
        # This would create a spike raster plot
        pass
    
    def _create_weight_distribution_plot(self, **kwargs):
        """Create weight distribution visualization"""
        # This would create weight distribution histograms
        pass
    
    def _create_plastic_changes_plot(self, **kwargs):
        """Create plasticity changes visualization"""
        # This would show plasticity changes over time
        pass
    
    def save_debug_data(self, filepath):
        """Save debug data to file"""
        import json
        
        # Convert numpy arrays to lists for JSON serialization
        serializable_data = self._make_serializable(self.debug_data)
        
        with open(filepath, 'w') as f:
            json.dump(serializable_data, f, indent=2)
    
    def load_debug_data(self, filepath):
        """Load debug data from file"""
        import json
        
        with open(filepath, 'r') as f:
            self.debug_data = json.load(f)
    
    def _make_serializable(self, data):
        """Convert data to JSON-serializable format"""
        if isinstance(data, dict):
            return {key: self._make_serializable(value) for key, value in data.items()}
        elif isinstance(data, list):
            return [self._make_serializable(item) for item in data]
        elif isinstance(data, np.ndarray):
            return data.tolist()
        elif isinstance(data, (np.int64, np.int32)):
            return int(data)
        elif isinstance(data, (np.float64, np.float32)):
            return float(data)
        else:
            return data
    
    def export_for_analysis(self, output_dir):
        """Export debug data for external analysis"""
        import os
        
        os.makedirs(output_dir, exist_ok=True)
        
        # Save main debug data
        self.save_debug_data(os.path.join(output_dir, 'debug_data.json'))
        
        # Create analysis scripts
        self._create_analysis_scripts(output_dir)
        
        # Create summary report
        self._create_summary_report(output_dir)
    
    def _create_analysis_scripts(self, output_dir):
        """Create Python scripts for analyzing debug data"""
        analysis_script = '''
import json
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

def load_debug_data(filepath):
    with open(filepath, 'r') as f:
        return json.load(f)

def analyze_firing_dynamics(data):
    """Analyze firing dynamics"""
    steps = [s['step'] for s in data['neuron_states']]
    firing_rates = [s['average_firing_rate'] for s in data['neuron_states']]
    
    # Calculate statistics
    mean_rate = np.mean(firing_rates)
    std_rate = np.std(firing_rates)
    
    # Find significant events
    events = []
    for i, (step, rate) in enumerate(zip(steps, firing_rates)):
        if rate > mean_rate + 2 * std_rate:
            events.append({'step': step, 'rate': rate, 'type': 'burst'})
        elif rate < mean_rate - std_rate:
            events.append({'step': step, 'rate': rate, 'type': 'quiescence'})
    
    return {
        'mean_rate': mean_rate,
        'std_rate': std_rate,
        'events': events,
        'duration': len(steps),
    }

def analyze_weight_distribution(data):
    """Analyze synapse weight distribution"""
    all_weights = []
    for synapse_state in data['synapse_weights']:
        weights = []
        if 'weight_range' in synapse_state:
            weights.append(synapse_state['weight_range'][0])
            weights.append(synapse_state['weight_range'][1])
            if 'mean_weight' in synapse_state:
                weights.append(synapse_state['mean_weight'])
        
        all_weights.extend(weights)
    
    return {
        'mean': np.mean(all_weights),
        'std': np.std(all_weights),
        'min': np.min(all_weights),
        'max': np.max(all_weights),
        'range': np.max(all_weights) - np.min(all_weights),
    }

# Run analysis
if __name__ == "__main__":
    data = load_debug_data('debug_data.json')
    
    firing_analysis = analyze_firing_dynamics(data)
    weight_analysis = analyze_weight_distribution(data)
    
    print("Firing Dynamics Analysis:")
    print(f"  Mean firing rate: {firing_analysis['mean_rate']:.3f} Hz")
    print(f"  Std firing rate: {firing_analysis['std_rate']:.3f} Hz")
    print(f"  Number of events: {len(firing_analysis['events'])}")
    
    print("\\nWeight Distribution Analysis:")
    print(f"  Mean weight: {weight_analysis['mean']:.3f}")
    print(f"  Std weight: {weight_analysis['std']:.3f}")
    print(f"  Min weight: {weight_analysis['min']:.3f}")
    print(f"  Max weight: {weight_analysis['max']:.3f}")
'''
        
        with open(os.path.join(output_dir, 'analyze_debug.py'), 'w') as f:
            f.write(analysis_script)
    
    def _create_summary_report(self, output_dir):
        """Create a summary report of debug data"""
        report = {
            'simulation_summary': {
                'total_steps': len(self.debug_data['steps']),
                'total_neurons': self.debug_data['neuron_states'][-1]['total_neurons'] if self.debug_data['neuron_states'] else 0,
                'final_firing_rate': self.debug_data['neuron_states'][-1]['average_firing_rate'] if self.debug_data['neuron_states'] else 0,
                'total_spikes': self.debug_data['neuron_states'][-1]['total_spikes'] if self.debug_data['neuron_states'] else 0,
            },
            'performance_metrics': self._calculate_performance_metrics(),
            'anomalies': self._detect_anomalies(),
            'patterns': self._analyze_patterns(),
        }
        
        import json
        with open(os.path.join(output_dir, 'summary_report.json'), 'w') as f:
            json.dump(report, f, indent=2)
    
    def _calculate_performance_metrics(self):
        """Calculate performance metrics"""
        metrics = {
            'convergence_rate': self._calculate_convergence_rate(),
            'stability_index': self._calculate_stability_index(),
            'efficiency_score': self._calculate_efficiency_score(),
        }
        
        return metrics
    
    def _calculate_convergence_rate(self):
        """Calculate how quickly the simulation converges"""
        if len(self.debug_data['neuron_states']) < 10:
            return 0.0
        
        # Calculate rate of change in firing rate
        firing_rates = [s['average_firing_rate'] for s in self.debug_data['neuron_states']]
        
        # Find stabilization point
        for i in range(10, len(firing_rates)):
            recent_std = np.std(firing_rates[i-10:i+1])
            if recent_std < 0.01:  # Low variation
                return i / len(firing_rates)
        
        return 1.0  # Never stabilized
    
    def _calculate_stability_index(self):
        """Calculate stability index"""
        if len(self.debug_data['neuron_states']) < 20:
            return 0.0
        
        firing_rates = [s['average_firing_rate'] for s in self.debug_data['neuron_states']]
        
        # Calculate autocorrelation
        if len(firing_rates) > 10:
            autocorr = np.corrcoef(firing_rates[:-10], firing_rates[10:])[0, 1]
            return max(0, 1 - abs(autocorr))
        
        return 0.0
    
    def _calculate_efficiency_score(self):
        """Calculate efficiency score"""
        if not self.debug_data['neuron_states']:
            return 0.0
        
        total_steps = len(self.debug_data['steps'])
        final_spikes = self.debug_data['neuron_states'][-1]['total_spikes']
        
        # Calculate spikes per step
        return final_spikes / total_steps if total_steps > 0 else 0.0
```
        
        with open(os.path.join(output_dir, 'analysis_script.py'), 'w') as f:
            f.write(analysis_script)
```

---

# 7. Hardware Acceleration

## 7.1 SIMD Optimization

```python
class SIMDOptimizer:
    """Optimize NLM operations for SIMD (Single Instruction Multiple Data)"""
    
    def __init__(self):
        self.supported_instructions = {
            'SSE': 128,
            'AVX': 256,
            'AVX2': 512,
        }
    
    def optimize_neuron_step(self, neurons, current_time, dt):
        """Optimize neuron stepping for SIMD"""
        # Group neurons by type for better optimization
        neuron_groups = self._group_neurons_by_type(neurons)
        
        # Process in SIMD chunks
        results = []
        
        for group_name, group_neurons in neuron_groups.items():
            # Optimize based on neuron type
            if group_name == 'excitatory':
                results.extend(self._simd_process_excitatory(group_neurons, current_time, dt))
            elif group_name == 'inhibitory':
                results.extend(self._simd_process_inhibitory(group_neurons, current_time, dt))
            elif group_name == 'sensory':
                results.extend(self._simd_process_sensory(group_neurons, current_time, dt))
            elif group_name == 'motor':
                results.extend(self._simd_process_motor(group_neurons, current_time, dt))
        
        return results
    
    def _group_neurons_by_type(self, neurons):
        """Group neurons by type for SIMD optimization"""
        groups = defaultdict(list)
        
        for neuron in neurons:
            neuron_type = neuron.getType()
            if neuron_type == NeuronType.Excitatory:
                groups['excitatory'].append(neuron)
            elif neuron_type == NeuronType.Inhibitory:
                groups['inhibitory'].append(neuron)
            elif neuron_type == NeuronType.Sensory:
                groups['sensory'].append(neuron)
            elif neuron_type == NeuronType.Motor:
                groups['motor'].append(neuron)
            else:
                groups['internal'].append(neuron)
        
        return groups
    
    def _simd_process_excitatory(self, neurons, current_time, dt):
        """Process excitatory neurons with SIMD"""
        results = []
        
        # Batch process neurons
        batch_size = 8  # For AVX
        
        for i in range(0, len(neurons), batch_size):
            batch = neurons[i:i + batch_size]
            
            # SIMD operations
            for neuron in batch:
                # Apply LIF dynamics
                result = self._simd_lif_step(neuron, current_time, dt)
                results.append(result)
        
        return results
    
    def _simd_lif_step(self, neuron, current_time, dt):
        """SIMD-optimized LIF step"""
        state = neuron.getState()
        
        # Vectorized operations
        v = state.membranePotential
        v_rest = state.restingPotential
        threshold = state.threshold
        
        # Leaky integration
        v += dt * (v_rest - v) * (1.0 / state.leakConductance)
        
        # Check for spike
        spiked = v > threshold
        
        # Reset if spiked
        if spiked:
            v = state.resetPotential
        
        # Update neuron
        neuron.setMembranePotential(v)
        
        return {
            'neuron_id': neuron.getId(),
            'spiked': spiked,
            'membrane_potential': v,
        }
    
    def optimize_synapse_updates(self, synapses, pre_spikes, current_time):
        """Optimize synapse updates for SIMD"""
        # Group synapses by type and delay
        synapse_groups = self._group_synapses_for_simd(synapses, pre_spikes)
        
        # Process groups in parallel
        results = []
        
        for group_name, group_synapses in synapse_groups.items():
            results.extend(self._simd_process_synapse_group(group_synapses, current_time))
        
        return results
    
    def _group_synapses_for_simd(self, synapses, pre_spikes):
        """Group synapses for SIMD processing"""
        groups = defaultdict(list)
        
        current_time = time.time()
        
        for synapse in synapses:
            # Check if synapse should spike
            should_spike = self._should_spike(synapse, pre_spikes, current_time)
            
            if should_spike:
                # Group by delay for parallel processing
                delay = synapse.getDelay()
                delay_group = delay // 4  # Group delays in 4-step buckets
                groups[f'delay_{delay_group}'].append(synapse)
        
        return groups
    
    def _simd_process_synapse_group(self, synapses, current_time):
        """Process a group of synapses with SIMD"""
        results = []
        
        # Batch process synapses
        batch_size = 16  # For AVX2
        
        for i in range(0, len(synapses), batch_size):
            batch = synapses[i:i + batch_size]
            
            # SIMD operations
            for synapse in batch:
                # Apply synaptic dynamics
                result = self._simd_synapse_step(synapse, current_time)
                results.append(result)
        
        return results
    
    def _simd_synapse_step(self, synapse, current_time):
        """SIMD-optimized synapse step"""
        # Get synapse properties
        weight = synapse.getWeight()
        delay = synapse.getDelay()
        syn_type = synapse.getType()
        
        # Apply synaptic transmission
        if delay <= 1:  # No delay
            # Simple transmission
            transmission = weight
        else:
            # Delayed transmission (simplified)
            transmission = weight * 0.9 ** (delay - 1)
        
        # Apply synaptic type effects
        if syn_type == SynapseType.Excitatory:
            transmission *= 1.2
        elif syn_type == SynapseType.Inhibitory:
            transmission *= 0.8
        elif syn_type == SynapseType.GapJunction:
            transmission *= 1.5
        
        return {
            'synapse_id': synapse.getId(),
            'transmission': transmission,
            'delay': delay,
        }
```

## 7.2 GPU Acceleration

```python
class GPUEnabledNLM:
    """Enable GPU acceleration for NLM simulations"""
    
    def __init__(self):
        self.gpu_available = self._check_gpu_availability()
        self.cuda_available = self._check_cuda_availability()
        
        if self.gpu_available:
            import cupy as cp
            self.cp = cp
            self.use_gpu = True
        else:
            self.use_gpu = False
    
    def _check_gpu_availability(self):
        """Check if GPU is available"""
        try:
            import cupy as cp
            return True
        except ImportError:
            return False
    
    def _check_cuda_availability(self):
        """Check if CUDA is available"""
        try:
            import cupy as cp
            return cp.cuda.runtime.getDeviceCount() > 0
        except:
            return False
    
    def simulate_neuron_dynamics_gpu(self, neurons, time_steps, dt):
        """Run neuron dynamics simulation on GPU"""
        if not self.use_gpu:
            raise RuntimeError("GPU acceleration not available")
        
        # Transfer neuron data to GPU
        gpu_neurons = self._transfer_to_gpu(neurons)
        
        # Run simulation on GPU
        results = self._gpu_simulate(gpu_neurons, time_steps, dt)
        
        # Transfer results back to CPU
        cpu_results = self._transfer_to_cpu(results)
        
        return cpu_results
    
    def _transfer_to_gpu(self, neurons):
        """Transfer neuron data to GPU"""
        # Convert neuron states to CuPy arrays
        neuron_ids = self.cp.array([neuron.getId() for neuron in neurons])
        neuron_types = self.cp.array([neuron.getType().value for neuron in neurons])
        
        # Allocate arrays for neuron states
        membrane_potentials = self.cp.zeros(len(neurons), dtype=self.cp.float32)
        thresholds = self.cp.zeros(len(neurons), dtype=self.cp.float32)
        resting_potentials = self.cp.zeros(len(neurons), dtype=self.cp.float32)
        
        for i, neuron in enumerate(neurons):
            state = neuron.getState()
            membrane_potentials[i] = state.membranePotential
            thresholds[i] = state.threshold
            resting_potentials[i] = state.restingPotential
        
        # Create GPU neuron objects
        gpu_neurons = {
            'ids': neuron_ids,
            'types': neuron_types,
            'membrane_potentials': membrane_potentials,
            'thresholds': thresholds,
            'resting_potentials': resting_potentials,
            'spikes': self.cp.zeros(len(neurons), dtype=bool),
        }
        
        return gpu_neurons
    
    def _gpu_simulate(self, gpu_neurons, time_steps, dt):
        """Run GPU simulation"""
        # Kernel for neuron dynamics
        kernel_code = """
        extern "C" __global__ void neuron_dynamics(
            float* membrane_potentials,
            float* thresholds,
            float* resting_potentials,
            bool* spikes,
            float dt,
            int n_neurons
        ) {
            int i = blockIdx.x * blockDim.x + threadIdx.x;
            if (i < n_neurons) {
                // Leaky integrate-and-fire dynamics
                float v = membrane_potentials[i];
                float v_rest = resting_potentials[i];
                float threshold = thresholds[i];
                
                // Leaky integration
                v += dt * (v_rest - v) * 0.01f;  // Simplified leak
                
                // Check for spike
                if (v > threshold) {
                    v = -70.0f;  // Reset potential
                    spikes[i] = true;
                }
                
                membrane_potentials[i] = v;
            }
        }
        """
        
        # Compile and run kernel
        module = self.cp.RawModule(code=kernel_code)
        kernel = module.get_function("neuron_dynamics")
        
        # Launch kernel
        n_neurons = len(gpu_neurons['membrane_potentials'])
        threads_per_block = 256
        blocks_per_grid = (n_neurons + threads_per_block - 1) // threads_per_block
        
        kernel((blocks_per_grid,), (threads_per_block,),
               (gpu_neurons['membrane_potentials'],
                gpu_neurons['thresholds'],
                gpu_neurons['resting_potentials'],
                gpu_neurons['spikes'],
                dt,
                n_neurons))
        
        return gpu_neurons
    
    def _transfer_to_cpu(self, gpu_data):
        """Transfer GPU data back to CPU"""
        cpu_neurons = []
        
        for i, neuron_id in enumerate(gpu_data['ids']):
            # Get spike information
            spiked = gpu_data['spikes'][i]
            
            # Create CPU neuron (placeholder - would integrate with actual CPU neurons)
            cpu_neuron = {
                'id': int(neuron_id),
                'spiked': bool(spiked),
                'membrane_potential': float(gpu_data['membrane_potentials'][i]),
            }
            
            cpu_neurons.append(cpu_neuron)
        
        return cpu_neurons
```

---

# 8. Research and Development Features

## 8.1 Experiment Framework

```python
import json
import time
import statistics
from typing import Dict, List, Callable
import pandas as pd

class NLMSimulationExperiment:
    """Framework for running NLM experiments"""
    
    def __init__(self, name):
        self.name = name
        self.results = {}
        self.parameters = {}
        self.histories = {}
        self.start_time = None
        self.end_time = None
    
    def set_parameter(self, key, value):
        """Set a simulation parameter"""
        self.parameters[key] = value
    
    def get_parameter(self, key, default=None):
        """Get a simulation parameter"""
        return self.parameters.get(key, default)
    
    def run(self, simulate_function, num_replicates=1):
        """Run the experiment"""
        self.start_time = time.time()
        
        results = []
        
        for replicate in range(num_replicates):
            print(f"Running replicate {replicate + 1}/{num_replicates}")
            
            # Create fresh simulation
            replicate_result = self._run_replicate(simulate_function, replicate)
            results.append(replicate_result)
            
            # Save intermediate results
            self._save_intermediate_results(results)
        
        self.end_time = time.time()
        
        # Analyze results
        self._analyze_results(results)
        
        # Generate report
        self._generate_report()
        
        return results
    
    def _run_replicate(self, simulate_function, replicate):
        """Run a single replicate of the simulation"""
        # Create a copy of parameters for this replicate
        replicate_params = self.parameters.copy()
        
        # Add replicate-specific parameters
        replicate_params['replicate'] = replicate
        
        # Run simulation
        result = simulate_function(replicate_params)
        
        # Process and store results
        processed_result = self._process_result(result, replicate)
        
        # Store history
        self._store_history(result, replicate)
        
        return processed_result
    
    def _process_result(self, result, replicate):
        """Process simulation result"""
        # Extract key metrics
        processed = {
            'replicate': replicate,
            'timestamp': time.time(),
        }
        
        # Extract brain metrics
        if 'brain' in result:
            brain = result['brain']
            processed.update({
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'neuron_count': brain.getTotalNeuronCount(),
            })
        
        # Extract agent metrics
        if 'agent' in result:
            agent = result['agent']
            processed.update({
                'final_curiosity': agent.getCuriosityLevel(),
                'final_novelty': agent.getNoveltyLevel(),
                'final_prediction_error': agent.getPredictionError(),
            })
        
        # Extract performance metrics
        if 'performance' in result:
            processed.update(result['performance'])
        
        return processed
    
    def _store_history(self, result, replicate):
        """Store simulation history"""
        for key, value in result.items():
            if key not in self.histories:
                self.histories[key] = []
            
            self.histories[key].append(value)
    
    def _save_intermediate_results(self, results):
        """Save intermediate results to disk"""
        import os
        
        os.makedirs('experiment_results', exist_ok=True)
        
        # Save current results
        with open(f'experiment_results/{self.name}_intermediate.json', 'w') as f:
            json.dump({
                'name': self.name,
                'parameters': self.parameters,
                'results': results,
                'timestamp': time.time(),
            }, f, indent=2)
    
    def _analyze_results(self, results):
        """Analyze experimental results"""
        # Convert to DataFrame for analysis
        df = pd.DataFrame(results)
        
        # Calculate statistics
        self.results['statistics'] = {}
        
        for column in df.columns:
            if column not in ['replicate', 'timestamp']:
                try:
                    values = df[column].dropna()
                    if len(values) > 1:
                        self.results['statistics'][column] = {
                            'mean': values.mean(),
                            'std': values.std(),
                            'min': values.min(),
                            'max': values.max(),
                            'median': values.median(),
                        }
                except:
                    pass
        
        # Statistical tests
        self._run_statistical_tests(df)
        
        # Effect size calculations
        self._calculate_effect_sizes(df)
    
    def _run_statistical_tests(self, df):
        """Run statistical tests on results"""
        self.results['statistics_tests'] = {}
        
        # T-tests for key metrics
        key_metrics = ['final_firing_rate', 'total_spikes', 'e_i_ratio']
        
        for metric in key_metrics:
            if metric in df.columns:
                values = df[metric].dropna()
                if len(values) > 2:
                    # One-sample t-test against mean of all values
                    from scipy import stats
                    t_stat, p_value = stats.ttest_1samp(values, values.mean())
                    
                    self.results['statistics_tests'][metric] = {
                        't_statistic': t_stat,
                        'p_value': p_value,
                        'significant': p_value < 0.05,
                    }
    
    def _calculate_effect_sizes(self, df):
        """Calculate effect sizes for comparisons"""
        self.results['effect_sizes'] = {}
        
        # Calculate Cohen's d for key metrics
        for metric in ['final_firing_rate', 'total_spikes']:
            if metric in df.columns:
                values = df[metric].dropna()
                if len(values) > 1:
                    mean = values.mean()
                    std = values.std()
                    
                    # Effect size relative to standard deviation of means
                    effect_size = mean / std if std > 0 else 0
                    
                    self.results['effect_sizes'][metric] = {
                        'cohens_d': effect_size,
                        'interpretation': self._interpret_effect_size(effect_size),
                    }
    
    def _interpret_effect_size(self, d):
        """Interpret Cohen's d effect size"""
        if abs(d) < 0.2:
            return 'negligible'
        elif abs(d) < 0.5:
            return 'small'
        elif abs(d) < 0.8:
            return 'medium'
        else:
            return 'large'
    
    def _generate_report(self):
        """Generate experiment report"""
        report = {
            'experiment_name': self.name,
            'parameters': self.parameters,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration_seconds': self.end_time - self.start_time,
            'number_of_replicates': len([r for r in self.results.get('statistics', {}).keys()]),
            'statistics': self.results.get('statistics', {}),
            'statistical_tests': self.results.get('statistical_tests', {}),
            'effect_sizes': self.results.get('effect_sizes', {}),
        }
        
        # Save report
        with open(f'experiment_results/{self.name}_report.json', 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        # Generate summary
        self._generate_summary_report(report)
    
    def _generate_summary_report(self, report):
        """Generate a human-readable summary report"""
        summary = f"""
# NLM Experiment: {self.name}

## Simulation Summary
- **Duration**: {report['duration_seconds']:.1f} seconds
- **Number of Replicates**: {report['number_of_replicates']}
- **Parameters**: {json.dumps(report['parameters'], indent=2)}

## Key Results
"""
        
        for metric, stats in report['statistics'].items():
            summary += f"\n**{metric}**:\n"
            summary += f"  - Mean: {stats['mean']:.3f}\n"
            summary += f"  - Std: {stats['std']:.3f}\n"
            summary += f"  - Min: {stats['min']:.3f}\n"
            summary += f"  - Max: {stats['max']:.3f}\n"
        
        summary += "\n## Statistical Significance\n"
        for metric, test in report['statistical_tests'].items():
            summary += f"\n**{metric}**:\n"
            summary += f"  - T-statistic: {test['t_statistic']:.3f}\n"
            summary += f"  - P-value: {test['p_value']:.3f}\n"
            summary += f"  - Significant: {test['significant']}\n"
        
        summary += "\n## Effect Sizes\n"
        for metric, effect in report['effect_sizes'].items():
            summary += f"\n**{metric}**:\n"
            summary += f"  - Cohen's d: {effect['cohens_d']:.3f}\n"
            summary += f"  - Interpretation: {effect['interpretation']}\n"
        
        # Save summary
        with open(f'experiment_results/{self.name}_summary.md', 'w') as f:
            f.write(summary)
    
    def get_results(self):
        """Get experiment results"""
        return self.results
    
    def get_history(self, key):
        """Get history for a specific key"""
        return self.histories.get(key, [])
    
    def export_for_analysis(self, output_dir):
        """Export experiment data for external analysis"""
        import os
        
        os.makedirs(output_dir, exist_ok=True)
        
        # Export history
        for key, data in self.histories.items():
            with open(os.path.join(output_dir, f"{key}.csv"), 'w') as f:
                import csv
                if data and isinstance(data[0], dict):
                    # Write as CSV
                    writer = csv.DictWriter(f, fieldnames=data[0].keys())
                    writer.writeheader()
                    writer.writerows(data)
        
        # Export parameters
        with open(os.path.join(output_dir, 'parameters.json'), 'w') as f:
            json.dump(self.parameters, f, indent=2)
    
    def run_parameter_sweep(self, parameter_space, base_parameters=None):
        """Run parameter sweep over a space of parameters"""
        if base_parameters is None:
            base_parameters = {}
        
        # Generate all parameter combinations
        parameter_combinations = self._generate_parameter_combinations(
            parameter_space, base_parameters
        )
        
        # Run experiments
        all_results = []
        
        for params in parameter_combinations:
            print(f"Running with parameters: {params}")
            
            # Update experiment parameters
            original_params = self.parameters.copy()
            self.parameters.update(params)
            
            # Run experiment
            result = self.run(lambda p: self._run_simulation(p, params))
            
            # Restore original parameters
            self.parameters = original_params
            
            all_results.extend(result)
        
        return all_results
    
    def _generate_parameter_combinations(self, parameter_space, base_parameters):
        """Generate all combinations of parameters"""
        import itertools
        
        combinations = []
        
        # Get parameter names and values
        param_names = list(parameter_space.keys())
        param_values = list(parameter_space.values())
        
        # Generate all combinations
        for values in itertools.product(*param_values):
            params = dict(zip(param_names, values))
            params.update(base_parameters)
            combinations.append(params)
        
        return combinations
    
    def _run_simulation(self, params, override_params):
        """Run simulation with parameters"""
        # Update parameters
        self.parameters.update(override_params)
        
        # Run single simulation
        return self.run(lambda p: self._simple_simulate(p))
    
    def _simple_simulate(self, params):
        """Simple simulation function for testing"""
        # Create NLM components
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        if 'brain.neuron_count' in params:
            config.set('brain.neuron_count', params['brain.neuron_count'])
        
        # Create simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(params.get('simulation_steps', 100)):
            brain.step(step)
        
        return {
            'brain': brain,
            'performance': {
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
            }
        }
```

---

# 9. Advanced Configuration and Deployment

## 9.1 Distributed Computing

```python
import multiprocessing
import threading
import queue
import time
from typing import List, Dict, Callable

class DistributedNLM:
    """Enable distributed computing for large-scale NLM simulations"""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or multiprocessing.cpu_count()
        self.workers = []
        self.task_queue = queue.Queue()
        self.result_queue = queue.Queue()
        self.is_running = False
    
    def start_workers(self):
        """Start worker processes"""
        self.is_running = True
        
        for i in range(self.num_workers):
            worker = multiprocessing.Process(
                target=self._worker_loop,
                args=(i,)
            )
            worker.daemon = True
            worker.start()
            self.workers.append(worker)
    
    def stop_workers(self):
        """Stop worker processes"""
        self.is_running = False
        
        # Send termination signal to all workers
        for _ in range(self.num_workers):
            self.task_queue.put(('terminate', None, None))
        
        # Wait for workers to finish
        for worker in self.workers:
            worker.join()
    
    def submit_task(self, task, priority=0):
        """Submit a task to the distributed system"""
        self.task_queue.put(('task', task, priority))
    
    def submit_simulation(self, simulation_params, agent_id):
        """Submit a simulation task"""
        task = {
            'type': 'simulation',
            'params': simulation_params,
            'agent_id': agent_id,
            'timestamp': time.time(),
        }
        
        self.submit_task(task)
    
    def submit_analysis(self, data, analysis_function):
        """Submit analysis task"""
        task = {
            'type': 'analysis',
            'data': data,
            'function': analysis_function,
        }
        
        self.submit_task(task)
    
    def _worker_loop(self, worker_id):
        """Worker process loop"""
        print(f"Worker {worker_id} started")
        
        while self.is_running:
            try:
                # Get task
                priority, task, data = self.task_queue.get(timeout=0.1)
                
                if priority == 'terminate':
                    break
                
                elif priority == 'task':
                    self._process_task(task, data)
                
            except queue.Empty:
                continue
            except Exception as e:
                print(f"Worker {worker_id} error: {e}")
                continue
        
        print(f"Worker {worker_id} stopped")
    
    def _process_task(self, task, priority):
        """Process a task"""
        task_type = task['type']
        
        if task_type == 'simulation':
            self._process_simulation_task(task)
        elif task_type == 'analysis':
            self._process_analysis_task(task)
    
    def _process_simulation_task(self, task):
        """Process a simulation task"""
        # Import here to avoid issues with multiprocessing
        import pynlm
        
        params = task['params']
        agent_id = task['agent_id']
        
        try:
            # Create simulation
            config = pynlm.createDefaultConfig()
            
            # Apply parameters
            if 'brain.neuron_count' in params:
                config.set('brain.neuron_count', params['brain.neuron_count'])
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            steps = params.get('simulation_steps', 100)
            
            for step in range(steps):
                brain.step(step)
            
            # Prepare result
            result = {
                'agent_id': agent_id,
                'success': True,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'timestamp': time.time(),
            }
            
            # Send result back
            self.result_queue.put(('result', result))
            
        except Exception as e:
            # Send error result
            result = {
                'agent_id': agent_id,
                'success': False,
                'error': str(e),
                'timestamp': time.time(),
            }
            
            self.result_queue.put(('result', result))
    
    def _process_analysis_task(self, task):
        """Process an analysis task"""
        try:
            data = task['data']
            analysis_function = task['function']
            
            # Perform analysis
            result = analysis_function(data)
            
            # Send result back
            self.result_queue.put(('result', {
                'type': 'analysis',
                'result': result,
                'timestamp': time.time(),
            }))
            
        except Exception as e:
            # Send error result
            self.result_queue.put(('result', {
                'type': 'analysis',
                'error': str(e),
                'timestamp': time.time(),
            }))
    
    def collect_results(self):
        """Collect results from workers"""
        results = []
        
        while not self.result_queue.empty():
            priority, result = self.result_queue.get()
            
            if priority == 'result':
                results.append(result)
        
        return results
```

## 9.2 Cloud Deployment

```python
class CloudNLMDeployer:
    """Deploy NLM simulations in the cloud"""
    
    def __init__(self, provider='aws', region='us-west-2'):
        self.provider = provider
        self.region = region
        self.compute_resources = {}
        self.storage_resources = {}
    
    def create_compute_cluster(self, num_instances, instance_type='c5.large'):
        """Create a compute cluster for distributed simulations"""
        if self.provider == 'aws':
            self._create_aws_cluster(num_instances, instance_type)
        elif self.provider == 'gcp':
            self._create_gcp_cluster(num_instances, instance_type)
        elif self.provider == 'azure':
            self._create_azure_cluster(num_instances, instance_type)
        
        return self.compute_resources
    
    def _create_aws_cluster(self, num_instances, instance_type):
        """Create AWS cluster"""
        # This would use AWS SDK to create EC2 instances
        self.compute_resources = {
            'provider': 'aws',
            'region': self.region,
            'instance_type': instance_type,
            'num_instances': num_instances,
            'status': 'creating',
        }
    
    def _create_gcp_cluster(self, num_instances, instance_type):
        """Create GCP cluster"""
        self.compute_resources = {
            'provider': 'gcp',
            'region': self.region,
            'instance_type': instance_type,
            'num_instances': num_instances,
            'status': 'creating',
        }
    
    def _create_azure_cluster(self, num_instances, instance_type):
        """Create Azure cluster"""
        self.compute_resources = {
            'provider': 'azure',
            'region': self.region,
            'instance_type': instance_type,
            'num_instances': num_instances,
            'status': 'creating',
        }
    
    def deploy_simulation(self, simulation_name, parameters, num_workers=4):
        """Deploy a simulation to the cloud"""
        # Create compute cluster
        cluster = self.create_compute_cluster(num_workers)
        
        # Prepare simulation parameters
        simulation_params = {
            'name': simulation_name,
            'parameters': parameters,
            'num_workers': num_workers,
        }
        
        # Submit to cluster
        if self.provider == 'aws':
            self._submit_to_aws(cluster, simulation_params)
        elif self.provider == 'gcp':
            self._submit_to_gcp(cluster, simulation_params)
        elif self.provider == 'azure':
            self._submit_to_azure(cluster, simulation_params)
        
        return simulation_params
    
    def _submit_to_aws(self, cluster, simulation_params):
        """Submit simulation to AWS cluster"""
        # This would use AWS Batch or similar service
        simulation_params['cluster_id'] = cluster['id']
        simulation_params['status'] = 'submitted'
    
    def _submit_to_gcp(self, cluster, simulation_params):
        """Submit simulation to GCP cluster"""
        # This would use Google Cloud Batch or similar service
        simulation_params['cluster_id'] = cluster['id']
        simulation_params['status'] = 'submitted'
    
    def _submit_to_azure(self, cluster, simulation_params):
        """Submit simulation to Azure cluster"""
        # This would use Azure Batch or similar service
        simulation_params['cluster_id'] = cluster['id']
        simulation_params['status'] = 'submitted'
    
    def monitor_simulation(self, simulation_id):
        """Monitor simulation progress"""
        if self.provider == 'aws':
            return self._monitor_aws_simulation(simulation_id)
        elif self.provider == 'gcp':
            return self._monitor_gcp_simulation(simulation_id)
        elif self.provider == 'azure':
            return self._monitor_azure_simulation(simulation_id)
    
    def _monitor_aws_simulation(self, simulation_id):
        """Monitor AWS simulation"""
        # This would use AWS CloudWatch or similar
        return {
            'status': 'running',
            'progress': 0.5,
            'worker_status': 'active',
            'logs': 'cloud_watch_link',
        }
    
    def _monitor_gcp_simulation(self, simulation_id):
        """Monitor GCP simulation"""
        return {
            'status': 'running',
            'progress': 0.5,
            'worker_status': 'active',
            'logs': 'gcp_logging_link',
        }
    
    def _monitor_azure_simulation(self, simulation_id):
        """Monitor Azure simulation"""
        return {
            'status': 'running',
            'progress': 0.5,
            'worker_status': 'active',
            'logs': 'azure_monitor_link',
        }
```

---

# 10. Advanced Extensions

## 10.1 Custom Neuron Models

```python
import math
import numpy as np

class CustomNeuronModel:
    """Base class for custom neuron models"""
    
    def __init__(self, params=None):
        self.params = params or {}
        self.default_params = self._get_default_parameters()
        
        # Update with user params
        for key, value in self.default_params.items():
            if key not in self.params:
                self.params[key] = value
    
    def _get_default_parameters(self):
        """Get default parameters for neuron model"""
        return {
            'v_rest': -70.0,
            'v_thresh': -55.0,
            'v_reset': -70.0,
            'tau_m': 20.0,
            'tau_ref': 5.0,
            'conductance': 10.0,
        }
    
    def step(self, v, current, dt, time):
        """Compute next membrane potential"""
        raise NotImplementedError("Subclasses must implement step method")
    
    def is_spiking(self, v, threshold):
        """Check if neuron is spiking"""
        return v > threshold
class IzhikevichNeuronModel(CustomNeuronModel):
    """Izhikevich neuron model (more biologically realistic)"""
    
    def _get_default_parameters(self):
        params = super()._get_default_parameters()
        params.update({
            'a': 0.02,
            'b': -65.0,
            'c': -65.0,
            'd': 8.0,
            'v_peak': 40.0,
        })
        return params
    
    def step(self, v, current, dt, time):
        """Izhikevich neuron dynamics"""
        a = self.params['a']
        b = self.params['b']
        c = self.params['c']
        d = self.params['d']
        v_peak = self.params['v_peak']
        
        # Update voltage
        dv = v * (v - b) + d * current - a
        v += dv * dt
        
        # Update adaptation variable (simplified)
        u = v - c
        
        # Check for spike
        if v >= v_peak:
            v = c
            u += d
        
        return v, u
class HindmarshRoseNeuronModel(CustomNeuronModel):
    """Hindmarsh-Rose neuron model (bursting dynamics)"""
    
    def _get_default_parameters(self):
        params = super()._get_default_parameters()
        params.update({
            'a': 1.0,
            'b': 3.0,
            'c': 1.0,
            'r': 0.001,
        })
        return params
    
    def step(self, v, current, dt, time):
        """Hindmarsh-Rose neuron dynamics"""
        a = self.params['a']
        b = self.params['b']
        c = self.params['c']
        r = self.params['r']
        
        # Compute derivatives
        dx = v - a * v**3 + b * v**2 - current
        dy = c * (v - r * v**2 - u)
        
        # Update state
        v += dx * dt
        u += dy * dt
        
        return v, u
class AdaptiveExponentialLIFNeuronModel(CustomNeuronModel):
    """Adaptive Exponential LIF neuron model"""
    
    def _get_default_parameters(self):
        params = super()._get_default_parameters()
        params.update({
            'delta_T': 2.0,
            'tau_w': 100.0,
            'b': 0.0805,
            'c': -65.0,
        })
        return params
    
    def step(self, v, current, dt, time):
        """Adaptive Exponential LIF dynamics"""
        delta_T = self.params['delta_T']
        tau_w = self.params['tau_w']
        b = self.params['b']
        c = self.params['c']
        
        # Exponential integrate-and-fire
        dv = current + delta_T * (v - self.params['v_rest']) * (v - threshold)
        
        # Adaptation
        dw = (a * (v - b) - w) * dt / tau_w
        
        # Update voltage
        v += dv * dt
        
        # Check for spike
        if v > self.params['v_thresh']:
            v = self.params['v_reset']
            spiked = True
        else:
            spiked = False
        
        return v, u, spiked
```

## 10.2 Advanced Plasticity Models

```python
class AdvancedPlasticityModel:
    """Base class for advanced plasticity models"""
    
    def __init__(self, params=None):
        self.params = params or {}
    
    def update_weight(self, weight, pre_spike, post_spike, reward=0.0, time=None):
        """Update synaptic weight based on plasticity rule"""
        raise NotImplementedError("Subclasses must implement update_weight method")
class TraceBasedPlasticity(AdvancedPlasticityModel):
    """Trace-based plasticity model"""
    
    def __init__(self, params=None):
        super().__init__(params)
        self.default_params = {
            'pre_trace_decay': 0.95,
            'post_trace_decay': 0.95,
            'pre_trace_increment': 1.0,
            'post_trace_increment': 1.0,
            'weight_change': 0.1,
        }
        
        # Merge with user params
        for key, value in self.default_params.items():
            if key not in self.params:
                self.params[key] = value
        
        # Initialize traces
        self.pre_trace = 0.0
        self.post_trace = 0.0
    
    def update_weight(self, weight, pre_spike, post_spike, reward=0.0, time=None):
        """Update weight using trace-based plasticity"""
        # Update traces
        if pre_spike:
            self.pre_trace += self.params['pre_trace_increment']
        if post_spike:
            self.post_trace += self.params['post_trace_increment']
        
        # Decay traces
        self.pre_trace *= self.params['pre_trace_decay']
        self.post_trace *= self.params['post_trace_decay']
        
        # Calculate weight change
        weight_change = self.params['weight_change'] * self.pre_trace * self.post_trace
        
        # Apply reward modulation
        if reward != 0.0:
            weight_change += reward * 0.1 * self.pre_trace
        
        # Update weight
        new_weight = weight + weight_change
        
        # Clamp weight
        new_weight = max(-1.0, min(1.0, new_weight))
        
        return new_weight
classhomeostaticPlasticity(AdvancedPlasticityModel):
    """Homeostatic plasticity model"""
    
    def __init__(self, params=None):
        super().__init__(params)
        self.default_params = {
            'target_rate': 0.1,
            'learning_rate': 0.01,
            'stdev_window': 1000,
            'min_weight': -0.5,
            'max_weight': 0.5,
        }
        
        for key, value in self.default_params.items():
            if key not in self.params:
                self.params[key] = value
        
        self.spike_history = []
    
    def update_weight(self, weight, pre_spike, post_spike, reward=0.0, time=None):
        """Update weight using homeostatic plasticity"""
        if time is not None:
            self.spike_history.append((time, pre_spike or post_spike))
        
        # Calculate firing rate
        firing_rate = self._calculate_firing_rate()
        
        # Homeostatic adjustment
        deviation = firing_rate - self.params['target_rate']
        
        if deviation > 0:  # Too much firing
            # Weaken synapses
            weight_change = -self.params['learning_rate'] * deviation
        else:  # Too little firing
            # Strengthen synapses
            weight_change = -self.params['learning_rate'] * deviation
        
        new_weight = weight + weight_change
        
        # Apply bounds
        new_weight = max(self.params['min_weight'], min(self.params['max_weight'], new_weight))
        
        return new_weight
    
    def _calculate_firing_rate(self):
        """Calculate firing rate"""
        if not self.spike_history:
            return 0.0
        
        # Calculate firing rate over recent time window
        current_time = self.spike_history[-1][0]
        recent_time = current_time - self.params['stdev_window']
        
        recent_spikes = sum(1 for t, spiked in self.spike_history 
                           if spiked and t > recent_time)
        
        recent_duration = max(1.0, current_time - recent_time)
        
        return recent_spikes / recent_duration
```

This completes the advanced features guide for NLM. The features presented here demonstrate the flexibility and extensibility of the NLM framework, allowing users to implement custom neuron models, plasticity rules, distributed computing, cloud deployment, and advanced research capabilities.

For the best experience with these advanced features, I recommend:

1. **Starting simple**: Begin with basic NLM functionality before exploring advanced features
2. **Gradual implementation**: Add features incrementally to avoid breaking existing code
3. **Testing thoroughly**: Use the debugging and visualization tools to validate custom implementations
4. **Documenting**: Document custom features for future maintenance and collaboration

The NLM framework is designed to be extensible, allowing users to push the boundaries of neural simulation while maintaining a stable core implementation. These advanced features enable researchers and developers to create custom neural systems tailored to their specific needs and research questions.

Good luck with your advanced NLM development! 🚀