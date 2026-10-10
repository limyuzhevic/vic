#!/usr/bin/env python3

"""
NLM Configuration Wizard

Interactive script to help users create NLM configurations easily.
Generates configuration files optimized for different use cases.
"""

import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, List
import getpass

class ConfigWizard:
    def __init__(self):
        self.home_dir = Path.home() / ".nlm"
        self.configs_dir = self.home_dir / "configs"
        self.configs_dir.mkdir(parents=True, exist_ok=True)
    
    def run_wizard(self):
        """Run the interactive configuration wizard"""
        print("=== NLM Configuration Wizard ===")
        print("Welcome to the NLM Configuration Wizard!")
        print("This will help you create an optimized configuration for your simulation.\n")
        
        # Determine use case
        use_case = self._select_use_case()
        
        # Get user preferences
        preferences = self._get_preferences(use_case)
        
        # Generate configuration
        config = self._generate_config(use_case, preferences)
        
        # Save configuration
        config_name = self._get_config_name(use_case, preferences)
        config_file = self.configs_dir / f"{config_name}.json"
        
        self._save_config(config_file, config)
        
        print(f"\n✓ Configuration saved to: {config_file}")
        print("\nNext steps:")
        print(f"  1. Load the configuration: nlm-cli brain run --config {config_file}")
        print(f"  2. View the configuration: cat {config_file}")
        print("\nFor more options, see the available commands:")
        print("  nlm-cli config list")
        print("  nlm-cli brain create")
        print("  nlm-cli experiment run <config_file>")
    
    def _select_use_case(self) -> str:
        """Get the user's use case selection"""
        print("1. Select Use Case:")
        print("   1) Research Simulation - Scientific experiments and data collection")
        print("   2) Learning Study - Focus on synaptic learning and plasticity")
        print("   3) Development Study - Study brain development and maturation")
        print("   4) Behavior Study - Focus on agent behavior and decision making")
        print("   5) Quick Test - Fast, minimal setup for testing")
        print("   6) Custom - Fully custom configuration")
        
        while True:
            try:
                choice = input("   Enter choice (1-6): ").strip()
                use_cases = {
                    '1': 'research',
                    '2': 'learning', 
                    '3': 'development',
                    '4': 'behavior',
                    '5': 'quick_test',
                    '6': 'custom'
                }
                if choice in use_cases:
                    return use_cases[choice]
                print("   Invalid choice. Please enter 1-6.")
            except KeyboardInterrupt:
                print("\nConfiguration cancelled.")
                sys.exit(0)
    
    def _get_preferences(self, use_case: str) -> Dict[str, Any]:
        """Get user preferences based on use case"""
        print(f"\n2. Configure {use_case.replace('_', ' ').title()} Simulation:")
        
        preferences = {}
        
        if use_case == 'quick_test':
            # Fast, minimal configuration
            preferences['neuron_count'] = 50
            preferences['steps'] = 200
            preferences['enable_reward'] = False
            preferences['enable_curiosity'] = False
            preferences['enable_development'] = False
            preferences['enable_planning'] = False
            preferences['simulation_timestep'] = 0.001
            preferences['record_video'] = False
            preferences['detailed_logging'] = False
            
        elif use_case == 'research':
            # Comprehensive research configuration
            preferences['neuron_count'] = self._get_int(
                "   Number of neurons (50-1000): ", 200, 50, 1000
            )
            preferences['steps'] = self._get_int(
                "   Number of simulation steps: ", 5000, 100, 100000
            )
            preferences['enable_reward'] = self._get_bool(
                "   Enable reward-based learning? ", True
            )
            preferences['enable_curiosity'] = self._get_bool(
                "   Enable curiosity-driven exploration? ", True
            )
            preferences['enable_development'] = self._get_bool(
                "   Enable developmental stages? ", True
            )
            preferences['enable_planning'] = self._get_bool(
                "   Enable neural planning? ", True
            )
            preferences['simulation_timestep'] = self._get_float(
                "   Simulation timestep (s): ", 0.001, 0.0001, 0.01
            )
            preferences['record_video'] = self._get_bool(
                "   Record video of simulation? ", False
            )
            preferences['detailed_logging'] = self._get_bool(
                "   Enable detailed logging? ", True
            )
            
        elif use_case == 'learning':
            # Focus on learning and plasticity
            preferences['neuron_count'] = self._get_int(
                "   Number of neurons (50-500): ", 200, 50, 500
            )
            preferences['steps'] = self._get_int(
                "   Number of simulation steps: ", 3000, 100, 100000
            )
            preferences['enable_reward'] = self._get_bool(
                "   Enable reward-based learning? ", True
            )
            preferences['enable_curiosity'] = self._get_bool(
                "   Enable curiosity-driven exploration? ", True
            )
            preferences['enable_development'] = self._get_bool(
                "   Enable developmental plasticity? ", True
            )
            preferences['enable_planning'] = self._get_bool(
                "   Enable learning planning? ", False
            )
            preferences['simulation_timestep'] = self._get_float(
                "   Simulation timestep (s): ", 0.001, 0.0001, 0.01
            )
            preferences['record_video'] = self._get_bool(
                "   Record video of learning process? ", True
            )
            preferences['detailed_logging'] = self._get_bool(
                "   Enable detailed learning logs? ", True
            )
            
        elif use_case == 'development':
            # Focus on development and maturation
            preferences['neuron_count'] = self._get_int(
                "   Number of neurons (50-300): ", 100, 50, 300
            )
            preferences['steps'] = self._get_int(
                "   Number of simulation steps: ", 10000, 1000, 100000
            )
            preferences['enable_reward'] = self._get_bool(
                "   Enable reward-based learning? ", False
            )
            preferences['enable_curiosity'] = self._get_bool(
                "   Enable curiosity-driven exploration? ", False
            )
            preferences['enable_development'] = self._get_bool(
                "   Enable developmental stages? ", True
            )
            preferences['enable_planning'] = self._get_bool(
                "   Enable developmental planning? ", False
            )
            preferences['simulation_timestep'] = self._get_float(
                "   Simulation timestep (s): ", 0.001, 0.0001, 0.01
            )
            preferences['record_video'] = self._get_bool(
                "   Record development progression? ", True
            )
            preferences['detailed_logging'] = self._get_bool(
                "   Enable developmental logs? ", True
            )
            
        elif use_case == 'behavior':
            # Focus on behavior and action
            preferences['neuron_count'] = self._get_int(
                "   Number of neurons (50-500): ", 300, 50, 500
            )
            preferences['steps'] = self._get_int(
                "   Number of simulation steps: ", 2000, 100, 100000
            )
            preferences['enable_reward'] = self._get_bool(
                "   Enable reward-based learning? ", True
            )
            preferences['enable_curiosity'] = self._get_bool(
                "   Enable curiosity-driven exploration? ", True
            )
            preferences['enable_development'] = self._get_bool(
                "   Enable developmental stages? ", False
            )
            preferences['enable_planning'] = self._get_bool(
                "   Enable action planning? ", True
            )
            preferences['simulation_timestep'] = self._get_float(
                "   Simulation timestep (s): ", 0.001, 0.0001, 0.01
            )
            preferences['record_video'] = self._get_bool(
                "   Record behavior video? ", True
            )
            preferences['detailed_logging'] = self._get_bool(
                "   Enable behavioral logging? ", True
            )
            
        else:  # custom
            # Fully custom configuration
            preferences['neuron_count'] = self._get_int(
                "   Number of neurons (10-1000): ", 100, 10, 1000
            )
            preferences['steps'] = self._get_int(
                "   Number of simulation steps: ", 1000, 100, 100000
            )
            preferences['enable_reward'] = self._get_bool(
                "   Enable reward-based learning? ", True
            )
            preferences['enable_curiosity'] = self._get_bool(
                "   Enable curiosity-driven exploration? ", True
            )
            preferences['enable_development'] = self._get_bool(
                "   Enable developmental stages? ", True
            )
            preferences['enable_planning'] = self._get_bool(
                "   Enable action planning? ", True
            )
            preferences['simulation_timestep'] = self._get_float(
                "   Simulation timestep (s): ", 0.001, 0.0001, 0.01
            )
            preferences['record_video'] = self._get_bool(
                "   Record video? ", False
            )
            preferences['detailed_logging'] = self._get_bool(
                "   Enable detailed logging? ", True
            )
        
        return preferences
    
    def _generate_config(self, use_case: str, preferences: Dict[str, Any]) -> Dict[str, Any]:
        """Generate configuration based on use case and preferences"""
        config = {
            'metadata': {
                'name': f"{use_case}_simulation",
                'description': self._get_use_case_description(use_case),
                'created': self._get_timestamp(),
                'created_by': getpass.getuser()
            },
            'simulation': {
                'neuron_count': preferences['neuron_count'],
                'steps': preferences['steps'],
                'simulation_timestep': preferences['simulation_timestep'],
                'random_seed': 42  # Fixed seed for reproducibility
            },
            'systems': {
                'reward_modulation': preferences['enable_reward'],
                'curiosity': preferences['enable_curiosity'],
                'development': preferences['enable_development'],
                'planning': preferences['enable_planning'],
                'prediction': True,
                'attention': True,
                'memory': {
                    'working_memory': True,
                    'episodic_memory': True,
                    'associative_memory': True
                }
            },
            'plasticity': {
                'stdp': True,
                'hebbian': True,
                'structural': True,
                'development': preferences['enable_development']
            },
            'output': {
                'record_video': preferences['record_video'],
                'video_format': 'mp4' if preferences['record_video'] else 'none',
                'detailed_logging': preferences['detailed_logging'],
                'save_checkpoints': True,
                'checkpoint_interval': 1000,
                'results_file': f"results/{use_case}_results.json"
            }
        }
        
        # Add use-case specific parameters
        if use_case == 'learning':
            config['plasticity']['learning_rate'] = 0.01
            config['systems']['reward_prediction'] = True
            
        elif use_case == 'development':
            config['development'] = {
                'enable_maturation': True,
                'enable_pruning': True,
                'enable_synaptogenesis': True,
                'critical_period': preferences['steps'] // 4
            }
            
        elif use_case == 'behavior':
            config['action'] = {
                'enable_motivation': True,
                'enable_curiosity_drive': True,
                'exploration_rate': 0.1,
                'exploitation_rate': 0.9
            }
        
        return config
    
    def _get_use_case_description(self, use_case: str) -> str:
        """Get description for use case"""
        descriptions = {
            'research': 'Comprehensive scientific simulation for research purposes',
            'learning': 'Focus on synaptic learning and memory formation',
            'development': 'Study brain development and maturation processes',
            'behavior': 'Analyze agent behavior and decision making',
            'quick_test': 'Fast test configuration for basic verification',
            'custom': 'User-defined custom configuration'
        }
        return descriptions.get(use_case, 'Custom simulation configuration')
    
    def _get_config_name(self, use_case: str, preferences: Dict[str, Any]) -> str:
        """Generate configuration name"""
        base_name = use_case
        neuron_count = preferences['neuron_count']
        steps = preferences['steps']
        
        return f"{base_name}_neurons{neuron_count}_steps{steps}"
    
    def _save_config(self, config_file: Path, config: Dict[str, Any]):
        """Save configuration to file"""
        try:
            config_file.parent.mkdir(parents=True, exist_ok=True)
            
            with open(config_file, 'w') as f:
                json.dump(config, f, indent=2)
                
            print(f"\n✓ Configuration saved to: {config_file}")
            print(f"  File size: {config_file.stat().st_size / 1024:.1f} KB")
            
        except Exception as e:
            print(f"\n✗ Error saving configuration: {e}")
            sys.exit(1)
    
    def _get_int(self, prompt: str, default: int, min_val: int, max_val: int) -> int:
        """Get integer input from user"""
        while True:
            try:
                value = input(prompt).strip()
                if not value:
                    return default
                
                int_val = int(value)
                if min_val <= int_val <= max_val:
                    return int_val
                else:
                    print(f"   Please enter a value between {min_val} and {max_val}")
            except ValueError:
                print("   Please enter a valid integer")
    
    def _get_float(self, prompt: str, default: float, min_val: float, max_val: float) -> float:
        """Get float input from user"""
        while True:
            try:
                value = input(prompt).strip()
                if not value:
                    return default
                
                float_val = float(value)
                if min_val <= float_val <= max_val:
                    return float_val
                else:
                    print(f"   Please enter a value between {min_val} and {max_val}")
            except ValueError:
                print("   Please enter a valid number")
    
    def _get_bool(self, prompt: str, default: bool) -> bool:
        """Get boolean input from user"""
        while True:
            value = input(prompt).strip()
            if not value:
                return default
            
            if value.lower() in ['y', 'yes', 'true', '1']:
                return True
            elif value.lower() in ['n', 'no', 'false', '0']:
                return False
            else:
                print("   Please enter 'y' (yes) or 'n' (no)")
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        from datetime import datetime
        return datetime.now().isoformat()

def main():
    """Main entry point"""
    wizard = ConfigWizard()
    wizard.run_wizard()

if __name__ == "__main__":
    main()