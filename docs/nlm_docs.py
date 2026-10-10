#!/usr/bin/env python3

import os

# Make the script executable
os.chmod('nlm_docs.py', 0o755)
"""
NLM Documentation Generator
A comprehensive tool to generate API documentation from NLM source code.

This script analyzes NLM C++ source code and generates documentation in various formats.
"""

import os
import re
import json
import argparse
import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional
from dataclasses import dataclass
from jinja2 import Template

@dataclass
class MethodInfo:
    name: str
    return_type: str
    parameters: List[Dict[str, str]]
    description: str
    is_public: bool = True

@dataclass
class ClassInfo:
    name: str
    namespace: str
    file_path: str
    methods: List[MethodInfo]
    includes: List[str]
    dependencies: List[str]
    description: str
    is_template: bool = False

@dataclass
class ModuleInfo:
    name: str
    path: str
    classes: List[ClassInfo]
    description: str

def extract_classes_from_cpp(file_path: Path) -> List[ClassInfo]:
    """Extract class information from C++ source file."""
    classes = []
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Pattern to match class definitions
        class_pattern = r'class\s+(\w+)\s*(?::\s*public\s+\w+)?\s*{'
        matches = re.finditer(class_pattern, content)
        
        for match in matches:
            class_name = match.group(1)
            
            # Find the class in the file
            class_pattern_full = rf'class\s+{class_name}\s*(?::\s*public\s+\w+)?\s*{{(.*?)}}'
            class_match = re.search(class_pattern_full, content, re.DOTALL)
            
            if class_match:
                class_content = class_match.group(1)
                
                # Extract methods
                method_pattern = r'(\w+)\s+(\w+)\s*\((.*?)\)\s*;'
                method_matches = re.finditer(method_pattern, class_content)
                
                methods = []
                for method_match in method_matches:
                    return_type = method_match.group(1)
                    method_name = method_match.group(2)
                    params_str = method_match.group(3)
                    
                    # Parse parameters
                    parameters = []
                    if params_str.strip():
                        param_parts = [p.strip() for p in params_str.split(',')]
                        for param in param_parts:
                            if param:
                                # Simple parameter parsing (type name)
                                parts = param.split()
                                if len(parts) >= 2:
                                    param_type = ' '.join(parts[:-1])
                                    param_name = parts[-1]
                                    parameters.append({
                                        'type': param_type,
                                        'name': param_name
                                    })
                    
                    methods.append(MethodInfo(
                        name=method_name,
                        return_type=return_type,
                        parameters=parameters,
                        description='',  # Would need Doxygen comments for full extraction
                        is_public=True  # Simplified - would need more sophisticated parsing
                    ))
                
                classes.append(ClassInfo(
                    name=class_name,
                    namespace='nlm',  # Would need to parse namespace properly
                    file_path=str(file_path),
                    methods=methods,
                    includes=[],  # Would need to parse #include directives
                    dependencies=[],  # Would need to analyze dependencies
                    description='',  # Would need Doxygen comments
                    is_template=False
                ))
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
    
    return classes

def extract_modules_from_source(source_dir: Path) -> Dict[str, ModuleInfo]:
    """Extract module information from source directory."""
    modules = {}
    
    # Define NLM modules and their descriptions
    module_descriptions = {
        'core': 'Core system utilities and base types',
        'brain': 'Main brain simulation components',
        'memory': 'Memory systems (working, episodic, semantic, procedural)',
        'neuromodulation': 'Neuromodulator signals (dopamine, curiosity, novelty)',
        'cognition': 'Cognitive mechanisms (attention, planning, concept formation)',
        'plasticity': 'Plasticity rules (STDP, Hebbian, structural changes)',
        'development': 'Brain development and maturation processes',
        'sensory': 'Sensory processing systems',
        'motor': 'Motor system components',
        'environment': 'Environment and world simulation',
        'agent': 'Agent brain interface and body representation',
        'prediction': 'Prediction and planning systems',
        'visualization': 'Visualization and output systems',
        'experiments': 'Experiment frameworks and testing',
        'dynamics': 'Neural dynamics and simulation'
    }
    
    for module_name, description in module_descriptions.items():
        module_path = source_dir / 'src' / module_name
        if module_path.exists():
            classes = []
            
            # Look for .hpp files in the module
            for cpp_file in module_path.glob('*.hpp'):
                classes.extend(extract_classes_from_cpp(cpp_file))
            
            modules[module_name] = ModuleInfo(
                name=module_name,
                path=str(module_path),
                classes=classes,
                description=description
            )
        elif module_name == 'experiments':
            # Handle experiments module which is under src/experiments
            module_path = source_dir / 'src' / 'experiments'
            if module_path.exists():
                classes = []
                for cpp_file in module_path.glob('*.hpp'):
                    classes.extend(extract_classes_from_cpp(cpp_file))
                modules[module_name] = ModuleInfo(
                    name=module_name,
                    path=str(module_path),
                    classes=classes,
                    description='Experiment frameworks and testing systems'
                )
    
    return modules

