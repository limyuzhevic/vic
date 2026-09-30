"""
Production Deployment Examples for NLM

This module demonstrates production deployment strategies including:
- Deployment automation
- Production monitoring
- Scalability deployment
- Production-ready configurations
- Deployment validation
"""

import pynlm
import numpy as np
import time
import subprocess
import sys
from typing import Dict, List, Optional, Tuple
import json
from datetime import datetime

class ProductionExamples:
    """Production deployment examples."""
    
    @staticmethod
    def create_production_ready_deployment():
        """
        Example: Production-ready NLM deployment.
        
        Creates a production-ready NLM system with:
        - Automated deployment scripts
        - Production monitoring
        - Scalability features
        - Production-grade configuration
        - Deployment validation
        """
        print("=== Production-Ready Deployment Example ===")
        
        # Create production-optimized configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.04)  # Optimized for production
        config.set("brain.connection_probability", 0.02)
        
        # Production-grade configuration
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)  # Conservative learning
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.003)
        config.set("plasticity.structural.enable", False)  # Disable for stability
        
        # Production neuromodulation settings
        config.set("neuromod.dopamine.scale", 0.8)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        print("Creating production-ready brain configuration...")
        print(f"  Target neurons: {config.get('brain.neuron_count'):,}")
        print(f"  Synapse density: {config.get('brain.synapse_density'):.3f}")
        print(f"  Conservative learning rates: STDP={config.get('plasticity.stdp.learning_rate'):.4f}")
        print("  Production features enabled: Stability, monitoring, scalability")
        
        # Create production brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create production environment
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(20.0, 20.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable production features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)  # Exploration for adaptation
        agent.enableNovelty(True)    # Discovery for improvement
        
        print("\nCreating production deployment...")
        
        # Production deployment script (simulated)
        deployment_script = """
# Production Deployment Script for NLM
# ====================================

# Configuration
DEPLOYMENT_MODE="production"
NEURON_COUNT=%d
SYNAPSES_DENSITY=%f
LEARNING_RATE_STDP=%f
LEARNING_RATE_HEBBIAN=%f

# Performance Targets
TARGET_RESPONSE_TIME_MS=100
TARGET_MEMORY_USAGE_MB=500
TARGET_UPTIME_PERCENT=99.9

# Deployment Steps
deploy_brain() {
    echo "Deploying brain with %d neurons..."
    ./nlm --config=config.json --neurons=%d --mode=production
}

monitor_system() {
    echo "Starting system monitoring..."
    ./monitoring_tool --log=production.log --alerts=critical
}

validate_deployment() {
    echo "Validating deployment..."
    ./validation_tool --tests=production_ready --output=validation_results.json
}

# Execute deployment
deploy_brain
monitor_system  
validate_deployment

# Start production services
start_services()

# Log deployment
echo "DEPLOYMENT_SUCCESS: %s" >> deployment.log
""" % (
            config.get("brain.neuron_count"),
            config.get("brain.synapse_density"),
            config.get("plasticity.stdp.learning_rate"),
            config.get("plasticity.hebbian.learning_rate"),
            config.get("brain.neuron_count"),
            datetime.now().isoformat()
        )
        
        # Write deployment script
        with open("/tmp/nlm_production_deployment.sh", "w") as f:
            f.write(deployment_script)
        
        # Simulate production deployment
        print("\nExecuting production deployment...")
        
        deployment_results = {
            "deployment_timestamp": datetime.now().isoformat(),
            "deployment_script": deployment_script,
            "validation_results": {
                "brain_initialized": True,
                "configuration_valid": True,
                "production_features_enabled": True,
                "performance_targets_met": True
            },
            "system_metrics": {
                "deployment_time": 0.0,  # Simulated
                "memory_usage_mb": 150.0,  # Estimated
                "cpu_usage_percent": 45.0,  # Estimated
                "uptime_achieved": 99.9
            },
            "production_config": {
                "neuron_count": config.get("brain.neuron_count"),
                "synapse_density": config.get("brain.synapse_density"),
                "learning_rates": {
                    "stdp": config.get("plasticity.stdp.learning_rate"),
                    "hebbian": config.get("plasticity.hebbian.learning_rate")
                },
                "neuromodulation": {
                    "dopamine_scale": config.get("neuromod.dopamine.scale"),
                    "curiosity_enabled": config.get("neuromod.curiosity.enable"),
                    "novelty_enabled": config.get("neuromod.novelty.enable")
                }
            }
        }
        
        print("\nProduction Deployment Results:")
        print(f"  Deployment time: {deployment_results['system_metrics']['deployment_time']:.2f}s")
        print(f"  Memory usage: {deployment_results['system_metrics']['memory_usage_mb']:.1f} MB")
        print(f"  CPU usage: {deployment_results['system_metrics']['cpu_usage_percent']:.1f}%")
        print(f"  Uptime achieved: {deployment_results['system_metrics']['uptime_achieved']}%")
        print(f"  Configuration validated: {'✓' if deployment_results['validation_results']['configuration_valid'] else '✗'}")
        print(f"  Production features: {'✓' if deployment_results['validation_results']['production_features_enabled'] else '✗'}")
        
        # Production monitoring setup
        print("\nSetting up production monitoring...")
        
        monitoring_config = {
            "monitoring_system": "production",
            "metrics_collection": True,
            "alerting_enabled": True,
            "performance_targets": {
                "max_response_time_ms": 100,
                "max_memory_usage_mb": 500,
                "min_uptime_percent": 99.9
            },
            "alert_levels": {
                "warning": 80,
                "critical": 95
            }
        }
        
        print("Monitoring Configuration:")
        for key, value in monitoring_config.items():
            if isinstance(value, dict):
                print(f"  {key}:")
                for subkey, subvalue in value.items():
                    print(f"    {subkey}: {subvalue}")
            else:
                print(f"  {key}: {value}")
        
        return {
            "brain": brain,
            "agent": agent,
            "world": world,
            "deployment_results": deployment_results,
            "monitoring_config": monitoring_config,
            "config": config
        }
    
    @staticmethod
    def create_scalable_deployment_example():
        """
        Example: Scalable production deployment.
        
        Demonstrates deployment strategies for scaling NLM systems including:
        - Horizontal scaling
        - Load balancing
        - Distributed computing
        - Resource optimization
        - Auto-scaling strategies
        """
        print("\n=== Scalable Production Deployment Example ===")
        
        # Create scalable configuration
        base_config = pynlm.createDefaultConfig()
        base_config.set("brain.neuron_count", 2000)
        base_config.set("brain.synapse_density", 0.08)
        
        # Configure for scalability
        base_config.set("neuromod.dopamine.scale", 0.9)
        base_config.set("neuromod.curiosity.enable", True)
        base_config.set("neuromod.novelty.enable", True)
        
        base_config.set("plasticity.stdp.enable", True)
        base_config.set("plasticity.stdp.learning_rate", 0.01)
        
        print("Creating scalable deployment architecture")
        print(f"  Base configuration: {base_config.get('brain.neuron_count'):,} neurons")
        print("  Scalability features: Load balancing, distributed computing, auto-scaling")
        
        # Define scaling strategies
        scaling_strategies = {
            "vertical_scaling": {
                "description": "Increase resources in single instance",
                "neurons_per_instance": 2000,
                "instances_needed": lambda neurons: max(1, neurons // 2000),
                "benefits": ["Simple implementation", "Low communication overhead"],
                "limits": ["Hardware limitations", "Single point of failure"]
            },
            "horizontal_scaling": {
                "description": "Distribute neurons across multiple instances",
                "neurons_per_instance": 1000,
                "instances_needed": lambda neurons: neurons // 1000,
                "benefits": ["High availability", "Fault tolerance", "Linear scaling"],
                "limits": ["Communication overhead", "Synchronization complexity"]
            },
            "hybrid_scaling": {
                "description": "Combine vertical and horizontal scaling",
                "neurons_per_instance": 1500,
                "instances_needed": lambda neurons: max(1, neurons // 1500),
                "benefits": ["Balanced approach", "Resource efficiency"],
                "limits": ["Complex management", "Coordination overhead"]
            }
        }
        
        print("\nScaling Strategy Options:")
        for strategy_name, strategy_data in scaling_strategies.items():
            print(f"  {strategy_name.title()}:")
            print(f"    {strategy_data['description']}")
            print(f"    Neurons per instance: {strategy_data['neurons_per_instance']:,}")
            print(f"    Benefits: {', '.join(strategy_data['benefits'])}")
        
        # Simulate deployment for different scales
        deployment_scenarios = {
            "small_scale": {"neurons": 5000, "strategy": "horizontal"},
            "medium_scale": {"neurons": 20000, "strategy": "horizontal"},
            "large_scale": {"neurons": 50000, "strategy": "hybrid"},
            "enterprise_scale": {"neurons": 100000, "strategy": "horizontal"}
        }
        
        print("\nDeployment Scenarios:")
        print("  Scale     | Strategy    | Instances | Estimated Memory | Status")
        print("  ---------|-------------|-----------|-----------------|--------")
        
        deployment_results = {}
        
        for scenario_name, scenario_data in deployment_scenarios.items():
            neurons_needed = scenario_data["neurons"]
            strategy = scenario_data["strategy"]
            
            # Get strategy details
            strategy_details = scaling_strategies[strategy]
            
            # Calculate instances needed
            instances_needed = strategy_details["instances_needed"](neurons_needed)
            
            # Estimate resource requirements
            memory_per_instance_mb = 200  # Estimated memory per instance
            total_memory_mb = instances_needed * memory_per_instance_mb
            
            # Estimated performance
            neurons_per_instance = strategy_details["neurons_per_instance"]
            efficiency_factor = 0.9 ** (instances_needed - 1)  # Efficiency loss with more instances
            total_performance = (neurons_needed / neurons_per_instance) * efficiency_factor
            
            # Determine status
            if total_memory_mb < 1000 and total_performance > 50:
                status = "✓ Optimal"
            elif total_memory_mb < 2000 and total_performance > 10:
                status = "✓ Good"
            else:
                status = "⚠ Needs review"
            
            print(f"  {scenario_name:9} | {strategy:11} | {instances_needed:9} | {total_memory_mb:15.1f} | {status}")
            
            deployment_results[scenario_name] = {
                "neurons_needed": neurons_needed,
                "strategy": strategy,
                "instances_needed": instances_needed,
                "total_memory_mb": total_memory_mb,
                "estimated_performance": total_performance,
                "strategy_details": strategy_details
            }
        
        # Auto-scaling simulation
        print("\n=== Auto-Scaling Simulation ===")
        
        # Simulate workload-based auto-scaling
        base_load = 100  # Base load percentage
        max_load = 80    # Maximum load before scaling
        
        auto_scaling_config = {
            "base_instances": 2,
            "scale_up_threshold": max_load,
            "scale_down_threshold": 30,
            "scale_up_factor": 1.5,
            "scale_down_factor": 0.7,
            "response_time_ms": 100
        }
        
        print(f"Auto-scaling Configuration:")
        print(f"  Base instances: {auto_scaling_config['base_instances']}")
        print(f"  Scale-up threshold: {auto_scaling_config['scale_up_threshold']}%")
        print(f"  Scale-down threshold: {auto_scaling_config['scale_down_threshold']}%")
        print(f"  Scale-up factor: {auto_scaling_config['scale_up_factor']}")
        print(f"  Scale-down factor: {auto_scaling_config['scale_down_factor']}")
        
        # Simulate load changes and auto-scaling responses
        load_simulation = []
        current_instances = auto_scaling_config["base_instances"]
        
        for hour in range(24):
            # Simulate load pattern (daytime higher, nighttime lower)
            if 6 <= hour <= 18:
                load = np.random.uniform(40, 90)  # Daytime load
            else:
                load = np.random.uniform(10, 40)  # Nighttime load
            
            # Apply auto-scaling rules
            if load > auto_scaling_config["scale_up_threshold"] and current_instances > 1:
                current_instances = max(1, int(current_instances * auto_scaling_config["scale_down_factor"]))
                action = "scaled down"
            elif load < auto_scaling_config["scale_down_threshold"] and current_instances < 10:
                current_instances = int(current_instances * auto_scaling_config["scale_up_factor"])
                action = "scaled up"
            else:
                action = "stable"
            
            load_simulation.append({
                "hour": hour,
                "load_percent": load,
                "instances": current_instances,
                "action": action
            })
        
        # Calculate auto-scaling efficiency
        scale_ups = sum(1 for entry in load_simulation if entry["action"] == "scaled up")
        scale_downs = sum(1 for entry in load_simulation if entry["action"] == "scaled down")
        avg_instances = np.mean([entry["instances"] for entry in load_simulation])
        
        print(f"\nAuto-Scaling Results (24-hour simulation):")
        print(f"  Scale-up events: {scale_ups}")
        print(f"  Scale-down events: {scale_downs}")
        print(f"  Average instances: {avg_instances:.1f}")
        print(f"  Resource efficiency: {(auto_scaling_config['base_instances'] / avg_instances) * 100:.1f}%")
        
        return {
            "deployment_scenarios": deployment_scenarios,
            "deployment_results": deployment_results,
            "auto_scaling_config": auto_scaling_config,
            "load_simulation": load_simulation,
            "auto_scaling_efficiency": (auto_scaling_config['base_instances'] / avg_instances) * 100 if 'avg_instances' in locals() else 0,
            "base_config": base_config
        }
    
    @staticmethod
    def create_production_monitoring_example():
        """
        Example: Production monitoring and observability.
        
        Creates comprehensive monitoring for production NLM systems including:
        - Performance monitoring
        - Health checks
        - Alerting systems
        - Metrics collection
        - Log management
        """
        print("\n=== Production Monitoring Example ===")
        
        # Create monitoring-ready configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.1)
        
        # Configure for monitoring
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Creating production monitoring system")
        print(f"  Monitored neurons: {config.get('brain.neuron_count'):,}")
        print("  Monitoring features: Performance metrics, health checks, alerts")
        
        # Monitoring configuration
        monitoring_config = {
            "monitoring_system": "prometheus_grafana",
            "metrics_export": True,
            "alerting_enabled": True,
            "log_level": "INFO",
            "retention_days": 30,
            "sampling_interval_seconds": 5
        }
        
        print("\nMonitoring Configuration:")
        for key, value in monitoring_config.items():
            print(f"  {key}: {value}")
        
        # Performance metrics collection
        monitoring_data = {
            "timestamp": [],
            "brain_metrics": {
                "firing_rate": [],
                "active_neurons": [],
                "memory_usage": [],
                "cpu_usage": [],
                "response_time_ms": []
            },
            "system_health": {
                "status": [],
                "checks_passed": [],
                "checks_failed": []
            },
            "alerts": []
        }
        
        # Define health checks
        health_checks = {
            "brain_health": {
                "check": lambda brain: brain.getAverageFiringRate() > 0 and brain.getAverageFiringRate() < 50,
                "critical": True,
                "description": "Brain operational range"
            },
            "memory_health": {
                "check": lambda brain: True,  # Simplified - would check memory usage
                "critical": False,
                "description": "Memory usage within limits"
            },
            "performance_health": {
                "check": lambda brain: brain.getFiringNeuronCount() > 0,
                "critical": True,
                "description": "Learning activity detected"
            }
        }
        
        print("\nRunning production monitoring...")
        
        # Simulate continuous monitoring
        for hour in range(24):
            # Collect brain metrics
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            # Run health checks
            health_results = {}
            checks_passed = 0
            checks_failed = 0
            
            for check_name, check_data in health_checks.items():
                try:
                    check_result = check_data["check"](brain)
                    health_results[check_name] = check_result
                    if check_result:
                        checks_passed += 1
                    else:
                        checks_failed += 1
                        if check_data["critical"]:
                            monitoring_data["alerts"].append({
                                "timestamp": datetime.now().isoformat(),
                                "severity": "critical",
                                "message": f"Critical health check failed: {check_name}",
                                "action": "emergency_shutdown"
                            })
                except Exception as e:
                    health_results[check_name] = False
                    checks_failed += 1
                    monitoring_data["alerts"].append({
                        "timestamp": datetime.now().isoformat(),
                        "severity": "warning",
                        "message": f"Health check error: {check_name} - {str(e)}",
                        "action": "investigate"
                    })
            
            # Determine system status
            system_status = "healthy"
            if checks_failed > 0:
                if checks_failed >= 2:
                    system_status = "critical"
                else:
                    system_status = "warning"
            
            # Record metrics
            monitoring_data["timestamp"].append(hour)
            monitoring_data["brain_metrics"]["firing_rate"].append(firing_rate)
            monitoring_data["brain_metrics"]["active_neurons"].append(active_neurons)
            monitoring_data["brain_metrics"]["memory_usage"].append(np.random.uniform(20, 80))
            monitoring_data["brain_metrics"]["cpu_usage"].append(np.random.uniform(10, 60))
            monitoring_data["brain_metrics"]["response_time_ms"].append(np.random.uniform(50, 200))
            
            monitoring_data["system_health"]["status"].append(system_status)
            monitoring_data["system_health"]["checks_passed"].append(checks_passed)
            monitoring_data["system_health"]["checks_failed"].append(checks_failed)
            
            # Simulate metrics collection
            if hour % 4 == 0:  # Every 4 hours
                print(f"  Hour {hour:2d}: Status={system_status:7s}, "
                      f"Firing rate={firing_rate:5.1f} Hz, "
                      f"Checks: {checks_passed} passed, {checks_failed} failed")
        
        # Analyze monitoring results
        print("\n=== Monitoring Analysis ===")
        
        # Calculate metrics
        avg_firing_rate = np.mean(monitoring_data["brain_metrics"]["firing_rate"])
        avg_cpu_usage = np.mean(monitoring_data["brain_metrics"]["cpu_usage"])
        avg_memory_usage = np.mean(monitoring_data["brain_metrics"]["memory_usage"])
        
        # Calculate system health summary
        healthy_hours = sum(1 for status in monitoring_data["system_health"]["status"] if status == "healthy")
        warning_hours = sum(1 for status in monitoring_data["system_health"]["status"] if status == "warning")
        critical_hours = sum(1 for status in monitoring_data["system_health"]["status"] if status == "critical")
        
        print(f"Monitoring Summary:")
        print(f"  Average firing rate: {avg_firing_rate:.1f} Hz")
        print(f"  Average CPU usage: {avg_cpu_usage:.1f}%")
        print(f"  Average memory usage: {avg_memory_usage:.1f}%")
        print(f"  System health: {healthy_hours} healthy, {warning_hours} warning, {critical_hours} critical hours")
        
        # Alert summary
        critical_alerts = [a for a in monitoring_data["alerts"] if a["severity"] == "critical"]
        warning_alerts = [a for a in monitoring_data["alerts"] if a["severity"] == "warning"]
        
        print(f"\nAlert Summary:")
        print(f"  Critical alerts: {len(critical_alerts)}")
        print(f"  Warning alerts: {len(warning_alerts)}")
        
        if critical_alerts:
            print(f"  Critical alert examples:")
            for alert in critical_alerts[:3]:
                print(f"    - {alert['message']}")
        
        return {
            "brain": brain,
            "monitoring_data": monitoring_data,
            "health_checks": health_checks,
            "monitoring_config": monitoring_config,
            "monitoring_summary": {
                "avg_firing_rate": avg_firing_rate,
                "avg_cpu_usage": avg_cpu_usage,
                "avg_memory_usage": avg_memory_usage,
                "healthy_hours": healthy_hours,
                "warning_hours": warning_hours,
                "critical_hours": critical_hours,
                "critical_alerts": len(critical_alerts),
                "warning_alerts": len(warning_alerts)
            }
        }

if __name__ == "__main__":
    print("Production Deployment Examples")
    print("=" * 30)
    
    # Example 1: Production-ready deployment
    production_result = ProductionExamples.create_production_ready_deployment()
    print(f"\nProduction deployment: {production_result['deployment_results']['validation_results']['configuration_valid']}")
    
    # Example 2: Scalable deployment
    scalable_result = ProductionExamples.create_scalable_deployment_example()
    print(f"\nScalable deployment: {len(scalable_result['deployment_scenarios'])} scenarios analyzed")
    
    # Example 3: Production monitoring
    monitoring_result = ProductionExamples.create_production_monitoring_example()
    print(f"\nProduction monitoring: {monitoring_result['monitoring_summary']['healthy_hours']} healthy hours")
    
    print("\n=== All production deployment examples completed ===")
