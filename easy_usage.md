# NLM Advanced User Guide

This comprehensive guide provides advanced techniques for using the Neural Learning Machine (NLM) framework for complex neural simulation scenarios.

## Advanced Topics

### 1. High-Performance Simulation

#### Batch Processing Techniques

```python
import pynlm
from pynlm import SimulationSession

def batch_simulation(config, num_steps, batch_size=1000):
    """Advanced batch simulation with performance optimization."""
    
    with SimulationSession(config) as session:
        # Pre-allocate memory for better performance
        step_results = []
        
        for batch_start in range(0, num_steps, batch_size):
            batch_end = min(batch_start + batch_size, num_steps)
            
            # Process batch efficiently
            for step in range(batch_start, batch_end):
                # Use batch-friendly operations
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Collect metrics every N steps
                if step % 100 == 0:
                    metrics = {
                        'step': step,
                        'firing_rate': brain.getAverageFiringRate(),
                        'reward': get_reward_from_world(world),
                        'curiosity': agent.getCuriosityLevel(),
                        'memory': brain.getTotalMemoryUsage()
                    }
                    step_results.append(metrics)
        
        # Analyze results
        analyze_batch_results(step_results)
        return step_results

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.enable", True)
results = batch_simulation(config, 100000, batch_size=5000)
```

#### Memory Management Strategies

```python
import pynlm
import gc
import psutil
import os

class MemoryManager:
    def __init__(self):
        self.process = psutil.Process(os.getpid())
        self.baseline_memory = 0
        self.max_memory_usage = 0
    
    def track_memory_usage(self, label=""):
        """Track memory usage for debugging performance issues."""
        memory_mb = self.process.memory_info().rss / 1024 / 1024
        self.max_memory_usage = max(self.max_memory_usage, memory_mb)
        
        if self.baseline_memory == 0:
            self.baseline_memory = memory_mb
        
        print(f"[{label}] Memory usage: {memory_mb:.1f} MB (max: {self.max_memory_usage:.1f} MB)")
        return memory_mb
    
    def optimized_simulation_loop(self, brain, world, agent, steps):
        """Simulation loop with memory optimization."""
        self.track_memory_usage("Start")
        
        # Pre-allocate frequently used objects
        empty_percept = pynlm.SensoryPercept()
        empty_action = pynlm.Action()
        
        for step in range(steps):
            try:
                # Efficient memory usage patterns
                world.update(0.1)
                
                # Reuse percept if possible (reduces allocation)
                if step % 100 == 0:  # Update percept periodically
                    percept = world.getSensoryPercept()
                else:
                    # Clone for modification
                    percept = empty_percept.clone() if hasattr(empty_percept, 'clone') else empty_percept
                
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Reuse action object
                if step % 100 == 0:
                    action = agent.decodeMotorCommand()
                else:
                    action = empty_action.clone() if hasattr(empty_action, 'clone') else empty_action
                
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Manual garbage collection for large objects
                if step % 1000 == 0:
                    gc.collect()
                    self.track_memory_usage(f"Step {step}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                raise
        
        self.track_memory_usage("End")
        return self.max_memory_usage - self.baseline_memory

# Usage example
memory_manager = MemoryManager()
memory_manager.optimized_simulation_loop(brain, world, agent, 10000)
```

### 2. Advanced Brain Configuration

#### Custom Configuration Builder

