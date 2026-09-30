#!/usr/bin/env python3
# Comprehensive Test Report Generator for NLM Enhanced Testing
# Enterprise-grade report generation with multiple formats and analysis

import json
import xml.etree.ElementTree as ET
from xml.dom import minidom
import html
from datetime import datetime
import os
import sys
import glob
from pathlib import Path
from typing import Dict, List, Any, Optional
import subprocess
import re

class NLMTestReportGenerator:
    def __init__(self, project_root: str, report_dir: str):
        self.project_root = Path(project_root)
        self.report_dir = Path(report_dir)
        self.report_dir.mkdir(parents=True, exist_ok=True)
        
        # Report data structure
        self.report_data = {
            'timestamp': datetime.now().isoformat(),
            'project': 'NLM (Neural Learning Machine)',
            'version': '0.1.0',
            'test_suites': {},
            'summary': {
                'total_tests': 0,
                'passed_tests': 0,
                'failed_tests': 0,
                'skipped_tests': 0,
                'total_execution_time_ms': 0,
                'memory_leaks_detected': 0,
                'thread_safety_violations': 0
            }
        }
        
    def load_junit_reports(self):
        """Load all JUnit XML reports from the test reports directory"""
        junit_files = glob.glob(str(self.report_dir / "*.xml"))
        
        for junit_file in junit_files:
            test_name = Path(junit_file).stem
            suite_name = self._extract_suite_name(test_name)
            
            try:
                tree = ET.parse(junit_file)
                root = tree.getroot()
                
                suite_data = self._parse_junit_report(root, test_name, suite_name)
                self.report_data['test_suites'][test_name] = suite_data
                
                # Update summary
                self._update_summary(suite_data)
                
            except Exception as e:
                print(f"Warning: Failed to parse JUnit report {junit_file}: {e}")
    
    def load_coverage_reports(self):
        """Load coverage reports if available"""
        coverage_files = glob.glob(str(self.report_dir / "coverage*.info"))
        
        if coverage_files:
            self.report_data['coverage'] = {
                'files_analyzed': len(coverage_files),
                'total_lines': 0,
                'covered_lines': 0,
                'coverage_percentage': 0.0
            }
            
            # Parse lcov info files
            for coverage_file in coverage_files:
                try:
                    with open(coverage_file, 'r') as f:
                        for line in f:
                            if line.startswith('DA:') and ',' in line:
                                parts = line.strip().split(',')
                                total = int(parts[0])
                                covered = int(parts[1])
                                self.report_data['coverage']['total_lines'] += total
                                self.report_data['coverage']['covered_lines'] += covered
                except Exception as e:
                    print(f"Warning: Failed to parse coverage file {coverage_file}: {e}")
            
            if self.report_data['coverage']['total_lines'] > 0:
                self.report_data['coverage']['coverage_percentage'] = (
                    self.report_data['coverage']['covered_lines'] * 100.0 / 
                    self.report_data['coverage']['total_lines']
                )
    
    def load_html_reports(self):
        """Load HTML reports for metadata extraction"""
        html_files = glob.glob(str(self.report_dir / "*_report.html"))
        self.report_data['html_reports'] = len(html_files)
    
    def generate_junit_report(self, output_file: Optional[str] = None):
        """Generate comprehensive JUnit XML report"""
        if output_file is None:
            output_file = str(self.report_dir / "TEST-nlm-comprehensive.xml")
        
        # Create root element
        testsuites = ET.Element('testsuites')
        
        for suite_name, suite_data in self.report_data['test_suites'].items():
            testsuite = ET.Element('testsuite')
            testsuite.set('name', suite_name)
            testsuite.set('tests', str(suite_data['total']))
            testsuite.set('failures', str(suite_data['failures']))
            testsuite.set('errors', str(suite_data['errors']))
            testsuite.set('time', str(suite_data['time']))
            testsuite.set('timestamp', self.report_data['timestamp'])
            
            for test_case in suite_data['test_cases']:
                testcase = ET.Element('testcase')
                testcase.set('name', test_case['name'])
                testcase.set('classname', test_case['classname'])
                testcase.set('time', str(test_case['time']))
                
                if test_case['status'] == 'FAILED':
                    failure = ET.Element('failure')
                    failure.set('message', test_case['message'])
                    failure.text = test_case.get('failure_message', '')
                    testcase.append(failure)
                
                testsuite.append(testcase)
            
            testsuites.append(testsuite)
        
        # Write XML report
        rough_string = ET.tostring(testsuites, 'utf-8')
        reparsed = minidom.parseString(rough_string)
        xml_str = reparsed.toprettyxml(indent="  ")
        
        with open(output_file, 'w') as f:
            f.write(xml_str)
        
        print(f"JUnit report generated at {output_file}")
        return output_file
    
    def generate_html_report(self, output_file: Optional[str] = None):
        """Generate comprehensive HTML report"""
        if output_file is None:
            output_file = str(self.report_dir / "test_report_comprehensive.html")
        
        html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>NLM Enhanced Testing Report</title>
    <style>
        body {{ font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: 0 auto; background-color: white; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }}
        .header {{ background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 30px; text-align: center; }}
        .header h1 {{ margin: 0; font-size: 2.5em; }}
        .header p {{ margin: 10px 0 0 0; opacity: 0.9; }}
        .summary {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; padding: 30px; }}
        .summary-card {{ background: #f8f9fa; border-radius: 8px; padding: 20px; text-align: center; border: 1px solid #e9ecef; }}
        .summary-card h3 {{ margin: 0 0 10px 0; color: #6c757d; font-size: 1.1em; }}
        .summary-card .value {{ font-size: 2em; font-weight: bold; color: #007bff; }}
        .test-suite {{ margin: 30px; border: 1px solid #e9ecef; border-radius: 8px; overflow: hidden; }}
        .test-suite-header {{ background-color: #f8f9fa; padding: 20px; border-bottom: 1px solid #e9ecef; }}
        .test-suite-title {{ font-size: 1.3em; font-weight: bold; color: #495057; }}
        .test-suite-stats {{ display: flex; gap: 20px; margin-top: 10px; }}
        .stat {{ padding: 5px 10px; border-radius: 4px; font-size: 0.9em; }}
        .stat.passed {{ background-color: #d4edda; color: #155724; }}
        .stat.failed {{ background-color: #f8d7da; color: #721c24; }}
        .stat.skipped {{ background-color: #fff3cd; color: #856404; }}
        .test-cases {{ padding: 20px; }}
        .test-case {{ margin: 10px 0; padding: 15px; border-radius: 4px; border-left: 4px solid #007bff; background-color: #f8f9fa; }}
        .test-case.failed {{ border-left-color: #dc3545; background-color: #fff5f5; }}
        .test-case.skipped {{ border-left-color: #ffc107; background-color: #fffbf0; }}
        .test-case-header {{ display: flex; justify-content: space-between; align-items: center; }}
        .test-case-name {{ font-weight: bold; }}
        .test-case-status {{ padding: 5px 10px; border-radius: 4px; font-size: 0.85em; }}
        .coverage-section {{ margin: 30px; padding: 20px; background-color: #f8f9fa; border-radius: 8px; }}
        .coverage-bar {{ background-color: #e9ecef; border-radius: 4px; height: 30px; margin: 10px 0; }}
        .coverage-fill {{ background: linear-gradient(90deg, #28a745, #ffc107, #dc3545); height: 100%; border-radius: 4px; display: flex; align-items: center; justify-content: center; color: white; font-weight: bold; }}
        .recommendations {{ margin: 30px; padding: 20px; background-color: #e3f2fd; border-radius: 8px; border-left: 4px solid #2196f3; }}
        .recommendations h3 {{ color: #1565c0; margin-top: 0; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Enhanced Testing Report</h1>
            <p>Generated on {self.report_data['timestamp']}</p>
            <p>Comprehensive test analysis with enterprise-grade reporting</p>
        </div>
        
        <div class="summary">
            <div class="summary-card">
                <h3>Total Tests</h3>
                <div class="value">{self.report_data['summary']['total_tests']}</div>
            </div>
            <div class="summary-card">
                <h3>Passed</h3>
                <div class="value" style="color: #28a745;">{self.report_data['summary']['passed_tests']}</div>
            </div>
            <div class="summary-card">
                <h3>Failed</h3>
                <div class="value" style="color: #dc3545;">{self.report_data['summary']['failed_tests']}</div>
            </div>
            <div class="summary-card">
                <h3>Skipped</h3>
                <div class="value" style="color: #ffc107;">{self.report_data['summary']['skipped_tests']}</div>
            </div>
            <div class="summary-card">
                <h3>Coverage</h3>
                <div class="value">{self.report_data.get('coverage', {}).get('coverage_percentage', 0):.1f}%</div>
            </div>
        </div>
    """
        
        # Add test suite sections
        for suite_name, suite_data in self.report_data['test_suites'].items():
            html_content += f"""
        <div class="test-suite">
            <div class="test-suite-header">
                <div class="test-suite-title">{suite_name}</div>
                <div class="test-suite-stats">
                    <span class="stat passed">✓ Passed: {suite_data['passed']}</span>
                    <span class="stat failed">✗ Failed: {suite_data['failed']}</span>
                    <span class="stat skipped">~ Skipped: {suite_data['skipped']}</span>
                    <span class="stat">Time: {suite_data['time']:.2f}s</span>
                </div>
            </div>
            
            <div class="test-cases">
    """
            
            for test_case in suite_data['test_cases']:
                status_class = test_case['status'].lower()
                status_icon = {'PASSED': '✓', 'FAILED': '✗', 'SKIPPED': '~'}[test_case['status']]
                status_color = {'PASSED': '#28a745', 'FAILED': '#dc3545', 'SKIPPED': '#ffc107'}[test_case['status']]
                
                html_content += f"""
                <div class="test-case {status_class}">
                    <div class="test-case-header">
                        <div class="test-case-name">{status_icon} {test_case['name']}</div>
                        <div class="test-case-status" style="background-color: {status_color}; color: white;">{test_case['status']}</div>
                    </div>
                    <div style="margin-top: 10px; color: #6c757d; font-size: 0.9em;">
                        <strong>Class:</strong> {test_case['classname']} | 
                        <strong>Time:</strong> {test_case['time']:.4f}s
                    </div>
                    {f'<div style="margin-top: 5px; color: #dc3545;"><strong>Message:</strong> {html.escape(test_case["message"])}</div>' if test_case.get('message') else ''}
                </div>
            """
            
            html_content += """
            </div>
        </div>
        """
        
        # Add coverage section
        if 'coverage' in self.report_data:
            coverage = self.report_data['coverage']
            coverage_percentage = coverage.get('coverage_percentage', 0)
            coverage_color = '#28a745' if coverage_percentage >= 90 else '#ffc107' if coverage_percentage >= 80 else '#dc3545'
            
            html_content += f"""
        <div class="coverage-section">
            <h2>Code Coverage Analysis</h2>
            <p>Coverage analysis based on generated lcov data</p>
            <div class="coverage-bar">
                <div class="coverage-fill" style="width: {coverage_percentage}%; background-color: {coverage_color};">{coverage_percentage:.1f}%</div>
            </div>
            <p><strong>Total Lines:</strong> {coverage['total_lines']:,}</p>
            <p><strong>Covered Lines:</strong> {coverage['covered_lines']:,}</p>
            <p><strong>Files Analyzed:</strong> {coverage['files_analyzed']}</p>
        </div>
        """
        
        # Add recommendations
        html_content += """
        <div class="recommendations">
            <h3>Test Recommendations</h3>
            <ul>
                <li>Review failed tests and address any regressions</li>
                <li>Ensure code coverage meets project standards (>=80%)</li>
                <li>Consider adding more comprehensive integration tests</li>
                <li>Implement performance benchmarks for critical components</li>
                <li>Add edge case testing for neural computation boundaries</li>
            </ul>
        </div>
        
        <div style="text-align: center; padding: 30px; color: #6c757d; font-size: 0.9em;">
            <p>Generated by NLM Enhanced Testing Infrastructure</p>
            <p>For technical support, contact the development team</p>
        </div>
    </div>
</body>
</html>
"""
        
        with open(output_file, 'w') as f:
            f.write(html_content)
        
        print(f"HTML report generated at {output_file}")
        return output_file
    
    def generate_json_report(self, output_file: Optional[str] = None):
        """Generate JSON summary report"""
        if output_file is None:
            output_file = str(self.report_dir / "test_report_summary.json")
        
        with open(output_file, 'w') as f:
            json.dump(self.report_data, f, indent=2, default=str)
        
        print(f"JSON report generated at {output_file}")
        return output_file
    
    def generate_all_reports(self):
        """Generate all available report formats"""
        print("Generating comprehensive test reports...")
        
        # Load reports
        self.load_junit_reports()
        self.load_coverage_reports()
        self.load_html_reports()
        
        # Generate reports
        junit_report = self.generate_junit_report()
        html_report = self.generate_html_report()
        json_report = self.generate_json_report()
        
        print("\n=== Test Report Generation Complete ===")
        print(f"✓ JUnit Report: {junit_report}")
        print(f"✓ HTML Report: {html_report}")
        print(f"✓ JSON Summary: {json_report}")
        
        if 'coverage' in self.report_data:
            print(f"✓ Code Coverage: {self.report_data['coverage']['coverage_percentage']:.1f}%")
        
        return [junit_report, html_report, json_report]
    
    def _extract_suite_name(self, test_name: str) -> str:
        """Extract test suite name from test file name"""
        # Remove common suffixes
        suite_name = test_name.replace('_test', '').replace('_enhanced', '')
        suite_name = suite_name.replace('_comprehensive', '')
        
        # Handle special cases
        if suite_name == 'enhanced_unit_tests':
            return 'Enhanced Unit Tests'
        elif suite_name == 'memory_tests':
            return 'Memory Management Tests'
        elif suite_name == 'thread_safety_tests':
            return 'Thread Safety Tests'
        elif suite_name == 'performance_tests':
            return 'Performance Tests'
        else:
            return suite_name.replace('_', ' ').title()
    
    def _parse_junit_report(self, root: ET.Element, test_name: str, suite_name: str) -> Dict:
        """Parse JUnit XML report into structured data"""
        suite_data = {
            'name': suite_name,
            'total': int(root.get('tests', 0)),
            'passed': int(root.get('tests', 0)) - int(root.get('failures', 0)) - int(root.get('errors', 0)),
            'failed': int(root.get('failures', 0)) + int(root.get('errors', 0)),
            'skipped': 0,  # JUnit doesn't have a standard skipped count
            'time': float(root.get('time', 0)),
            'test_cases': []
        }
        
        # Look for testcases in the report
        for testcase in root.findall('.//testcase'):
            test_case = {
                'name': testcase.get('name', ''),
                'classname': testcase.get('classname', ''),
                'time': float(testcase.get('time', 0)),
                'status': 'PASSED'
            }
            
            # Check for failure
            failure = testcase.find('failure')
            if failure is not None:
                test_case['status'] = 'FAILED'
                test_case['message'] = failure.get('message', '')
                test_case['failure_message'] = failure.text or ''
            
            suite_data['test_cases'].append(test_case)
        
        return suite_data
    
    def _update_summary(self, suite_data: Dict):
        """Update the overall test summary"""
        self.report_data['summary']['total_tests'] += suite_data['total']
        self.report_data['summary']['passed_tests'] += suite_data['passed']
        self.report_data['summary']['failed_tests'] += suite_data['failed']
        self.report_data['summary']['skipped_tests'] += suite_data['skipped']
        self.report_data['summary']['total_execution_time_ms'] += (suite_data['time'] * 1000)


def main():
    project_root = str(Path(__file__).parent.parent)
    report_dir = str(Path(project_root) / "test_reports")
    
    print("=== NLM Enhanced Test Report Generator ===")
    print(f"Project Root: {project_root}")
    print(f"Report Directory: {report_dir}")
    
    # Create report generator
    generator = NLMTestReportGenerator(project_root, report_dir)
    
    # Generate all reports
    reports = generator.generate_all_reports()
    
    print("\n=== Report Generation Summary ===")
    print("All reports have been generated successfully!")
    print("\nNext Steps:")
    print("1. Review the HTML report for detailed test results")
    print("2. Use the JUnit XML report for CI/CD integration")
    print("3. Analyze the JSON summary for programmatic access")
    print("4. Check coverage report for code coverage analysis")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
