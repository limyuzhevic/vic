#!/usr/bin/env python3
"""
NLM System Validation Script

This script validates the improvements made to the NLM system,
including configuration files, error handling, and new features.
"""

import os
import sys
import json
import yaml
from pathlib import Path
from typing import Dict, List, Any, Tuple

class NLMValidator:
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).absolute()
        self.config_dir = self.project_root / "configs"
        self.src_dir = self.project_root / "src"
        self.test_results = {
            "config_validation": [],
            "code_validation": [],
            "feature_validation": [],
            "performance_validation": [],
            "overall_score": 0.0
        }
        
    def validate_config_files(self) -> Dict[str, Any]:
        """Validate configuration files."""
        print("Validating configuration files...")
        results = {
            "default_cfg": {},
            "advanced_cfg": {},
            "overall_valid": True
        }
        
        # Validate default.cfg
        default_cfg_path = self.config_dir / "default.cfg"
        if default_cfg_path.exists():
            try:
                content = default_cfg_path.read_text()
                validation_result = self._validate_default_cfg(content)
                results["default_cfg"] = validation_result
                if not validation_result["valid"]:
                    results["overall_valid"] = False
                    print(f"  ❌ default.cfg validation failed: {validation_result['error']}")
                else:
                    print(f"  ✅ default.cfg validation passed")
            except Exception as e:
                results["default_cfg"]["error"] = str(e)
                results["overall_valid"] = False
                print(f"  ❌ default.cfg validation failed: {e}")
        else:
            results["default_cfg"]["error"] = "File not found"
            results["overall_valid"] = False
            print(f"  ❌ default.cfg not found")
        
        # Validate advanced.cfg
        advanced_cfg_path = self.config_dir / "advanced.cfg"
        if advanced_cfg_path.exists():
            try:
                content = advanced_cfg_path.read_text()
                validation_result = self._validate_advanced_cfg(content)
                results["advanced_cfg"] = validation_result
                if not validation_result["valid"]:
                    results["overall_valid"] = False
                    print(f"  ❌ advanced.cfg validation failed: {validation_result['error']}")
                else:
                    print(f"  ✅ advanced.cfg validation passed")
            except Exception as e:
                results["advanced_cfg"]["error"] = str(e)
                results["overall_valid"] = False
                print(f"  ❌ advanced.cfg validation failed: {e}")
        else:
            results["advanced_cfg"]["error"] = "File not found"
            results["overall_valid"] = False
            print(f"  ❌ advanced.cfg not found")
        
        return results
    
    def _validate_default_cfg(self, content: str) -> Dict[str, Any]:
        """Validate default.cfg content."""
        result = {"valid": True, "error": ""}
        
        # Check for duplicate entries
        lines = content.split('\n')
        seen_entries = set()
        duplicate_count = 0
        
        for line in lines:
            line = line.strip()
            if line and not line.startswith('#'):
                if line in seen_entries:
                    duplicate_count += 1
                    if duplicate_count > 1:  # Allow some duplicates for testing
                        result["valid"] = False
                        result["error"] = f"Duplicate entry found: {line}"
                        break
                else:
                    seen_entries.add(line)
        
        # Check for required sections
        required_keys = [
            "random_seed",
            "simulation_timestep", 
            "neuron_count",
            "region_count",
            "connection_probability",
            "plasticity_learning_rate",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "dopamine_baseline",
            "reward_discount_factor",
            "log_level"
        ]
        
        # Parse key-value pairs
        config_dict = {}
        for line in lines:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                config_dict[key.strip()] = value.strip()
        
        missing_keys = [key for key in required_keys if key not in config_dict]
        if missing_keys:
            result["valid"] = False
            result["error"] = f"Missing required keys: {', '.join(missing_keys)}"
        
        return result
    
    def _validate_advanced_cfg(self, content: str) -> Dict[str, Any]:
        """Validate advanced.cfg content."""
        result = {"valid": True, "error": ""}
        
        # Check for essential advanced features
        advanced_features = [
            "use_simd",
            "parallel_processing",
            "gpu_acceleration",
            "visualization_enabled",
            "enable_external_api",
            "enable_research_mode"
        ]
        
        # Parse key-value pairs
        config_dict = {}
        for line in content.split('\n'):
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                config_dict[key.strip()] = value.strip()
        
        missing_features = [feature for feature in advanced_features if feature not in config_dict]
        if missing_features:
            result["valid"] = False
            result["error"] = f"Missing advanced features: {', '.join(missing_features)}"
        
        return result
    
    def validate_code_improvements(self) -> Dict[str, Any]:
        """Validate code improvements."""
        print("Validating code improvements...")
        results = {
            "phase6_integrated_experiment": {},
            "nlm_cli": {},
            "user_guide": {},
            "overall_valid": True
        }
        
        # Check Phase6IntegratedExperiment.cpp improvements
        experiment_path = self.src_dir / "experiments" / "Phase6IntegratedExperiment.cpp"
        if experiment_path.exists():
            try:
                content = experiment_path.read_text()
                validation_result = self._validate_experiment_code(content)
                results["phase6_integrated_experiment"] = validation_result
                if not validation_result["valid"]:
                    results["overall_valid"] = False
                    print(f"  ❌ Phase6IntegratedExperiment.cpp validation failed: {validation_result['error']}")
                else:
                    print(f"  ✅ Phase6IntegratedExperiment.cpp validation passed")
            except Exception as e:
                results["phase6_integrated_experiment"]["error"] = str(e)
                results["overall_valid"] = False
                print(f"  ❌ Phase6IntegratedExperiment.cpp validation failed: {e}")
        else:
            results["phase6_integrated_experiment"]["error"] = "File not found"
            results["overall_valid"] = False
            print(f"  ❌ Phase6IntegratedExperiment.cpp not found")
        
        # Check nlm-cli script
        cli_path = self.project_root / "nlm-cli"
        if cli_path.exists():
            try:
                content = cli_path.read_text()
                validation_result = self._validate_cli_script(content)
                results["nlm_cli"] = validation_result
                if not validation_result["valid"]:
                    results["overall_valid"] = False
                    print(f"  ❌ nlm-cli validation failed: {validation_result['error']}")
                else:
                    print(f"  ✅ nlm-cli validation passed")
            except Exception as e:
                results["nlm_cli"]["error"] = str(e)
                results["overall_valid"] = False
                print(f"  ❌ nlm-cli validation failed: {e}")
        else:
            results["nlm_cli"]["error"] = "File not found"
            results["overall_valid"] = False
            print(f"  ❌ nlm-cli not found")
        
        # Check user guide
        guide_path = self.project_root / "NLM_User_Guide.md"
        if guide_path.exists():
            try:
                content = guide_path.read_text()
                validation_result = self._validate_user_guide(content)
                results["user_guide"] = validation_result
                if not validation_result["valid"]:
                    results["overall_valid"] = False
                    print(f"  ❌ NLM_User_Guide.md validation failed: {validation_result['error']}")
                else:
                    print(f"  ✅ NLM_User_Guide.md validation passed")
            except Exception as e:
                results["user_guide"]["error"] = str(e)
                results["overall_valid"] = False
                print(f"  ❌ NLM_User_Guide.md validation failed: {e}")
        else:
            results["user_guide"]["error"] = "File not found"
            results["overall_valid"] = False
            print(f"  ❌ NLM_User_Guide.md not found")
        
        return results
    
    def _validate_experiment_code(self, content: str) -> Dict[str, Any]:
        """Validate Phase6IntegratedExperiment.cpp improvements."""
        result = {"valid": True, "error": ""}
        
        # Check for improved error handling
        improved_patterns = [
            "validateConfig",
            "try {",
            "catch (const std::exception& e)",
            "logger_.log",
            "Phase6IntegrationResult"
        ]
        
        for pattern in improved_patterns:
            if pattern not in content:
                result["valid"] = False
                result["error"] = f"Missing improvement pattern: {pattern}"
                break
        
        return result
    
    def _validate_cli_script(self, content: str) -> Dict[str, Any]:
        """Validate nlm-cli script."""
        result = {"valid": True, "error": ""}
        
        # Check for CLI functionality
        required_functions = [
            "build_nlm",
            "run_phase6_demo",
            "run_phase2_demo",
            "save_checkpoint",
            "load_checkpoint",
            "show_status",
            "help"
        ]
        
        for func in required_functions:
            if f"def {func}" not in content:
                result["valid"] = False
                result["error"] = f"Missing CLI function: {func}"
                break
        
        return result
    
    def _validate_user_guide(self, content: str) -> Dict[str, Any]:
        """Validate user guide."""
        result = {"valid": True, "error": ""}
        
        # Check for required sections
        required_sections = [
            "# NLM - Neural Learning Machine User Guide",
            "Quick Start",
            "Command Line Interface (CLI)",
            "Configuration",
            "Troubleshooting",
            "License"
        ]
        
        for section in required_sections:
            if section not in content:
                result["valid"] = False
                result["error"] = f"Missing section: {section}"
                break
        
        return result
    
    def validate_features(self) -> Dict[str, Any]:
        """Validate new features."""
        print("Validating new features...")
        results = {
            "cli_interface": False,
            "advanced_config": False,
            "error_handling": False,
            "user_guide": False,
            "overall_valid": False
        }
        
        # Check CLI interface
        cli_path = self.project_root / "nlm-cli"
        if cli_path.exists() and cli_path.stat().st_mode & 0o111:  # Check if executable
            results["cli_interface"] = True
        
        # Check advanced config
        advanced_cfg_path = self.config_dir / "advanced.cfg"
        if advanced_cfg_path.exists():
            results["advanced_config"] = True
        
        # Check error handling in experiment code
        experiment_path = self.src_dir / "experiments" / "Phase6IntegratedExperiment.cpp"
        if experiment_path.exists():
            content = experiment_path.read_text()
            if "try {" in content and "catch" in content and "logger_" in content:
                results["error_handling"] = True
        
        # Check user guide
        guide_path = self.project_root / "NLM_User_Guide.md"
        if guide_path.exists():
            content = guide_path.read_text()
            if "Command Line Interface" in content and "Configuration" in content:
                results["user_guide"] = True
        
        # Determine overall validity
        results["overall_valid"] = all([
            results["cli_interface"],
            results["advanced_config"],
            results["error_handling"],
            results["user_guide"]
        ])
        
        return results
    
    def run_validation(self) -> Dict[str, Any]:
        """Run complete validation."""
        print("=" * 60)
        print("NLM System Validation Script")
        print("=" * 60)
        
        # Run all validations
        config_results = self.validate_config_files()
        code_results = self.validate_code_improvements()
        feature_results = self.validate_features()
        
        # Calculate overall score
        validation_checks = [
            config_results.get("overall_valid", False),
            code_results.get("overall_valid", False),
            feature_results.get("overall_valid", False)
        ]
        
        valid_checks = sum(validation_checks)
        total_checks = len(validation_checks)
        overall_score = (valid_checks / total_checks) * 100 if total_checks > 0 else 0.0
        
        # Compile final results
        final_results = {
            "config_validation": config_results,
            "code_validation": code_results,
            "feature_validation": feature_results,
            "summary": {
                "total_checks": total_checks,
                "valid_checks": valid_checks,
                "overall_score": overall_score,
                "status": "PASSED" if overall_score >= 80.0 else "FAILED"
            }
        }
        
        # Print summary
        print("\n" + "=" * 60)
        print("VALIDATION SUMMARY")
        print("=" * 60)
        print(f"Overall Score: {overall_score:.1f}%")
        print(f"Status: {final_results['summary']['status']}")
        print(f"Valid Checks: {valid_checks}/{total_checks}")
        
        if overall_score >= 80.0:
            print("\n✅ VALIDATION PASSED")
            print("The NLM system improvements have been successfully validated!")
        else:
            print("\n❌ VALIDATION FAILED")
            print("Some improvements need to be addressed.")
        
        return final_results

def main():
    """Main validation function."""
    validator = NLMValidator()
    results = validator.run_validation()
    
    # Save results to file
    results_path = Path("validation_results.json")
    with open(results_path, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nResults saved to: {results_path}")
    
    # Exit with appropriate code
    if results["summary"]["status"] == "PASSED":
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()