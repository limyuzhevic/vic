#!/usr/bin/env python3
"""
Simplified Code Quality Analyzer for NLM Project

This script analyzes the NLM codebase and creates targeted improvement plans.
"""

import re
from pathlib import Path
from datetime import datetime

def analyze_config_file(filepath):
    """Analyze configuration file for issues."""
    
    issues = []
    try:
        content = Path(filepath).read_text()
        lines = content.split('\n')
        values = {}
        
        for i, line in enumerate(lines, 1):
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key = line.split('=')[0].strip()
                if key in values:
                    issues.append({
                        'file': filepath,
                        'line': i,
                        'issue': f'Duplicate configuration key: {key}',
                        'severity': 'HIGH',
                        'original': line
                    })
                else:
                    values[key] = i
                    
    except Exception as e:
        print(f"Error analyzing {filepath}: {e}")
    
    return issues

def analyze_todo_comments():
    """Analyze TODO and FIXME comments in source files."""
    
    todos = []
    source_extensions = ['.cpp', '.hpp', '.h']
    
    for ext in source_extensions:
        for filepath in Path('src').rglob(f'*{ext}'):
            try:
                content = filepath.read_text()
                lines = content.split('\n')
                
                for i, line in enumerate(lines, 1):
                    line_lower = line.lower()
                    if any(keyword in line_lower for keyword in ['todo', 'fixme', 'hack', 'xxx']):
                        todos.append({
                            'file': str(filepath),
                            'line': i,
                            'content': line.strip(),
                            'priority': 'HIGH' if any(kw in line_lower for kw in ['todo', 'fixme']) else 'MEDIUM'
                        })
            except Exception as e:
                print(f"Error analyzing {filepath}: {e}")
    
    return todos

def analyze_code_style(filepath):
    """Analyze code style issues."""
    
    issues = []
    
    try:
        content = Path(filepath).read_text()
        lines = content.split('\n')
        
        for i, line in enumerate(lines, 1):
            # Check line length
            if len(line) > 100 and line.strip() and not line.strip().startswith('#'):
                issues.append({
                    'file': filepath,
                    'line': i,
                    'issue': f'Line too long ({len(line)} chars)',
                    'severity': 'LOW'
                })
            
            # Check for tabs
            if '\t' in line:
                issues.append({
                    'file': filepath,
                    'line': i,
                    'issue': 'Contains tabs',
                    'severity': 'MEDIUM'
                })
                
    except Exception as e:
        print(f"Error analyzing {filepath}: {e}")
    
    return issues

