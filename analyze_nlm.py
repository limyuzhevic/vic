#!/usr/bin/env python3

"""
NLM Codebase Integration Analyzer

This script analyzes the NLM codebase and provides specific recommendations
for improving Phase 6 integration based on the Phase 6 Final Audit findings.
"""

import re
import os
from pathlib import Path
from typing import Dict, List, Tuple, Set

class NLMCodebaseAnalyzer:
    def __init__(self, workspace_dir: str):
        self.workspace_dir = Path(workspace_dir)
        self.analysis_results = {}
        
    def analyze_cpp_files(self) -> Dict:
        """Analyze C++ source files for integration issues"""
        print("🔍 Analyzing C++ source files...")
        
        cpp_files = list(self.workspace_dir.rglob("*.cpp"))
        hpp_files = list(self.workspace_dir.rglob("*.hpp"))
        
        analysis = {
            'total_cpp_files': len(cpp_files),
            'total_hpp_files': len(hpp_files),
            'disconnected_systems': {},
            'todo_phase_2_count': 0,
            'integrated_systems': {}
        }
        
        # Check for "TODO PHASE 2" comments
        todo_patterns = [
            r'TODO PHASE 2',
            r'PLACEHOLDER',
            r'TODO',
        ]
        
        for file_path in cpp_files:
            try:
                with open(file_path, 'r') as f:
                    content = f.read()
                    
                # Count TODOs
                for pattern in todo_patterns:
                    matches = re.findall(pattern, content, re.IGNORECASE)
                    analysis['todo_phase_2_count'] += len(matches)
                    
                # Check for system integration status
                if 'Brain.cpp' in str(file_path):
                    self._analyze_brain_cpp(file_path, analysis)
                    
            except Exception as e:
                print(f"   Warning: Could not analyze {file_path}: {e}")
                
        return analysis
    
    def _analyze_brain_cpp(self, brain_cpp_path: Path, analysis: Dict):
        """Analyze Brain.cpp for system integration"""
        try:
            with open(brain_cpp_path, 'r') as f:
                content = f.read()
                
            # Check what systems are actually used in the brain loop
            integration_points = {
                'workingMemory': r'workingMemory.*update',
                'episodicMemory': r'episodicMemory.*storeEpisode',
                'predictionSystem': r'predictionSystem',
                'attention': r'attention.*update',
                'conceptFormation': r'conceptFormation',
                'planner': r'planner',
                'dopamine': r'dopamine.*update',
                'curiosity': r'curiosity.*update',
                'novelty': r'novelty.*update',
            }
            
            for system, pattern in integration_points.items():
                matches = re.findall(pattern, content, re.IGNORECASE)
                analysis['integrated_systems'][system] = len(matches) > 0
                
        except Exception as e:
            print(f"   Warning: Could not analyze Brain.cpp: {e}")
    
    def analyze_config_system(self) -> Dict:
        """Analyze the configuration system"""
        print("⚙️  Analyzing configuration system...")
        
        config_dir = self.workspace_dir / "configs"
        if not config_dir.exists():
            return {'error': 'configs directory not found'}
            
        config_files = list(config_dir.glob("*.cfg"))
        
        analysis = {
            'config_files': len(config_files),
            'config_entries': {},
            'parser_type': 'none'
        }
        
        for config_file in config_files:
            try:
                with open(config_file, 'r') as f:
                    content = f.read()
                    
                # Count key-value pairs
                kv_pairs = re.findall(r'^\s*(\w+)\s*=\s*[^#\n]*', content, re.MULTILINE)
                analysis['config_entries'][config_file.name] = len(kv_pairs)
                
                # Check parser type
                config_cpp_path = self.workspace_dir / "src" / "core" / "Config" / "Config.cpp"
                if config_cpp_path.exists():
                    with open(config_cpp_path, 'r') as f:
                        cpp_content = f.read()
                        if 'TODO PHASE 2' in cpp_content and 'JSON' in cpp_content:
                            analysis['parser_type'] = 'simple_key_value_with_todos'
                        elif 'TODO PHASE 2' in cpp_content:
                            analysis['parser_type'] = 'simple_key_value'
                        elif 'JSON' in cpp_content or 'YAML' in cpp_content:
                            analysis['parser_type'] = 'json_yaml'
                            
            except Exception as e:
                print(f"   Warning: Could not analyze {config_file}: {e}")
                
        return analysis
    
    def analyze_python_bindings(self) -> Dict:
        """Analyze Python bindings"""
        print("🐍 Analyzing Python bindings...")
        
        analysis = {
            'pyproject_exists': False,
            'python_files': [],
            'binding_quality': 'none'
        }
        
        pyproject_path = self.workspace_dir / "pyproject.toml"
        if pyproject_path.exists():
            analysis['pyproject_exists'] = True
            
            try:
                with open(pyproject_path, 'r') as f:
                    content = f.read()
                    
                # Check for pybind11 and scikit-build
                if 'pybind11' in content:
                    analysis['binding_quality'] = 'pybind11'
                if 'scikit-build' in content:
                    analysis['binding_quality'] += '_scikit'
                    
            except Exception as e:
                print(f"   Warning: Could not analyze pyproject.toml: {e}")
                
        # Look for any Python files
        python_files = list(self.workspace_dir.rglob("*.py"))
        analysis['python_files'] = [str(f) for f in python_files]
        
        return analysis
    
    def generate_integration_recommendations(self, cpp_analysis: Dict) -> List[str]:
        """Generate specific integration recommendations"""
        print("💡 Generating integration recommendations...")
        
        recommendations = []
        
        # Based on Phase 6 Final Audit
        if not cpp_analysis.get('integrated_systems', {}).get('predictionSystem', False):
            recommendations.append(
                "1. IMPLEMENT PREDICTION SYSTEM INTEGRATION: "
                "Connect prediction system to sensory processing loop in Brain::step()"
            )
            
        if not cpp_analysis.get('integrated_systems', {}).get('conceptFormation', False):
            recommendations.append(
                "2. IMPLEMENT CONCEPT FORMATION: "
                "Add concept formation processing in Step 10 of brain loop"
            )
            
        if not cpp_analysis.get('integrated_systems', {}).get('planner', False):
            recommendations.append(
                "3. IMPLEMENT NEURAL PLANNER: "
                "Connect neural planner to action selection for intelligent planning"
            )
            
        if cpp_analysis.get('todo_phase_2_count', 0) > 10:
            recommendations.append(
                "4. ADDRESS TODO PHASE 2 ITEMS: "
                f"There are {cpp_analysis['todo_phase_2_count']} TODOs needing implementation"
            )
            
        # Always add key improvements
        recommendations.extend([
            "5. ADD ADVANCED COMMANDS: Implement command-line tools for:",
            "   - Configuration management",
            "   - Checkpoint save/load",
            "   - Performance profiling",
            "   - Debugging tools",
            "",
            "6. IMPROVE ERROR HANDLING: Add comprehensive error handling",
            "   - Input validation",
            "   - Exception safety",
            "   - Better logging",
            "",
            "7. ADD DEVELOPER FEATURES: Implement:",
            "   - API documentation generation",
            "   - Code examples",
            "   - Testing framework integration",
            "   - Performance monitoring"
        ])
        
        return recommendations
    
    def analyze_documentation(self) -> Dict:
        """Analyze documentation quality"""
        print("📚 Analyzing documentation...")
        
        docs_dir = self.workspace_dir / "docs"
        if not docs_dir.exists():
            return {'error': 'docs directory not found'}
            
        md_files = list(docs_dir.glob("*.md"))
        
        analysis = {
            'total_md_files': len(md_files),
            'key_files_found': {},
            'architecture_coverage': False,
            'api_documentation': False
        }
        
        key_files = ['README.md', 'HOW_TO_USE.md', 'easy_usage.md']
        for key_file in key_files:
            file_path = docs_dir / key_file
            if file_path.exists():
                analysis['key_files_found'][key_file] = True
                
        # Check for architecture documentation
        arch_files = ['ARCHITECTURE.md', 'PHASE6_FINAL_AUDIT.md']
        for arch_file in arch_files:
            if (docs_dir / arch_file).exists():
                analysis['architecture_coverage'] = True
                
        return analysis
    
    def create_integration_plan(self) -> Dict:
        """Create a concrete integration plan"""
        print("📋 Creating integration plan...")
        
        plan = {
            'phase1_fix_existing': [
                "Connect prediction system to sensory input in Brain::step()",
                "Implement concept formation processing loop",
                "Integrate neural planner with action selection",
                "Add memory consolidation mechanisms",
                "Implement complete neuromodulation integration"
            ],
            'phase2_advanced_features': [
                "Add command-line interface for advanced operations",
                "Implement configuration file validation and schema",
                "Add performance monitoring and profiling",
                "Implement checkpoint system with compression",
                "Add multi-threading support for scalability"
            ],
            'phase3_extensibility': [
                "Implement plugin architecture for custom systems",
                "Add Python API with comprehensive documentation",
                "Implement automatic code generation for new systems",
                "Add integration testing framework",
                "Create community contribution guidelines"
            ]
        }
        
        return plan
    
    def run_complete_analysis(self) -> Dict:
        """Run complete analysis and return results"""
        print("=" * 60)
        print("NLM Codebase Integration Analysis")
        print("=" * 60)
        
        # Run all analyses
        cpp_analysis = self.analyze_cpp_files()
        config_analysis = self.analyze_config_system()
        python_analysis = self.analyze_python_bindings()
        doc_analysis = self.analyze_documentation()
        recommendations = self.generate_integration_recommendations(cpp_analysis)
        integration_plan = self.create_integration_plan()
        
        results = {
            'cpp_analysis': cpp_analysis,
            'config_analysis': config_analysis,
            'python_analysis': python_analysis,
            'doc_analysis': doc_analysis,
            'recommendations': recommendations,
            'integration_plan': integration_plan
        }
        
        # Print summary
        self._print_analysis_summary(results)
        
        return results
    
    def _print_analysis_summary(self, results: Dict):
        """Print analysis summary"""
        print("\n" + "=" * 60)
        print("ANALYSIS SUMMARY")
        print("=" * 60)
        
        cpp = results['cpp_analysis']
        print(f"📁 C++ Files: {cpp['total_cpp_files']} .cpp, {cpp['total_hpp_files']} .hpp")
        print(f"⚠️  TODO PHASE 2 items: {cpp['todo_phase_2_count']}")
        
        if 'integrated_systems' in cpp:
            print("\n🔧 System Integration Status:")
            for system, integrated in cpp['integrated_systems'].items():
                status = "✅" if integrated else "❌"
                print(f"   {status} {system}")
                
        config = results['config_analysis']
        if 'error' not in config:
            print(f"\n⚙️  Config: {config['config_files']} files, {config['parser_type']}")
            
        python = results['python_analysis']
        print(f"\n🐍 Python: {'✅' if python['pyproject_exists'] else '❌'} pyproject.toml, {len(python['python_files'])} Python files")
        
        doc = results['doc_analysis']
        if 'error' not in doc:
            print(f"\n📚 Documentation: {doc['total_md_files']} files, {'✅' if doc['architecture_coverage'] else '❌'} architecture coverage")
            
        print(f"\n💡 Key Recommendations ({len(results['recommendations'])}):")
        for rec in results['recommendations'][:3]:  # Show top 3
            print(f"   {rec}")
            
        if len(results['recommendations']) > 3:
            print(f"   ... and {len(results['recommendations']) - 3} more")
            
        print(f"\n📋 Integration Plan: {len(results['integration_plan']['phase1_fix_existing'])} Phase 1 fixes, {len(results['integration_plan']['phase2_advanced_features'])} Phase 2 features")

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Analyze NLM codebase for improvements")
    parser.add_argument("workspace_dir", help="Path to NLM workspace directory")
    parser.add_argument("--output", "-o", help="Output file for analysis results (JSON)")
    
    args = parser.parse_args()
    
    analyzer = NLMCodebaseAnalyzer(args.workspace_dir)
    results = analyzer.run_complete_analysis()
    
    if args.output:
        import json
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"\n💾 Analysis results saved to {args.output}")
        
    print("\n✨ Analysis complete!")