"""
NLM Exceptions

Custom exception hierarchy for NLM errors.
"""

class NLMBrainError(Exception):
    """Base exception for all NLM brain-related errors."""
    pass

class NLMConfigurationError(NLMBrainError):
    """Exception for configuration-related errors."""
    pass

class NLMMemoryError(NLMBrainError):
    """Exception for memory system errors."""
    pass

class NLMPredictionError(NLMBrainError):
    """Exception for prediction system errors."""
    pass

# Convenience exception checks
def is_nlm_error(error):
    """Check if an error is an NLM-related error."""
    return isinstance(error, NLMBrainError)

def get_error_category(error):
    """Get the category of an NLM error."""
    if isinstance(error, NLMConfigurationError):
        return "configuration"
    elif isinstance(error, NLMMemoryError):
        return "memory"
    elif isinstance(error, NLMPredictionError):
        return "prediction"
    else:
        return "brain"