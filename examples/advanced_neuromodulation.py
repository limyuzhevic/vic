"""
Advanced Neuromodulation Examples

This module demonstrates advanced neuromodulation strategies for NLM.
Key features include:
- Multi-modulator coordination
- Learning signal processing
- Behavioral control mechanisms
- Adaptive neuromodulation
- Integration with plasticity systems

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import numpy as np
from typing import Dict, List, Callable, Any
import time
class NeuromodulatorBase:
    """
    Base class for neuromodulators.
    
    Neuromodulators are chemical signals that modulate neural activity and plasticity.
    This base class provides common functionality for all neuromodulators.
    """
    
    def __init__(self, name: str, brain: pynlm.Brain):
        self.name = name
        self.brain = brain
        self.level = 0.0
        self.history = []
        self.target_neuron_types = [pynlm.NeuronType.Internal]
        
    def setLevel(self, level: float):
        """Set the neuromodulator level."""
        self.level = max(0.0, min(1.0, level))  # Clamp to [0, 1]
        self.history.append((time.time(), self.level))
        
    def getLevel(self) -> float:
        """Get current neuromodulator level."""
        return self.level
        
    def update(self, time_step: float):
        """
        Update neuromodulator state.
        
        Args:
            time_step: Time step duration
        """
        pass
        
    def apply(self, signal: float):
        """
        Apply neuromodulation signal.
        
        Args:
            signal: Neuromodulation signal strength
        """
        self.setLevel(self.level + signal * 0.1)
        
    def getEffect(self, neuron_type: pynlm.NeuronType) -> float:
        """
        Get modulation effect on specific neuron type.
        
        Args:
            neuron_type: Type of neuron to modulate
            
        Returns:
            Modulation strength (0.0 to 1.0)
        """
        if neuron_type in self.target_neuron_types:
            return self.level
        return 0.0
class DopamineModulator(NeuromodulatorBase):
    """
    Dopamine neuromodulator.
    
    Dopamine modulates:
    - Neural excitability
    - Plasticity strength
    - Action selection
    - Reward prediction
    
    This implementation includes:
    - Reward prediction error processing
    - Incentive salience modulation
    - Working memory enhancement
    - Decision-making influence
    """
    
    def __init__(self, brain: pynlm.Brain):
        super().__init__("dopamine", brain)
        self.target_neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal
        ]
        self.prediction_error = 0.0
        self.incentive_strength = 0.0
        self.learning_rate = 0.01
        
    def setPredictionError(self, error: float):
        """Set prediction error signal."""
        self.prediction_error = error
        self.apply(error)
        
    def getPlasticityFactor(self) -> float:
        """Get plasticity modulation factor."""
        # Dopamine enhances plasticity when prediction error is large
        plasticity_factor = 1.0 + self.prediction_error * 0.5
        return max(0.5, min(2.0, plasticity_factor))
        
    def getDecisionBias(self) -> float:
        """Get decision-making bias."""
        # Dopamine biases toward rewarding actions
        return np.tanh(self.level * 2.0)
        
    def update(self, time_step: float):
        """Update dopamine state."""
        # Decay dopamine level
        self.level *= 0.95
        
        # Apply prediction error decay
        self.prediction_error *= 0.9
class CuriosityModulator(NeuromodulatorBase):
    """
    Curiosity neuromodulator.
    
    Curiosity modulates:
    - Exploration behavior
    - Novelty seeking
    - Information gathering
    - Risk-taking
    
    This implementation includes:
    - Novelty detection
    - Information gain calculation
    - Exploration-exploitation balance
    - Learning progress tracking
    """
    
    def __init__(self, brain: pynlm.Brain):
        super().__init__("curiosity", brain)
        self.target_neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Internal
        ]
        self.novelty_score = 0.0
        self.exploration_rate = 0.1
        self.information_gain = 0.0
        self.interest_level = 0.0
        
    def setNovelty(self, novelty: float):
        """Set novelty level."""
        self.novelty_score = max(0.0, min(1.0, novelty))
        self.apply(novelty)
        
    def setInformationGain(self, gain: float):
        """Set information gain."""
        self.information_gain = gain
        self.interest_level = max(self.interest_level, abs(gain))
        
    def getExplorationBonus(self) -> float:
        """Get exploration bonus."""
        # Curiosity encourages exploration of novel stimuli
        exploration_bonus = self.level * self.exploration_rate
        return min(1.0, exploration_bonus)
        
    def getInterestThreshold(self) -> float:
        """Get interest threshold for exploration."""
        # Lower interest when sufficiently curious
        return max(0.1, 1.0 - self.interest_level)
        
    def update(self, time_step: float):
        """Update curiosity state."""
        # Decay novelty
        self.novelty_score *= 0.9
        
        # Decay information gain
        self.information_gain *= 0.95
        
        # Decay interest
        self.interest_level *= 0.98
class NoveltyModulator(NeuromodulatorBase):
    """
    Novelty neuromodulator.
    
    Novelty modulates:
    - Attention to new stimuli
    - Learning rate for novel inputs
    - Memory encoding
    - Pattern detection
    
    This implementation includes:
    - Fast/slow novelty processing
    - Memory updating signals
    - Pattern change detection
    - Learning rate modulation
    """
    
    def __init__(self, brain: pynlm.Brain):
        super().__init__("novelty", brain)
        self.target_neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Internal
        ]
        self.fast_novelty = 0.0
        self.slow_novelty = 0.0
        self.pattern_change = 0.0
        self.memory_update_signal = 0.0
        
    def setFastNovelty(self, novelty: float):
        """Set fast novelty level."""
        self.fast_novelty = max(0.0, min(1.0, novelty))
        
    def setSlowNovelty(self, novelty: float):
        """Set slow novelty level."""
        self.slow_novelty = max(0.0, min(1.0, novelty))
        
    def setPatternChange(self, change: float):
        """Set pattern change level."""
        self.pattern_change = max(0.0, min(1.0, change))
        
    def getLearningRateBoost(self) -> float:
        """Get learning rate boost for novel patterns."""
        # Novelty increases learning rate
        learning_boost = (self.fast_novelty + self.slow_novelty) / 2.0
        learning_boost *= (1.0 + self.pattern_change)
        return min(2.0, learning_boost)
        
    def getMemoryUpdateStrength(self) -> float:
        """Get memory update strength."""
        # Strong novelty triggers memory updating
        memory_strength = self.fast_novelty * (1.0 + self.pattern_change)
        return min(1.0, memory_strength)
        
    def update(self, time_step: float):
        """Update novelty state."""
        # Decay fast novelty
        self.fast_novelty *= 0.8
        
        # Decay slow novelty
        self.slow_novelty *= 0.95
        
        # Decay pattern change
        self.pattern_change *= 0.9
class PredictionErrorModulator(NeuromodulatorBase):
    """
    Prediction Error neuromodulator.
    
    Prediction error modulates:
    - Learning precision
    - Weight adjustment
    - Expectation updating
    - Error signaling
    
    This implementation includes:
    - Temporal difference learning
    - Reward prediction error
    - Surprise signals
    - Uncertainty estimation
    """
    
    def __init__(self, brain: pynlm.Brain):
        super().__init__("prediction_error", brain)
        self.target_neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Internal
        ]
        self.reward_prediction_error = 0.0
        self.surprise_signal = 0.0
        self.uncertainty = 0.0
        self.temporal_difference = 0.0
        
    def setRewardPredictionError(self, error: float):
        """Set reward prediction error."""
        self.reward_prediction_error = error
        
    def setSurprise(self, surprise: float):
        """Set surprise signal."""
        self.surprise_signal = surprise
        
    def setUncertainty(self, uncertainty: float):
        """Set uncertainty level."""
        self.uncertainty = uncertainty
        
    def getLearningPrecision(self) -> float:
        """Get learning precision (inverse variance)."""
        # Prediction error inversely related to uncertainty
        precision = 1.0 / (1.0 + self.uncertainty)
        return precision
        
    def getWeightAdjustment(self) -> float:
        """Get weight adjustment factor."""
        # Prediction error drives weight updates
        adjustment = self.reward_prediction_error * self.surprise_signal
        return np.clip(adjustment, -0.5, 0.5)
        
    def update(self, time_step: float):
        """Update prediction error state."""
        # Decay reward prediction error
        self.reward_prediction_error *= 0.9
        
        # Decay surprise
        self.surprise_signal *= 0.95
        
        # Decay uncertainty
        self.uncertainty *= 0.98
class AdvancedNeuromodulation:
    """
    Advanced neuromodulation system.
    
    This system integrates multiple neuromodulators to coordinate:
    - Learning and plasticity
    - Behavior and motivation
    - Attention and focus
    - Memory and consolidation
    
    Key features:
    - Multi-modulator coordination
    - Hierarchical control
    - Feedback loops
    - Adaptive responses
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        
        # Initialize neuromodulators
        self.dopamine = DopamineModulator(brain)
        self.curiosity = CuriosityModulator(brain)
        self.novelty = NoveltyModulator(brain)
        self.prediction_error = PredictionErrorModulator(brain)
        
        # Integration weights
        self.neuromod_integration_weights = {
            'dopamine': {'curiosity': 0.3, 'novelty': 0.2, 'prediction_error': 0.5},
            'curiosity': {'dopamine': 0.4, 'novelty': 0.3, 'prediction_error': 0.3},
            'novelty': {'dopamine': 0.2, 'curiosity': 0.4, 'prediction_error': 0.4},
            'prediction_error': {'dopamine': 0.5, 'curiosity': 0.2, 'novelty': 0.3}
        }
        
        # State variables
        self.time_step = 0.0
        self.episode_start_time = time.time()
        
    def processEnvironmentalSignals(self, state: Dict[str, Any]):
        """
        Process environmental signals and update neuromodulators.
        
        Args:
            state: Current environmental state
        """
        # Extract signals from environment
        reward = state.get('reward', 0.0)
        novelty = state.get('novelty', 0.0)
        surprise = state.get('surprise', 0.0)
        uncertainty = state.get('uncertainty', 0.1)
        
        # Update neuromodulators based on signals
        self.dopamine.setPredictionError(reward)
        self.curiosity.setNovelty(novelty)
        self.curiosity.setInformationGain(state.get('information_gain', 0.0))
        self.novelty.setFastNovelty(novelty)
        self.novelty.setSlowNovelty(state.get('slow_novelty', 0.0))
        self.novelty.setPatternChange(state.get('pattern_change', 0.0))
        self.prediction_error.setRewardPredictionError(reward)
        self.prediction_error.setSurprise(surprise)
        self.prediction_error.setUncertainty(uncertainty)
        
    def applyNeuromodulation(self):
        """Apply neuromodulation to brain systems."""
        # Calculate integrated neuromodulation signals
        dopamine_effect = self.dopamine.getLevel()
        curiosity_effect = self.curiosity.getLevel()
        novelty_effect = self.novelty.getLevel()
        prediction_error_effect = self.prediction_error.getLevel()
        
        # Apply effects to different brain systems
        self.applyPlasticityModulation(
            dopamine_effect,
            curiosity_effect,
            novelty_effect,
            prediction_error_effect
        )
        
        self.applyAttentionModulation(
            dopamine_effect,
            curiosity_effect,
            novelty_effect,
            prediction_error_effect
        )
        
        self.applyLearningModulation(
            dopamine_effect,
            curiosity_effect,
            novelty_effect,
            prediction_error_effect
        )
        
    def applyPlasticityModulation(self, dopamine: float, curiosity: float,
                                novelty: float, pred_error: float):
        """Apply neuromodulation to plasticity systems."""
        # Calculate combined plasticity factor
        plasticity_factor = (
            dopamine * self.neuromod_integration_weights['dopamine']['novelty'] +
            curiosity * self.neuromod_integration_weights['curiosity']['prediction_error'] +
            novelty * self.neuromod_integration_weights['novelty']['dopamine'] +
            pred_error * self.neuromod_integration_weights['prediction_error']['curiosity']
        )
        
        # Normalize to reasonable range
        plasticity_factor = np.tanh(plasticity_factor) * 1.5
        
        # Apply to STDP
        stdp_factor = self.dopamine.getPlasticityFactor()
        
        # Apply to Hebbian
        hebbian_factor = novelty * 0.5 + curiosity * 0.3
        
        # Apply to structural plasticity
        structural_factor = dopamine * pred_error * 0.5
        
        # Log modulation effects
        print(f"Plasticity modulation: STDP={stdp_factor:.3f}, "
              f"Hebbian={hebbian_factor:.3f}, Structural={structural_factor:.3f}")
        
    def applyAttentionModulation(self, dopamine: float, curiosity: float,
                               novelty: float, pred_error: float):
        """Apply neuromodulation to attention systems."""
        # Calculate attention modulation
        attention_boost = dopamine * 0.4 + curiosity * 0.3 + novelty * 0.2
        attention_focus = pred_error * 0.5 + novelty * 0.3
        
        print(f"Attention modulation: Boost={attention_boost:.3f}, "
              f"Focus={attention_focus:.3f}")
        
    def applyLearningModulation(self, dopamine: float, curiosity: float,
                               novelty: float, pred_error: float):
        """Apply neuromodulation to learning systems."""
        # Calculate learning rate modulation
        learning_rate = (
            self.dopamine.getLearningPrecision() * 0.3 +
            self.novelty.getLearningRateBoost() * 0.4 +
            curiosity * 0.2 +
            pred_error * 0.1
        )
        
        print(f"Learning modulation: Rate={learning_rate:.3f}")
        
    def update(self, time_step: float):
        """
        Update neuromodulation system.
        
        Args:
            time_step: Time step duration
        """
        self.time_step = time_step
        
        # Update individual neuromodulators
        self.dopamine.update(time_step)
        self.curiosity.update(time_step)
        self.novelty.update(time_step)
        self.prediction_error.update(time_step)
        
        # Apply neuromodulation
        self.applyNeuromodulation()
        
    def getNeuromodulationReport(self) -> Dict[str, Any]:
        """Get comprehensive neuromodulation report."""
        return {
            'time_step': self.time_step,
            'dopamine_level': self.dopamine.getLevel(),
            'curiosity_level': self.curiosity.getLevel(),
            'novelty_level': self.novelty.getLevel(),
            'prediction_error_level': self.prediction_error.getLevel(),
            'dopamine_effects': {
                'plasticity_factor': self.dopamine.getPlasticityFactor(),
                'decision_bias': self.dopamine.getDecisionBias()
            },
            'curiosity_effects': {
                'exploration_bonus': self.curiosity.getExplorationBonus(),
                'interest_threshold': self.curiosity.getInterestThreshold()
            },
            'novelty_effects': {
                'learning_rate_boost': self.novelty.getLearningRateBoost(),
                'memory_update_strength': self.novelty.getMemoryUpdateStrength()
            },
            'prediction_error_effects': {
                'learning_precision': self.prediction_error.getLearningPrecision(),
                'weight_adjustment': self.prediction_error.getWeightAdjustment()
            }
        }
    
    def runAdvancedSimulation(self, duration: float, time_step: float = 0.01):
        """
        Run advanced neuromodulation simulation.
        
        Args:
            duration: Simulation duration
            time_step: Simulation time step
        """
        print(f"Running advanced neuromodulation simulation for {duration}s...")
        print(f"Time step: {time_step}s")
        
        steps = int(duration / time_step)
        
        for step in range(steps):
            # Generate simulated environment state
            state = self.generateEnvironmentState()
            
            # Process environmental signals
            self.processEnvironmentalSignals(state)
            
            # Update neuromodulation
            self.update(time_step)
            
            # Log progress periodically
            if step % 100 == 0:
                report = self.getNeuromodulationReport()
                print(f"Step {step}: Dopamine={report['dopamine_level']:.3f}, "
                      f"Curiosity={report['curiosity_level']:.3f}, "
                      f"Novelty={report['novelty_level']:.3f}, "
                      f"PE={report['prediction_error_level']:.3f}")
        
        print("\n=== Advanced Neuromodulation Simulation Complete ===")
        
    def generateEnvironmentState(self) -> Dict[str, Any]:
        """Generate simulated environment state."""
        import random
        
        return {
            'reward': random.uniform(-1.0, 2.0),
            'novelty': random.random(),
            'surprise': random.random() * 0.5,
            'uncertainty': random.random() * 0.3,
            'information_gain': random.random() * 0.2,
            'slow_novelty': random.random() * 0.4,
            'pattern_change': random.random() * 0.6
        }
