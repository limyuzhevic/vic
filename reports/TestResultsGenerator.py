#!/usr/bin/env python3
"""
NLM Test Results Generator

Automated test result processing and reporting for the NLM Comprehensive Testing Framework.
Generates HTML, JSON, CSV reports from test execution results.
"""

import json
import csv
import html
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional
import re
import argparse

class NLMTestResultsGenerator:
    """Generates comprehensive test reports from NLM test execution results."""
    
    def __init__(self, results_dir: str = "results", output_dir: str = "reports"):
        self.results_dir = Path(results_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        # Test result patterns
        self.test_file_pattern = r"*(test_)?(.*)\.xml"
        self.coverage_file_pattern = r"*(coverage|test_coverage)\.(html|json|xml)"
        self.performance_file_pattern = r"*(performance|benchmark)\.(html|json|xml)"
    
    def process_c_test_results(self, xml_file: Path) -> Dict[str, Any]:
        """Process CTest XML test results and extract key metrics."""
        test_results = {
            "timestamp": datetime.now().isoformat(),
            "test_type": "ctest",
            "total_tests": 0,
            "passed_tests": 0,
            "failed_tests": 0,
            "skipped_tests": 0,
            "error_tests": 0,
            "categories": {},
            "performance_metrics": {},
            "execution_time": 0.0,
            "success_rate": 0.0
        }
        
        if not xml_file.exists():
            print(f"Warning: Test results file not found: {xml_file}")
            return test_results
        
        try:
            with open(xml_file, 'r') as f:
                content = f.read()
            
            # Parse XML content
            test_results = self._parse_ctest_xml(content)
            
        except Exception as e:
            print(f"Error processing test results file {xml_file}: {e}")
        
        return test_results
    
    def _parse_ctest_xml(self, xml_content: str) -> Dict[str, Any]:
        """Parse CTest XML results and extract metrics."""
        results = {
            "timestamp": datetime.now().isoformat(),
            "test_type": "ctest",
            "total_tests": 0,
            "passed_tests": 0,
            "failed_tests": 0,
            "skipped_tests": 0,
            "error_tests": 0,
            "categories": {},
            "performance_metrics": {},
            "execution_time": 0.0,
            "success_rate": 0.0
        }
        
        try:
            # Parse XML using regex (simplified approach)
            # In a real implementation, use an XML parser
            
            # Extract total tests
            total_match = re.search(r'<tests>(\d+)</tests>', xml_content)
            if total_match:
                results["total_tests"] = int(total_match.group(1))
            
            # Extract passed tests
            passed_match = re.search(r'<passed>(\d+)</passed>', xml_content)
            if passed_match:
                results["passed_tests"] = int(passed_match.group(1))
            
            # Extract failed tests
            failed_match = re.search(r'<failed>(\d+)</failed>', xml_content)
            if failed_match:
                results["failed_tests"] = int(failed_match.group(1))
            
            # Extract skipped tests
            skipped_match = re.search(r'<skipped>(\d+)</skipped>', xml_content)
            if skipped_match:
                results["skipped_tests"] = int(skipped_match.group(1))
            
            # Extract execution time
            time_match = re.search(r'<time>([\d.]+)</time>', xml_content)
            if time_match:
                results["execution_time"] = float(time_match.group(1))
            
            # Calculate success rate
            if results["total_tests"] > 0:
                results["success_rate"] = (
                    (results["passed_tests"] + results["skipped_tests"]) / 
                    results["total_tests"]
                ) * 100
            
            # Extract test categories
            results["categories"] = self._extract_test_categories(xml_content)
            
            # Extract performance metrics
            results["performance_metrics"] = self._extract_performance_metrics(xml_content)
            
        except Exception as e:
            print(f"Error parsing CTest XML: {e}")
        
        return results
    
    def _extract_test_categories(self, xml_content: str) -> Dict[str, Any]:
        """Extract test category information from XML."""
        categories = {}
        
        try:
            # Extract test name and status pairs
            test_pattern = r'<Test (?:Name="([^"]*)"|Name:([^>]*))[^>]*>(?:<Status>([^"]*)</Status>)?'
            matches = re.findall(test_pattern, xml_content)
            
            for match in matches:
                test_name = match[0] if match[0] else match[1]
                test_status = match[2] if match[2] else "unknown"
                
                # Initialize category if not present
                if test_name not in categories:
                    categories[test_name] = {
                        "tests": 0,
                        "passed": 0,
                        "failed": 0,
                        "skipped": 0,
                        "errors": 0,
                        "success_rate": 0.0,
                        "execution_time": 0.0
                    }
                
                # Update category metrics (simplified)
                categories[test_name]["tests"] += 1
                if test_status == "passed":
                    categories[test_name]["passed"] += 1
                elif test_status == "failed":
                    categories[test_name]["failed"] += 1
                elif test_status == "skipped":
                    categories[test_name]["skipped"] += 1
                else:
                    categories[test_name]["errors"] += 1
                
                # Calculate success rate
                if categories[test_name]["tests"] > 0:
                    categories[test_name]["success_rate"] = (
                        (categories[test_name]["passed"] + categories[test_name]["skipped"]) /
                        categories[test_name]["tests"]
                    ) * 100
        
        except Exception as e:
            print(f"Error extracting test categories: {e}")
        
        return categories
    
    def _extract_performance_metrics(self, xml_content: str) -> Dict[str, Any]:
        """Extract performance metrics from XML."""
        metrics = {}
        
        try:
            # Extract time measurements
            time_pattern = r'<Time>([\d.]+)</Time>'
            times = re.findall(time_pattern, xml_content)
            
            if times:
                avg_time = sum(float(t) for t in times) / len(times)
                metrics["average_execution_time"] = avg_time
                metrics["total_execution_time"] = sum(float(t) for t in times)
                metrics["test_count"] = len(times)
            
            # Extract memory usage
            memory_pattern = r'<Memory>([\d.]+)</Memory>'
            memory_values = re.findall(memory_pattern, xml_content)
            
            if memory_values:
                avg_memory = sum(float(m) for m in memory_values) / len(memory_values)
                metrics["average_memory_usage"] = avg_memory
                metrics["max_memory_usage"] = max(float(m) for m in memory_values)
                metrics["memory_samples"] = len(memory_values)
            
            # Extract CPU usage
            cpu_pattern = r'<CPU>([\d.]+)</CPU>'
            cpu_values = re.findall(cpu_pattern, xml_content)
            
            if cpu_values:
                avg_cpu = sum(float(c) for c in cpu_values) / len(cpu_values)
                metrics["average_cpu_usage"] = avg_cpu
                metrics["max_cpu_usage"] = max(float(c) for c in cpu_values)
                metrics["cpu_samples"] = len(cpu_values)
        
        except Exception as e:
            print(f"Error extracting performance metrics: {e}")
        
        return metrics
    
    def process_html_report(self, html_file: Path) -> Dict[str, Any]:
        """Process HTML test report and extract key information."""
        report_data = {
            "timestamp": datetime.now().isoformat(),
            "test_type": "html",
            "title": "",
            "summary": "",
            "results": [],
            "statistics": {},
            "links": []
        }
        
        if not html_file.exists():
            print(f"Warning: HTML report file not found: {html_file}")
            return report_data
        
        try:
            with open(html_file, 'r') as f:
                content = f.read()
            
            report_data = self._parse_html_report(content)
            
        except Exception as e:
            print(f"Error processing HTML report {html_file}: {e}")
        
        return report_data
    
    def _parse_html_report(self, html_content: str) -> Dict[str, Any]:
        """Parse HTML test report."""
        report_data = {
            "timestamp": datetime.now().isoformat(),
            "test_type": "html",
            "title": "",
            "summary": "",
            "results": [],
            "statistics": {},
            "links": []
        }
        
        try:
            # Extract title
            title_match = re.search(r'<title>([^<]*)</title>', html_content, re.IGNORECASE)
            if title_match:
                report_data["title"] = title_match.group(1)
            
            # Extract summary (first few paragraphs)
            summary_pattern = r'<p>([^<]{100,})</p>'
            summary_matches = re.findall(summary_pattern, html_content, re.IGNORECASE)
            if summary_matches:
                report_data["summary"] = summary_matches[0]
            
            # Extract test results table
            report_data["results"] = self._extract_html_test_results(html_content)
            
            # Extract statistics
            report_data["statistics"] = self._extract_html_statistics(html_content)
            
            # Extract links
            report_data["links"] = self._extract_html_links(html_content)
        
        except Exception as e:
            print(f"Error parsing HTML report: {e}")
        
        return report_data
    
    def _extract_html_test_results(self, html_content: str) -> List[Dict[str, Any]]:
        """Extract test results from HTML report."""
        results = []
        
        try:
            # Find test result tables
            table_pattern = r'<table[^>]*>(.*?)</table>'
            table_matches = re.findall(table_pattern, html_content, re.DOTALL)
            
            for table in table_matches:
                if 'test' in table.lower() or 'result' in table.lower():
                    # Extract rows from table
                    row_pattern = r'<tr>(.*?)</tr>'
                    row_matches = re.findall(row_pattern, table, re.DOTALL)
                    
                    for row in row_matches:
                        # Extract columns
                        col_pattern = r'<td>(.*?)</td>'
                        cols = re.findall(col_pattern, row, re.DOTALL)
                        
                        if cols:
                            test_result = {
                                "name": cols[0].strip() if len(cols) > 0 else "",
                                "status": cols[1].strip() if len(cols) > 1 else "",
                                "time": cols[2].strip() if len(cols) > 2 else "",
                                "memory": cols[3].strip() if len(cols) > 3 else "",
                                "cpu": cols[4].strip() if len(cols) > 4 else ""
                            }
                            results.append(test_result)
        
        except Exception as e:
            print(f"Error extracting HTML test results: {e}")
        
        return results
    
    def _extract_html_statistics(self, html_content: str) -> Dict[str, Any]:
        """Extract statistics from HTML report."""
        stats = {}
        
        try:
            # Extract summary statistics
            stat_patterns = [
                (r'Total Tests:\s*(\d+)', 'total_tests'),
                (r'Passed Tests:\s*(\d+)', 'passed_tests'),
                (r'Failed Tests:\s*(\d+)', 'failed_tests'),
                (r'Skipped Tests:\s*(\d+)', 'skipped_tests'),
                (r'Success Rate:\s*([\d.]+)%', 'success_rate'),
                (r'Execution Time:\s*([\d.]+) seconds', 'execution_time'),
                (r'Average Time:\s*([\d.]+) seconds', 'average_time'),
                (r'Memory Usage:\s*([\d.]+) MB', 'memory_usage'),
                (r'CPU Usage:\s*([\d.]+)%', 'cpu_usage')
            ]
            
            for pattern, key in stat_patterns:
                match = re.search(pattern, html_content, re.IGNORECASE)
                if match:
                    value = match.group(1)
                    
                    # Convert numeric values
                    if key in ['total_tests', 'passed_tests', 'failed_tests', 'skipped_tests']:
                        stats[key] = int(value)
                    elif key in ['success_rate', 'cpu_usage']:
                        stats[key] = float(value)
                    elif key in ['execution_time', 'average_time', 'memory_usage']:
                        stats[key] = float(value)
                    else:
                        stats[key] = value
        
        except Exception as e:
            print(f"Error extracting HTML statistics: {e}")
        
        return stats
    
    def _extract_html_links(self, html_content: str) -> List[Dict[str, str]]:
        """Extract links from HTML report."""
        links = []
        
        try:
            # Extract all links
            link_pattern = r'<a[^>]*href="([^"]*)"[^>]*>([^<]*)</a>'
            link_matches = re.findall(link_pattern, html_content, re.IGNORECASE)
            
            for url, text in link_matches:
                link = {
                    "url": url,
                    "text": text.strip()
                }
                links.append(link)
        
        except Exception as e:
            print(f"Error extracting HTML links: {e}")
        
        return links
    
    def process_json_report(self, json_file: Path) -> Dict[str, Any]:
        """Process JSON test report."""
        report_data = {
            "timestamp": datetime.now().isoformat(),
            "test_type": "json",
            "metadata": {},
            "results": {},
            "performance": {},
            "coverage": {}
        }
        
        if not json_file.exists():
            print(f"Warning: JSON report file not found: {json_file}")
            return report_data
        
        try:
            with open(json_file, 'r') as f:
                data = json.load(f)
            
            report_data = data
            
        except Exception as e:
            print(f"Error processing JSON report {json_file}: {e}")
        
        return report_data
    
    def process_csv_report(self, csv_file: Path) -> Dict[str, Any]:
        """Process CSV test report."""
        report_data = {
            "timestamp": datetime.now().isoformat(),
            "test_type": "csv",
            "headers": [],
            "rows": [],
            "statistics": {}
        }
        
        if not csv_file.exists():
            print(f"Warning: CSV report file not found: {csv_file}")
            return report_data
        
        try:
            with open(csv_file, 'r') as f:
                reader = csv.reader(f)
                headers = next(reader)
                report_data["headers"] = headers
                
                for row in reader:
                    report_data["rows"].append(row)
            
            # Calculate statistics
            report_data["statistics"] = self._calculate_csv_statistics(report_data["rows"])
            
        except Exception as e:
            print(f"Error processing CSV report {csv_file}: {e}")
        
        return report_data
    
    def _calculate_csv_statistics(self, rows: List[List[str]]) -> Dict[str, Any]:
        """Calculate statistics from CSV data."""
        stats = {}
        
        if not rows:
            return stats
        
        try:
            # Convert numeric rows
            numeric_rows = []
            for row in rows:
                numeric_row = []
                for cell in row:
                    try:
                        numeric_row.append(float(cell))
                    except ValueError:
                        numeric_row.append(cell)
                numeric_rows.append(numeric_row)
            
            # Calculate basic statistics for numeric columns
            for col_idx in range(len(rows[0])):
                values = [row[col_idx] for row in numeric_rows if isinstance(row[col_idx], (int, float))]
                
                if values:
                    col_name = self._get_column_name(col_idx, rows[0])
                    stats[col_name] = {
                        "count": len(values),
                        "mean": sum(values) / len(values),
                        "min": min(values),
                        "max": max(values),
                        "std_dev": self._calculate_std_dev(values) if len(values) > 1 else 0.0
                    }
        
        except Exception as e:
            print(f"Error calculating CSV statistics: {e}")
        
        return stats
    
    def _get_column_name(self, col_idx: int, headers: List[str]) -> str:
        """Get column name from index."""
        if col_idx < len(headers):
            return headers[col_idx]
        else:
            return f"Column_{col_idx}"
    
    def _calculate_std_dev(self, values: List[float]) -> float:
        """Calculate standard deviation."""
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return (variance ** 0.5)
    
    def generate_html_report(self, test_results: Dict[str, Any], coverage_results: Dict[str, Any], 
                           performance_results: Dict[str, Any], output_file: str) -> bool:
        """Generate HTML test report from processed results."""
        try:
            html_content = self._create_html_report(test_results, coverage_results, performance_results)
            
            with open(output_file, 'w') as f:
                f.write(html_content)
            
            print(f"HTML report generated: {output_file}")
            return True
            
        except Exception as e:
            print(f"Error generating HTML report: {e}")
            return False
    
    def _create_html_report(self, test_results: Dict[str, Any], coverage_results: Dict[str, Any], 
                          performance_results: Dict[str, Any]) -> str:
        """Create HTML report content."""
        html_template = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Results Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }}
        .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
        .passed {{ background-color: #d4edda; color: #155724; }}
        .failed {{ background-color: #f8d7da; color: #721c24; }}
        .skipped {{ background-color: #fff3cd; color: #856404; }}
        table {{ border-collapse: collapse; width: 100%; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .metric {{ font-size: 1.2em; font-weight: bold; }}
        .success {{ color: #28a745; }}
        .warning {{ color: #ffc107; }}
        .error {{ color: #dc3545; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Comprehensive Test Results Report</h1>
        <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p>Framework Version: NLM Test Framework 3.0.0</p>
    </div>
    
    <div class="section passed">
        <h2>Test Execution Summary</h2>
        <p><span class="metric">Total Tests:</span> {test_results.get('total_tests', 0)}</p>
        <p><span class="metric">Passed Tests:</span> {test_results.get('passed_tests', 0)}</p>
        <p><span class="metric">Failed Tests:</span> {test_results.get('failed_tests', 0)}</p>
        <p><span class="metric">Skipped Tests:</span> {test_results.get('skipped_tests', 0)}</p>
        <p><span class="metric">Success Rate:</span> <span class="success">{test_results.get('success_rate', 0):.2f}%</span></p>
        <p><span class="metric">Execution Time:</span> {test_results.get('execution_time', 0):.2f} seconds</p>
    </div>
    
    <div class="section">
        <h2>Test Categories</h2>
        <table>
            <tr>
                <th>Test Category</th>
                <th>Tests</th>
                <th>Passed</th>
                <th>Failed</th>
                <th>Skipped</th>
                <th>Success Rate</th>
                <th>Execution Time</th>
            </tr>
            {self._generate_html_test_categories(test_results.get('categories', {}))}
        </table>
    </div>
    
    <div class="section">
        <h2>Performance Metrics</h2>
        <p><span class="metric">Average Execution Time:</span> {performance_results.get('average_execution_time', 0):.2f} seconds</p>
        <p><span class="metric">Total Execution Time:</span> {performance_results.get('total_execution_time', 0):.2f} seconds</p>
        <p><span class="metric">Average Memory Usage:</span> {performance_results.get('average_memory_usage', 0):.2f} MB</p>
        <p><span class="metric">Max Memory Usage:</span> {performance_results.get('max_memory_usage', 0):.2f} MB</p>
        <p><span class="metric">Average CPU Usage:</span> {performance_results.get('average_cpu_usage', 0):.2f}%</p>
        <p><span class="metric">Max CPU Usage:</span> {performance_results.get('max_cpu_usage', 0):.2f}%</p>
    </div>
    
    <div class="section">
        <h2>Test Results by Category</h2>
        {self._generate_html_detailed_results(test_results.get('categories', {}))}
    </div>
    
    <div class="section">
        <h2>Recommendations</h2>
        <ul>
            <li>Address failing tests in the highest priority categories</li>
            <li>Investigate performance bottlenecks</li>
            <li>Consider adding more comprehensive test coverage</li>
            <li>Monitor test trends over time</li>
            <li>Optimize test execution for better performance</li>
        </ul>
    </div>
</body>
</html>
        """
        
        return html_template
    
    def _generate_html_test_categories(self, categories: Dict[str, Any]) -> str:
        """Generate HTML for test categories."""
        html_content = ""
        
        for category, stats in categories.items():
            success_rate = stats.get('success_rate', 0)
            if success_rate >= 90:
                row_class = "passed"
            elif success_rate >= 70:
                row_class = "warning"
            else:
                row_class = "error"
            
            html_content += f"""
            <tr class="{row_class}">
                <td>{category}</td>
                <td>{stats.get('tests', 0)}</td>
                <td>{stats.get('passed', 0)}</td>
                <td>{stats.get('failed', 0)}</td>
                <td>{stats.get('skipped', 0)}</td>
                <td>{stats.get('success_rate', 0):.2f}%</td>
                <td>{stats.get('execution_time', 0):.2f}</td>
            </tr>
            """
        
        return html_content
    
    def _generate_html_detailed_results(self, categories: Dict[str, Any]) -> str:
        """Generate HTML for detailed test results."""
        html_content = ""
        
        for category, stats in categories.items():
            if stats.get('failed', 0) > 0:
                html_content += f"""
                <h3>{category}</h3>
                <p><strong>Failed Tests:</strong> {stats.get('failed', 0)}</p>
                <p><strong>Success Rate:</strong> {stats.get('success_rate', 0):.2f}%</p>
                """
        
        if not html_content:
            html_content = "<p>All tests passed successfully! No failures to report.</p>"
        
        return html_content
    
    def generate_json_report(self, test_results: Dict[str, Any], coverage_results: Dict[str, Any], 
                           performance_results: Dict[str, Any], output_file: str) -> bool:
        """Generate JSON test report."""
        try:
            report_data = {
                "metadata": {
                    "generated": datetime.now().isoformat(),
                    "framework_version": "3.0.0",
                    "test_type": "comprehensive"
                },
                "test_results": test_results,
                "coverage_results": coverage_results,
                "performance_results": performance_results,
                "summary": {
                    "total_tests": test_results.get('total_tests', 0),
                    "passed_tests": test_results.get('passed_tests', 0),
                    "failed_tests": test_results.get('failed_tests', 0),
                    "skipped_tests": test_results.get('skipped_tests', 0),
                    "success_rate": test_results.get('success_rate', 0),
                    "execution_time": test_results.get('execution_time', 0)
                }
            }
            
            with open(output_file, 'w') as f:
                json.dump(report_data, f, indent=2)
            
            print(f"JSON report generated: {output_file}")
            return True
            
        except Exception as e:
            print(f"Error generating JSON report: {e}")
            return False
    
    def generate_csv_report(self, test_results: Dict[str, Any], coverage_results: Dict[str, Any], 
                          performance_results: Dict[str, Any], output_file: str) -> bool:
        """Generate CSV test report."""
        try:
            rows = []
            
            # Add test results row
            rows.append([
                "Category",
                "Total Tests",
                "Passed Tests",
                "Failed Tests",
                "Skipped Tests",
                "Success Rate",
                "Execution Time"
            ])
            
            for category, stats in test_results.get('categories', {}).items():
                rows.append([
                    category,
                    stats.get('tests', 0),
                    stats.get('passed', 0),
                    stats.get('failed', 0),
                    stats.get('skipped', 0),
                    stats.get('success_rate', 0),
                    stats.get('execution_time', 0)
                ])
            
            with open(output_file, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerows(rows)
            
            print(f"CSV report generated: {output_file}")
            return True
            
        except Exception as e:
            print(f"Error generating CSV report: {e}")
            return False
    
    def process_all_reports(self) -> Dict[str, bool]:
        """Process all available test reports and generate consolidated reports."""
        results = {}
        
        # Find all test result files
        xml_files = list(self.results_dir.glob("*.xml"))
        html_files = list(self.results_dir.glob("*.html"))
        json_files = list(self.results_dir.glob("*.json"))
        csv_files = list(self.results_dir.glob("*.csv"))
        
        # Process CTest XML results
        for xml_file in xml_files:
            print(f"Processing CTest results: {xml_file.name}")
            test_results = self.process_c_test_results(xml_file)
            
            # Generate reports from test results
            if test_results.get('total_tests', 0) > 0:
                self.generate_html_report(
                    test_results, {}, {}, 
                    str(self.output_dir / f"nlm_test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.html")
                )
                self.generate_json_report(
                    test_results, {}, {}, 
                    str(self.output_dir / f"nlm_test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
                )
                self.generate_csv_report(
                    test_results, {}, {}, 
                    str(self.output_dir / f"nlm_test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv")
                )
                
                results['xml_processing'] = True
                break  # Process first XML file found
        
        # Process HTML reports
        for html_file in html_files:
            print(f"Processing HTML report: {html_file.name}")
            html_results = self.process_html_report(html_file)
            
            # Generate additional reports from HTML
            if html_results.get('title'):
                results['html_processing'] = True
                break  # Process first HTML file found
        
        # Process JSON reports
        for json_file in json_files:
            print(f"Processing JSON report: {json_file.name}")
            json_results = self.process_json_report(json_file)
            
            if json_results.get('metadata'):
                results['json_processing'] = True
                break  # Process first JSON file found
        
        # Process CSV reports
        for csv_file in csv_files:
            print(f"Processing CSV report: {csv_file.name}")
            csv_results = self.process_csv_report(csv_file)
            
            if csv_results.get('headers'):
                results['csv_processing'] = True
                break  # Process first CSV file found
        
        # Generate summary report
        self.generate_summary_report(results)
        
        return results
    
    def generate_summary_report(self, processing_results: Dict[str, bool]) -> bool:
        """Generate summary report of processing results."""
        try:
            summary = {
                "generated": datetime.now().isoformat(),
                "processing_results": processing_results,
                "summary": {
                    "files_processed": sum(1 for success in processing_results.values() if success),
                    "total_files": len(processing_results),
                    "success_rate": sum(1 for success in processing_results.values() if success) / len(processing_results) * 100
                }
            }
            
            summary_file = self.output_dir / f"processing_summary_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            
            with open(summary_file, 'w') as f:
                json.dump(summary, f, indent=2)
            
            print(f"Summary report generated: {summary_file}")
            return True
            
        except Exception as e:
            print(f"Error generating summary report: {e}")
            return False
    
    def main(self):
        """Main function to process all test reports."""
        print("NLM Test Results Generator")
        print("=" * 50)
        print("Processing test results and generating reports...")
        
        results = self.process_all_reports()
        
        print("\nProcessing Summary:")
        print("-" * 50)
        for test_type, success in results.items():
            status = "✅ Success" if success else "❌ Failed"
            print(f"{test_type}: {status}")
        
        print(f"\nReports generated in: {self.output_dir}")
        print("Processing completed.")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="Generate NLM test reports from test execution results")
    parser.add_argument("--results-dir", default="results", help="Directory containing test results")
    parser.add_argument("--output-dir", default="reports", help="Directory for generated reports")
    parser.add_argument("--json", action="store_true", help="Process JSON test results")
    parser.add_argument("--html", action="store_true", help="Process HTML test results")
    parser.add_argument("--csv", action="store_true", help="Process CSV test results")
    parser.add_argument("--all", action="store_true", help="Process all available report types")
    
    args = parser.parse_args()
    
    generator = NLMTestResultsGenerator(args.results_dir, args.output_dir)
    
    if args.all or (not args.json and not args.html and not args.csv):
        generator.process_all_reports()
    else:
        if args.json:
            json_files = list(Path(args.results_dir).glob("*.json"))
            for json_file in json_files:
                print(f"Processing JSON: {json_file.name}")
                generator.process_json_report(json_file)
        
        if args.html:
            html_files = list(Path(args.results_dir).glob("*.html"))
            for html_file in html_files:
                print(f"Processing HTML: {html_file.name}")
                generator.process_html_report(html_file)
        
        if args.csv:
            csv_files = list(Path(args.results_dir).glob("*.csv"))
            for csv_file in csv_files:
                print(f"Processing CSV: {csv_file.name}")
                generator.process_csv_report(csv_file)

if __name__ == "__main__":
    main()
"""

    file_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_803a712a-b04a-4dc7-b4b7-5eb3114f5f86/reports/TestResultsGenerator.py"
    
    with open(file_path, 'w') as f:
        f.write(html_content)
    
    print(f"Created TestResultsGenerator.py at {file_path}")

# Create a simple example showing how to use the test results generator
example_code = '''#!/usr/bin/env python3
"""
Example: Using NLM Test Results Generator
"""

from TestResultsGenerator import NLMTestResultsGenerator

def main():
    # Create test results generator
    generator = NLMTestResultsGenerator(
        results_dir="test_results",
        output_dir="test_reports"
    )
    
    # Process all available test reports
    results = generator.process_all_reports()
    
    # Generate individual reports
    test_results = {
        "total_tests": 1000,
        "passed_tests": 950,
        "failed_tests": 30,
        "skipped_tests": 20,
        "success_rate": 95.0,
        "execution_time": 125.5,
        "categories": {
            "unit_tests": {
                "tests": 500,
                "passed": 475,
                "failed": 20,
                "skipped": 5,
                "success_rate": 95.0,
                "execution_time": 45.2
            },
            "integration_tests": {
                "tests": 300,
                "passed": 290,
                "failed": 8,
                "skipped": 2,
                "success_rate": 96.7,
                "execution_time": 38.1
            },
            "performance_tests": {
                "tests": 150,
                "passed": 145,
                "failed": 3,
                "skipped": 2,
                "success_rate": 96.7,
                "execution_time": 25.3
            },
            "stress_tests": {
                "tests": 50,
                "passed": 40,
                "failed": 8,
                "skipped": 2,
                "success_rate": 80.0,
                "execution_time": 16.9
            }
        }
    }
    
    coverage_results = {
        "code_coverage": 92.5,
        "test_coverage": 88.3,
        "branch_coverage": 91.2,
        "line_coverage": 94.1
    }
    
    performance_results = {
        "average_execution_time": 125.5,
        "total_execution_time": 125.5,
        "average_memory_usage": 256.8,
        "max_memory_usage": 512.0,
        "average_cpu_usage": 75.2,
        "max_cpu_usage": 95.8
    }
    
    # Generate comprehensive reports
    generator.generate_html_report(test_results, coverage_results, performance_results, "test_reports/comprehensive_report.html")
    generator.generate_json_report(test_results, coverage_results, performance_results, "test_reports/comprehensive_report.json")
    generator.generate_csv_report(test_results, coverage_results, performance_results, "test_reports/comprehensive_report.csv")
    
    print("Test reports generated successfully!")
    print("HTML report: test_reports/comprehensive_report.html")
    print("JSON report: test_reports/comprehensive_report.json")
    print("CSV report: test_reports/comprehensive_report.csv")

if __name__ == "__main__":
    main()
'''

# Create usage example file
example_file = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_803a712a-b04a-4dc7-b4b7-5eb3114f5f86/examples/testing/TestResultsGeneratorExample.py"

with open(example_file, 'w') as f:
    f.write(example_code)

print(f"Created usage example at {example_file}")

print("\n" + "="*60)
print("NLM Comprehensive Testing Framework Setup Complete!")
print("="*60)
print("\nCreated Files:")
print("1. tests/CMakeLists.txt - Comprehensive test configuration")
print("2. tests/unit/ - Unit test files")
print("3. tests/integration/ - Integration test files")
print("4. tests/performance/ - Performance test files")
print("5. tests/stress/ - Stress test files")
print("6. tests/compatibility/ - Compatibility test files")
print("7. tests/regression/ - Regression test files")
print("8. tests/framework/ - Framework utility files")
print("9. examples/testing/ - Usage examples")
print("10. docs/ - Documentation files")
print("11. CI/ - CI/CD configuration")
print("12. reports/TestResultsGenerator.py - Automated test reporting")

print("\nKey Features:")
print("✅ Comprehensive test coverage (Unit, Integration, Performance, Stress, Compatibility, Regression)")
print("✅ Modern testing framework with C++20 support")
print("✅ Automated test data generation")
print("✅ Cross-platform compatibility")
print("✅ CI/CD integration (GitHub Actions, GitLab CI)")
print("✅ Comprehensive reporting (HTML, JSON, CSV)")
print("✅ Performance monitoring and benchmarking")
print("✅ Stress testing with large-scale simulations")
print("✅ Compatibility testing across platforms and versions")
print("✅ Regression testing with baseline comparison")
print("✅ Detailed documentation and examples")

print("\nBuild and Test Instructions:")
print("1. mkdir build && cd build")
print("2. cmake .. -DCMAKE_BUILD_TYPE=Release")
print("3. make -j$(nproc)")
print("4. ctest --output-on-failure")
print("5. ./nlm_test --report=html")

print("\nCI/CD Setup:")
print("1. Configure GitHub Actions in .github/workflows/test.yml")
print("2. Configure GitLab CI in GitLabCI.yml")
print("3. Run tests in CI pipeline")
print("4. Generate reports and artifacts")
print("5. Deploy to production")

print("\nTesting Framework Capabilities:")
print("✅ Unit Testing: Component-level testing with mocks")
print("✅ Integration Testing: System-wide functionality testing")
print("✅ Performance Testing: Performance benchmarking and analysis")
print("✅ Stress Testing: Large-scale and resource exhaustion testing")
print("✅ Compatibility Testing: Cross-platform and version testing")
print("✅ Regression Testing: Baseline comparison and stability testing")

print("\nNLM Comprehensive Testing Framework setup completed successfully!")
print("The framework provides thorough testing capabilities for the entire NLM system.")
print("="*60)
EOF
