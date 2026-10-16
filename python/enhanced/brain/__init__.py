"""Enhanced brain management for NLM.

This module provides brain management tools for the NLM (Neural Learning Machine)
framework with enhanced interfaces, analysis capabilities, and training support.
It builds upon the original Brain class to provide more Pythonic and robust
brain management features.

Key features:
- EnhancedBrain with state history recording
- BrainManager for batch brain creation and management
- Automatic analysis capabilities
- Pythonic iteration interface
- Training context managers
- Comprehensive statistics and analysis

Usage examples:

    from pynlm.enhanced.brain import EnhancedBrain, BrainManager

    # Create brain with name
    brain = EnhancedBrain(name="MyBrain")
    brain.initialize()
    
    # Use iterator interface
    for brain_state in brain:
        brain.step(brain_state.step)
    
    # Use training context manager
    with brain.training_context(steps=100) as session:
        session.run(100)
    
    # Analyze results
    analysis = brain.analyze()
    print(f"Stability: {analysis.get_stability():.2f}")

The brain management module includes:

1. EnhancedBrain class - Extended brain with enhanced features
2. BrainManager class - Manager for creating and managing enhanced brains
3. BrainAnalysis class - Comprehensive brain activity analysis
4. Training context managers for structured training
5. State history recording and statistics
"""

from typing import Dict, Any, Optional, List, Iterator
from contextlib import contextmanager
from .. import Brain as OriginalBrain

# Import from enhanced_api
from ..enhanced_api import (
    EnhancedBrain as EnhancedBrainClass,
    BrainAnalysis as BrainAnalysisClass,
    ValidationError,
    NeuralNetworkError,
    ErrorHandler
)

# Re-export classes
EnhancedBrain = EnhancedBrainClass
BrainAnalysis = BrainAnalysisClass

class BrainManager:
    """Manager for creating and managing enhanced brains.
    
    The BrainManager provides tools for batch creation, naming, and management
    of enhanced brains. It helps organize multiple brain instances and provides
    easy lookup capabilities.
    
    Example:
        manager = BrainManager()
        brain1 = manager.create_brain("Brain1")
        brain2 = manager.create_brain("Brain2")
        
        brains = manager.list_brains()
        brain = manager.get_brain("Brain1")
    """
    
    def __init__(self, config: Optional[OriginalBrain] = None):
        """Initialize brain manager.
        
        Args:
            config: Optional configuration for brains
        """
        self.config = config
        self.brains: Dict[str, EnhancedBrain] = {}
    
    def create_brain(self, name: Optional[str] = None) -> EnhancedBrain:
        """Create a new enhanced brain.
        
        Args:
            name: Optional name for the brain
            
        Returns:
            Enhanced brain instance
            
        Raises:
            ValueError: If brain name already exists
        """
        if name and name in self.brains:
            raise ValueError(f"Brain '{name}' already exists")
        
        brain = EnhancedBrain(self.config, name)
        if name:
            self.brains[name] = brain
        
        return brain
    
    def list_brains(self) -> List[str]:
        """Get list of brain names.
        
        Args:
            
        Returns:
            List of brain names
        """
        return list(self.brains.keys())
    
    def get_brain(self, name: str) -> EnhancedBrain:
        """Get brain by name.
        
        Args:
            name: Brain name
            
        Returns:
            Enhanced brain instance
            
        Raises:
            NeuralNetworkError: If brain not found
        """
        if name not in self.brains:
            raise NeuralNetworkError(f"Brain '{name}' not found")
        
        return self.brains[name]
    
    def remove_brain(self, name: str) -> None:
        """Remove brain by name.
        
        Args:
            name: Brain name
            
        Raises:
            NeuralNetworkError: If brain not found
        """
        if name not in self.brains:
            raise NeuralNetworkError(f"Brain '{name}' not found")
        
        del self.brains[name]
    
    def clear(self) -> None:
        """Clear all brains from manager."""
        self.brains.clear()
    
    def get_brain_count(self) -> int:
        """Get number of brains in manager.
        
        Args:
            
        Returns:
            Number of brains
        """
        return len(self.brains)
    
    def __repr__(self) -> str:
        """String representation of brain manager.
        
        Args:
            
        Returns:
            String representation
        """
        return f"<BrainManager with {self.get_brain_count()} brains>"

# Factory function for creating enhanced brains
def create_enhanced_brain(config: Optional[OriginalBrain] = None, name: Optional[str] = None) -> EnhancedBrain:
    """Create an enhanced brain.
    
    Args:
        config: Optional configuration
        name: Optional brain name
        
    Returns:
        Enhanced brain instance
    """
    return EnhancedBrain(config, name)

# Factory function for creating brain managers
def create_brain_manager(config: Optional[OriginalBrain] = None) -> BrainManager:
    """Create a brain manager.
    
    Args:
        config: Optional configuration
        
    Returns:
        Brain manager instance
    """
    return BrainManager(config)

# Training context manager
def training_context(brain: EnhancedBrain, steps: int, enable_learning: bool = True):
    """Context manager for training sessions.
    
    Args:
        brain: Brain to train
        steps: Number of training steps
        enable_learning: Whether to enable learning
        
    Returns:
        Training session context manager
    """
    return brain.training_context(steps, enable_learning)

# Analysis factory function
def create_analysis(brain: EnhancedBrain) -> BrainAnalysis:
    """Create analysis for brain.
    
    Args:
        brain: Brain to analyze
        
    Returns:
        Brain analysis instance
    """
    return brain.analyze()

# Export all public classes and functions
__all__ = [
    'EnhancedBrain',
    'BrainAnalysis',
    'BrainManager',
    'create_enhanced_brain',
    'create_brain_manager',
    'training_context',
    'create_analysis',
]