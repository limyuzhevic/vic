#!/usr/bin/env python3
"""
NLM Test Analyzer - Test result analysis tool for the Neural Learning Machine system.

This script provides comprehensive analysis of test results with support for:
- Test trend analysis
- Performance metrics calculation
- Failure pattern identification
- Correlation analysis
- Visualization preparation
"""

import json
import sys
import os
import argparse
from datetime import datetime
from pathlib import Path
from typing import List, Dict, Any, Optional
import statistics
import math

class NLMTestAnalyzer:
    def __init__(self, results_file: str = ""):
        self.results_file = results_file
        self.results = []
        
    def load_results(self, results_file: str = ""):
        """Load test results from file."""
        if results_file:
            self.results_file = results_file
        
        if not self.results_file:
            # Find the most recent results file
            results_dir = Path(__file__).parent / "results"
            if not results_dir.exists():
                results_dir = Path(__file__).parent
            
            result_files = list(results_dir.glob("test_results_*.json"))
            if result_files:
                # Sort by modification time, newest first
                result_files.sort(key=os.path.getmtime, reverse=True)
                self.results_file = str(result_files[0])
            else:
                print("No results files found")
                return False
        
        if not os.path.exists(self.results_file):
            print(f"Results file not found: {self.results_file}")
            return False
        
        with open(self.results_file, 'r') as f:
            self.results = json.load(f)
        
        print(f"Loaded results from: {self.results_file}")
        return True
    
    def analyze_trends(self):
        """Analyze test trend patterns."""
        print("\n=== Test Trend Analysis ===")
        
        # Group results by category
        categories = {}
        for result in self.results:
            category = result.get('suite', 'unknown')
            if category not in categories:
                categories[category] = []
            categories[category].append(result)
        
        print(f"Test categories found: {len(categories)}")
        
        # Analyze trends in each category
        for category, category_results in categories.items():
            print(f"\nCategory: {category}")
            
            # Calculate statistics
            total_tests = len(category_results)
            passed_tests = sum(1 for r in category_results if r.get('status') == 'PASSED')
            failed_tests = sum(1 for r in category_results if r.get('status') == 'FAILED')
            skipped_tests = sum(1 for r in category_results if r.get('status') == 'SKIPPED')
            
            pass_rate = (passed_tests + skipped_tests) / total_tests * 100 if total_tests > 0 else 0
            
            print(f"  Total Tests: {total_tests}")
            print(f"  Passed: {passed_tests}")
            print(f"  Failed: {failed_tests}")
            print(f"  Skipped: {skipped_tests}")
            print(f"  Pass Rate: {pass_rate:.1f}%")
            
            # Calculate average execution time
            durations = [r.get('duration', 0) for r in category_results if 'duration' in r]
            if durations:
                avg_duration = statistics.mean(durations)
                print(f"  Average Duration: {avg_duration:.2f}s")
    
    def analyze_performance(self):
        """Analyze performance metrics."""
        print("\n=== Performance Analysis ===")
        
        if not self.results:
            print("No test results to analyze")
            return
        
        # Calculate overall statistics
        total_tests = len(self.results)
        passed_tests = sum(1 for r in self.results if r.get('status') == 'PASSED')
        failed_tests = sum(1 for r in self.results if r.get('status') == 'FAILED')
        
        # Calculate performance metrics
        durations = [r.get('duration', 0) for r in self.results if 'duration' in r]
        if durations:
            avg_duration = statistics.mean(durations)
            std_duration = statistics.stdev(durations) if len(durations) > 1 else 0
            min_duration = min(durations)
            max_duration = max(durations)
            
            print(f"Total Execution Time: {sum(durations):.2f}s")
            print(f"Average Time per Test: {avg_duration:.2f}s")
            print(f"Standard Deviation: {std_duration:.2f}s")
            print(f"Fastest Test: {min_duration:.2f}s")
            print(f"Slowest Test: {max_duration:.2f}s")
            print(f"Performance Ratio: {max_duration/min_duration:.2f}")
        
        # Analyze failure patterns
        failed_results = [r for r in self.results if r.get('status') == 'FAILED']
        if failed_results:
            print(f"\nFailure Analysis:")
            print(f"  Failed Tests: {len(failed_results)}")
            
            # Count error types
            error_types = {}
            for result in failed_results:
                error = result.get('error', 'Unknown')
                error_types[error] = error_types.get(error, 0) + 1
            
            print("  Error Types:")
            for error, count in error_types.items():
                print(f"    {error}: {count}")
        
        # Analyze timing outliers
        if durations:
            q1 = statistics.quantiles(durations, n=4)[0]
            q3 = statistics.quantiles(durations, n=4)[2]
            iqr = q3 - q1
            
            print(f"\nTiming Statistics:")
            print(f"  First Quartile (Q1): {q1:.2f}s")
            print(f"  Third Quartile (Q3): {q3:.2f}s")
            print(f"  Interquartile Range (IQR): {iqr:.2f}s")
            
            # Identify outliers
            outliers = []
            for duration in durations:
                if duration < q1 - 1.5 * iqr or duration > q3 + 1.5 * iqr:
                    outliers.append(duration)
            
            if outliers:
                print(f"  Outliers Detected: {len(outliers)}")
                print(f"  Outlier Durations: {[f'{d:.2f}s' for d in outliers]}")
    
    def analyze_correlations(self):
        """Analyze correlations between different metrics."""
        print("\n=== Correlation Analysis ===")
        
        if not self.results:
            print("No test results to analyze")
            return
        
        # Extract metrics for correlation analysis
        durations = []
        status_values = []
        
        for result in self.results:
            if 'duration' in result:
                durations.append(result['duration'])
            
            # Convert status to numeric value for correlation
            status = result.get('status', 'UNKNOWN')
            if status == 'PASSED':
                status_values.append(1)
            elif status == 'FAILED':
                status_values.append(-1)
            elif status == 'SKIPPED':
                status_values.append(0)
            elif status == 'ERROR':
                status_values.append(-2)
            elif status == 'TIMEOUT':
                status_values.append(-3)
            else:
                status_values.append(-999)
        
        # Calculate correlation between duration and status
        if durations and status_values:
            try:
                correlation = sum((d - statistics.mean(durations)) * (s - statistics.mean(status_values)) 
                                 for d, s in zip(durations, status_values)) / (
                    len(durations) * statistics.stdev(durations) * statistics.stdev(status_values))
                
                print(f"Correlation between duration and status: {correlation:.3f}")
                
                if abs(correlation) > 0.7:
                    print("  Strong correlation detected!")
                elif abs(correlation) > 0.3:
                    print("  Moderate correlation detected!")
                else:
                    print("  Weak or no correlation detected!")
            except (statistics.StatisticsError, ZeroDivisionError):
                print("  Cannot calculate correlation (insufficient data)")
    
    def generate_report(self, report_file: str = ""):
        """Generate comprehensive analysis report."""
        print("\n=== Generating Analysis Report ===")
        
        if not report_file:
            report_file = f"test_analysis_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
        
        with open(report_file, 'w') as f:
            f.write("NLM Test Analysis Report\n")
            f.write("=" * 50 + "\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Source File: {self.results_file}\n\n")
            
            # Write analysis results
            f.write("=== Test Trend Analysis ===\n")
            self.analyze_trends_to_file(f)
            
            f.write("\n=== Performance Analysis ===\n")
            self.analyze_performance_to_file(f)
            
            f.write("\n=== Correlation Analysis ===\n")
            self.analyze_correlations_to_file(f)
            
            f.write("\n=== Recommendations ===\n")
            self.generate_recommendations(f)
        
        print(f"Analysis report generated: {report_file}")
    
    def analyze_trends_to_file(self, file):
        """Write trend analysis to file."""
        categories = {}
        for result in self.results:
            category = result.get('suite', 'unknown')
            if category not in categories:
                categories[category] = []
            categories[category].append(result)
        
        file.write(f"Test categories found: {len(categories)}\n")
        
        for category, category_results in categories.items():
            file.write(f"\nCategory: {category}\n")
            
            total_tests = len(category_results)
            passed_tests = sum(1 for r in category_results if r.get('status') == 'PASSED')
            skipped_tests = sum(1 for r in category_results if r.get('status') == 'SKIPPED')
            
            pass_rate = (passed_tests + skipped_tests) / total_tests * 100 if total_tests > 0 else 0
            file.write(f"  Total Tests: {total_tests}\n")
            file.write(f"  Pass Rate: {pass_rate:.1f}%\n")
    
    def analyze_performance_to_file(self, file):
        """Write performance analysis to file."""
        durations = [r.get('duration', 0) for r in self.results if 'duration' in r]
        
        if durations:
            avg_duration = statistics.mean(durations)
            file.write(f"Average Execution Time: {avg_duration:.2f}s\n")
            
            # Count status distribution
            status_counts = {}
            for result in self.results:
                status = result.get('status', 'UNKNOWN')
                status_counts[status] = status_counts.get(status, 0) + 1
            
            file.write("Status Distribution:\n")
            for status, count in status_counts.items():
                file.write(f"  {status}: {count}\n")
    
    def analyze_correlations_to_file(self, file):
        """Write correlation analysis to file."""
        durations = []
        status_values = []
        
        for result in self.results:
            if 'duration' in result:
                durations.append(result['duration'])
            
            status = result.get('status', 'UNKNOWN')
            if status == 'PASSED':
                status_values.append(1)
            elif status == 'FAILED':
                status_values.append(-1)
            elif status == 'SKIPPED':
                status_values.append(0)
            elif status == 'ERROR':
                status_values.append(-2)
            elif status == 'TIMEOUT':
                status_values.append(-3)
            else:
                status_values.append(-999)
        
        if durations and status_values:
            try:
                correlation = sum((d - statistics.mean(durations)) * (s - statistics.mean(status_values)) 
                                 for d, s in zip(durations, status_values)) / (
                    len(durations) * statistics.stdev(durations) * statistics.stdev(status_values))
                
                file.write(f"Correlation between duration and status: {correlation:.3f}\n")
            except (statistics.StatisticsError, ZeroDivisionError):
                file.write("Cannot calculate correlation (insufficient data)\n")
    
    def generate_recommendations(self, file):
        """Generate recommendations based on analysis."""
        if not self.results:
            file.write("No recommendations - insufficient data\n")
            return
        
        # Check for common issues
        failed_count = sum(1 for r in self.results if r.get('status') == 'FAILED')
        if failed_count > 0:
            file.write("1. Investigate failed tests for root causes\n")
        
        # Check for performance issues
        durations = [r.get('duration', 0) for r in self.results if 'duration' in r]
        if durations:
            avg_duration = statistics.mean(durations)
            if avg_duration > 10:  # If average test takes more than 10 seconds
                file.write("2. Consider optimizing slow-performing tests\n")
        
        # Check for error patterns
        error_count = sum(1 for r in self.results if r.get('status') == 'ERROR')
        if error_count > 0:
            file.write("3. Review error patterns and fix recurring issues\n")
        
        # Check for timeout issues
        timeout_count = sum(1 for r in self.results if r.get('status') == 'TIMEOUT')
        if timeout_count > 0:
            file.write("4. Consider increasing test timeouts or optimizing slow tests\n")
        
        # Check for skipped tests
        skipped_count = sum(1 for r in self.results if r.get('status') == 'SKIPPED')
        if skipped_count > 0:
            file.write("5. Investigate skipped tests for configuration or environment issues\n")
        
        file.write("6. Consider adding more comprehensive test coverage for critical components\n")
        file.write("7. Implement automated test scheduling for continuous integration\n")

def main():
    parser = argparse.ArgumentParser(description="NLM Test Analyzer - Analyze test results for the Neural Learning Machine system")
    parser.add_argument("results", help="Test results file path", default="")
    parser.add_argument("--report", help="Analysis report file path")
    parser.add_argument("--verbose", action='store_true', help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Create analyzer
    analyzer = NLMTestAnalyzer(args.results)
    
    # Load results
    if not analyzer.load_results(args.results):
        print("Failed to load results. Exiting.")
        return 1
    
    # Perform analysis
    if args.verbose:
        analyzer.analyze_trends()
        analyzer.analyze_performance()
        analyzer.analyze_correlations()
    
    # Generate report
    report_file = args.report
    if not report_file:
        report_file = f"test_analysis_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
    
    analyzer.generate_report(report_file)
    
    print(f"\nAnalysis complete. Report saved to: {report_file}")
    return 0

if __name__ == "__main__":
    exit(main())