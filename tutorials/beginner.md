#!/usr/bin/env python3
"""
Configuration Validation Script

Validates NLM configuration files against schema and consistency rules.
Generates detailed reports with helpful error messages.

Usage: python scripts/validate_config.py [config_file] [options]
"""

import configparser
import json
import sys
import os
from pathlib import Path
from typing import Dict, List, Any, Tuple
import numpy as np

class ConfigValidator:
    """Validates configuration files for NLM (Neural Learning Machine)."""
    
    def __init__(self, config_path: str):
        """Initialize validator with config file path.
        
        Args:
            config_path: Path to configuration file
        """
        self.config_path = Path(config_path)
        self.config = configparser.ConfigParser()
        self.errors = []
        self.warnings = []
        self.validation_results = {}
        
    def load_config(self) -> bool:
        """Load and parse configuration file.
        
        Returns:
            bool: True if config loaded successfully, False otherwise
        """
        try:
            self.config.read(self.config_path)
            if not self.config.sections():
                self.errors.append(f"Configuration file is empty or malformed: {self.config_path}")
                return False
            return True
        except Exception as e:
            self.errors.append(f"Failed to parse configuration file: {e}")
            return False
    
    def validate_basic_structure(self) -> None:
        """Validate basic configuration structure."""
        required_sections = ['core', 'neuron', 'synapse', 'agent', 'simulation']
        
        for section in required_sections:
            if section not in self.config:
                self.errors.append(f"Missing required section: '{section}'")
        
        if 'core' in self.config:
            core = self.config['core']
            required_core_keys = ['seed', 'timestep', 'neuron_count']
            
            for key in required_core_keys:
                if key not in core:
                    self.errors.append(f"Missing required core setting: '{key}'")
                else:
                    try:
                        float(core[key])
                    except ValueError:
                        self.warnings.append(f"Core setting '{key}' should be a numeric value: {core[key]}")
    
    def validate_neural_network(self) -> None:
        """Validate neural network configuration parameters."""
        if 'core' in self.config:
            core = self.config['core']
            
            neuron_count = float(core.get('neuron_count', 0))
            if neuron_count <= 0:
                self.errors.append("neuron_count must be positive")
            elif neuron_count > 100000:
                self.warnings.append("Large neuron count may impact performance")
            
            timestep = float(core.get('timestep', 0))
            if timestep <= 0 or timestep > 1:
                self.errors.append("timestep must be between 0 and 1")
            
            if 'synapse' in self.config:
                synapse_prob = float(self.config['synapse'].get('connection_probability', 0))
                if not 0 <= synapse_prob <= 1:
                    self.errors.append("connection_probability must be between 0 and 1")
    
    def validate_memory_systems(self) -> None:
        """Validate memory system configurations."""
        if 'memory' not in self.config:
            self.warnings.append("Memory systems are not configured - performance may be limited")
            return
        
        memory = self.config['memory']
        memory_configs = {
            'working_memory': memory.get('working_memory_capacity', '0'),
            'episodic_memory': memory.get('episodic_memory_capacity', '0'),
            'semantic_memory': memory.get('semantic_memory_capacity', '0'),
            'procedural_memory': memory.get('procedural_memory_capacity', '0'),
            'associative_memory': memory.get('associative_memory_capacity', '0'),
        }
        
        for mem_type, capacity in memory_configs.items():
            try:
                cap_value = int(capacity)
                if cap_value <= 0:
                    self.warnings.append(f"{mem_type} capacity is zero or negative - may limit learning")
            except ValueError:
                self.errors.append(f"{mem_type} capacity must be an integer")
    
    def validate_consistency(self) -> None:
        """Validate consistency between settings."""
        if 'core' not in self.config or 'synapse' not in self.config:
            return
        
        try:
            neuron_count = float(self.config['core'].get('neuron_count', 0))
            max_synapses = float(self.config['synapse'].get('max_synapses_per_neuron', 0))
            
            if max_synapses >= neuron_count:
                self.errors.append("max_synapses_per_neuron should be less than neuron_count for reasonable connectivity")
        except (KeyError, ValueError):
            pass
    
    def validate_advanced_features(self) -> None:
        """Validate advanced feature configurations."""
        if 'neuromodulation' in self.config:
            neuromod = self.config['neuromodulation']
            
            if neuromod.get('curiosity_enabled', 'true').lower() == 'true':
                if 'curiosity' in self.config['neuromodulation']:
                    curiosity = self.config['neuromodulation']['curiosity']
                    
                    try:
                        threshold = float(curiosity.get('novelty_threshold', 1))
                        if not 0 <= threshold <= 1:
                            self.errors.append("novelty_threshold must be between 0 and 1")
                    except ValueError:
                        self.errors.append("novelty_threshold must be a numeric value")
    
    def validate_all(self) -> Dict[str, Any]:
        """Run all validation checks.
        
        Returns:
            Dict containing validation results
        """
        if not self.load_config():
            return {"status": "error", "errors": self.errors, "warnings": self.warnings}
        
        self.validate_basic_structure()
        self.validate_neural_network()
        self.validate_memory_systems()
        self.validate_consistency()
        self.validate_advanced_features()
        
        self.validation_results = {
            "file": str(self.config_path),
            "sections": self.config.sections(),
            "errors": self.errors,
            "warnings": self.warnings,
            "status": "valid" if not self.errors else "invalid"
        }
        
        return self.validation_results
    
    def generate_report(self) -> str:
        """Generate human-readable validation report.
        
        Returns:
            Formatted report string
        """
        if not self.validation_results:
            return "No validation results available. Run validate_all() first."
        
        report = []
        report.append("=" * 60)
        report.append(f"NLM Configuration Validation Report")
        report.append("=" * 60)
        report.append(f"File: {self.validation_results['file']}")
        report.append(f"Status: {self.validation_results['status'].upper()}")
        report.append("")
        
        if self.validation_results['warnings']:
            report.append("Warnings:")
            for warning in self.validation_results['warnings']:
                report.append(f"  ⚠ {warning}")
            report.append("")
        
        if self.validation_results['errors']:
            report.append("Errors:")
            for error in self.validation_results['errors']:
                report.append(f"  ✗ {error}")
            report.append("")
        
        if not self.validation_results['warnings'] and not self.validation_results['errors']:
            report.append("✓ Configuration appears valid")
        
        report.append("=" * 60)
        return "\n".join(report)
    
    def save_report(self, output_path: str = None) -> None:
        """Save validation report to file.
        
        Args:
            output_path: Output file path. If None, uses config_path + '_validation_report.txt'
        """
        if not output_path:
            output_path = str(self.config_path.parent / f"{self.config_path.stem}_validation_report.txt")
        
        with open(output_path, 'w') as f:
            f.write(self.generate_report())
        print(f"Report saved to: {output_path}")
    
    def save_json_report(self, output_path: str = None) -> None:
        """Save validation results as JSON.
        
        Args:
            output_path: Output file path. If None, uses config_path + '_validation.json'
        """
        if not output_path:
            output_path = str(self.config_path.parent / f"{self.config_path.stem}_validation.json")
        
        with open(output_path, 'w') as f:
            json.dump(self.validation_results, f, indent=2)
        print(f"JSON report saved to: {output_path}")


def main():
    """Main entry point for the validation script."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Validate NLM configuration files")
    parser.add_argument("config", help="Path to configuration file")
    parser.add_argument("-j", "--json", action="store_true", help="Output results as JSON")
    parser.add_argument("-s", "--save", action="store_true", help="Save reports to files")
    parser.add_argument("--verbose", action="store_true", help="Show detailed validation output")
    
    args = parser.parse_args()
    
    validator = ConfigValidator(args.config)
    results = validator.validate_all()
    
    if args.json:
        print(json.dumps(results, indent=2))
    else:
        print(validator.generate_report())
    
    if args.save:
        validator.save_report()
        validator.save_json_report()
    
    sys.exit(1 if results['status'] == 'invalid' else 0)


if __name__ == "__main__":
    main()
