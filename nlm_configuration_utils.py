# NLM Configuration Management Utilities

This module provides advanced configuration management utilities for NLM (Neural Learning Machine) simulations, making it easier to manage complex experiment configurations and optimize brain performance.

## Overview

The configuration management system provides:

1. **Advanced configuration templates** for different types of experiments
2. **Configuration validation** and error handling
3. **Optimization tools** for finding best parameter combinations
4. **Parameter search capabilities** for hyperparameter tuning
5. **Configuration persistence** with backup and recovery
6. **Multi-environment support** for different simulation scenarios

## Key Features

### 1. Configuration Templates

#### Predefined Experiment Templates

```python
class ExperimentTemplates:
    """Predefined templates for common NLM experiments."""
    
    @staticmethod
    def create_exploratory_learning_template(neuron_count=1000, regions=2):
        """Template for exploratory learning experiments."""
        config = {
            'name': 'Exploratory Learning',
            'description': 'Ex exploratory learning with curiosity-driven exploration',
            'neuron_count': neuron_count,
            'region_count': regions,
            'connection_probability': 0.12,
            'stdp_ltp_weight': 0.025,
            'stdp_ltd_weight': 0.02,
            'synaptogenesis_rate': 0.0005,
            'pruning_rate': 0.00005,
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True,
            'enable_structural_plasticity': True,
            'developmental_stage': 'CriticalPeriod',
            'simulation_duration': 500,
            'world_size': {'width': 15, 'height': 15},
            'sensory_resolution': {'vision_width': 8, 'vision_height': 8}
        }
        return config
    
    @staticmethod
    def create_memory_consolidation_template(neuron_count=2000, regions=4):
        """Template for memory consolidation experiments."""
        config = {
            'name': 'Memory Consolidation',
            'description': 'Ex intensive memory consolidation and replay',
            'neuron_count': neuron_count,
            'region_count': regions,
            'connection_probability': 0.08,
            'stdp_ltp_weight': 0.018,
            'stdp_ltd_weight': 0.015,
            'synaptogenesis_rate': 0.0002,
            'pruning_rate': 0.00002,
            'replay_interval': 50,
            'consolidation_interval': 200,
            'enable_reward_modulation': False,
            'enable_curiosity': False,
            'enable_development': True,
            'enable_structural_plasticity': True,
            'developmental_stage': 'Maturation',
            'simulation_duration': 1000,
            'world_size': {'width': 20, 'height': 20},
            'sensory_resolution': {'vision_width': 12, 'vision_height': 12}
        }
        return config
    
    @staticmethod
    def create_social_cognition_template(neuron_count=1500, regions=3):
        """Template for social cognition experiments."""
        config = {
            'name': 'Social Cognition',
            'description': 'Ex social learning and theory of mind',
            'neuron_count': neuron_count,
            'region_count': regions,
            'connection_probability': 0.15,
            'stdp_ltp_weight': 0.03,
            'stdp_ltd_weight': 0.025,
            'synaptogenesis_rate': 0.0008,
            'pruning_rate': 0.00008,
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True,
            'enable_structural_plasticity': True,
            'developmental_stage': 'Adult',
            'simulation_duration': 800,
            'world_size': {'width': 25, 'height': 25},
            'sensory_resolution': {'vision_width': 10, 'vision_height': 10},
            'social_interactions': True,
            'multi_agent': True
        }
        return config

class AdvancedConfiguration:
    """Advanced configuration manager with validation and optimization."""
    
    def __init__(self):
        self.config_templates = ExperimentTemplates()
        self.validation_rules = self._load_validation_rules()
        self.optimization_history = []
        self.parameter_search_space = {}
    
    def _load_validation_rules(self):
        """Load configuration validation rules."""
        return {
            'neuron_count': {
                'type': 'int',
                'min': 10,
                'max': 10000,
                'default': 1000
            },
            'region_count': {
                'type': 'int',
                'min': 1,
                'max': 20,
                'default': 2
            },
            'connection_probability': {
                'type': 'float',
                'min': 0.01,
                'max': 0.5,
                'default': 0.1
            },
            'stdp_ltp_weight': {
                'type': 'float',
                'min': 0.001,
                'max': 0.1,
                'default': 0.01
            },
            'stdp_ltd_weight': {
                'type': 'float',
                'min': 0.001,
                'max': 0.1,
                'default': 0.01
            },
            'enable_reward_modulation': {
                'type': 'bool',
                'default': True
            },
            'enable_curiosity': {
                'type': 'bool',
                'default': True
            },
            'enable_development': {
                'type': 'bool',
                'default': True
            },
            'enable_structural_plasticity': {
                'type': 'bool',
                'default': True
            }
        }
    
    def create_config(self, template: Optional[str] = None, 
                     custom_params: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Create advanced configuration with template and custom parameters."""
        if template:
            # Create template configuration
            if template == 'exploratory':
                config = self.config_templates.create_exploratory_learning_template()
            elif template == 'memory':
                config = self.config_templates.create_memory_consolidation_template()
            elif template == 'social':
                config = self.config_templates.create_social_cognition_template()
            else:
                raise ValueError(f"Unknown template: {template}")
        else:
            # Create empty configuration
            config = {}
        
        # Apply custom parameters
        if custom_params:
            config.update(custom_params)
        
        # Validate configuration
        validated_config = self._validate_config(config)
        
        # Set defaults for missing parameters
        validated_config = self._apply_defaults(validated_config)
        
        # Perform parameter consistency checks
        self._check_parameter_consistency(validated_config)
        
        return validated_config
    
    def _validate_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration against rules."""
        validated = config.copy()
        
        for key, rules in self.validation_rules.items():
            if key in validated:
                # Type validation
                expected_type = rules['type']
                value = validated[key]
                
                if expected_type == 'int' and not isinstance(value, int):
                    try:
                        validated[key] = int(value)
                    except (ValueError, TypeError):
                        raise ValueError(f"Configuration parameter '{key}' must be an integer, got {type(value)}")
                
                elif expected_type == 'float' and not isinstance(value, (int, float)):
                    try:
                        validated[key] = float(value)
                    except (ValueError, TypeError):
                        raise ValueError(f"Configuration parameter '{key}' must be a number, got {type(value)}")
                
                elif expected_type == 'bool' and not isinstance(value, bool):
                    try:
                        validated[key] = bool(value)
                    except (ValueError, TypeError):
                        raise ValueError(f"Configuration parameter '{key}' must be a boolean, got {type(value)}")
                
                # Range validation
                if 'min' in rules and 'max' in rules:
                    if validated[key] < rules['min'] or validated[key] > rules['max']:
                        raise ValueError(f"Configuration parameter '{key}' must be between {rules['min']} and {rules['max']}, got {validated[key]}")
            else:
                # Set default
                validated[key] = rules.get('default', None)
        
        return validated
    
    def _apply_defaults(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Apply defaults for missing parameters."""
        defaults = {
            'simulation_duration': 500,
            'world_size': {'width': 15, 'height': 15},
            'sensory_resolution': {'vision_width': 8, 'vision_height': 8},
            'replay_interval': 100,
            'consolidation_interval': 1000,
            'developmental_stage': 'CriticalPeriod',
            'enable_structural_plasticity': True,
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True
        }
        
        for key, value in defaults.items():
            if key not in config:
                config[key] = value
        
        return config
    
    def _check_parameter_consistency(self, config: Dict[str, Any]):
        """Check for parameter consistency issues."""
        errors = []
        
        # Check for incompatible parameter combinations
        if config.get('enable_structural_plasticity', True):
            if config.get('neuron_count', 1000) > 5000:
                print("Warning: High neuron count with structural plasticity may cause performance issues")
            if config.get('stdp_ltp_weight', 0.01) > 0.05:
                print("Warning: High STDP LTP weight may cause runaway excitation")
        
        # Check for developmental stage consistency
        stage = config.get('developmental_stage', 'CriticalPeriod')
        if stage == 'Adult' and config.get('enable_development', False):
            print("Warning: Development enabled for adult stage may be redundant")
        
        # Check for simulation duration consistency
        duration = config.get('simulation_duration', 500)
        if duration > 5000:
            print("Warning: Very long simulation duration may be computationally expensive")
        
        return errors
    
    def optimize_configuration(self, objective_function: Callable[[Dict[str, Any]], float], 
                             param_ranges: Optional[Dict[str, Tuple[float, float]]] = None,
                             iterations: int = 50) -> Dict[str, Any]:
        """Optimize configuration using simple parameter search."""
        if param_ranges is None:
            param_ranges = {
                'neuron_count': (500, 5000),
                'region_count': (1, 10),
                'connection_probability': (0.05, 0.3),
                'stdp_ltp_weight': (0.01, 0.05),
                'stdp_ltd_weight': (0.01, 0.05),
                'synaptogenesis_rate': (0.0001, 0.001),
                'pruning_rate': (0.00001, 0.0001)
            }
        
        best_config = None
        best_score = float('-inf')
        
        print(f"Running parameter search for {iterations} iterations")
        
        for i in range(iterations):
            # Generate random configuration
            candidate_config = {}
            for param, (min_val, max_val) in param_ranges.items():
                if param in self.validation_rules:
                    # Generate integer or float based on rules
                    if self.validation_rules[param]['type'] == 'int':
                        candidate_config[param] = np.random.randint(min_val, max_val + 1)
                    else:
                        candidate_config[param] = np.random.uniform(min_val, max_val)
                else:
                    # Default ranges
                    if 'neuron' in param:
                        candidate_config[param] = np.random.randint(min_val, max_val + 1)
                    elif 'rate' in param or 'probability' in param:
                        candidate_config[param] = np.random.uniform(min_val, max_val)
            
            # Fill with defaults
            candidate_config.update(self._apply_defaults({}))
            
            # Evaluate objective function
            try:
                score = objective_function(candidate_config)
                
                if score > best_score:
                    best_score = score
                    best_config = candidate_config.copy()
                
                print(f"Iteration {i + 1}/{iterations}: Score = {score:.4f}, Best = {best_score:.4f}")
                
            except Exception as e:
                print(f"Iteration {i + 1}/{iterations}: Error - {e}")
                continue
        
        if best_config:
            print(f"\nOptimization completed!")
            print(f"Best score: {best_score:.4f}")
            print(f"Best configuration:")
            for key, value in best_config.items():
                print(f"  {key}: {value}")
        
        return best_config or {}
class ConfigurationValidator:
    """Comprehensive configuration validation and analysis."""
    
    def __init__(self):
        self.validation_rules = AdvancedConfiguration().validation_rules
    
    def validate_configuration(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Perform comprehensive configuration validation."""
        validation_results = {
            'is_valid': True,
            'warnings': [],
            'errors': [],
            'recommendations': [],
            'statistics': {}
        }
        
        # Check each parameter
        for param, rules in self.validation_rules.items():
            if param in config:
                value = config[param]
                
                # Type check
                if rules['type'] == 'int' and not isinstance(value, int):
                    validation_results['errors'].append(f"Parameter '{param}' must be integer, got {type(value)}")
                    validation_results['is_valid'] = False
                
                elif rules['type'] == 'float' and not isinstance(value, (int, float)):
                    validation_results['errors'].append(f"Parameter '{param}' must be number, got {type(value)}")
                    validation_results['is_valid'] = False
                
                elif rules['type'] == 'bool' and not isinstance(value, bool):
                    validation_results['errors'].append(f"Parameter '{param}' must be boolean, got {type(value)}")
                    validation_results['is_valid'] = False
                
                # Range check
                if 'min' in rules and 'max' in rules:
                    if value < rules['min'] or value > rules['max']:
                        validation_results['errors'].append(
                            f"Parameter '{param}' must be between {rules['min']} and {rules['max']}, got {value}"
                        )
                        validation_results['is_valid'] = False
                
                # Warning for extreme values
                if 'min' in rules and value < rules['min'] * 2:
                    validation_results['warnings'].append(
                        f"Parameter '{param}' ({value}) is much lower than minimum ({rules['min']})"
                    )
                
                if 'max' in rules and value > rules['max'] * 0.8:
                    validation_results['warnings'].append(
                        f"Parameter '{param}' ({value}) is close to maximum ({rules['max']})"
                    )
                
                # Recommendations
                if param == 'connection_probability' and value > 0.3:
                    validation_results['recommendations'].append(
                        "High connection probability may increase computational load"
                    )
                
                elif param == 'neuron_count' and value > 3000:
                    validation_results['recommendations'].append(
                        "High neuron count may require more computational resources"
                    )
                
                elif param == 'stdp_ltp_weight' and value > 0.04:
                    validation_results['recommendations'].append(
                        "High STDP LTP weight may cause unstable learning dynamics"
                    )
            else:
                validation_results['warnings'].append(
                    f"Parameter '{param}' not specified, using default"
                )
        
        # Calculate statistics
        validation_results['statistics'] = self._calculate_config_statistics(config)
        
        # Generate recommendations
        validation_results['recommendations'].extend(self._generate_general_recommendations(config))
        
        return validation_results
    
    def _calculate_config_statistics(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate statistics about configuration."""
        stats = {
            'total_parameters': len(config),
            'has_required_parameters': all(
                param in config for param in ['neuron_count', 'region_count', 'connection_probability']
            ),
            'plasticity_enabled': config.get('enable_structural_plasticity', True) or
                                 config.get('enable_reward_modulation', True) or
                                 config.get('enable_curiosity', True),
            'complexity_score': self._calculate_complexity_score(config)
        }
        
        return stats
    
    def _calculate_complexity_score(self, config: Dict[str, Any]) -> float:
        """Calculate complexity score based on configuration."""
        score = 0.0
        
        # Add complexity based on number of enabled features
        features = ['enable_structural_plasticity', 'enable_reward_modulation', 
                   'enable_curiosity', 'enable_development']
        enabled_features = sum(1 for feature in features if config.get(feature, True))
        score += enabled_features * 0.2
        
        # Add complexity based on parameter ranges
        if config.get('neuron_count', 1000) > 2000:
            score += 0.3
        
        if config.get('region_count', 2) > 4:
            score += 0.2
        
        if config.get('connection_probability', 0.1) > 0.2:
            score += 0.2
        
        return min(score, 1.0)  # Cap at 1.0
    
    def _generate_general_recommendations(self, config: Dict[str, Any]) -> List[str]:
        """Generate general recommendations for configuration."""
        recommendations = []
        
        # Recommendation based on neuron count
        neuron_count = config.get('neuron_count', 1000)
        if neuron_count < 100:
            recommendations.append("Consider increasing neuron count for more complex behavior")
        elif neuron_count > 5000:
            recommendations.append("Consider decreasing neuron count for better performance")
        
        # Recommendation based on connection probability
        conn_prob = config.get('connection_probability', 0.1)
        if conn_prob < 0.05:
            recommendations.append("Very low connection probability may result in disconnected networks")
        elif conn_prob > 0.3:
            recommendations.append("High connection probability may increase computational load")
        
        # Recommendation based on developmental stage
        stage = config.get('developmental_stage', 'CriticalPeriod')
        if stage not in ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']:
            recommendations.append(f"Unrecognized developmental stage: {stage}")
        
        # Recommendation for memory vs processing balance
        if not config.get('enable_structural_plasticity', True):
            recommendations.append("Disable structural plasticity may reduce network adaptability")
        
        return recommendations
    
    def generate_validation_report(self, config: Dict[str, Any]) -> str:
        """Generate comprehensive validation report."""
        validation = self.validate_configuration(config)
        
        report = f"Configuration Validation Report\n"
        report += f"{'='*50}\n"
        report += f"Validation Status: {'PASS' if validation['is_valid'] else 'FAIL'}\n\n"
        
        if validation['warnings']:
            report += f"Warnings ({len(validation['warnings'])}):\n"
            for warning in validation['warnings']:
                report += f"  - {warning}\n"
            report += "\n"
        
        if validation['errors']:
            report += f"Errors ({len(validation['errors'])}):\n"
            for error in validation['errors']:
                report += f"  - {error}\n"
            report += "\n"
        
        if validation['recommendations']:
            report += f"Recommendations ({len(validation['recommendations'])}):\n"
            for recommendation in validation['recommendations']:
                report += f"  - {recommendation}\n"
            report += "\n"
        
        stats = validation['statistics']
        report += f"Statistics:\n"
        report += f"  - Total parameters: {stats['total_parameters']}\n"
        report += f"  - Has required parameters: {'Yes' if stats['has_required_parameters'] else 'No'}\n"
        report += f"  - Plasticity enabled: {'Yes' if stats['plasticity_enabled'] else 'No'}\n"
        report += f"  - Complexity score: {stats['complexity_score']:.2f}\n"
        
        return report
class ParameterSearchSpace:
    """Define and manage parameter search spaces for optimization."""
    
    def __init__(self):
        self.search_space = {}
        self.bounds = {}
        self.distributions = {}
    
    def add_parameter(self, name: str, min_val: float, max_val: float, 
                     distribution: str = 'uniform', discrete: bool = False):
        """Add a parameter to the search space."""
        self.search_space[name] = {
            'min': min_val,
            'max': max_val,
            'distribution': distribution,
            'discrete': discrete
        }
        
        self.bounds[name] = (min_val, max_val)
        self.distributions[name] = distribution
    
    def sample_parameter(self, name: str) -> Any:
        """Sample a parameter value from its distribution."""
        if name not in self.search_space:
            raise ValueError(f"Parameter '{name}' not found in search space")
        
        param_config = self.search_space[name]
        min_val = param_config['min']
        max_val = param_config['max']
        distribution = param_config['distribution']
        discrete = param_config['discrete']
        
        if discrete:
            return np.random.randint(min_val, max_val + 1)
        elif distribution == 'uniform':
            return np.random.uniform(min_val, max_val)
        elif distribution == 'normal':
            # For normal distribution, clip to bounds
            value = np.random.normal((min_val + max_val) / 2, (max_val - min_val) / 6)
            return np.clip(value, min_val, max_val)
        elif distribution == 'log_uniform':
            # Log-uniform distribution
            log_min = np.log(min_val)
            log_max = np.log(max_val)
            return np.exp(np.random.uniform(log_min, log_max))
        else:
            raise ValueError(f"Unknown distribution: {distribution}")
    
    def sample_configuration(self) -> Dict[str, Any]:
        """Sample a complete configuration from the search space."""
        config = {}
        for param_name in self.search_space:
            config[param_name] = self.sample_parameter(param_name)
        return config
    
    def get_parameter_info(self, name: str) -> Dict[str, Any]:
        """Get information about a specific parameter."""
        if name not in self.search_space:
            raise ValueError(f"Parameter '{name}' not found in search space")
        
        return self.search_space[name].copy()
    
    def visualize_search_space(self, save_path: Optional[str] = None):
        """Visualize the search space (simplified version)."""
        try:
            import matplotlib.pyplot as plt
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            axes = axes.flatten()
            
            # Plot parameter distributions
            param_names = list(self.search_space.keys())
            
            for i, (name, config) in enumerate(self.search_space.items()):
                if i >= len(param_names):
                    break
                
                ax = axes[i]
                
                # Generate samples for visualization
                samples = []
                for _ in range(1000):
                    samples.append(self.sample_parameter(name))
                
                ax.hist(samples, bins=30, alpha=0.7, edgecolor='black')
                ax.set_title(f'{name} Distribution')
                ax.set_xlabel(name)
                ax.set_ylabel('Frequency')
                
                # Add vertical lines for bounds
                ax.axvline(x=config['min'], color='red', linestyle='--', alpha=0.7)
                ax.axvline(x=config['max'], color='red', linestyle='--', alpha=0.7)
            
            # Hide unused subplots
            for i in range(len(param_names), len(axes)):
                axes[i].set_visible(False)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
                print(f"Search space visualization saved to {save_path}")
            
            plt.show()
            
        except ImportError:
            print("Matplotlib not available for visualization")
        except Exception as e:
            print(f"Error visualizing search space: {e}")
class ConfigurationBackupManager:
    """Manage configuration backups and recovery."""
    
    def __init__(self, backup_dir: str = "config_backups"):
        self.backup_dir = backup_dir
        os.makedirs(self.backup_dir, exist_ok=True)
        self.backup_history = []
    
    def create_backup(self, config: Dict[str, Any], description: str = "") -> str:
        """Create a backup of a configuration."""
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        
        backup_data = {
            'timestamp': timestamp,
            'description': description,
            'config': config,
            'version': '1.0'
        }
        
        backup_filename = f"backup_{timestamp}.json"
        backup_path = os.path.join(self.backup_dir, backup_filename)
        
        try:
            with open(backup_path, 'w') as f:
                json.dump(backup_data, f, indent=2, default=str)
            
            self.backup_history.append({
                'timestamp': timestamp,
                'description': description,
                'path': backup_path,
                'config_hash': self._calculate_config_hash(config)
            })
            
            print(f"Configuration backup created: {backup_path}")
            return backup_path
            
        except Exception as e:
            print(f"Failed to create backup: {e}")
            return ""
    
    def load_backup(self, backup_path: str) -> Optional[Dict[str, Any]]:
        """Load a configuration from backup."""
        try:
            if not os.path.exists(backup_path):
                print(f"Backup file not found: {backup_path}")
                return None
            
            with open(backup_path, 'r') as f:
                backup_data = json.load(f)
            
            print(f"Configuration loaded from backup: {backup_path}")
            return backup_data.get('config', {})
            
        except Exception as e:
            print(f"Failed to load backup: {e}")
            return None
    
    def list_backups(self) -> List[Dict[str, Any]]:
        """List all available backups."""
        backups = []
        
        for filename in os.listdir(self.backup_dir):
            if filename.startswith('backup_') and filename.endswith('.json'):
                filepath = os.path.join(self.backup_dir, filename)
                try:
                    with open(filepath, 'r') as f:
                        backup_data = json.load(f)
                    
                    backups.append({
                        'timestamp': backup_data['timestamp'],
                        'description': backup_data['description'],
                        'path': filepath,
                        'config_keys': list(backup_data['config'].keys())
                    })
                except Exception as e:
                    print(f"Error reading backup {filename}: {e}")
        
        # Sort by timestamp (newest first)
        backups.sort(key=lambda x: x['timestamp'], reverse=True)
        return backups
    
    def restore_from_backup(self, backup_path: str, merge_with: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Restore configuration from backup, optionally merging with new config."""
        backup_config = self.load_backup(backup_path)
        
        if backup_config is None:
            raise ValueError(f"Could not load backup from {backup_path}")
        
        if merge_with:
            # Merge backup with new config (new config takes precedence)
            restored_config = backup_config.copy()
            restored_config.update(merge_with)
        else:
            restored_config = backup_config
        
        print(f"Configuration restored from backup: {backup_path}")
        return restored_config
    
    def _calculate_config_hash(self, config: Dict[str, Any]) -> str:
        """Calculate hash of configuration for integrity checking."""
        import hashlib
        
        config_str = json.dumps(config, sort_keys=True, default=str)
        return hashlib.md5(config_str.encode()).hexdigest()
    
    def cleanup_old_backups(self, keep_last_n: int = 10):
        """Clean up old backups, keeping only the last N."""
        backups = self.list_backups()
        
        if len(backups) <= keep_last_n:
            return
        
        # Remove oldest backups
        for backup in backups[keep_last_n:]:
            try:
                os.remove(backup['path'])
                print(f"Removed old backup: {backup['path']}")
            except Exception as e:
                print(f"Error removing backup {backup['path']}: {e}")
class ExperimentConfigurationManager:
    """Manager for experiment-specific configurations."""
    
    def __init__(self):
        self.experiments = {}
        self.current_experiment = None
    
    def create_experiment(self, name: str, config: Dict[str, Any], 
                         description: str = ""):
        """Create a new experiment configuration."""
        experiment = {
            'name': name,
            'description': description,
            'config': config,
            'created_at': datetime.datetime.now(),
            'last_modified': datetime.datetime.now(),
            'version': '1.0'
        }
        
        self.experiments[name] = experiment
        self.current_experiment = name
        
        print(f"Experiment created: {name}")
        return experiment
    
    def load_experiment(self, name: str) -> Optional[Dict[str, Any]]:
        """Load an experiment configuration."""
        if name not in self.experiments:
            print(f"Experiment not found: {name}")
            return None
        
        self.current_experiment = name
        print(f"Experiment loaded: {name}")
        return self.experiments[name]['config']
    
    def update_experiment(self, name: str, config: Dict[str, Any], 
                         version_increment: bool = True):
        """Update an experiment configuration."""
        if name not in self.experiments:
            raise ValueError(f"Experiment not found: {name}")
        
        old_version = self.experiments[name]['version']
        
        # Update experiment
        self.experiments[name]['config'] = config
        self.experiments[name]['last_modified'] = datetime.datetime.now()
        
        if version_increment:
            # Increment version (simple implementation)
            major, minor = map(int, old_version.split('.'))
            self.experiments[name]['version'] = f"{major}.{minor + 1}"
        
        print(f"Experiment updated: {name}")
        return self.experiments[name]
    
    def delete_experiment(self, name: str) -> bool:
        """Delete an experiment configuration."""
        if name not in self.experiments:
            print(f"Experiment not found: {name}")
            return False
        
        del self.experiments[name]
        
        if self.current_experiment == name:
            self.current_experiment = None
        
        print(f"Experiment deleted: {name}")
        return True
    
    def list_experiments(self) -> List[Dict[str, Any]]:
        """List all experiments."""
        experiments_list = []
        
        for name, experiment in self.experiments.items():
            experiments_list.append({
                'name': name,
                'description': experiment['description'],
                'version': experiment['version'],
                'last_modified': experiment['last_modified'],
                'config_keys': list(experiment['config'].keys())
            })
        
        # Sort by last modified date
        experiments_list.sort(key=lambda x: x['last_modified'], reverse=True)
        return experiments_list
    
    def export_experiments(self, filepath: str):
        """Export all experiments to file."""
        export_data = {
            'experiments': self.experiments,
            'export_date': datetime.datetime.now(),
            'version': '1.0'
        }
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"Experiments exported to {filepath}")
    
    def import_experiments(self, filepath: str):
        """Import experiments from file."""
        try:
            with open(filepath, 'r') as f:
                import_data = json.load(f)
            
            # Import experiments
            for name, experiment in import_data.get('experiments', {}).items():
                self.experiments[name] = experiment
            
            print(f"Experiments imported from {filepath}")
            return len(import_data.get('experiments', {}))
            
        except Exception as e:
            print(f"Failed to import experiments: {e}")
            return 0
# Main configuration management toolkit
class ConfigurationToolkit:
    """Main toolkit providing access to all configuration management utilities."""
    
    def __init__(self):
        self.advanced_config = AdvancedConfiguration()
        self.validator = ConfigurationValidator()
        self.parameter_search = ParameterSearchSpace()
        self.backup_manager = ConfigurationBackupManager()
        self.experiment_manager = ExperimentConfigurationManager()
    
    def create_advanced_config(self, template: Optional[str] = None, 
                             custom_params: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Create advanced configuration."""
        return self.advanced_config.create_config(template, custom_params)
    
    def validate_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration."""
        return self.validator.validate_configuration(config)
    
    def generate_validation_report(self, config: Dict[str, Any]) -> str:
        """Generate validation report."""
        return self.validator.generate_validation_report(config)
    
    def optimize_config(self, objective_function: Callable[[Dict[str, Any]], float],
                      param_ranges: Optional[Dict[str, Tuple[float, float]]] = None,
                      iterations: int = 50) -> Dict[str, Any]:
        """Optimize configuration."""
        return self.advanced_config.optimize_configuration(objective_function, param_ranges, iterations)
    
    def create_backup(self, config: Dict[str, Any], description: str = "") -> str:
        """Create configuration backup."""
        return self.backup_manager.create_backup(config, description)
    
    def load_backup(self, backup_path: str) -> Optional[Dict[str, Any]]:
        """Load configuration from backup."""
        return self.backup_manager.load_backup(backup_path)
    
    def create_experiment(self, name: str, config: Dict[str, Any], 
                         description: str = "") -> Dict[str, Any]:
        """Create experiment."""
        return self.experiment_manager.create_experiment(name, config, description)
    
    def run_configuration_optimization_study(self, objective_function: Callable[[Dict[str, Any]], float],
                                           param_ranges: Optional[Dict[str, Tuple[float, float]]] = None,
                                           num_studies: int = 5) -> List[Dict[str, Any]]:
        """Run multiple configuration optimization studies."""
        studies = []
        
        for study in range(num_studies):
            print(f"\nRunning optimization study {study + 1}/{num_studies}")
            
            optimized_config = self.optimize_config(objective_function, param_ranges, 50)
            
            study_result = {
                'study': study + 1,
                'config': optimized_config,
                'score': objective_function(optimized_config) if optimized_config else None
            }
            
            studies.append(study_result)
        
        return studies
    
    def export_all_data(self, export_dir: str = "exports"):
        """Export all configuration data."""
        os.makedirs(export_dir, exist_ok=True)
        
        # Export experiments
        experiments_file = os.path.join(export_dir, "experiments.json")
        self.experiment_manager.export_experiments(experiments_file)
        
        # Export backup history
        backups = self.backup_manager.list_backups()
        if backups:
            backups_file = os.path.join(export_dir, "backups.json")
            with open(backups_file, 'w') as f:
                json.dump(backups, f, indent=2, default=str)
        
        # Export parameter search space
        search_space_file = os.path.join(export_dir, "search_space.json")
        with open(search_space_file, 'w') as f:
            json.dump(self.parameter_search.search_space, f, indent=2)
        
        print(f"All data exported to {export_dir}")
# Global configuration toolkit instance
_config_toolkit = ConfigurationToolkit()

# Convenience functions
def create_advanced_config(template: Optional[str] = None, 
                        custom_params: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """Create advanced configuration."""
    return _config_toolkit.create_advanced_config(template, custom_params)

def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
    """Validate configuration."""
    return _config_toolkit.validate_config(config)

def generate_validation_report(config: Dict[str, Any]) -> str:
    """Generate validation report."""
    return _config_toolkit.generate_validation_report(config)

def create_backup(config: Dict[str, Any], description: str = "") -> str:
    """Create configuration backup."""
    return _config_toolkit.create_backup(config, description)

def load_backup(backup_path: str) -> Optional[Dict[str, Any]]:
    """Load configuration from backup."""
    return _config_toolkit.load_backup(backup_path)

# Module information
__version__ = "2.0.0"
__author__ = "NLM Configuration Team"
__description__ = "Advanced configuration management toolkit for NLM experiments"

# Export public API
__all__ = [
    'ExperimentTemplates',
    'AdvancedConfiguration',
    'ConfigurationValidator',
    'ParameterSearchSpace',
    'ConfigurationBackupManager',
    'ExperimentConfigurationManager',
    'ConfigurationToolkit',
    'create_advanced_config',
    'validate_config',
    'generate_validation_report',
    'create_backup',
    'load_backup'
]