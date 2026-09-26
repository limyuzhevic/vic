# NLM Phase 6 API Documentation

This document provides comprehensive API documentation for the Phase 6 NLM enhanced Python bindings. It documents all new features, classes, and methods introduced in Phase 6.

## Table of Contents

1. [Overview of Phase 6 Enhanced API](#overview-of-phase-6-enhanced-api)
2. [ConceptFormation API](#conceptformation-api)
3. [NeuralPlanner API](#neuralplanner-api)
4. [PredictionSystem API](#predictionsystem-api)
5. [Memory Systems API](#memory-systems-api)
6. [Neuromodulation Systems API](#neuromodulation-systems-api)
7. [Development System API](#development-system-api)
8. [Attention System API](#attention-system-api)
9. [Python Integration Examples](#python-integration-examples)
10. [API Design Principles](#api-design-principles)
11. [Migration Guide](#migration-guide)
12. [Performance Considerations](#performance-considerations)
13. [Error Handling](#error-handling)
14. [Type System](#type-system)

## Overview of Phase 6 Enhanced API

The Phase 6 enhanced Python API provides direct access to all Phase 6 integrated systems through Pybind11 bindings. This enables developers to leverage the full power of the integrated artificial brain without requiring deep C++ expertise.

### Key Enhancements Over Original API

1. **Direct System Access**: Access to all Phase 6 systems (ConceptFormation, NeuralPlanner, etc.)
2. **Enhanced Functionality**: New methods for concept management, planning, prediction
3. **Improved Integration**: Better integration with existing Python APIs
4. **Comprehensive Documentation**: Complete documentation with examples
5. **Performance Monitoring**: Tools for monitoring system performance
6. **Experiment Management**: Framework for running and analyzing experiments

### API Structure

```python
# Core NLM modules
from nlm import Brain, AgentBrain, SimpleWorld, Experiment, Phase6IntegrationExperiment
\n# Phase 6 enhanced modules
from nlm.concept_formation import ConceptFormation
from nlm.planning import NeuralPlanner
from nlm.prediction import PredictionSystem
from nlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory
from nlm.neuromodulation import Dopamine, Curiosity, Novelty, PredictionError
from nlm.development import DevelopmentSystem
from nlm.attention import AttentionalSelection
```

## ConceptFormation API

The ConceptFormation class provides access to concept learning, management, and analysis.

### Class: ConceptFormation

```python
class ConceptFormation:
    """
    Concept formation system for abstract pattern learning and representation.
    
    Phase 6 Enhancement: Full integration with neural processing and prediction systems.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize concept formation system.
        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def presentExperience(self, pattern: List[float], features: List[float], 
                         reward: float, timestamp: int) -> int:
        """
        Present an experience to the concept formation system.
        
        Args:
            pattern: Neural activity pattern from working memory
            features: Extracted features from the pattern
            reward: Reward signal for the experience
            timestamp: Current simulation step
            
        Returns:
            Concept ID of newly formed/updated concept, or 0 if no concept formed
            
        Raises:
            ValueError: If pattern or features are invalid
        """
        pass
    
    def getConcepts(self) -> List['Concept']:
        """
        Get all current concepts.
        
        Returns:
            List of all concepts in the system
        """
        pass
    
    def getConcept(self, concept_id: int) -> 'Concept':
        """
        Get a specific concept by ID.
        
        Args:
            concept_id: ID of the concept to retrieve
            
        Returns:
            Concept object, or None if concept doesn't exist
        """
        pass
    
    def getConceptPrototype(self, concept_id: int) -> List[float]:
        """
        Get concept prototype (abstract representation).
        
        Args:
            concept_id: ID of the concept
            
        Returns:
            Concept prototype as list of floats, or empty list if concept not found
        """
        pass
    
    def getConceptInstances(self, concept_id: int) -> List[List[float]]:
        """
        Get all concept instances (experiences that formed this concept).
        
        Args:
            concept_id: ID of the concept
            
        Returns:
            List of concept instances, or empty list if concept not found
        """
        pass
    
    def getConceptStability(self, concept_id: int) -> float:
        """
        Get concept stability (how stable/well-established the concept is).
        
        Args:
            concept_id: ID of the concept
            
        Returns:
            Concept stability value between 0.0 and 1.0
        """
        pass
    
    def getConceptCount(self) -> int:
        """
        Get total number of concepts.
        
        Returns:
            Number of concepts in the system
        """
        pass
    
    def getMatchingConcept(self, pattern: List[float], 
                          similarity_threshold: float = 0.7) -> int:
        """
        Find concept that matches the given pattern.
        
        Args:
            pattern: Pattern to match against
            similarity_threshold: Minimum similarity for match (0.0-1.0)
            
        Returns:
            Matching concept ID, or 0 if no match found
        """
        pass
    
    def isNovel(self, pattern: List[float], 
                similarity_threshold: float = 0.7) -> bool:
        """
        Check if pattern represents a novel experience.
        
        Args:
            pattern: Pattern to check for novelty
            similarity_threshold: Similarity threshold for novelty detection
            
        Returns:
            True if pattern is novel, False if it matches existing concepts
        """
        pass
    
    def findConceptForPattern(self, pattern: List[float]) -> int:
        """
        Find the best concept for a given pattern.
        
        Args:
            pattern: Pattern to find concept for
            
        Returns:
            Best matching concept ID, or 0 if no suitable concept found
        """
        pass
    
    def mergeConcepts(self, concept_a: int, concept_b: int) -> bool:
        """
        Merge two concepts into one.
        
        Args:
            concept_a: ID of first concept
            concept_b: ID of second concept
            
        Returns:
            True if concepts were successfully merged, False otherwise
        """
        pass
    
    def updateConcept(self, concept_id: int, new_pattern: List[float], 
                     features: List[float], reward: float) -> bool:
        """
        Update a concept with new information.
        
        Args:
            concept_id: ID of concept to update
            new_pattern: New pattern to incorporate
            features: New features to incorporate
            reward: Reward signal for update
            
        Returns:
            True if concept was successfully updated, False otherwise
        """
        pass
    
    def computeSimilarity(self, a: List[float], b: List[float]) -> float:
        """
        Compute similarity between two patterns.
        
        Args:
            a: First pattern
            b: Second pattern
            
        Returns:
            Similarity value between 0.0 and 1.0
        """
        pass
    
    def clear(self) -> None:
        """
        Clear all concepts and reset the system.
        """
        pass
    
    def getGeneralizationAbility(self, concept_id: int) -> float:
        """
        Get concept generalization ability (how well it applies to new patterns).
        
        Args:
            concept_id: ID of the concept
            
        Returns:
            Generalization ability value between 0.0 and 1.0
        """
        pass
    
    def analyzeConceptFormation(self) -> Dict:
        """
        Analyze concept formation statistics and performance.
        
        Returns:
            Dictionary containing concept formation analysis metrics
        """
        pass

# Supporting Classes

```python
class Concept:
    """
    Represents a learned concept with prototype, instances, and stability.
    """
    
    def __init__(self, concept_id: int):
        self.concept_id = concept_id
        self.prototype = []  # Concept prototype
        self.instances = []  # Concept instances
        self.stability = 0.0  # Concept stability
        self.total_observations = 0  # Total experiences for this concept
        self.creation_timestamp = 0  # When concept was created
    
    @property
    def id(self) -> int:
        return self.concept_id
    
    @property
    def is_stable(self) -> bool:
        return self.stability > 0.5
```

## NeuralPlanner API

The NeuralPlanner class generates action sequences based on concepts, predictions, and goals.

### Class: NeuralPlanner

```python
class NeuralPlanner:
    """
    Neural planner for generating action sequences based on goals and predictions.
    
    Phase 6 Enhancement: Full integration with concept formation and prediction systems.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize neural planner.
        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def setGoal(self, goal_pattern: List[float]) -> bool:
        """
        Set current goal for planning.
        
        Args:
            goal_pattern: Goal pattern to plan towards
            
        Returns:
            True if goal was set successfully, False otherwise
        """
        pass
    
    def planActions(self, input_signals: 'InternalSignals', 
                   goal_pattern: List[float], prediction_system: 'PredictionSystem',
                   horizon: int = 5) -> List['ActionSequence']:
        """
        Generate action sequences to achieve the goal.
        
        Args:
            input_signals: Current internal state signals
            goal_pattern: Goal pattern to achieve
            prediction_system: Prediction system for outcome evaluation
            horizon: Planning horizon (number of actions ahead)
            
        Returns:
            List of action sequences, sorted by expected utility
        """
        pass
    
    def evaluateActionSequence(self, actions: List[Action],
                              input_signals: 'InternalSignals',
                              goal_pattern: List[float]) -> float:
        """
        Evaluate an action sequence's expected utility.
        
        Args:
            actions: Action sequence to evaluate
            input_signals: Current internal state signals
            goal_pattern: Goal pattern for evaluation
            
        Returns:
            Expected utility of the action sequence
        """
        pass
    
    def getActionSequences(self) -> List['ActionSequence']:
        """
        Get all available action sequences.
        
        Returns:
            List of all action sequences
        """
        pass
    
    def addSkill(self, concept_id: int) -> bool:
        """
        Add a skill based on a concept.
        
        Args:
            concept_id: ID of concept to use as skill template
            
        Returns:
            True if skill was added successfully, False otherwise
        """
        pass
    
    def getSkills(self) -> List['Skill']:
        """
        Get all skills.
        
        Returns:
            List of all skills
        """
        pass
    
    def removeSkill(self, skill_id: int) -> bool:
        """
        Remove a skill.
        
        Args:
            skill_id: ID of skill to remove
            
        Returns:
            True if skill was removed successfully, False otherwise
        """
        pass
    
    def getPlanningDepth(self) -> int:
        """
        Get current planning depth.
        
        Returns:
            Planning depth
        """
        pass
    
    def setPlanningDepth(self, depth: int) -> None:
        """
        Set planning depth.
        
        Args:
            depth: Planning depth
        """
        pass
    
    def enableUncertaintyHandling(self, enabled: bool) -> None:
        """
        Enable/disable uncertainty handling.
        
        Args:
            enabled: Whether to enable uncertainty handling
        """
        pass
    
    def enableRiskAwarePlanning(self, enabled: bool) -> None:
        """
        Enable/disable risk-aware planning.
        
        Args:
            enabled: Whether to enable risk-aware planning
        """
        pass
    
    def setRiskAversion(self, risk_aversion: float) -> None:
        """
        Set risk aversion factor.
        
        Args:
            risk_aversion: Risk aversion factor (0.0-1.0)
        """
        pass
    
    def setRiskTolerance(self, tolerance: float) -> None:
        """
        Set risk tolerance.
        
        Args:
            tolerance: Risk tolerance (0.0-1.0)
        """
        pass
    
    def generateAllActionSequences(self) -> List['ActionSequence']:
        """
        Generate all possible action sequences.
        
        Returns:
            List of all possible action sequences
        """
        pass
    
    def storeRiskAwarePlans(self, plans: List['ActionSequence'],
                          risk_assessments: List['RiskAssessment']) -> None:
        """
        Store risk-aware action plans.
        
        Args:
            plans: Action sequences
            risk_assessments: Corresponding risk assessments
        """
        pass
    
    def getAllPlans(self) -> List['ActionSequence']:
        """
        Get all stored action plans.
        
        Returns:
            List of all stored action plans
        """
        pass
    
    def getAllRiskAssessments(self) -> List['RiskAssessment']:
        """
        Get all risk assessments.
        
        Returns:
            List of all risk assessments
        """
        pass

# Supporting Classes

```python
class ActionSequence:
    """
    A sequence of actions to achieve a goal.
    """
    
    def __init__(self):
        self.actions = []  # List of actions
        self.utility = 0.0  # Expected utility
        self.confidence = 0.0  # Confidence in this plan
        self.uncertainty = 0.0  # Uncertainty in plan execution
        self.risk_score = 0.0  # Risk score
        self.timestamp = 0  # When this plan was created
    
    @property
    def length(self) -> int:
        return len(self.actions)
    
    @property
    def is_feasible(self) -> bool:
        return len(self.actions) > 0

class Skill:
    """
    A skill based on a concept that can be executed.
    """
    
    def __init__(self, skill_id: int, concept_id: int):
        self.skill_id = skill_id
        self.concept_id = concept_id
        self.mastery_level = 0.0  # Skill mastery level
        self.acquisition_timestamp = 0  # When skill was acquired
    
    @property
    def is_mastered(self) -> bool:
        return self.mastery_level > 0.8

class RiskAssessment:
    """
    Risk assessment for an action sequence.
    """
    
    def __init__(self):
        self.risk_score = 0.0
        self.risk_factors = []
        self.potential_reward = 0.0
        self.risk_adjusted_utility = 0.0
```

## PredictionSystem API

The PredictionSystem class anticipates future states based on current input and learned patterns.

### Class: PredictionSystem

```python
class PredictionSystem:
    """
    Prediction system for anticipating future states.
    
    Phase 6 Enhancement: Full integration with concept formation and planning.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize prediction system.
        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def predictOutcome(self, actions: List[Action],
                      state: 'InternalSignals') -> 'InternalSignals':
        """
        Predict outcome of executing actions in given state.
        
        Args:
            actions: Actions to execute
            state: Current state
            
        Returns:
            Predicted outcome state
        """
        pass
    
    def updatePredictions(self, input_signals: 'InternalSignals',
                         previous_predictions: 'InternalSignals') -> bool:
        """
        Update predictions based on new input.
        
        Args:
            input_signals: New input signals
            previous_predictions: Previous predictions to update
            
        Returns:
            True if predictions were updated successfully, False otherwise
        """
        pass
    
    def getPredictionError(self) -> float:
        """
        Get prediction error.
        
        Returns:
            Prediction error value
        """
        pass
    
    def getConfidence(self) -> float:
        """
        Get prediction confidence.
        
        Returns:
            Prediction confidence value (0.0-1.0)
        """
        pass
    
    def getPrediction(self, timestep_ahead: int = 0) -> 'InternalSignals':
        """
        Get prediction for specific time ahead.
        
        Args:
            timestep_ahead: Number of timesteps ahead to predict
            
        Returns:
            Predicted state
        """
        pass
    
    def updateParameters(self, learning_rate: float, confidence: float,
                        novelty_threshold: float, concept_influence: float) -> None:
        """
        Update prediction system parameters.
        
        Args:
            learning_rate: Learning rate
            confidence: Confidence threshold
            novelty_threshold: Novelty threshold
            concept_influence: Concept influence factor
        """
        pass
    
    def addConceptTarget(self, concept_id: int) -> bool:
        """
        Add concept as prediction target.
        
        Args:
            concept_id: ID of concept to use as prediction target
            
        Returns:
            True if target was added successfully, False otherwise
        """
        pass
    
    def removeConceptTarget(self, concept_id: int) -> bool:
        """
        Remove concept from prediction targets.
        
        Args:
            concept_id: ID of concept to remove from targets
            
        Returns:
            True if target was removed successfully, False otherwise
        """
        pass
    
    def getConceptTargets(self) -> List[int]:
        """
        Get all concept targets.
        
        Returns:
            List of concept target IDs
        """
        pass
    
    def getConceptUpdateSignal(self) -> float:
        """
        Get concept update signal.
        
        Returns:
            Concept update signal value
        """
        pass
    
    def predictNextState(self, input_signals: 'InternalSignals') -> 'InternalSignals':
        """
        Predict next state.
        
        Args:
            input_signals: Current input signals
            
        Returns:
            Predicted next state
        """
        pass
    
    def getAllPredictions(self) -> List['InternalSignals']:
        """
        Get all predictions.
        
        Returns:
            List of all predictions
        """
        pass
    
    def updateFromConcepts(self, concept_formation: 'ConceptFormation') -> None:
        """
        Update predictions from concepts.
        
        Args:
            concept_formation: Concept formation system
        """
        pass
```

## Memory Systems API

The memory systems provide storage for experiences, active traces, and associations.

### Classes: WorkingMemory, EpisodicMemory, AssociativeMemory

```python
class WorkingMemory:
    """
    Working memory for maintaining active traces of neural activity.
    
    Phase 6 Enhancement: Fully integrated with all cognitive systems.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize working memory.
n
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def storeToNeuron(self, neuron_id: int, activation: float) -> bool:
        """
        Store activation for a neuron in working memory.
        
        Args:
            neuron_id: ID of neuron
            activation: Activation level (0.0-1.0)
            
        Returns:
            True if trace was stored successfully, False otherwise
        """
        pass
    
    def getActiveTraces(self) -> List['MemoryTrace']:
        """
        Get all active memory traces.
        
        Returns:
            List of active memory traces
        """
        pass
    
    def getTrace(self, neuron_id: int) -> 'MemoryTrace':
        """
        Get memory trace for specific neuron.
        
        Args:
            neuron_id: ID of neuron
            
        Returns:
            Memory trace, or None if not found
        """
        pass
    
    def update(self, timestep_duration: float) -> None:
        """
        Update working memory state.
n        
        Args:
            timestep_duration: Duration of timestep
        """
        pass
    
    def getMemoryNeurons(self) -> List[int]:
        """
        Get IDs of neurons with active traces.
        
        Returns:
            List of neuron IDs
        """
        pass
    
    def getMemoryNeurons(self) -> List[int]:
        """
        Get IDs of neurons with active traces.
        
        Returns:
            List of neuron IDs
        """
        pass
    
    def setCapacity(self, capacity: int) -> None:
        """
        Set working memory capacity.
n        
        Args:
            capacity: Maximum number of traces
        """
        pass
    
    def getCapacity(self) -> int:
        """
        Get working memory capacity.
        
        Returns:
            Working memory capacity
        """
        pass
    
    def clearAllTraces(self) -> None:
        """
        Clear all memory traces.
        """
        pass
    
    def cleanOldTraces(self, decay_threshold: float) -> None:
        """
        Clean old traces based on decay threshold.
n        
        Args:
            decay_threshold: Trace strength threshold for cleanup
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
        
        Returns:
            Memory usage in bytes
        """
        pass
```

```python
class EpisodicMemory:
    """
    Episodic memory for storing experienced episodes.
n    
    Phase 6 Enhancement: Fully integrated with replay and consolidation.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize episodic memory.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def storeEpisode(self, episode: 'EpisodicMemoryItem') -> bool:
        """
        Store an episode.
n        
        Args:
            episode: Episode to store
            
        Returns:
            True if episode was stored successfully, False otherwise
        """
        pass
    
    def getEpisodeCount(self) -> size_t:
        """
        Get number of stored episodes.
n        
        Returns:
            Number of episodes
        """
        pass
    
    def getEpisodes(self) -> List['EpisodicMemoryItem']:
        """
        Get all stored episodes.
n        
        Returns:
            List of all episodes
        """
        pass
    
    def getEpisodesForReplay(self, count: int) -> List['EpisodicMemoryItem']:
        """
        Get episodes for replay.
n        
        Args:
            count: Number of episodes to retrieve
            
        Returns:
            List of episodes for replay
        """
        pass
    
    def getEpisode(self, episode_index: size_t) -> 'EpisodicMemoryItem':
        """
        Get specific episode by index.
n        
        Args:
            episode_index: Index of episode to retrieve
            
        Returns:
            Episode, or None if not found
        """
        pass
    
    def setMaxEpisodes(self, max_episodes: size_t) -> None:
        """
        Set maximum number of episodes.
n        
        Args:
            max_episodes: Maximum number of episodes
        """
        pass
    
    def getMaxEpisodes(self) -> size_t:
        """
        Get maximum number of episodes.
n        
        Returns:
            Maximum number of episodes
        """
        pass
    
    def cleanupOldEpisodes(self) -> None:
        """
        Clean up old episodes.
        """
        pass
    
    def clearAllEpisodes(self) -> None:
        """
        Clear all episodes.
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

```python
class AssociativeMemory:
    """
    Associative memory for storing connections between patterns.
n    
    Phase 6 Enhancement: Integrated with concept formation.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize associative memory.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def associate(self, pattern1: List[float], pattern2: List[float],
                 strength: float) -> bool:
        """
        Create association between two patterns.
n        
        Args:
            pattern1: First pattern
            pattern2: Second pattern
            strength: Association strength
            
        Returns:
            True if association was created successfully, False otherwise
        """
        pass
    
    def getSimilarity(self, pattern1: List[float],
                     pattern2: List[float]) -> float:
        """
        Get similarity between two patterns.
n        
        Args:
            pattern1: First pattern
            pattern2: Second pattern
            
        Returns:
            Similarity value (0.0-1.0)
        """
        pass
    
    def getAssociationStrength(self, pattern1: List[float],
                             pattern2: List[float]) -> float:
        """
        Get association strength between two patterns.
n        
        Args:
            pattern1: First pattern
            pattern2: Second pattern
            
        Returns:
            Association strength
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

## Neuromodulation Systems API

The neuromodulation systems provide signals that affect learning, attention, and behavior.

### Classes: Dopamine, Curiosity, Novelty, PredictionError

```python
class Dopamine:
    """
    Dopamine system for reward prediction error and learning modulation.
n    
    Phase 6 Enhancement: Full integration with all plasticity and learning systems.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize dopamine system.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def getLevel(self) -> float:
        """
        Get current dopamine level.
n        
        Returns:
            Dopamine level (-1.0 to 1.0)
        """
        pass
    
    def setLevel(self, level: float) -> None:
        """
        Set dopamine level.
n        
        Args:
            level: Dopamine level (-1.0 to 1.0)
        """
        pass
    
    def signalReward(self, reward: float) -> None:
        """
        Signal a reward.
n        
        Args:
            reward: Reward value
        """
        pass
    
    def signalRewardPredictionError(self, error: float) -> None:
        """
        Signal a reward prediction error.
n        
        Args:
            error: Reward prediction error (-1.0 to 1.0)
        """
        pass
    
    def getPlasticityFactor(self) -> float:
        """
        Get plasticity factor.
n        
        Returns:
            Plasticity factor (0.0-2.0)
        """
        pass
    
    def update(self, timestep_duration: float) -> None:
        """
        Update dopamine system.
n        
        Args:
            timestep_duration: Duration of timestep
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

```python
class Curiosity:
    """
    Curiosity system for exploration behavior.
n    
    Phase 6 Enhancement: Integrated with exploration and action selection.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize curiosity system.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def getLevel(self) -> float:
        """
        Get current curiosity level.
n        
        Returns:
            Curiosity level (0.0-1.0)
        """
        pass
    
    def update(self, timestep_duration: float) -> None:
        """
        Update curiosity system.
n        
        Args:
            timestep_duration: Duration of timestep
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

```python
class Novelty:
    """
    Novelty detection system for identifying new experiences.
n    
    Phase 6 Enhancement: Integrated with attention and memory.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize novelty system.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def update(self, timestep_duration: float) -> None:
        """
        Update novelty system.
n        
        Args:
            timestep_duration: Duration of timestep
        """
        pass
    
    def getLevel(self) -> float:
        """
        Get current novelty level.
n        
        Returns:
            Novelty level (0.0-1.0)
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

```python
class PredictionError:
    """
    Prediction error system for learning signals.
n    
    Phase 6 Enhancement: Integrated with learning and attention.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize prediction error system.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def update(self, timestep_duration: float) -> None:
        """
        Update prediction error system.
n        
        Args:
            timestep_duration: Duration of timestep
        """
        pass
    
    def getSignal(self) -> float:
        """
        Get current prediction error signal.
n        
        Returns:
            Prediction error signal (-1.0 to 1.0)
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

## Development System API

The DevelopmentSystem class manages developmental stages and age-based effects.

### Class: DevelopmentSystem

```python
class DevelopmentSystem:
    """
    Development system for managing developmental stages and age-based effects.
n    
    Phase 6 Enhancement: Full integration with all neural and cognitive systems.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize development system.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def update(self, brain: 'Brain', rng: RandomGenerator,
              timestep_duration: float) -> None:
        """
        Update development system.
n        
        Args:
            brain: Reference to brain
            rng: Random number generator
            timestep_duration: Duration of timestep
        """
        pass
    
    def getCurrentStage(self) -> 'DevelopmentalStage':
        """
        Get current developmental stage.
n        
        Returns:
            Current developmental stage
        """
        pass
    
    def getAge(self) -> float:
        """
        Get current age.
n        
        Returns:
            Current age in simulation time units
        """
        pass
    
    def getPlasticityModifier(self) -> float:
        """
        Get plasticity modifier.
n        
        Returns:
            Plasticity modifier (0.0-2.0)
        """
        pass
    
    def getExcitabilityModifier(self) -> float:
        """
        Get excitability modifier.
n        
        Returns:
            Excitability modifier (0.0-2.0)
        """
        pass
    
    def getAttentionModifier(self) -> float:
        """
        Get attention modifier.
n        
        Returns:
            Attention modifier (0.0-2.0)
        """
        pass
    
    def getMemoryModifier(self) -> float:
        """
        Get memory consolidation modifier.
n        
        Returns:
            Memory consolidation modifier (0.0-2.0)
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

## Attention System API

The AttentionalSelection class manages competitive selection of concepts and actions.

### Class: AttentionalSelection

```python
class AttentionalSelection:
    """
    Attention system for competitive selection of concepts and actions.
n    
    Phase 6 Enhancement: Integrated with all cognition and perception systems.
    """
    
    def __init__(self, brain: 'Brain'):
        """
        Initialize attention system.
n        
        Args:
            brain: Reference to the Brain instance this system is integrated with
        """
        pass
    
    def update(self, timestep_duration: float) -> None:
        """
        Update attention system.
n        
        Args:
            timestep_duration: Duration of timestep
        """
        pass
    
    def setInhibitionStrength(self, strength: float) -> None:
        """
        Set inhibition strength.
n        
        Args:
            strength: Inhibition strength (0.0-1.0)
        """
        pass
    
    def setExcitationStrength(self, strength: float) -> None:
        """
        Set excitation strength.
n        
        Args:
            strength: Excitation strength (0.0-2.0)
        """
        pass
    
    def getCurrentFocus(self) -> 'AttentionWeights':
        """
        Get current attentional focus.
n        
        Returns:
            Current attentional focus
        """
        pass
    
    def processCompetition(self, competitors: List[int]) -> None:
        """
        Process competition between competitors.
n        
        Args:
            competitors: List of competitor neuron IDs
        """
        pass
    
    def getAttendedTargets(self) -> List[int]:
        """
        Get attended targets.
n        
        Returns:
            List of attended target IDs
        """
        pass
    
    def setPredictionErrorBias(self, bias: float) -> None:
        """
        Set prediction error bias.
n        
        Args:
            bias: Prediction error bias (-1.0 to 1.0)
        """
        pass
    
    def setNoveltyBias(self, bias: float) -> None:
        """
        Set novelty bias.
n        
        Args:
            bias: Novelty bias (0.0-1.0)
        """
        pass
    
    def getMemoryUsage(self) -> size_t:
        """
        Get memory usage.
n        
        Returns:
            Memory usage in bytes
        """
        pass
```

## Python Integration Examples

### Complete Brain Integration Example

```python
import pybind11
from nlm import Brain, AgentBrain, SimpleWorld

# Create brain with Phase 6 features
config = {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1
}

brain = Brain(config)
brain.initialize()

# Create agent with enhanced APIs
agent = AgentBrain(brain)

# Create environment
world = SimpleWorld()
world.initialize(16, 16)

# Run simulation with concept formation and planning
for step in range(1000):
    # Get observation
    percept = world.getPercept(agent.getBrain().getRegions()[0].get())
    
    # Process input (triggers concept formation)
    agent.processSensoryInput(percept)
    
    # Brain step (triggers prediction and planning)
    brain.step(step, step * 0.001)
    
    # Get planned actions
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply actions
    world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd)
    
    # Compute reward
    reward = world.computeReward(agent.getBrain()->getRegions()[0].get())
    
    # Apply neuromodulation
    agent.applyRewardModulation(reward, 0.0)
    
    # Check for new concepts
    concept_formation = brain.getConceptFormation()
    if concept_formation and concept_formation.getConceptCount() > 0:
        print(f"New concepts formed: {concept_formation.getConceptCount()}")

print("Simulation complete with concept formation and neural planning")
```

### Experiment with Memory and Replay

```python
# Create experiment for memory analysis
experiment = Experiment()
experiment.initialize({
    "max_steps": 2000,
    "neuron_count": 500
})

# Run experiment and collect metrics
results = experiment.run()

# Analyze memory integration
working_mem = experiment.getBrain().getWorkingMemory()
episodic_mem = experiment.getBrain().getEpisodicMemory()

print(f"Working memory active traces: {working_mem.getActiveTraces()}")
print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
print(f"Replay episodes available: {len(episodic_mem.getEpisodesForReplay(5))}")

# Check prediction accuracy
prediction = experiment.getBrain().getPredictionSystem()
if prediction:
    print(f"Prediction confidence: {prediction.getConfidence()}")
    print(f"Prediction error: {prediction.getPredictionError()}")
```

### Integration with Enhanced Python API

```python
from nlm.concept_formation import ConceptFormation
from nlm.planning import NeuralPlanner
from nlm.prediction import PredictionSystem
from nlm.memory import WorkingMemory, EpisodicMemory
from nlm.neuromodulation import Dopamine, Curiosity, Novelty
from nlm.development import DevelopmentSystem

# Access all Phase 6 systems through enhanced API
concept_form = ConceptFormation(brain)
planner = NeuralPlanner(brain)
prediction = PredictionSystem(brain)
working_mem = WorkingMemory(brain)
episodic_mem = EpisodicMemory(brain)
dopamine = Dopamine(brain)
curiosity = Curiosity(brain)
novelty = Novelty(brain)
development = DevelopmentSystem(brain)

# Use enhanced features
concept_id = concept_form.presentExperience(
    pattern=[0.1, 0.5, 0.9, 0.2],
    features=[0.3, 0.7, 0.1],
    reward=0.8,
    timestamp=current_step
)

# Plan actions with neural planning
plans = planner.planActions(
    input_signals=current_state,
    goal_pattern=goal_pattern,
    prediction_system=prediction,
    horizon=5
)

# Analyze concept formation
concepts = concept_form.getConcepts()
concept_analysis = concept_form.analyzeConceptFormation()
```

## API Design Principles

### 1. System Independence

Each Phase 6 system is implemented as an independent class with its own responsibilities:
- **ConceptFormation**: Pattern abstraction and concept management
- **NeuralPlanner**: Action sequence generation and evaluation
- **PredictionSystem**: Future state anticipation
- **Memory Systems**: Storage and retrieval of experiences
- **Neuromodulation**: Learning signals and behavioral modulation
- **Development System**: Age-based system regulation
- **Attention System**: Competitive selection of representations

### 2. Integration Through Brain Accessor Methods

All systems access the main Brain instance through accessor methods:
```python
# Access systems through Brain class
concept_form = brain.getConceptFormation()
planner = brain.getPlanner()
prediction = brain.getPredictionSystem()
working_mem = brain.getWorkingMemory()
```

### 3. Event-Driven Architecture

Systems communicate through events and callbacks:
- **Concept Formation**: Receives patterns from working memory
- **Prediction System**: Updated by concept formation
- **Neuromodulation**: Responds to rewards and prediction errors
- **Planning**: Uses concepts and predictions for action selection

### 4. Type Safety

All Python classes use type annotations and validation:
```python
def presentExperience(self, pattern: List[float], features: List[float],
                      reward: float, timestamp: int) -> int:
    """Present experience with type checking"""
```

### 5. Performance Considerations

- Efficient memory management through custom deleters
- Minimal overhead for system access
- Optimized for large-scale simulations

## Migration Guide

### From Original NLM to Phase 6

#### Old API (Original NLM):
```python
from nlm import Brain

# Limited system access
brain = Brain(config)
brain.initialize()

# Limited functionality
some_result = brain.some_method()
```

#### New API (Phase 6 Enhanced):
```python
from nlm import Brain, ConceptFormation, NeuralPlanner, PredictionSystem

# Full system access
brain = Brain(config)
brain.initialize()

# Enhanced system access
concept_form = brain.getConceptFormation()
planner = brain.getPlanner()
prediction = brain.getPredictionSystem()

# Enhanced functionality
concept_id = concept_form.presentExperience(pattern, features, reward)
plans = planner.planActions(current_state, goal_pattern, prediction)
```

### Migration Steps

1. **Update Imports**: Add imports for new Phase 6 modules
2. **Update System Access**: Use Brain accessor methods for system access
3. **Update Functionality**: Replace old methods with new enhanced methods
4. **Update Experiment Code**: Use Phase 6IntegrationExperiment for comprehensive validation
5. **Update Configuration**: Update configuration for Phase 6 features

### Example Migration

```python
# OLD CODE
from nlm import Brain

brain = Brain(config)
brain.initialize()
result = brain.getSomeResult()

# NEW CODE
from nlm import Brain, Phase6IntegrationExperiment

brain = Brain(config)
brain.initialize()

# Access Phase 6 systems
concept_form = brain.getConceptFormation()
planner = brain.getPlanner()

# Use enhanced functionality
concept_id = concept_form.presentExperience(pattern, features, reward)

# Run validation experiment
experiment = Phase6IntegrationExperiment()
results = experiment.run(config)
```

## Performance Considerations

### Memory Management

Phase 6 systems use efficient memory management:
- Custom deleters prevent memory leaks
- Reference counting for system access
- Automatic cleanup of old traces and concepts

### Computational Complexity

- **Concept Formation**: O(n) pattern matching and similarity computation
- **Neural Planning**: O(m^2) action sequence evaluation (where m = number of actions)
- **Prediction System**: O(n) prediction computation
- **Memory Systems**: O(1) access, O(log n) updates for large-scale simulations

### Scalability

Phase 6 systems scale efficiently:
- **Memory**: Linear scaling with simulation size
- **Computation**: Parallelizable operations for large neural networks
- **Storage**: Configurable limits for memory and concepts

## Error Handling

### Standard Exceptions

```python
try:
    concept_id = concept_form.presentExperience(pattern, features, reward)
except ValueError as e:
    print(f"Invalid input: {e}")
except RuntimeError as e:
    print(f"System error: {e}")
```

### System Validation

```python
# Check if all systems are properly initialized
if brain.getConceptFormation() is None:
    print("Error: Concept formation system not initialized")
if brain.getPredictionSystem() is None:
    print("Error: Prediction system not initialized")
if brain.getWorkingMemory() is None:
    print("Error: Working memory system not initialized")
```

### Input Validation

```python
def presentExperience(self, pattern: List[float], features: List[float],
                      reward: float, timestamp: int) -> int:
    # Validate input parameters
    if not pattern:
        raise ValueError("Pattern cannot be empty")
    if not features:
        raise ValueError("Features cannot be empty")
    if not (0.0 <= reward <= 1.0):
        raise ValueError("Reward must be between 0.0 and 1.0")
    if timestamp < 0:
        raise ValueError("Timestamp cannot be negative")
    
    # Additional validation...
```

## Type System

### Core Types

```python
from typing import List, Dict, Optional, Union

# Basic types
NeuronId = int
SynapseId = int
RegionId = int
Timestamp = float
SimulationStep = int

# Signal types
InternalSignals = List[float]
AttentionWeights = Dict[int, float]
Reward = float

# Action types
Action = str  # "MoveForward", "MoveBackward", "TurnLeft", etc.

# System types
class ConceptFormation
class NeuralPlanner
class PredictionSystem
class WorkingMemory
class EpisodicMemory
class AssociativeMemory
class Dopamine
class Curiosity
class Novelty
class PredictionError
class DevelopmentSystem
class AttentionalSelection
```

### Type Definitions

```python
# Type aliases for common types
Pattern = List[float]
FeatureVector = List[float]
ConceptId = int
NeuronState = Dict[str, float]
SynapseState = Dict[str, float]
```

This comprehensive API documentation provides complete reference material for using the Phase 6 NLM enhanced Python bindings. The Phase 6 integration enables developers to leverage all the power of the integrated artificial brain while maintaining compatibility with existing NLM APIs.

<document>
<document>
<document>