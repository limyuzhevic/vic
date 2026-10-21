# Advanced NLM Features and Extensions

This module provides advanced features for the NLM Python interface, including:
- Enhanced simulation control
- Memory management utilities
- Experiment management
- Performance monitoring
- Debugging tools

## Features Overview:

### 1. Enhanced Simulation Control
- **Advanced context managers** for complex simulation scenarios
- **State management** for saving and restoring brain states
- **Multi-environment support** for running simulations in different conditions
- **Batch processing** for running multiple simulations efficiently

### 2. Memory Management
- **Automatic memory optimization** based on usage patterns
- **Memory profiling** tools to identify memory leaks
- **Memory compression** for checkpoint saving
- **Memory consolidation** strategies for episodic memory

### 3. Experiment Management
- **Experiment runner** for systematic testing
- **Parameter sweeps** for exploring parameter space
- **Statistical analysis** tools for experiment results
- **Experiment database** for storing and comparing results

### 4. Performance Monitoring
- **Real-time performance metrics** during simulation
- **Bottleneck detection** for performance optimization
- **Resource monitoring** for system resource usage
- **Performance profiling** for deep performance analysis

### 5. Debugging Tools
- **Debug mode** with enhanced logging and visualization
- **State inspection** tools for examining brain state
- **Step-by-step execution** with pause and resume capabilities
- **Breakpoint support** for debugging complex simulations

## Usage Examples:

### 1. Advanced Simulation Control
```python
import pynlm
from advanced_features import SimulationController, BatchRunner, EnvironmentManager

class AdvancedSimulation:
    def __init__(self):
        self.controller = SimulationController()
        self.batch_runner = BatchRunner()
        self.env_manager = EnvironmentManager()
    
    def run_complex_simulation(self, num_steps=1000, timesteps=0.01):
        """Run a complex simulation with advanced features."""
        # Create and configure brain
        config = pynlm.create_default_config()
        config.set('brain.neuron_count', 2000)
        config.set('brain.synapse_density', 0.15)
        config.set('plasticity.enable', True)
        
        brain = pynlm.createBrain(config)
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        agent.enable_structural_plasticity(True)
        
        # Run simulation with advanced monitoring
        with self.controller.simulate(brain, world, agent, num_steps, timesteps) as sim:
            for step in sim.steps():
                # Advanced control options
                if step % 100 == 0:
                    print(f"Step {step}: Curiosity={sim.get_curiosity():.2f}, Novelty={sim.get_novelty():.2f}")
                
                # Dynamic parameter adjustment
                if step % 300 == 0:
                    sim.adjust_curiosity(0.1)
                    sim.adjust_plasticity(0.8)
    
    def run_batch_simulation(self, parameter_sets):
        """Run multiple simulations with different parameters."""
        results = []
        
        for params in parameter_sets:
            with self.batch_runner.simulation(**params) as sim:
                sim_result = sim.run()
                results.append(sim_result)
                print(f"Completed simulation with params: {params}")
        
        return self.batch_runner.analyze_results(results)
```

### 2. Memory Management
```python
import pynlm
from advanced_features import MemoryManager, MemoryProfiler

class AdvancedMemoryManager(MemoryManager):
    def __init__(self, brain):
        super().__init__(brain)
        self.profiler = MemoryProfiler(brain)
    
    def optimize_memory_automatically(self):
        """Automatically optimize memory based on usage patterns."""
        stats = self.profiler.get_memory_stats()
        
        # Adjust based on usage patterns
        if stats['working_memory']['active_traces'] > stats['working_memory']['capacity'] * 0.9:
            self.optimize_working_memory(int(stats['working_memory']['capacity'] * 0.8))
        
        if stats['episodic_memory']['episodes'] > 1000:
            self.consolidate_memories(0.8)
    
    def profile_memory_usage(self, duration=60):
        """Profile memory usage over time."""
        print("Starting memory profiling...")
        start_stats = self.profiler.get_memory_stats()
        
        # Run simulation for profiling duration
        import time
        start_time = time.time()
        
        while time.time() - start_time < duration:
            # Run a few simulation steps
            for step in range(10):
                self.brain.step(step)
        
        end_stats = self.profiler.get_memory_stats()
        
        # Analyze memory changes
        memory_diff = {}
        for key in start_stats:
            if key in end_stats:
                for subkey in start_stats[key]:
                    if subkey in end_stats[key]:
                        memory_diff[f"{key}.{subkey}"] = end_stats[key][subkey] - start_stats[key][subkey]
        
        return memory_diff

# Usage
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

memory_manager = AdvancedMemoryManager(brain)
memory_manager.optimize_memory_automatically()
memory_profile = memory_manager.profile_memory_usage(30)
print("Memory profile:", memory_profile)
```

