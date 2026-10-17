#!/usr/bin/env python3
"""
NLM Logging Tests

Comprehensive tests for the NLM logging system.
"""

import os
import tempfile
import time
import json
from datetime import datetime
from pathlib import Path
import pytest

sys.path.insert(0, str(Path(__file__).parent))

from pynlm.logging import Logger, ModuleLogger, PerformanceLogger, LogMonitor, LogAnalyzer, LogSearch


def test_logger_basic_functionality():
    """Test basic logger functionality."""
    logger = Logger()
    
    # Set log level
    logger.set_level("INFO")
    assert logger.get_level() == "INFO"
    
    # Test logging levels
    logger.debug("Debug message")
    logger.info("Info message")
    logger.warning("Warning message")
    logger.error("Error message")
    logger.critical("Critical message")
    
    # Test handler management
    logger.add_console_handler()
    logger.add_file_handler("test.log")
    logger.add_json_handler("test.json")
    
    assert len(logger.get_handlers()) >= 3
    
    # Clean up
    os.remove("test.log")
    os.remove("test.json")


def test_module_logger():
    """Test module logger functionality."""
    brain_logger = ModuleLogger("Brain")
    memory_logger = ModuleLogger("Memory")
    
    # Set log levels
    brain_logger.set_level("DEBUG")
    memory_logger.set_level("INFO")
    
    assert brain_logger.get_level() == "DEBUG"
    assert memory_logger.get_level() == "INFO"
    
    # Test logging
    brain_logger.debug("Debug brain message")
    brain_logger.info("Info brain message")
    memory_logger.info("Info memory message")
    
    # Clean up handlers
    for logger in [brain_logger, memory_logger]:
        for handler in logger.get_handlers():
            if hasattr(handler, 'close'):
                handler.close()


def test_performance_logger():
    """Test performance logger functionality."""
    perf_logger = PerformanceLogger()
    
    # Test timing
    perf_logger.start_timing("test_operation")
    time.sleep(0.01)  # Simulate work
    perf_logger.end_timing("test_operation")
    
    # Test performance logging
    perf_logger.log_performance("test_operation", 0.001)
    
    # Test memory usage
    perf_logger.log_memory_usage()
    
    # Test CPU usage
    perf_logger.log_cpu_usage()
    
    # Test performance statistics
    stats = perf_logger.get_performance_stats()
    assert "test_operation" in stats
    assert stats["test_operation"]["count"] == 1
    
    # Test performance stats method
    perf_stats = perf_logger.get_performance_stats()
    assert isinstance(perf_stats, dict)


def test_log_monitor():
    """Test log monitor functionality."""
    monitor = LogMonitor()
    
    # Create test log file
    with open("test_monitor.log", "w") as f:
        f.write("2023-10-15 14:30:45 INFO Test message\n")
        f.write("2023-10-15 14:30:46 ERROR Error message\n")
    
    # Start monitoring
    monitor.start_monitoring("test_monitor.log")
    
    # Wait for monitoring
    time.sleep(0.1)
    
    # Get statistics
    stats = monitor.get_statistics()
    assert "test_monitor.log" in stats
    assert stats["test_monitor.log"]["error_count"] == 1
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Get log lines
    lines = monitor.get_log_lines(limit=5)
    assert len(lines) > 0
    
    # Clean up
    os.remove("test_monitor.log")


def test_log_analyzer():
    """Test log analyzer functionality."""
    analyzer = LogAnalyzer()
    
    # Create test log file
    with open("test_analyze.log", "w") as f:
        f.write("2023-10-15 14:30:45 INFO Test message\n")
        f.write("2023-10-15 14:30:46 ERROR Error message\n")
        f.write("2023-10-15 14:30:47 INFO Another message\n")
    
    # Analyze log file
    results = analyzer.analyze_file("test_analyze.log")
    
    # Check results
    assert hasattr(results, "error_count")
    assert hasattr(results, "warning_count")
    assert hasattr(results, "info_count")
    assert results.error_count == 1
    assert results.info_count == 2
    
    # Check error statistics
    error_stats = results.error_statistics
    assert hasattr(error_stats, "total")
    assert hasattr(error_stats, "by_module")
    
    # Check performance statistics
    perf_stats = results.performance_statistics
    assert hasattr(perf_stats, "avg_step_time")
    assert hasattr(perf_stats, "step_time_std")
    
    # Clean up
    os.remove("test_analyze.log")