```python
import pynlm
import json

class AdvancedBrainBuilder:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.neuron_configs = []
        self.region_configs = []
        self.connection_configs = []
    
    def add_neural_population(self, name, neuron_type="excitatory", 
                             count=1000, density=0.1):
        """Add a neural population with specific properties."""
        self.neuron_configs.append({
            'name': name,
            'type': neuron_type,
            'count': count,
            'density': density
        })
        
        # Apply configuration
        base_id = len(self.neuron_configs) - 1
        start_id = base_id * 1000
        
        for i in range(count):
            neuron_id = start_id + i
            self.config.set(f"neuron.{neuron_id}.type", neuron_type)
            self.config.set(f"neuron.{neuron_id}.density", density)
        
        return self
    
    def add_brain_region(self, name, neuron_ids, external_connections=None):
        """Create a brain region with specific neuron connectivity."""
        self.region_configs.append({
            'name': name,
            'neuron_ids': neuron_ids,
            'external_connections': external_connections or {}
        })
        
        region_id = len(self.region_configs) - 1
        for neuron_id in neuron_ids:
            self.config.set(f"neuron.{neuron_id}.region_id", region_id)
        
        return self
    
    def add_plasticity_rule(self, rule_type="stdp", learning_rate=0.001,
                           time_constant=20.0):
        """Add plasticity rules with specific parameters."""
        self.connection_configs.append({
            'type': rule_type,
            'learning_rate': learning_rate,
            'time_constant': time_constant
        })
        
        return self
    
    def configure_development(self, stage="adult", plasticity_rate=0.01,
                            critical_period=True):
        """Configure developmental parameters."""
        self.config.set("development.stage", stage)
        self.config.set("development.plasticity_rate", plasticity_rate)
        self.config.set("development.critical_period", critical_period)
        
        return self
    
    def build(self):
        """Build and return the final configuration."""
        # Apply all configurations
        self._apply_neuron_configurations()
        self._apply_region_configurations()
        self._apply_plasticity_configurations()
        
        return self.config
    
    def _apply_neuron_configurations(self):
        """Apply neuron-specific configurations."""
        for pop_config in self.neuron_configs:
            # Set baseline neuron properties
            self.config.set(f"brain.{pop_config['name']}.type", pop_config['type'])
            self.config.set(f"brain.{pop_config['name']}.count", pop_config['count'])
            self.config.set(f"brain.{pop_config['name']}.density", pop_config['density'])
    
    def _apply_region_configurations(self):
        """Apply brain region configurations."""
        for region_config in self.region_configs:
            self.config.set(f"region.{region_config['name']}.neuron_count", 
                          len(region_config['neuron_ids']))
            if region_config['external_connections']:
                self.config.set(f"region.{region_config['name']}.external_connections",
                              json.dumps(region_config['external_connections']))
    
    def _apply_plasticity_configurations(self):
        """Apply plasticity rule configurations."""
        for rule_config in self.connection_configs:
            self.config.set(f"plasticity.{rule_config['type']}.learning_rate",
                          rule_config['learning_rate'])
            self.config.set(f"plasticity.{rule_config['type']}.time_constant",
                          rule_config['time_constant'])

# Advanced usage example
builder = (AdvancedBrainBuilder()
    .add_neural_population("cortex", "excitatory", 2000, 0.15)
    .add_neural_population("striatum", "inhibitory", 500, 0.2)
    .add_brain_region("cortex_region", list(range(2000)))
    .add_brain_region("striatum_region", list(range(2000, 2500)))
    .add_plasticity_rule("stdp", 0.001, 20.0)
    .add_plasticity_rule("hebbian", 0.01, 10.0)
    .configure_development("adult", 0.01, False))

config = builder.build()
```

### 3. Agent Behavior Control

#### Goal-Based Agent Control

