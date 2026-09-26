#!/usr/bin/env python3
# NLM CLI Debug Module
# 
# Debugging and analysis tools for NLM advanced CLI

import os
import json
import re
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
import traceback

class NLMDebugger:
    def __init__(self):
        self.debug_sessions = {}
        self.current_session = None
        self.debug_level = "info"  # debug, info, warning, error
    
    def create_debug_session(self, session_name: str) -> bool:
        """Create a new debug session"""
        print(f"Creating debug session: {session_name}")
        
        if session_name in self.debug_sessions:
            print(f"Debug session already exists: {session_name}")
            return False
        
        self.debug_sessions[session_name] = {
            'name': session_name,
            'start_time': time.time(),
            'logs': [],
            'variables': {},
            'breakpoints': [],
            'call_stack': []
        }
        
        self.current_session = session_name
        print(f"Debug session created: {session_name}")
        return True
    
    def add_log(self, session_name: str, level: str, message: str, 
                details: Dict[str, Any] = None, traceback_str: str = None):
        """Add a log entry to a debug session"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return False
        
        if self.debug_level not in ['debug', 'info'] and level == 'debug':
            return False
        
        log_entry = {
            'timestamp': time.time(),
            'level': level,
            'message': message,
            'details': details or {},
            'traceback': traceback_str
        }
        
        self.debug_sessions[session_name]['logs'].append(log_entry)
        
        # Print to console if appropriate level
        if level in ['debug', 'info'] or self.debug_level == 'debug':
            prefix = f"[{level.upper()}]"
            print(f"{prefix} {session_name}: {message}")
        
        return True
    
    def set_variable(self, session_name: str, var_name: str, value: Any):
        """Set a variable in a debug session"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return False
        
        self.debug_sessions[session_name]['variables'][var_name] = {
            'value': value,
            'timestamp': time.time(),
            'type': type(value).__name__
        }
        
        print(f"Variable set: {session_name}.{var_name} = {value}")
        return True
    
    def get_variable(self, session_name: str, var_name: str) -> Any:
        """Get a variable from a debug session"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return None
        
        variables = self.debug_sessions[session_name]['variables']
        if var_name in variables:
            return variables[var_name]['value']
        else:
            print(f"Variable not found: {var_name} in session {session_name}")
            return None
    
    def add_breakpoint(self, session_name: str, condition: str, 
                      action: str = "pause", details: Dict[str, Any] = None):
        """Add a breakpoint to a debug session"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return False
        
        breakpoint = {
            'id': len(self.debug_sessions[session_name]['breakpoints']) + 1,
            'condition': condition,
            'action': action,
            'details': details or {},
            'triggered': False,
            'last_triggered': None
        }
        
        self.debug_sessions[session_name]['breakpoints'].append(breakpoint)
        print(f"Breakpoint added: {condition} -> {action}")
        return True
    
    def evaluate_condition(self, session_name: str, condition: str) -> bool:
        """Evaluate a condition for breakpoint triggering"""
        try:
            # Simple condition evaluation (in real implementation, would be more sophisticated)
            if '=' in condition:
                var_name, value = condition.split('=', 1)
                var_name = var_name.strip()
                value = value.strip()
                
                actual_value = self.get_variable(session_name, var_name)
                if actual_value is None:
                    return False
                
                # Try to evaluate as numbers if possible
                try:
                    actual_float = float(actual_value)
                    condition_float = float(value)
                    return actual_float == condition_float
                except (ValueError, TypeError):
                    return str(actual_value) == value
            
            elif '>' in condition:
                var_name, value = condition.split('>', 1)
                var_name = var_name.strip()
                value = value.strip()
                
                actual_value = self.get_variable(session_name, var_name)
                if actual_value is None:
                    return False
                
                try:
                    actual_float = float(actual_value)
                    condition_float = float(value)
                    return actual_float > condition_float
                except (ValueError, TypeError):
                    return False
            
            elif '<' in condition:
                var_name, value = condition.split('<', 1)
                var_name = var_name.strip()
                value = value.strip()
                
                actual_value = self.get_variable(session_name, var_name)
                if actual_value is None:
                    return False
                
                try:
                    actual_float = float(actual_value)
                    condition_float = float(value)
                    return actual_float < condition_float
                except (ValueError, TypeError):
                    return False
            
            else:
                # Simple variable existence check
                return self.get_variable(session_name, condition) is not None
                
        except Exception as e:
            print(f"Error evaluating condition '{condition}': {e}")
            return False
    
    def get_session_info(self, session_name: str) -> Dict[str, Any]:
        """Get information about a debug session"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return {}
        
        return self.debug_sessions[session_name].copy()
    
    def get_all_sessions(self) -> List[str]:
        """Get all debug session names"""
        return list(self.debug_sessions.keys())
    
    def clear_session(self, session_name: str) -> bool:
        """Clear a debug session"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return False
        
        self.debug_sessions[session_name] = {
            'name': session_name,
            'start_time': time.time(),
            'logs': [],
            'variables': {},
            'breakpoints': [],
            'call_stack': []
        }
        
        print(f"Debug session cleared: {session_name}")
        return True
    
    def export_session(self, session_name: str, filepath: str) -> bool:
        """Export debug session to file"""
        if session_name not in self.debug_sessions:
            print(f"Debug session not found: {session_name}")
            return False
        
        try:
            with open(filepath, 'w') as f:
                json.dump(self.debug_sessions[session_name], f, indent=2)
            
            print(f"Debug session exported: {session_name} -> {filepath}")
            return True
            
        except Exception as e:
            print(f"Error exporting debug session: {e}")
            return False
    
    def import_session(self, filepath: str) -> bool:
        """Import debug session from file"""
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return False
        
        try:
            with open(filepath, 'r') as f:
                session_data = json.load(f)
            
            session_name = session_data['name']
            if session_name in self.debug_sessions:
                print(f"Debug session already exists: {session_name}")
                return False
            
            self.debug_sessions[session_name] = session_data
            self.current_session = session_name
            
            print(f"Debug session imported: {session_name}")
            return True
            
        except Exception as e:
            print(f"Error importing debug session: {e}")
            return False

