#!/usr/bin/env python3
"""
Simple Code Quality Checker for NLM Project
Checks for common code quality issues.
"""

import os
import re
import json
from pathlib import Path
from typing import Dict, List, Tuple

class CodeQualityChecker:
    def __init__(self, workspace_path: str):
        self.workspace_path = Path(workspace_path)
        self.results = {
            "config_issues": [],
            "brain_issues": [],
            "neuron_issues": [],
            "experiment_issues": [],
            "total_issues": 0
        }
    
    def check_config_cpp(self) -> List[str]:
        """Check Config.cpp for issues."""
        issues = []
        config_file = self.workspace_path / "src" / "core" / "Config" / "Config.cpp"
        
        if not config_file.exists():
            return issues
            
        content = config_file.read_text()
        
        # Check for TODO comments that need fixing
        if "TODO PHASE 2: Implement proper JSON/YAML parser" in content:
            issues.append("Config.cpp: Missing JSON/YAML parser implementation")
        
        # Check for incomplete error handling
        if "if (file.is_open()" in content and "else" not in content[content.find("if (file.is_open()"):content.find("if (file.is_open()") + 200]:
            issues.append("Config.cpp: Missing else clause for file open error handling")
        
        return issues
    
    def check_brain_cpp(self) -> List[str]:
        """Check Brain.cpp for issues."""
        issues = []
        brain_file = self.workspace_path / "src" / "brain" / "Brain.cpp"
        
        if not brain_file.exists():
            return issues
            
        content = brain_file.read_text()
        
        # Check for uninitialized variables in error paths
        if "result.startTime = time(nullptr);" in content and "result.endTime = time(nullptr);" in content:
            issues.append("Brain.cpp: Missing error path cleanup in Phase6IntegratedExperiment")
        
        # Check for hardcoded magic numbers
        if "return " + str(1000) in content and "step % 1000" in content:
            issues.append("Brain.cpp: Hardcoded magic number 1000 for status logging interval")
        
        return issues
    
    def check_neuron_cpp(self) -> List[str]:
        """Check Neuron.cpp for issues."""
        issues = []
        neuron_file = self.workspace_path / "src" / "brain" / "Neuron.cpp"
        
        if not neuron_file.exists():
            return issues
            
        content = neuron_file.read_text()
        
        # Check for uninitialized variables
        if "MembranePotential synapticInput;" in content and "synapticInput" not in content[content.find("private:"):content.find("public:")]:
            issues.append("Neuron.cpp: Member variable synapticInput not initialized in constructor")
        
        # Check for incomplete reset
        if "void Neuron::reset()" in content and "reset(" not in content[content.find("void Neuron::reset()"):content.find("void Neuron::reset()") + 100]:
            issues.append("Neuron.cpp: Reset method may not fully reset all state")
        
        return issues
    
    def check_experiments(self) -> List[str]:
        """Check experiment files for issues."""
        issues = []
        experiment_dir = self.workspace_path / "src" / "experiments"
        
        if not experiment_dir.exists():
            return issues
        
        for exp_file in experiment_dir.glob("*.cpp"):
            content = exp_file.read_text()
            
            # Check for missing error handling for initialization
            if "->initialize()" in content and "if (!" not in content[content.find("->initialize()")-50:content.find("->initialize()")+50]:
                issues.append(f"{exp_file.name}: Missing error handling for initialize() call")
            
            # Check for uninitialized variables in loops
            if "for (int i = 0; i < 100; ++i)" in content:
                issues.append(f"{exp_file.name}: Using uninitialized loop variable i in multiple contexts")
        
        return issues
    
    def run_checks(self) -> Dict:
        """Run all code quality checks."""
        print("Running code quality checks...")
        
        self.results["config_issues"] = self.check_config_cpp()
        self.results["brain_issues"] = self.check_brain_cpp()
        self.results["neuron_issues"] = self.check_neuron_cpp()
        self.results["experiment_issues"] = self.check_experiments()
        
        self.results["total_issues"] = (
            len(self.results["config_issues"]) +
            len(self.results["brain_issues"]) +
            len(self.results["neuron_issues"]) +
            len(self.results["experiment_issues"])
        )
        
        return self.results
    
    def print_report(self):
        """Print code quality report."""
        print("\n" + "="*80)
        print("NLM PROJECT CODE QUALITY REPORT")
        print("="*80)
        
        print(f"\n📊 QUALITY METRICS:")
        print(f"   Config system issues: {len(self.results['config_issues'])}")
        print(f"   Brain system issues: {len(self.results['brain_issues'])}")
        print(f"   Neuron system issues: {len(self.results['neuron_issues'])}")
        print(f"   Experiment issues: {len(self.results['experiment_issues'])}")
        print(f"   Total issues: {self.results['total_issues']}")
        
        if self.results["config_issues"]:
            print(f"\n⚙️  CONFIG SYSTEM ISSUES:")
            for issue in self.results["config_issues"]:
                print(f"   • {issue}")
        
        if self.results["brain_issues"]:
            print(f"\n🧠 BRAIN SYSTEM ISSUES:")
            for issue in self.results["brain_issues"]:
                print(f"   • {issue}")
        
        if self.results["neuron_issues"]:
            print(f"\n⚡ NEURON SYSTEM ISSUES:")
            for issue in self.results["neuron_issues"]:
                print(f"   • {issue}")
        
        if self.results["experiment_issues"]:
            print(f"\n🧪 EXPERIMENT ISSUES:")
            for issue in self.results["experiment_issues"]:
                print(f"   • {issue}")
        
        print(f"\n" + "="*80)
        print("QUALITY CHECK COMPLETE")
        print("="*80)

def main():
    """Main function."""
    workspace = os.environ.get('WORKSPACE_PATH', '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_e1f2c671-f6a3-4bb9-9f5e-e58d235d68bf')
    
    checker = CodeQualityChecker(workspace)
    checker.run_checks()
    checker.print_report()
    
    # Save report
    with open('quality_report.json', 'w') as f:
        json.dump(checker.results, f, indent=2)
    
    print(f"\n📄 Detailed report saved to 'quality_report.json'")

if __name__ == "__main__":
    main()
