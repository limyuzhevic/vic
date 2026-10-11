#!/usr/bin/env python3
"""
Code Analysis Tool for NLM Project
Analyzes codebase for improvements, bugs, and enhancement opportunities.
"""

import os
import re
import json
from pathlib import Path
from typing import Dict, List, Tuple, Set, Optional

class NLMAnalyzer:
    def __init__(self, workspace_path: str):
        self.workspace_path = Path(workspace_path)
        self.python_bindings_file = self.workspace_path / "python" / "bindings.cpp"
        self.config_file = self.workspace_path / "src" / "core" / "Config" / "Config.cpp"
        self.brain_cpp = self.workspace_path / "src" / "brain" / "Brain.cpp"
        self.neuron_cpp = self.workspace_path / "src" / "brain" / "Neuron.cpp"
        
        # Analysis results
        self.issues = []
        self.improvements = []
        self.stats = {}
        
    def analyze_python_bindings(self) -> Dict:
        """Analyze Python bindings for issues and improvements."""
        print("Analyzing Python bindings...")
        issues = []
        improvements = []
        
        if not self.python_bindings_file.exists():
            return {"issues": issues, "improvements": improvements}
            
        content = self.python_bindings_file.read_text()
        
        # Check for missing docstrings
        py_class_pattern = r'py::class_<([^>]+)>'
        for match in re.finditer(py_class_pattern, content):
            class_name = match.group(1).split(' ')[0]
            class_content = self._extract_class_content(content, match.start())
            
            # Find methods without docstrings
            method_pattern = r'\.def\([^)]+\)'
            for method_match in re.finditer(method_pattern, class_content):
                method_text = method_match.group(0)
                if '"""' not in method_text and '.def("Get' not in method_text:
                    # Extract method name
                    def_match = re.search(r'def\("([^"]+)"', method_text)
                    if def_match:
                        method_name = def_match.group(1)
                        if 'get' not in method_text.lower() and 'repr' not in method_text.lower():
                            issues.append(f"Class {class_name}: Method {method_name} missing docstring")
        
        # Check for consistent argument naming
        py_arg_pattern = r'py::arg\("[^"]+"\)'
        all_args = re.findall(py_arg_pattern, content)
        for arg in all_args:
            if '.def("Get' not in content[content.find(arg)-100:content.find(arg)]:
                # Check if py::arg is used consistently with def
                pass
        
        improvements.append("Add missing docstrings to Python methods for better API documentation")
        improvements.append("Improve error handling in Python bindings with proper exception conversion")
        improvements.append("Add NumPy array support for faster numerical operations")
        
        return {"issues": issues, "improvements": improvements}
    
    def analyze_config_system(self) -> Dict:
        """Analyze config system for bugs and improvements."""
        print("Analyzing config system...")
        issues = []
        improvements = []
        
        if not self.config_file.exists():
            return {"issues": issues, "improvements": improvements}
            
        content = self.config_file.read_text()
        
        # Check for placeholder values
        if '"PLACEHOLDER_VALUE"' in content:
            issues.append("Config.cpp: Line contains placeholder value instead of actual value in saveToFile")
        
        # Check missing newlines in save file
        if 'file << " = PLACEHOLDER_VALUE\\n";' in content:
            issues.append("Config.cpp: Missing newline after each entry in saveToFile")
        
        # Check for TODO comments
        if 'TODO PHASE 2: Implement proper JSON/YAML parser' in content:
            improvements.append("Implement proper JSON/YAML config file parser for Phase 2")
            improvements.append("Add advanced config features like inheritance and validation")
        
        return {"issues": issues, "improvements": improvements}
    
    def analyze_integration_points(self) -> Dict:
        """Analyze integration points for issues."""
        print("Analyzing integration points...")
        issues = []
        improvements = []
        
        # Check for hardcoded indices in experiments
        experiment_files = list(self.workspace_path.glob("src/experiments/*.cpp"))
        for exp_file in experiment_files:
            content = exp_file.read_text()
            
            # Check for hardcoded region access
            if 'getRegions()[0]' in content:
                issues.append(f"{exp_file.name}: Hardcoded access to regions()[0] without bounds checking")
                improvements.append(f"Add bounds checking for region access in {exp_file.name}")
            
            # Check for missing error handling
            if 'brain->initialize()' in content and 'if (!brain->initialize())' not in content:
                issues.append(f"{exp_file.name}: Missing error handling for brain initialization")
                improvements.append(f"Add proper error handling in {exp_file.name}")
        
        return {"issues": issues, "improvements": improvements}
    
    def generate_improvement_plan(self) -> Dict:
        """Generate comprehensive improvement plan."""
        print("Generating improvement plan...")
        
        # Run all analyses
        bindings_analysis = self.analyze_python_bindings()
        config_analysis = self.analyze_config_system()
        integration_analysis = self.analyze_integration_points()
        
        # Combine all results
        all_issues = (
            bindings_analysis["issues"] +
            config_analysis["issues"] +
            integration_analysis["issues"]
        )
        
        all_improvements = (
            bindings_analysis["improvements"] +
            config_analysis["improvements"] +
            integration_analysis["improvements"]
        )
        
        # Priority-based improvements
        high_priority = []
        medium_priority = []
        low_priority = []
        
        for improvement in all_improvements:
            if "missing docstring" in improvement.lower():
                high_priority.append(improvement)
            elif "JSON/YAML parser" in improvement:
                high_priority.append(improvement)
            elif "bounds checking" in improvement.lower():
                high_priority.append(improvement)
            elif "NumPy array support" in improvement:
                medium_priority.append(improvement)
            elif "error handling" in improvement.lower():
                medium_priority.append(improvement)
            else:
                low_priority.append(improvement)
        
        return {
            "critical_issues": all_issues,
            "high_priority_improvements": high_priority,
            "medium_priority_improvements": medium_priority,
            "low_priority_improvements": low_priority,
            "total_files_analyzed": 3,
            "total_issues_found": len(all_issues),
            "total_improvements_suggested": len(all_improvements)
        }
    
    def _extract_class_content(self, full_content: str, start_pos: int, max_length: int = 2000) -> str:
        """Extract class content for analysis."""
        end_pos = full_content.find(');', start_pos)
        if end_pos == -1:
            return ""
        end_pos += 2
        return full_content[start_pos:end_pos]
    
    def print_report(self, report: Dict):
        """Print analysis report."""
        print("\n" + "="*80)
        print("NLM PROJECT CODE ANALYSIS REPORT")
        print("="*80)
        
        print(f"\n📊 STATISTICS:")
        print(f"   Total files analyzed: {report['total_files_analyzed']}")
        print(f"   Total issues found: {report['total_issues_found']}")
        print(f"   Total improvements suggested: {report['total_improvements_suggested']}")
        
        print(f"\n⚠️  CRITICAL ISSUES FOUND:")
        if report['critical_issues']:
            for issue in report['critical_issues']:
                print(f"   • {issue}")
        else:
            print("   ✓ No critical issues found")
        
        print(f"\n🔧 HIGH PRIORITY IMPROVEMENTS:")
        if report['high_priority_improvements']:
            for improvement in report['high_priority_improvements']:
                print(f"   • {improvement}")
        else:
            print("   ✓ No high priority improvements needed")
        
        print(f"\n📈 MEDIUM PRIORITY IMPROVEMENTS:")
        if report['medium_priority_improvements']:
            for improvement in report['medium_priority_improvements']:
                print(f"   • {improvement}")
        else:
            print("   ✓ No medium priority improvements needed")
        
        print(f"\n📝 LOW PRIORITY IMPROVEMENTS:")
        if report['low_priority_improvements']:
            for improvement in report['low_priority_improvements']:
                print(f"   • {improvement}")
        else:
            print("   ✓ No low priority improvements needed")
        
        print(f"\n" + "="*80)
        print("ANALYSIS COMPLETE")
        print("="*80)

def main():
    """Main analysis function."""
    workspace = os.environ.get('WORKSPACE_PATH', '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_e1f2c671-f6a3-4bb9-9f5e-e58d235d68bf')
    
    analyzer = NLMAnalyzer(workspace)
    report = analyzer.generate_improvement_plan()
    analyzer.print_report(report)
    
    # Save report to file
    with open('analysis_report.json', 'w') as f:
        json.dump(report, f, indent=2)
    
    print(f"\n📄 Detailed report saved to 'analysis_report.json'")

if __name__ == "__main__":
    main()
