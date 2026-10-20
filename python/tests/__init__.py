Integration tests for NLM Python bindings

This file contains integration tests that validate the complete workflow
of the enhanced NLM Python bindings including:

1. Configuration system integration
2. Brain functionality integration  
3. World simulation integration
4. Complete end-to-end simulations
5. Error recovery and resilience
6. Performance benchmarking

All tests are designed to work with the enhanced test framework and
validate the integration between C++ core components and Python bindings.

Usage:
    Run with pytest: python -m pytest python/tests/test_integration.py -v
    Or run directly: python python/tests/test_integration.py

Features:
- Complete configuration workflow testing
- Brain-agent integration validation
- World simulation end-to-end testing
- Performance regression detection
- Error recovery and resilience testing
- Comprehensive benchmarking

Author: NLM Development Team
Version: 0.1.0
Date: 2025-06-19