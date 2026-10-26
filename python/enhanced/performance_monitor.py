"""
Performance Monitor for NLM

This module provides comprehensive performance monitoring and metrics
collection for NLM systems.
"""

import sys
from pathlib import Path

# Add parent directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

class PerformanceMonitor:
    """
    Performance monitoring and metrics collection.
    
    This class provides real-time performance monitoring including:
    - Metrics collection
    - Historical tracking
    - Statistical analysis
    - Performance reporting
    """
    
    def __init__(self):
        """Initialize performance monitor"""
        try:
            from pynlm_enhanced import PerformanceMonitor as EnhancedMonitor
            self._monitor = EnhancedMonitor()
        except ImportError:
            raise ImportError("Enhanced NLM bindings not available")
    
    def start_recording(self):
        """Start recording performance metrics"""
        self._monitor.startRecording()
    
    def stop_recording(self):
        """Stop recording performance metrics"""
        self._monitor.stopRecording()
    
    def record_step(self, firing_rate, reward, novelty=0.0, curiosity=0.0):
        """
        Record metrics for a simulation step.
        
        Args:
            firing_rate: Average firing rate of the brain
            reward: Reward received
            novelty: Novelty level (default: 0.0)
            curiosity: Curiosity level (default: 0.0)
        """
        self._monitor.recordStep(firing_rate, reward, novelty, curiosity)
    
    def calculate_average_firing_rate(self):
        """
        Calculate average firing rate from recorded metrics.
        
        Returns:
            Average firing rate
        """
        return self._monitor.calculateAverageFiringRate()
    
    def calculate_average_reward(self):
        """
        Calculate average reward from recorded metrics.
        
        Returns:
            Average reward
        """
        return self._monitor.calculateAverageReward()
    
    def generate_report(self):
        """
        Generate a performance report.
        
        Returns:
            Formatted performance report string
        """
        return self._monitor.generateReport()