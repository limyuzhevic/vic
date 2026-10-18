"""
Sensory module for NLM

This module provides sensory processing classes for NLM.
"""

import time
from typing import List
from .core import SensoryInput, NeuronType

class Vision(SensoryInput):
    def __init__(self, width: int = 8, height: int = 8, channels: int = 3):
        super().__init__()
        self._width = width
        self._height = height
        self._channels = channels
        self._data: List[float] = []
        self._timestamp = time.time()

    def setData(self, data: List[float]):
        self._data = data

    def getWidth(self) -> int:
        return self._width

    def getHeight(self) -> int:
        return self._height

    def getChannels(self) -> int:
        return self._channels

    def __repr__(self):
        return f"Vision(width={self._width}, height={self._height}, channels={self._channels})"

class Audio(SensoryInput):
    def __init__(self, sample_rate: int = 44100, num_samples: int = 1024):
        super().__init__()
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._data: List[float] = []
        self._timestamp = time.time()

    def setData(self, data: List[float]):
        self._data = data

    def setSampleRate(self, sample_rate: int):
        self._sample_rate = sample_rate

    def getSampleRate(self) -> int:
        return self._sample_rate

    def getNumSamples(self) -> int:
        return self._num_samples

    def __repr__(self):
        return f"Audio(sample_rate={self._sample_rate}, samples={self._num_samples})"

class InternalSignals(SensoryInput):
    def __init__(self):
        super().__init__()
        self._signals: List[float] = []
        self._timestamp = time.time()

    def addSignal(self, value: float):
        self._signals.append(value)

    def clearSignals(self):
        self._signals.clear()

    def __repr__(self):
        return f"InternalSignals(signals={len(self._signals)})"

# Convenience functions

def createVision(width: int = 8, height: int = 8, channels: int = 3) -> Vision:
    return Vision(width, height, channels)

__all__ = [
    'Vision',
    'Audio',
    'InternalSignals',
    'createVision',
]