def generate_html_documentation(modules: Dict[str, ModuleInfo], output_path: Path):
    """Generate HTML documentation from modules."""
    output_path.mkdir(parents=True, exist_ok=True)
    
    # Create index.html
    index_template = Template("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>NLM API Documentation</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .module { margin: 20px 0; padding: 15px; border: 1px solid #ddd; }
        .class { margin: 10px 0; padding: 10px; background-color: #f9f9f9; }
        .method { margin: 5px 0; padding: 5px; background-color: #e9e9e9; }
    </style>
</head>
<body>
    <h1>NLM API Documentation</h1>
    <p>Generated on {{ date }}</p>
    
    {% for module_name, module in modules.items() %}
        <div class="module">
            <h2>{{ module_name }}</h2>
            <p>{{ module.description }}</p>
            
            {% for class in module.classes %}
                <div class="class">
                    <h3>{{ class.name }}</h3>
                    <p><strong>Namespace:</strong> {{ class.namespace }}</p>
                    <p><strong>File:</strong> {{ class.file_path }}</p>
                    
                    {% if class.methods %}
                        <h4>Methods:</h4>
                        {% for method in class.methods %}
                            <div class="method">
                                <p><strong>{{ method.name }}</strong>({% for param in method.parameters %}{{ param.type }} {{ param.name }}{% if not loop.last %}, {% endif %}{% endfor %}) -> {{ method.return_type }}</p>
                            </div>
                        {% endfor %}
                    {% endif %}
                </div>
            {% endfor %}
        </div>
    {% endfor %}
</body>
</html>
""")
    
    html_content = index_template.render(
        modules=modules,
        date=datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    )
    
    index_file = output_path / 'index.html'
    with open(index_file, 'w', encoding='utf-8') as f:
        f.write(html_content)

def generate_markdown_documentation(modules: Dict[str, ModuleInfo], output_path: Path):
    """Generate Markdown documentation from modules."""
    output_path.mkdir(parents=True, exist_ok=True)
    
    markdown_content = f"""# NLM API Documentation

Generated on {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

## Overview

This documentation provides comprehensive API reference for the NLM (Neural Learning Machine) project.

## Modules

"""
    
    for module_name, module in modules.items():
        markdown_content += f"""
### {module_name}

{module.description}

"""
        
        for class_info in module.classes:
            markdown_content += f"""
#### {class_info.name}

- **Namespace**: `{class_info.namespace}`
- **File**: `{class_info.file_path}`

"""
            
            if class_info.methods:
                markdown_content += """
**Methods:**

"""
                for method in class_info.methods:
                    params = ', '.join([f"{p['type']} {p['name']}" for p in method.parameters])
                    markdown_content += f"- `{method.name}({params}) -> {method.return_type}`\n"
    
    # Write to index.md
    index_file = output_path / 'README.md'
    with open(index_file, 'w', encoding='utf-8') as f:
        f.write(markdown_content)

def generate_json_documentation(modules: Dict[str, ModuleInfo], output_path: Path):
    """Generate JSON documentation from modules."""
    output_path.mkdir(parents=True, exist_ok=True)
    
    # Convert modules to JSON-serializable format
    modules_json = {}
    for module_name, module in modules.items():
        modules_json[module_name] = {
            'name': module.name,
            'path': module.path,
            'description': module.description,
            'classes': []
        }
        
        for class_info in module.classes:
            class_json = {
                'name': class_info.name,
                'namespace': class_info.namespace,
                'file_path': class_info.file_path,
                'description': class_info.description,
                'is_template': class_info.is_template,
                'methods': []
            }
            
            for method in class_info.methods:
                method_json = {
                    'name': method.name,
                    'return_type': method.return_type,
                    'parameters': method.parameters,
                    'description': method.description,
                    'is_public': method.is_public
                }
                class_json['methods'].append(method_json)
            
            modules_json[module_name]['classes'].append(class_json)
    
    # Write to api.json
    json_file = output_path / 'api.json'
    with open(json_file, 'w', encoding='utf-8') as f:
        json.dump(modules_json, f, indent=2)

def main():
    parser = argparse.ArgumentParser(description='Generate NLM API documentation')
    parser.add_argument('--source-dir', default='./src', help='Source directory path')
    parser.add_argument('--output-dir', default='./docs/api', help='Output directory path')
    parser.add_argument('--format', choices=['html', 'markdown', 'json'], default='html', 
                       help='Output format')
    parser.add_argument('--modules', help='Comma-separated list of modules to process')
    
    args = parser.parse_args()
    
    # Convert to Path objects
    source_dir = Path(args.source_dir)
    output_dir = Path(args.output_dir)
    
    # Extract modules
    print("Extracting module information from source code...")
    modules = extract_modules_from_source(source_dir)
    
    if args.modules:
        # Filter modules based on command line argument
        module_names = [m.strip() for m in args.modules.split(',')]
        filtered_modules = {}
        for module_name in module_names:
            if module_name in modules:
                filtered_modules[module_name] = modules[module_name]
        modules = filtered_modules
    
    print(f"Found {len(modules)} modules")
    total_classes = sum(len(module.classes) for module in modules.values())
    print(f"Found {total_classes} classes")
    
    # Generate documentation
    print(f"Generating {args.format} documentation to {output_dir}...")
    
    if args.format == 'html':
        generate_html_documentation(modules, output_dir)
    elif args.format == 'markdown':
        generate_markdown_documentation(modules, output_dir)
    elif args.format == 'json':
        generate_json_documentation(modules, output_dir)
    
    print("Documentation generation completed!")

if __name__ == '__main__':
    main()