```python
import pynlm
import time

class GoalBasedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.world = world
        self.brain = brain
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Goal management
        self.current_goal = None
        self.goal_progress = 0.0
        self.objective_values = {}
    
    def set_navigation_goal(self, target_x, target_y, priority=1.0):
        """Set a navigation goal for the agent."""
        self.current_goal = {
            'type': 'navigation',
            'target': (target_x, target_y),
            'priority': priority,
            'start_time': time.time()
        }
        
        # Compute action to reach goal
        self._update_navigation_behavior()
        return True
    
    def set_resource_gathering_goal(self, resource_type, quantity, location_hint=None):
        """Set a resource gathering goal."""
        self.current_goal = {
            'type': 'gathering',
            'resource_type': resource_type,
            'quantity': quantity,
            'location_hint': location_hint,
            'progress': 0.0
        }
        
        return True
    
    def update_behavior(self):
        """Update agent behavior based on current goals."""
        if not self.current_goal:
            return self._idle_behavior()
        
        goal_type = self.current_goal['type']
        
        if goal_type == 'navigation':
            self._update_navigation_behavior()
        elif goal_type == 'gathering':
            self._update_gathering_behavior()
        elif goal_type == 'social':
            self._update_social_behavior()
        else:
            self._default_behavior()
    
    def _update_navigation_behavior(self):
        """Update behavior for navigation goals."""
        world = self.agent.getBrain().getConfig()
        agent_pos = self.world.getAgentBody()
        
        if not self.current_goal:
            return
        
        target_x, target_y = self.current_goal['target']
        distance = ((agent_pos.x - target_x)**2 + (agent_pos.y - target_y)**2)**0.5
        
        # Adjust goal progress based on proximity
        max_distance = self.world.getWidth()
        self.goal_progress = max(0.0, 1.0 - distance / max_distance)
        
        # Enhanced navigation with sensory feedback
        percept = self.world.getSensoryPercept()
        
        # Increase action selection for goal-directed behavior
        self.agent.setGoalPriority(self.current_goal['priority'])
        
        # Apply exploration bonus near goal
        if self.goal_progress > 0.8:
            self.agent.enableCuriosity(True)  # Reduce exploration near goal
            self.agent.applyCuriosityModifier(-0.5)
        else:
            self.agent.enableCuriosity(True)  # Maintain exploration
        
        print(f"Navigation goal progress: {self.goal_progress:.2f}, distance: {distance:.1f}")
    
    def _update_gathering_behavior(self):
        """Update behavior for resource gathering goals."""
        goal = self.current_goal
        goal_type = goal['resource_type']
        
        # Find nearby resources
        percept = self.world.getSensoryPercept()
        vision = percept.getVision() if hasattr(percept, 'getVision') else []
        
        # Check for resources
        resources_found = 0
        for resource in vision:
            if self._is_desired_resource(resource, goal_type):
                resources_found += 1
        
        # Update progress
        goal['progress'] = min(1.0, resources_found / goal['quantity'])
        
        # Modify behavior based on progress
        if goal['progress'] >= 0.8:
            # Near completion: refine actions
            self.agent.enableCuriosity(False)  # Focused behavior
            self.agent.enableRewardModulation(True)  # Maximize reward
        else:
            # Early stage: explore for resources
            self.agent.enableCuriosity(True)  # Maintain exploration
        
        print(f"Gathering goal progress: {goal['progress']:.2f}, resources found: {resources_found}")
    
    def _is_desired_resource(self, resource, goal_type):
        """Check if resource matches goal requirements."""
        # This would check resource properties against goal type
        # Implementation depends on world object structure
        return True  # Placeholder
    
    def check_goal_completion(self):
        """Check if current goal is complete."""
        if not self.current_goal:
            return False
        
        if self.current_goal['progress'] >= 1.0:
            print(f"Goal completed: {self.current_goal}")
            self.current_goal = None
            self.goal_progress = 0.0
            return True
        
        # Check time-based completion for navigation goals
        if self.current_goal['type'] == 'navigation':
            elapsed = time.time() - self.current_goal['start_time']
            max_time = self.current_goal.get('timeout', 300.0)  # 5 minute default
            
            if elapsed > max_time:
                print(f"Goal timeout: {self.current_goal}")
                self.current_goal = None
                self.goal_progress = 0.0
                return True
        
        return False
    
    def _idle_behavior(self):
        """Default idle behavior when no goal is set."""
        self.agent.enableCuriosity(True)  # Normal exploration
        self.agent.applyCuriosityModifier(0.0)  # Reset curiosity
        print("Agent in idle mode - normal exploration")
    
    def _update_social_behavior(self):
        """Update behavior for social goals."""
        # Social learning and interaction logic
        pass
    
    def _default_behavior(self):
        """Default behavior for unknown goal types."""
        self.agent.enableCuriosity(True)  # Maintain exploration

# Advanced goal-based agent usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(50, 50, 16, 16)
world.reset()
world.setAgentStart(5, 5)

agent = GoalBasedAgent(brain, world)

# Set a navigation goal
agent.set_navigation_goal(45, 45, priority=2.0)

# Run simulation with goal-based behavior
for step in range(1000):
    world.update(0.1)
    agent.update_behavior()
    
    percept = world.getSensoryPercept()
    agent.agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check if goal completed
    if agent.check_goal_completion():
        print("Goal achieved! Continuing with new goals...")
        break
```

### 4. Performance Monitoring and Optimization

#### Real-time Performance Monitoring