def create_improvement_report():
    """Create a comprehensive improvement report."""
    
    print("NLM Project Code Improvement Analysis")
    print("=" * 60)
    print(f"Analysis started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    
    # Analyze configuration files
    config_dir = Path('configs')
    config_issues = []
    
    for config_file in config_dir.glob('*.cfg'):
        issues = analyze_config_file(config_file)
        config_issues.extend(issues)
    
    # Analyze TODO comments
    todos = analyze_todo_comments()
    
    # Analyze code style
    style_issues = []
    for ext in ['.cpp', '.hpp', '.h']:
        for filepath in Path('src').rglob(f'*{ext}'):
            issues = analyze_code_style(filepath)
            style_issues.extend(issues)
    
    # Create summary
    print("ANALYSIS SUMMARY")
    print("=" * 60)
    print(f"Configuration files analyzed: {len(config_dir.glob('*.cfg'))}")
    print(f"Configuration issues found: {len(config_issues)}")
    print(f"Source files analyzed: {len(list(Path('src').rglob('*.*')))}")
    print(f"TODO/FIXME comments found: {len(todos)}")
    print(f"Code style issues: {len(style_issues)}")
    print()
    
    # Display issues
    if config_issues:
        print("CONFIGURATION ISSUES:")
        for i, issue in enumerate(config_issues, 1):
            print(f"{i}. {issue['file']} (line {issue['line']}): {issue['issue']}")
            print(f"   Priority: {issue['severity']}")
        print()
    
    if todos:
        print("TODO/FIXME COMMENTS:")
        for i, todo in enumerate(todos[:5], 1):  # Show first 5
            print(f"{i}. {todo['file']} (line {todo['line']}): {todo['content']}")
            print(f"   Priority: {todo['priority']}")
        if len(todos) > 5:
            print(f"   ... and {len(todos) - 5} more")
        print()
    
    # Create improvement plans directory
    improvement_dir = Path('improvement_plans')
    improvement_dir.mkdir(exist_ok=True)
    
    # Create configuration fix plan
    if config_issues:
        config_plan = improvement_dir / 'configuration_fixes.md'
        config_plan_content = f"""# Configuration Fixes for NLM Project

**Generated**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
**Total Issues**: {len(config_issues)}

## Priority Fixes

### 1. Duplicate Configuration Values
The following configuration files contain duplicate keys that need to be fixed:

"""
        
        for i, issue in enumerate(config_issues, 1):
            config_plan_content += f"""
#### {i}. {issue['file']}
- **Line**: {issue['line']}
- **Issue**: {issue['issue']}
- **Action**: Remove duplicate line and ensure unique configuration values
- **Priority**: {issue['severity']}
"""
        
        config_plan_content += f"""
## Recommended Action Plan

### Phase 1: Immediate Fixes (High Priority)
1. Fix duplicate configuration values in all affected files
2. Validate configuration consistency
3. Test configuration loading with corrected files

### Phase 2: Long-term Improvements
1. Implement configuration validation system
2. Add JSON/YAML support for better configuration management
3. Create configuration templates and documentation

## Files to Modify
"""
        
        for issue in config_issues:
            config_plan_content += f"- {issue['file']}\n"
        
        config_plan.write_text(config_plan_content)
        print(f"Created configuration fixes plan: improvement_plans/configuration_fixes.md")
    
    # Create TODO handling plan
    if todos:
        todo_plan = improvement_dir / 'todo_handling.md'
        todo_plan_content = f"""# TODO/FIXME Handling for NLM Project

**Generated**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
**Total TODOs**: {len(todos)}
**High Priority**: {len([t for t in todos if t['priority'] == 'HIGH'])}

## Overview
This project contains {len(todos)} TODO and FIXME comments that indicate incomplete implementations or areas requiring attention.

## High Priority TODOs (Immediate Action Required)

"""
        
        high_priority_todos = [t for t in todos if t['priority'] == 'HIGH']
        for i, todo in enumerate(high_priority_todos[:3], 1):
            todo_plan_content += f"""
#### {i}. {todo['file']} (Line {todo['line']})
- **Content**: {todo['content']}
- **Action**: Implement missing functionality or fix placeholder code
- **Risk**: High - May affect core simulation behavior
- **Estimated Effort**: 8-24 hours per TODO

"""
        
        todo_plan_content += """
## Medium Priority TODOs (Scheduled for Later)

"""
        
        medium_priority_todos = [t for t in todos if t['priority'] == 'MEDIUM']
        for i, todo in enumerate(medium_priority_todos[:3], 1):
            todo_plan_content += f"""
#### {i}. {todo['file']} (Line {todo['line']})
- **Content**: {todo['content']}
- **Action**: Improve code quality or add enhancements
- **Risk**: Medium - May affect performance or usability
- **Estimated Effort**: 4-8 hours per TODO

"""
        
        todo_plan_content += f"""
## Recommended Action Plan

### Immediate (Week 1)
1. **Priority 1**: Fix all high-priority TODOs that affect Phase 6 integration
2. **Priority 2**: Address configuration file duplication issues
3. **Priority 3**: Improve error handling in critical neural systems

### Medium-term (Weeks 2-4)
1. **Priority 4**: Address medium-priority TODOs
2. **Priority 5**: Implement code quality improvements
3. **Priority 6**: Add documentation for new functionality

### Long-term (Ongoing)
1. **Continuous**: Regular code reviews to prevent TODO accumulation
2. **Continuous**: Automated testing to ensure no regressions
3. **Continuous**: Performance optimization and profiling

## Files Most Affected
"""
        
        affected_files = {}
        for todo in todos:
            if todo['file'] not in affected_files:
                affected_files[todo['file']] = 0
            affected_files[todo['file']] += 1
        
        for file, count in sorted(affected_files.items(), key=lambda x: x[1], reverse=True)[:5]:
            todo_plan_content += f"- {file}: {count} TODOs\n"
        
        todo_plan_content += f"""
## Impact Assessment

### High Impact Areas
- **Neural Dynamics**: Core neuron and synapse implementations
- **Memory Systems**: Working memory and episodic memory
- **Plasticity**: STDP and Hebbian learning mechanisms
- **Integration**: Phase 6 system integration

### Medium Impact Areas
- **Configuration Management**: Improved configuration parsing
- **Error Handling**: Better error reporting and recovery
- **Code Quality**: Documentation and coding standards

### Low Impact Areas
- **Visualization**: Enhanced visualization capabilities
- **User Interface**: Advanced user interaction features
- **Performance**: Minor optimizations and caching

## Success Metrics
1. **TODO Removal**: Reduce TODO count by 90% within 6 months
2. **Code Quality**: Achieve 100% documentation coverage for public APIs
3. **Testing**: Increase test coverage to 80% for critical components
4. **Performance**: Improve performance by 20-50% where possible
5. **Reliability**: Reduce runtime errors by 50% through better error handling
"""
        
        todo_plan.write_text(todo_plan_content)
        print(f"Created TODO handling plan: improvement_plans/todo_handling.md")
    
    # Create code quality improvement plan
    if style_issues:
        quality_plan = improvement_dir / 'code_quality_improvements.md'
        quality_plan_content = f"""# Code Quality Improvements for NLM Project

**Generated**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
**Total Issues**: {len(style_issues)}

## Code Quality Issues Identified

### Line Length Issues
"""
        
        long_lines = [i for i in style_issues if 'too long' in i['issue'].lower()]
        if long_lines:
            quality_plan_content += f"- **Long lines**: {len(long_lines)} lines exceed 100 characters\n"
        
        tab_issues = [i for i in style_issues if 'tabs' in i['issue'].lower()]
        if tab_issues:
            quality_plan_content += f"- **Tab characters**: {len(tab_issues)} instances found\n"
        
        quality_plan_content += """
## Recommended Code Quality Improvements

### 1. Coding Standards
- [ ] Adopt consistent indentation (2 spaces)
- [ ] Use camelCase for variables and methods
- [ ] Use PascalCase for classes
- [ ] Implement consistent naming conventions
- [ ] Add comprehensive comments and documentation

### 2. Code Style
- [ ] Fix line length issues (break long lines)
- [ ] Replace tabs with spaces
- [ ] Implement include guards for all header files
- [ ] Add proper error handling
- [ ] Use smart pointers appropriately

### 3. Best Practices
- [ ] Implement RAII (Resource Acquisition Is Initialization)
- [ ] Use const-correctness
- [ ] Follow single responsibility principle
- [ ] Implement proper error handling and validation
- [ ] Add unit tests for new and existing code

## Files Requiring Code Quality Improvements

"""
        
        affected_files = {}
        for issue in style_issues:
            if issue['file'] not in affected_files:
                affected_files[issue['file']] = 0
            affected_files[issue['file']] += 1
        
        for file, count in sorted(affected_files.items(), key=lambda x: x[1], reverse=True):
            quality_plan_content += f"- {file}: {count} issues\n"
        
        quality_plan_content += """
## Implementation Strategy

### Phase 1: Critical Files (Week 1)
1. **Brain.hpp and Brain.cpp**: Core neural system
2. **Neuron.hpp and Neuron.cpp**: Fundamental neural units
3. **Synapse.hpp and Synapse.cpp**: Neural connections
4. **Config.hpp and Config.cpp**: Configuration management

### Phase 2: Supporting Files (Week 2-3)
1. **Memory systems**: Working memory, episodic memory
2. **Plasticity systems**: STDP, Hebbian learning
3. **Neuromodulation**: Dopamine, curiosity systems
4. **Cognition**: Attention, planning systems

### Phase 3: Integration and Testing (Week 4)
1. **System integration**: Phase 6 final integration
2. **Performance testing**: Profiling and optimization
3. **Documentation**: Complete API documentation
4. **Testing**: Comprehensive test suite

## Tools and Resources

### Automated Tools
- **Code formatters**: clang-format, astyle
- **Linters**: cppcheck, clang-tidy
- **Static analyzers**: clang-analyzer
- **Test frameworks**: Google Test, Catch2

### Documentation
- **API documentation**: Doxygen, Sphinx
- **Code documentation**: Inline comments, docstrings
- **Architecture documentation**: Architecture diagrams
- **User documentation**: User guides and tutorials

## Success Metrics

### Code Quality Metrics
1. **Cyclomatic Complexity**: Average < 10 per function
2. **Lines of Code**: Maintain reasonable LOC per module
3. **Documentation**: 100% of public APIs documented
4. **Test Coverage**: 80% minimum test coverage
5. **Code Standards**: 100% compliance with coding standards

### Maintainability Metrics
1. **Code Duplication**: Reduce by 30%
2. **Coupling**: Reduce inter-component dependencies
3. **Cohesion**: Improve within-component functionality
4. **Modularity**: Increase component independence
5. **Extensibility**: Make easier to add new features
"""
        
        quality_plan.write_text(quality_plan_content)
        print(f"Created code quality improvement plan: improvement_plans/code_quality_improvements.md")
    
    print(f"\n{'='*60}")
    print("ANALYSIS COMPLETE")
    print(f"{'='*60}")
    print(f"Analysis completed: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"\nImprovement plans created in: improvement_plans/")
    print(f"\nNext steps:")
    print(f"1. Review configuration fixes and implement them immediately")
    print(f"2. Address high-priority TODOs that affect core functionality")
    print(f"3. Implement code quality improvements for critical files")
    print(f"4. Run tests to ensure no regressions")
    print(f"5. Update documentation and add comments")
    
    return {
        'config_issues': len(config_issues),
        'todos': len(todos),
        'style_issues': len(style_issues),
        'plans_created': len([f for f in improvement_dir.glob('*') if f.is_file()])
    }

def main():
    """Main function."""
    return create_improvement_report()

if __name__ == "__main__":
    main()