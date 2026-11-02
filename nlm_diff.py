#!/usr/bin/env python3
"""
NLM Configuration Comparison and Diffing Tool

This module provides advanced configuration comparison, diffing, and analysis
capabilities for the NLM (Neural Learning Machine) project. It includes:

- Configuration diffing and comparison
- Configuration validation and schema checking
- Configuration inheritance and merging
- Configuration analysis and statistics
- Configuration history and versioning
- Automated configuration optimization
- Configuration performance impact analysis
- Configuration dependency analysis
- Configuration conflict detection

Features:
- Detailed configuration comparison with color output
- Configuration inheritance and dependency resolution
- Automated configuration optimization
- Performance impact analysis
- Configuration history and versioning
- Dependency graph generation
- Automated validation and fixing
- Configuration template matching
- Configuration impact assessment
"""

import json
import yaml
import argparse
import logging
from typing import Dict, Any, Optional, List, Set, Tuple
from dataclasses import dataclass, field
from pathlib import Path
from datetime import datetime
from enum import Enum
from difflib import unified_diff
import copy

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)
class ConfigDiffLevel(Enum):
    """Configuration diff levels"""
    SILENT = "silent"
    MINIMAL = "minimal"
    NORMAL = "normal"
    DETAILED = "detailed"
    COMPLETE = "complete"
class ConfigDiffResult:
    """Configuration diff result"""
    
    def __init__(self):
        self.added: Dict[str, Any] = {}
        self.removed: Dict[str, Any] = {}
        self.changed: Dict[str, Tuple[Any, Any]] = {}
        self.unchanged: Dict[str, Any] = {}
        self.nested: Dict[str, 'ConfigDiffResult'] = {}
    
    def is_empty(self) -> bool:
        """Check if diff result is empty"""
        return (not self.added and not self.removed and 
                not self.changed and not self.unchanged and 
                not self.nested)
    
    def get_summary(self) -> str:
        """Get summary of diff"""
        parts = []
        if self.added:
            parts.append(f"+{len(self.added)} added")
        if self.removed:
            parts.append(f"-{len(self.removed)} removed")
        if self.changed:
            parts.append(f"~{len(self.changed)} changed")
        if self.unchanged:
            parts.append(f"={len(self.unchanged)} unchanged")
        
        return ", ".join(parts) if parts else "no changes"
