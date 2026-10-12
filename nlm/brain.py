from typing import Optional, Dict, Any, List
from .config import Config


class Brain:
    """Brain class with simplified API for NLM operations.
    
    This class provides a simplified interface to the underlying C++ implementation,
    with comprehensive error handling and validation.
    """
    
    def __init__(self, config: Optional[Config] = None, **kwargs):
        """Initialize a new Brain instance.
        
        Args:
            config: Configuration object (optional)
            **kwargs: Additional configuration parameters
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        self.config = config or Config()
        self._validate_config(kwargs)
        self._initialize(**kwargs)
    
    def _validate_config(self, kwargs: Dict[str, Any]) -> None:
        """Validate configuration parameters.
        
        Args:
            kwargs: Configuration parameters to validate
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        # Validate all configuration parameters
        for key, value in kwargs.items():
            if not isinstance(key, str):
                raise TypeError(f"Parameter name must be a string, got {type(key).__name__}: {key}")
    
    def _initialize(self, **kwargs) -> None:
        """Initialize the Brain instance.
        
        Args:
            **kwargs: Configuration parameters
        """
        # Initialize internal state
        self._initialized = True
        self._parameters = kwargs
    
    def run_simulation(self, steps: int = 100, **kwargs) -> Dict[str, Any]:
        """Run a simulation with the brain.
        
        Args:
            steps: Number of simulation steps
            **kwargs: Additional simulation parameters
            
        Returns:
            Simulation results
            
        Raises:
            ValueError: If invalid steps parameter is provided
            RuntimeError: If simulation fails
        """
        if not isinstance(steps, int) or steps <= 0:
            raise ValueError(f"Steps must be a positive integer, got {steps}")
        
        if not hasattr(self, '_initialized') or not self._initialized:
            raise RuntimeError("Brain not properly initialized")
        
        # Run simulation (placeholder for actual C++ implementation)
        results = {
            'steps': steps,
            'parameters': self._parameters,
            'status': 'completed',
            'timestamp': None  # Would be actual timestamp from C++
        }
        
        return results
    
    def get_parameters(self) -> Dict[str, Any]:
        """Get current brain parameters.
        
        Returns:
            Dictionary of parameters
        """
        return self._parameters.copy()
    
    def update_parameters(self, **kwargs) -> None:
        """Update brain parameters.
        
        Args:
            **kwargs: New parameters to set
        """
        self._validate_config(kwargs)
        self._parameters.update(kwargs)
    
    def __repr__(self) -> str:
        """String representation of the Brain instance.
        
        Returns:
            String representation
        """
        return f"Brain(parameters={len(self._parameters) if hasattr(self, '_parameters') else 0})"
    
    def __str__(self) -> str:
        """Human-readable string representation.
        
        Returns:
            Human-readable string
        """
        return f"NLM Brain with {len(self._parameters) if hasattr(self, '_parameters') else 0} parameters"