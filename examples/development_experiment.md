# ADVANCED NLM EXAMPLES

## Overview

This section contains advanced NLM examples demonstrating real-world use cases, research experiments, and complex applications. These examples showcase the full power of NLM for scientific research and engineering applications.

## Example 1: Cognitive Development Experiment

### Overview
This example simulates cognitive development over an extended time period, examining how different developmental stages affect learning and behavior.

### Learning Objectives
- Understand developmental stage effects on plasticity
- Observe emergence of cognitive abilities
- Analyze learning curves across developmental periods
- Implement custom developmental parameters

### Experimental Setup
```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class CognitiveDevelopmentExperiment:
    def __init__(self):
        self.experiment_data = {
            'steps': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'developmental_stages': [],
            'learning_metrics': []
        }
        
    def run_development_experiment(self, num_steps=5000):
        """Run comprehensive development experiment"""
        
        # 1. Create developmentally-sensitive brain
        config = self.create_development_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 2. Create agent and world
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world for developmental exploration
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent.initialize(world)
        
        # 3. Enable all learning subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 4. Run development experiment
        for step in range(num_steps):
            # Update environment
            world.update(0.1)
            
            # Get environment observations
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain processes and learns
            brain.step(step)
            
            # Get motor commands
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.calculateDevelopmentalReward(world, step)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record data
            self.recordExperimentalData(step, brain, agent, world)
            
            # Apply developmental effects
            self.applyDevelopmentalEffects(brain, step)
            
            # Print progress
            if step % 500 == 0:
                self.printProgress(step, brain, agent)
        
        return self.analyzeDevelopmentResults()
    
    def create_development_config(self):
        """Create configuration optimized for developmental studies"""
        config = pynlm.createDefaultConfig()
        
        # Large network for rich dynamics
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.02)
        
        # Development-sensitive parameters
        config.set("brain.v_thresh", -50.0)
        config.set("brain.tau_mem", 15.0)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Neuromodulation for developmental effects
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        return config
    
    def calculateDevelopmentalReward(self, world, step):
        """Calculate reward that varies with developmental stage"""
        
        base_reward = 0.1
        
        # Early development: reward exploration
        if step < 1000:
            return base_reward * 0.5
        
        # Critical period: reward novelty
        elif step < 2000:
            novelty_factor = world.getSensoryPercept().getNovelty() or 1.0
            return base_reward * (1.0 + novelty_factor * 2.0)
        
        # Maturation: reward efficiency
        elif step < 3500:
            # Reward efficient behavior
            return base_reward * 2.0
        
        # Adult: balanced rewards
        else:
            return base_reward
    
    def applyDevelopmentalEffects(self, brain, step):
        """Apply stage-specific developmental effects"""
        
        developmental_stage = brain.getDevelopmentalStage()
        
        if developmental_stage == pynlm.DevelopmentalStage.CriticalPeriod:
            # Critical period: enhanced plasticity
            config = pynlm.createDefaultConfig()
            config.set("plasticity.stdp.learning_rate", 0.005)
            config.set("plasticity.hebbian.enable", True)
            config.set("neuromod.dopamine.scale", 2.0)
            
        elif developmental_stage == pynlm.DevelopmentalStage.Maturation:
            # Maturation: balanced plasticity
            config = pynlm.createDefaultConfig()
            config.set("plasticity.stdp.learning_rate", 0.001)
            config.set("plasticity.hebbian.enable", True)
            config.set("neuromod.dopamine.scale", 1.0)
            
        elif developmental_stage == pynlm.DevelopmentalStage.Adult:
            # Adult: stable performance
            config = pynlm.createDefaultConfig()
            config.set("plasticity.stdp.learning_rate", 0.0005)
            config.set("plasticity.hebbian.enable", True)
            config.set("neuromod.dopamine.scale", 0.5)
    
    def recordExperimentalData(self, step, brain, agent, world):
        """Record experimental data for analysis"""
        
        self.experiment_data['steps'].append(step)
        self.experiment_data['firing_rates'].append(brain.getAverageFiringRate())
        self.experiment_data['curiosity_levels'].append(agent.getCuriosityLevel())
        self.experiment_data['novelty_levels'].append(agent.getNoveltyLevel())
        self.experiment_data['developmental_stages'].append(
            brain.getDevelopmentalStage().value)
        
        # Calculate learning metrics
        learning_metric = self.calculateLearningMetric(brain, agent, step)
        self.experiment_data['learning_metrics'].append(learning_metric)
    
    def calculateLearningMetric(self, brain, agent, step):
        """Calculate learning progress metric"""
        
        # Combine multiple indicators of learning
        firing_stability = self.calculateFiringStability(brain, step)
        curiosity_consistency = self.calculateCuriosityConsistency(agent, step)
        novelty_response = self.calculateNoveltyResponse(agent, step)
        
        # Weighted combination
        learning_metric = (
            0.4 * firing_stability +
            0.3 * curiosity_consistency +
            0.3 * novelty_response
        )
        
        return learning_metric
    
    def calculateFiringStability(self, brain, step):
        """Calculate stability of neural firing"""
        
        if step < 10:
            return 0.0
        
        # Calculate coefficient of variation of firing rate
        recent_rates = self.experiment_data['firing_rates'][-10:]
        if len(recent_rates) < 2:
            return 0.0
        
        mean_rate = np.mean(recent_rates)
        std_rate = np.std(recent_rates)
        cv = std_rate / (mean_rate + 1e-6)
        
        # Lower CV indicates more stable, learned behavior
        return np.exp(-cv)
    
    def calculateCuriosityConsistency(self, agent, step):
        """Calculate consistency of curiosity-driven exploration"""
        
        if step < 10:
            return 0.0
        
        # Look for sustained curiosity
        curiosity_levels = self.experiment_data['curiosity_levels'][-20:]
        mean_curiosity = np.mean(curiosity_levels)
        
        # Curiously, mature systems show moderate, stable curiosity
        if mean_curiosity > 0.3 and mean_curiosity < 0.7:
            return 1.0
        else:
            return 0.0
    
    def calculateNoveltyResponse(self, agent, step):
        """Calculate response to novel stimuli"""
        
        # Novelty levels should fluctuate initially, then stabilize
        if step < 50:
            return 0.0  # Too early to judge
        
        novelty_levels = self.experiment_data['novelty_levels'][-50:]
        
        # Look for appropriate novelty response
        # Early development: high novelty response
        # Mature systems: balanced novelty response
        avg_novelty = np.mean(novelty_levels)
        
        if step < 1000:  # Early development
            return avg_novelty  # Should be high
        else:  # Mature systems
            return 1.0 - abs(avg_novelty - 0.5) * 2.0  # Should be around 0.5
    
    def printProgress(self, step, brain, agent):
        """Print experimental progress"""
        print(f"Step {step:4d}: "
              f"Firing: {brain.getAverageFiringRate():6.2f} Hz, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}, "
              f"Stage: {brain.getDevelopmentalStage().name}")
    
    def analyzeDevelopmentResults(self):
        """Analyze and return development experiment results"""
        
        results = {
            'stage_transitions': self.analyzeStageTransitions(),
            'learning_curves': self.analyzeLearningCurves(),
            'behavior_patterns': self.analyzeBehaviorPatterns(),
            'development_metrics': self.calculateDevelopmentMetrics()
        }
        
        return results
    
    def analyzeStageTransitions(self):
        """Analyze transitions between developmental stages"""
        
        stages = self.experiment_data['developmental_stages']
        transitions = []
        
        for i in range(1, len(stages)):
            if stages[i] != stages[i-1]:
                transition = {
                    'from_stage': stages[i-1],
                    'to_stage': stages[i],
                    'step': self.experiment_data['steps'][i],
                    'time': self.experiment_data['steps'][i] * 0.1
                }
                transitions.append(transition)
        
        return transitions
    
    def analyzeLearningCurves(self):
        """Analyze learning curves for different metrics"""
        
        learning_curves = {
            'firing_rate': self.experiment_data['firing_rates'],
            'curiosity': self.experiment_data['curiosity_levels'],
            'novelty': self.experiment_data['novelty_levels'],
            'learning_metric': self.experiment_data['learning_metrics']
        }
        
        return learning_curves
    
    def analyzeBehaviorPatterns(self):
        """Analyze emergent behavior patterns"""
        
        patterns = {
            'exploration_phases': self.identifyExplorationPhases(),
            'learning_phases': self.identifyLearningPhases(),
            'behavioral_regularity': self.calculateBehavioralRegularity()
        }
        
        return patterns
    
    def calculateDevelopmentMetrics(self):
        """Calculate overall development metrics"""
        
        metrics = {
            'plasticity_integral': self.calculatePlasticityIntegral(),
            'exploration_integral': self.calculateExplorationIntegral(),
            'stability_score': self.calculateStabilityScore(),
            'maturity_score': self.calculateMaturityScore()
        }
        
        return metrics
    
    def calculatePlasticityIntegral(self):
        """Calculate cumulative plasticity effects"""
        
        # Approximate by summing changes in connectivity
        stages = self.experiment_data['developmental_stages']
        if len(stages) < 2:
            return 0.0
        
        # Count stage transitions as plasticity events
        transitions = sum(1 for i in range(1, len(stages)) 
                         if stages[i] != stages[i-1])
        
        return transitions
    
    def calculateExplorationIntegral(self):
        """Calculate cumulative exploration behavior"""
        
        curiosity_levels = self.experiment_data['curiosity_levels']
        if not curiosity_levels:
            return 0.0
        
        # Area under curiosity curve
        return np.trapz(curiosity_levels, dx=1.0)
    
    def calculateStabilityScore(self):
        """Calculate behavioral stability"""
        
        firing_rates = self.experiment_data['firing_rates']
        if len(firing_rates) < 10:
            return 0.0
        
        # Recent stability (last 10% of experiment)
        recent_rates = firing_rates[-len(firing_rates)//10:]
        if len(recent_rates) < 2:
            return 0.0
        
        cv = np.std(recent_rates) / (np.mean(recent_rates) + 1e-6)
        
        # Lower CV = higher stability
        return np.exp(-cv)
    
    def calculateMaturityScore(self):
        """Calculate developmental maturity"""
        
        stages = self.experiment_data['developmental_stages']
        if not stages:
            return 0.0
        
        # Final stage reached
        final_stage = stages[-1]
        
        # Map stages to maturity scores
        maturity_map = {
            0: 0.2,  # Initial
            1: 0.5,  # CriticalPeriod
            2: 0.7,  # Maturation
            3: 0.9,  # Adult
            4: 1.0   # Aging
        }
        
        return maturity_map.get(final_stage, 0.0)
    
    def identifyExplorationPhases(self):
        """Identify phases of exploration and exploitation"""
        
        curiosity_levels = self.experiment_data['curiosity_levels']
        if len(curiosity_levels) < 100:
            return []
        
        phases = []
        current_phase = 'exploration'
        phase_start = 0
        
        for i, curiosity in enumerate(curiosity_levels):
            if curiosity < 0.2 and current_phase == 'exploration' and i > 10:
                # Transition to exploitation
                phases.append({
                    'type': current_phase,
                    'start_step': phase_start,
                    'end_step': i-1,
                    'duration': (i-1) - phase_start
                })
                
                current_phase = 'exploitation'
                phase_start = i
            
            elif curiosity > 0.7 and current_phase == 'exploitation' and i > 100:
                # Transition back to exploration
                phases.append({
                    'type': current_phase,
                    'start_step': phase_start,
                    'end_step': i-1,
                    'duration': (i-1) - phase_start
                })
                
                current_phase = 'exploration'
                phase_start = i
        
        # Add final phase
        phases.append({
            'type': current_phase,
            'start_step': phase_start,
            'end_step': len(curiosity_levels)-1,
            'duration': len(curiosity_levels)-1 - phase_start
        })
        
        return phases
    
    def identifyLearningPhases(self):
        """Identify distinct learning phases"""
        
        learning_metrics = self.experiment_data['learning_metrics']
        if len(learning_metrics) < 50:
            return []
        
        phases = []
        phase_type = 'rapid_learning'
        phase_start = 0
        
        # Find phase transitions based on learning rate changes
        for i in range(50, len(learning_metrics)):
            window = learning_metrics[i-50:i]
            current_trend = calculate_trend(window)
            
            if current_trend < 0.01 and phase_type == 'rapid_learning':
                # Transition to consolidation
                phases.append({
                    'type': phase_type,
                    'start_step': phase_start,
                    'end_step': i-51,
                    'duration': (i-51) - phase_start
                })
                
                phase_type = 'consolidation'
                phase_start = i-50
            
            elif current_trend > 0.02 and phase_type == 'consolidation':
                # Rapid learning resumed
                phases.append({
                    'type': phase_type,
                    'start_step': phase_start,
                    'end_step': i-51,
                    'duration': (i-51) - phase_start
                })
                
                phase_type = 'rapid_learning'
                phase_start = i-50
        
        # Add final phase
        phases.append({
            'type': phase_type,
            'start_step': phase_start,
            'end_step': len(learning_metrics)-51,
            'duration': len(learning_metrics)-51 - phase_start
        })
        
        return phases
    
    def calculateBehavioralRegularity(self):
        """Calculate regularity of behavioral patterns"""
        
        firing_rates = self.experiment_data['firing_rates']
        if len(firing_rates) < 100:
            return 0.0
        
        # Calculate various regularity measures
        # 1. Coefficient of variation
        cv = np.std(firing_rates) / (np.mean(firing_rates) + 1e-6)
        
        # 2. Autocorrelation at lag 1
        if len(firing_rates) > 1:
            autocorr = np.corrcoef(firing_rates[:-1], firing_rates[1:])[0, 1]
        else:
            autocorr = 0.0
        
        # 3. Spectral power (indicates rhythmic patterns)
        fft_result = np.fft.fft(firing_rates)
        power_spectrum = np.abs(fft_result) ** 2
        dominant_frequency = np.argmax(power_spectrum[1:]) + 1
        power_ratio = power_spectrum[dominant_frequency] / np.sum(power_spectrum)
        
        # Combine measures
        regularity = (
            0.4 * np.exp(-cv) +
            0.3 * abs(autocorr) +
            0.3 * power_ratio
        )
        
        return regularity

# Helper function for trend calculation
def calculate_trend(values, window=10):
    """Calculate linear trend in a series of values"""
    
    if len(values) < window:
        return 0.0
    
    x = np.arange(len(values))
    y = np.array(values)
    
    # Linear regression
    coeffs = np.polyfit(x[-window:], y[-window:], 1)
    
    return coeffs[0]  # Slope

# Run the development experiment
if __name__ == "__main__":
    print("=== Cognitive Development Experiment ===")
    
    experiment = CognitiveDevelopmentExperiment()
    results = experiment.run_development_experiment(5000)
    
    print("\n=== Experiment Results ===")
    print(f"Stage transitions: {len(results['stage_transitions'])}")
    print(f"Developmental maturity score: {results['development_metrics']['maturity_score']:.2f}")
    print(f"Stability score: {results['development_metrics']['stability_score']:.2f}")
    
    print("\nExploration phases:")
    for phase in results['behavior_patterns']['exploration_phases']:
        print(f"  {phase['type']}: steps {phase['start_step']}-{phase['end_step']}")
    
    print("\n✅ Cognitive development experiment complete!")