```python
import pynlm
import time
import threading
from collections import deque

class PerformanceMonitor:
    def __init__(self, brain, world, agent, update_interval=1.0):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.update_interval = update_interval
        
        # Performance metrics
        self.step_times = deque(maxlen=1000)
        self.firing_rates = deque(maxlen=1000)
        self.memory_usages = deque(maxlen=1000)
        self.cpu_usage = deque(maxlen=1000)
        
        self.monitoring = False
        self.monitor_thread = None
        
        # Performance alerts
        self.performance_issues = []
        
        # Performance thresholds
        self.thresholds = {
            'max_step_time': 0.1,  # 100ms per step
            'min_firing_rate': 10.0,  # Hz
            'max_memory_growth': 100.0,  # MB
            'max_cpu_usage': 80.0  # %
        }
    
    def start_monitoring(self):
        """Start performance monitoring in background."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print("Performance monitoring started")
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        print("Performance monitoring stopped")
    
    def _monitor_loop(self):
        """Background monitoring loop."""
        last_update = time.time()
        
        while self.monitoring:
            current_time = time.time()
            
            # Check if it's time for performance update
            if current_time - last_update >= self.update_interval:
                self._update_performance_metrics()
                self._check_performance_issues()
                last_update = current_time
            
            time.sleep(0.1)  # Small sleep to prevent busy waiting
    
    def _update_performance_metrics(self):
        """Update performance metrics."""
        # Get current metrics
        step_time = self._get_last_step_time()
        firing_rate = self.brain.getAverageFiringRate()
        memory_usage = self._get_memory_usage()
        cpu_usage = self._get_cpu_usage()
        
        # Add to metrics queues
        self.step_times.append(step_time)
        self.firing_rates.append(firing_rate)
        self.memory_usages.append(memory_usage)
        self.cpu_usage.append(cpu_usage)
        
        # Print performance summary
        self._print_performance_summary()
    
    def _get_last_step_time(self):
        """Get time taken for last step."""
        return 0.001  # Placeholder - would measure actual step time
    
    def _get_memory_usage(self):
        """Get current memory usage in MB."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024
    
    def _get_cpu_usage(self):
        """Get current CPU usage percentage."""
        import psutil
        return psutil.cpu_percent()
    
    def _print_performance_summary(self):
        """Print performance summary."""
        if len(self.step_times) < 10:
            return
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        avg_firing_rate = sum(self.firing_rates) / len(self.firing_rates)
        avg_memory_usage = sum(self.memory_usages) / len(self.memory_usages)
        avg_cpu_usage = sum(self.cpu_usage) / len(self.cpu_usage)
        
        print(f"Performance Summary:")
        print(f"  Steps/sec: {1.0/avg_step_time:.1f}")
        print(f"  Avg firing rate: {avg_firing_rate:.1f} Hz")
        print(f"  Memory usage: {avg_memory_usage:.1f} MB")
        print(f"  CPU usage: {avg_cpu_usage:.1f}%")
        print()
    
    def _check_performance_issues(self):
        """Check for performance issues based on thresholds."""
        if len(self.step_times) < 10:
            return
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        avg_firing_rate = sum(self.firing_rates) / len(self.firing_rates)
        avg_memory_usage = sum(self.memory_usages) / len(self.memory_usages)
        avg_cpu_usage = sum(self.cpu_usage) / len(self.cpu_usage)
        
        # Check step time
        if avg_step_time > self.thresholds['max_step_time']:
            issue = f"Slow step processing: {avg_step_time:.3f}s > {self.thresholds['max_step_time']}s"
            self._add_performance_issue(issue, 'performance')
        
        # Check firing rate
        if avg_firing_rate < self.thresholds['min_firing_rate']:
            issue = f"Low firing rate: {avg_firing_rate:.1f}Hz < {self.thresholds['min_firing_rate']}Hz"
            self._add_performance_issue(issue, 'activity')
        
        # Check memory growth
        if len(self.memory_usages) >= 2:
            memory_growth = self.memory_usages[-1] - self.memory_usages[0]
            if memory_growth > self.thresholds['max_memory_growth']:
                issue = f"High memory growth: {memory_growth:.1f}MB > {self.thresholds['max_memory_growth']}MB"
                self._add_performance_issue(issue, 'memory')
        
        # Check CPU usage
        if avg_cpu_usage > self.thresholds['max_cpu_usage']:
            issue = f"High CPU usage: {avg_cpu_usage:.1f}% > {self.thresholds['max_cpu_usage']}%"
            self._add_performance_issue(issue, 'cpu')
    
    def _add_performance_issue(self, issue, category):
        """Add performance issue to list."""
        self.performance_issues.append({
            'issue': issue,
            'category': category,
            'timestamp': time.time()
        })
        
        # Keep only recent issues
        cutoff = time.time() - 300.0  # 5 minutes
        self.performance_issues = [i for i in self.performance_issues 
                                  if i['timestamp'] > cutoff]
    
    def get_performance_report(self):
        """Get comprehensive performance report."""
        report = {
            'summary': {
                'steps_performed': len(self.step_times),
                'avg_step_time': sum(self.step_times) / len(self.step_times) if self.step_times else 0,
                'avg_firing_rate': sum(self.firing_rates) / len(self.firing_rates) if self.firing_rates else 0,
                'avg_memory_usage': sum(self.memory_usages) / len(self.memory_usages) if self.memory_usages else 0,
                'avg_cpu_usage': sum(self.cpu_usage) / len(self.cpu_usage) if self.cpu_usage else 0,
                'performance_issues': len(self.performance_issues)
            },
            'issues': self.performance_issues,
            'thresholds': self.thresholds
        }
        
        return report
    
    def optimize_for_performance(self):
        """Analyze performance and suggest optimizations."""
        report = self.get_performance_report()
        
        print("=== Performance Optimization Recommendations ===")
        
        suggestions = []
        
        # Analyze issues
        for issue in report['issues']:
            if issue['category'] == 'performance':
                suggestions.append(f"✓ Consider optimizing step processing - {issue['issue']}")
            elif issue['category'] == 'activity':
                suggestions.append(f"✓ Consider adjusting neural parameters - {issue['issue']}")
            elif issue['category'] == 'memory':
                suggestions.append(f"✓ Consider memory optimization strategies - {issue['issue']}")
            elif issue['category'] == 'cpu':
                suggestions.append(f"✓ Consider CPU parallelization - {issue['issue']}")
        
        if not suggestions:
            print("✓ No performance issues detected")
        else:
            for suggestion in suggestions:
                print(suggestion)
        
        # Print summary metrics
        summary = report['summary']
        print(f"\nPerformance Metrics:")
        print(f"  Steps/sec: {1.0/summary['avg_step_time']:.1f}")
        print(f"  Memory usage: {summary['avg_memory_usage']:.1f} MB")
        print(f"  CPU usage: {summary['avg_cpu_usage']:.1f}%")
        print(f"  Issues detected: {summary['performance_issues']}")
    
    def get_efficiency_score(self):
        """Calculate overall efficiency score (0-100)."""
        if len(self.step_times) == 0:
            return 0.0
        
        # Calculate efficiency components
        avg_step_time = sum(self.step_times) / len(self.step_times)
        avg_firing_rate = sum(self.firing_rates) / len(self.firing_rates)
        avg_memory_usage = sum(self.memory_usages) / len(self.memory_usages)
        avg_cpu_usage = sum(self.cpu_usage) / len(self.cpu_usage)
        
        # Normalize metrics (higher is better)
        step_efficiency = max(0, 100 * (0.1 / avg_step_time))  # Optimal ~100ms/step
        activity_efficiency = min(100, avg_firing_rate / 50.0)  # Target ~50Hz
        memory_efficiency = max(0, 100 * (1.0 - avg_memory_usage / 1000.0))  # Target <1GB
        cpu_efficiency = 100 - avg_cpu_usage  # Target <80% CPU
        
        # Weighted average
        efficiency_score = (
            step_efficiency * 0.3 +
            activity_efficiency * 0.2 +
            memory_efficiency * 0.3 +
            cpu_efficiency * 0.2
        )
        
        return efficiency_score

# Performance monitoring usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start monitoring
monitor = PerformanceMonitor(brain, world, agent, update_interval=2.0)
monitor.start_monitoring()

# Run simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Stop monitoring and get report
monitor.stop_monitoring()
monitor.optimize_for_performance()
efficiency_score = monitor.get_efficiency_score()
print(f"Overall efficiency score: {efficiency_score:.1f}/100")
```

## Advanced Topics Summary

This advanced user guide covers:

1. **High-Performance Simulation**: Batch processing, memory optimization, and efficient resource usage
2. **Advanced Brain Configuration**: Custom builders for complex neural architectures
3. **Agent Behavior Control**: Goal-based agents with navigation, gathering, and social behaviors
4. **Performance Monitoring and Optimization**: Real-time monitoring, issue detection, and optimization recommendations

These advanced techniques enable users to build more complex, efficient, and intelligent NLM simulations for research and production use.

## Additional Resources

For more advanced usage, refer to:
- `HOW_TO_USE.md` - Detailed Python API documentation
- `docs/ARCHITECTURE.md` - Technical architecture details
- `docs/SCIENCE.md` - Scientific background and limitations
- `docs/EXPERIMENTS.md` - Experiment procedures and metrics
- `PHASE6_FINAL_AUDIT.md` - Phase 6 implementation details