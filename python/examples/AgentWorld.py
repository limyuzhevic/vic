# NLM Advanced Examples and Tutorials

This directory contains advanced examples and tutorials for the NLM Python bindings. These examples demonstrate complex usage patterns and advanced features.

## Contents

1. **LearningExperiment.py** - Advanced learning and plasticity experiments
2. **AgentWorld.py** - Complete agent-based world simulation
3. **PatternAnalysis.py** - Neural pattern analysis and visualization
4. **ConfigurationExamples.py** - Advanced configuration management
5. **PerformanceBenchmark.py** - Performance benchmarking and optimization
6. **MultiAgentSimulation.py** - Multi-agent coordination and social learning
7. **DevelopmentStages.py** - Developmental stage analysis and tracking
8. **MemorySystems.py** - Working memory and memory consolidation experiments

## LearningExperiment.py

Advanced learning experiment demonstrating synaptic plasticity, memory consolidation, and developmental changes.

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import json
from datetime import datetime

class AdvancedLearningExperiment:
    """Advanced learning experiment with multiple interconnected systems."""
    
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        
        if config_file:
            self.config.loadFromFile(config_file)
        else:
            # Enhanced configuration for learning experiments
            self.config.set_int("brain.neuron_count", 2000)
            self.config.set_float("brain.synapse_density", 0.15)
            self.config.set_float("plasticity.stdp.enable", True)
            self.config.set_float("plasticity.hebbian.enable", True)
            self.config.set_float("plasticity.structural.enable", True)
            self.config.set_float("neuromod.dopamine.scale", 2.0)
            self.config.set_float("neuromod.curiosity.enable", True)
            self.config.set_float("neuromod.novelty.enable", True)
            self.config.set_int("random_seed", 42)
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.brain.initialize()
        
        # Initialize subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Data collection
        self.data = {
            'steps': [],
            'synaptic_weights': [],
            'firing_rates': [],
            'novelty_levels': [],
            'curiosity_levels': [],
            'prediction_errors': [],
            'developmental_stages': [],
            'reward_signals': [],
            'memory_traces': []
        }
        
        self.simulation_time = 0.0
        self.target_patterns = self.generateTargetPatterns()
        
    def generateTargetPatterns(self):
        """Generate target patterns for learning."""
        patterns = {
            'simple_linear': np.random.randn(100) * 0.1,
            'complex_spatial': np.random.rand(2500) * 0.5,
            'temporal_sequence': np.sin(np.linspace(0, 4*np.pi, 100)),
            'binary_pattern': np.random.choice([0, 1], size=1000, p=[0.7, 0.3])
        }
        return patterns
    
    def applyPatternedInput(self, pattern_name, step):
        """Apply specific input patterns to stimulate learning."""
        pattern = self.target_patterns.get(pattern_name, self.target_patterns['simple_linear'])
        
        # Map pattern to sensory neurons based on type
        if pattern_name == 'simple_linear':
            # Apply to vision neurons
            vision_neuron_count = self.agent.getSensoryInputSize()
            for i in range(min(len(pattern), vision_neuron_count)):
                current = pattern[i] * 20.0  # Scale to reasonable current range
                self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, current)
                
        elif pattern_name == 'complex_spatial':
            # Apply distributed input
            for i in range(min(len(pattern), self.brain.getTotalNeuronCount())):
                current = pattern[i] * 5.0
                self.brain.injectCurrent(self.brain.getRegion(pynlm.RegionId(1))->getAllNeurons()[i % 100], current)
                
        elif pattern_name == 'temporal_sequence':
            # Time-dependent input
            current = pattern[step % len(pattern)] * 15.0
            self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, current)
            
        elif pattern_name == 'binary_pattern':
            # Sparse binary pattern
            if pattern[step % len(pattern)] > 0.5:
                self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 25.0)
    
    def calculateSynapticStats(self):
        """Calculate detailed synaptic statistics."""
        total_weight = 0.0
        weight_count = 0
        weight_variance = 0.0
        weight_mean = 0.0
        
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                weight = synapse.getWeight()
                total_weight += weight
                weight_count += 1
                weight_variance += (weight - weight_mean) * (weight - weight_mean) / weight_count
                weight_mean = total_weight / weight_count
        
        return {
            'mean_weight': weight_mean,
            'weight_variance': weight_variance,
            'total_synapses': weight_count,
            'total_weight': total_weight
        }
    
    def detectLearningProgress(self, step):
        """Detect if learning is occurring through multiple indicators."""
        stats = self.calculateSynapticStats()
        novelty = self.agent.getNoveltyLevel()
        curiosity = self.agent.getCuriosityLevel()
        prediction_error = self.agent.getPredictionError()
        
        # Learning indicators
        weight_change = abs(stats['mean_weight'] - self.previous_stats.get('mean_weight', stats['mean_weight']))
        weight_stable = weight_change < 0.01
        
        novelty_high = novelty > 0.5
        curiosity_active = curiosity > 0.3
        prediction_active = abs(prediction_error) > 0.1
        
        # Learning is considered occurring if any of these are true
        learning_detected = (
            weight_change > 0.01 or
            novelty_high or
            curiosity_active or
            prediction_active or
            (step > 100 and weight_stable and novelty_high)
        )
        
        return {
            'learning_detected': learning_detected,
            'weight_change': weight_change,
            'novelty': novelty,
            'curiosity': curiosity,
            'prediction_error': prediction_error,
            'weight_stable': weight_stable
        }
    
    def runPhase(self, phase_name, steps, pattern_name='simple_linear'):
        """Run a specific experimental phase."""
        print(f"Starting {phase_name} (Phase {self.brain.getDevelopmentalStage()})...")
        
        phase_data = {
            'steps': [],
            'weights': [],
            'firing_rates': [],
            'novelty': [],
            'curiosity': [],
            'prediction_errors': []
        }
        
        for step in range(steps):
            # Record previous stats for learning detection
            self.previous_stats = self.calculateSynapticStats()
            
            # Apply patterned input
            self.applyPatternedInput(pattern_name, step)
            
            # Process world and brain
            # (In a real experiment, this would include world updates, etc.)
            self.brain.step(step, self.simulation_time)
            
            # Record data
            phase_data['steps'].append(step)
            phase_data['weights'].append(self.calculateSynapticStats()['mean_weight'])
            phase_data['firing_rates'].append(self.brain.getFiringNeuronCount())
            phase_data['novelty'].append(self.agent.getNoveltyLevel())
            phase_data['curiosity'].append(self.agent.getCuriosityLevel())
            phase_data['prediction_errors'].append(self.agent.getPredictionError())
            
            # Apply neuromodulation based on performance
            reward = 0.0
            if step % 50 == 0:
                # Simulate reward based on performance
                firing_rate = self.brain.getFiringNeuronCount()
                reward = min(1.0, firing_rate / 500.0)  # Normalize reward
                self.agent.applyRewardModulation(reward, 0.0)
            
            self.simulation_time += 0.1
            
            # Print progress
            if step % 50 == 0:
                learning = self.detectLearningProgress(step)
                print(f"  Step {step}: Firing={self.brain.getFiringNeuronCount()}, "
                      f"Learning={learning['learning_detected']}, Novelty={novelty:.3f}")
        
        print(f"Completed {phase_name}")
        return phase_data
    
    def analyzeResults(self, phase_data):
        """Analyze experimental results and generate insights."""
        import numpy as np
        
        steps = phase_data['steps']
        weights = phase_data['weights']
        firing_rates = phase_data['firing_rates']
        novelty = phase_data['novelty']
        curiosity = phase_data['curiosity']
        prediction_errors = phase_data['prediction_errors']
        
        # Calculate statistics
        weight_trend = np.polyfit(steps, weights, 1)[0]  # Linear trend
        firing_trend = np.polyfit(steps, firing_rates, 1)[0]
        
        # Find learning milestones
        weight_stabilization_step = None
        novelty_peak_step = None
        curiosity_peak_step = None
        
        for i, (w, n, c) in enumerate(zip(weights, novelty, curiosity)):
            if weight_stabilization_step is None and i > 50 and abs(w - weights[0]) < 0.01:
                weight_stabilization_step = i
            
            if novelty_peak_step is None and n == max(novelty):
                novelty_peak_step = i
            
            if curiosity_peak_step is None and c == max(curiosity):
                curiosity_peak_step = i
        
        insights = {
            'total_steps': len(steps),
            'weight_trend': weight_trend,
            'firing_trend': firing_trend,
            'final_weight': weights[-1],
            'initial_weight': weights[0],
            'weight_change': weights[-1] - weights[0],
            'max_novelty': max(novelty),
            'max_curiosity': max(curiosity),
            'avg_prediction_error': np.mean(prediction_errors),
            'weight_stabilization_step': weight_stabilization_step,
            'novelty_peak_step': novelty_peak_step,
            'curiosity_peak_step': curiosity_peak_step,
            'converged': abs(weight_trend) < 0.001
        }
        
        return insights
    
    def saveExperimentData(self, filename):
        """Save experiment data to JSON file."""
        data = {
            'configuration': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'developmental_stage': str(self.brain.getDevelopmentalStage())
            },
            'final_stats': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'excitatory_inhibitory_ratio': self.brain.getExcitationInhibitionRatio()
            },
            'learning_outcomes': self.analyzeResults(self.data)
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Experiment data saved to {filename}")
    
    def runCompleteExperiment(self):
        """Run the complete advanced learning experiment."""
        print("=== Advanced Learning Experiment ===")
        
        # Phase 1: Initial learning with simple patterns
        phase1_data = self.runPhase("Initial Learning", 200, 'simple_linear')
        
        # Phase 2: Complex pattern learning
        phase2_data = self.runPhase("Complex Pattern Learning", 300, 'complex_spatial')
        
        # Phase 3: Temporal sequence learning
        phase3_data = self.runPhase("Temporal Sequence Learning", 200, 'temporal_sequence')
        
        # Phase 4: Binary pattern learning
        phase4_data = self.runPhase("Binary Pattern Learning", 150, 'binary_pattern')
        
        # Phase 5: Integration and consolidation
        phase5_data = self.runPhase("Integration and Consolidation", 100, 'simple_linear')
        
        # Analyze all phases
        print("\n=== Experiment Analysis ===")
        
        for phase_name, phase_data in [
            ('Phase 1', phase1_data),
            ('Phase 2', phase2_data),
            ('Phase 3', phase3_data),
            ('Phase 4', phase4_data),
            ('Phase 5', phase5_data)
        ]:
            insights = self.analyzeResults(phase_data)
            print(f"{phase_name}:")
            print(f"  Steps: {insights['total_steps']}")
            print(f"  Weight Change: {insights['weight_change']:.4f}")
            print(f"  Learning Converged: {insights['converged']}")
            print(f"  Max Novelty: {insights['max_novelty']:.3f}")
            print(f"  Max Curiosity: {insights['max_curiosity']:.3f}")
        
        # Save results
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.saveExperimentData(f"advanced_learning_experiment_{timestamp}.json")
        
        print("\n=== Experiment Complete ===")
        return {
            'phase1': phase1_data,
            'phase2': phase2_data,
            'phase3': phase3_data,
            'phase4': phase4_data,
            'phase5': phase5_data
        }

# Example usage
if __name__ == "__main__":
    # Create and run advanced learning experiment
    experiment = AdvancedLearningExperiment()
    results = experiment.runCompleteExperiment()
    
    # Print summary
    print("\n=== Experiment Summary ===")
    print(f"Total neurons: {experiment.brain.getTotalNeuronCount()}")
    print(f"Total synapses: {experiment.brain.getTotalSynapseCount()}")
    print(f"Final developmental stage: {experiment.brain.getDevelopmentalStage()}")
    print(f"Total spikes: {experiment.brain.getTotalSpikeCount()}")
    print(f"Learning achieved: {experiment.agent.getCuriosityLevel() > 0.5}")
