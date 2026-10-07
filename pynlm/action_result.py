# Action result implementation for NLM Python API

"""
Action result for NLM Python API

Represents the result from applying a motor command to the world,
containing reward, success status, and message about the action execution.
"""

from typing import Any, Optional
from dataclasses import dataclass, field

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class ActionResultError(NLMError):
    """Raised when action result operations fail"""
    pass

class ActionResult:
    """
    Result from applying a motor command to the world
    
    Contains reward, success status, and message about the action execution.
    
    Attributes:
        reward: Reward value from action (higher is better)
        success: Whether action was successful
        message: Description of action result
        confidence: Confidence level of result (0.0 to 1.0)
        metadata: Additional metadata about the result
    """
    
    def __init__(self):
        """
        Initialize action result.
        """
        # Note: In actual implementation, this would wrap C++ ActionResult object
        self.reward = 0.0
        self.success = False
        self.message = ""
        self.confidence = 0.0
        self.metadata = {}
        self._initialized = True
    
    def set_reward(self, reward: float) -> None:
        """
        Set reward value.
        
        Args:
            reward: Reward value
            
        Raises:
            ActionResultError: If reward cannot be set
        """
        self.reward = float(reward)
    
    def set_success(self, success: bool) -> None:
        """
        Set success status.
        
        Args:
            success: Success status
            
        Raises:
            ActionResultError: If success status cannot be set
        """
        self.success = bool(success)
    
    def set_message(self, message: str) -> None:
        """
        Set result message.
        
        Args:
            message: Result message
            
        Raises:
            ActionResultError: If message cannot be set
        """
        self.message = str(message)
    
    def set_confidence(self, confidence: float) -> None:
        """
        Set confidence level.
        
        Args:
            confidence: Confidence level (0.0 to 1.0)
            
        Raises:
            ActionResultError: If confidence cannot be set
        """
        self.confidence = max(0.0, min(1.0, float(confidence)))
    
    def set_metadata(self, key: str, value: Any) -> None:
        """
        Set metadata value.
        
        Args:
            key: Metadata key
            value: Metadata value
            
        Raises:
            ActionResultError: If metadata cannot be set
        """
        self.metadata[key] = value
    
    def get_metadata(self, key: str, default: Any = None) -> Any:
        """
        Get metadata value.
        
        Args:
            key: Metadata key
            default: Default value if key doesn't exist
            
        Returns:
            Metadata value or default
        """
        return self.metadata.get(key, default)
    
    def has_metadata(self, key: str) -> bool:
        """
        Check if metadata has key.
        
        Args:
            key: Metadata key
            
        Returns:
            True if metadata has key
        """
        return key in self.metadata
    
    def get_total_score(self) -> float:
        """
        Get total score (reward weighted by success and confidence).
        
        Returns:
            Total score
            
        Examples:
            >>> result = ActionResult()
            >>> result.set_reward(10.0)
            >>> result.setSuccess(True)
            >>> result.setConfidence(0.9)
            >>> score = result.getTotalScore()
            >>> print(f"Score: {score}")  # Should be 9.0
        """
        return self.reward * (1.0 if self.success else 0.0) * self.confidence
    
    def is_good(self, min_reward: float = 0.0, min_confidence: float = 0.5) -> bool:
        """
        Check if result is good based on criteria.
        
        Args:
            min_reward: Minimum reward for "good" result
            min_confidence: Minimum confidence for "good" result
            
        Returns:
            True if result is good
            
        Examples:
            >>> result = ActionResult()
            >>> result.setReward(5.0)
            >>> result.setSuccess(True)
            >>> result.setConfidence(0.8)
            >>> if result.isGood(minReward=3.0, minConfidence=0.7):
            ...     print("Result is good!")
        """
        return (self.reward >= min_reward and 
                self.success and 
                self.confidence >= min_confidence)
    
    def is_bad(self, max_penalty: float = -10.0) -> bool:
        """
        Check if result is bad (penalizing negative rewards).
        
        Args:
            max_penalty: Maximum penalty for "bad" result
            
        Returns:
            True if result is bad
            
        Examples:
            >>> result = ActionResult()
            >>> result.setReward(-5.0)
            >>> result.setSuccess(False)
            >>> if result.isBad(maxPenalty=-1.0):
            ...     print("Result is bad!")
        """
        return self.reward <= max_penalty or not self.success
    
    def to_dict(self) -> dict:
        """
        Convert result to dictionary.
        
        Returns:
            Dictionary representation
            
        Examples:
            >>> result = ActionResult()
            >>> result.setReward(10.0)
            >>> result.setSuccess(True)
            >>> result.setMessage("Success!")
            >>> data = result.toDict()
        """
        return {
            'reward': self.reward,
            'success': self.success,
            'message': self.message,
            'confidence': self.confidence,
            'metadata': self.metadata.copy(),
            'total_score': self.get_total_score()
        }
    
    def __repr__(self) -> str:
        """String representation of action result"""
        status = "SUCCESS" if self.success else "FAILURE"
        return f"<ActionResult {status} reward={self.reward:.2f}, confidence={self.confidence:.2f}, message='{self.message}'>"
    
    def __str__(self) -> str:
        """String representation of action result"""
        return self.__repr__()

# Factory function for action result creation
def create_action_result() -> ActionResult:
    """
    Create an action result.
    
    Returns:
        New ActionResult instance
        
    Examples:
        >>> result = createActionResult()
    """
    return ActionResult()