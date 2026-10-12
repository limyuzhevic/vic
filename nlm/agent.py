from typing import Optional, Dict, Any
from .brain import Brain


class Agent:
    """Agent wrapper for NLM with simplified API.
    
    This class provides a simplified interface to the underlying C++ implementation,
    with comprehensive error handling and validation.
    """
    
    def __init__(self, name: str, brain: Optional[Brain] = None, **kwargs):
        """Initialize a new Agent instance.
        
        Args:
            name: Agent name
            brain: Brain instance (optional)
            **kwargs: Additional configuration parameters
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        if not isinstance(name, str):
            raise TypeError(f"Agent name must be a string, got {type(name).__name__}: {name}")
        
        self.name = name
        self.brain = brain
        self._parameters = kwargs
        self._initialized = True
    
    def set_brain(self, brain: Brain) -> None:
        """Set the agent's brain.
        
        Args:
            brain: Brain instance to set
            
        Raises:
            TypeError: If brain is not a Brain instance
        """
        if not isinstance(brain, Brain):
            raise TypeError(f"brain must be a Brain instance, got {type(brain).__name__}")
        
        self.brain = brain
    
    def get_parameters(self) -> Dict[str, Any]:
        """Get agent parameters.
        
        Returns:
            Dictionary of parameters
        """
        return self._parameters.copy()
    
    def update_parameters(self, **kwargs) -> None:
        """Update agent parameters.
        
        Args:
            **kwargs: New parameters to set
        """
        self._parameters.update(kwargs)
    
    def think(self, observation: Any = None) -> Dict[str, Any]:
        """Make a decision based on observation.
        
        Args:
            observation: Environmental observation (optional)
            
        Returns:
            Decision dictionary
            
        Raises:
            RuntimeError: If agent has no brain set
        """
        if self.brain is None:
            raise RuntimeError("Agent has no brain set")
        
        # Make decision (placeholder for actual implementation)
        decision = {
            'agent_name': self.name,
            'action': 'think',
            'observation': observation,
            'brain_parameters': self.brain.get_parameters(),
            'timestamp': None  # Would be actual timestamp
        }
        
        return decision
    
    def act(self, action: str, parameters: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Perform an action.
        
        Args:
            action: Action to perform
            parameters: Action parameters (optional)
            
        Returns:
            Action results
            
        Raises:
            RuntimeError: If agent has no brain set
        """
        if self.brain is None:
            raise RuntimeError("Agent has no brain set")
        
        # Perform action (placeholder for actual implementation)
        results = {
            'agent_name': self.name,
            'action': action,
            'parameters': parameters or {},
            'brain_parameters': self.brain.get_parameters(),
            'status': 'completed',
            'timestamp': None  # Would be actual timestamp
        }
        
        return results
    
    def run_simulation(self, steps: int = 100) -> Dict[str, Any]:
        """Run a simulation with the agent.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Simulation results
        """
        if not isinstance(steps, int) or steps <= 0:
            raise ValueError(f"Steps must be a positive integer, got {steps}")
        
        step_results = []
        for i in range(steps):
            step_results.append(self.think())
        
        total_results = {
            'agent_name': self.name,
            'total_steps': steps,
            'step_results': step_results,
            'brain_parameters': self.brain.get_parameters() if self.brain else None,
            'status': 'completed'
        }
        
        return total_results
    
    def __repr__(self) -> str:
        """String representation of the Agent instance.
        
        Returns:
            String representation
        """
        brain_info = f"brain={type(self.brain).__name__}" if self.brain else "brain=None"
        return f"Agent(name={self.name!r}, {brain_info})"
    
    def __str__(self) -> str:
        """Human-readable string representation.
        
        Returns:
            Human-readable string
        """
        return f"NLM Agent '{self.name}'" + (f" with brain" if self.brain else " without brain")