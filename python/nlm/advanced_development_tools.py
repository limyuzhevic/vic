#!/usr/bin/env python3
"""
Advanced Development Tools for NLM Neural Data

This module provides advanced development tools for neural simulation,
including code generation utilities, template systems, plugin framework,
extension API, and integration utilities.
"""

import os
import sys
import json
import tempfile
import shutil
from pathlib import Path
from typing import List, Dict, Tuple, Optional, Any, Union, Callable, Type, Iterator
from dataclasses import dataclass, field
from enum import Enum
import warnings
import jinja2
import string
from string import Template
import subprocess
import importlib.util
from abc import ABC, abstractmethod
from datetime import datetime

warnings.filterwarnings('ignore')

class DevelopmentComponentType(Enum):
    """Types of development components"""
    UTILITY = "utility"
    SCRIPT = "script"
    TEMPLATE = "template"
    PLUGIN = "plugin"
    INTEGRATION = "integration"
    GENERATOR = "generator"
    EXTENSION = "extension"
    ANALYZER = "analyzer"

class CodeGenerationTarget(Enum):
    """Targets for code generation"""
    JUPYTER_NOTEBOOK = "jupyter_notebook"
    PYTHON_MODULE = "python_module"
    COMMAND_LINE_TOOL = "command_line_tool"
    WEB_APPLICATION = "web_application"
    API_ENDPOINT = "api_endpoint"
    DOCUMENTATION = "documentation"
    TEST_SUITE = "test_suite"
    SETUP_SCRIPT = "setup_script"

class TemplateEngine(Enum):
    """Template engines"""
    JINJA2 = "jinja2"
    STRING_TEMPLATE = "string_template"
    MUSTACHE = "mustache"
    HANDLEBARS = "handlebars"
    PYTHON_FORMAT = "python_format"

@dataclass
class DevelopmentConfig:
    """Configuration for development tools"""
    project_root: str = "./"
    template_engine: TemplateEngine = TemplateEngine.JINJA2
    template_directory: str = "templates"
    output_directory: str = "output"
    code_generation_directory: str = "generated_code"
    template_cache_enabled: bool = True
    template_cache_duration: int = 3600  # seconds
    validation_enabled: bool = True
    linting_enabled: bool = True
    type_checking_enabled: bool = True
    documentation_generation_enabled: bool = True
    example_generation_enabled: bool = True
    test_generation_enabled: bool = True
    project_structure_template: str = "standard"
    naming_conventions: Dict[str, str] = field(default_factory=dict)

@dataclass
class CodeTemplate:
    """Template for code generation"""
    name: str
    description: str
    template_type: DevelopmentComponentType
    target: CodeGenerationTarget
    template_content: str
    variables: Dict[str, Any] = field(default_factory=dict)
    conditional_sections: List[Dict[str, Any]] = field(default_factory=list)
    file_pattern: str = "{name}.{extension}"
    dependencies: List[str] = field(default_factory=list)
    tags: List[str] = field(default_factory=list)
    author: Optional[str] = None
    version: str = "1.0.0"
    created_at: str = field(default_factory=lambda: datetime.now().isoformat())

@dataclass
class GeneratedComponent:
    """Generated code component"""
    name: str
    path: str
    content: str
    component_type: DevelopmentComponentType
    target: CodeGenerationTarget
    dependencies: List[str]
    metadata: Dict[str, Any] = field(default_factory=dict)
    generated_at: str = field(default_factory=lambda: datetime.now().isoformat())

@dataclass
class ExtensionDescriptor:
    """Descriptor for extensions"""
    name: str
    version: str
    author: str
    description: str
    entry_point: str
    dependencies: List[str] = field(default_factory=list)
    capabilities: List[str] = field(default_factory=list)
    extension_type: str
    configuration: Dict[str, Any] = field(default_factory=dict)
    enabled: bool = True
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class PluginInfo:
    """Information about plugin"""
    plugin_id: str
    name: str
    version: str
    author: str
    description: str
    plugin_class: str
    entry_point: str
    capabilities: List[str]
    configuration: Dict[str, Any] = field(default_factory=dict)
    enabled: bool = True
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class IntegrationRule:
    """Rule for system integration"""
    rule_id: str
    source_component: str
    target_component: str
    transformation_type: str
    configuration: Dict[str, Any] = field(default_factory=dict)
    priority: int = 1
    enabled: bool = True
