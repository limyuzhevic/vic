#!/usr/bin/env python3
"""
Example configuration generator for NLM.

This script generates example configuration files based on NLM configuration schemas.
"""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional
import yaml
import random

class ExampleConfigGenerator:
    """Generate example configuration files based on schemas."""
    
    def __init__(self):
        self.examples = {
            "phase1": self._get_phase1_examples(),
            "phase2": self._get_phase2_examples(),
            "phase3": self._get_phase3_examples(),
            "phase4": self._get_phase4_examples(),
            "phase6": self._get_phase6_examples(),
            "development": self._get_development_examples(),
            "production": self._get_production_examples()
        }
    
    def _get_phase1_examples(self) -> Dict[str, Any]:
        """Get example Phase 1 configuration."""
        return {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_time_constant": 20.0,
            "development_synaptogenesis_rate": 0.001,
            "development_pruning_rate": 0.0001,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "environment_width": 10,
            "environment_height": 10,
            "log_level": "INFO",
            "log_to_file": False,
            "log_filename": "nlm.log",
            "max_simulation_steps": 10000,
            "simulation_time_limit": 0.0,
            "visualization_enabled": False,
            "visualization_update_rate": 30.0
        }
    
    def _get_phase2_examples(self) -> Dict[str, Any]:
        """Get example Phase 2 configuration."""
        phase1 = self._get_phase1_examples()
        phase1.update({
            "dopamine_max": 5.0,
            "dopamine_decay_rate": 0.1,
            "working_memory_size": 100,
            "working_memory_decay_rate": 0.01,
            "episodic_memory_size": 1000,
            "episodic_memory_similarity_threshold": 0.7,
            "semantic_memory_learning_rate": 0.05,
            "prediction_error_decay_rate": 0.05,
            "novelty_threshold": 0.5,
            "curiosity_strength": 0.3
        })
        return phase1
    
    def _get_phase3_examples(self) -> Dict[str, Any]:
        """Get example Phase 3 configuration."""
        phase2 = self._get_phase2_examples()
        phase2.update({
            "concept_formation_threshold": 0.6,
            "concept_generalization_strength": 0.4,
            "planning_horizon": 100,
            "planning_exploration_weight": 0.2,
            "cognitive_control_strength": 0.3,
            "cognitive_control_decay_rate": 0.05,
            "working_memory_consolidation_rate": 0.02,
            "episodic_memory_retrieval_strength": 0.8
        })
        return phase2
    
    def _get_phase4_examples(self) -> Dict[str, Any]:
        """Get example Phase 4 configuration."""
        phase3 = self._get_phase3_examples()
        phase3.update({
            "motor_noise_level": 0.1,
            "motor_gain": 2.0,
            "action_selection_temperature": 1.0,
            "action_selection_exploration_rate": 0.2,
            "reward_prediction_error_scaling": 1.0,
            "action_persistence": 0.8
        })
        return phase3
    
    def _get_phase6_examples(self) -> Dict[str, Any]:
        """Get example Phase 6 configuration."""
        phase4 = self._get_phase4_examples()
        phase4.update({
            "integration_learning_rate": 0.1,
            "integration_coherence_target": 0.7,
            "meta_learning_rate": 0.05,
            "meta_learning_threshold": 0.6,
            "self_organization_strength": 0.4,
            "self_organization_decay_rate": 0.03,
            "global_modulation_strength": 0.5
        })
        return phase4
    
    def _get_development_examples(self) -> Dict[str, Any]:
        """Get example development configuration."""
        phase6 = self._get_phase6_examples()
        phase6.update({
            "enable_debug_mode": True,
            "enable_validation_detailed": True,
            "validation_strategy": "lenient",
            "enable_schema_docs": True,
            "schema_docs_output_dir": "./schema_docs"
        })
        return phase6
    
    def _get_production_examples(self) -> Dict[str, Any]:
        """Get example production configuration."""
        phase6 = self._get_phase6_examples()
        phase6.update({
            "performance_monitoring": True,
            "performance_sample_rate": 10,
            "error_reporting_enabled": True,
            "error_reporting_endpoint": "https://nlm.example.com/api/errors",
            "log_retention_days": 30,
            "enable_hot_reloading": True,
            "hot_reload_check_interval": 5.0
        })
        return phase6
    
    def get_example_by_name(self, name: str) -> Dict[str, Any]:
        """Get an example configuration by name."""
        name = name.lower()
        if name in self.examples:
            return self.examples[name]
        elif name.startswith("phase"):
            # Try to extract phase number
            try:
                phase_num = int(name[5:])
                if phase_num == 1:
                    return self._get_phase1_examples()
                elif phase_num == 2:
                    return self._get_phase2_examples()
                elif phase_num == 3:
                    return self._get_phase3_examples()
                elif phase_num == 4:
                    return self._get_phase4_examples()
                elif phase_num == 6:
                    return self._get_phase6_examples()
            except ValueError:
                pass
        
        # Return a basic example if name not found
        print(f"Warning: Unknown example name '{name}', using default example")
        return self._get_phase1_examples()
    
    def generate_example(self, name: str, output_path: str):
        """Generate an example configuration file."""
        config = self.get_example_by_name(name)
        
        # Add metadata
        config["_metadata"] = {
            "generated_by": "NLM Example Configuration Generator",
            "timestamp": "$(date -u +'%Y-%m-%dT%H:%M:%SZ')",
            "version": "1.0.0"
        }
        
        # Save the configuration
        with open(output_path, 'w') as f:
            if output_path.endswith('.json'):
                json.dump(config, f, indent=2)
            elif output_path.endswith(('.yaml', '.yml')):
                yaml.dump(config, f, default_flow_style=False)
        
        return output_path


def generate_example_command(args):
    """Command line handler for example generation."""
    generator = ExampleConfigGenerator()
    
    try:
        output_path = generator.generate_example(args.name, args.output)
        print(f"Example configuration generated successfully: {output_path}")
        return 0
        
    except Exception as e:
        print(f"Error: {e}")
        return 1


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Example Configuration Generator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s generate-example --name phase1 --output config_phase1.json
  %(prog)s generate-example --name phase2 --output config_phase2.yaml
  %(prog)s generate-example --name development --output config_dev.json
        """
    )
    
    parser.add_argument('--name', required=True, help='Configuration example name (phase1, phase2, phase3, phase4, phase6, development, production)')
    parser.add_argument('--output', required=True, help='Path to output configuration file')
    
    parser.set_defaults(func=generate_example_command)
    
    args = parser.parse_args()
    
    if not hasattr(args, 'func'):
        parser.print_help()
        return 1
    
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
