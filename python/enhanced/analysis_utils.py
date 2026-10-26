"""
Analysis Utilities for NLM

This module provides analysis and utility functions for NLM systems,
including neural activity analysis and learning progress tracking.
"""

import sys
from pathlib import Path

# Add parent directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

class AnalysisUtils:
    """
    Analysis utilities for NLM systems.
    
    This class provides analysis functions for:
    - Neural activity patterns
    - Learning progress tracking
    - Statistical computations
    - Performance analysis
    """
    
    def analyze_firing_pattern(self, brain, steps):
        """
        Analyze neural firing patterns over time.
        
        Args:
            brain: Brain object to analyze
            steps: Number of steps to simulate and analyze
            
        Returns:
            List of firing rates
        """
        try:
            from pynlm_enhanced import AnalysisUtils as EnhancedAnalysis
            return EnhancedAnalysis.analyzeFiringPattern(brain, steps)
        except ImportError:
            # Fallback implementation
            firing_rates = []
            for step in range(steps):
                brain.step(step)
                firing_rates.append(brain.getAverageFiringRate())
            return firing_rates
    
    def calculate_firing_rate_trend(self, firing_rates):
        """
        Calculate trend of firing rates over time.
        
        Args:
            firing_rates: List of firing rates
            
        Returns:
            Trend slope (positive = increasing, negative = decreasing)
        """
        try:
            from pynlm_enhanced import AnalysisUtils as EnhancedAnalysis
            return EnhancedAnalysis.calculateFiringRateTrend(firing_rates)
        except ImportError:
            # Simple linear regression fallback
            if len(firing_rates) < 2:
                return 0.0
            
            n = len(firing_rates)
            sum_x = sum_y = sum_xy = sum_x2 = 0.0
            
            for i, y in enumerate(firing_rates):
                x = float(i)
                sum_x += x
                sum_y += y
                sum_xy += x * y
                sum_x2 += x * x
            
            slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
            return slope
    
    def analyze_learning_progress(self, rewards, firing_rates, novelty_levels):
        """
        Analyze learning progress across multiple metrics.
        
        Args:
            rewards: List of reward values
            firing_rates: List of firing rates
            novelty_levels: List of novelty levels
            
        Returns:
            Dictionary with analysis results
        """
        try:
            from pynlm_enhanced import AnalysisUtils as EnhancedAnalysis
            return EnhancedAnalysis.analyzeLearningProgress(rewards, firing_rates, novelty_levels)
        except ImportError:
            # Basic analysis fallback
            analysis = {}
            
            if rewards:
                analysis['reward_trend'] = self._calculate_trend(rewards)
            
            if firing_rates:
                analysis['firing_trend'] = self._calculate_trend(firing_rates)
            
            if rewards and firing_rates and len(rewards) == len(firing_rates):
                analysis['reward_firing_coherence'] = self._calculate_coherence(rewards, firing_rates)
            
            return analysis
    
    def calculate_mean(self, values):
        """
        Calculate mean of a list of values.
        
        Args:
            values: List of numeric values
            
        Returns:
            Mean value
        """
        if not values:
            return 0.0
        return sum(values) / len(values)
    
    def calculate_std_dev(self, values, mean):
        """
        Calculate standard deviation of a list of values.
        
        Args:
            values: List of numeric values
            mean: Mean of the values
            
        Returns:
            Standard deviation
        """
        if len(values) <= 1:
            return 0.0
        
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def generate_analysis_report(self, analysis):
        """
        Generate a formatted analysis report.
        
        Args:
            analysis: Dictionary with analysis results
            
        Returns:
            Formatted report string
        """
        try:
            from pynlm_enhanced import AnalysisUtils as EnhancedAnalysis
            return EnhancedAnalysis.generateAnalysisReport(analysis)
        except ImportError:
            # Basic report generation
            lines = ["=== NLM Analysis Report ==="]
            for key, value in analysis.items():
                lines.append(f"{key}: {value}")
            return "\n".join(lines)
    
    def _calculate_trend(self, values):
        """Calculate simple trend of values"""
        if len(values) < 2:
            return 0.0
        
        # Calculate linear regression slope
        n = len(values)
        sum_x = sum_y = sum_xy = sum_x2 = 0.0
        
        for i, y in enumerate(values):
            x = float(i)
            sum_x += x
            sum_y += y
            sum_xy += x * y
            sum_x2 += x * x
        
        return (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
    
    def _calculate_coherence(self, values1, values2):
        """Calculate coherence between two lists of values"""
        if len(values1) != len(values2) or len(values1) < 2:
            return 0.0
        
        n = len(values1)
        sum_x = sum_y = sum_xy = sum_x2 = sum_y2 = 0.0
        
        for x, y in zip(values1, values2):
            sum_x += x
            sum_y += y
            sum_xy += x * y
            sum_x2 += x * x
            sum_y2 += y * y
        
        numerator = n * sum_xy - sum_x * sum_y
        denominator = ((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y)) ** 0.5
        
        return numerator / denominator if denominator > 0 else 0.0