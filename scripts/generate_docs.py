#!/usr/bin/env python3
"""
Documentation Generation Tool for NLM
Generates comprehensive documentation from source code comments and API
"""

import os
import re
import argparse
import json
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any, Optional

class NLMDocumentationGenerator:
    def __init__(self, source_dir: str, output_dir: str):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.doc_templates = {}
        
    def generate_all_documentation(self):
        """Generate all types of documentation"""
        print("Generating NLM Documentation...")
        
        # Create output directory
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Generate API documentation
        self.generate_api_docs()
        
        # Generate architecture documentation
        self.generate_architecture_docs()
        
        # Generate user guide
        self.generate_user_guide()
        
        # Generate installation guide
        self.generate_installation_guide()
        
        # Generate examples
        self.generate_examples()
        
        # Generate changelog
        self.generate_changelog()
        
        print(f"Documentation generated in {self.output_dir}")
        
    def extract_doxygen_comments(self, filepath: Path) -> Dict[str, str]:
        """Extract Doxygen-style comments from a source file"""
        comments = {"brief": "", "detailed": "", "examples": [], "see_also": []}
        
        try:
            content = filepath.read_text(encoding='utf-8')
            
            # Extract brief description
            brief_match = re.search(r'/\*\*\s*\n\s*(.*?)\n\s*\*/\s*\n', content, re.DOTALL)
            if brief_match:
                comments["brief"] = brief_match.group(1).strip()
            
            # Extract detailed description
            detailed_match = re.search(r'/\*\*\s*\n(?:.*?\n)*?\s*\n\s*\n', content, re.DOTALL)
            if detailed_match:
                detailed_text = detailed_match.group(0)
                # Remove brief description to avoid duplication
                if brief_match:
                    detailed_text = detailed_text.replace(brief_match.group(0), "")
                comments["detailed"] = detailed_text.strip()
            
            # Extract examples (C++ code blocks in comments)
            example_matches = re.findall(r'/\*\*\s*\n(?:.*?\n)*?\n\s*\n(.*?)(?=\n\s*\n|\n\s*\*\/)', content, re.DOTALL)
            for example in example_matches:
                if "```cpp" in example or "example" in example.lower():
                    comments["examples"].append(example.strip())
                    
        except Exception as e:
            print(f"Warning: Could not parse {filepath}: {e}")
        
        return comments
        
    def generate_api_docs(self):
        """Generate API documentation for all headers"""
        print("  Generating API documentation...")
        
        api_doc = {
            "title": "NLM API Documentation",
            "version": "0.1.0",
            "generated": datetime.now().isoformat(),
            "modules": {}
        }
        
        # Find all header files
        header_files = list(self.source_dir.rglob("*.hpp"))
        header_files.extend(list(self.source_dir.rglob("*.h")))
        
        for header in header_files:
            if ".git" in str(header) or "build" in str(header):
                continue
                
            rel_path = header.relative_to(self.source_dir)
            module_name = str(rel_path.parent).replace('/', '_')
            
            if module_name == '.':
                module_name = header.stem
            
            comments = self.extract_doxygen_comments(header)
            
            api_doc["modules"][module_name] = {
                "file": str(rel_path),
                "brief": comments["brief"],
                "description": comments["detailed"],
                "examples": comments["examples"],
                "classes": self.extract_class_definitions(header)
            }
        
        # Write API documentation
        with open(self.output_dir / "api_reference.md", "w", encoding='utf-8') as f:
            f.write(self.generate_api_markdown(api_doc))
            
        with open(self.output_dir / "api_reference.json", "w", encoding='utf-8') as f:
            json.dump(api_doc, f, indent=2)
            
    def extract_class_definitions(self, filepath: Path) -> List[Dict[str, Any]]:
        """Extract class definitions from header file"""
        classes = []
        
        try:
            content = filepath.read_text(encoding='utf-8')
            
            # Find class definitions
            class_pattern = r'class\s+(\w+)\s*:\s*public\s+(.*?)(?=\n\s*class|\n\s*$|\n\s*\})'
            matches = re.finditer(class_pattern, content, re.DOTALL)
            
            for match in matches:
                class_name = match.group(1)
                class_body = match.group(2)
                
                # Extract methods (simplified)
                methods = re.findall(r'\s*(\w+)\s*\(.*?\)\s*;', class_body)
                
                classes.append({
                    "name": class_name,
                    "methods": methods,
                    "file": str(filepath)
                })
                
        except Exception as e:
            print(f"Warning: Could not extract class definitions from {filepath}: {e}")
        
        return classes
        
    def generate_architecture_docs(self):
        """Generate architecture documentation"""
        print("  Generating architecture documentation...")
        
        # Read existing architecture documentation
        arch_docs = []
        for doc_file in self.source_dir.parent.glob("docs/*.md"):
            if "architecture" in doc_file.name.lower():
                arch_docs.append(doc_file)
        
        # Generate enhanced architecture documentation
        arch_md = "# Enhanced NLM Architecture Documentation\n\n"
        
        arch_md += "## Automated Architecture Analysis\n\n"
        arch_md += f"*Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*\n\n"
        
        # Extract system components from source
        components = self.extract_system_components()
        
        arch_md += "## System Components\n\n"
        for component in components:
            arch_md += f"### {component['name']}\n\n"
            arch_md += f"{component['description']}\n\n"
            arch_md += f"**Location:** {component['location']}\n\n"
        
        with open(self.output_dir / "architecture.md", "w", encoding='utf-8') as f:
            f.write(arch_md)
            
    def extract_system_components(self) -> List[Dict[str, str]]:
        """Extract system components from source code"""
        components = []
        
        # Define component patterns
        component_patterns = [
            ("Core Layer", "core/", ["Types", "Config", "Random", "Logger", "SimulationClock"]),
            ("Brain Layer", "brain/", ["Neuron", "Synapse", "Brain", "NeuralPopulation", "NeuralRegion"]),
            ("Memory Systems", "memory/", ["WorkingMemory", "EpisodicMemory", "SemanticMemory", "ProceduralMemory", "AssociativeMemory"]),
            ("Prediction System", "prediction/", ["PredictionSystem", "NeuralPrediction"]),
            ("Cognition Systems", "cognition/", ["NeuralPlanner", "ConceptFormation", "AttentionalSelection"]),
            ("Neuromodulation", "neuromodulation/", ["Dopamine", "Curiosity", "Novelty", "PredictionError"]),
            ("Development", "development/", ["DevelopmentSystem", "Synaptogenesis", "Pruning", "Maturation"]),
            ("Sensory", "sensory/", ["SensoryInput", "Vision", "Audio", "InternalSignals"]),
            ("Motor", "motor/", ["MotorSystem", "Action"]),
            ("Environment", "environment/", ["Environment", "Observation"]),
        ]
        
        for component_name, directory, subcomponents in component_patterns:
            component_path = self.source_dir / directory
            if component_path.exists():
                for subcomponent in subcomponents:
                    header_file = component_path / f"{subcomponent}.hpp"
                    if header_file.exists():
                        comments = self.extract_doxygen_comments(header_file)
                        components.append({
                            "name": f"{component_name} - {subcomponent}",
                            "description": comments["brief"] or f"{subcomponent} component in {component_name}",
                            "location": f"src/{directory}/{subcomponent}.hpp"
                        })
        
        return components
        
    def generate_user_guide(self):
        """Generate comprehensive user guide"""
        print("  Generating user guide...")
        
        user_guide = "# NLM User Guide\n\n"
        
        user_guide += "## Quick Start\n\n"
        user_guide += "This guide provides comprehensive instructions for using the NLM (Neural Learning Machine) framework.\n\n"
        
        user_guide += "## Installation\n\n"
        user_guide += "```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```\n\n"
        
        user_guide += "## Basic Usage\n\n"
        user_guide += "### Command Line Interface\n\n"
        user_guide += "The NLM provides several executables:\n\n"
        user_guide += "1. **nlm** - Basic neural computation demo\n"
        user_guide += "2. **nlm_phase3_demo** - Phase 3 world interaction demo\n"
        user_guide += "3. **nlm_phase4_demo** - Phase 4 cognitive experiments\n\n"
        
        user_guide += "### Configuration Files\n\n"
        user_guide += "Configuration files are stored in the `configs/` directory.\n\n"
        
        user_guide += "### Python API (pynlm)\n\n"
        user_guide += "NLM provides comprehensive Python bindings via `pynlm`.\n\n"
        
        with open(self.output_dir / "user_guide.md", "w", encoding='utf-8') as f:
            f.write(user_guide)
            
    def generate_examples(self):
        """Generate code examples"""
        print("  Generating examples...")
        
        examples_dir = self.output_dir / "examples"
        examples_dir.mkdir(exist_ok=True)
        
        # Example 1: Basic brain simulation
        example1 = """#!/usr/bin/env python3
""""""# Basic Brain Simulation Example

This example demonstrates creating and running a simple neural brain simulation.

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Key Features Demonstrated:
- Brain creation and initialization
- Step-by-step simulation
- Monitoring brain activity
- Basic configuration usage
""""

        with open(examples_dir / "basic_brain_simulation.md", "w", encoding='utf-8') as f:
            f.write(example1)
            
        # Example 2: Advanced agent simulation
        example2 = """# Advanced Agent Simulation Example

This example demonstrates creating a complete agent with brain, world, and sensory processing.

```python
import pynlm

# Setup components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
```

### Key Features Demonstrated:
- Agent-world integration
- Sensory processing pipeline
- Motor command decoding
- Learning feature activation
- Complete simulation loop
"""

        with open(examples_dir / "advanced_agent_simulation.md", "w", encoding='utf-8') as f:
            f.write(example2)
            
    def generate_installation_guide(self):
        """Generate installation guide"""
        print("  Generating installation guide...")
        
        installation = "# NLM Installation Guide\n\n"
        
        installation += "## System Requirements\n\n"
        installation += "- C++ compiler with C++20 support\n"
        installation += "- CMake 3.16 or higher\n"
        installation += "- Python 3.8 or higher (for bindings)\n"
        installation += "- pybind11 2.11 or higher\n\n"
        
        installation += "## Building from Source\n\n"
        installation += "### Linux/macOS\n\n"
        installation += "```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```\n\n"
        
        installation += "### Windows\n\n"
        installation += "```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```\n\n"
        
        installation += "## Installing Python Bindings\n\n"
        installation += "The Python bindings are automatically installed with the build.\n\n"
        installation += "```bash