class CodeGenerator:
    """Advanced code generator for neural simulation projects"""
    
    def __init__(self, config: Optional[DevelopmentConfig] = None):
        self.config = config or DevelopmentConfig()
        self.template_engine = self._init_template_engine()
        self.template_cache = {}
        self._setup_directories()
    
    def _init_template_engine(self):
        """Initialize template engine"""
        if self.config.template_engine == TemplateEngine.JINJA2:
            return Jinja2TemplateEngine()
        elif self.config.template_engine == TemplateEngine.STRING_TEMPLATE:
            return StringTemplateEngine()
        else:
            return Jinja2TemplateEngine()  # Default to Jinja2
    
    def _setup_directories(self):
        """Setup project directories"""
        os.makedirs(self.config.template_directory, exist_ok=True)
        os.makedirs(self.config.output_directory, exist_ok=True)
        os.makedirs(self.config.code_generation_directory, exist_ok=True)
    
    def load_template(self, template_path: str) -> CodeTemplate:
        """Load template from file"""
        try:
            with open(template_path, 'r') as f:
                content = f.read()
            
            # Parse template metadata from content
            metadata = self._parse_template_metadata(content)
            
            # Create template file
            template_file = Path(template_path)
            template = CodeTemplate(
                name=template_file.stem,
                description=metadata.get('description', ''),
                template_type=DevelopmentComponentType(metadata.get('type', 'utility')),
                target=CodeGenerationTarget(metadata.get('target', 'python_module')),
                template_content=content,
                variables=metadata.get('variables', {}),
                conditional_sections=metadata.get('conditional_sections', []),
                file_pattern=metadata.get('file_pattern', '{name}.{extension}'),
                dependencies=metadata.get('dependencies', []),
                tags=metadata.get('tags', []),
                author=metadata.get('author'),
                version=metadata.get('version', '1.0.0')
            )
            
            # Cache template
            self.template_cache[template.name] = template
            
            return template
            
        except Exception as e:
            raise GenerationError(f"Failed to load template from {template_path}: {e}")
    
    def _parse_template_metadata(self, content: str) -> Dict[str, Any]:
        """Parse template metadata from content"""
        metadata = {}
        
        # Look for YAML front matter (for Jinja2) or comment blocks
        lines = content.split('\n')
        in_metadata = False
        metadata_lines = []
        
        for line in lines:
            if line.strip().startswith('#') and 'TEMPLATE METADATA' in line:
                in_metadata = True
                continue
            
            if in_metadata and line.strip().startswith('#') and 'END METADATA' in line:
                break
            
            if in_metadata:
                metadata_lines.append(line.strip())
        
        # Parse metadata lines
        for line in metadata_lines:
            if ':' in line:
                key, value = line.split(':', 1)
                metadata[key.strip()] = value.strip()
        
        return metadata
    
    def generate_code(self, template: CodeTemplate, variables: Dict[str, Any],
                     output_path: Optional[str] = None) -> GeneratedComponent:
        """Generate code from template"""
        try:
            # Prepare template variables
            template_vars = self._prepare_template_variables(template, variables)
            
            # Render template
            rendered_content = self.template_engine.render(template.template_content, template_vars)
            
            # Determine output path
            if output_path is None:
                output_path = self._determine_output_path(template, template_vars)
            
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            
            # Write generated code
            with open(output_path, 'w') as f:
                f.write(rendered_content)
            
            # Create generated component info
            component = GeneratedComponent(
                name=template.name,
                path=output_path,
                content=rendered_content,
                component_type=template.template_type,
                target=template.target,
                dependencies=template.dependencies,
                metadata={
                    'template_name': template.name,
                    'template_version': template.version,
                    'author': template.author,
                    'description': template.description,
                    'tags': template.tags,
                    'target': template.target.value
                }
            )
            
            # Validate generated code
            if self.config.validation_enabled:
                self._validate_generated_code(component)
            
            return component
            
        except Exception as e:
            raise GenerationError(f"Failed to generate code: {e}")
    
    def _prepare_template_variables(self, template: CodeTemplate, variables: Dict[str, Any]) -> Dict[str, Any]:
        """Prepare template variables with defaults and transformations"""
        # Merge template defaults with user variables
        merged_vars = {}
        
        # Add template variables
        for key, value in template.variables.items():
            merged_vars[key] = value
        
        # Add user variables (override template defaults)
        for key, value in variables.items():
            merged_vars[key] = value
        
        # Apply template-specific transformations
        merged_vars = self._apply_template_transformations(merged_vars, template)
        
        # Apply naming conventions
        merged_vars = self._apply_naming_conventions(merged_vars)
        
        return merged_vars
    
    def _apply_template_transformations(self, variables: Dict[str, Any], template: CodeTemplate) -> Dict[str, Any]:
        """Apply template-specific transformations to variables"""
        transformed_vars = variables.copy()
        
        # Handle conditional sections
        for condition in template.conditional_sections:
            condition_name = condition.get('name')
            condition_vars = condition.get('variables', {})
            condition_check = condition.get('check')
            
            if condition_check:
                # Evaluate condition
                try:
                    # Simple condition evaluation
                    if condition_name in variables:
                        transformed_vars.update(condition_vars)
                except:
                    pass
        
        return transformed_vars
    
    def _apply_naming_conventions(self, variables: Dict[str, Any]) -> Dict[str, Any]:
        """Apply naming conventions to variables"""
        if not self.config.naming_conventions:
            return variables
        
        transformed_vars = variables.copy()
        
        for var_name, convention in self.config.naming_conventions.items():
            if var_name in transformed_vars:
                # Apply naming convention transformation
                if convention == 'snake_case':
                    transformed_vars[var_name] = self._to_snake_case(transformed_vars[var_name])
                elif convention == 'kebab_case':
                    transformed_vars[var_name] = self._to_kebab_case(transformed_vars[var_name])
                elif convention == 'camel_case':
                    transformed_vars[var_name] = self._to_camel_case(transformed_vars[var_name])
                elif convention == 'pascal_case':
                    transformed_vars[var_name] = self._to_pascal_case(transformed_vars[var_name])
        
        return transformed_vars
    
    def _to_snake_case(self, text: str) -> str:
        """Convert text to snake_case"""
        import re
        text = re.sub('([A-Z])', r'_\1', text).lower()
        return re.sub('^_', '', text)
    
    def _to_kebab_case(self, text: str) -> str:
        """Convert text to kebab-case"""
        import re
        text = re.sub('([A-Z])', r'-\1', text).lower()
        return re.sub('^-', '', text)
    
    def _to_camel_case(self, text: str) -> str:
        """Convert text to camelCase"""
        import re
        words = re.sub('([A-Z])', r' \1', text).split()
        return words[0].lower() + ''.join(word.capitalize() for word in words[1:])
    
    def _to_pascal_case(self, text: str) -> str:
        """Convert text to PascalCase"""
        import re
        words = re.sub('([A-Z])', r' \1', text).split()
        return ''.join(word.capitalize() for word in words)
    
    def _determine_output_path(self, template: CodeTemplate, variables: Dict[str, Any]) -> str:
        """Determine output file path based on template and variables"""
        file_pattern = template.file_pattern
        
        # Replace placeholders in file pattern
        for key, value in variables.items():
            placeholder = '{' + key + '}'
            if placeholder in file_pattern:
                file_pattern = file_pattern.replace(placeholder, str(value))
        
        # Add extension if not present
        if '{extension}' not in file_pattern:
            extension = self._get_extension_for_target(template.target)
            file_pattern = file_pattern + '.' + extension
        
        # Replace remaining placeholders
        for key, value in variables.items():
            placeholder = '{' + key + '}'
            if placeholder in file_pattern:
                file_pattern = file_pattern.replace(placeholder, str(value))
        
        # Construct full path
        output_path = os.path.join(self.config.output_directory, file_pattern)
        
        return output_path
    
    def _get_extension_for_target(self, target: CodeGenerationTarget) -> str:
        """Get file extension for target type"""
        extensions = {
            CodeGenerationTarget.JUPYTER_NOTEBOOK: 'ipynb',
            CodeGenerationTarget.PYTHON_MODULE: 'py',
            CodeGenerationTarget.COMMAND_LINE_TOOL: 'py',
            CodeGenerationTarget.WEB_APPLICATION: 'py',
            CodeGenerationTarget.API_ENDPOINT: 'py',
            CodeGenerationTarget.DOCUMENTATION: 'md',
            CodeGenerationTarget.TEST_SUITE: 'py',
            CodeGenerationTarget.SETUP_SCRIPT: 'py'
        }
        
        return extensions.get(target, 'txt')
    
    def _validate_generated_code(self, component: GeneratedComponent):
        """Validate generated code"""
        # This would be expanded with actual validation logic
        # For now, perform basic checks
        if not component.content:
            raise GenerationError("Generated code is empty")
        
        # Check for syntax errors (for Python)
        if component.component_type == DevelopmentComponentType.SCRIPT:
            try:
                compile(component.content, component.name, 'exec')
            except SyntaxError as e:
                raise GenerationError(f"Syntax error in generated code: {e}")
    
    def generate_project_structure(self, project_type: str, project_name: str,
                                  variables: Dict[str, Any]) -> List[GeneratedComponent]:
        """Generate complete project structure"""
        components = []
        
        # Get template for project type
        template_name = f"{project_type}_project"
        
        if template_name in self.template_cache:
            template = self.template_cache[template_name]
        else:
            # Load project template
            template_path = os.path.join(self.config.template_directory, f"{template_name}.j2")
            if os.path.exists(template_path):
                template = self.load_template(template_path)
                self.template_cache[template_name] = template
            else:
                # Create default project template
                template = self._create_default_project_template(project_type, project_name)
                self.template_cache[template_name] = template
        
        # Generate components
        for sub_template in self._get_sub_templates(template):
            component = self.generate_code(sub_template, variables)
            components.append(component)
        
        return components
    
    def _create_default_project_template(self, project_type: str, project_name: str) -> CodeTemplate:
        """Create default project template"""
        template_content = f"""# {project_name} Project
# Type: {project_type}
# Generated by Advanced Code Generator

{{{{
    "name": "{project_name}",
    "type": "{project_type}",
    "description": "A neural simulation project generated by NLM development tools",
    "version": "0.1.0",
    "author": "Generated Author",
    "created_at": "{datetime.now().isoformat()}",
    "variables": {{"n_components": 10, "simulation_duration": 60, "target_fps": 60}},
    "conditional_sections": [],
    "dependencies": ["numpy", "scipy", "sklearn"],
    "tags": ["{project_type}", "neural", "simulation"],
    "file_pattern": "{{{{name}}}}.{{{{extension}}}}",
    "target": "python_module"
}}}}

# Main module
import numpy as np
import pandas as pd
from typing import List, Dict, Any

class {project_name.title().replace('_', '')}:
    \"\"\"Main class for {project_name} project\"\"\"
    
    def __init__(self, config: Dict[str, Any] = None):
        self.config = config or {{}}
        self.initialized = False
    
    def initialize(self):
        \"\"\"Initialize the project\"\"\"
        print(f"Initializing {{self.config.get('name', '{project_name}')}}...")
        self.initialized = True
    
    def run(self):
        \"\"\"Run the main simulation\"\"\"
        if not self.initialized:
            self.initialize()
        
        print("Running simulation...")
        # Simulation logic would go here
        return {{'status': 'completed', 'results': {{}}}}
    
    def analyze(self, data):
        \"\"\"Analyze data\"\"\"
        # Analysis logic would go here
        return {{'analysis': 'results'}}
"""
        
        return CodeTemplate(
            name=f"{project_name}_project",
            description=f"Project template for {project_type}",
            template_type=DevelopmentComponentType.SCRIPT,
            target=CodeGenerationTarget.PYTHON_MODULE,
            template_content=template_content,
            variables={
                'name': project_name,
                'type': project_type,
                'description': f'A neural simulation project generated by NLM development tools',
                'version': '0.1.0',
                'author': 'Generated Author',
                'created_at': datetime.now().isoformat(),
                'dependencies': ['numpy', 'scipy', 'sklearn'],
                'tags': [project_type, 'neural', 'simulation'],
                'file_pattern': '{name}.{extension}',
                'target': 'python_module'
            },
            conditional_sections=[],
            file_pattern="{name}.{extension}",
            dependencies=['numpy', 'scipy', 'sklearn'],
            tags=[project_type, 'neural', 'simulation'],
            author='Generated Author',
            version='0.1.0'
        )
    
    def _get_sub_templates(self, template: CodeTemplate) -> List[CodeTemplate]:
        """Get sub-templates for project generation"""
        # This would be expanded with actual sub-template logic
        # For now, return the main template
        return [template]

