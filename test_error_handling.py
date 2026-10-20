#!/usr/bin/env python3
"""
Comprehensive Test Suite for NLM CLI Error Handling

Tests for the enhanced error handling system in the NLM CLI tool.
Covers error handling infrastructure, exception hierarchy, validation,
error recovery, and diagnostics.
"""

import sys
import os
import unittest
import json
import logging
from pathlib import Path
from typing import Dict, Any
from unittest.mock import Mock, patch, MagicMock

# Add the nlm_cli directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'nlm_cli'))

from nlm_cli.nlm_cli import (
    NLMCLI, LogLevel, OutputFormat, ErrorCode, NLMException, 
    ConfigurationException, BrainException, SimulationException,
    ValidationException, MemoryException, SensoryException,
    MotorException, PlasticityException, NeuromodulationException,
    PredictionException, CognitionException, ErrorHandling,
    ErrorRecovery, Validation, ErrorReporter, ErrorCode
)
class TestErrorHandlingSystem(unittest.TestCase):
    """Test the error handling infrastructure"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.cli = NLMCLI()
        self.cli.setup_logging()
        
    def test_exception_hierarchy(self):
        """Test exception hierarchy and error codes"""
        # Test base exception
        base_exc = NLMException(ErrorCode.UNKNOWN_ERROR, "Test error")
        self.assertEqual(base_exc.error_code, ErrorCode.UNKNOWN_ERROR)
        self.assertEqual(str(base_exc), "[ErrorCode.UNKNOWN_ERROR] Test error")
        
        # Test specific exception types
        config_exc = ConfigurationException("Config error")
        self.assertEqual(config_exc.error_code, ErrorCode.CONFIG_VALIDATION_FAILED)
        self.assertIsInstance(config_exc, NLMException)
        
        brain_exc = BrainException("Brain error")
        self.assertEqual(brain_exc.error_code, ErrorCode.BRAIN_INITIALIZATION_FAILED)
        
        # Test error codes
        self.assertTrue(ErrorCode.CONFIG_FILE_NOT_FOUND.value > 0)
        self.assertTrue(ErrorCode.BRAIN_INITIALIZATION_FAILED.value > 1000)
        self.assertTrue(ErrorCode.SIMULATION_STEP_FAILED.value > 2000)
        self.assertTrue(ErrorCode.MEMORY_ALLOCATION_FAILED.value > 3000)
        self.assertTrue(ErrorCode.INPUT_VALIDATION_FAILED.value > 4000)
        
    def test_error_serialization(self):
        """Test error serialization and deserialization"""
        # Create exception with context
        original_exc = ConfigurationException(
            "Test config error",
            context={"file": "config.json", "key": "neuron_count"},
            file="/app/config.py",
            line=42,
            function="load_config"
        )
        
        # Convert to dictionary
        exc_dict = original_exc.to_dict()
        self.assertIn("error_code", exc_dict)
        self.assertIn("message", exc_dict)
        self.assertIn("context", exc_dict)
        self.assertEqual(exc_dict["context"]["file"], "config.json")
        self.assertEqual(exc_dict["context"]["key"], "neuron_count")
        
        # Recreate from dictionary
        recreated_exc = NLMException.from_dict(exc_dict)
        self.assertEqual(recreated_exc.error_code, original_exc.error_code)
        self.assertEqual(recreated_exc.message, original_exc.message)
        self.assertEqual(recreated_exc.context, original_exc.context)
        
    def test_error_logging(self):
        """Test error logging functionality"""
        with self.assertLogs(level='ERROR') as log_capture:
            test_exc = NLMException(ErrorCode.SYSTEM_ERROR, "Test logging error")
            test_exc.log(self.cli.logger)
            
            # Check that error was logged
            self.assertTrue(len(log_capture.records) > 0)
            self.assertIn("Test logging error", log_capture.records[0].getMessage())
class TestValidationSystem(unittest.TestCase):
    """Test the validation system"""
    
    def test_range_validation(self):
        """Test range validation"""
        # Valid values should pass
        self.assertTrue(Validation.validate_range(5, 0, 10))
        self.assertTrue(Validation.validate_range(-5, -10, 0))
        
        # Invalid values should fail
        self.assertFalse(Validation.validate_range(15, 0, 10))
        self.assertFalse(Validation.validate_range(-15, -10, 0))
        
        # Test with exception raising
        Validation.validate_range(5, 0, 10, raise_exception=True)  # Should pass
        
        with self.assertRaises(ValidationException):
            Validation.validate_range(15, 0, 10, raise_exception=True)
            
    def test_positive_validation(self):
        """Test positive value validation"""
        self.assertTrue(Validation.validate_positive(5))
        self.assertTrue(Validation.validate_positive(0.1))
        
        self.assertFalse(Validation.validate_positive(0))
        self.assertFalse(Validation.validate_positive(-1))
        
    def test_type_validation(self):
        """Test type validation"""
        self.assertTrue(Validation.validate_type("hello", str))
        self.assertTrue(Validation.validate_type(42, int))
        self.assertTrue(Validation.validate_type(3.14, float))
        
        self.assertFalse(Validation.validate_type("hello", int))
        self.assertFalse(Validation.validate_type(42, str))
        
    def test_dict_keys_validation(self):
        """Test dictionary keys validation"""
        # Valid case
        test_dict = {"a": 1, "b": 2, "c": 3}
        required_keys = ["a", "b"]
        self.assertTrue(Validation.validate_dict_keys(test_dict, required_keys))
        
        # Missing keys
        test_dict2 = {"a": 1}
        self.assertFalse(Validation.validate_dict_keys(test_dict2, required_keys))
        
    def test_exception_raising(self):
        """Test that validation exceptions are raised correctly"""
        with self.assertRaises(ValidationException):
            Validation.validate_range(15, 0, 10, raise_exception=True, name="test_value")
            
        with self.assertRaises(ValidationException):
            Validation.validate_positive(-1, raise_exception=True)
            
        with self.assertRaises(ValidationException):
            Validation.validate_type("hello", int, raise_exception=True)
class TestErrorRecovery(unittest.TestCase):
    """Test error recovery mechanisms"""
    
    def test_retry_mechanism(self):
        """Test retry mechanism"""
        call_count = 0
        
        def failing_function():
            nonlocal call_count
            call_count += 1
            if call_count < 3:
                raise ValidationException("Still failing")
            return "Success"
        
        config = ErrorRecovery.RecoveryConfig(
            strategy=ErrorRecovery.RecoveryStrategy.RETRY,
            max_retries=3,
            retry_delay_ms=1
        )
        
        result = ErrorRecovery.with_retry(config, failing_function)
        self.assertEqual(result, "Success")
        self.assertEqual(call_count, 3)
        
    def test_circuit_breaker(self):
        """Test circuit breaker mechanism"""
        config = ErrorRecovery.RecoveryConfig(
            strategy=ErrorRecovery.RecoveryStrategy.CIRCUIT_BREAKER,
            circuit_breaker_threshold=3
        )
        
        # Initially should be able to proceed
        self.assertTrue(config.can_proceed())
        
        # After failures, circuit should open
        config.record_operation(False)
        config.record_operation(False)
        config.record_operation(False)
        
        self.assertFalse(config.can_proceed())
        
        # After timeout, circuit should close
        config.last_failure_time = time.time() - 2  # 2 seconds ago
        config.failed_operations = 0
        config.last_failure_time = None
        
        self.assertTrue(config.can_proceed())
        
    def test_graceful_degradation(self):
        """Test graceful degradation mechanism"""
        config = ErrorRecovery.RecoveryConfig(
            strategy=ErrorRecovery.RecoveryStrategy.GRACEFUL_DEGRADATION,
            fallback_condition=lambda: True
        )
        
        def failing_function():
            raise ValidationException("Function failed")
        
        # With fallback condition true, should raise fallback exception
        with self.assertRaises(ValidationException) as cm:
            ErrorRecovery.with_graceful_degradation(config, failing_function)
        
        self.assertIn("fallback", str(cm.exception))
        
        # With fallback condition false, should raise original exception
        config.fallback_condition = lambda: False
        with self.assertRaises(ValidationException) as cm:
            ErrorRecovery.with_graceful_degradation(config, failing_function)
        
        self.assertIn("Function failed", str(cm.exception))
class TestErrorHandlingInfrastructure(unittest.TestCase):
    """Test error handling infrastructure"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.error_handler = ErrorHandling()
        
    def test_error_history(self):
        """Test error history management"""
        # Initially empty
        self.assertEqual(len(self.error_handler.error_history), 0)
        
        # Add error
        test_exc = NLMException(ErrorCode.SYSTEM_ERROR, "Test error")
        self.error_handler.handle_error(test_exc)
        
        # Check that error was stored
        self.assertEqual(len(self.error_handler.error_history), 1)
        self.assertEqual(self.error_handler.error_history[0]["error_code"], "SYSTEM_ERROR")
        
        # Test summary
        summary = self.error_handler.get_error_summary()
        self.assertEqual(summary["total_errors"], 1)
        self.assertIn("SYSTEM_ERROR", summary["error_by_code"])
        
    def test_error_handlers(self):
        """Test custom error handlers"""
        handler_called = []
        
        def custom_handler(exception):
            handler_called.append(exception)
        
        self.error_handler.add_error_handler(custom_handler)
        
        # Trigger error
        test_exc = NLMException(ErrorCode.SYSTEM_ERROR, "Test error")
        self.error_handler.handle_error(test_exc)
        
        # Check that custom handler was called
        self.assertEqual(len(handler_called), 1)
        self.assertEqual(handler_called[0], test_exc)
        
    def test_error_context_manager(self):
        """Test error context manager"""
        with self.error_handler.error_context("test_operation", param1="value1", param2="value2"):
            # Inside context, should work normally
            pass
            
        # Test exception in context
        try:
            with self.error_handler.error_context("test_operation", param1="value1"):
                raise ValidationException("Test error in context")
        except ValidationException:
            # Exception should have context
            pass
            
    def test_error_persistence(self):
        """Test error persistence to file"""
        # Add some errors
        test_exc1 = NLMException(ErrorCode.SYSTEM_ERROR, "Test error 1")
        test_exc2 = NLMException(ErrorCode.CONFIG_ERROR, "Test error 2")
        
        self.error_handler.handle_error(test_exc1)
        self.error_handler.handle_error(test_exc2)
        
        # Save to file
        self.error_handler.save_error_history()
        
        # Create new handler and load history
        new_handler = ErrorHandling()
        loaded_history = new_handler.load_error_history()
        
        # Check that errors were loaded
        self.assertEqual(len(loaded_history), 2)
