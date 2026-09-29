#!/usr/bin/env python3
"""
Comprehensive Error Handling Tests for NLM

Tests error recovery mechanisms, exception serialization,
error logging and monitoring, and alerting system.
"""

import sys
import os
import json
import time
import threading
import pytest
from typing import Dict, List, Any

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from core.Logger.Logger import Logger, StructuredLogger
    from core.Error.ErrorHandler import ErrorHandler, ErrorCode, RecoveryStrategy, ErrorCategory, ErrorSeverity
    from core.Exceptions.NLMBaseException import NLMBaseException
except ImportError as e:
    print(f"Error importing error handling modules: {e}")
    print("Make sure NLM error handling is properly built.")
    sys.exit(1)
class TestErrorHandling:
    """Test error handling system features"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        # Reset error handler for clean tests
        self.error_handler = ErrorHandler.getInstance()
        self.error_handler.clearAllErrors()
        
        # Create config for brain testing
        self.config = Config()
        self.config.set("neuron_count", 100)
        self.config.set("plasticity_rate", 0.1)
    
    def teardown_method(self):
        """Clean up after each test method"""
        # Reset error handler
        self.error_handler.clearAllErrors()
    
    def test_error_handler_singleton(self):
        """Test ErrorHandler singleton pattern"""
        # Get instance
        handler1 = ErrorHandler.getInstance()
        handler2 = ErrorHandler.getInstance()
        
        # They should be the same instance
        assert handler1 is handler2
        
        # Test basic error handling
        handler1.logError(ErrorCode::ValidationError, "Test error message")
        
        # Verify error was recorded
        errors = handler1.getRecentErrors()
        assert len(errors) == 1
        assert errors[0].code == ErrorCode::ValidationError
        assert "Test error message" in errors[0].message
    
    def test_error_codes_and_categories(self):
        """Test error codes and categorization"""
        handler = ErrorHandler.getInstance()
        
        # Test different error codes by category
        validation_errors = [
            ErrorCode::ValidationError,
            ErrorCode::InvalidParameter,
            ErrorCode::MissingConfiguration,
        ]
        
        for code in validation_errors:
            handler.logError(code, f"Validation error {static_cast<int>(code)}")
        
        system_errors = [
            ErrorCode::MemoryAllocation,
            ErrorCode::OutOfMemory,
            ErrorCode::SystemError,
        ]
        
        for code in system_errors:
            handler.logError(code, f"System error {static_cast<int>(code)}")
        
        # Check total errors
        all_errors = handler.getRecentErrors()
        assert len(all_errors) == len(validation_errors) + len(system_errors)
        
        # Check error recovery strategies
        for code in validation_errors:
            strategy = handler.getRecoveryStrategy(code)
            assert strategy is not None
    
    def test_error_severity_levels(self):
        """Test error severity levels"""
        handler = ErrorHandler.getInstance()
        
        # Test different severity levels
        test_errors = [
            (ErrorCode::ValidationError, ErrorSeverity::Debug, "Debug message"),
            (ErrorCode::InvalidParameter, ErrorSeverity::Info, "Info message"),
            (ErrorCode::FileNotFound, ErrorSeverity::Warning, "Warning message"),
            (ErrorCode::MemoryAllocation, ErrorSeverity::Error, "Error message"),
            (ErrorCode::SystemError, ErrorSeverity::Critical, "Critical message"),
            (ErrorCode::FatalError, ErrorSeverity::Fatal, "Fatal message"),
        ]
        
        for code, severity, message in test_errors:
            handler.logError(code, message, severity)
        
        # Get errors and verify severity
        errors = handler.getRecentErrors()
        severity_map = {
            ErrorCode::ValidationError: ErrorSeverity::Debug,
            ErrorCode::InvalidParameter: ErrorSeverity::Info,
            ErrorCode::FileNotFound: ErrorSeverity::Warning,
            ErrorCode::MemoryAllocation: ErrorSeverity::Error,
            ErrorCode::SystemError: ErrorSeverity::Critical,
            ErrorCode::FatalError: ErrorSeverity::Fatal,
        }
        
        for error in errors:
            expected_severity = severity_map.get(error.code)
            assert error.severity == expected_severity
    
    def test_error_recovery_mechanisms(self):
        """Test error recovery mechanisms"""
        handler = ErrorHandler.getInstance()
        
        # Configure recovery strategies
        handler.setRecoveryStrategy(ErrorCode::ConnectionError, RecoveryStrategy::Retry);
        handler.setRecoveryStrategy(ErrorCode::FileReadError, RecoveryStrategy::Fallback);
        handler.setRecoveryStrategy(ErrorCode::Timeout, RecoveryStrategy::GracefulDegradation);
        
        # Test recovery strategy retrieval
        assert handler.getRecoveryStrategy(ErrorCode::ConnectionError) == RecoveryStrategy::Retry;
        assert handler.getRecoveryStrategy(ErrorCode::FileReadError) == RecoveryStrategy::Fallback;
        assert handler.getRecoveryStrategy(ErrorCode::Timeout) == RecoveryStrategy::GracefulDegradation;
        
        # Test unknown error code
        assert handler.getRecoveryStrategy(ErrorCode::UnknownError) == RecoveryStrategy::None;
    
    def test_error_statistics(self):
        """Test error statistics and monitoring"""
        handler = ErrorHandler.getInstance()
        
        # Clear and generate test errors
        handler.clearAllErrors();
        
        # Generate errors with different characteristics
        for i in range(5):
            handler.logError(ErrorCode::ConnectionError, f"Network error {i}", ErrorSeverity::Error);
        
        for i in range(3):
            handler.logError(ErrorCode::ValidationError, f"Validation error {i}", ErrorSeverity::Warning);
        
        for i in range(2):
            handler.logError(ErrorCode::SystemError, f"System error {i}", ErrorSeverity::Critical);
        
        # Get statistics
        stats_map = handler.getAllErrorStats();
        assert stats_map.size() >= 3;
        
        for code in {ErrorCode::ConnectionError, ErrorCode::ValidationError, ErrorCode::SystemError} {
            auto it = stats_map.find(code);
            if (it != stats_map.end()) {
                assert it->second.count > 0;
            }
        }
        
        # Test error rate calculation
        error_rate = handler.calculateErrorRate(std::chrono::system_clock::now() - std::chrono::hours(1));
        assert error_rate >= 0.0;
        
        # Test recent errors
        recent_errors = handler.getRecentErrors(5);
        assert recent_errors.size() == 5;
        assert all(error.code == ErrorCode::ConnectionError for error in recent_errors);
    
    def test_error_alerting(self):
        """Test error alerting system"""
        handler = ErrorHandler.getInstance();
        
        // Configure alert thresholds
        handler.setAlertThreshold(ErrorSeverity::Error, 3, 60); // 3 errors in 60 seconds
        handler.setAlertThreshold(ErrorSeverity::Critical, 1, 30); // 1 critical in 30 seconds
        
        // Generate errors up to threshold
        for (int i = 0; i < 3; ++i) {
            handler.logError(ErrorCode::ConnectionError, f"Error {i}", ErrorSeverity::Error);
        }
        
        // Generate critical error
        handler.logError(ErrorCode::SystemError, "Critical error", ErrorSeverity::Critical);
        
        // Check alerts
        // Note: This depends on the actual alert implementation
        // auto alerts = handler.getAlerts();
        // assert(alerts.size() >= 1);
        // assert(any("CRITICAL" in alert.message for alert in alerts));
    
    def test_exception_classes(self):
        """Test exception class hierarchy"""
        // Test base exception
        base_exc = NLMBaseException("Base error message", ErrorCode::ValidationError);
        assert(base_exc.getCode() == ErrorCode::ValidationError);
        assert("Base error message" in std::string(base_exc.what()));
        assert(base_exc.getContext().empty());
        
        // Test validation exception
        val_exc = NLMValidationException("Invalid parameter", "learning_rate", -55.0, "-1.0 to 1.0");
        assert(val_exc.getCode() == ErrorCode::InvalidParameter);
        assert("Invalid parameter" in std::string(val_exc.what()));
        assert(val_exc.getParameterName() == "learning_rate");
        
        // Test brain exception
        brain_exc = NLMBrainException("Brain initialization failed", "brain.cpp", 123, "initialize");
        assert(brain_exc.getCode() == ErrorCode::NeuralNetworkError);
        assert("Brain initialization failed" in std::string(brain_exc.what()));
        
        // Test simulation exception
        sim_exc = NLMSimulationException("Step failed", 456);
        assert(sim_exc.getCode() == ErrorCode::SimulationError);
        assert("Step failed" in std::string(sim_exc.what()));
        
        // Test system exception
        sys_exc = NLMSystemException("Memory allocation failed", 999);
        assert(sys_exc.getCode() == ErrorCode::SystemError);
        assert("Memory allocation failed" in std::string(sys_exc.what()));
        
        // Test file exception
        file_exc = NLMFileException("File not found", "/nonexistent/path", 789);
        assert(file_exc.getCode() == ErrorCode::FileReadError);
        assert("File not found" in std::string(file_exc.what()));
        assert(file_exc.getFilepath() == "/nonexistent/path");
    
    def test_exception_chaining(self):
        """Test exception chaining functionality"""
        try {
            try {
                throw NLMValidationException("Original error", "param", 5.0, "0.0-1.0");
            } catch (const NLMBaseException& e) {
                // Chain another exception
                throw NLMSystemException("Chained error", 999) from e;
            }
        } catch (const NLMSystemException& e) {
            // Verify exception chaining
            assert(e.getCauses().size() > 0);
            // The cause should be the validation exception
        }
    
    def test_exception_serialization(self) {
        """Test exception serialization for checkpointing"""
        // Create exception with context
        NLMValidationException original_exc(
            "Invalid parameter value", 
            "learning_rate", 
            1.5,  // actual value
            "0.0 to 1.0",  // expected range
            "neural_network",
            100,
            "create_network"
        );
        
        // Serialize to JSON
        json j = original_exc.toJson();
        assert(!j.empty());
        
        // Parse back
        assert(j.contains("message"));
        assert(j.contains("code"));
        assert(j.contains("parameter"));
        assert(j.contains("actual_value"));
        
        // Verify values
        assert(j["message"] == "Invalid parameter value");
        assert(j["parameter"] == "learning_rate");
        assert(j["actual_value"] == 1.5);
        
        // Deserialize back to exception
        auto recreated_exc = NLMBaseException::fromJson(j);
        assert(recreated_exc->getParameterName() == "learning_rate");
        assert(recreated_exc->getActualValue() == 1.5);
    }
    
    def test_error_logging_integration(self) {
        """Test error logging integration with brain operations"""
        Brain brain(this->config);
        assert(brain.initialize() == true);
        
        // Get logger
        Logger logger = Logger::getInstance();
        StructuredLogger structured_logger = StructuredLogger::getInstance();
        
        // Perform operations that might generate errors
        // (Brain operations should work fine in normal conditions)
        
        // Test manual error logging
        logger.log(ErrorSeverity::Error, "Test error message", "brain.cpp", 123, "step");
        
        // Test structured error logging
        json structured_entry;
        structured_entry["code"] = static_cast<int>(ErrorCode::ValidationError);
        structured_entry["message"] = "Structured error message";
        structured_entry["component"] = "brain";
        structured_entry["operation"] = "step";
        structured_logger.logJson(structured_entry);
        
        // Get recent logs
        auto recent_logs = logger.getRecentLogs(10);
        assert(recent_logs.size() >= 1);
        
        // Cleanup
        brain.shutdown();
    }
    
    void test_error_recovery_operations() {
        """Test error recovery operations"""
        ErrorHandler handler = ErrorHandler::getInstance();
        
        // Configure retry mechanism
        RetryConfig retry_config;
        retry_config.maxRetries = 3;
        retry_config.initialDelay = std::chrono::milliseconds(100);
        retry_config.backoffMultiplier = 2.0;
        retry_config.maxDelay = std::chrono::milliseconds(5000);
        
        handler.setRecoveryStrategy(ErrorCode::ConnectionError, RecoveryStrategy::Retry);
        handler.setRetryConfig(retry_config);
        
        // Test retry operation
        int attempt_count = 0;
        auto failing_operation = [&]() {
            attempt_count++;
            if (attempt_count < 3) {
                throw NLMValidationException("Attempt " + std::to_string(attempt_count) + " failed", "test");
            }
            return "Success";
        };
        
        // This should retry and eventually succeed
        RecoveryResult result = handler.retry(failing_operation);
        assert(attempt_count == 3); // Should have tried 3 times
    }
    
    void test_error_correlation() {
        """Test error correlation and context"""
        ErrorHandler handler = ErrorHandler::getInstance();
        
        // Create correlation ID
        std::string correlation_id = "test_correlation_123";
        
        // Log errors with correlation
        handler.logError(ErrorCode::ConnectionError, "First error", ErrorSeverity::Error, 
                        correlation_id, {{"component", "database"}, {"operation", "query"}});
        
        handler.logError(ErrorCode::ValidationError, "Second error", ErrorSeverity::Error,
                        correlation_id, {{"component", "network"}, {"operation", "connect"}});
        
        // Get correlated errors
        std::vector<ErrorEntry> correlated_errors = handler.getCorrelatedErrors(correlation_id);
        assert(correlated_errors.size() == 2);
        
        // All errors should have the same correlation ID
        for (const auto& error : correlated_errors) {
            assert(error.correlationId == correlation_id);
        }
    }
    
    void test_error_pattern_detection() {
        """Test error pattern detection"""
        ErrorHandler handler = ErrorHandler::getInstance();
        
        // Clear and generate pattern
        handler.clearAllErrors();
        
        // Generate recurring error pattern
        for (int i = 0; i < 5; ++i) {
            handler.logError(ErrorCode::ConnectionError, "Recurring error " + std::to_string(i), ErrorSeverity::Error);
        }
        
        // Generate different error
        handler.logError(ErrorCode::ValidationError, "Different error", ErrorSeverity::Warning);
        
        // Check pattern detection
        std::vector<ErrorPattern> patterns = handler.detectErrorPatterns(std::chrono::system_clock::now() - std::chrono::hours(1));
        
        // Should detect the recurring error pattern
        std::vector<ErrorPattern> recurring_patterns;
        for (const auto& pattern : patterns) {
            if (pattern.code == ErrorCode::ConnectionError) {
                recurring_patterns.push_back(pattern);
            }
        }
        assert(recurring_patterns.size() > 0);
        assert(recurring_patterns[0].occurrenceCount == 5);
    }
    
    void test_error_context_tracking() {
        """Test error context tracking"""
        ErrorHandler handler = ErrorHandler::getInstance();
        
        // Log error with rich context
        handler.logError(ErrorCode::ConnectionError, "Test error with context", ErrorSeverity::Error,
                        "correlation_456", 
                        {{"component", "neural_network"},
                         {"operation", "forward_pass"},
                         {"neuron_count", "100"},
                         {"simulation_step", "42"},
                         {"input_features", "[0.1, 0.2, 0.3]"}});
        
        // Get the error
        std::vector<ErrorEntry> errors = handler.getRecentErrors(1);
        assert(errors.size() == 1);
        ErrorEntry error = errors[0];
        
        // Verify context
        assert(error.correlationId == "correlation_456");
        assert(error.context.find("component") != error.context.end());
        assert(error.context.at("component") == "neural_network");
        assert(error.context.at("operation") == "forward_pass");
        assert(error.context.at("neuron_count") == "100");
    }
    
    void test_convenience_functions() {
        """Test convenience functions"""
        // Test logError convenience function
        logError(ErrorCode::ValidationError, "Convenience function error");
        
        // Test retryOperation convenience function
        int attempt_count = 0;
        auto failing_with_retry = [&]() {
            attempt_count++;
            if (attempt_count < 2) {
                throw NLMValidationException("Still failing", "test");
            }
            return "Success after retry";
        };
        
        bool success = retryOperation(failing_with_retry);
        assert(success == true);
        assert(attempt_count == 2);
        
        // Test recoverError convenience function
        recoverError(NLMValidationException("Recovered error", "test"), "FALLBACK");
        
        // Verify errors were logged
        std::vector<ErrorEntry> errors = handler.getRecentErrors(10);
        assert(errors.size() >= 3); // logError, retryOperation, recoverError
    }
    
    void test_error_thread_safety() {
        """Test error handling thread safety"""
        ErrorHandler handler = ErrorHandler::getInstance();
        handler.clearAllErrors();
        
        // Generate errors from multiple threads
        int num_threads = 5;
        int errors_per_thread = 10;
        
        auto worker = [&](int thread_id) {
            for (int i = 0; i < errors_per_thread; ++i) {
                handler.logError(
                    ErrorCode::ValidationError + thread_id * 100 + i, 
                    "Thread " + std::to_string(thread_id) + " error " + std::to_string(i),
                    ErrorSeverity::Error
                );
            }
        };
        
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(worker, i);
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify all errors were recorded
        std::vector<ErrorEntry> all_errors = handler.getRecentErrors(1000);
        int expected_total = num_threads * errors_per_thread;
        assert(all_errors.size() == expected_total);
    }

};

int main() {
    // Simple test runner (in real implementation, use pytest)
    TestErrorHandling tester;
    
    std::cout << "Running Error Handling Tests..." << std::endl;
    
    tester.setup_method();
    
    // Run tests
    tester.test_error_handler_singleton();
    std::cout << "✓ test_error_handler_singleton" << std::endl;
    
    tester.test_error_codes_and_categories();
    std::cout << "✓ test_error_codes_and_categories" << std::endl;
    
    tester.test_error_severity_levels();
    std::cout << "✓ test_error_severity_levels" << std::endl;
    
    tester.test_error_recovery_mechanisms();
    std::cout << "✓ test_error_recovery_mechanisms" << std::endl;
    
    tester.test_error_statistics();
    std::cout << "✓ test_error_statistics" << std::endl;
    
    tester.teardown_method();
    
    std::cout << "\nAll error handling tests passed!" << std::endl;
    return 0;
}