class Jinja2TemplateEngine:
    """Jinja2 template engine"""
    
    def __init__(self):
        try:
            import jinja2
            self.jinja2 = jinja2
        except ImportError:
            raise TemplateEngineError("Jinja2 not installed. Install with: pip install jinja2")
    
    def render(self, template_content: str, variables: Dict[str, Any]) -> str:
        """Render template with variables"""
        # Create Jinja2 environment
        env = self.jinja2.Environment(
            loader=self.jinja2.DictLoader({'template': template_content}),
            trim_blocks=True,
            lstrip_blocks=True
        )
        
        # Add custom filters
        env.filters['to_json'] = json.dumps
        env.filters['list'] = list
        env.filters['dict'] = dict
        
        # Render template
        template = env.get_template('template')
        return template.render(**variables)

class StringTemplateEngine:
    """String template engine"""
    
    def render(self, template_content: str, variables: Dict[str, Any]) -> str:
        """Render template with variables using string.Template"""
        template = Template(template_content)
        return template.substitute(variables)

class TemplateSystem:
    """Advanced template system for neural simulation projects"""
    
    def __init__(self, template_dir: str = "templates"):
        self.template_dir = template_dir
        self.templates: Dict[str, Dict[str, Any]] = {}
        self._load_templates()
    
    def _load_templates(self):
        """Load all templates from template directory"""
        if os.path.exists(self.template_dir):
            for filename in os.listdir(self.template_dir):
                if filename.endswith('.j2') or filename.endswith('.jinja2'):
                    template_name = filename[:-3]  # Remove extension
                    try:
                        with open(os.path.join(self.template_dir, filename), 'r') as f:
                            template_content = f.read()
                        
                        # Parse template metadata
                        metadata = self._parse_template_metadata(template_content)
                        
                        self.templates[template_name] = {
                            'content': template_content,
                            'metadata': metadata,
                            'path': os.path.join(self.template_dir, filename)
                        }
                    except Exception as e:
                        print(f"Error loading template {filename}: {e}")
    
    def _parse_template_metadata(self, content: str) -> Dict[str, Any]:
        """Parse template metadata from content"""
        metadata = {}
        
        # Look for YAML front matter
        if content.startswith('---'):
            parts = content.split('---')
            if len(parts) >= 3:
                metadata_text = parts[1]
                try:
                    metadata = yaml.safe_load(metadata_text)
                except:
                    pass
        
        return metadata
    
    def list_templates(self) -> List[str]:
        """List available templates"""
        return list(self.templates.keys())
    
    def get_template(self, template_name: str) -> Dict[str, Any]:
        """Get template by name"""
        if template_name not in self.templates:
            raise TemplateNotFoundError(f"Template '{template_name}' not found")
        
        return self.templates[template_name]
    
    def create_from_template(self, template_name: str, variables: Dict[str, Any],
                           output_path: str) -> bool:
        """Create content from template"""
        try:
            template = self.get_template(template_name)
            
            # Create Jinja2 environment
            env = jinja2.Environment(
                trim_blocks=True,
                lstrip_blocks=True
            )
            
            template_obj = env.from_string(template['content'])
            rendered_content = template_obj.render(**variables)
            
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            
            # Write rendered content
            with open(output_path, 'w') as f:
                f.write(rendered_content)
            
            return True
            
        except Exception as e:
            print(f"Error creating template {template_name}: {e}")
            return False

