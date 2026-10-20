# NLM Python Exceptions

class NLMRuntimeError(RuntimeError):
    """Base exception for NLM runtime errors"""
    pass

class NLMConfigError(NLMRuntimeError):
    """Exception for configuration errors"""
    pass

class NLMValidationError(NLMRuntimeError):
    """Exception for validation errors"""
    pass

class NLMImportError(NLMRuntimeError):
    """Exception for import errors"""
    pass

class NLMBenchmarkError(NLMRuntimeError):
    """Exception for benchmark errors"""
    pass