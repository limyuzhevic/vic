"""
Custom exception classes for NLM Python bindings.
These exceptions translate C++ errors into meaningful Python exceptions.
"""

class NLMException(Exception):
    """Base exception for all NLM errors"""
    def __init__(self, message, error_code=None, details=None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        
    def __str__(self):
        if self.error_code:
            return f"NLM Error [{self.error_code}]: {self.message}"
        return f"NLM Error: {self.message}"
class FileIOException(NLMException):
    """Exception for file I/O operations"""
    def __init__(self, message, filepath=None, operation="read", error_code="FILE_IO_ERROR"):
        super().__init__(message, error_code)
        self.filepath = filepath
        self.operation = operation
class ValidationError(NLMException):
    """Exception for validation errors"""
    def __init__(self, message, field=None, value=None, expected=None, error_code="VALIDATION_ERROR"):
        super().__init__(message, error_code)
        self.field = field
        self.value = value
        self.expected = expected
class ConfigurationError(NLMException):
    """Exception for configuration-related errors"""
    def __init__(self, message, key=None, error_code="CONFIGURATION_ERROR"):
        super().__init__(message, error_code)
        self.key = key
class BrainError(NLMException):
    """Exception for brain-related errors"""
    def __init__(self, message, neuron_id=None, error_code="BRAIN_ERROR"):
        super().__init__(message, error_code)
        self.neuron_id = neuron_id
class WorldError(NLMException):
    """Exception for world-related errors"""
    def __init__(self, message, position=None, error_code="WORLD_ERROR"):
        super().__init__(message, error_code)
        self.position = position
class SensoryError(NLMException):
    """Exception for sensory-related errors"""
    def __init__(self, message, sensor_type=None, error_code="SENSORY_ERROR"):
        super().__init__(message, error_code)
        self.sensor_type = sensor_type
class AudioError(NLMException):
    """Exception for audio-related errors"""
    def __init__(self, message, sample_rate=None, error_code="AUDIO_ERROR"):
        super().__init__(message, error_code)
        self.sample_rate = sample_rate
class ActionError(NLMException):
    """Exception for action-related errors"""
    def __init__(self, message, action_type=None, error_code="ACTION_ERROR"):
        super().__init__(message, error_code)
        self.action_type = action_type
class AgentError(NLMException):
    """Exception for agent-related errors"""
    def __init__(self, message, agent_id=None, error_code="AGENT_ERROR"):
        super().__init__(message, error_code)
        self.agent_id = agent_id
# Error code constants for easier reference
class ErrorCodes:
    # File I/O errors
    FILE_NOT_FOUND = "FILE_NOT_FOUND"
    FILE_PERMISSION_ERROR = "FILE_PERMISSION_ERROR"
    FILE_FORMAT_ERROR = "FILE_FORMAT_ERROR"
    FILE_WRITE_ERROR = "FILE_WRITE_ERROR"
    
    # Validation errors
    INVALID_DIMENSIONS = "INVALID_DIMENSIONS"
    INVALID_RANGE = "INVALID_RANGE"
    INVALID_FORMAT = "INVALID_FORMAT"
    NULL_OR_EMPTY = "NULL_OR_EMPTY"
    
    # Configuration errors
    KEY_NOT_FOUND = "KEY_NOT_FOUND"
    TYPE_MISMATCH = "TYPE_MISMATCH"
    MISSING_REQUIRED_KEY = "MISSING_REQUIRED_KEY"
    
    # Brain errors
    NEURON_NOT_FOUND = "NEURON_NOT_FOUND"
    INVALID_CURRENT_VALUE = "INVALID_CURRENT_VALUE"
    CURRENT_OUT_OF_RANGE = "CURRENT_OUT_OF_RANGE"
    
    # World errors
    POSITION_OUT_OF_BOUNDS = "POSITION_OUT_OF_BOUNDS"
    INVALID_COMMAND = "INVALID_COMMAND"
    WORLD_BOUNDARY_VIOLATION = "WORLD_BOUNDARY_VIOLATION"
    
    # Sensory errors
    INVALID_DATA_SIZE = "INVALID_DATA_SIZE"
    INVALID_DATA_FORMAT = "INVALID_DATA_FORMAT"
    
    # Audio errors
    INVALID_SAMPLE_RATE = "INVALID_SAMPLE_RATE"
    SAMPLE_RATE_MISMATCH = "SAMPLE_RATE_MISMATCH"
    AUDIO_LENGTH_MISMATCH = "AUDIO_LENGTH_MISMATCH"
    
    # Action errors
    INVALID_ACTION_TYPE = "INVALID_ACTION_TYPE"
    INVALID_PARAMETER = "INVALID_PARAMETER"
    PARAMETER_OUT_OF_RANGE = "PARAMETER_OUT_OF_RANGE"
    
    # Agent errors
    SENSOR_INPUT_ERROR = "SENSOR_INPUT_ERROR"
    MOTOR_OUTPUT_ERROR = "MOTOR_OUTPUT_ERROR"
    
    # General errors
    MEMORY_ALLOCATION_ERROR = "MEMORY_ALLOCATION_ERROR"
    UNKNOWN_ERROR = "UNKNOWN_ERROR"
    UNEXPECTED_ERROR = "UNEXPECTED_ERROR"