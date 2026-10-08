#!/usr/bin/env python3
"""
Simple test to run the NLM test suite
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from test_suite import NLMTestSuite

if __name__ == "__main__":
    suite = NLMTestSuite()
    results = suite.run_all_tests()
    
    if results["success_rate"] < 0.8:
        sys.exit(1)
    else:
        sys.exit(0)
