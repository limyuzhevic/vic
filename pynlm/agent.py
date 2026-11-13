"""
Pythonic agent brain class for NLM.

This module provides enhanced Pythonic APIs for agent brain simulation,
featuring simplified sensory-motor integration, comprehensive neuromodulation,
and improved error handling for autonomous agents.
"""

import sys
from typing import List, Optional, Tuple, Any

# Add parent directory to path for imports
if len(sys.path) == 1:
    sys.path.append("..")

from nlm import AgentBrain, Brain, SimpleWorld, SensoryPercept, Action,
    MotorCommand, DevelopmentalStage, Neuromodulator, Reward

class AgentError(Exception):
    """Exception raised for agent errors."""
    pass

class AgentWrapper:
    """
    Enhanced Pythonic wrapper for NLM agent brain simulation.
    
    Provides simplified sensory-motor integration, comprehensive neuromodulation,
    and improved error handling for autonomous agents interacting with environments.
    """
    
    def __init__(self, brain: Optional[Brain] = None, agent: Optional[AgentBrain] = None):
        """
        Initialize agent wrapper.
        
        Args:
            brain: Configuration for creating agent brain. If agent is provided, ignored.
            agent: Existing NLM AgentBrain object to wrap
        """
        if agent is None:
            if brain is None:
                brain = Brain(Config())
            self.agent = AgentBrain(brain)
        else:
            self.agent = agent
    
    def initialize(self, world: SimpleWorld) -> None:
        """
        Initialize agent with world.
        
        Args:
            world: World environment for agent
        
        Raises:
            AgentError: If initialization fails
        """
        try:
            self.agent.initialize(world)
        except Exception as e:
            raise AgentError(f"Agent initialization failed: {e}")
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        return self.agent.getSensoryInputSize()
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        return self.agent.getMotorOutputSize()
    
    def process_sensory_input(self, percept: SensoryPercept) -> None:
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process
        """
        self.agent.processSensoryInput(percept)
    
    def decode_motor_command(self) -> MotorCommand:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            Motor command
        
        Raises:
            AgentError: If decoding fails
        """
        try:
            return self.agent.decodeMotorCommand()
        except Exception as e:
            raise AgentError(f"Motor command decoding failed: {e}")
    
    # Neuromodulation
    def apply_reward_modulation(self, reward: float, 
                               predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        self.agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float) -> None:
        """
        Update development system.
        
        Args:
            timestep: Time step
        """
        self.agent.updateDevelopment(timestep)
    
    # State
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self.agent.getDevelopmentalStage()
    
    # Neuromodulation levels
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return self.agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self.agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self.agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self.agent.getPredictionError()
    
    # Cognitive features
    def get_thought_trace(self) -> str:
        """
        Get thought trace.
        
        Returns:
            String describing last perceived scene, planner suggestion,
            decoded action, and outcome reward
        """
        return self.agent.getThoughtTrace()
    
    def get_learned_preference(self) -> float:
        """Get learned preference trait."""
        return self.agent.getLearnedPreference()
    
    # Configuration flags
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation."""
        self.agent.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity."""
        self.agent.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool) -> None:
        """Enable or disable development."""
        self.agent.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity."""
        self.agent.enableCuriosity(enable)
    
    # Status checks
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self.agent.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self.agent.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self.agent.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self.agent.isCuriosityEnabled()
    
    # Diagnostic counters
    def get_reflex_bonus_count(self) -> int:
        """Get vision-gated reflex bonus count."""
        return self.agent.getReflexBonusCount()
    
    def get_reflex_suppress_count(self) -> int:
        """Get vision-gated hazard suppression count."""
        return self.agent.getReflexSuppressCount()
    
    def get_wta_pulse_count(self) -> int:
        """Get winner-take-all pulse count."""
        return self.agent.getWtaPulseCount()
    
    def get_planner_override_count(self) -> int:
        """Get planner override count."""
        return self.agent.getPlannerOverrideCount()
    
    def get_planner_agreement_count(self) -> int:
        """Get planner agreement count."""
        return self.agent.getPlannerAgreementCount()
    
    def get_planner_decision_count(self) -> int:
        """Get planner decision count."""
        return self.agent.getPlannerDecisionCount()
    
    def get_concept_bias_count(self) -> int:
        """Get concept bias count."""
        return self.agent.getConceptBiasCount()
    
    def get_self_bias_count(self) -> int:
        """Get self-bias count."""
        return self.agent.getSelfBiasCount()
    
    def get_social_bias_count(self) -> int:
        """Get social bias count."""
        return self.agent.getSocialBiasCount()
    
    def get_concept_count(self) -> int:
        """Get concept count."""
        return self.agent.getConceptCount()
    
    def get_self_model_confidence(self) -> float:
        """Get self-model confidence."""
        return self.agent.getSelfModelConfidence()
    
    def get_social_observation_count(self) -> int:
        """Get social observation count."""
        return self.agent.getSocialObservationCount()
    
    # Text encoding (Phase 6 feature)
    def encode_text(self, text: str) -> List[int]:
        """
        Encode text into neural pattern.
        
        Args:
            text: Text to encode
        
        Returns:
            List of neuron indices representing text pattern
        """
        return AgentBrain.encodeText(text)
    
    def get_internal_firing_rates(self) -> List[float]:
        """Get internal firing rates for text recall."""
        return self.agent.getInternalFiringRates()
    
    # Social learning
    def observe_other_agent(self, observed: MotorCommand,
                           before_vision: List[float],
                           after_vision: List[float]) -> None:
        """
        Observe other agent's actions and vision.
        
        Args:
            observed: Motor command observed
            before_vision: Vision before action
            after_vision: Vision after action
        """
        self.agent.observeOtherAgent(observed, before_vision, after_vision)
    
    # Reset
    def reset(self) -> None:
        """Reset agent for new episode."""
        self.agent.reset()
    
    # Brain access
    def get_brain(self) -> Optional[Brain]:
        """
        Get underlying brain.
        
        Returns:
            Underlying Brain object
        """
        return self.agent.getBrain()
    
    # Utility methods
    def get_sensory_percept_size(self) -> int:
        """Get sensory percept size."""
        percept = self.get_sensory_percept()
        vision = percept.getVision()
        return len(vision) * len(vision[0]) if vision else 0
    
    def __repr__(self) -> str:
        """String representation."""
        if hasattr(self.agent, 'getBrain'):
            brain = self.agent.getBrain()
            if brain:
                neurons = brain.getTotalNeuronCount()
                return f"<AgentWrapper: {neurons} neurons>"
        return "<AgentWrapper: agent>"
    
    def __str__(self) -> str:
        """String representation."""
        stage = self.get_developmental_stage()
        curiosity = self.get_curiosity_level()
        novelty = self.get_novelty_level()
        
        return (f"Agent: Stage={stage}, Curiosity={curiosity:.2f}, "
                f"Novelty={novelty:.2f}, "
                f"Reward={self.is_reward_modulation_enabled()}")
    
    def get_status_summary(self) -> str:
        """Get comprehensive status summary."""
        stage = self.get_developmental_stage()
        curiosity = self.get_curiosity_level()
        novelty = self.get_novelty_level()
        pred_error = self.get_prediction_error()
        
        return (f"Agent: Stage={stage}, "
                f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}, "
                f"PredError={pred_error:.3f}, "
                f"Reward={self.is_reward_modulation_enabled()}, "
                f"Develop={self.is_development_enabled()}")