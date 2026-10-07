# World sensory implementation

from typing import Dict, Any

class SensoryPercept:
    """Sensory perception from the world."""
    
    def __init__(self, percept_data: Dict[str, Any] = None):
        if percept_data is None:
            percept_data = {
                'vision': [],
                'internal': [],
                'sound': []
            }
        self._percept = percept_data
    
    @property
    def vision(self) -> list:
        """Get visual input."""
        return self._percept.get('vision', [])
    
    @property
    def internal(self) -> list:
        """Get internal signals."""
        return self._percept.get('internal', [])
    
    @property
    def sound(self) -> list:
        """Get audio input."""
        return self._percept.get('sound', [])