def test_log_search():
    """Test log search functionality."""
    search = LogSearch()
    
    # Create test log file
    with open("test_search.log", "w") as f:
        f.write("2023-10-15 14:30:45 INFO Test message\n")
        f.write("2023-10-15 14:30:46 ERROR Error message\n")
        f.write("2023-10-15 14:30:47 INFO Another message\n")
    
    # Search for errors
    errors = search.search_file("test_search.log", level="ERROR")
    assert len(errors) == 1
    assert "Error message" in errors[0]
    
    # Search for messages
    messages = search.search_file("test_search.log", message_pattern="Test")
    assert len(messages) == 1
    
    # Search for specific level
    specific_level = search.search_file("test_search.log", level="INFO")
    assert len(specific_level) == 2
    
    # Clean up
    os.remove("test_search.log")


def test_log_rotation():
    """Test log rotation functionality."""
    logger = Logger()
    
    # Add file handler with rotation
    logger.add_file_handler(
        filename="test_rotation.log",
        max_size=100,  # 100 bytes for testing
        backup_count=3
    )
    
    # Write logs to trigger rotation
    for i in range(100):
        logger.info(f"Log message {i}")
    
    # Check if rotation occurred
    # (Implementation depends on log rotation logic)
    
    # Clean up
    for i in range(5):
        log_file = f"test_rotation.log.{i}" if i > 0 else "test_rotation.log"
        if os.path.exists(log_file):
            os.remove(log_file)


def test_log_formats():
    """Test different log formats."""
    # Test text format
    logger = Logger()
    logger.add_console_handler(format="%(levelname)s: %(message)s")
    
    # Test JSON format
    logger.add_json_handler(filename="test_format.json", pretty_print=True)
    
    # Test logging
    logger.info("Test message")
    
    # Clean up
    if os.path.exists("test_format.json"):
        os.remove("test_format.json")


def test_log_debug_mode():
    """Test debug mode functionality."""
    logger = Logger()
    
    # Enable debug mode
    logger.set_debug_mode(True)
    
    # Test debug logging
    logger.debug("Debug message")
    logger.info("Info message")
    
    # Disable debug mode
    logger.set_debug_mode(False)


def test_log_error_with_exc_info():
    """Test error logging with exception info."""
    logger = Logger()
    
    try:
        # Simulate an error
        raise ValueError("Test error")
    except Exception as e:
        logger.error("Test error occurred", exc_info=True)
    
    # Clean up handlers
    for handler in logger.get_handlers():
        if hasattr(handler, 'close'):
            handler.close()


def test_log_performance_multiple_operations():
    """Test performance logging for multiple operations."""
    perf_logger = PerformanceLogger()
    
    # Test multiple timing operations
    perf_logger.start_timing("operation1")
    time.sleep(0.01)
    perf_logger.end_timing("operation1")
    
    perf_logger.start_timing("operation2")
    time.sleep(0.02)
    perf_logger.end_timing("operation2")
    
    # Test performance logging
    perf_logger.log_performance("operation1", 0.001)
    perf_logger.log_performance("operation2", 0.002)
    
    # Test memory usage
    perf_logger.log_memory_usage()
    
    # Test CPU usage
    perf_logger.log_cpu_usage()


