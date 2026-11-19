#!/usr/bin/env python3
"""
NLM - Advanced Error Handling and Validation Utilities

Comprehensive error handling and validation tools for NLM:
- Brain state validation
- Configuration validation
- Simulation integrity checks
- Memory management validation
- Performance monitoring and alerting
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any, Tuple
import time
import traceback
from enum import Enum

# Add the parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class ErrorSeverity(Enum):
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"

class NLMError(Exception):
    """Base exception class for NLM errors"""
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 code: Optional[str] = None, details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.severity = severity
        self.code = code
        self.details = details or {}
        self.timestamp = time.time()

class ValidationResult:
    """Result of validation operations"""
    def __init__(self):
        self.errors: List[NLMError] = []
        self.warnings: List[NLMError] = []
        self.info: List[NLMError] = []
    
    def add_error(self, message: str, code: Optional[str] = None, details: Optional[Dict] = None):
        self.errors.append(NLMError(message, ErrorSeverity.ERROR, code, details))
    
    def add_warning(self, message: str, code: Optional[str] = None, details: Optional[Dict] = None):
        self.warnings.append(NLMError(message, ErrorSeverity.WARNING, code, details))
    
    def add_info(self, message: str, code: Optional[str] = None, details: Optional[Dict] = None):
        self.info.append(NLMError(message, ErrorSeverity.INFO, code, details))
    
    def is_valid(self) -> bool:
        return len(self.errors) == 0
    
    def has_warnings(self) -> bool:
        return len(self.warnings) > 0
    
    def get_summary(self) -> Dict:
        return {
            "valid": self.is_valid(),
            "error_count": len(self.errors),
            "warning_count": len(self.warnings),
            "info_count": len(self.info),
            "errors": [{"message": e.message, "code": e.code, "details": e.details} 
                      for e in self.errors],
            "warnings": [{"message": e.message, "code": e.code, "details": e.details} 
                        for e in self.warnings],
            "info": [{"message": e.message, "code": e.code, "details": e.details} 
                    for e in self.info]
        }

class BrainValidator:
    """Validator for brain states and configurations"""
    
    def __init__(self, brain=None, config=None):
        self.brain = brain
        self.config = config
    
    def validate_brain_integrity(self) -> ValidationResult:
        """Validate the integrity of a brain object"""
        result = ValidationResult()
        
        if not self.brain:
            result.add_error("No brain object provided for validation")
            return result
        
        # Basic brain properties
        try:
            neuron_count = self.brain.getTotalNeuronCount()
            synapse_count = self.brain.getTotalSynapseCount()
            region_count = self.brain.getRegionCount()
            firing_rate = self.brain.getAverageFiringRate()
            eir_ratio = self.brain.getExcitationInhibitionRatio()
            total_spikes = self.brain.getTotalSpikeCount()
            
            # Validate neuron counts
            if neuron_count == 0:
                result.add_error("Brain has zero neurons", "BRAIN_001")
            elif neuron_count < 10:
                result.add_warning("Brain has very few neurons (<10), may not be functional", "BRAIN_002")
            elif neuron_count > 100000:
                result.add_warning("Brain has extremely many neurons (>100k), may be slow", "BRAIN_003")
            
            # Validate synapse counts
            if synapse_count == 0:
                result.add_warning("Brain has zero synapses, no connectivity", "BRAIN_004")
            elif synapse_count / neuron_count > 10:
                result.add_warning("Synapse density is extremely high (>10/neuron)", "BRAIN_005")
            
            # Validate firing rate
            if firing_rate < 0.1:
                result.add_warning("Firing rate is very low (<0.1 Hz), brain may be inactive", "BRAIN_006")
            elif firing_rate > 100:
                result.add_error("Firing rate is extremely high (>100 Hz), may indicate runaway excitation", "BRAIN_007")
            elif 5 <= firing_rate <= 50:
                result.add_info("Firing rate is in healthy range (5-50 Hz)", "BRAIN_008")
            
            # Validate E/I ratio
            if abs(eir_ratio - 1.0) > 0.3:
                result.add_warning(f"E/I ratio is unbalanced ({eir_ratio:.2f}), target is 1.0", "BRAIN_009")
            elif abs(eir_ratio - 1.0) < 0.1:
                result.add_info(f"E/I ratio is well balanced ({eir_ratio:.2f})", "BRAIN_010")
            
            # Validate spike activity
            if total_spikes == 0:
                result.add_warning("Brain has no spikes recorded, may not be learning", "BRAIN_011")
            elif total_spikes < 100:
                result.add_warning("Brain has very few spikes (<100), may not be learning effectively", "BRAIN_012")
            
        except Exception as e:
            result.add_error(f"Exception during brain integrity validation: {str(e)}", "BRAIN_013")
        
        return result
    
    def validate_simulation_stability(self, max_firing_rate: float = 50.0, 
                                     min_firing_rate: float = 0.1) -> ValidationResult:
        """Validate simulation stability parameters"""
        result = ValidationResult()
        
        if not self.brain:
            result.add_error("No brain object provided for stability validation")
            return result
        
        try:
            firing_rate = self.brain.getAverageFiringRate()
            eir_ratio = self.brain.getExcitationInhibitionRatio()
            total_spikes = self.brain.getTotalSpikeCount()
            
            # Check for instability indicators
            if firing_rate > max_firing_rate:
                result.add_error(f"Firing rate {firing_rate:.2f} exceeds maximum stable rate {max_firing_rate}", 
                               "STABILITY_001", {"current_rate": firing_rate, "max_rate": max_firing_rate})
            
            if firing_rate < min_firing_rate and total_spikes > 0:
                result.add_warning(f"Firing rate {firing_rate:.2f} is below minimum stable rate {min_firing_rate}", 
                                 "STABILITY_002", {"current_rate": firing_rate, "min_rate": min_firing_rate})
            
            if abs(eir_ratio - 1.0) > 0.5:
                result.add_error(f"E/I ratio {eir_ratio:.2f} indicates severe instability, target is 1.0", 
                               "STABILITY_003", {"current_ratio": eir_ratio})
            
            # Check for learning indicators
            if total_spikes < 50:
                result.add_warning(f"Very low spike count ({total_spikes}) suggests poor learning", "STABILITY_004")
            
        except Exception as e:
            result.add_error(f"Exception during stability validation: {str(e)}", "STABILITY_005")
        
        return result
    
    def validate_configuration(self) -> ValidationResult:
        """Validate brain configuration"""
        result = ValidationResult()
        
        if not self.config:
            result.add_error("No configuration provided for validation")
            return result
        
        try:
            # Check for required configuration keys
            required_keys = ["brain.neuron_count", "brain.synapse_density"]
            for key in required_keys:
                if not self.config.has(key):
                    result.add_error(f"Missing required configuration key: {key}", "CONFIG_001")
            
            # Validate neuron count
            if self.config.has("brain.neuron_count"):
                neuron_count = self.config.getOr[int64_t]("brain.neuron_count", 0)
                if neuron_count <= 0:
                    result.add_error(f"Invalid neuron count: {neuron_count}", "CONFIG_002")
                elif neuron_count > 100000:
                    result.add_warning(f"Extremely high neuron count: {neuron_count}", "CONFIG_003")
            
            # Validate plasticity settings
            if self.config.has("plasticity.stdp.enable"):
                stdp_enabled = self.config.getOr[bool]("plasticity.stdp.enable", false)
                if not stdp_enabled:
                    result.add_warning("STDP is disabled, may limit learning capacity", "CONFIG_004")
            
            if self.config.has("plasticity.hebbian.enable"):
                hebbian_enabled = self.config.getOr[bool]("plasticity.hebbian.enable", false)
                if not hebbian_enabled:
                    result.add_warning("Hebbian learning is disabled, may limit learning capacity", "CONFIG_005")
            
        except Exception as e:
            result.add_error(f"Exception during configuration validation: {str(e)}", "CONFIG_006")
        
        return result

class SimulationMonitor:
    """Monitor simulation performance and health"""
    
    def __init__(self):
        self.history: List[Dict] = []
        self.alerts: List[NLMError] = []
    
    def record_step(self, step: int, brain, world=None, agent=None) -> None:
        """Record metrics for a simulation step"""
        if not brain:
            return
        
        try:
            metrics = {
                "step": step,
                "timestamp": time.time(),
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "eir_ratio": brain.getExcitationInhibitionRatio(),
                "total_spikes": brain.getTotalSpikeCount(),
                "region_count": brain.getRegionCount()
            }
            
            if world:
                metrics["world_time"] = world.getSimulationTime()
                metrics["world_width"] = world.getWidth()
                metrics["world_height"] = world.getHeight()
            
            if agent:
                metrics["curiosity"] = agent.getCuriosityLevel()
                metrics["novelty"] = agent.getNoveltyLevel()
                metrics["prediction_error"] = agent.getPredictionError()
                metrics["development_stage"] = str(agent.getDevelopmentalStage())
            
            self.history.append(metrics)
            
            # Check for anomalies
            self.check_anomalies(metrics)
            
        except Exception as e:
            self.add_alert(f"Failed to record step {step}: {str(e)}", ErrorSeverity.WARNING, "MONITOR_001")
    
    def check_anomalies(self, metrics: Dict) -> None:
        """Check for anomalous metrics that may indicate problems"""
        try:
            # Check for extreme firing rates
            firing_rate = metrics["firing_rate"]
            if firing_rate > 100:
                self.add_alert(f"Extreme firing rate detected: {firing_rate:.2f} Hz", 
                             ErrorSeverity.WARNING, "MONITOR_002", {"rate": firing_rate})
            
            if firing_rate < 0.01 and metrics["total_spikes"] > 0:
                self.add_alert(f"Extremely low firing rate: {firing_rate:.2f} Hz with spikes", 
                             ErrorSeverity.INFO, "MONITOR_003", {"rate": firing_rate, "spikes": metrics["total_spikes"]})
            
            # Check for E/I imbalance
            eir_ratio = metrics["eir_ratio"]
            if abs(eir_ratio - 1.0) > 0.5:
                self.add_alert(f"Severe E/I imbalance: ratio = {eir_ratio:.2f}", 
                             ErrorSeverity.ERROR, "MONITOR_004", {"ratio": eir_ratio})
            
            if abs(eir_ratio - 1.0) > 0.2:
                self.add_alert(f"Moderate E/I imbalance: ratio = {eir_ratio:.2f}", 
                             ErrorSeverity.WARNING, "MONITOR_005", {"ratio": eir_ratio})
            
            # Check for lack of learning
            if metrics["total_spikes"] == 0 and len(self.history) > 100:
                self.add_alert("No spikes recorded after many steps, may not be learning", 
                             ErrorSeverity.WARNING, "MONITOR_006")
            
        except Exception as e:
            self.add_alert(f"Anomaly detection failed: {str(e)}", ErrorSeverity.WARNING, "MONITOR_007")
    
    def add_alert(self, message: str, severity: ErrorSeverity, code: str, details: Optional[Dict] = None):
        """Add an alert to the monitor"""
        alert = NLMError(message, severity, code, details)
        self.alerts.append(alert)
    
    def get_alerts(self, severity_filter: Optional[ErrorSeverity] = None) -> List[NLMError]:
        """Get alerts, optionally filtered by severity"""
        if severity_filter is None:
            return self.alerts
        return [alert for alert in self.alerts if alert.severity == severity_filter]
    
    def get_statistics(self) -> Dict:
        """Get monitoring statistics"""
        if not self.history:
            return {}
        
        firing_rates = [m["firing_rate"] for m in self.history]
        eir_ratios = [m["eir_ratio"] for m in self.history]
        
        return {
            "total_steps": len(self.history),
            "avg_firing_rate": sum(firing_rates) / len(firing_rates),
            "min_firing_rate": min(firing_rates),
            "max_firing_rate": max(firing_rates),
            "avg_eir_ratio": sum(eir_ratios) / len(eir_ratios),
            "total_alerts": len(self.alerts),
            "critical_alerts": len([a for a in self.alerts if a.severity == ErrorSeverity.CRITICAL]),
            "error_alerts": len([a for a in self.alerts if a.severity == ErrorSeverity.ERROR]),
            "warning_alerts": len([a for a in self.alerts if a.severity == ErrorSeverity.WARNING]),
            "info_alerts": len([a for a in self.alerts if a.severity == ErrorSeverity.INFO])
        }

class NLMToolkit:
    """Comprehensive NLM toolkit for error handling and validation"""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.monitor = SimulationMonitor()
    
    def setup_validation_tools(self, brain=None, world=None, agent=None):
        """Setup validation tools with brain components"""
        self.brain = brain
        self.world = world
        self.agent = agent
    
    def validate_brain_state(self) -> ValidationResult:
        """Perform comprehensive brain state validation"""
        validator = BrainValidator(self.brain)
        return validator.validate_brain_integrity()
    
    def validate_simulation(self) -> ValidationResult:
        """Validate simulation state and stability"""
        result = ValidationResult()
        
        if not self.brain:
            result.add_error("No brain available for simulation validation")
            return result
        
        # Run brain validation
        brain_result = BrainValidator(self.brain).validate_brain_integrity()
        result.errors.extend(brain_result.errors)
        result.warnings.extend(brain_result.warnings)
        result.info.extend(brain_result.info)
        
        # Run stability validation
        stability_result = BrainValidator(self.brain).validate_simulation_stability()
        result.errors.extend(stability_result.errors)
        result.warnings.extend(stability_result.warnings)
        result.info.extend(stability_result.info)
        
        # Run configuration validation if available
        if hasattr(self, 'config') and self.config:
            config_result = BrainValidator(self.brain, self.config).validate_configuration()
            result.errors.extend(config_result.errors)
            result.warnings.extend(config_result.warnings)
            result.info.extend(config_result.info)
        
        return result
    
    def get_monitoring_report(self) -> Dict:
        """Get comprehensive monitoring report"""
        stats = self.monitor.get_statistics()
        
        report = {
            "timestamp": time.time(),
            "monitoring_stats": stats,
            "alerts": [{
                "message": alert.message,
                "severity": alert.severity.value,
                "code": alert.code,
                "timestamp": alert.timestamp,
                "details": alert.details
            } for alert in self.monitor.get_alerts()],
            "health_status": self.get_health_status()
        }
        
        return report
    
    def get_health_status(self) -> str:
        """Get overall health status"""
        if not self.brain:
            return "UNKNOWN"
        
        validation = self.validate_brain_state()
        
        if validation.is_valid():
            if validation.has_warnings():
                return "WARNING"
            return "HEALTHY"
        else:
            critical_errors = [e for e in validation.errors if e.severity == ErrorSeverity.CRITICAL]
            if critical_errors:
                return "CRITICAL"
            return "UNHEALTHY"
    
    def export_report(self, filepath: str) -> bool:
        """Export validation and monitoring report to file"""
        try:
            report = self.get_monitoring_report()
            
            with open(filepath, 'w') as f:
                json.dump(report, f, indent=2, default=str)
            
            return True
        except Exception as e:
            print(f"Failed to export report: {str(e)}")
            return False

def main():
    """Main CLI entry point for validation tools"""
    parser = argparse.ArgumentParser(
        description="NLM Error Handling and Validation Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s validate --steps 100
  %(prog)s monitor --steps 500 --report validation_report.json
  %(prog)s check --brain_file brain_state.bin
  %(prog)s analyze --type comprehensive

Validation commands:
  validate - Run comprehensive validation on current brain state
  monitor - Monitor simulation and generate health reports
  check - Check and validate checkpoint files
  analyze - Analyze simulation performance and stability

For more help on specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available validation commands')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate brain state and simulation')
    validate_parser.add_argument('--steps', '-s', type=int, default=100,
                                help='Number of simulation steps for validation (default: 100)')
    validate_parser.add_argument('--verbose', '-v', action='store_true',
                                help='Enable verbose validation output')
    validate_parser.add_argument('--export', '-e', type=str,
                                help='Export validation report to file')
    
    # Monitor command
    monitor_parser = subparsers.add_parser('monitor', help='Monitor simulation performance')
    monitor_parser.add_argument('--steps', '-s', type=int, default=500,
                               help='Number of simulation steps for monitoring (default: 500)')
    monitor_parser.add_argument('--report', '-r', type=str,
                               help='Export monitoring report to file')
    monitor_parser.add_argument('--alerts', type=str, choices=['all', 'error', 'warning', 'info'],
                               default='all', help='Filter alert types (default: all)')
    
    # Check command
    check_parser = subparsers.add_parser('check', help='Validate checkpoint files and state')
    check_parser.add_argument('--file', '-f', type=str, required=True,
                             help='Checkpoint file to validate')
    check_parser.add_argument('--brain-config', type=str,
                             help='Brain configuration file')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze simulation performance')
    analyze_parser.add_argument('--type', choices=['basic', 'comprehensive', 'stability'],
                               default='comprehensive', help='Analysis type (default: comprehensive)')
    analyze_parser.add_argument('--export', '-e', type=str,
                               help='Export analysis results to file')
    
    # Setup command
    setup_parser = subparsers.add_parser('setup', help='Setup validation tools with components')
    setup_parser.add_argument('--steps', '-s', type=int, default=10,
                             help='Initial simulation steps (default: 10)')
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)
    
    args = parser.parse_args()
    
    try:
        toolkit = NLMToolkit()
        
        # Setup validation tools
        toolkit.setup_validation_tools()
        
        if args.command == 'validate':
            print("=== NLM VALIDATION ===")
            
            # Run simulation for validation
            for step in range(args.steps):
                toolkit.brain.step(step)
                toolkit.monitor.record_step(step, toolkit.brain)
            
            # Run validation
            validation = toolkit.validate_simulation()
            
            print(f"\n=== VALIDATION RESULTS ===")
            print(f"Status: {'✓ PASS' if validation.is_valid() else '✗ FAIL'}")
            print(f"Errors: {len(validation.errors)}")
            print(f"Warnings: {len(validation.warnings)}")
            print(f"Info: {len(validation.info)}")
            
            if validation.errors:
                print(f"\n=== ERRORS ===")
                for error in validation.errors:
                    print(f"  {error.message} (Code: {error.code})")
            
            if validation.warnings:
                print(f"\n=== WARNINGS ===")
                for warning in validation.warnings:
                    print(f"  {warning.message} (Code: {warning.code})")
            
            # Export report if requested
            if args.export:
                if toolkit.export_report(args.export):
                    print(f"✓ Validation report exported to {args.export}")
                else:
                    print(f"✗ Failed to export validation report")
                    sys.exit(1)
        
        elif args.command == 'monitor':
            print("=== NLM MONITORING ===")
            
            # Run simulation for monitoring
            for step in range(args.steps):
                toolkit.brain.step(step)
                toolkit.monitor.record_step(step, toolkit.brain)
            
            # Get monitoring report
            report = toolkit.get_monitoring_report()
            
            print(f"\n=== MONITORING REPORT ===")
            stats = report['monitoring_stats']
            print(f"Total steps monitored: {stats.get('total_steps', 0)}")
            print(f"Average firing rate: {stats.get('avg_firing_rate', 0):.2f} Hz")
            print(f"Health status: {report['health_status']}")
            print(f"Total alerts: {stats.get('total_alerts', 0)}")
            
            # Show alerts based on filter
            alert_filter = None
            if args.alerts == 'error':
                alert_filter = ErrorSeverity.ERROR
            elif args.alerts == 'warning':
                alert_filter = ErrorSeverity.WARNING
            elif args.alerts == 'info':
                alert_filter = ErrorSeverity.INFO
            
            alerts = toolkit.monitor.get_alerts(alert_filter)
            if alerts:
                print(f"\n=== ALERTS ({args.alerts.upper()}) ===")
                for alert in alerts:
                    severity_icon = {"CRITICAL": "🚨", "ERROR": "❌", "WARNING": "⚠️", "INFO": "ℹ️"}
                    icon = severity_icon.get(alert.severity.value, "⚪")
                    print(f"  {icon} {alert.message} (Code: {alert.code})")
            
            # Export report if requested
            if args.report:
                if toolkit.export_report(args.report):
                    print(f"✓ Monitoring report exported to {args.report}")
                else:
                    print(f"✗ Failed to export monitoring report")
                    sys.exit(1)
        
        elif args.command == 'check':
            print(f"=== CHECKPOINT VALIDATION ===")
            print(f"Validating file: {args.file}")
            
            if not os.path.exists(args.file):
                print(f"✗ File not found: {args.file}")
                sys.exit(1)
            
            # Create configuration if provided
            config = None
            if args.brain_config and os.path.exists(args.brain_config):
                try:
                    config = pynlm.createDefaultConfig()
                    if config.loadFromFile(args.brain_config):
                        print(f"✓ Configuration loaded from {args.brain_config}")
                    else:
                        print(f"✗ Failed to load configuration from {args.brain_config}")
                except Exception as e:
                    print(f"✗ Error loading configuration: {str(e)}")
            
            # Validate checkpoint file
            validator = BrainValidator(None, config)
            
            # This would typically involve loading the checkpoint and validating it
            print(f"Note: Full checkpoint validation would load the file and validate its contents")
            print(f"✓ Checkpoint file validation completed (file exists and is accessible)")
        
        elif args.command == 'analyze':
            print(f"=== SIMULATION ANALYSIS ===")
            
            if args.type == 'basic':
                print("Running basic analysis...")
                # Basic analysis would include:
                # - Firing rate statistics
                # - Spike count analysis
                # - Region distribution
                if toolkit.brain:
                    print(f"  Neurons: {toolkit.brain.getTotalNeuronCount()}")
                    print(f"  Synapses: {toolkit.brain.getTotalSynapseCount()}")
                    print(f"  Firing rate: {toolkit.brain.getAverageFiringRate():.2f} Hz")
                    print(f"  E/I ratio: {toolkit.brain.getExcitationInhibitionRatio():.3f}")
            
            elif args.type == 'comprehensive':
                print("Running comprehensive analysis...")
                
                # Run comprehensive analysis
                validation = toolkit.validate_simulation()
                
                print(f"\n=== COMPREHENSIVE ANALYSIS ===")
                print(f"Network Health: {toolkit.get_health_status()}")
                print(f"Validation Status: {'PASS' if validation.is_valid() else 'FAIL'}")
                
                # Analyze monitoring data
                if toolkit.monitor.history:
                    recent_data = toolkit.monitor.history[-100:]  # Last 100 steps
                    if recent_data:
                        recent_rates = [d['firing_rate'] for d in recent_data]
                        recent_eir = [d['eir_ratio'] for d in recent_data]
                        
                        print(f"Recent Performance (last 100 steps):")
                        print(f"  Firing rate: {sum(recent_rates)/len(recent_rates):.2f} Hz (avg)")
                        print(f"  Firing rate std: {(sum((x - sum(recent_rates)/len(recent_rates))**2 for x in recent_rates)/len(recent_rates))**0.5:.2f}")
                        print(f"  E/I ratio: {sum(recent_eir)/len(recent_eir):.3f} (avg)")
                        
                        # Check for trends
                        if len(recent_rates) > 10:
                            rate_trend = recent_rates[-10:] - recent_rates[:-10]
                            avg_change = sum(rate_trend) / len(rate_trend)
                            if abs(avg_change) > 1.0:
                                trend_dir = "increasing" if avg_change > 0 else "decreasing"
                                print(f"  Firing rate trend: {trend_dir} ({avg_change:+.2f} Hz/step)")
            
            elif args.type == 'stability':
                print("Running stability analysis...")
                
                # Stability analysis would include:
                # - Spike rate volatility
                # - Network stability metrics
                # - Convergence analysis
                print("  Stability analysis requires detailed time-series data")
                print("  Current simulation monitor provides stability metrics")
            
            # Export analysis if requested
            if args.export:
                if toolkit.export_report(args.export):
                    print(f"✓ Analysis exported to {args.export}")
                else:
                    print(f"✗ Failed to export analysis")
                    sys.exit(1)
        
        elif args.command == 'setup':
            print("=== SETUP VALIDATION TOOLS ===")
            
            # Create a basic brain for testing
            toolkit.brain = pynlm.createBrain(pynlm.createDefaultConfig())
            toolkit.brain.initialize()
            
            # Run initial simulation
            print(f"Initializing brain with {args.steps} steps...")
            for step in range(args.steps):
                toolkit.brain.step(step)
                toolkit.monitor.record_step(step, toolkit.brain)
            
            print(f"✓ Validation tools setup completed")
            print(f"  Brain created with {toolkit.brain.getTotalNeuronCount()} neurons")
            print(f"  Monitored {len(toolkit.monitor.history)} steps")
            print(f"  Health status: {toolkit.get_health_status()}")
        
        else:
            print(f"Unknown command: {args.command}")
            sys.exit(1)
        
        print(f"\n✓ Command completed successfully")
        
    except Exception as e:
        print(f"✗ Command failed: {str(e)}")
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
