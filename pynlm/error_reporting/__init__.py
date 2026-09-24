"""
pynlm.error_reporting - Enhanced error reporting and diagnostics for NLM project

This module provides:
- Detailed error messages with context
- Stack trace generation
- Error categorization and handling
- Error recovery suggestions
"""

import traceback
import sys
import json
import time
from typing import Dict, Any, List, Optional, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
import logging
import inspect

# Import logging components
from pynlm.logging import get_simulation_logger, get_brain_logger, get_memory_logger

logger = get_simulation_logger()


class ErrorSeverity(Enum):
    """Enumeration of error severity levels."""
    DEBUG = "debug"
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"
    FATAL = "fatal"


class ErrorCategory(Enum):
    """Enumeration of error categories."""
    BRAIN_STATE = "brain_state"
    SIMULATION = "simulation"
    MEMORY = "memory"
    NEUROMODULATION = "neuromodulation"
    PLASTICITY = "plasticity"
    CONNECTIVITY = "connectivity"
    VISUALIZATION = "visualization"
    CHECKPOINT = "checkpoint"
    CONFIGURATION = "configuration"
    PERFORMANCE = "performance"
    NETWORK = "network"
    VALIDATION = "validation"
    UNKNOWN = "unknown"


@dataclass
class ErrorContext:
    """Context information for errors."""
    brain_region: Optional[str] = None
    simulation_step: Optional[int] = None
    agent_id: Optional[str] = None
    component: Optional[str] = None
    timestamp: Optional[float] = None
    experiment_id: Optional[str] = None
    session_id: Optional[str] = None
    input_parameters: Optional[Dict[str, Any]] = None
    expected_values: Optional[Dict[str, Any]] = None
    actual_values: Optional[Dict[str, Any]] = None
    recovery_suggestions: List[str] = field(default_factory=list)

    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = time.time()


@dataclass
class ErrorDetails:
    """Detailed error information."""
    error_id: str
    message: str
    severity: ErrorSeverity
    category: ErrorCategory
    exception_type: Optional[str] = None
    stack_trace: Optional[str] = None
    file_path: Optional[str] = None
    line_number: Optional[int] = None
    function_name: Optional[str] = None
    traceback_info: Optional[Dict[str, Any]] = None
    context: Optional[ErrorContext] = None
    recovery_action: Optional[str] = None
    recovery_suggestions: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    created_at: float = field(default_factory=time.time)


