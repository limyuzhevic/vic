"""Learning and plasticity control for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Callable
from abc import ABC, abstractmethod
import time
from enum import Enum


class LearningRule(Enum):
    STDP = "STDP"
    HEBBIAN = "Hebbian"
    LTP = "LTP"
    LTD = "LTD"
    Oja = "Oja"
    BCM = "BCM"


class PlasticityType(Enum):
    SYNAPTIC = "synaptic"
    STRUCTURAL = "structural"
    NEURONAL = "neuronal"


class LearningScheduler:
    """
    Schedule learning sessions for neural networks.
    
    This class manages the timing and sequencing of learning sessions,
    allowing for adaptive scheduling based on network performance and
    environmental factors.
    """
    
    def __init__(self, base_interval: float = 1.0, adaptation_rate: float = 0.1):
        """
        Initialize learning scheduler.
        
        Args:
            base_interval: Base interval between learning sessions
            adaptation_rate: Rate at which intervals adapt to performance
        """
        self.base_interval = base_interval
        self.adaptation_rate = adaptation_rate
        self.schedule = []
        self.performance_history = deque(maxlen=100)
        self.current_interval = base_interval
        self.next_learning_time = time.time()
        
    def schedule_learning(self, trigger: str = "time", performance: float = None, 
                         priority: str = "normal", custom_time: float = None) -> Dict[str, Any]:
        """
        Schedule a learning session.
        
        Args:
            trigger: What triggered this learning ("time", "performance", "manual")
            performance: Current performance (for performance-based scheduling)
            priority: Learning priority ("high", "normal", "low")
            custom_time: Custom scheduled time (if None, use automatic scheduling)
            
        Returns:
            Dict[str, Any]: Learning session information
        """
        if custom_time is not None:
            scheduled_time = custom_time
        elif trigger == "time":
            scheduled_time = self.next_learning_time
        elif trigger == "performance" and performance is not None:
            # Adaptive scheduling based on performance
            if performance < 0.5:  # Poor performance
                self.current_interval *= (1 - self.adaptation_rate)
            elif performance > 0.8:  # Good performance
                self.current_interval *= (1 + self.adaptation_rate)
            
            scheduled_time = time.time() + self.current_interval
        else:
            scheduled_time = time.time() + self.base_interval
            
        # Cap the interval to reasonable bounds
        self.current_interval = max(0.1, min(self.current_interval, 100.0))
        self.next_learning_time = scheduled_time
        
        learning_session = {
            "id": f"learning_{len(self.schedule)}",
            "scheduled_time": scheduled_time,
            "trigger": trigger,
            "performance": performance,
            "priority": priority,
            "status": "scheduled"
        }
        
        self.schedule.append(learning_session)
        return learning_session
        
    def execute_scheduled_sessions(self) -> List[Dict[str, Any]]:
        """
        Execute all scheduled learning sessions that are due.
        
        Returns:
            List[Dict[str, Any]]: Executed sessions
        """
        current_time = time.time()
        completed = []
        
        for session in self.schedule:
            if session["status"] == "scheduled" and current_time >= session["scheduled_time"]:
                session["status"] = "completed"
                session["actual_time"] = current_time
                completed.append(session)
                
        # Update performance history
        if completed:
            avg_performance = np.mean([s.get("performance", 0.5) for s in completed])
            self.performance_history.append(avg_performance)
            
        return completed
        
    def get_schedule(self) -> List[Dict[str, Any]]:
        """Get current learning schedule."""
        return self.schedule.copy()
        
    def cancel_session(self, session_id: str) -> bool:
        """
        Cancel a scheduled learning session.
        
        Args:
            session_id: ID of session to cancel
            
        Returns:
            bool: True if session was found and cancelled
        """
        for session in self.schedule:
            if session["id"] == session_id:
                session["status"] = "cancelled"
                return True
        return False
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get learning statistics."""
        if not self.performance_history:
            return {"total_sessions": len(self.schedule), "current_interval": self.current_interval}
            
        completed_sessions = [s for s in self.schedule if s["status"] == "completed"]
        
        return {
            "total_sessions": len(self.schedule),
            "completed_sessions": len(completed_sessions),
            "scheduled_sessions": len([s for s in self.schedule if s["status"] == "scheduled"]),
            "cancelled_sessions": len([s for s in self.schedule if s["status"] == "cancelled"]),
            "current_interval": self.current_interval,
            "avg_performance": np.mean(self.performance_history) if self.performance_history else 0.0,
            "performance_trend": self._calculate_performance_trend()
        }
        
    def _calculate_performance_trend(self) -> float:
        """Calculate performance trend (positive = improving)."""
        if len(self.performance_history) < 2:
            return 0.0
            
        x = np.arange(len(self.performance_history))
        y = np.array(self.performance_history)
        
        slope = np.polyfit(x, y, 1)[0]
        return slope