class ExtensionAPI:
    """Extension API for adding new functionality to NLM"""
    
    def __init__(self, extensions_dir: str = "extensions"):
        self.extensions_dir = extensions_dir
        self.extensions: Dict[str, ExtensionDescriptor] = {}
        self.loaded_extensions: Dict[str, Any] = {}
        self._load_extensions()
    
    def _load_extensions(self):
        """Load all extensions from extensions directory"""
        if os.path.exists(self.extensions_dir):
            for extension_dir in os.listdir(self.extensions_dir):
                extension_path = os.path.join(self.extensions_dir, extension_dir)
                if os.path.isdir(extension_path):
                    self._load_extension(extension_dir, extension_path)
    
    def _load_extension(self, extension_id: str, extension_path: str):
        """Load single extension"""
        try:
            # Look for extension descriptor file
            descriptor_path = os.path.join(extension_path, 'extension.json')
            if not os.path.exists(descriptor_path):
                return
            
            with open(descriptor_path, 'r') as f:
                descriptor = json.load(f)
            
            # Validate extension descriptor
            extension_info = ExtensionDescriptor(
                name=descriptor.get('name', extension_id),
                version=descriptor.get('version', '1.0.0'),
                author=descriptor.get('author', 'Unknown'),
                description=descriptor.get('description', ''),
                entry_point=descriptor.get('entry_point', 'main'),
                dependencies=descriptor.get('dependencies', []),
                capabilities=descriptor.get('capabilities', []),
                extension_type=descriptor.get('extension_type', 'plugin'),
                configuration=descriptor.get('configuration', {}),
                enabled=descriptor.get('enabled', True),
                metadata=descriptor.get('metadata', {})
            )
            
            # Store extension info
            self.extensions[extension_id] = extension_info
            
            # Load extension if enabled
            if extension_info.enabled:
                self._load_extension_module(extension_id, extension_path, extension_info)
            
        except Exception as e:
            print(f"Error loading extension {extension_id}: {e}")
    
    def _load_extension_module(self, extension_id: str, extension_path: str,
                              extension_info: ExtensionDescriptor):
        """Load extension Python module"""
        try:
            # Add extension path to sys.path
            sys.path.insert(0, extension_path)
            
            # Import extension module
            module_name = extension_info.entry_point
            spec = importlib.util.spec_from_file_location(
                module_name,
                os.path.join(extension_path, f"{module_name}.py")
            )
            
            if spec and spec.loader:
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                
                # Store loaded module
                self.loaded_extensions[extension_id] = module
                
                # Call initialization function if available
                if hasattr(module, 'initialize'):
                    module.initialize(extension_info.configuration)
            
        except Exception as e:
            print(f"Error loading extension module {extension_id}: {e}")
    
    def list_extensions(self) -> List[Dict[str, Any]]:
        """List all loaded extensions"""
        extensions_list = []
        
        for extension_id, extension_info in self.extensions.items():
            extensions_list.append({
                'id': extension_id,
                'name': extension_info.name,
                'version': extension_info.version,
                'author': extension_info.author,
                'description': extension_info.description,
                'enabled': extension_info.enabled,
                'capabilities': extension_info.capabilities,
                'extension_type': extension_info.extension_type
            })
        
        return extensions_list
    
    def enable_extension(self, extension_id: str) -> bool:
        """Enable extension"""
        if extension_id not in self.extensions:
            return False
        
        extension_info = self.extensions[extension_id]
        extension_info.enabled = True
        
        # Reload extension module
        if extension_id in self.loaded_extensions:
            del self.loaded_extensions[extension_id]
        
        # Load extension module
        extension_path = os.path.join(self.extensions_dir, extension_id)
        if os.path.exists(extension_path):
            self._load_extension_module(extension_id, extension_path, extension_info)
        
        return True
    
    def disable_extension(self, extension_id: str) -> bool:
        """Disable extension"""
        if extension_id not in self.extensions:
            return False
        
        extension_info = self.extensions[extension_id]
        extension_info.enabled = False
        
        # Remove extension module
        if extension_id in self.loaded_extensions:
            del self.loaded_extensions[extension_id]
        
        return True
    
    def get_extension(self, extension_id: str) -> Optional[Any]:
        """Get loaded extension module"""
        return self.loaded_extensions.get(extension_id)
    
    def call_extension_function(self, extension_id: str, function_name: str,
                               *args, **kwargs) -> Any:
        """Call function in extension"""
        extension = self.get_extension(extension_id)
        
        if not extension:
            raise ExtensionError(f"Extension '{extension_id}' not loaded")
        
        if not hasattr(extension, function_name):
            raise ExtensionError(f"Function '{function_name}' not found in extension '{extension_id}'")
        
        return getattr(extension, function_name)(*args, **kwargs)