class TestNLMCliErrorHandling(unittest.TestCase):
    """Test NLM CLI error handling integration"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.cli = NLMCLI()
        self.cli.setup_logging()
        
    def test_config_loading_with_error_handling(self):
        """Test config loading with error handling"""
        # Test successful loading
        config_file = "configs/default.cfg"
        if os.path.exists(config_file):
            self.cli.load_config(config_file)
            self.assertTrue(len(self.cli.config) > 0)
        else:
            self.skipTest("Config file not found")
            
        # Test error handling for invalid config
        with patch('builtins.open', side_effect=IOError("File not found")):
            try:
                self.cli.load_config("nonexistent.json")
                self.fail("Expected ConfigurationException")
            except ConfigurationException:
                pass
                
    def test_argument_parsing_with_error_handling(self):
        """Test argument parsing with error handling"""
        # Test valid arguments
        test_args = ["brain", "create", "--neuron-count", "100", "--timestep", "0.001"]
        parsed_args = self.cli.parse_args(test_args)
        self.assertEqual(parsed_args["command"], "brain")
        self.assertEqual(parsed_args["brain_command"], "create")
        self.assertEqual(parsed_args["neuron_count"], "100")
        
        # Test invalid arguments with error handling
        invalid_args = ["--invalid-option"]
        with self.assertRaises(ConfigurationException):
            self.cli.parse_args(invalid_args)
            
    def test_command_dispatch_with_error_handling(self):
        """Test command dispatch with error handling"""
        # Mock the brain create handler
        with patch.object(self.cli, '_handle_brain_create') as mock_handler:
            test_args = {"command": "brain", "brain_command": "create"}
            self.cli._dispatch_command(test_args)
            mock_handler.assert_called_once_with(test_args)
            
    def test_error_handler_initialization(self):
        """Test error handler initialization"""
        self.assertIsNotNone(self.cli.error_handler)
        self.assertIsInstance(self.cli.error_handler, ErrorHandling)
        
    def test_global_exception_handler(self):
        """Test global exception handler"""
        self.assertIsNotNone(self.cli.exception_handler)
class TestErrorReporting(unittest.TestCase):
    """Test error reporting and diagnostics"""
    
    def test_diagnostic_collection(self):
        """Test diagnostic collection"""
        test_exc = NLMException(
            ErrorCode.SYSTEM_ERROR,
            "Test diagnostic error",
            context={"module": "test", "operation": "validate"}
        )
        
        diagnostic = ErrorReporter.collect_diagnostics(test_exc)
        
        self.assertEqual(diagnostic.error_type, "Exception")
        self.assertEqual(diagnostic.error_message, str(test_exc))
        self.assertIn("test", diagnostic.context["module"])
        
    def test_report_generation(self):
        """Test report generation"""
        diagnostic = ErrorReporter.DiagnosticInfo(
            error_type="TestException",
            error_message="Test error",
            stack_trace="Stack trace here",
            system_state="System state here",
            performance_impact="Performance impact here",
            context={"key": "value"},
            recovery_suggestions=["Fix 1", "Fix 2"]
        )
        
        with self.assertLogs(level='ERROR') as log_capture:
            report = ErrorReporter.generate_report(diagnostic, logging.getLogger(__name__))
            
            self.assertIn("Error Report", report)
            self.assertIn("TestException", report)
            self.assertIn("Fix 1", report)
            
    def test_error_reporter_logging(self):
        """Test error reporter logging"""
        test_exc = NLMException(ErrorCode.SYSTEM_ERROR, "Test error for logging")
        
        with self.assertLogs(level='ERROR') as log_capture:
            ErrorReporter.generate_report(
                ErrorReporter.collect_diagnostics(test_exc),
                logging.getLogger(__name__)
            )
            
            self.assertTrue(any("Error Report" in record.getMessage() for record in log_capture.records))
class TestInteractiveErrorHandling(unittest.TestCase):
    """Test interactive session error handling"""
    
    def test_interactive_error_handler(self):
        """Test interactive error handler"""
        from nlm_cli.interactive import InteractiveNLMSession, InteractiveErrorHandler
        
        # Create mock CLI
        mock_cli = Mock()
        mock_cli.logger = Mock()
        mock_cli.logger.error = Mock()
        mock_cli.logger.warning = Mock()
        
        # Create error handler
        error_handler = InteractiveErrorHandler(mock_cli)
        
        # Test error handling
        test_exc = NLMException(ErrorCode.SYSTEM_ERROR, "Test interactive error")
        result = error_handler.handle_command_error("test_command", test_exc)
        
        self.assertTrue(result)
        self.assertEqual(error_handler.error_count, 1)
        
    def test_error_recovery_suggestions(self):
        """Test error recovery suggestions"""
        from nlm_cli.interactive import InteractiveNLMSession
        
        mock_cli = Mock()
        session = InteractiveNLMSession(mock_cli)
        
        # Test suggestions for different error types
        test_exc = ConfigurationException("Config error")
        suggestions = session.error_handler.suggest_recovery(test_exc)
        
        self.assertTrue(any("configuration" in s.lower() for s in suggestions))
        
        test_exc2 = BrainException("Brain error")
        suggestions2 = session.error_handler.suggest_recovery(test_exc2)
        
        self.assertTrue(any("brain" in s.lower() for s in suggestions2))
class TestApiServerErrorHandling(unittest.TestCase):
    """Test API server error handling"""
    
    def test_api_error_reporter(self):
        """Test API error reporter"""
        from nlm_cli.api_server import NLMApiErrorReporter
        
        reporter = NLMApiErrorReporter()
        
        # Test request tracking
        reporter.record_request("GET", "/api/health", 200)
        reporter.record_request("POST", "/api/run", 500)
        
        stats = reporter.get_error_summary()
        self.assertEqual(stats["total_requests"], 2)
        self.assertEqual(stats["error_breakdown"]["500"], 1)
        
    def test_api_error_response_format(self):
        """Test API error response format"""
        from nlm_cli.api_server import NLMAPIError
        
        # Test API error
        api_exc = NLMAPIError("Test API error", status_code=400)
        self.assertEqual(api_exc.status_code, 400)
        self.assertEqual(str(api_exc), "Test API error")
        
        # Test validation error
        val_exc = ValidationException("Validation failed")
        self.assertEqual(val_exc.error_code, ErrorCode.INPUT_VALIDATION_FAILED)
class TestIntegration(unittest.TestCase):
    """Integration tests for error handling"""
    
    def test_full_error_handling_flow(self):
        """Test complete error handling flow"""
        # Create error handler
        error_handler = ErrorHandling()
        
        # Create test exception
        test_exc = ConfigurationException(
            "Integration test error",
            context={"module": "integration", "step": "validation"}
        )
        
        # Handle error
        error_handler.handle_error(test_exc)
        
        # Verify error was handled
        self.assertEqual(len(error_handler.error_history), 1)
        
        # Test error recovery
        config = ErrorRecovery.RecoveryConfig(
            strategy=ErrorRecovery.RecoveryStrategy.GRACEFUL_DEGRADATION,
            fallback_condition=lambda: True
        )
        
        def failing_function():
            raise ConfigurationException("Function failed")
        
        # Should use fallback
        with self.assertRaises(ValidationException) as cm:
            ErrorRecovery.with_graceful_degradation(config, failing_function)
        
        self.assertIn("fallback", str(cm.exception))
        
    def test_validation_and_recovery_integration(self):
        """Test validation and recovery integration"""
        # Test that validation prevents errors from reaching recovery
        result = Validation.validate_range(5, 0, 10, raise_exception=True)
        self.assertTrue(result)
        
        # Test that validation catches invalid values
        with self.assertRaises(ValidationException):
            Validation.validate_range(15, 0, 10, raise_exception=True)
            
    def test_error_serialization_integration(self):
        """Test error serialization integration"""
        # Create and serialize error
        original_exc = BrainException(
            "Integration test",
            context={"test": "value"},
            file="test.py",
            line=123,
            function="test_function"
        )
        
        # Serialize
        exc_dict = original_exc.to_dict()
        self.assertIsInstance(exc_dict, dict)
        
        # Deserialize
        recreated_exc = NLMException.from_dict(exc_dict)
        
        # Compare
        self.assertEqual(original_exc.error_code, recreated_exc.error_code)
        self.assertEqual(original_exc.message, recreated_exc.message)
        self.assertEqual(original_exc.context, recreated_exc.context)
        self.assertEqual(original_exc.file, recreated_exc.file)
        self.assertEqual(original_exc.line, recreated_exc.line)
        self.assertEqual(original_exc.function, recreated_exc.function)
def run_comprehensive_tests():
    """Run comprehensive test suite"""
    print("=" * 80)
    print("NLM CLI Error Handling - Comprehensive Test Suite")
    print("=" * 80)
    
    # Create test suite
    suite = unittest.TestSuite()
    
    # Add test cases
    suite.addTest(unittest.makeSuite(TestErrorHandlingSystem))
    suite.addTest(unittest.makeSuite(TestValidationSystem))
    suite.addTest(unittest.makeSuite(TestErrorRecovery))
    suite.addTest(unittest.makeSuite(TestErrorHandlingInfrastructure))
    suite.addTest(unittest.makeSuite(TestNLMCliErrorHandling))
    suite.addTest(unittest.makeSuite(TestErrorReporting))
    suite.addTest(unittest.makeSuite(TestInteractiveErrorHandling))
    suite.addTest(unittest.makeSuite(TestApiServerErrorHandling))
    suite.addTest(unittest.makeSuite(TestIntegration))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 80)
    print("TEST SUMMARY")
    print("=" * 80)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Success rate: {((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100):.1f}%")
    
    if result.failures:
        print("\nFAILURES:")
        for test, traceback in result.failures:
            print(f"\n{test}: {traceback}")
    
    if result.errors:
        print("\nERRORS:")
        for test, traceback in result.errors:
            print(f"\n{test}: {traceback}")
    
    return result.wasSuccessful()
if __name__ == "__main__":
    success = run_comprehensive_tests()
    sys.exit(0 if success else 1)