def runAdvancedNeuromodulationExample():
    """Run the advanced neuromodulation example."""
    print("=== Advanced Neuromodulation Example ===\n")
    
    # Create brain
    config = pynlm.createDemoConfig(1000, 0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize neuromodulation system
    neuromod = AdvancedNeuromodulation(brain)
    
    print("Neuromodulation system initialized with:")
    print("- Dopamine modulator (reward prediction)")
    print("- Curiosity modulator (exploration)")
    print("- Novelty modulator (attention)")
    print("- Prediction Error modulator (learning)")
    print()
    
    # Run advanced simulation
    neuromod.runAdvancedSimulation(duration=10.0, time_step=0.01)
    
    # Display final report
    report = neuromod.getNeuromodulationReport()
    print("\n=== Final Neuromodulation Report ===")
    
    for modulator_name, level in [
        ('Dopamine', report['dopamine_level']),
        ('Curiosity', report['curiosity_level']),
        ('Novelty', report['novelty_level']),
        ('Prediction Error', report['prediction_error_level'])
    ]:
        print(f"{modulator_name}: {level:.3f}")
    
    print("\nModulator Effects:")
    for modulator_name, effects in report.items():
        if isinstance(effects, dict) and modulator_name != 'time_step':
            print(f"{modulator_name}:")
            for effect_name, value in effects.items():
                if isinstance(value, float):
                    print(f"  {effect_name}: {value:.3f}")
                elif isinstance(value, dict):
                    print(f"  {effect_name}:")
                    for sub_effect, sub_value in value.items():
                        print(f"    {sub_effect}: {sub_value:.3f}")

if __name__ == "__main__":
    runAdvancedNeuromodulationExample()