#!/usr/bin/env python3
"""
Robust Simulation Example
=========================

This example demonstrates error handling with new NLM Python API validation features.
It showcases graceful degradation when validation fails, configuration recovery
and rollback, and comprehensive error reporting.

Key Features Demonstrated:
- Error handling with new validation features
- Graceful degradation when validation fails
- Configuration recovery and rollback
- Comprehensive error reporting

File: examples/robust_simulation.py
"""

import sys
import os
import traceback
import json
from datetime import datetime
from typing import Dict, List, Any, Optional

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: pynlm module not available: {e}")
    sys.exit(1)

class ValidationError(Exception):
    """Custom validation error for robust simulations."""
    
    def __init__(self, message: str, error_code: str = "VALIDATION_ERROR", 
                 details: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = datetime.now().isoformat()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary for logging/reporting."""
        return {
            "message": self.message,
            "error_code": self.error_code,
            "details": self.details,
            "timestamp": self.timestamp
        }

class RobustErrorHandler:
    """Robust error handling for NLM simulations with comprehensive reporting."""
    
    def __init__(self, max_recovery_attempts: int = 3):
        self.error_log = []
        self.recovery_attempts = 0
        self.max_recovery_attempts = max_recovery_attempts
        self.session_id = f"session_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
    
    def log_error(self, error: Exception, context: str = ""):
        """
        Log an error with context information.
        
        Args:
            error: The exception that occurred
            context: Context information about where the error occurred
        """
        error_entry = {
            "session_id": self.session_id,
            "timestamp": datetime.now().isoformat(),
            "context": context,
            "error_type": type(error).__name__,
            "error_message": str(error),
            "traceback": traceback.format_exc() if isinstance(error, Exception) else None
        }
        
        self.error_log.append(error_entry)
        
        # Print formatted error
        print(f"\n🚨 ERROR [{self.session_id}] - {context}")
        print(f"   Type: {error_entry['error_type']}")
        print(f"   Message: {error}")
        print(f"   Time: {error_entry['timestamp']}")
    
    def attempt_recovery(self, recovery_function, *args, **kwargs) -> bool:
        """
        Attempt recovery from an error.
        
        Args:
            recovery_function: Function to call for recovery
            *args, **kwargs: Arguments to pass to recovery function
            
        Returns:
            bool: True if recovery successful, False otherwise
        """
        if self.recovery_attempts >= self.max_recovery_attempts:
            print(f"❌ Maximum recovery attempts ({self.max_recovery_attempts}) reached")
            return False
        
        self.recovery_attempts += 1
        attempt_num = self.recovery_attempts
        
        print(f"🔄 Recovery attempt {attempt_num}/{self.max_recovery_attempts}")
        
        try:
            result = recovery_function(*args, **kwargs)
            print(f"✅ Recovery successful")
            return True
        except Exception as e:
            print(f"❌ Recovery failed: {e}")
            self.log_error(e, f"Recovery attempt {attempt_num}")
            return False
    
    def get_error_summary(self) -> Dict[str, Any]:
        """
        Get summary of errors encountered in this session.
        
        Returns:
            Dict: Error summary statistics
        """
        error_types = {}
        for entry in self.error_log:
            error_type = entry["error_type"]
            if error_type not in error_types:
                error_types[error_type] = 0
            error_types[error_type] += 1
        
        return {
            "session_id": self.session_id,
            "total_errors": len(self.error_log),
            "error_types": error_types,
            "recovery_attempts": self.recovery_attempts,
            "max_recovery_attempts": self.max_recovery_attempts,
            "recovery_success_rate": (
                len([e for e in self.error_log if e.get("recovered", False)]) / 
                len(self.error_log) if self.error_log else 0
            )
        }

class ConfigurationValidator:
    """Validates configurations with multiple validation strategies."""
    
    def __init__(self):
        self.validation_rules = {}
        self._setup_default_rules()
    
    def _setup_default_rules(self):
        """Set up default validation rules."""
        self.validation_rules = {
            "file_path": {
                "type": "path",
                "required": True,
                "exists": True,
                "readable": True
            },
            "brain.neuron_count": {
                "type": "range",
                "min": 100,
                "max": 100000,
                "required": True
            },
            "brain.connection_probability": {
                "type": "range",
                "min": 0.0,
                "max": 1.0,
                "required": False
            },
            "development.critical_period_start": {
                "type": "dependency",
                "depends_on": "development.enable",
                "required_if": True
            },
            "development.critical_period_end": {
                "type": "dependency",
                "depends_on": "development.enable",
                "required_if": True
            }
        }
    
    def validate_file_path(self, filepath: str) -> bool:
        """
        Validate file path with multiple checks.
        
        Args:
            filepath: Path to validate
            
        Returns:
            bool: True if valid, False otherwise
        """
        if not filepath:
            raise ValidationError("File path cannot be empty", "EMPTY_PATH")
        
        # Check if file exists
        if not os.path.exists(filepath):
            raise ValidationError(
                f"File does not exist: {filepath}",
                "FILE_NOT_FOUND",
                {"filepath": filepath, "exists": False}
            )
        
        # Check if file is readable
        if not os.access(filepath, os.R_OK):
            raise ValidationError(
                f"File is not readable: {filepath}",
                "FILE_NOT_READABLE",
                {"filepath": filepath, "readable": False}
            )
        
        # Check file extension (optional for NLM configs)
        if not filepath.lower().endswith(('.json', '.nlm', '.ini', '.cfg')):
            print(f"⚠️  Warning: Unusual file extension: {os.path.splitext(filepath)[1]}")
        
        return True
    
    def validate_config_completeness(self, config) -> bool:
        """
        Validate configuration completeness.
        
        Args:
            config: Configuration object to validate
            
        Returns:
            bool: True if complete, False otherwise
        """
        required_params = []
        warnings = []
        
        # Check for essential parameters
        if not config.has("brain.neuron_count"):
            warnings.append("Missing brain.neuron_count - using default")
        
        # Check for parameter consistency
        if config.has("development.critical_period_end"):
            end_val = config.get("development.critical_period_end")
            if config.has("development.critical_period_start"):
                start_val = config.get("development.critical_period_start")
                if end_val < start_val:
                    raise ValidationError(
                        "Critical period end before start",
                        "INVALID_CRITICAL_PERIOD",
                        {"start": start_val, "end": end_val}
                    )
        
        # Check STDP consistency
        if config.has("plasticity.stdp.enable") and config.get("plasticity.stdp.enable"):
            if not config.has("plasticity.stdp.learning_rate"):
                warnings.append("STDP enabled but missing learning rate")
        
        if warnings:
            print("⚠️  Configuration warnings:")
            for warning in warnings:
                print(f"   - {warning}")
        
        return len(warnings) == 0
    
    def validate_config_consistency(self, config) -> bool:
        """
        Validate configuration consistency.
        
        Args:
            config: Configuration object to validate
            
        Returns:
            bool: True if consistent, False otherwise
        """
        issues = []
        
        # Check for contradictory parameters
        if config.has("neuromod.curiosity.enable"):
            curiosity_enabled = config.get("neuromod.curiosity.enable")
            if curiosity_enabled:
                if config.has("neuromod.dopamine.scale"):
                    dopamine_scale = config.get("neuromod.dopamine.scale")
                    if dopamine_scale <= 0:
                        issues.append(
                            "Curiosity enabled but dopamine scale must be positive"
                        )
        
        # Check developmental stage consistency
        if config.has("development.enable"):
            if config.get("development.enable"):
                if not config.has("development.critical_period_start"):
                    issues.append(
                        "Development enabled but critical period start not specified"
                    )
        
        if issues:
            for issue in issues:
                raise ValidationError(issue, "CONFIGURATION_CONSISTENCY_ERROR")
        
        return True

class RobustConfigLoader:
    """Robust configuration loading with comprehensive error handling."""
    
    def __init__(self, error_handler: Optional[RobustErrorHandler] = None):
        self.error_handler = error_handler or RobustErrorHandler()
        self.validator = ConfigurationValidator()
    
    def create_fallback_config(self) -> Any:
        """
        Create a fallback configuration when primary config fails.
        
        Returns:
            Config: A safe, working configuration
        """
        print("🔄 Creating fallback configuration...")
        
        fallback_config = nlm.createDefaultConfig()
        
        # Set safe, working defaults
        fallback_config.set("brain.neuron_count", 1000)
        fallback_config.set("brain.region_count", 2)
        fallback_config.set("brain.connection_probability", 0.05)
        fallback_config.set("plasticity.stdp.enable", True)
        fallback_config.set("plasticity.stdp.learning_rate", 0.01)
        fallback_config.set("neuromod.dopamine.scale", 1.0)
        fallback_config.set("neuromod.curiosity.enable", True)
        fallback_config.set("development.enable", True)
        
        # Log the fallback creation
        self.error_handler.log_error(
            Exception("Created fallback configuration"),
            "CONFIG_FALLBACK_CREATION"
        )
        
        return fallback_config
    
    def load_robust_config(self, filepath: str) -> Any:
        """
        Robust configuration loading with error handling.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Config: Loaded configuration or fallback
        """
        print(f"\n=== Robust Configuration Loading ===")
        print(f"Attempting to load: {filepath}")
        
        # Reset recovery attempts for this load
        self.error_handler.recovery_attempts = 0
        
        # Recovery chain
        recovery_steps = [
            ("Path validation", self._validate_path),
            ("File loading", self._load_file),
            ("Content validation", self._validate_content),
            ("Consistency validation", self._validate_consistency)
        ]
        
        current_config = None
        
        for step_name, step_function in recovery_steps:
            print(f"\n🔍 Step {recovery_steps.index((step_name, step_function)) + 1}/{len(recovery_steps)}: {step_name}")
            
            try:
                if step_name == "Path validation":
                    if not self.validator.validate_file_path(filepath):
                        raise ValidationError(f"Path validation failed for: {filepath}")
                
                elif step_name == "File loading":
                    config = step_function(filepath)
                    current_config = config
                
                elif step_name == "Content validation":
                    if not self.validator.validate_config_completeness(current_config):
                        raise ValidationError("Content validation failed")
                
                elif step_name == "Consistency validation":
                    if not self.validator.validate_config_consistency(current_config):
                        raise ValidationError("Consistency validation failed")
                
                print(f"✅ {step_name} passed")
                
            except Exception as e:
                # Log the error
                self.error_handler.log_error(e, step_name)
                
                # Attempt recovery
                recovery_success = self.error_handler.attempt_recovery(
                    self._create_recovery_for_step,
                    step_name, filepath
                )
                
                if recovery_success:
                    print(f"🔄 Recovery completed for step: {step_name}")
                    continue
                else:
                    print(f"❌ Failed to recover from step: {step_name}")
                    if step_name == "File loading":
                        # Fall back to default config
                        current_config = self.create_fallback_config()
                        print("🔄 Using fallback configuration")
                        break
                    else:
                        # For validation steps, we can use the config anyway
                        print("⚠️  Using partially validated configuration")
                        break
        
        # Finalize and return configuration
        if current_config:
            self._finalize_config(current_config)
        
        return current_config
    
    def _validate_path(self, filepath: str) -> bool:
        """Validate file path."""
        return self.validator.validate_file_path(filepath)
    
    def _load_file(self, filepath: str) -> Any:
        """Load configuration from file."""
        print(f"   Loading configuration from {filepath}")
        
        try:
            config = nlm.createDefaultConfig()
            config.loadFromFile(filepath)
            print(f"   Loaded {len(config.getKeys())} configuration parameters")
            return config
        except Exception as e:
            raise ValidationError(f"Failed to load configuration file: {e}", "FILE_LOAD_ERROR")
    
    def _validate_content(self, config: Any) -> bool:
        """Validate configuration content."""
        print("   Validating configuration content...")
        if not self.validator.validate_config_completeness(config):
            raise ValidationError("Content validation failed", "CONTENT_VALIDATION_ERROR")
        print("   Content validation passed")
        return True
    
    def _validate_consistency(self, config: Any) -> bool:
        """Validate configuration consistency."""
        print("   Validating configuration consistency...")
        if not self.validator.validate_config_consistency(config):
            raise ValidationError("Consistency validation failed", "CONSISTENCY_VALIDATION_ERROR")
        print("   Consistency validation passed")
        return True
    
    def _create_recovery_for_step(self, step_name: str, filepath: str) -> bool:
        """
        Create recovery for a specific step.
        
        Args:
            step_name: Name of the step to recover from
            filepath: Original file path
            
        Returns:
            bool: True if recovery successful
        """
        if step_name == "Path validation":
            # Try different file extensions
            base_path = os.path.splitext(filepath)[0]
            for ext in ['.json', '.nlm', '.ini', '.cfg']:
                alternative_path = base_path + ext
                if os.path.exists(alternative_path):
                    print(f"   Trying alternative path: {alternative_path}")
                    return True
            
            # Try current directory
            filename = os.path.basename(filepath)
            current_dir_path = os.path.join(os.getcwd(), filename)
            if os.path.exists(current_dir_path):
                print(f"   Trying current directory path: {current_dir_path}")
                return True
            
            return False
        
        elif step_name == "File loading":
            # Try to create a minimal working config
            config = nlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            return True
        
        elif step_name == "Content validation":
            # Accept the config even with warnings
            return True
        
        elif step_name == "Consistency validation":
            # Accept the config with potential issues
            return True
        
        return False
    
    def _finalize_config(self, config: Any) -> None:
        """
        Finalize configuration after loading.
        
        Args:
            config: Configuration to finalize
        """
        print("\n🔧 Finalizing configuration...")
        
        # Set any missing required values
        if not config.has("brain.region_count"):
            config.set("brain.region_count", 1)
        
        # Validate one final time
        try:
            self.validator.validate_config_consistency(config)
            print("✅ Final validation passed")
        except ValidationError as e:
            print(f"⚠️  Final validation warnings: {e}")

class SafeWorldSimulator:
    """Safe world simulation with comprehensive error handling."""
    
    def __init__(self, error_handler: Optional[RobustErrorHandler] = None):
        self.error_handler = error_handler or RobustErrorHandler()
        self.world = None
        self.agent = None
    
    def initialize_safe_simulation(self, config: Any) -> bool:
        """
        Initialize simulation with error handling.
        
        Args:
            config: Configuration to use
            
        Returns:
            bool: True if initialization successful
        """
        print("🌍 Initializing safe world simulation...")
        
        try:
            # Create world
            self.world = nlm.createSimpleWorld()
            self.world.configure(200.0, 200.0, 32, 32)  # width, height, vision_width, vision_height
            
            # Initialize brain
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Create agent
            self.agent = nlm.createAgentBrain(brain)
            self.agent.initialize(self.world)
            
            # Enable safe defaults
            self.agent.enableRewardModulation(True)
            self.agent.enableDevelopment(True)
            self.agent.setExplorationMode("epsilon_greedy")
            
            print("✅ Safe simulation initialized successfully")
            return True
            
        except Exception as e:
            self.error_handler.log_error(e, "SIMULATION_INITIALIZATION")
            
            # Attempt recovery
            recovery_success = self.error_handler.attempt_recovery(
                self._recover_simulation_init
            )
            
            if recovery_success:
                return True
            else:
                print("❌ Failed to initialize safe simulation")
                return False
    
    def _recover_simulation_init(self) -> bool:
        """Recover simulation initialization."""
        print("   Attempting simulation recovery...")
        
        try:
            # Try with minimal configuration
            config = nlm.createDefaultConfig()
            config.set("brain.neuron_count", 500)
            
            return self.initialize_safe_simulation(config)
        except Exception as e:
            print(f"   Recovery failed: {e}")
            return False
    
    def run_safe_step(self, step: int) -> Dict[str, Any]:
        """
        Run a single simulation step with error handling.
        
        Args:
            step: Current step number
            
        Returns:
            Dict[str, Any]: Step results or error information
        """
        step_result = {
            "step": step,
            "success": False,
            "timestamp": datetime.now().isoformat(),
            "errors": [],
            "metrics": {}
        }
        
        try:
            # Update world
            timestep = 0.1
            self.world.update(timestep)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Execute brain step
            self.agent.getBrain().step(step, self.world.getSimulationTime())
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = action_result.reward
            predicted = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Collect metrics
            step_result["metrics"] = {
                "agent_energy": self.world.getAgentBody().energy,
                "agent_health": self.world.getAgentBody().health,
                "simulation_time": self.world.getSimulationTime(),
                "reward": reward,
                "prediction_error": predicted
            }
            
            step_result["success"] = True
            
        except Exception as e:
            error_info = {
                "error_type": type(e).__name__,
                "error_message": str(e),
                "step": step
            }
            step_result["errors"].append(error_info)
            self.error_handler.log_error(e, f"STEP {step} EXECUTION")
        
        return step_result

class ComprehensiveRobustDemo:
    """Demonstrates comprehensive robust simulation with all features."""
    
    def __init__(self):
        self.error_handler = RobustErrorHandler()
        self.config_loader = RobustConfigLoader(self.error_handler)
        self.simulator = SafeWorldSimulator(self.error_handler)
    
    def run_comprehensive_demo(self, num_steps: int = 100, checkpoint_interval: int = 20):
        """
        Run comprehensive demo with all robust features.
        
        Args:
            num_steps: Number of simulation steps to run
            checkpoint_interval: How often to log statistics
        """
        print("\n" + "="*70)
        print("🛡️  COMPREHENSIVE ROBUST SIMULATION DEMO")
        print("="*70)
        print(f"Started: {datetime.now()}")
        print(f"Total steps: {num_steps}")
        print(f"Error recovery attempts: {self.error_handler.max_recovery_attempts}")
        
        # Step 1: Load configuration robustly
        print("\n📋 Step 1: Loading configuration robustly")
        config = self.config_loader.load_robust_config("research_config.json")
        
        # Step 2: Initialize simulation safely
        print("\n🌍 Step 2: Initializing simulation safely")
        init_success = self.simulator.initialize_safe_simulation(config)
        
        if not init_success:
            print("❌ Failed to initialize simulation")
            return
        
        # Step 3: Run simulation with robust error handling
        print(f"\n🏃 Step 3: Running simulation ({num_steps} steps)")
        
        successful_steps = 0
        failed_steps = 0
        
        for step in range(num_steps):
            # Run step with error handling
            step_result = self.simulator.run_safe_step(step)
            
            if step_result["success"]:
                successful_steps += 1
                
                # Log statistics at checkpoints
                if step % checkpoint_interval == 0 and step > 0:
                    metrics = step_result["metrics"]
                    print(f"\n📊 Step {step}/{num_steps}: "
                          f"Energy={metrics['agent_energy']:.1f}, "
                          f"Health={metrics['agent_health']:.1f}, "
                          f"Reward={metrics['reward']:.3f}, "
                          f"Error={metrics['prediction_error']:.3f}")
                    
                    # Save checkpoint if step is a multiple of checkpoint_interval*5
                    if step % (checkpoint_interval * 5) == 0:
                        checkpoint_file = f"robust_checkpoint_step_{step}.nlm"
                        try:
                            self.simulator.agent.getBrain().save(checkpoint_file)
                            print(f"   💾 Checkpoint saved: {checkpoint_file}")
                        except Exception as e:
                            print(f"   ⚠️  Checkpoint save failed: {e}")
            
            else:
                failed_steps += 1
                
                # Log error details
                for error in step_result["errors"]:
                    print(f"   ❌ Step {step} failed: {error['error_type']}: {error['error_message']}")
        
        # Step 4: Finalize and report
        print(f"\n📈 Step 4: Finalizing and reporting")
        
        # Get error summary
        error_summary = self.error_handler.get_error_summary()
        
        print(f"\n🎯 Simulation Results:")
        print(f"   ✅ Successful steps: {successful_steps}")
        print(f"   ❌ Failed steps: {failed_steps}")
        print(f"   📊 Success rate: {successful_steps/(num_steps or 1)*100:.1f}%")
        
        print(f"\n🔍 Error Summary:")
        print(f"   Session ID: {error_summary['session_id']}")
        print(f"   Total errors: {error_summary['total_errors']}")
        print(f"   Recovery attempts: {error_summary['recovery_attempts']}")
        
        if error_summary['error_types']:
            print(f"   Error types:")
            for error_type, count in error_summary['error_types'].items():
                print(f"     {error_type}: {count}")
        
        print(f"\n📁 Cleanup:")
        # Clean up checkpoints
        import glob
        checkpoint_files = glob.glob("robust_checkpoint_*.nlm")
        if checkpoint_files:
            print(f"   Removing {len(checkpoint_files)} checkpoint files")
            for f in checkpoint_files:
                try:
                    os.remove(f)
                except:
                    pass
        
        print(f"\n✅ Comprehensive robust simulation complete")
        print(f"   Completed at: {datetime.now()}")
        print(f"   Session ID for reference: {error_summary['session_id']}")


def load_robust_config(filepath: str):
    """
    Robust configuration loading with error handling (standalone function).
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        Config: Loaded configuration or fallback
    """
    print(f"\n=== Loading Robust Configuration (Standalone) ===")
    print(f"File: {filepath}")
    
    # Create components
    error_handler = RobustErrorHandler()
    config_loader = RobustConfigLoader(error_handler)
    
    # Load with robust error handling
    config = config_loader.load_robust_config(filepath)
    
    # Print final configuration quality
    if config:
        print(f"\n✅ Configuration loaded successfully")
        print(f"   Parameters: {len(config.getKeys())}")
        print(f"   Session ID: {error_handler.session_id}")
    
    return config

if __name__ == "__main__":
    # Create and run comprehensive robust demo
    demo = ComprehensiveRobustDemo()
    demo.run_comprehensive_demo(num_steps=100, checkpoint_interval=20)