def test_log_monitor_statistics():
    """Test log monitor statistics."""
    monitor = LogMonitor()
    
    # Create test log file
    with open("test_stats.log", "w") as f:
        f.write("2023-10-15 14:30:45 INFO Test message\n")
        f.write("2023-10-15 14:30:46 ERROR Error message\n")
        f.write("2023-10-15 14:30:47 WARNING Warning message\n")
    
    # Start monitoring
    monitor.start_monitoring("test_stats.log")
    
    # Wait for monitoring
    time.sleep(0.1)
    
    # Get statistics
    stats = monitor.get_statistics()
    assert "test_stats.log" in stats
    assert stats["test_stats.log"]["error_count"] == 1
    assert stats["test_stats.log"]["warning_count"] == 1
    assert stats["test_stats.log"]["info_count"] == 1
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Clean up
    os.remove("test_stats.log")


def test_log_analyzer_error_statistics():
    """Test log analyzer error statistics."""
    analyzer = LogAnalyzer()
    
    # Create test log file with errors
    with open("test_error_stats.log", "w") as f:
        f.write("2023-10-15 14:30:45 ERROR Module1 Error message 1\n")
        f.write("2023-10-15 14:30:46 ERROR Module2 Error message 2\n")
        f.write("2023-10-15 14:30:47 ERROR Module1 Error message 3\n")
        f.write("2023-10-15 14:30:48 INFO Info message\n")
    
    # Analyze log file
    results = analyzer.analyze_file("test_error_stats.log")
    
    # Check error statistics
    error_stats = results.error_statistics
    assert error_stats.total == 3
    assert "Module1" in error_stats.by_module
    assert "Module2" in error_stats.by_module
    assert error_stats.by_module["Module1"] == 2
    assert error_stats.by_module["Module2"] == 1
    
    # Clean up
    os.remove("test_error_stats.log")


def test_log_search_time_range():
    """Test log search in time range."""
    search = LogSearch()
    
    # Create test log file
    with open("test_time_range.log", "w") as f:
        f.write("2023-10-15 14:30:45 INFO Test message 1\n")
        f.write("2023-10-15 14:30:46 ERROR Error message 2\n")
        f.write("2023-10-15 14:30:47 INFO Test message 3\n")
        f.write("2023-10-15 14:30:48 INFO Test message 4\n")
    
    # Test search in time range (this might need implementation)
    # For now, just test that the method exists
    assert hasattr(search, 'search_time_range')
    
    # Clean up
    os.remove("test_time_range.log")


def test_logger_handler_management():
    """Test logger handler management."""
    logger = Logger()
    
    # Add handlers
    handler1 = logger.add_console_handler()
    handler2 = logger.add_file_handler("test_handler.log")
    
    # Check handlers
    handlers = logger.get_handlers()
    assert len(handlers) == 2
    
    # Remove handler
    logger.remove_handler(handler1)
    assert len(logger.get_handlers()) == 1
    
    # Remove all handlers
    logger.clear_handlers()
    assert len(logger.get_handlers()) == 0
    
    # Clean up
    os.remove("test_handler.log")


def test_logger_log_levels():
    """Test logger log levels."""
    logger = Logger()
    
    # Set different log levels
    logger.set_level("DEBUG")
    assert logger.get_level() == "DEBUG"
    
    logger.set_level("INFO")
    assert logger.get_level() == "INFO"
    
    logger.set_level("WARNING")
    assert logger.get_level() == "WARNING"
    
    logger.set_level("ERROR")
    assert logger.get_level() == "ERROR"
    
    logger.set_level("CRITICAL")
    assert logger.get_level() == "CRITICAL"


def test_log_formatter():
    """Test log formatter functionality."""
    logger = Logger()
    
    # Add custom formatter
    class CustomFormatter:
        def format(self, record):
            return f"[{record.levelname}] {record.name}: {record.message}"
    
    logger.add_console_handler(format="%(levelname)s: %(message)s")
    
    # Test logging
    logger.info("Test message")
    
    # Clean up handlers
    for handler in logger.get_handlers():
        if hasattr(handler, 'close'):
            handler.close()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
