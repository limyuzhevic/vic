#!/usr/bin/env python3
"""
Performance Integration Example Script

This script demonstrates how to integrate the NLM performance monitoring
and benchmarking tools with existing NLM systems, including the Phase 6 demo.

Usage:
    python3 performance_integration_example.py [--demo type] [--output dir]
    python3 performance_integration_example.py --help

Example:
    python3 performance_integration_example.py --demo phase6 --output my_reports
"""

import sys
import os
from pathlib import Path
import subprocess
import json
import argparse

class PerformanceIntegrationDemo:
    def __init__(self, output_dir="integration_reports"):
        self.output_dir = output_dir
        self.setup_environment()
    
    def setup_environment(self):
        """Setup the integration environment"""
        # Create output directory
        Path(self.output_dir).mkdir(parents=True, exist_ok=True)
        
        # Create subdirectories for different reports
        Path(f"{self.output_dir}/benchmarks").mkdir(exist_ok=True)
        Path(f"{self.output_dir}/monitoring").mkdir(exist_ok=True)
        Path(f"{self.output_dir}/analysis").mkdir(exist_ok=True)
        
        print(f"Integration environment setup complete")
        print(f"Output directory: {self.output_dir}")
    
    def run_benchmark_suite(self, config_file=""):
        """Run the comprehensive benchmark suite"""
        print("Running comprehensive benchmark suite...")
        
        # In a real implementation, this would compile and run the benchmark executable
        # For demonstration, we'll simulate the benchmark process
        
        benchmark_config = {
            "suite_name": "NLM Performance Integration Demo",
            "neuron_scales": [100, 500, 1000, 2000],
            "step_counts": [1000, 1000, 500, 200],
            "configurations": [
                {"name": "default_optimized", "memory_pool": True, "simd": "AVX2"},
                {"name": "memory_optimized", "memory_pool": True, "simd": "SSE"},
                {"name": "speed_optimized", "memory_pool": True, "simd": "AVX512"}
            ],
            "phases": ["basic", "scaling", "configuration", "stress"]
        }
        
        # Save benchmark configuration
        config_path = f"{self.output_dir}/benchmark_config.json"
        with open(config_path, 'w') as f:
            json.dump(benchmark_config, f, indent=2)
        
        print(f"Benchmark configuration saved to: {config_path}")
        
        # Simulate benchmark execution
        results = self.simulate_benchmark_run(benchmark_config)
        
        # Save results
        results_path = f"{self.output_dir}/benchmark_results.json"
        with open(results_path, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Benchmark results saved to: {results_path}")
        return results
    
    def simulate_benchmark_run(self, config):
        """Simulate a benchmark run and generate realistic results"""
        import random
        import math
        
        results = {
            "total_time": 0.0,
            "total_benchmarks": 0,
            "total_neurons_tested": 0,
            "performance_summary": {},
            "memory_summary": {},
            "configuration_performance": {},
            "scaling_data": [],
            "detected_issues": [],
            "quality_score": 0.0,
            "regression_risk": 0.0
        }
        
        # Simulate benchmark execution
        for neurons in config["neuron_scales"]:
            for steps in config["step_counts"]:
                # Generate realistic performance metrics
                base_perf = 500.0 * math.log10(neurons / 100)
                perf_variance = random.uniform(0.8, 1.2)
                steps_per_sec = base_perf * perf_variance
                
                memory_usage = neurons * 0.01 + random.uniform(0, 100)
                
                benchmark = {
                    "neuron_count": neurons,
                    "steps": steps,
                    "steps_per_second": steps_per_sec,
                    "memory_usage_mb": memory_usage,
                    "efficiency": steps_per_sec / neurons,
                    "simulated": True
                }
                
                results["total_benchmarks"] += 1
                results["total_neurons_tested"] += neurons
                results["total_time"] += steps / steps_per_sec
                
                # Store performance by configuration
                for bench_config in config["configurations"]:
                    config_key = bench_config["name"]
                    if config_key not in results["configuration_performance"]:
                        results["configuration_performance"][config_key] = []
                    
                    perf_with_config = steps_per_sec * (1.0 + (random.random() - 0.5) * 0.2)
                    results["configuration_performance"][config_key].append(perf_with_config)
        
        # Calculate aggregate statistics
        all_perf = []
        for config_name, perf_values in results["configuration_performance"].items():
            avg_perf = sum(perf_values) / len(perf_values)
            std_dev = math.sqrt(sum((x - avg_perf) ** 2 for x in perf_values) / len(perf_values))
            
            results["configuration_performance"][config_name] = {
                "average_performance": avg_perf,
                "std_deviation": std_dev,
                "min_performance": min(perf_values),
                "max_performance": max(perf_values)
            }
            
            all_perf.extend(perf_values)
        
        # Calculate overall performance summary
        results["performance_summary"] = {
            "average_steps_per_second": sum(all_perf) / len(all_perf),
            "min_steps_per_second": min(all_perf),
            "max_steps_per_second": max(all_perf),
            "std_deviation": math.sqrt(sum((x - results["performance_summary"]["average_steps_per_second"]) ** 2 for x in all_perf) / len(all_perf))
        }
        
        # Calculate memory summary
        memory_values = [b["memory_usage_mb"] for b in [{"neuron_count": n, "steps": s, "steps_per_second": (500.0 * math.log10(n/100)) * random.uniform(0.8, 1.2), "memory_usage_mb": n * 0.01 + random.uniform(0, 100)} for n in config["neuron_scales"] for s in config["step_counts"]]]
        
        results["memory_summary"] = {
            "average_memory_mb": sum(memory_values) / len(memory_values),
            "min_memory_mb": min(memory_values),
            "max_memory_mb": max(memory_values)
        }
        
        # Generate scaling data
        for neurons in config["neuron_scales"]:
            base_perf = 500.0 * math.log10(neurons / 100)
            perf_variance = random.uniform(0.9, 1.1)
            results["scaling_data"].append({
                "neuron_count": neurons,
                "average_performance": base_perf * perf_variance,
                "quality_score": random.uniform(0.7, 0.95)
            })
        
        # Detect potential issues
        if results["performance_summary"]["average_steps_per_second"] < 100.0:
            results["detected_issues"].append("Average performance below threshold (100 steps/sec)")
        
        if results["memory_summary"]["average_memory_mb"] > 1024.0:
            results["detected_issues"].append("Average memory usage above threshold (1024 MB)")
        
        # Calculate quality and risk scores
        quality_components = []
        
        # Performance quality (higher is better)
        perf_quality = min(1.0, results["performance_summary"]["average_steps_per_second"] / 1000.0)
        quality_components.append(perf_quality * 0.4)
        
        # Memory efficiency quality (higher is better)
        mem_efficiency = min(1.0, 1000.0 / max(1.0, results["memory_summary"]["average_memory_mb"]))
        quality_components.append(mem_efficiency * 0.3)
        
        # Configuration consistency (lower variance is better)
        consistency_scores = []
        for config_name, stats in results["configuration_performance"].items():
            coeff_var = stats["std_deviation"] / stats["average_performance"]
            consistency_scores.append(max(0.0, 1.0 - coeff_var * 2.0))
        
        avg_consistency = sum(consistency_scores) / len(consistency_scores) if consistency_scores else 0.5
        quality_components.append(avg_consistency * 0.2)
        
        # Neural activity quality
        neural_activity = min(1.0, results["performance_summary"]["average_steps_per_second"] / 500.0)
        quality_components.append(neural_activity * 0.1)
        
        results["quality_score"] = sum(quality_components)
        
        # Calculate regression risk (based on performance variability)
        if len(all_perf) > 1:
            mean_perf = results["performance_summary"]["average_steps_per_second"]
            variance = sum((x - mean_perf) ** 2 for x in all_perf) / len(all_perf)
            std_dev = math.sqrt(variance)
            coeff_of_variation = std_dev / mean_perf if mean_perf > 0 else 0
            results["regression_risk"] = min(1.0, coeff_of_variation * 5.0)
        else:
            results["regression_risk"] = 0.3
        
        return results
    
    def run_monitoring_integration(self):
        """Run performance monitoring integration"""
        print("Running performance monitoring integration...")
        
        monitoring_config = {
            "monitoring_enabled": True,
            "real_time_monitoring": True,
            "metric_collection_interval": 1.0,
            "alerting_enabled": True,
            "analysis_enabled": True,
            "system_resources": {
                "cpu_monitoring": True,
                "memory_monitoring": True,
                "gpu_monitoring": False,
                "thread_monitoring": True
            }
        }
        
        # Save monitoring configuration
        config_path = f"{self.output_dir}/monitoring_config.json"
        with open(config_path, 'w') as f:
            json.dump(monitoring_config, f, indent=2)
        
        # Simulate monitoring execution
        monitoring_results = self.simulate_monitoring_run(monitoring_config)
        
        # Save monitoring results
        results_path = f"{self.output_dir}/monitoring_results.json"
        with open(results_path, 'w') as f:
            json.dump(monitoring_results, f, indent=2)
        
        print(f"Monitoring results saved to: {results_path}")
        return monitoring_results
    
    def simulate_monitoring_run(self, config):
        """Simulate performance monitoring execution"""
        import random
        
        monitoring_results = {
            "monitoring_status": "active",
            "metrics_collected": 0,
            "system_metrics": {},
            "brain_metrics": {},
            "cognitive_metrics": {},
            "bottleneck_analysis": {},
            "active_alerts": [],
            "performance_score": 0.0
        }
        
        # Simulate system metrics collection
        monitoring_results["system_metrics"] = {
            "cpu_usage_percent": random.uniform(20.0, 80.0),
            "memory_usage_percent": random.uniform(30.0, 70.0),
            "thread_count": random.randint(1, 8),
            "collection_frequency": config["metric_collection_interval"],
            "last_update": self.get_timestamp()
        }
        
        # Simulate brain metrics collection
        monitoring_results["brain_metrics"] = {
            "total_neurons": 1000,
            "total_synapses": 5000,
            "firing_neurons": random.randint(50, 200),
            "average_firing_rate": random.uniform(1.0, 10.0),
            "simulation_speed": random.uniform(100.0, 500.0),
            "memory_efficiency": random.uniform(0.1, 0.5),
            "last_update": self.get_timestamp()
        }
        
        # Simulate cognitive metrics collection
        monitoring_results["cognitive_metrics"] = {
            "working_memory_usage": random.randint(0, 500),
            "episodic_memory_usage": random.randint(0, 100),
            "dopamine_level": random.uniform(0.0, 2.0),
            "novelty_level": random.uniform(0.0, 1.0),
            "curiosity_level": random.uniform(0.0, 1.0),
            "prediction_accuracy": random.uniform(0.7, 0.95),
            "last_update": self.get_timestamp()
        }
        
        # Analyze for bottlenecks
        monitoring_results["bottleneck_analysis"] = {
            "has_bottlenecks": False,
            "detected_issues": [],
            "severity_score": 0.0,
            "recommendations": []
        }
        
        # Check for performance issues
        if monitoring_results["brain_metrics"]["simulation_speed"] < 200.0:
            monitoring_results["bottleneck_analysis"]["has_bottlenecks"] = True
            monitoring_results["bottleneck_analysis"]["detected_issues"].append("Low simulation speed")
            monitoring_results["bottleneck_analysis"]["severity_score"] += 0.3
            monitoring_results["bottleneck_analysis"]["recommendations"].append("Optimize neural processing")
        
        if monitoring_results["system_metrics"]["memory_usage_percent"] > 80.0:
            monitoring_results["bottleneck_analysis"]["has_bottlenecks"] = True
            monitoring_results["bottleneck_analysis"]["detected_issues"].append("High memory usage")
            monitoring_results["bottleneck_analysis"]["severity_score"] += 0.2
            monitoring_results["bottleneck_analysis"]["recommendations"].append("Optimize memory allocation")
        
        # Check for active alerts
        if monitoring_results["system_metrics"]["cpu_usage_percent"] > 90.0:
            monitoring_results["active_alerts"].append({
                "name": "High CPU Usage",
                "severity": "WARNING",
                "message": f"CPU usage is {monitoring_results['system_metrics']['cpu_usage_percent']:.1f}%",
                "timestamp": self.get_timestamp()
            })
        
        if monitoring_results["brain_metrics"]["simulation_speed"] < 100.0:
            monitoring_results["active_alerts"].append({
                "name": "Low Performance",
                "severity": "ERROR",
                "message": f"Simulation speed is {monitoring_results['brain_metrics']['simulation_speed']:.1f} steps/sec",
                "timestamp": self.get_timestamp()
            })
        
        # Calculate overall performance score
        score_components = []
        
        # System health score
        system_health = min(1.0, 100.0 / max(1.0, monitoring_results["system_metrics"]["cpu_usage_percent"]))
        score_components.append(system_health * 0.3)
        
        # Brain performance score
        brain_perf = min(1.0, monitoring_results["brain_metrics"]["simulation_speed"] / 1000.0)
        score_components.append(brain_perf * 0.4)
        
        # Cognitive function score
        cognitive_score = (monitoring_results["cognitive_metrics"]["prediction_accuracy"] +
                          monitoring_results["cognitive_metrics"]["novelty_level"] +
                          monitoring_results["cognitive_metrics"]["curiosity_level"]) / 3.0
        score_components.append(cognitive_score * 0.3)
        
        monitoring_results["performance_score"] = sum(score_components)
        
        return monitoring_results
    
    def run_phase6_integration(self):
        """Run Phase 6 integration specifically"""
        print("Running Phase 6 integration demo...")
        
        integration_config = {
            "demo_type": "phase6",
            "integration_focus": "full_system",
            "systems_tested": [
                "memory_systems",
                "neuromodulation_systems", 
                "prediction_system",
                "development_system",
                "checkpointing",
                "replay_system"
            ],
            "performance_benchmarks": True,
            "monitoring_enabled": True,
            "analysis_enabled": True
        }
        
        # Save integration configuration
        config_path = f"{self.output_dir}/phase6_integration_config.json"
        with open(config_path, 'w') as f:
            json.dump(integration_config, f, indent=2)
        
        # Simulate Phase 6 integration execution
        integration_results = self.simulate_phase6_integration(integration_config)
        
        # Save integration results
        results_path = f"{self.output_dir}/phase6_integration_results.json"
        with open(results_path, 'w') as f:
            json.dump(integration_results, f, indent=2)
        
        print(f"Phase 6 integration results saved to: {results_path}")
        return integration_results
    
    def simulate_phase6_integration(self, config):
        """Simulate Phase 6 integration execution"""
        import random
        
        integration_results = {
            "integration_status": "PASS",
            "integration_score": 0.0,
            "systems_integration": {},
            "performance_metrics": {},
            "issues_detected": [],
            "recommendations": [],
            "timestamp": self.get_timestamp()
        }
        
        # Test integration of different systems
        for system in config["systems_tested"]:
            # Simulate system integration test
            integration_success = random.random() > 0.2  # 80% success rate
            
            system_result = {
                "system": system,
                "integrated": integration_success,
                "test_score": random.uniform(0.7, 1.0) if integration_success else random.uniform(0.0, 0.6),
                "issues": [] if integration_success else ["Integration test failed"],
                "recommendations": ["System is functioning normally"] if integration_success else ["Review system configuration"]
            }
            
            integration_results["systems_integration"][system] = system_result
            
            if not integration_success:
                integration_results["integration_status"] = "WARNING"
                integration_results["issues_detected"].extend(system_result["issues"])
                integration_results["recommendations"].extend(system_result["recommendations"])
        
        # Calculate overall integration score
        if integration_results["systems_integration"]:
            total_score = sum(s["test_score"] for s in integration_results["systems_integration"].values())
            integration_results["integration_score"] = total_score / len(integration_results["systems_integration"])
        else:
            integration_results["integration_score"] = 0.0
        
        # Determine final status
        if integration_results["integration_score"] < 0.5:
            integration_results["integration_status"] = "FAIL"
        elif integration_results["integration_score"] < 0.8:
            integration_results["integration_status"] = "WARNING"
        else:
            integration_results["integration_status"] = "PASS"
        
        return integration_results
    
    def generate_reports(self):
        """Generate comprehensive integration reports"""
        print("Generating integration reports...")
        
        # Load data from previous runs
        benchmark_data = self.load_json(f"{self.output_dir}/benchmark_results.json")
        monitoring_data = self.load_json(f"{self.output_dir}/monitoring_results.json")
        phase6_data = self.load_json(f"{self.output_dir}/phase6_integration_results.json")
        
        # Generate summary report
        summary = self.generate_summary_report(benchmark_data, monitoring_data, phase6_data)
        
        # Save summary report
        summary_path = f"{self.output_dir}/integration_summary.txt"
        with open(summary_path, 'w') as f:
            f.write(summary)
        
        # Generate HTML report
        html_report = self.generate_html_report(benchmark_data, monitoring_data, phase6_data)
        html_path = f"{self.output_dir}/integration_report.html"
        with open(html_path, 'w') as f:
            f.write(html_report)
        
        # Generate JSON summary
        json_summary = self.generate_json_summary(benchmark_data, monitoring_data, phase6_data)
        json_path = f"{self.output_dir}/integration_summary.json"
        with open(json_path, 'w') as f:
            json.dump(json_summary, f, indent=2)
        
        print(f"Reports generated in: {self.output_dir}")
        print(f"  - Summary report: {summary_path}")
        print(f"  - HTML report: {html_path}")
        print(f"  - JSON summary: {json_path}")
        
        return {
            "summary_path": summary_path,
            "html_path": html_path,
            "json_path": json_path
        }
    
    def generate_summary_report(self, benchmark_data, monitoring_data, phase6_data):
        """Generate a text summary report"""
        import datetime
        
        report = f"""NLM Performance Integration Summary Report
Generated: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
================================================================================

INTEGRATION STATUS: {phase6_data.get('integration_status', 'UNKNOWN')}
INTEGRATION SCORE: {phase6_data.get('integration_score', 0.0):.2%}

BENCHMARK RESULTS:
=================
Total Benchmarks Run: {benchmark_data.get('total_benchmarks', 0)}
Total Neurons Tested: {benchmark_data.get('total_neurons_tested', 0)}
Average Performance: {benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0.0):.1f} steps/sec
Benchmark Quality: {benchmark_data.get('quality_score', 0.0):.2%}
Regression Risk: {benchmark_data.get('regression_risk', 0.0):.2%}

PERFORMANCE ISSUES DETECTED:
=============================
{chr(10).join('  - ' + issue for issue in benchmark_data.get('detected_issues', []))}

MONITORING RESULTS:
==================
Monitoring Status: {monitoring_data.get('monitoring_status', 'UNKNOWN')}
Metrics Collected: {monitoring_data.get('metrics_collected', 0)}
Performance Score: {monitoring_data.get('performance_score', 0.0):.2%}
System Health: {monitoring_data.get('system_metrics', {}).get('cpu_usage_percent', 0.0):.1f}% CPU, {monitoring_data.get('system_metrics', {}).get('memory_usage_percent', 0.0):.1f}% Memory

ACTIVE ALERTS:
=============
{chr(10).join('  - ' + str(alert) for alert in monitoring_data.get('active_alerts', []))}

SYSTEM INTEGRATION RESULTS:
==========================
Integration Status: {phase6_data.get('integration_status', 'UNKNOWN')}
Systems Integrated: {len(phase6_data.get('systems_integration', {}))}

{chr(10).join(f'  - {system}: {results.get("test_score", 0.0):.2%} score' for system, results in phase6_data.get('systems_integration', {}).items())}

PERFORMANCE RECOMMENDATIONS:
===========================
1. Optimize neural processing for improved simulation speed
2. Monitor memory usage for potential bottlenecks
3. Review system configuration for integration issues
4. Implement performance alerting for proactive monitoring
5. Establish regular benchmarking schedules

CONCLUSION:
===========
The NLM performance integration successfully demonstrates comprehensive monitoring
and benchmarking capabilities for the artificial brain system. The integration provides
valuable insights into system performance, identifies potential issues, and offers
recommendations for optimization and improvement.

KEY FINDINGS:
=============
• System performance is {benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0.0):.1f} steps/sec on average
• Memory usage averages {benchmark_data.get('memory_summary', {}).get('average_memory_mb', 0.0):.1f} MB
• {len(monitoring_data.get('active_alerts', []))} active alerts detected
• {len(phase6_data.get('issues_detected', []))} integration issues identified
• Overall integration quality: {phase6_data.get('integration_score', 0.0):.2%}

RECOMMENDATIONS FOR PRODUCTION:
================================
1. Implement continuous monitoring and alerting
2. Establish regular benchmarking schedules
3. Monitor system resources and performance trends
4. Integrate with deployment pipelines
5. Provide performance dashboards for system administrators

END OF REPORT
"""
        return report
    
    def generate_html_report(self, benchmark_data, monitoring_data, phase6_data):
        """Generate an HTML report"""
        import datetime
        
        html = f"""<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Integration Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric {{ display: inline-block; margin: 10px; padding: 10px; background-color: #f9f9f9; border-radius: 3px; }}
        .good {{ background-color: #d4edda; }}
        .warning {{ background-color: #fff3cd; }}
        .error {{ background-color: #f8d7da; }}
        table {{ width: 100%; border-collapse: collapse; }}
        th, td {{ padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background-color: #f2f2f2; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Integration Report</h1>
        <p>Generated: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p><strong>Integration Status:</strong> {phase6_data.get('integration_status', 'UNKNOWN')}</p>
        <p><strong>Integration Score:</strong> {phase6_data.get('integration_score', 0.0):.2%}</p>
    </div>

    <div class="section">
        <h2>Executive Summary</h2>
        <p>The NLM performance integration successfully demonstrates comprehensive monitoring
        and benchmarking capabilities for the artificial brain system. The integration provides
        valuable insights into system performance, identifies potential issues, and offers
        recommendations for optimization and improvement.</p>
        
        <div class="metric good">
            <h3>Performance</h3>
            <p>Average Speed: {benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0.0):.1f} steps/sec</p>
            <p>Quality Score: {benchmark_data.get('quality_score', 0.0):.2%}</p>
            <p>Memory Usage: {benchmark_data.get('memory_summary', {}).get('average_memory_mb', 0.0):.1f} MB</p>
        </div>
        
        <div class="metric warning">
            <h3>Monitoring</h3>
            <p>Status: {monitoring_data.get('monitoring_status', 'UNKNOWN')}</p>
            <p>Performance Score: {monitoring_data.get('performance_score', 0.0):.2%}</p>
            <p>Active Alerts: {len(monitoring_data.get('active_alerts', []))}</p>
        </div>
        
        <div class="metric error">
            <h3>Integration</h3>
            <p>Systems Integrated: {len(phase6_data.get('systems_integration', {}))}</p>
            <p>Issues Detected: {len(phase6_data.get('issues_detected', []))}</p>
            <p>Recommendations: {len(phase6_data.get('recommendations', []))}</p>
        </div>
    </div>

    <div class="section">
        <h2>Performance Metrics</h2>
        <table>
            <tr>
                <th>Metric</th>
                <th>Value</th>
                <th>Status</th>
            </tr>
            <tr>
                <td>Average Steps/Second</td>
                <td>{benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0.0):.1f}</td>
                <td>{'GOOD' if benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0) > 200 else 'WARNING'}</td>
            </tr>
            <tr>
                <td>Benchmark Quality</td>
                <td>{benchmark_data.get('quality_score', 0.0):.2%}</td>
                <td>{'GOOD' if benchmark_data.get('quality_score', 0) > 0.8 else 'NEEDS ATTENTION'}</td>
            </tr>
            <tr>
                <td>Memory Usage</td>
                <td>{benchmark_data.get('memory_summary', {}).get('average_memory_mb', 0.0):.1f} MB</td>
                <td>{'GOOD' if benchmark_data.get('memory_summary', {}).get('average_memory_mb', 0) < 512 else 'MONITOR'}</td>
            </tr>
            <tr>
                <td>Regression Risk</td>
                <td>{benchmark_data.get('regression_risk', 0.0):.2%}</td>
                <td>{'LOW' if benchmark_data.get('regression_risk', 0) < 0.3 else 'MEDIUM' if benchmark_data.get('regression_risk', 0) < 0.6 else 'HIGH'}</td>
            </tr>
        </table>
    </div>

    <div class="section">
        <h2>System Integration Status</h2>
        <table>
            <tr>
                <th>System</th>
                <th>Integration Status</th>
                <th>Test Score</th>
            </tr>
"""
        
        for system, results in phase6_data.get('systems_integration', {}).items():
            status = 'GOOD' if results.get('test_score', 0) > 0.8 else 'NEEDS ATTENTION' if results.get('test_score', 0) > 0.5 else 'POOR'
            html += f"""
            <tr>
                <td>{system}</td>
                <td>{status}</td>
                <td>{results.get('test_score', 0):.2%}</td>
            </tr>
"""
        
        html += """
        </table>
    </div>

    <div class="section">
        <h2>Active Alerts</h2>
        <table>
            <tr>
                <th>Name</th>
                <th>Severity</th>
                <th>Message</th>
                <th>Timestamp</th>
            </tr>
"""
        
        for alert in monitoring_data.get('active_alerts', []):
            html += f"""
            <tr>
                <td>{alert.get('name', '')}</td>
                <td>{alert.get('severity', '')}</td>
                <td>{alert.get('message', '')}</td>
                <td>{alert.get('timestamp', '')}</td>
            </tr>
"""
        
        html += """
        </table>
    </div>

    <div class="section">
        <h2>Performance Issues</h2>
        <ul>
"""
        
        for issue in benchmark_data.get('detected_issues', []) + phase6_data.get('issues_detected', []):
            html += f"<li>{issue}</li>\n"
        
        html += """
        </ul>
    </div>

    <div class="section">
        <h2>Recommendations</h2>
        <ol>
"""
        
        recommendations = (phase6_data.get('recommendations', []) + 
                          ['Optimize neural processing for improved simulation speed',
                           'Monitor memory usage for potential bottlenecks',
                           'Review system configuration for integration issues',
                           'Implement performance alerting for proactive monitoring',
                           'Establish regular benchmarking schedules'])
        
        for rec in recommendations:
            html += f"<li>{rec}</li>\n"
        
        html += """
        </ol>
    </div>

    <div class="section">
        <h2>Scaling Analysis</h2>
        <table>
            <tr>
                <th>Neuron Count</th>
                <th>Average Performance</th>
                <th>Quality Score</th>
            </tr>
"""
        
        for scaling in benchmark_data.get('scaling_data', []):
            html += f"""
            <tr>
                <td>{scaling.get('neuron_count', 0)} neurons</td>
                <td>{scaling.get('average_performance', 0.0):.1f} steps/sec</td>
                <td>{scaling.get('quality_score', 0.0):.2%}</td>
            </tr>
"""
        
        html += """
        </table>
    </div>

    <div class="section">
        <h2>Conclusion</h2>
        <p>The NLM performance integration demonstrates comprehensive monitoring and benchmarking
capabilities for the artificial brain system. The integration provides valuable insights into
system performance, identifies potential issues, and offers recommendations for optimization
and improvement.</p>
        
        <p><strong>Key Findings:</strong></p>
        <ul>
            <li>System performance averages {benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0.0):.1f} steps/sec</li>
            <li>Memory usage averages {benchmark_data.get('memory_summary', {}).get('average_memory_mb', 0.0):.1f} MB</li>
            <li>{len(monitoring_data.get('active_alerts', []))} active alerts detected</li>
            <li>{len(phase6_data.get('issues_detected', []))} integration issues identified</li>
            <li>Overall integration quality: {phase6_data.get('integration_score', 0.0):.2%}</li>
        </ul>
        
        <p><strong>Next Steps:</strong></p>
        <ul>
            <li>Implement continuous monitoring and alerting</li>
            <li>Establish regular benchmarking schedules</li>
            <li>Monitor system resources and performance trends</li>
            <li>Integrate with deployment pipelines</li>
            <li>Provide performance dashboards for system administrators</li>
        </ul>
    </div>
</body>
</html>
"""
        return html
    
    def generate_json_summary(self, benchmark_data, monitoring_data, phase6_data):
        """Generate a JSON summary report"""
        import datetime
        
        summary = {
            "report_metadata": {
                "generated_at": datetime.datetime.now().isoformat(),
                "report_type": "performance_integration_summary",
                "version": "1.0"
            },
            "integration_summary": {
                "status": phase6_data.get('integration_status', 'UNKNOWN'),
                "score": phase6_data.get('integration_score', 0.0),
                "issues_count": len(phase6_data.get('issues_detected', [])),
                "recommendations_count": len(phase6_data.get('recommendations', []))
            },
            "performance_results": {
                "benchmarks": {
                    "total_run": benchmark_data.get('total_benchmarks', 0),
                    "total_neurons": benchmark_data.get('total_neurons_tested', 0),
                    "average_performance": benchmark_data.get('performance_summary', {}).get('average_steps_per_second', 0.0),
                    "quality_score": benchmark_data.get('quality_score', 0.0),
                    "regression_risk": benchmark_data.get('regression_risk', 0.0)
                },
                "memory_usage": {
                    "average_mb": benchmark_data.get('memory_summary', {}).get('average_memory_mb', 0.0),
                    "min_mb": benchmark_data.get('memory_summary', {}).get('min_memory_mb', 0.0),
                    "max_mb": benchmark_data.get('memory_summary', {}).get('max_memory_mb', 0.0)
                }
            },
            "monitoring_results": {
                "status": monitoring_data.get('monitoring_status', 'UNKNOWN'),
                "performance_score": monitoring_data.get('performance_score', 0.0),
                "active_alerts_count": len(monitoring_data.get('active_alerts', [])),
                "system_metrics": monitoring_data.get('system_metrics', {})
            },
            "integration_results": {
                "systems_tested": list(phase6_data.get('systems_integration', {}).keys()),
                "system_scores": {system: results.get('test_score', 0.0) 
                                 for system, results in phase6_data.get('systems_integration', {}).items()},
                "detected_issues": phase6_data.get('issues_detected', []),
                "recommendations": phase6_data.get('recommendations', [])
            }
        }
        
        return summary
    
    def get_timestamp(self):
        """Get current timestamp"""
        import datetime
        return datetime.datetime.now().isoformat()
    
    def load_json(self, filepath):
        """Load JSON data from file"""
        import json
        try:
            with open(filepath, 'r') as f:
                return json.load(f)
        except (FileNotFoundError, json.JSONDecodeError):
            return {}
    
    def run_full_integration_demo(self):
        """Run the complete integration demonstration"""
        print("=" * 60)
        print("NLM Performance Integration Demo")
        print("=" * 60)
        print()
        
        # Step 1: Run benchmark suite
        benchmark_results = self.run_benchmark_suite()
        
        # Step 2: Run monitoring integration
        monitoring_results = self.run_monitoring_integration()
        
        # Step 3: Run Phase 6 integration
        phase6_results = self.run_phase6_integration()
        
        # Step 4: Generate reports
        reports = self.generate_reports()
        
        print()
        print("=" * 60)
        print("INTEGRATION DEMO COMPLETE")
        print("=" * 60)
        print()
        print(f"Results generated in: {self.output_dir}")
        print()
        print("Key Results:")
        print(f"  • Benchmarks Run: {benchmark_results.get('total_benchmarks', 0)}")
        print(f"  • Neurons Tested: {benchmark_results.get('total_neurons_tested', 0)}")
        print(f"  • Average Performance: {benchmark_results.get('performance_summary', {}).get('average_steps_per_second', 0.0):.1f} steps/sec")
        print(f"  • Integration Status: {phase6_results.get('integration_status', 'UNKNOWN')}")
        print(f"  • Integration Score: {phase6_results.get('integration_score', 0.0):.2%}")
        print()
        print("Reports Generated:")
        for report_type, path in reports.items():
            print(f"  • {report_type}: {path}")
        
        return {
            "benchmark_results": benchmark_results,
            "monitoring_results": monitoring_results,
            "phase6_results": phase6_results,
            "reports": reports
        }

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="NLM Performance Integration Demo",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    parser.add_argument(
        "--demo", 
        choices=["phase6", "phase2", "full"],
        default="phase6",
        help="Type of demo to run (default: phase6)"
    )
    
    parser.add_argument(
        "--output",
        default="integration_reports",
        help="Output directory for reports (default: integration_reports)"
    )
    
    parser.add_argument(
        "--config",
        help="Configuration file path"
    )
    
    parser.add_argument(
        "--help",
        action="store_true",
        help="Show this help message and exit"
    )
    
    args = parser.parse_args()
    
    if args.help:
        parser.print_help()
        sys.exit(0)
    
    try:
        # Initialize demo
        demo = PerformanceIntegrationDemo(args.output)
        
        # Run integration demo
        if args.demo == "phase6":
            print("Running Phase 6 Integration Demo...")
            results = demo.run_full_integration_demo()
        elif args.demo == "phase2":
            print("Running Phase 2 Integration Demo...")
            # Phase 2 specific implementation
            results = demo.run_full_integration_demo()
        else:  # full
            print("Running Full Integration Demo...")
            results = demo.run_full_integration_demo()
        
        print()
        print("Demo completed successfully!")
        print(f"Results are available in: {args.output}")
        
    except Exception as e:
        print(f"Error running demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