class CodeGeneratorPlugin:
    """Plugin for code generation"""
    
    def __init__(self, config: Optional[DevelopmentConfig] = None):
        self.config = config or DevelopmentConfig()
        self.code_generator = CodeGenerator(self.config)
    
    def generate_utility_script(self, template_name: str, variables: Dict[str, Any],
                               output_path: Optional[str] = None) -> GeneratedComponent:
        """Generate utility script"""
        # Load template
        template = self.code_generator.load_template(
            os.path.join(self.config.template_directory, f"{template_name}.j2")
        )
        
        # Generate code
        return self.code_generator.generate_code(template, variables, output_path)
    
    def generate_analysis_script(self, template_name: str, variables: Dict[str, Any],
                                output_path: Optional[str] = None) -> GeneratedComponent:
        """Generate analysis script"""
        # Load template
        template = self.code_generator.load_template(
            os.path.join(self.config.template_directory, f"{template_name}.j2")
        )
        
        # Generate code
        return self.code_generator.generate_code(template, variables, output_path)
    
    def generate_test_suite(self, template_name: str, variables: Dict[str, Any],
                           output_path: Optional[str] = None) -> GeneratedComponent:
        """Generate test suite"""
        # Load template
        template = self.code_generator.load_template(
            os.path.join(self.config.template_directory, f"{template_name}.j2")
        )
        
        # Generate code
        return self.code_generator.generate_code(template, variables, output_path)
