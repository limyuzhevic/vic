"""
ErrorTracker class for error tracking and debugging.

Provides comprehensive error tracking, error analysis, and debugging capabilities
for neural simulations. Supports error categorization, error resolution tracking,
and detailed error analysis.
"""

import numpy as np
import traceback
import time
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum

class ErrorSeverity(Enum):
    """Error severity levels."""
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"

class ErrorCategory(Enum):
    """Error categories."""
    SIMULATION_ERROR = "simulation_error"
    CONFIGURATION_ERROR = "configuration_error"
    MEMORY_ERROR = "memory_error"
    PERFORMANCE_ERROR = "performance_error"
    NETWORK_ERROR = "network_error"
    PLUGIN_ERROR = "plugin_error"
    UNKNOWN_ERROR = "unknown_error"

@dataclass
class ErrorInfo:
    """Error information for tracking and analysis."""
    timestamp: float
    error_id: str
    severity: ErrorSeverity
    category: ErrorCategory
    message: str
    stack_trace: Optional[str] = None
    context: Dict[str, Any] = field(default_factory=dict)
    resolved: bool = False
    resolved_at: Optional[float] = None
    resolution_notes: Optional[str] = None
    affected_components: List[str] = field(default_factory=list)
    reproduction_steps: Optional[List[str]] = None
    related_errors: List[str] = field(default_factory=list)

@dataclass
class ErrorPattern:
    """Error pattern for anomaly detection."""
    pattern_type: str
    frequency: float
    severity_correlation: float
    component_correlation: Dict[str, float]
    time_pattern: Dict[str, Any]
    suggested_fix: Optional[str] = None
    prevention_strategy: Optional[str] = None

