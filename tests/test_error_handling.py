"""Test error handling of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config
import pynlm.utils as utils

class TestErrorHandling:
    """Test error handling and exception hierarchy."""
    
    def test_error_classes_exist(self):
        """Test that custom exception classes are defined."""
        assert hasattr(pynlm, 'PynlmError')
        assert hasattr(pynlm, 'InvalidConfigurationError')
        assert hasattr(pynlm, 'NotInitializedError')
        assert hasattr(pynlm, 'InvalidInputError')
        
        # Verify they are proper exception classes
        from pynlm import PynlmError, InvalidConfigurationError, NotInitializedError, InvalidInputError
        
        assert issubclass(PynlmError, Exception)
        assert issubclass(InvalidConfigurationError, PynlmError)
        assert issubclass(NotInitializedError, PynlmError)
        assert issubclass(InvalidInputError, PynlmError)
    
    def test_not_initialized_error(self, brain):
        """Test NotInitializedError for operations on uninitialized brain."""
        # Brain is initialized by fixture, but let's reset it to test
        brain.reset()
        
        # These operations should raise NotInitializedError
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            brain.step()
        
        assert "Brain must be initialized before stepping" in str(exc_info.value)
        
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            brain.receive_sensory_input([1, 2, 3])
        
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            brain.inject_current(pynlm.NeuronId(1), 1.0)
        
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            brain.step_batch(10)
    
    def test_invalid_input_error(self, brain):
        """Test InvalidInputError for invalid input parameters."""
        brain.initialize()
        
        # Test invalid filepath
        with pytest.raises(pynlm.InvalidInputError) as exc_info:
            brain.save("")
        
        assert "Filepath cannot be empty" in str(exc_info.value)
        
        with pytest.raises(pynlm.InvalidInputError) as exc_info:
            brain.load("")
    
    def test_error_context_manager(self):
        """Test ErrorHandlingContext error context manager."""
        context = utils.ErrorHandlingContext()
        
        # Test that context manager is properly initialized
        assert context.last_error is None
        
        # Test context manager with exception
        test_error = ValueError("Test error")
        
        with pytest.raises(ValueError) as exc_info:
            with context:
                raise test_error
        
        # After exception, context should capture the error
        assert context.last_error is not None
        assert isinstance(context.last_error, ValueError)
        
        # Test context manager without exception
        with context:
            pass
        
        # Context should remain clean after no-exception execution
        assert context.last_error is None
    
    def test_parameter_validation(self, brain):
        """Test parameter validation."""
        brain.initialize()
        
        # Test invalid neuron IDs
        with pytest.raises(pynlm.InvalidInputError) as exc_info:
            brain.inject_current(pynlm.NeuronId(-1), 1.0)
        
        # Test invalid current values
        with pytest.raises(pynlm.InvalidInputError) as exc_info:
            brain.inject_current(pynlm.NeuronId(1), float('nan'))
        
        with pytest.raises(pynlm.InvalidInputError) as exc_info:
            brain.inject_current(pynlm.NeuronId(1), float('inf'))
    
    def test_brain_state_validation(self, brain):
        """Test brain state validation."""
        brain.initialize()
        
        # Test operations on invalid state
        brain.reset()
        
        # Should raise NotInitializedError
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            _ = brain.spike_count
        
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            _ = brain.active_neurons
        
        with pytest.raises(pynlm.NotInitializedError) as exc_info:
            _ = brain.ei_ratio
    
    def test_utility_function_error_handling(self):
        """Test error handling in utility functions."""
        # Test validate_brain with uninitialized brain
        uninitialized_brain = Brain()
        
        # Should handle uninitialized brain gracefully
        result = utils.validate_brain(uninitialized_brain)
        assert isinstance(result, bool)
    
    def test_error_chaining(self):
        """Test that errors are properly chained and have context."""
        brain = Brain()
        brain.reset()
        
        # Test error message quality
        try:
            brain.step()
        except pynlm.NotInitializedError as e:
            assert "must be initialized" in str(e).lower()
        
        # Test that errors have descriptive messages
        try:
            brain.save("")
        except pynlm.InvalidInputError as e:
            assert "cannot be empty" in str(e).lower()

class TestErrorContextManager:
    """Test ErrorHandlingContext functionality."""
    
    def test_context_manager_initial_state(self):
        """Test context manager initial state."""
        context = utils.ErrorHandlingContext()
        assert context.last_error is None
    
    def test_context_manager_exception_capture(self):
        """Test context manager captures exceptions."""
        context = utils.ErrorHandlingContext()
        
        test_exception = RuntimeError("Test exception")
        
        with pytest.raises(RuntimeError) as exc_info:
            with context:
                raise test_exception
        
        assert context.last_error is test_exception
        assert context.last_error is exc_info.value
    
    def test_context_manager_nested_usage(self):
        """Test nested context manager usage."""
        context1 = utils.ErrorHandlingContext()
        context2 = utils.ErrorHandlingContext()
        
        error1 = ValueError("Error 1")
        error2 = TypeError("Error 2")
        
        # First context catches error1
        with pytest.raises(ValueError) as exc_info1:
            with context1:
                raise error1
        
        assert context1.last_error is error1
        
        # Second context catches error2
        with pytest.raises(TypeError) as exc_info2:
            with context2:
                raise error2
        
        assert context2.last_error is error2
        
        # Contexts should be independent
        assert context1.last_error is not error2
        assert context2.last_error is not error1

class TestEnhancedErrorFeatures:
    """Test enhanced error handling features."""
    
    def test_error_hierarchy(self):
        """Test custom error class hierarchy."""
        from pynlm import PynlmError, InvalidConfigurationError
        
        # Test inheritance
        assert issubclass(InvalidConfigurationError, PynlmError)
        
        # Test that we can catch specific error types
        try:
            raise InvalidConfigurationError("Test config error")
        except pynlm.PynlmError:
            pass  # Should catch as base class
        except:
            pytest.fail("Should have caught as PynlmError")
        
        # Test specific error catch
        try:
            raise InvalidConfigurationError("Test config error")
        except pynlm.InvalidConfigurationError:
            pass  # Should catch as specific class
        except:
            pytest.fail("Should have caught as InvalidConfigurationError")
    
    def test_error_descriptive_messages(self):
        """Test that errors have descriptive messages."""
        errors_to_test = [
            pynlm.PynlmError("Base error"),
            pynlm.InvalidConfigurationError("Invalid config"),
            pynlm.NotInitializedError("Not initialized"),
            pynlm.InvalidInputError("Invalid input")
        ]
        
        for error in errors_to_test:
            assert str(error) != ""
            assert error.args is not None
            assert len(error.args) > 0