class DocumentationGenerator:
    """Generate documentation for neural simulation projects"""
    
    def __init__(self, docs_dir: str = "docs"):
        self.docs_dir = docs_dir
        os.makedirs(self.docs_dir, exist_ok=True)
    
    def generate_api_documentation(self, project_name: str, components: List[GeneratedComponent]) -> str:
        """Generate API documentation"""
        api_doc = f"# {project_name} API Documentation\n\n"
        
        for component in components:
            api_doc += f"## {component.name}\n\n"
            api_doc += f"**File**: {component.path}\n\n"
            api_doc += f"**Type**: {component.component_type.value}\n\n"
            api_doc += f"**Generated**: {component.generated_at}\n\n"
            api_doc += f"```python\n{component.content}\n```\n\n"
        
        # Write to file
        output_path = os.path.join(self.docs_dir, f"{project_name}_api.md")
        with open(output_path, 'w') as f:
            f.write(api_doc)
        
        return output_path
    
    def generate_user_guide(self, project_name: str, variables: Dict[str, Any]) -> str:
        """Generate user guide"""
        guide = f"# {project_name} User Guide\n\n"
        guide += "## Overview\n\n"
        guide += f"This project generates neural simulations using {project_name}.\n\n"
        guide += "## Usage Examples\n\n"
        guide += "```python\n"
        guide += f"from {project_name} import {project_name.title().replace('_', '')}\n"
        guide += f"\n"
        guide += f"# Create instance\n"
        guide += f"sim = {project_name.title().replace('_', '')}()\n"
        guide += f"\n"
        guide += f"# Initialize and run\n"
        guide += f"sim.initialize()\n"
        guide += f"results = sim.run()\n"
        guide += "```\n\n"
        guide += "## Configuration\n\n"
        guide += f"The project supports the following configuration variables:\n\n"
        for key, value in variables.items():
            guide += f"- `{key}`: {value}\n"
        
        # Write to file
        output_path = os.path.join(self.docs_dir, f"{project_name}_user_guide.md")
        with open(output_path, 'w') as f:
            f.write(guide)
        
        return output_path
    
    def generate_tutorial(self, project_name: str, tutorial_steps: List[Dict[str, Any]]) -> str:
        """Generate tutorial"""
        tutorial = f"# {project_name} Tutorial\n\n"
        
        for i, step in enumerate(tutorial_steps, 1):
            title = step.get('title', f'Step {i}')
            content = step.get('content', '')
            code = step.get('code', '')
            
            tutorial += f"## {title}\n\n"
            tutorial += f"{content}\n\n"
            
            if code:
                tutorial += "```python\n"
                tutorial += f"{code}\n"
                tutorial += "```\n\n"
        
        # Write to file
        output_path = os.path.join(self.docs_dir, f"{project_name}_tutorial.md")
        with open(output_path, 'w') as f:
            f.write(tutorial)
        
        return output_path
