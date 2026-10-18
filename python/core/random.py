"""
Random number generation for NLM

This module provides random number generation functionality for the NLM system.
"""

import random
import time

class RandomGenerator:
    def __init__(self, seed: int = None):
        if seed is None:
            import time
            seed = int(time.time() * 1000) % 2**31
        import random
        self._random = random.Random(seed)

    def getSeed(self) -> int:
        return self._random.getstate()[1][0]

    def uniform(self, min_val: float = 0.0, max_val: float = 1.0) -> float:
        if not isinstance(min_val, (int, float)) or not isinstance(max_val, (int, float)):
            return 0.0
        return self._random.uniform(min_val, max_val)

    def normal(self, mean: float = 0.0, stddev: float = 1.0) -> float:
        if not isinstance(mean, (int, float)) or not isinstance(stddev, (int, float)):
            return 0.0
        return self._random.gauss(mean, stddev)

    def exponential(self, lambd: float = 1.0) -> float:
        if not isinstance(lambd, (int, float)) or lambd <= 0:
            return 0.0
        return self._random.expovariate(lambd)

    def integer(self, min_val: int = 0, max_val: int = 100) -> int:
        if not isinstance(min_val, int) or not isinstance(max_val, int):
            return 0
        if min_val > max_val:
            min_val, max_val = max_val, min_val
        return self._random.randint(min_val, max_val)

    def shuffle(self, values: list) -> list:
        if not isinstance(values, list):
            return []
        shuffled = values.copy()
        self._random.shuffle(shuffled)
        return shuffled

    def sample(self, population: list, k: int) -> list:
        if not isinstance(population, list) or not isinstance(k, int):
            return []
        if k < 0 or k > len(population):
            return []
        return self._random.sample(population, k)

    def choice(self, population: list):
        if not isinstance(population, list) or not population:
            return None
        return self._random.choice(population)

    def random(self) -> float:
        return self._random.random()

    def gauss(self, mu: float = 0.0, sigma: float = 1.0) -> float:
        if not isinstance(mu, (int, float)) or not isinstance(sigma, (int, float)):
            return 0.0
        return self._random.gauss(mu, sigma)

    def __repr__(self):
        return f"RandomGenerator(seed={self.getSeed()})"

# Create default random generator
_default_random = RandomGenerator()

def get_default_random():
    return _default_random

# Convenience function
def createRandomGenerator(seed: int = None) -> RandomGenerator:
    return RandomGenerator(seed)

__all__ = [
    'RandomGenerator',
    'createRandomGenerator',
    'get_default_random',
]