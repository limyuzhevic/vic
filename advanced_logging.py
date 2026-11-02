#!/usr/bin/env python3
"""
NLM Advanced Logging and Error Reporting System

This module provides enhanced logging, error reporting, and user feedback mechanisms
for the NLM system, designed to be both developer-friendly and user-accessible.
"""

import sys
import argparse
import json
import traceback
import datetime
from pathlib import Path
from enum import Enum
from typing import Dict, Any, Optional, List
import pynlm

class LogLevel(Enum):
    """Enhanced log levels with detailed severity hierarchy"""
    DEBUG = 0
    INFO = 1
    WARNING = 2
    ERROR = 3
    CRITICAL = 4
    SUCCESS = 5

class UserFeedbackLevel(Enum):
    """User feedback levels for different types of information"""
    QUIET = 0
    SUMMARY = 1
    DETAILED = 2
    VERBOSE = 3

class AdvancedLogger:
    """
    Enhanced logging system with multiple output formats, error classification,
    and user-friendly feedback mechanisms.
    """
    
    def __init__(self, log_level: LogLevel = LogLevel.INFO, 
                 feedback_level: UserFeedbackLevel = UserFeedbackLevel.DETAILED,
                 enable_console: bool = True,
                 enable_file: bool = False,
                 log_file: str = "nlm_log.txt"):
        
        self.log_level = log_level
        self.feedback_level = feedback_level
        self.enable_console = enable_console
        self.enable_file = enable_file
        self.log_file = log_file
        
        # Statistics and metrics
        self.log_count = 0
        self.error_count = 0
        self.warning_count = 0
        self.start_time = datetime.datetime.now()
        
        # Error categorization
        self.error_categories = {
            'configuration': [],
            'brain': [],
            'world': [],
            'simulation': [],
            'memory': [],
            'plasticity': [],
            'neuromodulation': [],
            'planning': []
        }
        
        # Session information
        self.session_id = f"NLM_SESSION_{self.start_time.strftime('%Y%m%d_%H%M%S')}"
        self.current_simulation = None
        
        # File handler
        if self.enable_file:
            self.log_file_handle = open(self.log_file, 'a')
            self._write_header()
    
    def _write_header(self):
        """Write session header to log file"""
        header = f"\n{'='*80}\n"
        header += f"NLM Advanced Logging System - Session: {self.session_id}\n"
        header += f"Started: {self.start_time.isoformat()}\n"
        header += f"Python Bindings: {getattr(pynlm, '__version__', 'unknown')}\n"
        header += f"{'='*80}\n\n"
        
        if self.enable_console:
            print(header)
        self.log_file_handle.write(header)
    
    def _should_log(self, level: LogLevel) -> bool:
        """Check if message should be logged based on log level"""
        return level.value >= self.log_level.value
    
    def _format_message(self, level: LogLevel, category: str, 
                       message: str, simulation_id: str = None) -> str:
        """Format log message with timestamp and metadata"""
        timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        
        category_icon = {
            LogLevel.DEBUG: "🔍",
            LogLevel.INFO: "ℹ️",
            LogLevel.WARNING: "⚠️",
            LogLevel.ERROR: "❌",
            LogLevel.CRITICAL: "🔥",
            LogLevel.SUCCESS: "✅"
        }
        
        category_emoji = category_icon.get(level, "📝")
        
        formatted = f"{timestamp} [{level.name:10}] {category_emoji} {category:20} "
        if simulation_id:
            formatted += f"[{simulation_id}] "
        formatted += message
        
        return formatted
    
    def log(self, level: LogLevel, category: str, message: str, 
            simulation_id: str = None, user_facing: bool = True,
            include_traceback: bool = False, error_details: Dict = None):
        """
        Enhanced logging method with detailed metadata and user feedback
        
        Args:
            level: Log level (Debug, Info, Warning, Error, Critical, Success)
            category: Log category (Configuration, Brain, World, etc.)
            message: Log message
            simulation_id: Optional simulation identifier
            user_facing: Whether this message should be shown to end users
            include_traceback: Whether to include full traceback
            error_details: Additional error details dictionary
        """
        
        # Update statistics
        self.log_count += 1
        if level in [LogLevel.ERROR, LogLevel.CRITICAL]:
            self.error_count += 1
        elif level == LogLevel.WARNING:
            self.warning_count += 1
        
        # Format message
        formatted_msg = self._format_message(level, category, message, simulation_id)
        
        # Console output
        if self.enable_console and self._should_log(level):
            print(formatted_msg)
            
            # User-facing feedback
            if user_facing and self._is_user_facing_level(level):
                self._show_user_feedback(level, category, message, error_details)
            
            # Additional details for errors
            if (level in [LogLevel.ERROR, LogLevel.CRITICAL] and 
                self.feedback_level >= UserFeedbackLevel.DETAILED):
                
                if include_traceback:
                    traceback.print_exc()
                
                if error_details:
                    self._show_error_details(error_details)
        
        # File output
        if self.enable_file:
            log_entry = {
                'timestamp': timestamp,
                'level': level.name,
                'category': category,
                'simulation_id': simulation_id,
                'message': message,
                'user_facing': user_facing,
                'error_details': error_details
            }
            
            self.log_file_handle.write(json.dumps(log_entry) + '\n')
            self.log_file_handle.flush()
        
        # Categorize errors for analysis
        if level in [LogLevel.ERROR, LogLevel.CRITICAL] and error_details:
            error_key = self._categorize_error(error_details)
            if error_key:
                self.error_categories[error_key].append({
                    'timestamp': timestamp,
                    'message': message,
                    'details': error_details,
                    'simulation_id': simulation_id
                })
    
    def _is_user_facing_level(self, level: LogLevel) -> bool:
        """Determine if a log level is user-facing"""
        return level in [LogLevel.INFO, LogLevel.WARNING, LogLevel.ERROR, LogLevel.SUCCESS]
    
    def _show_user_feedback(self, level: LogLevel, category: str, 
                          message: str, error_details: Dict = None):
        """Show user-friendly feedback"""
        
        if self.feedback_level == UserFeedbackLevel.QUIET:
            return
        
        # Map log levels to user-friendly messages
        user_messages = {
            LogLevel.INFO: f"📊 {category}: {message}",
            LogLevel.WARNING: f"⚠️ Warning ({category}): {message}",
            LogLevel.ERROR: f"❌ Error ({category}): {message}",
            LogLevel.SUCCESS: f"✅ Success ({category}): {message}",
            LogLevel.CRITICAL: f"🔥 Critical Error ({category}): {message}"
        }
        
        user_msg = user_messages.get(level, f"📝 {category}: {message}")
        
        if self.feedback_level >= UserFeedbackLevel.DETAILED and error_details:
            user_msg += f"\n   Details: {json.dumps(error_details, indent=2)}"
        
        print(f"\n{user_msg}")
    
    def _show_error_details(self, error_details: Dict):
        """Show detailed error information"""
        if self.feedback_level < UserFeedbackLevel.DETAILED:
            return
        
        print("\n   Error Details:")
        for key, value in error_details.items():
            if key != 'traceback':  # Avoid duplicating traceback
                print(f"     {key}: {value}")
    
    def _categorize_error(self, error_details: Dict) -> Optional[str]:
        """Categorize error based on error details"""
        # Common error patterns for categorization
        error_message = error_details.get('message', '').lower()
        error_type = error_details.get('type', '').lower()
        
        if 'config' in error_message or 'configuration' in error_message:
            return 'configuration'
        elif 'brain' in error_message:
            return 'brain'
        elif 'world' in error_message or 'environment' in error_message:
            return 'world'
        elif 'simulation' in error_message:
            return 'simulation'
        elif 'memory' in error_message:
            return 'memory'
        elif 'plasticity' in error_message or 'synaptic' in error_message:
            return 'plasticity'
        elif 'neuromod' in error_message:
            return 'neuromodulation'
        elif 'planner' in error_message or 'planning' in error_message:
            return 'planning'
        
        # Check error type
        if 'config' in error_type or 'parse' in error_type:
            return 'configuration'
        elif 'init' in error_type:
            return 'brain'
        
        return None
    
    def simulation_start(self, simulation_name: str, config_params: Dict):
        """Log simulation start"""
        self.current_simulation = simulation_name
        sim_id = f"{simulation_name}_{self.log_count}"
        
        self.log(LogLevel.INFO, "Simulation", 
                 f"Starting simulation: {simulation_name}",
                 simulation_id=sim_id,
                 user_facing=True)
        
        if self.feedback_level >= UserFeedbackLevel.DETAILED:
            self.log(LogLevel.INFO, "Configuration", 
                     f"Parameters: {json.dumps(config_params, indent=2)}",
                     simulation_id=sim_id,
                     user_facing=False)
    
    def simulation_end(self, simulation_name: str, result_stats: Dict):
        """Log simulation end"""
        self.log(LogLevel.SUCCESS, "Simulation",
                 f"Simulation completed: {simulation_name}",
                 simulation_id=self.current_simulation,
                 user_facing=True)
        
        if self.feedback_level >= UserFeedbackLevel.DETAILED:
            self.log(LogLevel.INFO, "Results",
                     f"Results: {json.dumps(result_stats, indent=2)}",
                     simulation_id=self.current_simulation,
                     user_facing=False)
        
        self.current_simulation = None
    
    def performance_report(self, steps_performed: int, total_time: float,
                          spikes_generated: int):
        """Generate performance report"""
        steps_per_sec = steps_performed / total_time if total_time > 0 else 0
        
        self.log(LogLevel.INFO, "Performance",
                 f"Performance report: {steps_performed} steps in {total_time:.3f}s "
                 f"({steps_per_sec:.1f} steps/s, {spikes_generated} spikes)",
                 simulation_id=self.current_simulation,
                 user_facing=True)
    
    def checkpoint_operation(self, operation: str, filepath: str, 
                           success: bool, details: Dict = None):
        """Log checkpoint operations"""
        level = LogLevel.SUCCESS if success else LogLevel.ERROR
        status = "successful" if success else "failed"
        
        self.log(level, "Checkpoint",
                 f"Checkpoint {operation}: {filepath} - {status}",
                 simulation_id=self.current_simulation,
                 user_facing=True)
        
        if details and not success:
            self.log(LogLevel.ERROR, "Checkpoint",
                     f"Checkpoint {operation} failed: {details}",
                     simulation_id=self.current_simulation,
                     user_facing=True,
                     error_details=details)
    
    def export_session_data(self, output_file: str = None) -> Dict:
        """Export session data for analysis"""
        session_data = {
            'session_id': self.session_id,
            'start_time': self.start_time.isoformat(),
            'end_time': datetime.datetime.now().isoformat(),
            'log_count': self.log_count,
            'error_count': self.error_count,
            'warning_count': self.warning_count,
            'error_categories': self.error_categories,
            'current_simulation': self.current_simulation
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(session_data, f, indent=2)
            
            self.log(LogLevel.SUCCESS, "Export",
                     f"Session data exported to: {output_file}",
                     user_facing=True)
        
        return session_data
    
    def close(self):
        """Close logging session"""
        if self.enable_file and hasattr(self, 'log_file_handle'):
            session_data = self.export_session_data()
            
            footer = f"\n{'='*80}\n"
            footer += f"Session Summary: {self.log_count} log entries, "
            footer += f"{self.error_count} errors, {self.warning_count} warnings\n"
            footer += f"{'='*80}\n"
            
            self.log_file_handle.write(footer)
            self.log_file_handle.close()

class ErrorHandler:
    """Enhanced error handler with user-friendly error messages and recovery suggestions"""
    
    @staticmethod
    def handle_configuration_error(error: Exception, context: Dict = None) -> Dict:
        """Handle configuration-related errors"""
        error_details = {
            'type': 'ConfigurationError',
            'message': str(error),
            'context': context or {},
            'suggestion': ErrorHandler._get_configuration_suggestion(error),
            'recoverable': ErrorHandler._is_configuration_error_recoverable(error)
        }
        
        return error_details
    
    @staticmethod
    def handle_brain_error(error: Exception, context: Dict = None) -> Dict:
        """Handle brain-related errors"""
        error_details = {
            'type': 'BrainError',
            'message': str(error),
            'context': context or {},
            'suggestion': ErrorHandler._get_brain_suggestion(error),
            'recoverable': ErrorHandler._is_brain_error_recoverable(error)
        }
        
        return error_details
    
    @staticmethod
    def handle_simulation_error(error: Exception, context: Dict = None) -> Dict:
        """Handle simulation-related errors"""
        error_details = {
            'type': 'SimulationError',
            'message': str(error),
            'context': context or {},
            'suggestion': ErrorHandler._get_simulation_suggestion(error),
            'recoverable': ErrorHandler._is_simulation_error_recoverable(error)
        }
        
        return error_details
    
    @staticmethod
    def _get_configuration_suggestion(error: Exception) -> str:
        """Get user-friendly suggestion for configuration errors"""
        error_msg = str(error).lower()
        
        if 'file not found' in error_msg or 'no such file' in error_msg:
            return "Check that the configuration file exists and the path is correct."
        elif 'json' in error_msg or 'parse' in error_msg:
            return "Check that the configuration file is valid JSON and has proper syntax."
        elif 'required' in error_msg:
            return "Ensure all required configuration keys are present."
        elif 'type' in error_msg or 'convert' in error_msg:
            return "Check that configuration values have the correct data type."
        else:
            return "Check the configuration file for syntax errors and valid values."
    
    @staticmethod
    def _get_brain_suggestion(error: Exception) -> str:
        """Get user-friendly suggestion for brain errors"""
        error_msg = str(error).lower()
        
        if 'initialize' in error_msg:
            return "Ensure the brain configuration is complete and valid."
        elif 'step' in error_msg:
            return "Check that the simulation parameters are reasonable."
        elif 'memory' in error_msg:
            return "Consider reducing memory requirements or increasing available memory."
        elif 'synapse' in error_msg:
            return "Check synapse connectivity and weight parameters."
        else:
            return "Ensure the brain is properly initialized before use."
    
    @staticmethod
    def _get_simulation_suggestion(error: Exception) -> str:
        """Get user-friendly suggestion for simulation errors"""
        error_msg = str(error).lower()
        
        if 'step' in error_msg or 'time' in error_msg:
            return "Check simulation time steps and duration."
        elif 'spike' in error_msg:
            return "Check neuron parameters and input currents."
        elif 'memory' in error_msg:
            return "Check working memory capacity and settings."
        elif 'plastic' in error_msg:
            return "Check plasticity parameters and learning rates."
        else:
            return "Check simulation parameters and try reducing complexity."
    
    @staticmethod
    def _is_configuration_error_recoverable(error: Exception) -> bool:
        """Check if configuration error is recoverable"""
        return True  # Configuration errors are usually recoverable
    
    @staticmethod
    def _is_brain_error_recoverable(error: Exception) -> bool:
        """Check if brain error is recoverable"""
        error_msg = str(error).lower()
        return not any(phrase in error_msg for phrase in ['memory', 'corrupt', 'fatal'])
    
    @staticmethod
    def _is_simulation_error_recoverable(error: Exception) -> bool:
        """Check if simulation error is recoverable"""
        error_msg = str(error).lower()
        return not any(phrase in error_msg for phrase in ['memory', 'stack overflow', 'segfault'])

class UserInterface:
    """User interface components for improved user experience"""
    
    @staticmethod
    def create_progress_bar(current: int, total: int, width: int = 50) -> str:
        """Create a progress bar string"""
        if total == 0:
            return "[" + "=" * width + "] 0%"
        
        percentage = (current / total) * 100
        filled = int((width * current) / total)
        
        bar = "[" + "=" * filled + ">" + " " * (width - filled - 1) + "]"
        
        return f"{bar} {percentage:.1f}% ({current}/{total})"
    
    @staticmethod
    def format_duration(seconds: float) -> str:
        """Format duration in human-readable format"""
        if seconds < 60:
            return f"{seconds:.2f} seconds"
        elif seconds < 3600:
            minutes = seconds / 60
            return f"{minutes:.1f} minutes"
        else:
            hours = seconds / 3600
            return f"{hours:.1f} hours"
    
    @staticmethod
    def create_help_text(command_list: List[str]) -> str:
        """Create formatted help text"""
        help_text = "Available commands:\n\n"
        
        for cmd in command_list:
            help_text += f"  {cmd}\n"
        
        return help_text

# Convenience functions for easy integration
def create_logger(log_level: LogLevel = LogLevel.INFO,
                 feedback_level: UserFeedbackLevel = UserFeedbackLevel.DETAILED,
                 enable_console: bool = True,
                 enable_file: bool = False,
                 log_file: str = "nlm_log.txt") -> AdvancedLogger:
    """Create an enhanced logger with default settings"""
    return AdvancedLogger(log_level, feedback_level, enable_console, enable_file, log_file)

def log_info(logger: AdvancedLogger, category: str, message: str, 
             simulation_id: str = None):
    """Log information message"""
    logger.log(LogLevel.INFO, category, message, simulation_id)

def log_error(logger: AdvancedLogger, category: str, message: str,
              simulation_id: str = None, error_details: Dict = None):
    """Log error message"""
    logger.log(LogLevel.ERROR, category, message, simulation_id, 
               error_details=error_details)

def log_warning(logger: AdvancedLogger, category: str, message: str,
                simulation_id: str = None):
    """Log warning message"""
    logger.log(LogLevel.WARNING, category, message, simulation_id)

def log_success(logger: AdvancedLogger, category: str, message: str,
                simulation_id: str = None):
    """Log success message"""
    logger.log(LogLevel.SUCCESS, category, message, simulation_id)

# Initialize default logger
_default_logger = create_logger()

def get_default_logger() -> AdvancedLogger:
    """Get the default logger instance"""
    return _default_logger

def set_log_level(level: LogLevel):
    """Set the default log level"""
    _default_logger.log_level = level

def log_info_default(category: str, message: str, simulation_id: str = None):
    """Log information message to default logger"""
    log_info(_default_logger, category, message, simulation_id)

def log_error_default(category: str, message: str, simulation_id: str = None,
                      error_details: Dict = None):
    """Log error message to default logger"""
    log_error(_default_logger, category, message, simulation_id, error_details)

# Error handling shortcuts
error_handler = ErrorHandler()

# User interface shortcuts
ui = UserInterface()
