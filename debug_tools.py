#!/usr/bin/env python3
"""
NLM Debug and Monitoring Tools

This script demonstrates the debugging and monitoring tools for NLM,
including performance monitoring, memory tracking, and debug logging.
"""

import pynlm
import time
import json
import os
from typing import Dict, List, Optional, Any

class NLMPerformanceMonitor:
    """Performance monitoring for NLM simulations."""
    
    def __init__(self):
        self.metrics = {
            'simulation_time': [],
            'spike_rates': [],
            'memory_usage': [],
            'neuron_counts': [],
            'synapse_counts': [],
            'firing_neuron_counts': []
        }
        self.start_time = None
        self.end_time = None
    
    def start_monitoring(self):
        """Start monitoring the simulation."""
        self.start_time = time.time()
        print("Performance monitoring started...")
    
    def stop_monitoring(self):
        """Stop monitoring and calculate performance metrics."""
        if self.start_time:
            self.end_time = time.time()
            duration = self.end_time - self.start_time
            
            print(f"\nPerformance Monitoring Summary:")
            print(f"Total simulation time: {duration:.2f} seconds")
            print(f"Average step time: {duration/100:.4f} seconds")  # Assuming 100 steps
            
            if self.metrics['spike_rates']:
                avg_spike_rate = sum(self.metrics['spike_rates']) / len(self.metrics['spike_rates'])
                print(f"Average spike rate: {avg_spike_rate:.2f} Hz")
            
            if self.metrics['neuron_counts']:
                avg_neuron_count = sum(self.metrics['neuron_counts']) / len(self.metrics['neuron_counts'])
                print(f"Average neuron count: {avg_neuron_count:.0f}")
            
            return {
                'duration': duration,
                'avg_step_time': duration/100,
                'avg_spike_rate': avg_spike_rate if self.metrics['spike_rates'] else 0,
                'avg_neuron_count': avg_neuron_count if self.metrics['neuron_counts'] else 0
            }
        return None
    
    def record_step_metrics(self, brain, step_num):
        """Record metrics for a simulation step."""
        self.metrics['simulation_time'].append(time.time())
        self.metrics['spike_rates'].append(brain.getAverageFiringRate())
        self.metrics['neuron_counts'].append(brain.getTotalNeuronCount())
        self.metrics['synapse_counts'].append(brain.getTotalSynapseCount())
        self.metrics['firing_neuron_counts'].append(brain.getFiringNeuronCount())
    
    def export_metrics(self, filename: str):
        """Export metrics to a JSON file."""
        data = {
            'metrics': self.metrics,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'total_steps': len(self.metrics['simulation_time'])
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Metrics exported to {filename}")

class NLMDebugLogger:
    """Debug logging for NLM simulations."""
    
    def __init__(self, log_file: str = "nlm_debug.log"):
        self.log_file = log_file
        self.log_entries = []
        self.error_count = 0
        self.warning_count = 0
        self.info_count = 0
    
    def log_error(self, message: str, brain=None, agent=None, world=None):
        """Log an error message."""
        entry = {
            'level': 'ERROR',
            'message': message,
            'timestamp': time.time(),
            'brain_stats': self._get_brain_stats(brain) if brain else None,
            'agent_stats': self._get_agent_stats(agent) if agent else None,
            'world_stats': self._get_world_stats(world) if world else None
        }
        
        self.log_entries.append(entry)
        self.error_count += 1
        
        # Always print error messages
        print(f"[ERROR] {message}")
        if brain:
            print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")
        
        # Write to log file
        self._write_to_file(entry)
    
    def log_warning(self, message: str, brain=None, agent=None, world=None):
        """Log a warning message."""
        entry = {
            'level': 'WARNING',
            'message': message,
            'timestamp': time.time(),
            'brain_stats': self._get_brain_stats(brain) if brain else None,
            'agent_stats': self._get_agent_stats(agent) if agent else None,
            'world_stats': self._get_world_stats(world) if world else None
        }
        
        self.log_entries.append(entry)
        self.warning_count += 1
        
        # Print warning messages
        print(f"[WARNING] {message}")
        
        # Write to log file
        self._write_to_file(entry)
    
    def log_info(self, message: str, brain=None, agent=None, world=None):
        """Log an info message."""
        entry = {
            'level': 'INFO',
            'message': message,
            'timestamp': time.time(),
            'brain_stats': self._get_brain_stats(brain) if brain else None,
            'agent_stats': self._get_agent_stats(agent) if agent else None,
            'world_stats': self._get_world_stats(world) if world else None
        }
        
        self.log_entries.append(entry)
        self.info_count += 1
        
        # Only print info messages in verbose mode
        # print(f"[INFO] {message}")
        
        # Write to log file
        self._write_to_file(entry)
    
    def log_simulation_event(self, event_type: str, brain, agent, world, step_num: int):
        """Log a simulation event with detailed statistics."""
        entry = {
            'level': 'EVENT',
            'event_type': event_type,
            'step': step_num,
            'timestamp': time.time(),
            'brain_stats': self._get_brain_stats(brain),
            'agent_stats': self._get_agent_stats(agent),
            'world_stats': self._get_world_stats(world)
        }
        
        self.log_entries.append(entry)
        
        # Print for important events
        if event_type in ['STEP_COMPLETE', 'EPISODE_COMPLETE', 'ERROR']:
            print(f"[EVENT] {event_type} at step {step_num}")
            if event_type == 'STEP_COMPLETE':
                print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
                print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    def _get_brain_stats(self, brain) -> Dict:
        """Get comprehensive brain statistics."""
        return {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
    
    def _get_agent_stats(self, agent) -> Dict:
        """Get comprehensive agent statistics."""
        return {
            'sensory_input_size': agent.getSensoryInputSize(),
            'motor_output_size': agent.getMotorOutputSize(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': agent.getDevelopmentalStage(),
            'feature_status': agent.getFeatureStatus(),
            'available_features': agent.getAvailableFeatures()
        }
    
    def _get_world_stats(self, world) -> Dict:
        """Get comprehensive world statistics."""
        return {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy(),
            'agent_position': {
                'x': world.getAgentBody().x,
                'y': world.getAgentBody().y,
                'orientation': world.getAgentBody().orientation
            }
        }
    
    def _write_to_file(self, entry: Dict):
        """Write log entry to file."""
        with open(self.log_file, 'a') as f:
            f.write(json.dumps(entry) + '\n')
    
    def print_summary(self):
        """Print a summary of the debug log."""
        print("\nDebug Log Summary:")
        print(f"  Total entries: {len(self.log_entries)}")
        print(f"  Errors: {self.error_count}")
        print(f"  Warnings: {self.warning_count}")
        print(f"  Info messages: {self.info_count}")
        
        # Count event types
        event_counts = {}
        for entry in self.log_entries:
            event_type = entry.get('event_type', 'UNKNOWN')
            event_counts[event_type] = event_counts.get(event_type, 0) + 1
        
        if event_counts:
            print("\nEvent counts:")
            for event_type, count in event_counts.items():
                print(f"  {event_type}: {count}")
    
    def export_log(self, filename: str):
        """Export log entries to a JSON file."""
        with open(filename, 'w') as f:
            json.dump(self.log_entries, f, indent=2)
        
        print(f"Log exported to {filename}")

class NLMMemoryMonitor:
    """Memory monitoring for NLM simulations."""
    
    def __init__(self):
        self.memory_samples = []
        self.peak_memory = 0
        self.start_time = None
    
    def start_monitoring(self):
        """Start memory monitoring."""
        self.start_time = time.time()
        print("Memory monitoring started...")
    
    def take_memory_sample(self, brain, agent, world):
        """Take a memory sample."""
        import sys
        
        # Get memory usage (approximate)
        memory_info = sys.getsizeof(brain) + sys.getsizeof(agent) + sys.getsizeof(world)
        
        # Get brain-specific memory (approximate)
        brain_memory = (
            brain.getTotalNeuronCount() * 64 +  # Rough estimate per neuron
            brain.getTotalSynapseCount() * 32 +  # Rough estimate per synapse
            1024  # Base overhead
        )
        
        total_memory = memory_info + brain_memory
        
        sample = {
            'timestamp': time.time(),
            'total_memory': total_memory,
            'brain_memory': brain_memory,
            'world_memory': memory_info,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount()
        }
        
        self.memory_samples.append(sample)
        self.peak_memory = max(self.peak_memory, total_memory)
        
        return total_memory
    
    def print_memory_report(self):
        """Print a memory usage report."""
        if not self.memory_samples:
            print("No memory samples collected.")
            return
        
        print("\nMemory Usage Report:")
        print(f"Peak memory: {self.peak_memory / 1024:.2f} KB")
        
        if len(self.memory_samples) > 1:
            # Calculate memory growth
            start_memory = self.memory_samples[0]['total_memory']
            end_memory = self.memory_samples[-1]['total_memory']
            memory_growth = end_memory - start_memory
            
            print(f"Memory growth: {memory_growth / 1024:.2f} KB")
            
            # Check for memory leaks
            if memory_growth > self.peak_memory * 0.1:  # More than 10% growth
                print("WARNING: Potential memory leak detected!")
        
        # Print memory samples
        print("\nMemory samples:")
        for i, sample in enumerate(self.memory_samples[::10]):  # Sample every 10th
            print(f"  Step {i*10}: {sample['total_memory'] / 1024:.2f} KB")

class NLMDebugSession:
    """Complete debug session with monitoring and logging."""
    
    def __init__(self, config_name: str = "debug"):
        self.config_name = config_name
        self.performance_monitor = NLMPerformanceMonitor()
        self.debug_logger = NLMDebugLogger(f"nlm_{config_name}_debug.log")
        self.memory_monitor = NLMMemoryMonitor()
        self.brain = None
        self.agent = None
        self.world = None
    
    def setup_debug_session(self, steps: int = 100):
        """Setup a debug session with comprehensive monitoring."""
        print(f"Setting up debug session: {self.config_name}")
        print("=" * 60)
        
        # Create components
        self.brain = pynlm.createBrain(pynlm.createQuickStartConfig())
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure for debugging
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Configure agent for comprehensive monitoring
        self.agent.configureForSimulation("balanced")
        self.agent.enableAllFeatures(True)
        
        # Initialize agent
        self.agent.initialize(world)
        
        print(f"Debug session setup complete:")
        print(f"  Brain: {self.brain.getTotalNeuronCount()} neurons")
        print(f"  Agent features: {self.agent.getFeatureStatus()}")
        print(f"  World: {world.getWidth()}x{world.getHeight()} units")
        print(f"  Simulation steps: {steps}")
        
        # Start monitoring
        self.performance_monitor.start_monitoring()
        self.memory_monitor.start_monitoring()
        
        return steps
    
    def run_debug_simulation(self, steps: int = 100):
        """Run a debug simulation with comprehensive logging."""
        print(f"\nStarting debug simulation with {steps} steps...")
        
        try:
            # Log session start
            self.debug_logger.log_info(f"Starting debug simulation with {steps} steps")
            
            # Run simulation
            for step in range(steps):
                try:
                    # Update world
                    self.world.update(0.1)
                    
                    # Log step start
                    self.debug_logger.log_info(f"Processing step {step}", 
                                             self.brain, self.agent, self.world)
                    
                    # Get sensory input
                    percept = self.world.getSensoryPercept()
                    
                    # Process sensory input
                    self.agent.processSensoryInput(percept)
                    
                    # Record brain activity
                    self.brain.step(step)
                    
                    # Get action
                    action = self.agent.decodeMotorCommand()
                    
                    # Apply action
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                    
                    # Apply reward modulation
                    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                    self.agent.applyRewardModulation(reward, 0.0)
                    
                    # Update development
                    self.agent.updateDevelopment(0.1)
                    
                    # Record metrics
                    self.performance_monitor.record_step_metrics(self.brain, step)
                    memory_used = self.memory_monitor.take_memory_sample(
                        self.brain, self.agent, self.world)
                    
                    # Log step completion
                    self.debug_logger.log_simulation_event(
                        'STEP_COMPLETE', self.brain, self.agent, self.world, step)
                    
                    # Print progress
                    if step % 20 == 0:
                        print(f"  Step {step}: "
                              f"{self.brain.getFiringNeuronCount()} neurons firing, "
                              f"{self.agent.getCuriosityLevel():.3f} curiosity")
                    
                except Exception as e:
                    self.debug_logger.log_error(f"Error at step {step}: {e}", 
                                               self.brain, self.agent, self.world)
                    # Continue with next step
                    continue
            
            # Log episode completion
            self.debug_logger.log_simulation_event(
                'EPISODE_COMPLETE', self.brain, self.agent, self.world, steps)
            
        except Exception as e:
            self.debug_logger.log_error(f"Simulation failed: {e}", 
                                       self.brain, self.agent, self.world)
            raise
        
        # Stop monitoring
        performance_metrics = self.performance_monitor.stop_monitoring()
        
        print(f"\nDebug simulation completed!")
        print(f"Performance metrics: {performance_metrics}")
        
        return performance_metrics
    
    def generate_debug_report(self):
        """Generate a comprehensive debug report."""
        print("\nGenerating debug report...")
        
        # Get final metrics
        final_brain_stats = self.debug_logger._get_brain_stats(self.brain) if self.brain else None
        final_agent_stats = self.debug_logger._get_agent_stats(self.agent) if self.agent else None
        final_world_stats = self.debug_logger._get_world_stats(self.world) if self.world else None
        
        # Performance report
        performance_metrics = self.performance_monitor.stop_monitoring()
        
        # Memory report
        self.memory_monitor.print_memory_report()
        
        # Log summary
        self.debug_logger.print_summary()
        
        # Create report dictionary
        report = {
            'session_config': self.config_name,
            'performance_metrics': performance_metrics,
            'final_brain_stats': final_brain_stats,
            'final_agent_stats': final_agent_stats,
            'final_world_stats': final_world_stats,
            'log_entries_count': len(self.debug_logger.log_entries),
            'error_count': self.debug_logger.error_count,
            'warning_count': self.debug_logger.warning_count,
            'info_count': self.debug_logger.info_count
        }
        
        # Save report to file
        report_filename = f"nlm_{self.config_name}_report.json"
        with open(report_filename, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\nDebug report saved to: {report_filename}")
        
        return report

# Example usage
if __name__ == "__main__":
    print("NLM Debug and Monitoring Tools")
    print("=" * 60)
    
    # Create a debug session
    debug_session = NLMDebugSession("example")
    
    # Setup debug session
    steps = debug_session.setup_debug_session(50)
    
    # Run debug simulation
    try:
        performance_metrics = debug_session.run_debug_simulation(steps)
        
        # Generate debug report
        report = debug_session.generate_debug_report()
        
        print("\nDebug session completed successfully!")
        print(f"Final performance metrics: {performance_metrics}")
        
    except Exception as e:
        print(f"\nDebug session failed: {e}")
        import traceback
        traceback.print_exc()
    
    print("\nDebug and monitoring tools demonstration complete.")