class NLMErrorAnalyzer:
    def __init__(self):
        self.error_patterns = []
        self.error_rules = []
    
    def add_error_pattern(self, pattern: str, handler: Callable):
        """Add an error pattern and handler"""
        self.error_patterns.append({'pattern': pattern, 'handler': handler})
    
    def analyze_error(self, error_message: str, context: Dict[str, Any] = None) -> Dict[str, Any]:
        """Analyze an error message and provide suggestions"""
        analysis = {
            'error_message': error_message,
            'context': context or {},
            'detected_patterns': [],
            'suggestions': [],
            'severity': 'low'
        }
        
        # Match error patterns
        for pattern_info in self.error_patterns:
            if re.search(pattern_info['pattern'], error_message, re.IGNORECASE):
                analysis['detected_patterns'].append(pattern_info['pattern'])
                try:
                    suggestion = pattern_info['handler'](error_message, context)
                    analysis['suggestions'].append(suggestion)
                except Exception as e:
                    analysis['suggestions'].append(f"Handler error: {e}")
        
        # Determine severity based on error message
        error_lower = error_message.lower()
        if any(word in error_lower for word in ['fatal', 'critical', 'segfault', 'exception']):
            analysis['severity'] = 'high'
        elif any(word in error_lower for word in ['error', 'fail', 'timeout']):
            analysis['severity'] = 'medium'
        
        return analysis
    
    def get_common_solutions(self, error_type: str) -> List[str]:
        """Get common solutions for specific error type"""
        # Common error solutions
        solutions = {
            'memory_error': [
                'Check for memory leaks in simulation code',
                'Reduce neuron count or simulation duration',
                'Implement checkpointing and save intermediate states',
                'Monitor memory usage during simulation'
            ],
            'performance_error': [
                'Optimize simulation parameters',
                'Consider using smaller networks for testing',
                'Enable performance monitoring',
                'Check for bottlenecks in neural dynamics'
            ],
            'plasticity_error': [
                'Check STDP and Hebbian parameters',
                'Verify synaptic connectivity',
                'Ensure plasticity rules are properly enabled',
                'Monitor spike timing and plasticity interactions'
            ],
            'memory_error': [
                'Check working memory capacity settings',
                'Verify episodic memory configuration',
                'Ensure memory consolidation is enabled',
                'Monitor memory usage and decay rates'
            ]
        }
        
        return solutions.get(error_type, ['No specific solutions available'])

