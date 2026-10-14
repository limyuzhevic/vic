#!/usr/bin/env python3
"""
NLM Test Runner
===============

Enhanced test runner for NLM (Neural Learning Machine) testing framework.
Provides modern test organization, categorization, and reporting.
"""

import os
import sys
import json
import argparse
import subprocess
import time
from pathlib import Path
from typing import Dict, List, Any, Optional

class NLTestRunner:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.test_dir = self.project_root / "tests"
        self.config_file = self.test_dir / "test_config.json"
        self.test_framework = self.project_root / "tests" / "nlm_testing_framework.hpp"
        
        self.config = self.load_config()
        self.test_results = []
        
    def load_config(self) -> Dict[str, Any]:
        """Load test configuration from JSON file."""
        try:
            with open(self.config_file, 'r') as f:
                return json.load(f)
        except Exception as e:
            print(f"Warning: Could not load test config: {e}")
            return self.get_default_config()
    
    def get_default_config(self) -> Dict[str, Any]:
        """Return default test configuration."""
        return {
            "test_configuration": {
                "framework": "doctest",
                "enabled_categories": ["unit", "integration", "performance", "configuration", "error", "cli", "system"]
            },
            "test_paths": {
                "unit_tests": ["test_types.cpp", "test_config.cpp", "test_neuron.cpp", "test_synapse.cpp", "test_stdp.cpp"],
                "integration_tests": ["test_integration.cpp"],
                "performance_tests": ["test_performance.cpp"],
                "configuration_tests": ["test_config_loading.cpp"],
                "cli_tests": ["test_cli.cpp"],
                "error_tests": ["test_error_handling.cpp"]
            }
        }
    
    def discover_test_files(self, category: str = None) -> List[Path]:
        """Discover test files based on category or all files."""
        test_files = []
        
        for category_path, files in self.config["test_paths"].items():
            if category and category_path != f"{category}_tests":
                continue
            elif not category:
                test_files.extend([self.test_dir / f for f in files])
            else:
                test_files.extend([self.test_dir / f for f in files])
        
        # Also include any .cpp files in the tests directory that contain test cases
        for test_file in self.test_dir.glob("*.cpp"):
            if test_file.name.endswith("_test.cpp") or "test_" in test_file.name:
                if test_file not in test_files:
                    test_files.append(test_file)
        
        return test_files
    
    def generate_test_report(self, test_files: List[Path], output_file: str = None) -> Dict[str, Any]:
        """Generate a comprehensive test report."""
        report = {
            "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
            "test_framework": "Modern Enhanced Testing Framework",
            "test_files": [],
            "category_breakdown": {},
            "recommendations": []
        }
        
        for test_file in test_files:
            file_info = {
                "filename": test_file.name,
                "path": str(test_file),
                "size": test_file.stat().st_size,
                "modified_time": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(test_file.stat().st_mtime))
            }
            
            # Determine category based on filename
            filename = test_file.name.lower()
            category = "unknown"
            if "brain" in filename or "neuron" in filename or "synapse" in filename:
                category = "unit"
            elif "integration" in filename:
                category = "integration"
            elif "performance" in filename or "bench" in filename:
                category = "performance"
            elif "config" in filename:
                category = "configuration"
            elif "cli" in filename:
                category = "cli"
            elif "error" in filename:
                category = "error"
            elif "plasticity" in filename or "memory" in filename or "neuromodulation" in filename:
                category = "unit"
            
            file_info["category"] = category
            report["test_files"].append(file_info)
            
            if category not in report["category_breakdown"]:
                report["category_breakdown"][category] = 0
            report["category_breakdown"][category] += 1
        
        # Generate recommendations
        report["recommendations"] = self.generate_recommendations(report)
        
        # Export report if requested
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
            print(f"Test report exported to: {output_file}")
        
        return report
    
    def generate_recommendations(self, report: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on test coverage analysis."""
        recommendations = []
        
        # Check if all major components have tests
        category_counts = report["category_breakdown"]
        
        if category_counts.get("performance", 0) < 2:
            recommendations.append("Add more performance benchmarking tests for critical operations")
        
        if category_counts.get("integration", 0) < 3:
            recommendations.append("Add integration tests for cross-component interactions")
        
        if category_counts.get("error", 0) < 2:
            recommendations.append("Add more error handling and exception safety tests")
        
        if category_counts.get("cli", 0) == 0:
            recommendations.append("Add command-line interface tests")
        
        # Check for test organization
        if report["test_files"]:
            recommendations.append("Consider organizing tests into sub-categories (unit, integration, performance, etc.)")
        
        return recommendations
    
    def create_test_template(self, test_type: str, category: str, output_dir: str = None):
        """Create a test template for a specific test type."""
        templates = {
            "unit": self.create_unit_test_template,
            "integration": self.create_integration_test_template,
            "performance": self.create_performance_test_template,
            "configuration": self.create_configuration_test_template,
            "error": self.create_error_test_template,
            "cli": self.create_cli_test_template
        }
        
        if test_type in templates:
            template = templates[test_type](category)
            
            if output_dir:
                os.makedirs(output_dir, exist_ok=True)
                filename = os.path.join(output_dir, f"test_{test_type}_{category}.cpp")
                with open(filename, 'w') as f:
                    f.write(template)
                print(f"Test template created: {filename}")
            else:
                print(template)
        else:
            print(f"Unknown test type: {test_type}")
    
    def create_unit_test_template(self, category: str) -> str:
        """Create unit test template."""
        return f'''// Unit Test Template
// Tests for {category} components
// This template demonstrates best practices for unit testing

#include "nlm_testing_framework.hpp"
#include <cassert>
#include <iostream>

TEST_CASE("test_{category}_basic", TestCategory::UNIT, "Basic {category} functionality test")
void test_{category}_basic() {{
    // TODO: Add actual test cases
    bool test_passed = true; // Placeholder
    
    if (test_passed) {{
        std::cout << "PASS: Basic {category} test" << std::endl;
    }} else {{
        std::cout << "FAIL: Basic {category} test" << std::endl;
    }}
    
    return test_passed;
}}

TEST_CASE("test_{category}_edge_cases", TestCategory::UNIT, "Edge cases for {category} tests")
void test_{category}_edge_cases() {{
    // TODO: Add edge case tests
    bool test_passed = true; // Placeholder
    
    return test_passed;
}}

// Additional test functions can be added here
'''
    
    def create_integration_test_template(self, category: str) -> str:
        """Create integration test template."""
        return f'''// Integration Test Template
// Tests for system integration and cross-component interactions

#include "nlm_testing_framework.hpp"
#include <cassert>
#include <iostream>

TEST_CASE("test_{category}_integration", TestCategory::INTEGRATION, "Integration test for {category}")
void test_{category}_integration() {{
    // TODO: Add actual integration test cases
    // These tests should verify:
    // 1. Cross-component interactions
    // 2. System-level functionality
    // 3. End-to-end workflows
    
    bool test_passed = true; // Placeholder
    
    if (test_passed) {{
        std::cout << "PASS: {category} integration test" << std::endl;
    }} else {{
        std::cout << "FAIL: {category} integration test" << std::endl;
    }}
    
    return test_passed;
}}

// Integration tests often require multiple components to work together
'''
    
    def create_performance_test_template(self, category: str) -> str:
        """Create performance test template."""
        return f'''// Performance Test Template
// Performance benchmarking for {category}

#include "nlm_testing_framework.hpp"
#include <chrono>
#include <iostream>

TEST_CASE("test_{category}_performance", TestCategory::PERFORMANCE, "Performance test for {category}")
void test_{category}_performance() {{
    // TODO: Add actual performance measurements
    // These tests should measure:
    // 1. Execution time
    // 2. Memory usage
    // 3. Scalability
    // 4. Throughput
    
    std::cout << "PERFORMANCE: {category} benchmark would run here" << std::endl;
    
    // Example performance measurement
    auto start = std::chrono::high_resolution_clock::now();
    
    // TODO: Add actual operation to benchmark
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "PERFORMANCE: Execution time = {duration.count() / 1000.0} ms" << std::endl;
    
    return true;
}}
'''
    
    def create_configuration_test_template(self, category: str) -> str:
        """Create configuration test template."""
        return f'''// Configuration Test Template
// Tests for configuration loading and validation for {category}

#include "nlm_testing_framework.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

TEST_CASE("test_{category}_config_loading", TestCategory::CONFIGURATION, "Configuration loading test for {category}")
void test_{category}_config_loading() {{
    // TODO: Add actual configuration loading tests
    // These tests should verify:
    // 1. Configuration file parsing
    // 2. Default value handling
    // 3. Configuration validation
    
    nlm::Config config;
    
    // TODO: Add actual configuration operations
    
    // For now, just verify the config can be created
    assert(&config != nullptr);
    
    std::cout << "PASS: {category} configuration loading test" << std::endl;
    
    return true;
}}
'''
    
    def create_error_test_template(self, category: str) -> str:
        """Create error handling test template."""
        return f'''// Error Handling Test Template
// Tests for error handling and exception safety for {category}

#include "nlm_testing_framework.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

TEST_CASE("test_{category}_error_handling", TestCategory::ERROR_HANDLING, "Error handling test for {category}")
void test_{category}_error_handling() {{
    // TODO: Add actual error handling tests
    // These tests should verify:
    // 1. Exception type handling
    // 2. Error message quality
    // 3. Error recovery mechanisms
    
    // For now, just verify basic error handling works
    try {{
        // TODO: Add operations that might throw exceptions
        throw std::runtime_error("Test exception");
    }} catch (const std::exception& e) {{
        // Exception caught as expected
        std::cout << "PASS: Error handling test for {category}" << std::endl;
        return true;
    }}
    
    // If we get here, exception was not thrown
    return false;
}}
'''
    
    def create_cli_test_template(self, category: str) -> str:
        """Create CLI test template."""
        return f'''// CLI Test Template
// Command-line interface tests for {category}

#include "nlm_testing_framework.hpp"
#include <cassert>
#include <iostream>

TEST_CASE("test_{category}_cli", TestCategory::CLI, "CLI test for {category}")
void test_{category}_cli() {{
    // TODO: Add actual CLI tests
    // These tests should verify:
    // 1. Command-line argument parsing
    // 2. Help/usage output
    // 3. Version information
    // 4. Configuration file options
    
    std::cout << "CLI: {category} tests would run here" << std::endl;
    
    // For now, just verify the test can be structured
    bool test_passed = true;
    
    if (test_passed) {{
        std::cout << "PASS: CLI test for {category}" << std::endl;
    }} else {{
        std::cout << "FAIL: CLI test for {category}" << std::endl;
    }}
    
    return test_passed;
}}
'''
    
    def run_tests(self, category: str = None, verbose: bool = False, benchmark: bool = False):
        """Run tests based on configuration."""
        print("=== NLM Enhanced Test Runner ===")
        print("Phase 3: Modern Neural Computation Testing")
        print()
        
        # Discover test files
        test_files = self.discover_test_files(category)
        
        print(f"Found {len(test_files)} test files:")
        for test_file in test_files:
            print(f"  - {test_file.name}")
        
        print()
        
        # Generate report
        report_file = "test_report.json"
        report = self.generate_test_report(test_files, report_file)
        
        print(f"Category breakdown: {report['category_breakdown']}")
        print()
        
        # Print recommendations
        print("Recommendations:")
        for recommendation in report["recommendations"]:
            print(f"  - {recommendation}")
        
        print()
        
        # In a real implementation, we would:
        # 1. Compile the tests with the NLM libraries
        # 2. Run the test executable
        # 3. Parse the test results
        # 4. Generate reports
        
        print("Note: This is a framework demonstration.")
        print("In a real implementation, this would compile and run the actual tests.")
        
        return report

def main():
    parser = argparse.ArgumentParser(description="NLM Enhanced Test Runner")
    parser.add_argument("--category", choices=["unit", "integration", "performance", "configuration", "error", "cli", "system"],
                       help="Filter by test category")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("--benchmark", action="store_true", help="Enable performance benchmarking")
    parser.add_argument("--output", help="Output file for test results")
    parser.add_argument("--template", help="Create test template for specified type")
    parser.add_argument("--template-category", default="unit", help="Category for test template")
    
    args = parser.parse_args()
    
    # Get project root (assuming we're in the tests directory or its parent)
    project_root = Path(__file__).parent.parent
    
    runner = NLTestRunner(str(project_root))
    
    if args.template:
        runner.create_test_template(args.template, args.template_category)
    else:
        report = runner.run_tests(
            category=args.category,
            verbose=args.verbose,
            benchmark=args.benchmark
        )
        
        if args.output:
            with open(args.output, 'w') as f:
                import json
                json.dump(report, f, indent=2)
            print(f"Report saved to: {args.output}")

if __name__ == "__main__":
    main()