class ErrorTracker:
    """
    Advanced error tracking and debugging for neural simulations.
    
    Provides comprehensive error tracking, error analysis, and debugging capabilities
    for neural simulations. Supports error categorization, error resolution tracking,
    and detailed error analysis.
    """
    
    def __init__(self, brain):
        self.brain = brain
        self.error_log = []
        self.error_patterns = []
        self.error_summary = {
            'total_errors': 0,
            'critical_errors': 0,
            'high_errors': 0,
            'medium_errors': 0,
            'low_errors': 0,
            'resolved_errors': 0,
            'unresolved_errors': 0,
            'errors_by_category': {},
            'errors_by_component': {},
            'error_rate': 0.0,
            'recent_errors': 0
        }
        self.error_rules = []
        self.auto_error_categorization = True
        self.error_thresholds = {}
        self.error_callbacks = []
        self.error_suppression = False
        self.suppressed_errors = []
        
        print(f"[ERROR_TRACKER] Error tracker initialized for brain with {brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 'unknown'} neurons")
    
    def log_error(self, error: Exception, severity: ErrorSeverity = ErrorSeverity.MEDIUM, 
                  category: Optional[ErrorCategory] = None, context: Optional[Dict] = None,
                  affected_components: Optional[List[str]] = None):
        """
        Log an error that occurred during simulation.
        
        Args:
            error: The exception that occurred
            severity: Error severity level
            category: Error category (auto-detected if not provided)
            context: Additional context information
            affected_components: List of affected components
        """
        error_id = f"error_{len(self.error_log)}_{int(time.time())}"
        
        # Auto-categorize error if not provided
        if category is None:
            category = self._categorize_error(error)
        
        # Create error info
        error_info = ErrorInfo(
            timestamp=time.time(),
            error_id=error_id,
            severity=severity,
            category=category,
            message=str(error),
            stack_trace=traceback.format_exc(),
            context=context or {},
            affected_components=affected_components or [],
            reproduction_steps=self._extract_reproduction_steps(error)
        )
        
        self.error_log.append(error_info)
        
        # Update error summary
        self._update_error_summary(error_info)
        
        # Check error thresholds
        if self._check_error_threshold(error_info):
            self._handle_error_threshold(error_info)
        
        # Trigger error callbacks
        self._trigger_error_callbacks(error_info)
        
        # Log the error
        self._format_error_log(error_info)
    
    def resolve_error(self, error_id: str, resolution_notes: Optional[str] = None):
        """
        Mark an error as resolved.
        
        Args:
            error_id: ID of the error to resolve
            resolution_notes: Notes about how the error was resolved
        """
        for error in self.error_log:
            if error.error_id == error_id:
                error.resolved = True
                error.resolved_at = time.time()
                error.resolution_notes = resolution_notes
                
                # Update error summary
                self._update_error_summary(error)
                
                print(f"[ERROR_TRACKER] Error {error_id} resolved: {resolution_notes}")
                return True
        
        print(f"[ERROR_TRACKER] Error {error_id} not found")
        return False
    
    def get_error(self, error_id: str) -> Optional[ErrorInfo]:
        """Get error by ID."""
        for error in self.error_log:
            if error.error_id == error_id:
                return error
        return None
    
    def get_errors(self, severity: Optional[ErrorSeverity] = None, 
                   category: Optional[ErrorCategory] = None, 
                   resolved: Optional[bool] = None,
                   limit: int = 100) -> List[ErrorInfo]:
        """
        Get errors with optional filters.
        
        Args:
            severity: Filter by error severity
            category: Filter by error category
            resolved: Filter by resolution status
            limit: Maximum number of errors to return
            
        Returns:
            List of error information objects
        """
        filtered_errors = self.error_log
        
        if severity is not None:
            filtered_errors = [e for e in filtered_errors if e.severity == severity]
        
        if category is not None:
            filtered_errors = [e for e in filtered_errors if e.category == category]
        
        if resolved is not None:
            filtered_errors = [e for e in filtered_errors if e.resolved == resolved]
        
        return filtered_errors[-limit:] if limit > 0 else filtered_errors
    
    def get_error_statistics(self) -> Dict[str, Any]:
        """Get comprehensive error statistics."""
        return self.error_summary.copy()
    
    def analyze_error_patterns(self) -> List[ErrorPattern]:
        """
        Analyze error patterns for trends and predictions.
        
        Returns:
            List of error patterns
        """
        patterns = []
        
        # Analyze by category
        for category in ErrorCategory:
            category_errors = [e for e in self.error_log if e.category == category]
            if category_errors:
                pattern = ErrorPattern(
                    pattern_type=f"category_{category.value}",
                    frequency=len(category_errors) / max(len(self.error_log), 1),
                    severity_correlation=self._calculate_severity_correlation(category_errors),
                    component_correlation=self._calculate_component_correlation(category_errors),
                    time_pattern=self._analyze_time_pattern(category_errors)
                )
                patterns.append(pattern)
        
        # Analyze by severity
        for severity in ErrorSeverity:
            severity_errors = [e for e in self.error_log if e.severity == severity]
            if severity_errors:
                pattern = ErrorPattern(
                    pattern_type=f"severity_{severity.value}",
                    frequency=len(severity_errors) / max(len(self.error_log), 1),
                    severity_correlation=1.0,
                    component_correlation=self._calculate_component_correlation(severity_errors),
                    time_pattern=self._analyze_time_pattern(severity_errors)
                )
                patterns.append(pattern)
        
        return patterns
    
    def add_error_rule(self, rule: Dict[str, Any]):
        """
        Add an error analysis rule.
        
        Args:
            rule: Error rule configuration
        """
        self.error_rules.append(rule)
    
    def suppress_errors(self, component: str, duration: float):
        """
        Temporarily suppress errors from a specific component.
        
        Args:
            component: Component name
            duration: Suppression duration in seconds
        """
        suppression_id = f"{component}_{int(time.time())}"
        self.suppressed_errors.append({
            'id': suppression_id,
            'component': component,
            'start_time': time.time(),
            'end_time': time.time() + duration,
            'count': 0
        })
        
        print(f"[ERROR_TRACKER] Error suppression enabled for {component} for {duration}s")
    
    def is_error_suppressed(self, component: str) -> bool:
        """Check if errors from a component are currently suppressed."""
        current_time = time.time()
        for suppression in self.suppressed_errors[:]:
            if suppression['end_time'] < current_time:
                self.suppressed_errors.remove(suppression)
            elif suppression['component'] == component:
                return True
        return False
    
    def clear_suppressed_errors(self):
        """Clear all error suppressions."""
        self.suppressed_errors.clear()
    
    def generate_error_report(self) -> Dict[str, Any]:
        """
        Generate comprehensive error report.
        
        Returns:
            Error report
        """
        report = {
            'summary': self.error_summary,
            'recent_errors': self.get_errors(limit=20),
            'error_patterns': self.analyze_error_patterns(),
            'error_trends': self._analyze_error_trends(),
            'recommendations': self._generate_error_recommendations()
        }
        
        return report
    
    def _categorize_error(self, error: Exception) -> ErrorCategory:
        """Auto-categorize error based on its characteristics."""
        error_msg = str(error).lower()
        error_type = type(error).__name__.lower()
        
        # Configuration-related errors
        if any(keyword in error_msg for keyword in ['config', 'configuration', 'parameter', 'setting']):
            return ErrorCategory.CONFIGURATION_ERROR
        
        # Memory-related errors
        if any(keyword in error_msg for keyword in ['memory', 'ram', 'storage', 'out of memory']):
            return ErrorCategory.MEMORY_ERROR
        
        # Performance-related errors
        if any(keyword in error_msg for keyword in ['performance', 'slow', 'timeout', 'rate limit']):
            return ErrorCategory.PERFORMANCE_ERROR
        
        # Network-related errors
        if any(keyword in error_msg for keyword in ['network', 'connection', 'socket', 'remote']):
            return ErrorCategory.NETWORK_ERROR
        
        # Plugin-related errors
        if any(keyword in error_msg for keyword in ['plugin', 'module', 'extension', 'addon']):
            return ErrorCategory.PLUGIN_ERROR
        
        # Default to simulation error
        return ErrorCategory.SIMULATION_ERROR
    
    def _extract_reproduction_steps(self, error: Exception) -> Optional[List[str]]:
        """Extract reproduction steps from error context."""
        # This would extract steps from error context if available
        return None
    
    def _update_error_summary(self, error: ErrorInfo):
        """Update error summary statistics."""
        self.error_summary['total_errors'] += 1
        
        # Count by severity
        if error.severity == ErrorSeverity.CRITICAL:
            self.error_summary['critical_errors'] += 1
        elif error.severity == ErrorSeverity.HIGH:
            self.error_summary['high_errors'] += 1
        elif error.severity == ErrorSeverity.MEDIUM:
            self.error_summary['medium_errors'] += 1
        elif error.severity == ErrorSeverity.LOW:
            self.error_summary['low_errors'] += 1
        
        # Count by resolution status
        if error.resolved:
            self.error_summary['resolved_errors'] += 1
        else:
            self.error_summary['unresolved_errors'] += 1
        
        # Count by category
        category = error.category.value
        if category not in self.error_summary['errors_by_category']:
            self.error_summary['errors_by_category'][category] = 0
        self.error_summary['errors_by_category'][category] += 1
        
        # Count by component
        for component in error.affected_components:
            if component not in self.error_summary['errors_by_component']:
                self.error_summary['errors_by_component'][component] = 0
            self.error_summary['errors_by_component'][component] += 1
        
        # Calculate error rate (errors per hour)
        if self.error_log:
            oldest_time = min(e.timestamp for e in self.error_log)
            current_time = time.time()
            hours_elapsed = max((current_time - oldest_time) / 3600, 1.0)
            self.error_summary['error_rate'] = len(self.error_log) / hours_elapsed
        
        # Count recent errors (last hour)
        recent_time = time.time() - 3600
        recent_count = sum(1 for e in self.error_log if e.timestamp > recent_time)
        self.error_summary['recent_errors'] = recent_count
    
    def _check_error_threshold(self, error: ErrorInfo) -> bool:
        """Check if error meets threshold criteria."""
        # Check category-specific thresholds
        category_thresholds = self.error_thresholds.get(error.category.value, {})
        
        if 'max_errors_per_hour' in category_thresholds:
            recent_errors = len([e for e in self.error_log if e.category == error.category and e.timestamp > time.time() - 3600])
            if recent_errors >= category_thresholds['max_errors_per_hour']:
                return True
        
        # Check overall error rate threshold
        if 'max_error_rate' in self.error_thresholds:
            if self.error_summary['error_rate'] >= self.error_thresholds['max_error_rate']:
                return True
        
        return False
    
    def _handle_error_threshold(self, error: ErrorInfo):
        """Handle error threshold breach."""
        print(f"[ERROR_TRACKER] ERROR THRESHOLD BREACHED: {error.category.value} errors too frequent")
        
        # Could trigger alerts, notifications, or automatic responses here
    
    def _trigger_error_callbacks(self, error: ErrorInfo):
        """Trigger registered error callbacks."""
        for callback in self.error_callbacks:
            try:
                callback(error)
            except Exception as e:
                print(f"[ERROR_TRACKER] Error callback failed: {e}")
    
    def _format_error_log(self, error: ErrorInfo):
        """Format error for logging."""
        if self.is_error_suppressed(error.affected_components[0] if error.affected_components else "general"):
            # Suppress logging if component is suppressed
            return
        
        print(f"[ERROR_TRACKER] {error.severity.value.upper()}: {error.category.value} - {error.message}")
    
    def _calculate_severity_correlation(self, errors: List[ErrorInfo]) -> float:
        """Calculate correlation between error frequency and severity."""
        if not errors:
            return 0.0
        
        # Simple correlation calculation
        severity_weights = {
            ErrorSeverity.LOW: 1,
            ErrorSeverity.MEDIUM: 2,
            ErrorSeverity.HIGH: 3,
            ErrorSeverity.CRITICAL: 4
        }
        
        total_weight = sum(severity_weights.get(e.severity, 1) for e in errors)
        max_possible_weight = len(errors) * 4
        
        return total_weight / max_possible_weight if max_possible_weight > 0 else 0.0
    
    def _calculate_component_correlation(self, errors: List[ErrorInfo]) -> Dict[str, float]:
        """Calculate correlation between errors and components."""
        correlation = {}
        
        for error in errors:
            for component in error.affected_components:
                if component not in correlation:
                    correlation[component] = 0
                correlation[component] += 1
        
        # Normalize by total errors
        total_errors = len(errors)
        for component in correlation:
            correlation[component] = correlation[component] / total_errors
        
        return correlation
    
    def _analyze_time_pattern(self, errors: List[ErrorInfo]) -> Dict[str, Any]:
        """Analyze time patterns in error occurrence."""
        if not errors:
            return {}
        
        # Group errors by hour
        hourly_counts = {}
        for error in errors:
            hour = int(error.timestamp) % 24
            if hour not in hourly_counts:
                hourly_counts[hour] = 0
            hourly_counts[hour] += 1
        
        return {
            'hourly_distribution': hourly_counts,
            'peak_hour': max(hourly_counts, key=hourly_counts.get) if hourly_counts else 0,
            'temporal_pattern': 'stable'  # Would require more sophisticated analysis
        }
    
    def _analyze_error_trends(self) -> Dict[str, Any]:
        """Analyze error trends over time."""
        if len(self.error_log) < 2:
            return {'insufficient_data': True}
        
        # Calculate error rate trend
        recent_errors = len([e for e in self.error_log if e.timestamp > time.time() - 3600])
        older_errors = len([e for e in self.error_log if e.timestamp > time.time() - 7200 and e.timestamp <= time.time() - 3600])
        
        return {
            'error_rate_trend': 'increasing' if recent_errors > older_errors else 'decreasing',
            'recent_error_rate': recent_errors / 1.0,  # per hour
            'older_error_rate': older_errors / 1.0 if older_errors > 0 else 0,
            'trend_confidence': min(recent_errors + older_errors, 10) / 10
        }
    
    def _generate_error_recommendations(self) -> List[str]:
        """Generate recommendations based on error analysis."""
        recommendations = []
        
        # Check for frequent errors in specific components
        if self.error_summary['errors_by_component']:
            most_affected = max(self.error_summary['errors_by_component'].items(), key=lambda x: x[1])
            recommendations.append(f"Focus on resolving issues in component: {most_affected[0]} ({most_affected[1]} errors)")
        
        # Check for high severity errors
        if self.error_summary['critical_errors'] > 0:
            recommendations.append(f"Address {self.error_summary['critical_errors']} critical errors immediately")
        
        # Check error rate trends
        if self._analyze_error_trends().get('error_rate_trend') == 'increasing':
            recommendations.append("Error rate is increasing - investigate recent changes")
        
        return recommendations

# Register error tracker with pynlm integration
from .. import pynlm
pynlm.register_error_tracker(ErrorTracker)