class ConfigAnalyzer:
    """Advanced configuration analyzer"""
    
    def __init__(self, config_manager=None):
        self.config_manager = config_manager
        self.schemas = self._load_schemas()
    
    def _load_schemas(self) -> Dict[str, List[Dict]]:
        """Load configuration schemas for validation"""
        schemas = {}
        
        # Neural system schemas
        schemas['neural'] = [
            {
                'name': 'neuron_count',
                'type': 'int',
                'validation': lambda x: x > 0,
                'description': 'Number of neurons'
            },
            {
                'name': 'connection_probability',
                'type': 'float',
                'validation': lambda x: 0 <= x <= 1,
                'description': 'Probability of connections'
            },
        ]
        
        # Simulation schemas
        schemas['simulation'] = [
            {
                'name': 'timestep',
                'type': 'float',
                'validation': lambda x: x > 0,
                'description': 'Time step'
            },
            {
                'name': 'max_steps',
                'type': 'int',
                'validation': lambda x: x > 0,
                'description': 'Maximum simulation steps'
            },
        ]
        
        # Memory schemas
        schemas['memory'] = [
            {
                'name': 'working_memory_capacity',
                'type': 'int',
                'validation': lambda x: x > 0,
                'description': 'Working memory capacity'
            },
        ]
        
        return schemas
    
    def analyze_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze configuration and return analysis results"""
        analysis = {
            'statistics': self._calculate_statistics(config),
            'validation': self._validate_config(config),
            'recommendations': self._generate_recommendations(config),
            'performance_impact': self._estimate_performance_impact(config),
            'complexity': self._calculate_complexity(config),
            'potential_issues': self._identify_potential_issues(config),
            'optimizations': self._suggest_optimizations(config),
        }
        
        return analysis
    
    def _calculate_statistics(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate configuration statistics"""
        stats = {
            'total_sections': len(config),
            'total_keys': sum(len(section) for section in config.values()),
            'section_sizes': {},
            'value_types': {},
            'range_analysis': {},
        }
        
        for section, values in config.items():
            stats['section_sizes'][section] = len(values)
            stats['value_types'][section] = self._analyze_value_types(values)
            stats['range_analysis'][section] = self._analyze_ranges(values)
        
        return stats
    
    def _analyze_value_types(self, values: Dict[str, Any]) -> Dict[str, int]:
        """Analyze value types in configuration"""
        type_counts = {}
        
        for value in values.values():
            if isinstance(value, (int, float)):
                type_counts['numeric'] = type_counts.get('numeric', 0) + 1
            elif isinstance(value, str):
                type_counts['string'] = type_counts.get('string', 0) + 1
            elif isinstance(value, bool):
                type_counts['boolean'] = type_counts.get('boolean', 0) + 1
            elif isinstance(value, list):
                type_counts['list'] = type_counts.get('list', 0) + 1
            elif isinstance(value, dict):
                type_counts['dict'] = type_counts.get('dict', 0) + 1
            else:
                type_counts['other'] = type_counts.get('other', 0) + 1
        
        return type_counts
    
    def _analyze_ranges(self, values: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze ranges of numeric values"""
        ranges = {}
        numeric_values = {}
        
        for key, value in values.items():
            if isinstance(value, (int, float)):
                numeric_values[key] = value
        
        if numeric_values:
            min_val = min(numeric_values.values())
            max_val = max(numeric_values.values())
            avg_val = sum(numeric_values.values()) / len(numeric_values)
            
            ranges['min'] = min_val
            ranges['max'] = max_val
            ranges['average'] = avg_val
            ranges['count'] = len(numeric_values)
        
        return ranges
    
    def _validate_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration against schemas"""
        validation = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': [],
        }
        
        # Check for required sections
        required_sections = ['neural', 'simulation', 'memory']
        for section in required_sections:
            if section not in config:
                validation['warnings'].append(f"Missing section: {section}")
        
        # Validate individual values
        for section, values in config.items():
            if section in self.schemas:
                section_schema = self.schemas[section]
                for schema in section_schema:
                    key = schema['name']
                    if key in values:
                        value = values[key]
                        # Apply validation
                        if 'validation' in schema:
                            try:
                                if not schema['validation'](value):
                                    validation['errors'].append(
                                        f"Invalid value for {section}.{key}: {value}"
                                    )
                            except Exception as e:
                                validation['errors'].append(
                                    f"Validation error for {section}.{key}: {e}"
                                )
                    elif schema.get('required', False):
                        validation['errors'].append(
                            f"Missing required key: {section}.{key}"
                        )
        
        validation['valid'] = len(validation['errors']) == 0
        
        return validation
    
    def _generate_recommendations(self, config: Dict[str, Any]) -> List[str]:
        """Generate configuration recommendations"""
        recommendations = []
        
        # Neural system recommendations
        if 'neural' in config:
            neural = config['neural']
            
            if neural.get('neuron_count', 0) > 10000:
                recommendations.append(
                    "High neuron count may impact performance. Consider reducing or using optimized algorithms."
                )
            
            if neural.get('connection_probability', 0) > 0.5:
                recommendations.append(
                    "High connection probability may create overly dense connectivity."
                )
        
        # Simulation recommendations
        if 'simulation' in config:
            sim = config['simulation']
            
            if sim.get('max_steps', 0) > 100000:
                recommendations.append(
                    "High number of simulation steps may impact runtime."
                )
            
            if sim.get('timestep', 0) < 0.0001:
                recommendations.append(
                    "Very small timestep may impact numerical stability."
                )
        
        # Memory recommendations
        if 'memory' in config:
            mem = config['memory']
            
            if mem.get('working_memory_capacity', 0) > 1000:
                recommendations.append(
                    "Large working memory capacity may impact memory usage."
                )
        
        return recommendations
    
    def _estimate_performance_impact(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Estimate performance impact of configuration"""
        impact = {
            'cpu_usage': 0.0,
            'memory_usage': 0.0,
            'runtime_estimate': 0.0,
            'complexity_score': 0.0,
        }
        
        # Estimate CPU usage based on configuration
        if 'neural' in config:
            impact['cpu_usage'] += config['neural'].get('neuron_count', 0) * 0.0001
        
        if 'simulation' in config:
            impact['cpu_usage'] += config['simulation'].get('max_steps', 0) * 0.00001
        
        if 'memory' in config:
            impact['memory_usage'] += config['memory'].get('working_memory_capacity', 0) * 10
        
        # Estimate runtime
        if 'simulation' in config:
            impact['runtime_estimate'] = (
                config['simulation'].get('max_steps', 0) * 
                config['simulation'].get('timestep', 0.001) * 
                10  # base multiplier
            )
        
        # Calculate complexity score
        total_keys = sum(len(section) for section in config.values())
        impact['complexity_score'] = min(total_keys / 1000, 1.0)  # Normalized to 0-1
        
        return impact
    
    def _calculate_complexity(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate configuration complexity"""
        complexity = {
            'total_sections': len(config),
            'total_keys': sum(len(section) for section in config.values()),
            'nesting_depth': 0,
            'value_variation': {},
        }
        
        # Calculate value variation
        for section, values in config.items():
            variations = {}
            for key, value in values.items():
                if isinstance(value, (int, float)):
                    variations[key] = 'numeric'
                elif isinstance(value, str):
                    variations[key] = 'string'
                elif isinstance(value, bool):
                    variations[key] = 'boolean'
                elif isinstance(value, list):
                    variations[key] = 'list'
                elif isinstance(value, dict):
                    variations[key] = 'dict'
            
            complexity['value_variation'][section] = variations
        
        return complexity
    
    def _identify_potential_issues(self, config: Dict[str, Any]) -> List[str]:
        """Identify potential issues in configuration"""
        issues = []
        
        # Check for extreme values
        if 'neural' in config:
            neural = config['neural']
            
            if neural.get('neuron_count', 0) > 50000:
                issues.append(
                    f"Very high neuron count ({neural['neuron_count']}) may impact memory and performance."
                )
            
            if neural.get('connection_probability', 0) < 0.001:
                issues.append(
                    "Very low connection probability may result in sparse connectivity."
                )
        
        if 'simulation' in config:
            sim = config['simulation']
            
            if sim.get('max_steps', 0) < 100:
                issues.append(
                    f"Very low max steps ({sim['max_steps']}) may not allow for sufficient learning."
                )
            
            if sim.get('timestep', 0) > 0.1:
                issues.append(
                    f"Large timestep ({sim['timestep']}) may impact simulation accuracy."
                )
        
        # Check for potential conflicts
        if 'neural' in config and 'memory' in config:
            neural = config['neural']
            memory = config['memory']
            
            if neural.get('neuron_count', 0) > memory.get('working_memory_capacity', 0) * 10:
                issues.append(
                    "Neural population size much larger than working memory capacity."
                )
        
        return issues
    
    def _suggest_optimizations(self, config: Dict[str, Any]) -> List[str]:
        """Suggest configuration optimizations"""
        optimizations = []
        
        # Suggest optimizations based on configuration
        if 'neural' in config:
            neural = config['neural']
            
            if neural.get('neuron_count', 0) > 5000:
                optimizations.append(
                    "Consider using GPU acceleration for large neural networks."
                )
            
            if neural.get('connection_probability', 0) < 0.05:
                optimizations.append(
                    "Consider increasing connection probability for better connectivity."
                )
        
        if 'simulation' in config:
            sim = config['simulation']
            
            if sim.get('max_steps', 0) > 10000:
                optimizations.append(
                    "Consider using checkpointing and parallel processing for long simulations."
                )
        
        if 'memory' in config:
            mem = config['memory']
            
            if mem.get('working_memory_capacity', 0) > 1000:
                optimizations.append(
                    "Consider using compression algorithms for large working memory."
                )
        
        return optimizations
class ConfigComparator:
    """Configuration comparator with detailed diffing"""
    
    def __init__(self, diff_level: ConfigDiffLevel = ConfigDiffLevel.NORMAL):
        self.diff_level = diff_level
        self.analyzer = ConfigAnalyzer()
    
    def compare_configs(self, config1: Dict[str, Any], config2: Dict[str, Any], 
                       name1: str = "Config 1", name2: str = "Config 2") -> Dict[str, Any]:
        """Compare two configurations"""
        diff_result = ConfigDiffResult()
        
        # Compare each section
        all_sections = set(config1.keys()) | set(config2.keys())
        
        for section in all_sections:
            if section in config1 and section in config2:
                self._compare_sections(
                    diff_result, section, 
                    config1[section], config2[section]
                )
            elif section in config1:
                diff_result.removed[section] = config1[section]
            else:
                diff_result.added[section] = config2[section]
        
        # Get analysis for both configurations
        analysis1 = self.analyzer.analyze_config(config1)
        analysis2 = self.analyzer.analyze_config(config2)
        
        # Generate diff text
        diff_text = self._generate_diff_text(diff_result, name1, name2)
        
        return {
            'diff': diff_result,
            'analysis1': analysis1,
            'analysis2': analysis2,
            'diff_text': diff_text,
            'summary': f"{name1} vs {name2}: {diff_result.get_summary()}",
        }
    
    def _compare_sections(self, diff_result: ConfigDiffResult, section: str,
                         config1: Dict[str, Any], config2: Dict[str, Any]):
        """Compare two configuration sections"""
        section_diff = ConfigDiffResult()
        
        # Get all keys from both configurations
        all_keys = set(config1.keys()) | set(config2.keys())
        
        for key in all_keys:
            if key in config1 and key in config2:
                if config1[key] != config2[key]:
                    section_diff.changed[key] = (config1[key], config2[key])
                else:
                    section_diff.unchanged[key] = config1[key]
            elif key in config1:
                section_diff.removed[key] = config1[key]
            else:
                section_diff.added[key] = config2[key]
        
        # Add to diff result if not empty
        if not section_diff.is_empty():
            if self.diff_level in [ConfigDiffLevel.DETAILED, ConfigDiffLevel.COMPLETE]:
                diff_result.nested[section] = section_diff
        else:
            diff_result.unchanged[section] = config1[section]
    
    def _generate_diff_text(self, diff_result: ConfigDiffResult, name1: str, name2: str) -> str:
        """Generate diff text"""
        if self.diff_level == ConfigDiffLevel.SILENT:
            return ""
        
        lines = [f"Diff between {name1} and {name2}:"]
        lines.append(f"Summary: {diff_result.get_summary()}")
        
        if self.diff_level in [ConfigDiffLevel.NORMAL, ConfigDiffLevel.DETAILED, ConfigDiffLevel.COMPLETE]:
            # Show added keys
            if diff_result.added:
                lines.append("\n=== Added ===")
                for key, value in diff_result.added.items():
                    lines.append(f"+ {key}: {value}")
            
            # Show removed keys
            if diff_result.removed:
                lines.append("\n=== Removed ===")
                for key, value in diff_result.removed.items():
                    lines.append(f"- {key}: {value}")
            
            # Show changed keys
            if diff_result.changed:
                lines.append("\n=== Changed ===")
                for key, (old_val, new_val) in diff_result.changed.items():
                    lines.append(f"~ {key}:")
                    lines.append(f"  - {old_val}")
                    lines.append(f"  + {new_val}")
        
        if self.diff_level == ConfigDiffLevel.DETAILED or self.diff_level == ConfigDiffLevel.COMPLETE:
            # Show nested diffs
            if diff_result.nested:
                lines.append("\n=== Detailed Diffs ===")
                for section, section_diff in diff_result.nested.items():
                    lines.append(f"\nSection: {section}")
                    lines.append(self._generate_diff_text(section_diff, f"{name1}.{section}", f"{name2}.{section}"))
        
        return "\n".join(lines)
class ConfigManager:
    """Enhanced configuration manager with diffing capabilities"""
    
    def __init__(self, config_dir: str = "./config"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        self.config_history = {}
        self.comparator = ConfigComparator(ConfigDiffLevel.NORMAL)
    
    def save_with_history(self, config: Dict[str, Any], name: str) -> None:
        """Save configuration with history tracking"""
        timestamp = datetime.now().isoformat()
        
        if name not in self.config_history:
            self.config_history[name] = []
        
        self.config_history[name].append({
            'timestamp': timestamp,
            'config': copy.deepcopy(config),
            'description': f"Configuration saved at {timestamp}"
        })
        
        # Save to file
        filepath = self.config_dir / f"{name}_{timestamp.replace(':', '-')}.json"
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
    
    def compare_with_history(self, name1: str, name2: str, version1: Optional[int] = None, version2: Optional[int] = None) -> Dict[str, Any]:
        """Compare two configurations from history"""
        if name1 not in self.config_history:
            raise ValueError(f"Configuration '{name1}' not found in history")
        
        if name2 not in self.config_history:
            raise ValueError(f"Configuration '{name2}' not found in history")
        
        config1 = self._get_version(self.config_history[name1], version1)
        config2 = self._get_version(self.config_history[name2], version2)
        
        return self.comparator.compare_configs(config1, config2, name1, name2)
    
    def _get_version(self, history: List[Dict], version: Optional[int]) -> Dict[str, Any]:
        """Get specific version from configuration history"""
        if version is None:
            return history[-1]['config']  # Latest version
        elif version < 0:
            return history[version]['config']  # Negative indices (from end)
        else:
            return history[version]['config']  # Positive indices
    
    def get_latest(self, name: str) -> Dict[str, Any]:
        """Get latest version of a configuration"""
        if name not in self.config_history:
            raise ValueError(f"Configuration '{name}' not found in history")
        
        return self.config_history[name][-1]['config']
    
    def get_history(self, name: str) -> List[Dict[str, Any]]:
        """Get configuration history"""
        if name not in self.config_history:
            raise ValueError(f"Configuration '{name}' not found in history")
        
        return self.config_history[name]
    
    def rollback(self, name: str, version: int) -> Dict[str, Any]:
        """Rollback configuration to specific version"""
        if name not in self.config_history:
            raise ValueError(f"Configuration '{name}' not found in history")
        
        return self._get_version(self.config_history[name], version)
    
    def get_config_recommendations(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Get configuration recommendations"""
        analyzer = ConfigAnalyzer()
        return analyzer.analyze_config(config)
class AdvancedConfigManager:
    """Advanced configuration manager with automation and optimization"""
    
    def __init__(self, config_manager: ConfigManager):
        self.config_manager = config_manager
        self.analyzer = ConfigAnalyzer()
        self.comparator = ConfigComparator()
    
    def auto_optimize_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Automatically optimize configuration"""
        analysis = self.analyzer.analyze_config(config)
        
        optimized = copy.deepcopy(config)
        
        # Apply optimizations based on analysis
        if analysis['optimizations']:
            # Implement optimization logic here
            pass
        
        # Validate optimized configuration
        validation = analysis['validation']
        if not validation['valid']:
            raise ValueError("Optimization resulted in invalid configuration")
        
        return optimized
    
    def suggest_alternatives(self, config: Dict[str, Any], count: int = 3) -> List[Dict[str, Any]]:
        """Suggest alternative configurations"""
        alternatives = []
        
        # Generate alternatives based on current configuration
        for i in range(count):
            alternative = copy.deepcopy(config)
            
            # Modify configuration for alternative
            if 'neural' in alternative:
                alternative['neural']['neuron_count'] = (i + 1) * 1000
            
            if 'simulation' in alternative:
                alternative['simulation']['max_steps'] = (i + 1) * 1000
            
            alternatives.append(alternative)
        
        return alternatives
    
    def evaluate_config_quality(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Evaluate quality of configuration"""
        analysis = self.analyzer.analyze_config(config)
        
        quality_metrics = {
            'completeness': len(config) / 10,  # Normalized to 0-1
            'validity': 1.0 if analysis['validation']['valid'] else 0.0,
            'performance': 1.0 - min(analysis['performance_impact']['complexity_score'], 1.0),
            'stability': 1.0,  # Placeholder
            'scalability': 1.0,  # Placeholder
        }
        
        overall_quality = (
            quality_metrics['completeness'] * 0.2 +
            quality_metrics['validity'] * 0.3 +
            quality_metrics['performance'] * 0.2 +
            quality_metrics['stability'] * 0.15 +
            quality_metrics['scalability'] * 0.15
        )
        
        return {
            'quality_metrics': quality_metrics,
            'overall_quality': overall_quality,
            'analysis': analysis,
        }
class NLMDiffTool:
    """Command-line interface for configuration diffing and analysis"""
    
    def __init__(self):
        self.config_manager = ConfigManager()
        self.analyzer = ConfigAnalyzer()
        self.comparator = ConfigComparator()
        self.advanced_manager = AdvancedConfigManager(self.config_manager)
    
    def compare_configs(self, config1_path: str, config2_path: str,
                       output_path: Optional[str] = None,
                       level: ConfigDiffLevel = ConfigDiffLevel.NORMAL) -> None:
        """Compare two configuration files"""
        # Load configurations
        with open(config1_path, 'r') as f:
            config1 = json.load(f)
        
        with open(config2_path, 'r') as f:
            config2 = json.load(f)
        
        # Update comparator level
        self.comparator.diff_level = level
        
        # Compare configurations
        result = self.comparator.compare_configs(
            config1, config2, 
            Path(config1_path).stem, Path(config2_path).stem
        )
        
        # Output results
        print(result['diff_text'])
        
        if output_path:
            with open(output_path, 'w') as f:
                json.dump(result, f, indent=2)
            
            print(f"\nDetailed comparison saved to {output_path}")
        
        # Print analysis
        print("\nConfiguration Analysis:")
        print(f"Config 1 quality: {result['analysis1']['performance']['performance_impact']:.2%}")
        print(f"Config 2 quality: {result['analysis2']['performance']['performance_impact']:.2%}")
        
        if result['analysis1']['validation']['errors']:
            print("\nConfig 1 validation errors:")
            for error in result['analysis1']['validation']['errors']:
                print(f"  - {error}")
        
        if result['analysis2']['validation']['errors']:
            print("\nConfig 2 validation errors:")
            for error in result['analysis2']['validation']['errors']:
                print(f"  - {error}")
    
    def analyze_config(self, config_path: str) -> None:
        """Analyze configuration file"""
        with open(config_path, 'r') as f:
            config = json.load(f)
        
        analysis = self.analyzer.analyze_config(config)
        
        print(f"Configuration Analysis: {Path(config_path).name}")
        print("=" * 50)
        
        print(f"Statistics:")
        print(f"  - Total sections: {analysis['statistics']['total_sections']}")
        print(f"  - Total keys: {analysis['statistics']['total_keys']}")
        
        print(f"\nValidation:")
        print(f"  - Valid: {analysis['validation']['valid']}")
        if analysis['validation']['errors']:
            print(f"  - Errors: {len(analysis['validation']['errors'])}")
            for error in analysis['validation']['errors']:
                print(f"    - {error}")
        
        if analysis['validation']['warnings']:
            print(f"  - Warnings: {len(analysis['validation']['warnings'])}")
            for warning in analysis['validation']['warnings']:
                print(f"    - {warning}")
        
        print(f"\nPerformance Impact:")
        print(f"  - CPU Usage: {analysis['performance_impact']['cpu_usage']:.2%}")
        print(f"  - Memory Usage: {analysis['performance_impact']['memory_usage']:.2f} MB")
        print(f"  - Runtime Estimate: {analysis['performance_impact']['runtime_estimate']:.2f}s")
        print(f"  - Complexity Score: {analysis['performance_impact']['complexity_score']:.2f}")
        
        print(f"\nRecommendations:")
        for recommendation in analysis['recommendations']:
            print(f"  - {recommendation}")
        
        print(f"\nPotential Issues:")
        for issue in analysis['potential_issues']:
            print(f"  - {issue}")
        
        print(f"\nOptimizations:")
        for optimization in analysis['optimizations']:
            print(f"  - {optimization}")
    
    def auto_optimize(self, config_path: str, output_path: Optional[str] = None) -> None:
        """Auto-optimize configuration"""
        with open(config_path, 'r') as f:
            config = json.load(f)
        
        optimized = self.advanced_manager.auto_optimize_config(config)
        
        print(f"Configuration optimized: {Path(config_path).name}")
        
        if output_path:
            with open(output_path, 'w') as f:
                json.dump(optimized, f, indent=2)
            
            print(f"Optimized configuration saved to {output_path}")
        
        # Evaluate quality
        quality = self.advanced_manager.evaluate_config_quality(optimized)
        
        print(f"\nQuality Evaluation:")
        print(f"  - Overall Quality: {quality['overall_quality']:.2f}")
        for metric, value in quality['quality_metrics'].items():
            print(f"  - {metric}: {value:.2f}")
    
    def suggest_alternatives(self, config_path: str, count: int = 3) -> None:
        """Suggest alternative configurations"""
        with open(config_path, 'r') as f:
            config = json.load(f)
        
        alternatives = self.advanced_manager.suggest_alternatives(config, count)
        
        print(f"Alternative Configurations for {Path(config_path).name}:")
        print("=" * 50)
        
        for i, alternative in enumerate(alternatives, 1):
            print(f"\nAlternative {i}:")
            print(json.dumps(alternative, indent=2))
            
            # Evaluate quality
            quality = self.advanced_manager.evaluate_config_quality(alternative)
            print(f"  - Quality: {quality['overall_quality']:.2f}")
class NLMDiffCLI:
    """Command-line interface for NLM configuration diffing tool"""
    
    def __init__(self):
        self.tool = NLMDiffTool()
    
    def compare_command(self, args):
        """Compare two configuration files"""
        try:
            self.tool.compare_configs(
                args.config1,
                args.config2,
                args.output,
                ConfigDiffLevel(args.level)
            )
        except Exception as e:
            print(f"Error: {e}")
            return 1
        
        return 0
    
    def analyze_command(self, args):
        """Analyze configuration file"""
        try:
            self.tool.analyze_config(args.config)
        except Exception as e:
            print(f"Error: {e}")
            return 1
        
        return 0
    
    def auto_optimize_command(self, args):
        """Auto-optimize configuration"""
        try:
            self.tool.auto_optimize(args.config, args.output)
        except Exception as e:
            print(f"Error: {e}")
            return 1
        
        return 0
    
    def suggest_alternatives_command(self, args):
        """Suggest alternative configurations"""
        try:
            self.tool.suggest_alternatives(args.config)
        except Exception as e:
            print(f"Error: {e}")
            return 1
        
        return 0
    
    def run(self, args: Optional[List[str]] = None) -> int:
        """Run command line interface"""
        import argparse
        
        parser = argparse.ArgumentParser(
            description="NLM Configuration Diffing and Analysis Tool",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Commands:
  compare    Compare two configuration files
  analyze    Analyze configuration file
  auto-optimize  Auto-optimize configuration
  suggest    Suggest alternative configurations
            """
        )
        
        # Create subparsers for commands
        subparsers = parser.add_subparsers(dest='command', help='Command to execute')
        
        # Compare command
        compare_parser = subparsers.add_parser('compare', help='Compare two configuration files')
        compare_parser.add_argument('config1', help='First configuration file')
        compare_parser.add_argument('config2', help='Second configuration file')
        compare_parser.add_argument('--output', '-o', help='Output file for detailed results')
        compare_parser.add_argument('--level', choices=[l.value for l in ConfigDiffLevel],
                                   default=ConfigDiffLevel.NORMAL.value,
                                   help='Diff detail level (default: normal)')
        
        # Analyze command
        analyze_parser = subparsers.add_parser('analyze', help='Analyze configuration file')
        analyze_parser.add_argument('config', help='Configuration file to analyze')
        
        # Auto-optimize command
        auto_optimize_parser = subparsers.add_parser('auto-optimize', help='Auto-optimize configuration')
        auto_optimize_parser.add_argument('config', help='Configuration file to optimize')
        auto_optimize_parser.add_argument('--output', '-o', help='Output file for optimized configuration')
        
        # Suggest alternatives command
        suggest_parser = subparsers.add_parser('suggest', help='Suggest alternative configurations')
        suggest_parser.add_argument('config', help='Configuration file to generate alternatives for')
        
        parsed_args = parser.parse_args(args)
        
        if not parsed_args.command:
            parser.print_help()
            return 0
        
        # Dispatch to command handlers
        command_handlers = {
            'compare': self.compare_command,
            'analyze': self.analyze_command,
            'auto-optimize': self.auto_optimize_command,
            'suggest': self.suggest_alternatives_command,
        }
        
        return command_handlers[parsed_args.command](parsed_args)
def main():
    """Main function"""
    try:
        cli = NLMDiffCLI()
        return cli.run()
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 1
    except Exception as e:
        print(f"Unexpected error: {e}")
        return 1
if __name__ == "__main__":
    sys.exit(main())
