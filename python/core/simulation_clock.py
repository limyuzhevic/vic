"""
Simulation clock for NLM

This module provides time and step management for the NLM simulation.
"""

import time

class SimulationClock:
    def __init__(self, timestep: float = 0.001):
        if not isinstance(timestep, (int, float)) or timestep <= 0:
            timestep = 0.001
        self._timestep = timestep
        self._current_step = 0
        self._current_time = 0.0
        import time
        self._start_time = time.time()

    def getTimestep(self) -> float:
        return self._timestep

    def getCurrentStep(self) -> int:
        return self._current_step

    def getCurrentTime(self) -> float:
        return self._current_time

    def getElapsedTime(self) -> float:
        import time
        return time.time() - self._start_time

    def step(self):
        self._current_step += 1
        self._current_time += self._timestep

    def getAbsoluteTime(self) -> float:
        import time
        return time.time() - self._start_time

    def syncToStep(self, step: int):
        if isinstance(step, int) and step >= 0:
            self._current_step = step
            self._current_time = step * self._timestep

    def __repr__(self):
        return f"SimulationClock(step={self._current_step}, time={self._current_time}, timestep={self._timestep})"

# Convenience function

def createSimulationClock(timestep: float = 0.001) -> SimulationClock:
    return SimulationClock(timestep)

__all__ = [
    'SimulationClock',
    'createSimulationClock',
]