class NLMCodeInspector:
    def __init__(self):
        self.inspection_rules = []
    
    def add_inspection_rule(self, name: str, check_func: Callable, severity: str = 'warning'):
        """Add a code inspection rule"""
        self.inspection_rules.append({
            'name': name,
            'check_func': check_func,
            'severity': severity
        })
    
    def inspect_code(self, code: str, filepath: str = None) -> List[Dict[str, Any]]:
        """Inspect code and return issues found"""
        issues = []
        
        for rule in self.inspection_rules:
            try:
                issue = rule['check_func'](code, filepath)
                if issue:
                    issues.append({
                        'rule': rule['name'],
                        'severity': rule['severity'],
                        'message': issue['message'],
                        'line': issue.get('line', 0),
                        'suggestion': issue.get('suggestion', '')
                    })
            except Exception as e:
                issues.append({
                    'rule': rule['name'],
                    'severity': 'error',
                    'message': f"Inspection rule error: {e}",
                    'line': 0,
                    'suggestion': 'Fix inspection rule'
                })
        
        return issues
    
    def check_for_placeholder_code(self, code: str, filepath: str = None) -> Dict[str, Any]:
        """Check for placeholder TODO or PLACEHOLDER comments"""
        issues = []
        
        # Check for TODO PHASE 2
        todo_lines = []
        for i, line in enumerate(code.split('\n'), 1):
            if 'TODO PHASE 2' in line:
                todo_lines.append((i, line.strip()))
        
        if todo_lines:
            issues.append({
                'rule': 'Placeholder Code',
                'severity': 'warning',
                'message': f"Found {len(todo_lines)} TODO PHASE 2 comments",
                'line': todo_lines[0][0],
                'suggestion': 'Implement placeholder functionality or remove comments'
            })
        
        # Check for PLACEHOLDER comments
        placeholder_lines = []
        for i, line in enumerate(code.split('\n'), 1):
            if 'PLACEHOLDER' in line:
                placeholder_lines.append((i, line.strip()))
        
        if placeholder_lines:
            issues.append({
                'rule': 'Placeholder Comments',
                'severity': 'info',
                'message': f"Found {len(placeholder_lines)} PLACEHOLDER comments",
                'line': placeholder_lines[0][0],
                'suggestion': 'Replace placeholder with actual implementation or mark as documentation'
            })
        
        return {
            'total_issues': len(todo_lines) + len(placeholder_lines),
            'todo_issues': len(todo_lines),
            'placeholder_issues': len(placeholder_lines),
            'details': issues
        }
    
    def check_neural_dynamics_completeness(self, code: str) -> Dict[str, Any]:
        """Check if neural dynamics implementation is complete"""
        required_methods = ['stepLIF', 'updateNeuron', 'updateSynapse', 'applySpikeInput']
        found_methods = 0
        
        for method in required_methods:
            if method in code:
                found_methods += 1
        
        completeness_score = found_methods / len(required_methods)
        
        issues = []
        if completeness_score < 0.5:
            issues.append({
                'rule': 'Neural Dynamics Completeness',
                'severity': 'error',
                'message': f'Neural dynamics implementation incomplete: {found_methods}/{len(required_methods)} required methods found',
                'line': 0,
                'suggestion': 'Implement missing LIF dynamics methods'
            })
        
        return {
            'completeness_score': completeness_score,
            'missing_methods': len(required_methods) - found_methods,
            'details': issues
        }

# Global debug tools instances
_debug_tools = None
_error_analyzer = None
_code_inspector = None

