#!/usr/bin/env python3
"""
NLM Research Example - Using Advanced Features

This example demonstrates advanced features for research and experimentation:
- Memory recording for experience tracking
- Statistical analysis of neural activity
- Configuration optimization
- Performance benchmarking
- Data export and analysis

Author: NLM Research Team
Version: Advanced Edition
"""

import pynlm
import json
import time
from datetime import datetime

class NLMResearchExperiment:
    """Research experiment class for managing NLM experiments"""
    
    def __init__(self, name, config_type="expert"):
        self.name = name
        self.start_time = time.time()
        self.config_type = config_type
        
        # Create experimental setup
        if config_type == "expert":
            self.config, self.brain, self.world, self.agent = pynlm.create_experimental_setup()
        elif config_type == "quick":
            self.config, self.brain, self.world, self.agent = pynlm.create_debug_setup()
        else:
            self.config, self.brain, self.world, self.agent = pynlm.create_experimental_setup()
        
        self.agent.initialize(self.world)
        self.memory_recorder = pynlm.createMemoryRecorder()
        self.simulation_results = []
        
        # Enable all research subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_experiment(self, num_steps=500, timestep=0.1):
        """Run the experiment with memory recording"""
        print(f"Running {self.name} - {num_steps} steps, {timestep}s timestep")
        
        # Add environmental variety
        self._setup_environment()
        
        # Run simulation
        for step in range(num_steps):
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Record experience
            self.memory_recorder.record(step, percept)
            
            # Process in brain
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action and apply
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(timestep)
            
            # Record step results
            step_results = {
                'step': step,
                'time': self.world.getSimulationTime(),
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'reward': reward,
                'development_stage': int(self.brain.getDevelopmentalStage()),
                'action_type': str(action.getType())
            }
            self.simulation_results.append(step_results)
            
            # Progress reporting
            if step % 100 == 0:
                print(f"  Step {step}/{num_steps}")
        
        print(f"Experiment {self.name} completed successfully!")
    
    def _setup_environment(self):
        """Setup diverse environment for research"""
        print("Setting up research environment...")
        
        # Add resources at various positions
        for i in range(20):
            x = (i * 0.5) % self.world.getWidth()
            y = (i * 0.7) % self.world.getHeight()
            value = 1.0 + (i % 5) * 0.5  # Variable resource values
            self.world.add_resource(x, y, value)
        
        # Add hazards
        hazard_positions = [(5.0, 5.0), (15.0, 15.0), (8.0, 12.0)]
        for x, y in hazard_positions:
            self.world.add_hazard(x, y, damage=0.3)
        
        # Add walls for structure
        for i in range(5):
            wall_x = i * 4.0
            self.world.add_wall(wall_x, 10.0, width=0.5)
            self.world.add_wall(wall_x, 15.0, width=0.5)
    
    def analyze_results(self):
        """Analyze experiment results"""
        print("\n=== Experiment Analysis ===")
        
        # Basic statistics
        total_steps = len(self.simulation_results)
        avg_firing_rate = sum(r['neurons_firing'] for r in self.simulation_results) / total_steps
        total_spikes = sum(r['total_spikes'] for r in self.simulation_results)
        
        # Development statistics
        curiosity_levels = [r['curiosity'] for r in self.simulation_results]
        novelty_levels = [r['novelty'] for r in self.simulation_results]
        
        # Learning analysis
        early_steps = self.simulation_results[:100]
        late_steps = self.simulation_results[-100:]
        
        print(f"Experiment: {self.name}")
        print(f"Duration: {total_steps} steps, {time.time() - self.start_time:.1f}s")
        print(f"Neural Activity:")
        print(f"  Average firing neurons: {avg_firing_rate:.1f}")
        print(f"  Total spikes generated: {total_spikes}")
        print(f"  Curiosity levels: avg={sum(curiosity_levels)/len(curiosity_levels):.3f}, "
              f"max={max(curiosity_levels):.3f}")
        print(f"  Novelty levels: avg={sum(novelty_levels)/len(novelty_levels):.3f}")
        
        # Development progression
        if early_steps and late_steps:
            early_curiosity = sum(r['curiosity'] for r in early_steps) / len(early_steps)
            late_curiosity = sum(r['curiosity'] for r in late_steps) / len(late_steps)
            early_novelty = sum(r['novelty'] for r in early_steps) / len(early_steps)
            late_novelty = sum(r['novelty'] for r in late_steps) / len(late_steps)
            
            print(f"\nLearning Progression (first 100 vs last 100 steps):")
            print(f"  Curiosity: {early_curiosity:.3f} → {late_curiosity:.3f} "
                  f"({'increase' if late_curiosity > early_curiosity else 'decrease'})")
            print(f"  Novelty: {early_novelty:.3f} → {late_novelty:.3f} "
                  f"({'increase' if late_novelty > early_novelty else 'decrease'})")
        
        # Action analysis
        action_types = {}
        for r in self.simulation_results:
            action_type = r['action_type']
            action_types[action_type] = action_types.get(action_type, 0) + 1
        
        print(f"\nAction Distribution:")
        for action_type, count in sorted(action_types.items(), key=lambda x: x[1], reverse=True):
            percentage = (count / total_steps) * 100
            print(f"  {action_type}: {count} ({percentage:.1f}%)")
        
        # Environment interaction
        resource_count = sum(1 for r in self.simulation_results if r['reward'] > 0)
        print(f"\nEnvironment Interaction:")
        print(f"  Steps with positive reward: {resource_count} ({resource_count/total_steps*100:.1f}%)")
    
    def export_results(self, filename_prefix="experiment"):
        """Export results to files for analysis"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Export neural statistics
        neural_stats = pynlm.compute_neural_statistics(self.brain)
        neural_stats['experiment'] = self.name
        neural_stats['config_type'] = self.config_type
        neural_stats['total_steps'] = len(self.simulation_results)
        
        with open(f"{filename_prefix}_{timestamp}_neural_stats.json", 'w') as f:
            json.dump(neural_stats, f, indent=2)
        
        # Export step-by-step data
        step_data = {
            'experiment': self.name,
            'config_type': self.config_type,
            'steps': self.simulation_results
        }
        
        with open(f"{filename_prefix}_{timestamp}_step_data.json", 'w') as f:
            json.dump(step_data, f, indent=2)
        
        # Export memory recording if significant
        if self.memory_recorder.size() > 100:
            self.memory_recorder.save_to_file(f"{filename_prefix}_{timestamp}_memory.csv")
        
        print(f"Results exported to files with prefix: {filename_prefix}_{timestamp}")
    
    def get_comprehensive_report(self):
        """Get comprehensive research report"""
        report = {
            'experiment_name': self.name,
            'config_type': self.config_type,
            'duration_seconds': time.time() - self.start_time,
            'total_steps': len(self.simulation_results),
            'neural_stats': pynlm.compute_neural_statistics(self.brain),
            'development_stats': {
                'curiosity_avg': sum(r['curiosity'] for r in self.simulation_results) / len(self.simulation_results),
                'novelty_avg': sum(r['novelty'] for r in self.simulation_results) / len(self.simulation_results),
                'development_stage_avg': sum(r['development_stage'] for r in self.simulation_results) / len(self.simulation_results)
            },
            'action_distribution': {},
            'learning_progress': {}
        }
        
        # Add action distribution
        action_counts = {}
        for r in self.simulation_results:
            action_type = r['action_type']
            action_counts[action_type] = action_counts.get(action_type, 0) + 1
        report['action_distribution'] = action_counts
        
        return report


def run_research_demo():
    """Demonstrate research capabilities"""
    print("=== NLM Research Demonstration ===")
    print("This demo shows advanced research capabilities\n")
    
    # Create research experiments
    experiments = [
        NLMResearchExperiment("Quick Learning Test", "quick"),
        NLMResearchExperiment("Standard Research", "default"),
        NLMResearchExperiment("Expert Neuroscience", "expert")
    ]
    
    all_results = []
    
    # Run each experiment
    for exp in experiments:
        print(f"\n{'='*60}")
        print(f"Running {exp.name}")
        print(f"{'='*60}")
        
        # Run experiment
        exp.run_experiment(num_steps=200, timestep=0.1)
        
        # Analyze results
        exp.analyze_results()
        
        # Export data
        exp.export_results(f"nlm_{exp.name.lower().replace(' ', '_')}")
        
        # Get comprehensive report
        report = exp.get_comprehensive_report()
        all_results.append(report)
    
    # Comparative analysis
    print(f"\n{'='*60}")
    print("COMPARATIVE ANALYSIS")
    print(f"{'='*60}")
    
    for result in all_results:
        print(f"\n{result['experiment_name']} ({result['config_type']} config):")
        print(f"  Neural efficiency: {result['neural_stats']['firing_rate']/result['neural_stats']['total_neurons']:.4f}")
        print(f"  Learning score: {result['development_stats']['curiosity_avg']:.3f}")
    
    print(f"\nResearch demonstration completed successfully!")
    print(f"All experiments exported for analysis.")


if __name__ == "__main__":
    try:
        run_research_demo()
    except Exception as e:
        print(f"Error during research demonstration: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
