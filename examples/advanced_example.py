#!/usr/bin/env python3
"""
NLM Advanced Example - Deep Dive into Phase 6 Features

This example demonstrates the most advanced features of NLM Phase 6:
- Complex configuration management
- Advanced plasticity control
- Real-time performance monitoring
- Error handling and recovery
- Visualization integration
- Multi-agent systems
- Advanced simulation control
"""

import pynlm
import time
import sys
import json
from datetime import datetime

class NLMAdvancedExample:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.agent = None
        self.world = None
        self.simulation_log = []
        self.performance_stats = {}
        
    def setup_advanced_configuration(self):
        """Set up advanced configuration with all available options."""
        print("=== Setting Up Advanced Configuration ===")
        
        # Brain configuration
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.2)
        self.config.set("brain.connection_probability", 0.1)
        self.config.set("brain.initial_weight_mean", 0.5)
        self.config.set("brain.initial_weight_std", 0.1)
        
        # Plasticity configuration
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.stdp.learning_rate", 0.005)
        self.config.set("plasticity.stdp.tau_plus", 20.0)
        self.config.set("plasticity.stdp.tau_minus", 20.0)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("plasticity.hebbian.learning_rate", 0.01)
        self.config.set("plasticity.structural.enable", True)
        self.config.set("plasticity.synaptogenesis_rate", 0.0001)
        self.config.set("plasticity.pruning_rate", 0.00001)
        
        # Neuromodulation configuration
        self.config.set("neuromod.dopamine.scale", 2.0)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.curiosity.threshold", 0.3)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.novelty.threshold", 0.2)
        self.config.set("neuromod.prediction_error.enable", True)
        self.config.set("neuromod.prediction_error.gain", 1.5)
        
        # Memory configuration
        self.config.set("working_memory.capacity", 200)
        self.config.set("working_memory.decay_rate", 0.05)
        self.config.set("episodic_memory.max_episodes", 5000)
        self.config.set("episodic_memory.replay_interval", 200)
        self.config.set("associative_memory.pattern_threshold", 0.7)
        self.config.set("associative_memory.pattern_capacity", 1000)
        
        # Cognitive configuration
        self.config.set("cognition.planning_depth", 8)
        self.config.set("cognition.attention.gain", 2.0)
        self.config.set("cognition.attention.inhibition_strength", 0.6)
        self.config.set("cognition.attention.excitation_strength", 2.0)
        self.config.set("cognition.concept_threshold", 0.6)
        
        # Development configuration
        self.config.set("development.critical_period_start", 50)
        self.config.set("development.critical_period_end", 500)
        self.config.set("development.plasticity_modulation_rate", 0.02)
        
        # Performance configuration
        self.config.set("performance.memory_pool_size", 1024 * 1024)
        self.config.set("performance.enable_simd", True)
        self.config.set("performance.enable_parallel", True)
        self.config.set("performance.thread_count", 4)
        self.config.set("performance.monitoring_enabled", True)
        
        # Checkpoint configuration
        self.config.set("checkpoint.directory", "./nlm_checkpoints")
        self.config.set("checkpoint.max_checkpoints", 1000)
        self.config.set("checkpoint.compression_level", 5)
        self.config.set("checkpoint.auto_save", True)
        
        # Simulation configuration
        self.config.set("simulation.timestep", 0.001)
        self.config.set("simulation.max_steps", 10000)
        self.config.set("simulation.paused", False)
        self.config.set("simulation.event_driven", True)
        
        print("Advanced configuration setup completed")
        
    def create_brain_with_monitoring(self):
        """Create brain with performance monitoring enabled."""
        print("\n=== Creating Brain with Performance Monitoring ===")
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable performance monitoring
        self.brain.enablePerformanceMonitoring(True)
        
        # Set up advanced plasticity
        self.brain.setSTDPParameters(0.01f, 0.012f, 15.0f)
        self.brain.setHebbianParameters(0.005f, 0.001f)
        self.brain.setStructuralPlasticityParameters(0.0002f, 0.00005f)
        
        # Set up neuromodulation
        self.brain.setNeuromodulationParameters(2.0f, 0.2f, 0.3f, 2.0f)
        
        # Set up memory systems
        self.brain.setWorkingMemoryParameters(500, 0.02f)
        self.brain.setEpisodicMemoryParameters(3000, 1000)
        
        # Set up cognitive systems
        self.brain.setCognitiveParameters(10, 2.5f, 0.5f, 1.8f)
        
        # Set up development system
        self.brain.setDevelopmentParameters(100, 1000, 0.03f)
        
        # Enable all subsystems
        self.brain.enableSubsystem("working_memory", True)
        self.brain.enableSubsystem("episodic_memory", True)
        self.brain.enableSubsystem("associative_memory", True)
        self.brain.enableSubsystem("planner", True)
        self.brain.enableSubsystem("concept_formation", True)
        self.brain.enableSubsystem("attention", True)
        self.brain.enableSubsystem("dopamine", True)
        self.brain.enableSubsystem("curiosity", True)
        self.brain.enableSubsystem("novelty", True)
        self.brain.enableSubsystem("prediction_error", True)
        
        print("Brain created with advanced configuration")
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Performance monitoring: {self.brain.isPerformanceMonitoringEnabled()}")
        
    def setup_agent_system(self):
        """Set up agent system with world interaction."""
        print("\n=== Setting Up Agent System ===")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable agent subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        print("Agent system setup completed")
        print(f"  World size: {self.world.getWidth()}x{self.world.getHeight()}")
        print(f"  Vision range: {self.world.getVisionWidth()}x{self.world.getVisionHeight()}")
        
    def run_advanced_simulation(self, max_steps=2000):
        """Run advanced simulation with monitoring and data collection."""
        print(f"\n=== Running Advanced Simulation ({max_steps} steps) ===")
        
        start_time = time.time()
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0f
            if percept.getInternal() and not percept.getInternal()->empty():
                reward = percept.getInternal()->at(0);
            self.agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect simulation data
            self.simulation_log.append({
                'step': step,
                'time': self.world.getSimulationTime(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'curiosity': self.agent.getCuriosityLevel() if self.agent.getCuriosity() else 0.0,
                'novelty': self.agent.getNoveltyLevel() if self.agent.getNovelty() else 0.0,
                'prediction_error': self.agent.getPredictionError() if self.agent.getPredictionErrorSignal() else 0.0,
                'reward': reward,
                'avg_firing_rate': self.brain.getAverageFiringRate()
            })
            
            # Log progress
            if step % 200 == 0:
                print(f"Step {step}/{max_steps}")
                print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
                if self.agent.getCuriosity():
                    print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
                if self.agent.getNovelty():
                    print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
                print(f"  Reward: {reward:.3f}")
                
                # Check for exploration triggers
                if self.agent.getCuriosity() and self.agent.getCuriosityLevel() > 0.7:
                    print(f"  HIGH CURIOSITY - Triggering exploration behavior")
                
                if self.agent.getNovelty() and self.agent.getNoveltyLevel() > 0.7:
                    print(f"  HIGH NOVELTY - Triggering novelty-seeking behavior")
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"\nAdvanced simulation completed in {duration:.2f} seconds")
        print(f"Average step time: {duration/max_steps*1000:.2f} ms")
        
        # Calculate performance statistics
        total_spikes = self.brain.getTotalSpikeCount()
        avg_firing_rate = self.brain.getAverageFiringRate()
        max_firing = max(entry['firing_neurons'] for entry in self.simulation_log)
        min_firing = min(entry['firing_neurons'] for entry in self.simulation_log)
        
        self.performance_stats = {
            'total_steps': max_steps,
            'duration': duration,
            'avg_step_time': duration/max_steps,
            'total_spikes': total_spikes,
            'avg_firing_rate': avg_firing_rate,
            'max_firing': max_firing,
            'min_firing': min_firing,
            'avg_curiosity': sum(entry['curiosity'] for entry in self.simulation_log) / len(self.simulation_log),
            'avg_novelty': sum(entry['novelty'] for entry in self.simulation_log) / len(self.simulation_log),
            'avg_prediction_error': sum(entry['prediction_error'] for entry in self.simulation_log) / len(self.simulation_log),
            'total_reward': sum(entry['reward'] for entry in self.simulation_log),
            'brain_efficiency': total_spikes / (max_steps * self.brain.getTotalNeuronCount()) * 100
        }
        
        print("\nPerformance Statistics:")
        for key, value in self.performance_stats.items():
            if isinstance(value, float):
                print(f"  {key}: {value:.3f}")
            else:
                print(f"  {key}: {value}")
        
    def run_performance_analysis(self):
        """Run detailed performance analysis."""
        print("\n=== Performance Analysis ===")
        
        if not self.performance_stats:
            print("No performance data available. Run simulation first.")
            return
        
        # Network analysis
        neurons = self.brain.getTotalNeuronCount()
        synapses = self.brain.getTotalSynapseCount()
        firing_rate = self.performance_stats['avg_firing_rate']
        e_i_ratio = self.brain.getExcitationInhibitionRatio()
        
        print("Network Analysis:")
        print(f"  Neurons: {neurons}")
        print(f"  Synapses: {synapses}")
        print(f"  Connectivity density: {synapses/(neurons*(neurons-1))*100:.2f}%")
        print(f"  Average firing rate: {firing_rate:.3f} Hz")
        print(f"  Excitation/Inhibition ratio: {e_i_ratio:.3f}")
        
        # Plasticity analysis
        print("\nPlasticity Analysis:")
        print(f"  STDP enabled: {self.config.has('plasticity.stdp.enable') and self.config.getOr('plasticity.stdp.enable', False)}")
        print(f"  Hebbian enabled: {self.config.has('plasticity.hebbian.enable') and self.config.getOr('plasticity.hebbian.enable', False)}")
        print(f"  Structural plasticity enabled: {self.config.has('plasticity.structural.enable') and self.config.getOr('plasticity.structural.enable', False)}")
        
        # Memory analysis
        print("\nMemory Analysis:")
        if self.brain.getWorkingMemory():
            working_memory_usage = self.brain.getWorkingMemoryUsage()
            working_memory_capacity = self.brain.getWorkingMemoryCapacity() if hasattr(self.brain.getWorkingMemory(), 'getCapacity') else 'N/A'
            print(f"  Working memory: {working_memory_usage}/{working_memory_capacity} traces")
        
        if self.brain.getEpisodicMemory():
            episodic_memory_usage = self.brain.getEpisodicMemoryUsage()
            episodic_memory_capacity = self.brain.getEpisodicMemoryCapacity() if hasattr(self.brain.getEpisodicMemory(), 'getMaxEpisodes') else 'N/A'
            print(f"  Episodic memory: {episodic_memory_usage}/{episodic_memory_capacity} episodes")
        
        # Neuromodulation analysis
        print("\nNeuromodulation Analysis:")
        if self.brain.getDopamine():
            print(f"  Dopamine level: {self.brain.getAverageDopamineLevel():.3f}")
        if self.brain.getCuriosity():
            print(f"  Curiosity level: {self.brain.getAverageCuriosityLevel():.3f}")
        if self.brain.getNovelty():
            print(f"  Novelty level: {self.brain.getAverageNoveltyLevel():.3f}")
        if self.brain.getPredictionErrorSignal():
            print(f"  Prediction error: {self.brain.getAveragePredictionError():.3f}")
        
        # Development analysis
        print("\nDevelopment Analysis:")
        developmental_stage = self.brain.getDevelopmentalStage()
        print(f"  Current stage: {developmental_stage}")
        print(f"  Plasticity rate: {self.brain.getPlasticityRate():.3f}")
        print(f"  Structural plasticity rate: {self.brain.getStructuralPlasticityRate():.3f}")
        
        # Visualization analysis
        print("\nVisualization Analysis:")
        print(f"  Performance monitoring enabled: {self.brain.isPerformanceMonitoringEnabled()}")
        print(f"  Debug mode: {self.brain.isDebugMode()}")
        print(f"  Simulation paused: {self.brain.isSimulationPaused()}")
        print(f"  Event-driven simulation: {self.brain.isEventDrivenSimulationEnabled()}")
        
        # Error analysis
        print("\nError Analysis:")
        # Note: This would require access to the error handler
        print("  Error statistics would be available through error handler")
        
    def export_results(self, output_dir="./nlm_results"):
        """Export simulation results and analysis."""
        print(f"\n=== Exporting Results to {output_dir} ===")
        
        import os
        os.makedirs(output_dir, exist_ok=True)
        
        # Export simulation log
        log_file = os.path.join(output_dir, "simulation_log.json")
        with open(log_file, 'w') as f:
            json.dump(self.simulation_log, f, indent=2, default=str)
        print(f"  Simulation log saved to {log_file}")
        
        # Export performance statistics
        stats_file = os.path.join(output_dir, "performance_stats.json")
        with open(stats_file, 'w') as f:
            json.dump(self.performance_stats, f, indent=2)
        print(f"  Performance statistics saved to {stats_file}")
        
        # Export configuration
        config_file = os.path.join(output_dir, "configuration.json")
        with open(config_file, 'w') as f:
            # Convert config to dict (simplified)
            config_dict = {}
            for key in self.config.getKeys():
                config_dict[key] = str(self.config.getOr(key, ""))
            json.dump(config_dict, f, indent=2)
        print(f"  Configuration saved to {config_file}")
        
        # Export brain state
        brain_file = os.path.join(output_dir, "brain_state.checkpoint")
        if self.brain.save(brain_file):
            print(f"  Brain state saved to {brain_file}")
        else:
            print(f"  Failed to save brain state")
        
        print(f"\nResults exported to {output_dir}")
        
    def run_advanced_example(self):
        """Run the complete advanced example."""
        print("NLM Advanced Example - Phase 6 Deep Dive")
        print("=" * 60)
        
        try:
            # Setup configuration
            self.setup_advanced_configuration()
            
            # Create brain
            self.create_brain_with_monitoring()
            
            # Setup agent system
            self.setup_agent_system()
            
            # Run simulation
            self.run_advanced_simulation(2000)
            
            # Run performance analysis
            self.run_performance_analysis()
            
            # Export results
            self.export_results()
            
            print("\n" + "=" * 60)
            print("Advanced example completed successfully!")
            print("\nKey achievements:")
            print("  ✓ Advanced configuration with all available options")
            print("  ✓ Enhanced plasticity control and neuromodulation")
            print("  ✓ Real-time performance monitoring")
            print("  ✓ Agent-world interaction system")
            print("  ✓ Comprehensive performance analysis")
            print("  ✓ Result export and documentation")
            
        except Exception as e:
            print(f"\nError in advanced example: {e}")
            import traceback
            traceback.print_exc()
            return False
        
        return True

def main():
    """Main function to run the advanced example."""
    example = NLMAdvancedExample()
    success = example.run_advanced_example()
    
    if success:
        print("\nAdvanced NLM example completed successfully!")
        return 0
    else:
        print("\nAdvanced NLM example failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
