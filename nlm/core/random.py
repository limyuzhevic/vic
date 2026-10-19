"""
Random number generation utilities for NLM.
"""

from typing import Tuple
import random
import numpy as np

class RandomGenerator:
    """
    Random number generator for NLM neural simulation.
    
    Provides various random number generation methods for initializing
    neural parameters and stochastic processes.
    """
    
    def __init__(self, seed: Optional[int] = None):
        """
        Initialize random generator.
        
        Args:
            seed: Random seed for reproducibility
        """
        if seed is not None:
            random.seed(seed)
            np.random.seed(seed)
        self._seed = seed
    
    def uniform_real(self, min_val: float, max_val: float) -> float:
        """
        Generate random float in range [min_val, max_val).
        
        Args:
            min_val: Minimum value (inclusive)
            max_val: Maximum value (exclusive)
            
        Returns:
            Random float
        """
        return random.uniform(min_val, max_val)
    
    def uniform_int(self, min_val: int, max_val: int) -> int:
        """
        Generate random integer in range [min_val, max_val].
        
        Args:
            min_val: Minimum value (inclusive)
            max_val: Maximum value (inclusive)
            
        Returns:
            Random integer
        """
        return random.randint(min_val, max_val)
    
    def normal(self, mean: float, std: float) -> float:
        """
        Generate normally distributed random float.
        
        Args:
            mean: Mean of the distribution
            std: Standard deviation
            
        Returns:
            Random float
        """
        return random.gauss(mean, std)
    
    def sample_from_distribution(self, weights: List[float]) -> int:
        """
        Sample from a discrete distribution.
        
        Args:
            weights: List of probabilities
            
        Returns:
            Index of selected category
        """
        total = sum(weights)
        if total <= 0:
            raise ValueError("Sum of weights must be positive")
        
        normalized_weights = [w / total for w in weights]
        rand = random.random()
        cumulative = 0.0
        for i, weight in enumerate(normalized_weights):
            cumulative += weight
            if rand <= cumulative:
                return i
        return len(weights) - 1
    
    def shuffle_list(self, lst: List) -> List:
        """
        Shuffle a list in-place.
        
        Args:
            lst: List to shuffle
            
        Returns:
            Shuffled list
        """
        random.shuffle(lst)
        return lst
    
    def get_seed(self) -> Optional[int]:
        """
        Get the random seed.
        
        Returns:
            Random seed or None
        """
        return self._seed

    @staticmethod
    def create_default() -> 'RandomGenerator':
        """
        Create a default random generator with fixed seed.
        
        Returns:
            Default random generator
        """
        return RandomGenerator(42)

__all__ = ['RandomGenerator']