class ErrorReporter:
    """Enhanced error reporting and diagnostics."""

    def __init__(self, error_log_path: Optional[str] = None):
        """
        Initialize error reporter.

        Parameters
        ----------
        error_log_path : str, optional
            Path to error log file
        """
        self.error_log_path = Path(error_log_path) if error_log_path else None
        self.error_history: List[ErrorDetails] = []
        self.error_counts: Dict[str, int] = defaultdict(int)
        self.recent_errors: deque = deque(maxlen=100)
        self.error_handlers: List[Callable] = []

    def report_error(self, error: Union[str, Exception], 
                    severity: ErrorSeverity = ErrorSeverity.ERROR,
                    category: ErrorCategory = ErrorCategory.UNKNOWN,
                    context: Optional[ErrorContext] = None,
                    recovery_suggestion: Optional[str] = None) -> ErrorDetails:
        """
        Report an error with context.

        Parameters
        ----------
        error : Union[str, Exception]
            Error message or exception
        severity : ErrorSeverity
            Error severity level
        category : ErrorCategory
            Error category
        context : ErrorContext, optional
            Error context
        recovery_suggestion : str, optional
            Suggestion for error recovery

        Returns
        -------
        ErrorDetails
            Reported error details
        """
        if isinstance(error, Exception):
            error_id = f"{category.value}_{int(time.time())}_{len(self.error_history)}"
            error_message = str(error)
            exception_type = type(error).__name__
            stack_trace = self._format_stack_trace(error)
            file_path, line_number, function_name = self._extract_stack_info(error)
        else:
            error_id = f"{category.value}_{int(time.time())}_{len(self.error_history)}"
            error_message = str(error)
            exception_type = None
            stack_trace = None
            file_path, line_number, function_name = self._extract_stack_info()

        error_details = ErrorDetails(
            error_id=error_id,
            message=error_message,
            severity=severity,
            category=category,
            exception_type=exception_type,
            stack_trace=stack_trace,
            file_path=file_path,
            line_number=line_number,
            function_name=function_name,
            context=context,
            recovery_action=recovery_suggestion,
            recovery_suggestions=context.recovery_suggestions if context else []
        )

        self.error_history.append(error_details)
        self.error_counts[category.value] += 1
        self.recent_errors.append(error_details)

        # Log the error
        self._log_error(error_details)

        # Call registered handlers
        self._handle_error(error_details)

        # Write to error log file if configured
        if self.error_log_path:
            self._write_to_error_log(error_details)

        logger.error(f"Reported error {error_id}: {error_message}")
        return error_details

    def _format_stack_trace(self, error: Exception) -> Optional[str]:
        """Format stack trace for an exception."""
        try:
            tb = traceback.extract_tb(error.__traceback__)
            return traceback.format_list(tb)
        except:
            return None

    def _extract_stack_info(self, error: Optional[Exception] = None) -> Tuple[Optional[str], Optional[int], Optional[str]]:
        """Extract file path, line number, and function name from current stack."""
        try:
            if error:
                tb = error.__traceback__
            else:
                tb = sys.exc_info()[2]

            if tb:
                frame = tb.tb_frame.f_back
                file_path = frame.f_code.co_filename
                line_number = tb.tb_lineno
                function_name = frame.f_code.co_name
                return file_path, line_number, function_name
        except:
            pass

        return None, None, None

    def _log_error(self, error_details: ErrorDetails) -> None:
        """Log error to appropriate logger based on category."""
        if error_details.category == ErrorCategory.BRAIN_STATE:
            brain_logger = get_brain_logger()
            brain_logger.error(f"Brain state error: {error_details.message}")
        elif error_details.category == ErrorCategory.MEMORY:
            memory_logger = get_memory_logger()
            memory_logger.error(f"Memory error: {error_details.message}")
        elif error_details.category == ErrorCategory.SIMULATION:
            logger.error(f"Simulation error: {error_details.message}")
        else:
            logger.error(f"Error ({error_details.category.value}): {error_details.message}")

    def _handle_error(self, error_details: ErrorDetails) -> None:
        """Handle error with registered handlers."""
        for handler in self.error_handlers:
            try:
                handler(error_details)
            except Exception as e:
                logger.error(f"Error in error handler: {e}")

    def _write_to_error_log(self, error_details: ErrorDetails) -> None:
        """Write error details to error log file."""
        try:
            self.error_log_path.parent.mkdir(parents=True, exist_ok=True)

            # Convert error details to dict
            error_dict = {
                "error_id": error_details.error_id,
                "message": error_details.message,
                "severity": error_details.severity.value,
                "category": error_details.category.value,
                "exception_type": error_details.exception_type,
                "stack_trace": error_details.stack_trace,
                "file_path": error_details.file_path,
                "line_number": error_details.line_number,
                "function_name": error_details.function_name,
                "context": error_details.context.__dict__ if error_details.context else None,
                "recovery_action": error_details.recovery_action,
                "recovery_suggestions": error_details.recovery_suggestions,
                "metadata": error_details.metadata,
                "created_at": error_details.created_at
            }

            with open(self.error_log_path, 'a') as f:
                f.write(json.dumps(error_dict, default=str) + '\n')

        except Exception as e:
            logger.error(f"Failed to write error to log file: {e}")

    def add_error_handler(self, handler: Callable[[ErrorDetails], None]) -> None:
        """
        Add an error handler.

        Parameters
        ----------
        handler : Callable[[ErrorDetails], None]
            Error handler function
        """
        self.error_handlers.append(handler)

    def get_error_statistics(self, hours: Optional[int] = None) -> Dict[str, Any]:
        """
        Get error statistics.

        Parameters
        ----------
        hours : int, optional
            Number of hours to look back

        Returns
        -------
        Dict[str, Any]
            Error statistics
        """
        if hours:
            cutoff_time = time.time() - (hours * 3600)
            recent_errors = [
                e for e in self.error_history if e.created_at >= cutoff_time
            ]
        else:
            recent_errors = self.error_history

        stats = {
            "total_errors": len(recent_errors),
            "by_category": defaultdict(int),
            "by_severity": defaultdict(int),
            "by_component": defaultdict(int),
            "most_recent_error": None,
            "error_frequency": {}
        }

        for error in recent_errors:
            stats["by_category"][error.category.value] += 1
            stats["by_severity"][error.severity.value] += 1
            if error.context and error.context.component:
                stats["by_component"][error.context.component] += 1

        if recent_errors:
            stats["most_recent_error"] = {
                "id": recent_errors[-1].error_id,
                "message": recent_errors[-1].message,
                "severity": recent_errors[-1].severity.value,
                "category": recent_errors[-1].category.value,
                "created_at": recent_errors[-1].created_at
            }

        # Calculate frequency (errors per hour)
        if recent_errors:
            time_range = (
                max(e.created_at for e in recent_errors) - 
                min(e.created_at for e in recent_errors)
            ) or 1.0  # Avoid division by zero
            hours_range = time_range / 3600
            stats["error_frequency"] = {
                "errors_per_hour": len(recent_errors) / hours_range
            }

        return stats

    def get_recent_errors(self, limit: int = 10) -> List[ErrorDetails]:
        """
        Get recent errors.

        Parameters
        ----------
        limit : int
            Maximum number of errors to return

        Returns
        -------
        List[ErrorDetails]
            Recent errors
        """
        return list(self.recent_errors)[-limit:]

    def get_error_details(self, error_id: str) -> Optional[ErrorDetails]:
        """
        Get error details by ID.

        Parameters
        ----------
        error_id : str
            Error ID

        Returns
        -------
        Optional[ErrorDetails]
            Error details or None
        """
        for error in self.error_history:
            if error.error_id == error_id:
                return error
        return None

    def analyze_error_patterns(self, days: int = 7) -> Dict[str, Any]:
        """
        Analyze error patterns over time.

        Parameters
        ----------
        days : int
        Number of days to analyze

        Returns
        -------
        Dict[str, Any]
            Error pattern analysis
        """
        cutoff_time = time.time() - (days * 24 * 3600)
        relevant_errors = [
            e for e in self.error_history if e.created_at >= cutoff_time
        ]

        analysis = {
            "total_errors": len(relevant_errors),
            "daily_average": len(relevant_errors) / days,
            "errors_by_hour": defaultdict(int),
            "errors_by_day_of_week": defaultdict(int),
            "error_categories": {},
            "most_common_errors": [],
            "recovery_success_rate": 0.0
        }

        # Analyze by time
        for error in relevant_errors:
            hour = datetime.fromtimestamp(error.created_at).hour
            day_of_week = datetime.fromtimestamp(error.created_at).weekday()

            analysis["errors_by_hour"][hour] += 1
            analysis["errors_by_day_of_week"][day_of_week] += 1

            # Track category distribution
            category = error.category.value
            if category not in analysis["error_categories"]:
                analysis["error_categories"][category] = {
                    "count": 0,
                    "severity_distribution": defaultdict(int),
                    "recovery_suggestions": []
                }

            analysis["error_categories"][category]["count"] += 1
            analysis["error_categories"][category]["severity_distribution"][
                error.severity.value
            ] += 1

        # Find most common errors (by message pattern)
        message_counts = defaultdict(int)
        for error in relevant_errors:
            # Use first 100 characters of message for pattern matching
            message_pattern = error.message[:100]
            message_counts[message_pattern] += 1

        analysis["most_common_errors"] = [
            {"pattern": msg, "count": count}
            for msg, count in sorted(
                message_counts.items(), key=lambda x: x[1], reverse=True
            )[:5]
        ]

        logger.info(f"Analyzed error patterns for {days} days: {len(relevant_errors)} errors")
        return analysis


