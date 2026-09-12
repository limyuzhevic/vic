from .visualization import Visualization
from .profiling import PerformanceProfiler

# Convenience factory functions
def create_advanced_visualization(brain=None):
    """Create an advanced visualization module."""
    return Visualization(brain)

def create_performance_profiler(brain=None):
    """Create a performance profiler."""
    return PerformanceProfiler(brain)

__all__ = [
    'Visualization',
    'PerformanceProfiler',
    'create_advanced_visualization',
    'create_performance_profiler',
]