### 3. Experiment Management
```python
import pynlm
import numpy as np
from advanced_features import ExperimentManager, StatisticalAnalyzer, ParameterSweeper

class AdvancedExperimentManager(ExperimentManager):
    def __init__(self, brain, world, agent):
        super().__init__(brain, world, agent)
        self.analyzer = StatisticalAnalyzer()
        self.sweeper = ParameterSweeper()
    
    def systematic_parameter_sweep(self, base_params, param_ranges, num_iterations=50):
        """Run systematic parameter sweep."""
        print("Running systematic parameter sweep...")
        
        # Generate parameter combinations
        param_combinations = self.sweeper.generate_combinations(
            base_params, param_ranges, num_iterations
        )
        
        results = []
        
        for i, params in enumerate(param_combinations):
            print(f"Running iteration {i+1}/{len(param_combinations)}")
            
            # Create simulation with these parameters
            sim_result = self.run_experiment(params)
            sim_result['params'] = params
            sim_result['iteration'] = i
            
            results.append(sim_result)
        
        # Analyze results
        analysis = self.analyzer.analyze_results(results)
        
        # Find optimal parameters
        optimal_params = self.sweeper.find_optimal_parameters(results)
        
        return results, analysis, optimal_params
    
    def analyze_learning_curves(self, experiment_results):
        """Analyze learning curves from experiment results."""
        learning_data = {}
        
        for result in experiment_results:
            # Extract learning metrics
            learning_data[result['params']] = {
                'curiosity': result['curiosity'],
                'novelty': result['novelty'],
                'prediction_error': result['prediction_error'],
                'firing_rate': result['average_firing_rate'],
                'spike_count': result['total_spikes']
            }
        
        # Generate learning curve analysis
        curve_analysis = self.analyzer.generate_learning_curves(learning_data)
        
        return curve_analysis
    
    def run_multi_stage_experiment(self, stages):
        """Run multi-stage experiment with different configurations."""
        all_results = []
        
        for stage_config in stages:
            print(f"Running stage: {stage_config['name']}")
            
            # Configure simulation for this stage
            self.configure_stage(stage_config)
            
            # Run stage
            stage_results = self.run_experiment()
            stage_results['stage'] = stage_config['name']
            stage_results['parameters'] = stage_config
            
            all_results.append(stage_results)
        
        # Compare stages
        stage_comparison = self.analyzer.compare_stages(all_results)
        
        return all_results, stage_comparison
```

### 4. Performance Monitoring
```python
import pynlm
import time
from advanced_features import PerformanceMonitor, ResourceMonitor, BottleneckDetector

class AdvancedPerformanceMonitor(PerformanceMonitor):
    def __init__(self, brain, world, agent):
        super().__init__(brain, world, agent)
        self.resource_monitor = ResourceMonitor()
        self.bottleneck_detector = BottleneckDetector()
    
    def run_with_monitoring(self, num_steps=1000, timestep=0.01):
        """Run simulation with comprehensive performance monitoring."""
        print("Starting performance monitoring...")
        
        # Start monitoring
        self.start_monitoring()
        self.resource_monitor.start_monitoring()
        
        # Run simulation
        start_time = time.time()
        
        for step in range(num_steps):
            # Record pre-execution metrics
            step_start_time = time.time()
            
            # Execute simulation step
            self.world.update(timestep)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.simulation_time)
            self.brain.step(step)
            
            # Record post-execution metrics
            step_end_time = time.time()
            step_duration = step_end_time - step_start_time
            
            # Check for bottlenecks
            self.bottleneck_detector.check_step_performance(step, step_duration)
            
            # Log performance metrics
            if step % 100 == 0:
                perf_metrics = self.get_performance_metrics()
                print(f"Step {step}: {step_duration:.4f}s, "
                      f"Spikes: {perf_metrics['spike_rate']:.0f}, "
                      f"Memory: {perf_metrics['memory_usage']:.1f}%")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Stop monitoring
        self.stop_monitoring()
        self.resource_monitor.stop_monitoring()
        
        # Get final metrics
        final_metrics = self.get_performance_metrics()
        
        # Generate performance report
        performance_report = {
            'total_time': total_time,
            'steps_per_second': num_steps / total_time,
            'timestep': timestep,
            'final_metrics': final_metrics,
            'bottlenecks': self.bottleneck_detector.get_bottlenecks(),
            'resource_usage': self.resource_monitor.get_resource_report()
        }
        
        return performance_report
    
    def profile_memory_and_performance(self, duration=300):
        """Profile memory and performance together."""
        print("Starting combined memory and performance profiling...")
        
        # Get initial metrics
        initial_perf = self.get_performance_metrics()
        initial_memory = self.resource_monitor.get_memory_usage()
        
        # Run simulation for profiling duration
        import time
        start_time = time.time()
        
        while time.time() - start_time < duration:
            for step in range(10):  # Run 10 steps per iteration
                self.brain.step(step)
        
        # Get final metrics
        final_perf = self.get_performance_metrics()
        final_memory = self.resource_monitor.get_memory_usage()
        
        # Analyze changes
        perf_change = {k: final_perf[k] - initial_perf[k] for k in final_perf}
        memory_change = {k: final_memory[k] - initial_memory[k] for k in final_memory}
        
        # Generate analysis
        analysis = {
            'performance_change': perf_change,
            'memory_change': memory_change,
            'memory_efficiency': final_perf['spike_rate'] / max(final_memory['memory_usage'], 1),
            'recommendations': self._generate_performance_recommendations(perf_change, memory_change)
        }
        
        return analysis
    
    def _generate_performance_recommendations(self, perf_change, memory_change):
        """Generate performance recommendations based on changes."""
        recommendations = []
        
        # Check performance changes
        if perf_change.get('spike_rate', 0) < 0:
            recommendations.append("Consider increasing simulation timestep for better performance")
        
        if perf_change.get('firing_neuron_count', 0) > perf_change.get('spike_rate', 0) * 2:
            recommendations.append("High firing rate relative to spike rate - check neuron parameters")
        
        # Check memory changes
        if memory_change.get('memory_usage', 0) > 50:
            recommendations.append("High memory usage - consider optimizing working memory settings")
        
        if memory_change.get('cached_objects', 0) > memory_change.get('active_traces', 0) * 5:
            recommendations.append("Many cached objects - consider adjusting cache size")
        
        return recommendations if recommendations else ["Performance is within acceptable ranges"]
```