# Convenience instances
error_reporter = ErrorReporter()


def get_error_reporter() -> ErrorReporter:
    """Get error reporter instance."""
    return error_reporter


def report_brain_state_error(error: Union[str, Exception],
                             brain_region: Optional[str] = None,
                             simulation_step: Optional[int] = None,
                             recovery_suggestion: Optional[str] = None) -> ErrorDetails:
    """
    Report a brain state error.

    Parameters
    ----------
    error : Union[str, Exception]
        Error message or exception
    brain_region : str, optional
        Brain region where error occurred
    simulation_step : int, optional
        Simulation step where error occurred
    recovery_suggestion : str, optional
        Suggestion for error recovery

    Returns
    -------
    ErrorDetails
        Reported error details
    """
    context = ErrorContext(
        brain_region=brain_region,
        simulation_step=simulation_step
    )

    return error_reporter.report_error(
        error,
        ErrorSeverity.ERROR,
        ErrorCategory.BRAIN_STATE,
        context,
        recovery_suggestion
    )


def report_simulation_error(error: Union[str, Exception],
                            component: Optional[str] = None,
                            recovery_suggestion: Optional[str] = None) -> ErrorDetails:
    """
    Report a simulation error.

    Parameters
    ----------
    error : Union[str, Exception]
        Error message or exception
    component : str, optional
        Component where error occurred
    recovery_suggestion : str, optional
        Suggestion for error recovery

    Returns
    -------
    ErrorDetails
        Reported error details
    """
    context = ErrorContext(
        component=component
    )

    return error_reporter.report_error(
        error,
        ErrorSeverity.ERROR,
        ErrorCategory.SIMULATION,
        context,
        recovery_suggestion
    )


def report_memory_error(error: Union[str, Exception],
                       recovery_suggestion: Optional[str] = None) -> ErrorDetails:
    """
    Report a memory error.

    Parameters
    ----------
    error : Union[str, Exception]
        Error message or exception
    recovery_suggestion : str, optional
        Suggestion for error recovery

    Returns
    -------
    ErrorDetails
        Reported error details
    """
    return error_reporter.report_error(
        error,
        ErrorSeverity.WARNING,
        ErrorCategory.MEMORY,
        recovery_suggestion=recovery_suggestion
    )