def get_debug_tools() -> NLMDebugger:
    """Get global debug tools instance"""
    global _debug_tools
    if _debug_tools is None:
        _debug_tools = NLMDebugger()
    return _debug_tools

def get_error_analyzer() -> NLMErrorAnalyzer:
    """Get global error analyzer instance"""
    global _error_analyzer
    if _error_analyzer is None:
        _error_analyzer = NLMErrorAnalyzer()
    return _error_analyzer

def get_code_inspector() -> NLMCodeInspector:
    """Get global code inspector instance"""
    global _code_inspector
    if _code_inspector is None:
        _code_inspector = NLMCodeInspector()
    return _code_inspector

# Convenience functions

def create_debug_session(session_name: str) -> bool:
    """Convenience function to create debug session"""
    return get_debug_tools().create_debug_session(session_name)

def add_debug_log(session_name: str, level: str, message: str, details=None):
    """Convenience function to add debug log"""
    return get_debug_tools().add_log(session_name, level, message, details)

def set_debug_variable(session_name: str, var_name: str, value):
    """Convenience function to set debug variable"""
    return get_debug_tools().set_variable(session_name, var_name, value)

def analyze_error(error_message: str, context: dict = None):
    """Convenience function to analyze error"""
    return get_error_analyzer().analyze_error(error_message, context)

def inspect_code(code: str, filepath: str = None):
    """Convenience function to inspect code"""
    return get_code_inspector().inspect_code(code, filepath)

if __name__ == "__main__":
    # Test the debug tools
    print("Testing NLM Debug Tools")
    
    # Create debug tools
    debugger = NLMDebugger()
    
    # Test debug session creation
    print("\n--- Testing Debug Session ---")
    debugger.create_debug_session("test_session")
    debugger.add_debug_log("test_session", "info", "Starting debug session")
    debugger.add_debug_log("test_session", "debug", "Debugging step 1", {"step": 1})
    debugger.set_debug_variable("test_session", "neuron_count", 1000)
    debugger.set_debug_variable("test_session", "connection_probability", 0.15)
    
    # Test breakpoint
    debugger.add_breakpoint("test_session", "neuron_count > 500", "pause")
    
    # Test error analyzer
    print("\n--- Testing Error Analyzer ---")
    error_analyzer = NLMErrorAnalyzer()
    
    # Add error patterns
    error_analyzer.add_error_pattern(
        r"memory.*error|out of memory|memory allocation",
        lambda msg, ctx: "Check memory usage and consider reducing simulation size"
    )
    
    error_analyzer.add_error_pattern(
        r"performance.*slow|too.*slow|high.*latency",
        lambda msg, ctx: "Optimize simulation parameters or check system resources"
    )
    
    # Analyze errors
    error_analysis1 = error_analyzer.analyze_error("Memory allocation failed")
    print(f"Error analysis 1: {error_analysis1}")
    
    error_analysis2 = error_analyzer.analyze_error("Performance too slow")
    print(f"Error analysis 2: {error_analysis2}")
    
    # Test code inspector
    print("\n--- Testing Code Inspector ---")
    code_inspector = NLMCodeInspector()
    
    # Add inspection rules
    def check_for_todo(code, filepath):
        if "TODO" in code:
            return {'message': "TODO comments found", 'suggestion': "Consider implementing TODOs"}
        return None
    
    code_inspector.add_inspection_rule("Check for TODOs", check_for_todo)
    
    # Test with sample code containing TODO
    sample_code = '''
void Neuron::stepLIF() {
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    // PLACEHOLDER: Simple leaky integrator
}
'''
    
    inspection_results = code_inspector.inspect_code(sample_code)
    placeholder_check = code_inspector.check_for_placeholder_code(sample_code)
    neural_check = code_inspector.check_neural_dynamics_completeness(sample_code)
    
    print(f"Inspection results: {inspection_results}")
    print(f"Placeholder check: {placeholder_check}")
    print(f"Neural dynamics check: {neural_check}")
    
    print("\nDebug tools test completed")