class PlasticityController:
    """
    Control plasticity rules for neural network adaptation.
    
    This class manages various plasticity rules and their parameters,
    allowing for fine-grained control over how neural connections change
    during learning.
    """
    
    def __init__(self):
        """Initialize plasticity controller."""
        self.current_rule = LearningRule.STDP
        self.rule_parameters = {}
        self.plasticity_types = {}
        self.adaptive_parameters = {}
        
        # Initialize default parameters
        self._initialize_default_parameters()
        
    def _initialize_default_parameters(self) -> None:
        """Set up default plasticity rule parameters."""
        self.rule_parameters = {
            LearningRule.STDP: {
                "learning_rate": 0.01,
                "time_constant_pre": 20.0,
                "time_constant_post": 20.0,
                "weight_range": (-1.0, 1.0),
                "baseline_weight": 0.0
            },
            LearningRule.HEBBIAN: {
                "learning_rate": 0.01,
                "weight_range": (-1.0, 1.0),
                "baseline_weight": 0.0
            },
            LearningRule.LTP: {
                "learning_rate": 0.02,
                "threshold_pre": 0.5,
                "threshold_post": 0.5,
                "weight_range": (0.0, 1.0)
            },
            LearningRule.LTD: {
                "learning_rate": -0.01,
                "threshold_pre": -0.5,
                "threshold_post": -0.5,
                "weight_range": (-1.0, 0.0)
            },
            LearningRule.Oja: {
                "learning_rate": 0.01,
                "weight_range": (-1.0, 1.0),
                "baseline_weight": 0.0
            },
            LearningRule.BCM: {
                "learning_rate": 0.01,
                "threshold": 0.0,
                "weight_range": (-1.0, 1.0),
                "baseline_weight": 0.0
            }
        }
        
    def set_rule(self, rule: LearningRule, **kwargs) -> None:
        """
        Set the current plasticity rule.
        
        Args:
            rule: Learning rule to use
            **kwargs: Additional rule parameters
            
        Example:
            >>> controller.set_rule(LearningRule.STDP, learning_rate=0.02)
        """
        if rule not in self.rule_parameters:
            raise ValueError(f"Unknown learning rule: {rule}")
            
        self.current_rule = rule
        
        # Update rule parameters
        for key, value in kwargs.items():
            if key in self.rule_parameters[rule]:
                self.rule_parameters[rule][key] = value
        
    def add_plasticity_type(self, plasticity_type: PlasticityType, parameters: Dict[str, Any]):
        """
        Add a plasticity type with its parameters.
        
        Args:
            plasticity_type: Type of plasticity
            parameters: Parameters for this plasticity type
        """
        self.plasticity_types[plasticity_type] = parameters
        
    def enable_adaptive_plasticity(self, learning_rate: float = 0.01, 
                                  target_performance: float = 0.8):
        """
        Enable adaptive plasticity based on performance.
        
        Args:
            learning_rate: Base learning rate
            target_performance: Target performance level
        """
        self.adaptive_parameters = {
            "enabled": True,
            "base_learning_rate": learning_rate,
            "target_performance": target_performance,
            "current_learning_rate": learning_rate,
            "performance_history": deque(maxlen=100)
        }
        
    def calculate_plasticity(self, pre_activity: float, post_activity: float,
                           weight: float, time_delta: float = 0.0) -> Dict[str, Any]:
        """
        Calculate plasticity change for a synapse.
        
        Args:
            pre_activity: Pre-synaptic neuron activity
            post_activity: Post-synaptic neuron activity
            weight: Current synaptic weight
            time_delta: Time since last spike (for STDP)
            
        Returns:
            Dict[str, Any]: Plasticity change results
        """
        rule_params = self.rule_parameters[self.current_rule]
        weight_range = rule_params["weight_range"]
        
        # Calculate learning signal based on rule
        if self.current_rule == LearningRule.STDP:
            if time_delta > 0:
                # STDP calculation
                learning_signal = rule_params["learning_rate"] * np.exp(-time_delta / rule_params["time_constant_pre"])
            else:
                learning_signal = -rule_params["learning_rate"] * np.exp(time_delta / rule_params["time_constant_post"])
                
        elif self.current_rule == LearningRule.HEBBIAN:
            learning_signal = rule_params["learning_rate"] * pre_activity * post_activity
            
        elif self.current_rule == LearningRule.LTP:
            learning_signal = rule_params["learning_rate"] if (pre_activity > rule_params.get("threshold_pre", 0) and 
                                                              post_activity > rule_params.get("threshold_post", 0)) else 0
            
        elif self.current_rule == LearningRule.LTD:
            learning_signal = rule_params["learning_rate"] if (pre_activity < rule_params.get("threshold_pre", 0) and 
                                                              post_activity < rule_params.get("threshold_post", 0)) else 0
            
        elif self.current_rule == LearningRule.Oja:
            learning_signal = rule_params["learning_rate"] * pre_activity * (post_activity - weight * post_activity)
            
        elif self.current_rule == LearningRule.BCM:
            bcm_threshold = rule_params.get("threshold", 0.0)
            bcm_signal = post_activity - bcm_threshold
            learning_signal = rule_params["learning_rate"] * pre_activity * bcm_signal
        else:
            learning_signal = 0.0
            
        # Apply adaptive plasticity if enabled
        if self.adaptive_parameters.get("enabled", False):
            learning_signal *= self.adaptive_parameters["current_learning_rate"] / rule_params["learning_rate"]
        
        # Calculate weight change
        weight_change = learning_signal * pre_activity
        
        # Clamp to weight range
        new_weight = np.clip(weight + weight_change, weight_range[0], weight_range[1])
        
        return {
            "learning_signal": learning_signal,
            "weight_change": weight_change,
            "old_weight": weight,
            "new_weight": new_weight,
            "rule_used": self.current_rule.value,
            "clamp_applied": new_weight != weight + weight_change
        }
        
    def get_current_parameters(self) -> Dict[str, Any]:
        """Get current plasticity parameters."""
        return {
            "current_rule": self.current_rule.value,
            "rule_parameters": self.rule_parameters[self.current_rule],
            "plasticity_types": {pt.value: params for pt, params in self.plasticity_types.items()},
            "adaptive_enabled": self.adaptive_parameters.get("enabled", False),
            "adaptive_parameters": self.adaptive_parameters
        }