class IntegrationManager:
    """Manage integration between neural simulation components"""
    
    def __init__(self):
        self.integration_rules: List[IntegrationRule] = []
        self.component_connections: Dict[str, List[str]] = {}
    
    def add_integration_rule(self, rule: IntegrationRule):
        """Add integration rule"""
        self.integration_rules.append(rule)
    
    def apply_integration(self, source_component: str, target_component: str,
                         data: Any, configuration: Dict[str, Any] = None) -> Any:
        """Apply integration rules"""
        # Find matching integration rules
        matching_rules = []
        
        for rule in self.integration_rules:
            if (rule.source_component == source_component and 
                rule.target_component == target_component and
                rule.enabled):
                matching_rules.append(rule)
        
        # Apply matching rules
        result = data
        
        # Sort by priority (higher priority first)
        matching_rules.sort(key=lambda x: x.priority, reverse=True)
        
        for rule in matching_rules:
            try:
                # Apply transformation
                result = self._apply_integration_rule(result, rule, configuration)
            except Exception as e:
                print(f"Error applying integration rule {rule.rule_id}: {e}")
                continue
        
        return result
    
    def _apply_integration_rule(self, data: Any, rule: IntegrationRule, 
                              configuration: Dict[str, Any]) -> Any:
        """Apply single integration rule"""
        if rule.transformation_type == 'data_transformation':
            return self._transform_data(data, rule.configuration)
        elif rule.transformation_type == 'function_binding':
            return self._bind_function(data, rule.configuration)
        elif rule.transformation_type == 'plugin_loading':
            return self._load_plugin(data, rule.configuration)
        else:
            return data
    
    def _transform_data(self, data: Any, config: Dict[str, Any]) -> Any:
        """Transform data based on configuration"""
        # This would be expanded with actual data transformation logic
        return data
    
    def _bind_function(self, data: Any, config: Dict[str, Any]) -> Any:
        """Bind function to data"""
        # This would be expanded with actual function binding logic
        return data
    
    def _load_plugin(self, data: Any, config: Dict[str, Any]) -> Any:
        """Load plugin based on configuration"""
        # This would be expanded with actual plugin loading logic
        return data
    
    def get_component_connections(self, component_id: str) -> List[str]:
        """Get components connected to specified component"""
        return self.component_connections.get(component_id, [])
    
    def add_component_connection(self, component_id: str, connected_component: str):
        """Add component connection"""
        if component_id not in self.component_connections:
            self.component_connections[component_id] = []
        
        if connected_component not in self.component_connections[component_id]:
            self.component_connections[component_id].append(connected_component)

