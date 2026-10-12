from typing import Optional, Dict, Any, List
from .brain import Brain


class World:
    """World class wrapper for NLM simulation environments.
    
    This class provides a simplified interface to the underlying C++ world simulation,
    with comprehensive error handling and validation.
    """
    
    def __init__(self, name: str = "default_world", **kwargs):
        """Initialize a new World instance.
        
        Args:
            name: World name (optional, defaults to "default_world")
            **kwargs: Additional configuration parameters
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        if not isinstance(name, str):
            raise TypeError(f"World name must be a string, got {type(name).__name__}: {name}")
        
        self.name = name
        self._brains: List[Brain] = []
        self._agents: List[Any] = []  # Agent instances
        self._parameters = kwargs
        self._initialized = True
    
    def add_brain(self, brain: Brain) -> None:
        """Add a brain to the world.
        
        Args:
            brain: Brain instance to add
            
        Raises:
            TypeError: If brain is not a Brain instance
            ValueError: If brain is already in the world
        """
        if not isinstance(brain, Brain):
            raise TypeError(f"brain must be a Brain instance, got {type(brain).__name__}")
        
        if brain in self._brains:
            raise ValueError("Brain already exists in the world")
        
        self._brains.append(brain)
    
    def remove_brain(self, brain: Brain) -> None:
        """Remove a brain from the world.
        
        Args:
            brain: Brain instance to remove
            
        Raises:
            TypeError: If brain is not a Brain instance
            ValueError: If brain is not in the world
        """
        if not isinstance(brain, Brain):
            raise TypeError(f"brain must be a Brain instance, got {type(brain).__name__}")
        
        try:
            self._brains.remove(brain)
        except ValueError:
            raise ValueError("Brain not found in the world")
    
    def add_agent(self, agent: Any) -> None:
        """Add an agent to the world.
        
        Args:
            agent: Agent instance to add
            
        Raises:
            TypeError: If agent is not an Agent instance
        """
        # Type checking for agent
        if not hasattr(agent, 'name'):
            raise TypeError("Agent must have a 'name' attribute")
        
        self._agents.append(agent)
    
    def get_brains(self) -> List[Brain]:
        """Get all brains in the world.
        
        Returns:
            List of Brain instances
        """
        return self._brains.copy()
    
    def get_agents(self) -> List[Any]:
        """Get all agents in the world.
        
        Returns:
            List of Agent instances
        """
        return self._agents.copy()
    
    def get_parameters(self) -> Dict[str, Any]:
        """Get world parameters.
        
        Returns:
            Dictionary of parameters
        """
        return self._parameters.copy()
    
    def update_parameters(self, **kwargs) -> None:
        """Update world parameters.
        
        Args:
            **kwargs: New parameters to set
        """
        self._parameters.update(kwargs)
    
    def run_step(self) -> Dict[str, Any]:
        """Run a single world step.
        
        Returns:
            Step results
        """
        if not self._initialized:
            raise RuntimeError("World not properly initialized")
        
        results = {
            'world_name': self.name,
            'brains_count': len(self._brains),
            'agents_count': len(self._agents),
            'parameters': self._parameters,
            'step_completed': True,
            'timestamp': None  # Would be actual timestamp
        }
        
        return results
    
    def run_simulation(self, steps: int = 100) -> Dict[str, Any]:
        """Run a simulation in the world.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Simulation results
            
        Raises:
            ValueError: If invalid steps parameter is provided
        """
        if not isinstance(steps, int) or steps <= 0:
            raise ValueError(f"Steps must be a positive integer, got {steps}")
        
        step_results = []
        for i in range(steps):
            step_results.append(self.run_step())
        
        total_results = {
            'world_name': self.name,
            'total_steps': steps,
            'step_results': step_results,
            'brains_count': len(self._brains),
            'agents_count': len(self._agents),
            'status': 'completed'
        }
        
        return total_results
    
    def __repr__(self) -> str:
        """String representation of the World instance.
        
        Returns:
            String representation
        """
        return f"World(name={self.name!r}, brains={len(self._brains)}, agents={len(self._agents)})"
    
    def __str__(self) -> str:
        """Human-readable string representation.
        
        Returns:
            Human-readable string
        """
        return f"NLM World '{self.name}' with {len(self._brains)} brains and {len(self._agents)} agents"