# After building
pip install pynlm
```\n\n"
        
        installation += "## Testing\n\n"
        installation += "Run the test suite to verify the installation:\n\n"
        installation += "```bash
cd build
ctest --output-on-failure
```\n\n"
        
        with open(self.output_dir / "installation.md", "w", encoding='utf-8') as f:
            f.write(installation)
            
    def generate_changelog(self):
        """Generate changelog"""
        print("  Generating changelog...")
        
        changelog = "# NLM Changelog\n\n"
        
        changelog += "## Recent Changes (2026.10.17)\n\n"
        changelog += "### Code Improvements\n"
        changelog += "- Fixed configuration file duplication (dopamine_baseline)\n"
        changelog += "- Enhanced build system with installation targets\n"
        changelog += "- Added command-line interface for common operations\n"
        changelog += "- Improved configuration validation and schema support\n"
        changelog += "- Enhanced error handling and logging\n"
        changelog += "- Added comprehensive documentation generation\n\n"
        
        changelog += "### Bug Fixes\n"
        changelog += "- Fixed dopamine_baseline configuration duplication\n"
        changelog += "- Improved Config loading error handling\n"
        changelog += "- Enhanced command-line argument parsing\n\n"
        
        changelog += "### Features Added\n"
        changelog += "- Configuration validation and schema support\n"
        changelog += "- Command-line help system\n"
        changelog += "- Package configuration support\n"
        changelog += "- Documentation generation tools\n"
        
        with open(self.output_dir / "changelog.md", "w", encoding='utf-8') as f:
            f.write(changelog)
            
    def generate_api_markdown(self, api_doc: Dict) -> str:
        """Generate markdown from API documentation"""
        md = "# NLM API Reference\n\n"
        
        for module_name, module_info in api_doc["modules"].items():
            md += f"## {module_name}\n\n"
            md += f"**File:** `{module_info['file']}`\n\n"
            
            if module_info['brief']:
                md += f"**Brief:** {module_info['brief']}\n\n"
            
            if module_info['description']:
                md += f"**Description:**\n{module_info['description']}\n\n"
            
            if module_info['classes']:
                md += "### Classes\n\n"
                for cls in module_info['classes']:
                    md += f"- **{cls['name']}**\n"
                    if cls['methods']:
                        md += f"  - Methods: {', '.join(cls['methods'][:5])}"
                        if len(cls['methods']) > 5:
                            md += f", ... and {len(cls['methods']) - 5} more"
                        md += "\n"
                md += "\n"
            
            if module_info['examples']:
                md += "### Examples\n\n"
                for example in module_info['examples'][:3]:
                    md += f"```cpp\n{example}\n```\n\n"
        
        return md

def main():
    parser = argparse.ArgumentParser(description="Generate NLM Documentation")
    parser.add_argument("--source", "-s", default="/workspace/app", 
                       help="Source directory (default: /workspace/app)")
    parser.add_argument("--output", "-o", default="/workspace/app/generated_docs",
                       help="Output directory for documentation (default: /workspace/app/generated_docs)")
    
    args = parser.parse_args()
    
    generator = NLMDocumentationGenerator(args.source, args.output)
    generator.generate_all_documentation()
    
    print("\nDocumentation generation completed successfully!")
    print(f"Documentation available at: {args.output}")
    print("\nKey files generated:")
    print("  - api_reference.md - Complete API documentation")
    print("  - architecture.md - Enhanced architecture documentation")
    print("  - user_guide.md - Comprehensive user guide")
    print("  - installation.md - Installation instructions")
    print("  - examples/ - Code examples and tutorials")
    print("  - changelog.md - Change history")

if __name__ == "__main__":
    main()