class AdaptiveLearning:
    """
    Adaptive learning algorithms that adjust based on performance.
    
    This class implements various adaptive learning strategies that modify
    learning behavior in response to network performance and environmental
    conditions.
    """
    
    def __init__(self):
        """Initialize adaptive learning system."""
        self.algorithms = {}
        self.performance_history = deque(maxlen=200)
        self.adaptation_strategies = {}
        self.current_algorithm = None
        self.algorithm_performance = {}
        
    def add_algorithm(self, name: str, algorithm: Callable, parameters: Dict[str, Any]):
        """
        Add an adaptive learning algorithm.
        
        Args:
            name: Algorithm name
            algorithm: Algorithm function (takes performance, history, parameters)
            parameters: Algorithm-specific parameters
        """
        self.algorithms[name] = {
            "function": algorithm,
            "parameters": parameters,
            "performance_history": deque(maxlen=100)
        }
        self.algorithm_performance[name] = deque(maxlen=50)
        
    def select_algorithm(self, current_performance: float) -> str:
        """
        Select the best algorithm based on current performance.
        
        Args:
            current_performance: Current network performance
            
        Returns:
            str: Selected algorithm name
        """
        if not self.algorithms:
            return None
            
        # Record current performance
        self.performance_history.append(current_performance)
        
        # Find best algorithm based on recent performance
        best_algorithm = None
        best_score = -float('inf')
        
        for name, algo_info in self.algorithms.items():
            if name in self.algorithm_performance:
                algo_perf = list(self.algorithm_performance[name])
                if algo_perf:
                    score = np.mean(algo_perf[-10:]) * (1 + len(algo_perf) * 0.01)  # Recent + diversity bonus
                    if score > best_score:
                        best_score = score
                        best_algorithm = name
                        
        self.current_algorithm = best_algorithm
        return best_algorithm
        
    def learn(self, performance: float, network_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Perform adaptive learning based on current performance.
        
        Args:
            performance: Current network performance
            network_state: Current network state
            
        Returns:
            Dict[str, Any]: Learning results
        """
        # Record performance
        self.performance_history.append(performance)
        
        # Select algorithm
        algorithm_name = self.select_algorithm(performance)
        
        if algorithm_name is None or algorithm_name not in self.algorithms:
            return {"algorithm": None, "learning_signal": 0.0, "message": "No algorithm selected"}
            
        algo_info = self.algorithms[algorithm_name]
        
        # Apply algorithm
        learning_signal = algo_info["function"](performance, self.performance_history, algo_info["parameters"])
        
        # Record algorithm performance
        if algorithm_name in self.algorithm_performance:
            self.algorithm_performance[algorithm_name].append(performance)
            
        return {
            "algorithm": algorithm_name,
            "learning_signal": learning_signal,
            "performance": performance,
            "adaptation_count": len(self.performance_history)
        }
        
    def add_adaptation_strategy(self, name: str, strategy: Callable, conditions: Dict[str, Any]):
        """
        Add an adaptation strategy that triggers under certain conditions.
        
        Args:
            name: Strategy name
            strategy: Strategy function (takes network state, returns adaptation)
            conditions: Conditions under which strategy is triggered
        """
        self.adaptation_strategies[name] = {
            "function": strategy,
            "conditions": conditions,
            "trigger_count": 0
        }
        
    def check_adaptation_opportunities(self, network_state: Dict[str, Any]) -> List[Dict[str, Any]]:
        """
        Check for opportunities to apply adaptation strategies.
        
        Args:
            network_state: Current network state
            
        Returns:
            List[Dict[str, Any]]: Applied adaptations
        """
        adaptations = []
        
        for name, strategy_info in self.adaptation_strategies.items():
            conditions_met = all(
                network_state.get(key, default) == value 
                for key, value in strategy_info["conditions"].items()
            )
            
            if conditions_met:
                adaptation = strategy_info["function"](network_state)
                adaptation["strategy"] = name
                adaptation["trigger_count"] = strategy_info["trigger_count"]
                adaptations.append(adaptation)
                
                strategy_info["trigger_count"] += 1
                
        return adaptations
        
    def get_algorithm_statistics(self) -> Dict[str, Any]:
        """Get statistics about algorithm performance."""
        stats = {}
        
        for name, algo_perf in self.algorithm_performance.items():
            if algo_perf:
                stats[name] = {
                    "average_performance": np.mean(algo_perf),
                    "recent_performance": np.mean(list(algo_perf)[-5:]) if len(algo_perf) >= 5 else np.mean(algo_perf),
                    "performance_std": np.std(algo_perf),
                    "usage_count": len(algo_perf)
                }
                
        return {
            "current_algorithm": self.current_algorithm,
            "algorithm_stats": stats,
            "overall_performance": np.mean(self.performance_history) if self.performance_history else 0.0,
            "performance_trend": self._calculate_performance_trend()
        }
        
    def _calculate_performance_trend(self) -> float:
        """Calculate performance trend."""
        if len(self.performance_history) < 2:
            return 0.0
            
        x = np.arange(len(self.performance_history))
        y = np.array(self.performance_history)
        
        slope = np.polyfit(x, y, 1)[0]
        return slope


class CuriosityDrivenLearning:
    """
    Intrinsic motivation and curiosity-driven learning.
    
    This class implements curiosity-driven learning mechanisms that encourage
    exploration and discovery through intrinsic motivation signals.
    """
    
    def __init__(self, exploration_rate: float = 0.1, novelty_threshold: float = 0.5):
        """
        Initialize curiosity-driven learning system.
        
        Args:
            exploration_rate: Base exploration rate
            novelty_threshold: Threshold for considering something novel
        """
        self.exploration_rate = exploration_rate
        self.novelty_threshold = novelty_threshold
        
        self.curiosity_signals = {}
        self.novelty_detector = NoveltyDetector(novelty_threshold)
        self.intrinsic_motivation = IntrinsicMotivation(exploration_rate)
        self.exploration_history = deque(maxlen=100)
        
    def process_experience(self, experience: Dict[str, Any]) -> Dict[str, Any]:
        """
        Process an experience through curiosity-driven mechanisms.
        
        Args:
            experience: Experience data
            
        Returns:
            Dict[str, Any]: Curiosity-driven learning results
        """
        # Extract relevant features from experience
        features = self._extract_features(experience)
        
        # Detect novelty
        novelty_score = self.novelty_detector.detect_novelty(features)
        
        # Calculate curiosity signal
        curiosity_signal = self.intrinsic_motivation.calculate_curiosity(
            novelty_score, experience
        )
        
        # Determine exploration action
        exploration_action = self._determine_exploration_action(
            curiosity_signal, experience
        )
        
        # Record exploration
        self.exploration_history.append({
            "experience": experience,
            "novelty_score": novelty_score,
            "curiosity_signal": curiosity_signal,
            "exploration_action": exploration_action,
            "timestamp": time.time()
        })
        
        return {
            "novelty_score": novelty_score,
            "curiosity_signal": curiosity_signal,
            "exploration_action": exploration_action,
            "should_explore": exploration_action["explore"],
            "exploration_strength": exploration_action["strength"]
        }
        
    def _extract_features(self, experience: Dict[str, Any]) -> Dict[str, Any]:
        """Extract relevant features from experience."""
        # Simple feature extraction (would be more sophisticated in real implementation)
        features = {}
        
        if "sensory_input" in experience:
            sensory = experience["sensory_input"]
            features["sensory_features"] = np.mean(np.abs(sensory))
            features["sensory_variance"] = np.var(sensory)
            
        if "action" in experience:
            action = experience["action"]
            features["action_features"] = np.mean(np.abs(action))
            features["action_variance"] = np.var(action)
            
        if "reward" in experience:
            features["reward"] = experience["reward"]
            
        return features
        
    def _determine_exploration_action(self, curiosity_signal: float, 
                                    experience: Dict[str, Any]) -> Dict[str, Any]:
        """Determine exploration action based on curiosity signal."""
        # Determine if exploration is needed
        explore = curiosity_signal > self.exploration_rate
        
        # Calculate exploration strength
        exploration_strength = min(1.0, curiosity_signal / self.exploration_rate)
        
        # Determine exploration type
        novelty_score = self.novelty_detector.detect_novelty(self._extract_features(experience))
        if novelty_score > 0.8:
            explore_type = "deep_exploration"
        elif novelty_score > 0.5:
            explore_type = "broad_exploration"
        else:
            explore_type = "focused_exploration"
            
        return {
            "explore": explore,
            "strength": exploration_strength,
            "type": explore_type,
            "novelty_score": novelty_score
        }
        
    def get_curiosity_statistics(self) -> Dict[str, Any]:
        """Get curiosity-driven learning statistics."""
        if not self.exploration_history:
            return {"total_experiences": 0}
            
        exploration_actions = [e for e in self.exploration_history if e["exploration_action"]["explore"]]
        
        return {
            "total_experiences": len(self.exploration_history),
            "exploration_count": len(exploration_actions),
            "exploration_percentage": len(exploration_actions) / len(self.exploration_history) * 100,
            "average_novelty_score": np.mean([e["novelty_score"] for e in self.exploration_history]),
            "average_curiosity_signal": np.mean([e["curiosity_signal"] for e in self.exploration_history]),
            "exploration_type_distribution": self._get_exploration_type_distribution(),
            "recent_exploration": list(self.exploration_history)[-10:]
        }
        
    def _get_exploration_type_distribution(self) -> Dict[str, int]:
        """Get distribution of exploration types."""
        type_counts = {}
        for event in self.exploration_history:
            explore_type = event["exploration_action"]["type"]
            type_counts[explore_type] = type_counts.get(explore_type, 0) + 1
            
        return type_counts


class NoveltyDetector:
    """Detect novelty in experiences."""
    
    def __init__(self, threshold: float = 0.5):
        """Initialize novelty detector."""
        self.threshold = threshold
        self.experience_buffer = deque(maxlen=1000)
        self.novelty_scores = deque(maxlen=100)
        
    def detect_novelty(self, features: Dict[str, Any]) -> float:
        """
        Detect novelty in features.
        
        Args:
            features: Feature vector
            
        Returns:
            float: Novelty score (0-1)
        """
        # Convert features to array
        feature_array = np.array(list(features.values()))
        
        # Calculate novelty based on buffer statistics
        if len(self.experience_buffer) < 10:
            # Not enough data yet
            novelty_score = 0.5
        else:
            # Calculate distance to nearest neighbor in buffer
            distances = []
            for buffer_features in self.experience_buffer:
                buffer_array = np.array(list(buffer_features.values()))
                distance = np.linalg.norm(feature_array - buffer_array)
                distances.append(distance)
                
            if distances:
                min_distance = min(distances)
                # Normalize distance (assuming reasonable feature scale)
                novelty_score = min(1.0, min_distance / 10.0)
            else:
                novelty_score = 0.5
                
        # Store in history
        self.novelty_scores.append(novelty_score)
        self.experience_buffer.append(features)
        
        return novelty_score
        
    def get_novelty_statistics(self) -> Dict[str, Any]:
        """Get novelty detection statistics."""
        if not self.novelty_scores:
            return {"total_detections": 0}
            
        return {
            "total_detections": len(self.novelty_scores),
            "average_novelty": np.mean(self.novelty_scores),
            "max_novelty": np.max(self.novelty_scores),
            "min_novelty": np.min(self.novelty_scores),
            "novelty_above_threshold": sum(1 for s in self.novelty_scores if s > self.threshold),
            "buffer_size": len(self.experience_buffer)
        }


class IntrinsicMotivation:
    """Intrinsic motivation system."""
    
    def __init__(self, exploration_rate: float = 0.1):
        """Initialize intrinsic motivation system."""
        self.exploration_rate = exploration_rate
        self.motivation_history = deque(maxlen=100)
        
    def calculate_curiosity(self, novelty_score: float, experience: Dict[str, Any]) -> float:
        """
        Calculate curiosity signal.
        
        Args:
            novelty_score: Novelty score
            experience: Experience data
            
        Returns:
            float: Curiosity signal (0-1)
        """
        # Curiosity is a combination of novelty and unexpected outcomes
        curiosity = novelty_score * self.exploration_rate
        
        # Adjust based on experience
        if "reward" in experience:
            reward = experience["reward"]
            if reward > 0:  # Positive reward
                curiosity *= 1.2
            elif reward < 0:  # Negative reward
                curiosity *= 0.8
                
        # Store in history
        self.motivation_history.append(curiosity)
        
        return min(1.0, curiosity)
        
    def get_motivation_statistics(self) -> Dict[str, Any]:
        """Get motivation statistics."""
        if not self.motivation_history:
            return {"total_calculations": 0}
            
        return {
            "total_calculations": len(self.motivation_history),
            "average_motivation": np.mean(self.motivation_history),
            "max_motivation": np.max(self.motivation_history),
            "min_motivation": np.min(self.motivation_history),
            "motivation_above_threshold": sum(1 for m in self.motivation_history if m > self.exploration_rate)
        }


# Re-export commonly used classes and functions
__all__ = [
    "LearningRule", "PlasticityType",
    "LearningScheduler", "PlasticityController",
    "AdaptiveLearning", "CuriosityDrivenLearning"
]