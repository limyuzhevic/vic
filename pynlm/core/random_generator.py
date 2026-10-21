"""Random number generator for NLM simulation."""

import random
from typing import Optional, Any


class RandomGenerator:
    """Random number generator for neural simulations."""
    
    def __init__(self, seed: Optional[int] = None):
        """
        Initialize random generator.
        
        Args:
            seed: Random seed (None for system time)
        """
        if seed is not None:
            random.seed(seed)
        
    def random(self) -> float:
        """Generate random float between 0 and 1."""
        return random.random()
        
    def uniform(self, a: float, b: float) -> float:
        """Generate random uniform number between a and b."""
        return random.uniform(a, b)
        
    def normal(self, mean: float, std: float) -> float:
        """Generate random normal distribution number."""
        return random.normalvariate(mean, std)
        
    def randint(self, a: int, b: int) -> int:
        """Generate random integer between a and b (inclusive)."""
        return random.randint(a, b)
        
    def choice(self, seq) -> Any:
        """Randomly choose from sequence."""
        return random.choice(seq)