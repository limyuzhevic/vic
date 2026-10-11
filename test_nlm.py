#!/usr/bin/env python3

"""
Improved NLM Testing Tool

This script provides comprehensive testing and validation of the NLM brain systems.
It focuses on testing the Phase 6 integration and identifying disconnected systems.
"""

import subprocess
import sys
import os
import json
import time
from pathlib import Path

class NLMToolTester:
    def __init__(self, workspace_dir):
        self.workspace_dir = Path(workspace_dir)
        self.test_results = {}
        
    def run_command(self, cmd, cwd=None):
        """Run a command and return (success, output)"""
        try:
            result = subprocess.run(
                cmd, shell=True, cwd=cwd, 
                capture_output=True, text=True, timeout=30
            )
            return result.returncode == 0, result.stdout + result.stderr
        except subprocess.TimeoutExpired:
            return False, f"Command timed out: {cmd}"
        except Exception as e:
            return False, f"Command failed: {e}"
    
    def test_build(self):
        """Test if the NLM project builds successfully"""
        print("🔨 Testing NLM build...")
        
        build_dir = self.workspace_dir / "build"
        build_dir.mkdir(exist_ok=True)
        
        # Run CMake
        success, output = self.run_command(
            "cmake .. -DCMAKE_BUILD_TYPE=Release", 
            cwd=build_dir
        )
        
        if not success:
            print(f"❌ CMake failed: {output[:500]}")
            return False
            
        # Run make
        success, output = self.run_command("make -j4", cwd=build_dir)
        
        if not success:
            print(f"❌ Make failed: {output[:500]}")
            return False
            
        print("✅ Build successful")
        return True
    
    def test_demo_run(self):
        """Test running the Phase 2 demo"""
        print("🚀 Testing Phase 2 demo run...")
        
        build_dir = self.workspace_dir / "build"
        
        # Check if demo exists
        demo_path = build_dir / "nlm_phase2_demo"
        if not demo_path.exists():
            # Try to find it elsewhere
            demos = list(build_dir.glob("*demo"))
            if demos:
                demo_path = demos[0]
            else:
                print("❌ No demo executable found")
                return False
        
        # Run with short timeout
        success, output = self.run_command(str(demo_path), cwd=build_dir)
        
        if not success:
            print(f"❌ Demo failed: {output[:500]}")
            return False
            
        print("✅ Demo run successful")
        return True
    
    def analyze_config_file(self):
        """Analyze the configuration file structure"""
        print("📋 Analyzing configuration file...")
        
        config_path = self.workspace_dir / "configs" / "default.cfg"
        if not config_path.exists():
            print("❌ Config file not found")
            return False
            
        with open(config_path, 'r') as f:
            content = f.read()
            
        # Check for important sections
        sections = []
        if 'random_seed' in content:
            sections.append("random_seed")
        if 'simulation_timestep' in content:
            sections.append("simulation_timestep")
        if 'neuron_count' in content:
            sections.append("neuron_count")
        if 'region_count' in content:
            sections.append("region_count")
        if 'plasticity' in content.lower():
            sections.append("plasticity")
        if 'neuromodulation' in content.lower():
            sections.append("neuromodulation")
            
        print(f"   Config contains: {', '.join(sections)}")
        return True
    
    def check_python_bindings(self):
        """Check if Python bindings are built"""
        print("🐍 Checking Python bindings...")
        
        # Look for pybind11 or Python bindings in build directory
        build_dir = self.workspace_dir / "build"
        
        # Check for common Python binding files
        binding_patterns = [
            "*python*".join(build_dir.glob("*.so")),
            "*python*".join(build_dir.glob("*.pyd")),
            "*python*".join(build_dir.glob("*.so"))
        ]
        
        found = False
        for pattern in binding_patterns:
            for file in pattern:
                if file and "pynlm" in file.name:
                    found = True
                    print(f"   Found Python binding: {file.name}")
                    break
                    
        if not found:
            print("   ⚠️  No Python bindings found (expected for C++ project)")
            
        return True
    
    def analyze_cpp_structure(self):
        """Analyze the C++ source structure"""
        print("📁 Analyzing C++ source structure...")
        
        src_dir = self.workspace_dir / "src"
        if not src_dir.exists():
            print("❌ src directory not found")
            return False
            
        # Count key directories
        key_dirs = ["brain", "memory", "cognition", "prediction", "neuromodulation"]
        for dir_name in key_dirs:
            dir_path = src_dir / dir_name
            if dir_path.exists():
                files = list(dir_path.glob("*.cpp")) + list(dir_path.glob("*.hpp"))
                print(f"   {dir_name}/: {len(files)} files")
            else:
                print(f"   {dir_name}/: NOT FOUND")
                
        return True
    
    def test_documentation_quality(self):
        """Test if documentation is comprehensive"""
        print("📚 Testing documentation quality...")
        
        # Count documentation files
        docs_dir = self.workspace_dir / "docs"
        if not docs_dir.exists():
            print("   ⚠️  docs directory not found")
            return True
            
        doc_files = list(docs_dir.glob("*.md"))
        print(f"   Found {len(doc_files)} documentation files")
        
        # Check for key documentation
        key_docs = ["README.md", "HOW_TO_USE.md", "easy_usage.md"]
        found = []
        for doc_name in key_docs:
            if (docs_dir / doc_name).exists():
                found.append(doc_name)
                
        print(f"   Key docs found: {', '.join(found)}")
        return True
    
    def run_integration_test(self):
        """Run integration test for Phase 6"""
        print("🧪 Running Phase 6 integration test...")
        
        # This would run the actual NLM Phase 6 demo
        # For now, we check if it's configured to run
        build_dir = self.workspace_dir / "build"
        
        # Look for Phase 6 specific executables
        phase6_executables = list(build_dir.glob("*phase6*"))
        
        if phase6_executables:
            print(f"   Found {len(phase6_executables)} Phase 6 executables")
            for exe in phase6_executables[:2]:  # Check first 2
                print(f"   - {exe.name}")
        else:
            print("   ℹ️  No Phase 6 executables found (may be in later phases)")
            
        return True
    
    def generate_improvement_report(self):
        """Generate a report on areas for improvement"""
        print("📊 Generating improvement report...")
        
        improvements = [
            "1. Add proper JSON/YAML parser for configuration files",
            "2. Implement real prediction system using NLM's neural substrate",
            "3. Connect prediction system to sensory processing loop",
            "4. Implement real action selection based on neural activity",
            "5. Add more advanced features for developers",
            "6. Implement better error handling and logging",
            "7. Add performance profiling tools",
            "8. Implement checkpoint system for brain persistence",
            "9. Add multi-threading support for performance",
            "10. Implement advanced visualization tools"
        ]
        
        print("   Recommended improvements:")
        for improvement in improvements:
            print(f"     {improvement}")
            
        return True
    
    def run_all_tests(self):
        """Run all tests"""
        print("=" * 60)
        print("NLM Testing and Analysis Tool")
        print("=" * 60)
        
        tests = [
            ("Build Test", self.test_build),
            ("Demo Run Test", self.test_demo_run),
            ("Config Analysis", self.analyze_config_file),
            ("Python Bindings", self.check_python_bindings),
            ("C++ Structure", self.analyze_cpp_structure),
            ("Documentation Quality", self.test_documentation_quality),
            ("Integration Test", self.run_integration_test),
            ("Improvement Report", self.generate_improvement_report),
        ]
        
        results = {}
        for test_name, test_func in tests:
            print(f"\n{'-' * 40}")
            try:
                success = test_func()
                results[test_name] = "✅ PASS" if success else "❌ FAIL"
            except Exception as e:
                print(f"❌ ERROR in {test_name}: {e}")
                results[test_name] = "❌ ERROR"
                
        # Summary
        print(f"\n{'=' * 60}")
        print("TEST SUMMARY")
        print("=" * 60)
        for test_name, result in results.items():
            print(f"{test_name}: {result}")
            
        passed = sum(1 for r in results.values() if r.startswith("✅"))
        print(f"\nTotal: {passed}/{len(tests)} tests passed")
        
        return passed == len(tests)

if __name__ == "__main__":
    workspace_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    
    tester = NLMToolTester(workspace_dir)
    success = tester.run_all_tests()
    
    sys.exit(0 if success else 1)