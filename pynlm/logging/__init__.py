"""Logging utilities for NLM."""


class Logger:
    """Logging utility for NLM system."""
    
    def __init__(self):
        """Initialize logger."""
        self.log_level = "INFO"
        self.log_file = None
        self.log_to_console = True
        
    def set_level(self, level: str):
        """Set log level."""
        self.log_level = level
        
    def set_file(self, filepath: str):
        """Set log file."""
        self.log_file = filepath
        
    def disable_console(self):
        """Disable console logging."""
        self.log_to_console = False
        
    def enable_console(self):
        """Enable console logging."""
        self.log_to_console = True
        
    def info(self, message: str):
        """Log info message."""
        if self.log_to_console:
            print(f"INFO: {message}")
        
    def error(self, message: str):
        """Log error message."""
        if self.log_to_console:
            print(f"ERROR: {message}")
            
    def debug(self, message: str):
        """Log debug message."""
        if self.log_level == "DEBUG" and self.log_to_console:
            print(f"DEBUG: {message}")
            
    def warning(self, message: str):
        """Log warning message."""
        if self.log_level in ["WARNING", "INFO"] and self.log_to_console:
            print(f"WARNING: {message}")
            
    def critical(self, message: str):
        """Log critical message."""
        if self.log_level in ["CRITICAL", "ERROR", "WARNING", "INFO"] and self.log_to_console:
            print(f"CRITICAL: {message}")
            
    def log(self, level: str, message: str):
        """Log message at specified level."""
        if level == "INFO":
            self.info(message)
        elif level == "ERROR":
            self.error(message)
        elif level == "DEBUG":
            self.debug(message)
        elif level == "WARNING":
            self.warning(message)
        elif level == "CRITICAL":
            self.critical(message)
            
    def log_to_file(self, filepath: str, message: str):
        """Log message to file."""
        if self.log_file:
            with open(self.log_file, 'a') as f:
                f.write(f"{message}\n")
                
    def log_brain_status(self, brain):
        """Log brain status."""
        if self.log_to_console:
            print(f"Brain Status: {brain.get_total_neuron_count()} neurons")
            
    def log_config_summary(self, config):
        """Log configuration summary."""
        if self.log_to_console:
            print(f"Config Summary: {config.summary()}")
            
    def log_experiment_result(self, result):
        """Log experiment result."""
        if self.log_to_console:
            print(f"Experiment Result: {result}")