### 5. Debugging Tools
```python
import pynlm
import numpy as np
from advanced_features import DebugSession, StateInspector, StepDebugger

class AdvancedDebugSession(DebugSession):
    def __init__(self, brain, world, agent):
        super().__init__(brain, world, agent)
        self.inspector = StateInspector(brain)
        self.debugger = StepDebugger(brain)
    
    def debug_simulation(self, num_steps=100, timestep=0.01, breakpoints=None):
        """Run simulation with debugging capabilities."""
        print("Starting debug session...")
        
        # Set up breakpoints
        if breakpoints:
            for step, condition in breakpoints.items():
                self.debugger.add_breakpoint(step, condition)
        
        # Run simulation with debugging
        for step in range(num_steps):
            try:
                # Check for breakpoints
                if self.debugger.should_break(step):
                    self._handle_breakpoint(step)
                    input("Press Enter to continue...")
                
                # Record state before step
                self.inspector.record_state(step, 'before')
                
                # Execute step
                self.world.update(timestep)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.simulation_time)
                self.brain.step(step)
                
                # Record state after step
                self.inspector.record_state(step, 'after')
                
                # Analyze state changes
                state_changes = self.inspector.analyze_state_changes(step)
                
                if state_changes:
                    print(f"Step {step} state changes: {state_changes}")
                
                # Log debug information
                self._log_debug_info(step)
                
            except Exception as e:
                print(f"Debug error at step {step}: {e}")
                import traceback
                traceback.print_exc()
                break
        
        # Generate debug report
        debug_report = self._generate_debug_report()
        
        return debug_report
    
    def step_by_step_debugging(self, num_steps=10):
        """Run simulation with step-by-step debugging."""
        print("Starting step-by-step debugging...")
        
        for step in range(num_steps):
            print(f"\n=== Step {step} ===")
            
            # Display current state
            self._display_state(step)
            
            # Show available actions
            print("Available commands:")
            print("  'n' - Next step")
            print("  's' - Show state details")
            print("  'r' - Record state")
            print("  'p' - Pause debugging")
            print("  'q' - Quit")
            
            # Execute based on user input
            command = input("Command: ").strip().lower()
            
            if command == 'q':
                break
            elif command == 'n':
                # Execute one step
                self._execute_single_step(step)
            elif command == 's':
                # Show state details
                self._display_state_details(step)
            elif command == 'r':
                # Record state
                self.inspector.record_state(step, 'manual')
                print(f"State recorded at step {step}")
            elif command == 'p':
                # Pause
                input("Paused. Press Enter to continue...")
            else:
                print(f"Unknown command: {command}")
    
    def _handle_breakpoint(self, step):
        """Handle breakpoint."""
        print(f"\n*** BREAKPOINT at step {step} ***")
        
        # Show debugger options
        print("Debugger options:")
        print("  'c' - Continue")
        print("  's' - Step into")
        print("  'o' - Step over")
        print("  'v' - Show variables")
        print("  'a' - Show stack")
        
        choice = input("Choice: ").strip().lower()
        
        if choice == 'c':
            return
        elif choice == 's':
            self.debugger.step_into()
        elif choice == 'o':
            self.debugger.step_over()
        elif choice == 'v':
            self._show_debug_variables(step)
        elif choice == 'a':
            self._show_debug_stack(step)
    
    def _display_state(self, step):
        """Display current state."""
        print(f"Brain state at step {step}:")
        print(f"  Neurons: {self.brain.total_neurons}")
        print(f"  Synapses: {self.brain.total_synapses}")
        print(f"  Firing neurons: {self.brain.firing_neurons}")
        print(f"  Average firing rate: {self.brain.average_firing_rate:.2f}")
        print(f"  Developmental stage: {self.brain.developmental_stage}")
        print(f"  Neuromodulation level: {self.brain.neuromodulation_level:.2f}")
    
    def _execute_single_step(self, step):
        """Execute single simulation step."""
        self.world.update(0.1)
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.simulation_time)
        self.brain.step(step)
    
    def _generate_debug_report(self):
        """Generate debug report."""
        report = {
            'total_steps': self.inspector.get_step_count(),
            'state_analyses': self.inspector.get_state_analyses(),
            'breakpoints_triggered': self.debugger.get_breakpoints_triggered(),
            'errors': self.debugger.get_errors(),
            'recommendations': self._generate_debug_recommendations()
        }
        
        return report
    
    def _generate_debug_recommendations(self):
        """Generate debug recommendations."""
        recommendations = []
        
        # Analyze debugging results
        state_analyses = self.inspector.get_state_analyses()
        
        for analysis in state_analyses:
            if analysis.get('unusual_activity', False):
                recommendations.append(
                    f"Unusual neural activity detected at step {analysis['step']}"
                )
            
            if analysis.get('memory_issues', False):
                recommendations.append(
                    f"Memory issues detected at step {analysis['step']}"
                )
        
        if not recommendations:
            recommendations.append("No issues detected during debugging")
        
        return recommendations
```