class AdvancedDevelopmentTools:
    """Main advanced development tools for NLM neural data"""
    
    def __init__(self):
        self.config = DevelopmentConfig()
        self.code_generator = CodeGenerator(self.config)
        self.template_system = TemplateSystem()
        self.extension_api = ExtensionAPI()
        self.code_generator_plugin = CodeGeneratorPlugin(self.config)
        self.documentation_generator = DocumentationGenerator()
        self.integration_manager = IntegrationManager()
    
    def generate_project(self, project_type: str, project_name: str,
                        variables: Dict[str, Any]) -> List[GeneratedComponent]:
        """Generate complete project"""
        return self.code_generator.generate_project_structure(project_type, project_name, variables)
    
    def create_template(self, template_name: str, template_content: str,
                       metadata: Dict[str, Any]) -> CodeTemplate:
        """Create new template"""
        template = CodeTemplate(
            name=template_name,
            description=metadata.get('description', ''),
            template_type=DevelopmentComponentType(metadata.get('type', 'utility')),
            target=CodeGenerationTarget(metadata.get('target', 'python_module')),
            template_content=template_content,
            variables=metadata.get('variables', {}),
            conditional_sections=metadata.get('conditional_sections', []),
            file_pattern=metadata.get('file_pattern', '{name}.{extension}'),
            dependencies=metadata.get('dependencies', []),
            tags=metadata.get('tags', []),
            author=metadata.get('author'),
            version=metadata.get('version', '1.0.0')
        )
        
        # Save template
        template_path = os.path.join(self.config.template_directory, f"{template_name}.j2")
        with open(template_path, 'w') as f:
            f.write(template_content)
        
        # Add to code generator cache
        self.code_generator.template_cache[template_name] = template
        
        return template
    
    def add_extension(self, extension_path: str) -> bool:
        """Add new extension"""
        # This would be expanded with actual extension loading logic
        extension_id = os.path.basename(extension_path)
        
        # Load extension
        self.extension_api._load_extension(extension_id, extension_path)
        
        return True
    
    def create_integration_rule(self, source_component: str, target_component: str,
                               transformation_type: str, configuration: Dict[str, Any] = None,
                               priority: int = 1) -> IntegrationRule:
        """Create integration rule"""
        rule = IntegrationRule(
            rule_id=f"{source_component}_{target_component}_{transformation_type}",
            source_component=source_component,
            target_component=target_component,
            transformation_type=transformation_type,
            configuration=configuration or {},
            priority=priority,
            enabled=True
        )
        
        self.integration_manager.add_integration_rule(rule)
        
        return rule
    
    def apply_integrations(self, source_component: str, target_component: str,
                          data: Any, configuration: Dict[str, Any] = None) -> Any:
        """Apply integrations between components"""
        return self.integration_manager.apply_integration(
            source_component, target_component, data, configuration
        )
    
    def get_extension_info(self, extension_id: str) -> Optional[Dict[str, Any]]:
        """Get extension information"""
        extension = self.extension_api.get_extension(extension_id)
        
        if extension is None:
            return None
        
        extension_info = self.extension_api.extensions.get(extension_id)
        
        if extension_info is None:
            return None
        
        return {
            'name': extension_info.name,
            'version': extension_info.version,
            'author': extension_info.author,
            'description': extension_info.description,
            'capabilities': extension_info.capabilities,
            'extension_type': extension_info.extension_type,
            'enabled': extension_info.enabled,
            'configuration': extension_info.configuration
        }
    
    def enable_extension(self, extension_id: str) -> bool:
        """Enable extension"""
        return self.extension_api.enable_extension(extension_id)
    
    def disable_extension(self, extension_id: str) -> bool:
        """Disable extension"""
        return self.extension_api.disable_extension(extension_id)

# Convenience functions

def create_development_tools() -> AdvancedDevelopmentTools:
    """Convenience function to create development tools"""
    return AdvancedDevelopmentTools()

def generate_neural_project(project_type: str, project_name: str,
                           variables: Dict[str, Any]) -> List[GeneratedComponent]:
    """Convenience function to generate neural project"""
    tools = AdvancedDevelopmentTools()
    return tools.generate_project(project_type, project_name, variables)

def create_template_system(template_dir: str = "templates") -> TemplateSystem:
    """Convenience function to create template system"""
    return TemplateSystem(template_dir)

def create_extension_api(extensions_dir: str = "extensions") -> ExtensionAPI:
    """Convenience function to create extension API"""
    return ExtensionAPI(extensions_dir)

# Custom exceptions

class GenerationError(Exception):
    """Error raised when code generation fails"""
    pass

class TemplateEngineError(Exception):
    """Error raised when template engine fails"""
    pass

class TemplateNotFoundError(Exception):
    """Error raised when template is not found"""
    pass

class ExtensionError(Exception):
    """Error raised when extension operation fails"""
    pass

# Export all classes and functions
__all__ = [
    'DevelopmentComponentType', 'CodeGenerationTarget', 'TemplateEngine',
    'DevelopmentConfig', 'CodeTemplate', 'GeneratedComponent',
    'ExtensionDescriptor', 'PluginInfo', 'IntegrationRule',
    'CodeGenerator', 'Jinja2TemplateEngine', 'StringTemplateEngine',
    'TemplateSystem', 'ExtensionAPI', 'CodeGeneratorPlugin',
    'DocumentationGenerator', 'IntegrationManager',
    'AdvancedDevelopmentTools',
    'create_development_tools', 'generate_neural_project',
    'create_template_system', 'create_extension_api',
    'GenerationError', 'TemplateEngineError', 'TemplateNotFoundError',
    'ExtensionError'
]