## Utility Classes:

### 1. SimulationController
Controls complex simulation scenarios with advanced features.

### 2. BatchRunner
Manages batch processing of multiple simulations.

### 3. EnvironmentManager
Manages different simulation environments.

### 4. MemoryManager
Advanced memory management with optimization.

### 5. PerformanceMonitor
Real-time performance monitoring and analysis.

### 6. ResourceMonitor
System resource monitoring.

### 7. BottleneckDetector
Detects performance bottlenecks.

### 8. ExperimentManager
Manages experiments with systematic parameter control.

### 9. StatisticalAnalyzer
Statistical analysis of experiment results.

### 10. ParameterSweeper
Generates and manages parameter combinations.

### 11. DebugSession
Advanced debugging session management.

### 12. StateInspector
Inspects and analyzes brain state.

### 13. StepDebugger
Step-by-step execution with breakpoints.

## Integration with NLM:

All advanced features integrate seamlessly with the core NLM framework:

1. **Compatibility**: All features work with the existing NLM API
2. **Performance**: Minimal performance overhead
3. **Flexibility**: Features can be enabled/disabled as needed
4. **Extensibility**: Easy to add new features
5. **Documentation**: Comprehensive documentation and examples

## Usage Examples:

```python
import pynlm
from advanced_features import (
    SimulationController, BatchRunner, MemoryManager,
    PerformanceMonitor, DebugSession
)

# Create NLM components
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Use advanced features
controller = SimulationController(brain, world, agent)
memory_manager = MemoryManager(brain)
performance_monitor = PerformanceMonitor(brain, world, agent)
debug_session = DebugSession(brain, world, agent)

# Run advanced simulation
results = controller.run_complex_simulation(1000, 0.01)
memory_manager.optimize_memory_automatically()
performance_report = performance_monitor.run_with_monitoring(1000, 0.01)
debug_report = debug_session.debug_simulation(100, 0.01)
```

## Conclusion:

The advanced NLM features provide:
1. **Enhanced control** over complex simulations
2. **Comprehensive monitoring** of performance and resources
3. **Advanced debugging** capabilities for development
4. **Systematic experimentation** with parameter control
5. **Memory optimization** for better performance
6. **Performance analysis** for optimization

These features make it easier to:
- Run complex simulations with advanced control
- Monitor and optimize performance
- Debug complex issues
- Conduct systematic experiments
- Manage memory efficiently
- Analyze results

The advanced features are designed to work seamlessly with the core NLM framework while providing additional capabilities for complex use cases.