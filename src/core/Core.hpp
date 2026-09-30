#pragma once

// Core namespace hierarchy to reduce nlm namespace pollution
namespace nlm {

namespace Core {
    namespace Utilities {
        // Math utilities
        class MathUtils;
        
        // Validation utilities  
        class ValidationUtils;
        
        // String utilities
        class StringUtils;
        
        // Container utilities
        class ContainerUtils;
        
        // Brain-specific utilities
        class BrainUtils;
        
        // Pattern utilities
        class Patterns;
        
        // Common utility functions and classes
        class Common;
    }
    
    namespace Config {
        class Config;
        class ConfigSchema;
        class ConfigEntry;
    }
    
    namespace Random {
        class RandomGenerator;
    }
    
    namespace Logger {
        class Logger;
    }
    
    namespace SimulationClock {
        class SimulationClock;
    }
    
    namespace Types {
        // Type definitions will be here
    }
}

namespace Agent {
    // Agent-specific modules
}

namespace World {
    // World-specific modules
}

namespace Neuromodulation {
    // Neuromodulation modules
}

namespace Development {
    // Development modules
}

namespace Dynamics {
    // Dynamics modules
}

namespace Plasticity {
    // Plasticity modules
}

namespace Memory {
    // Memory modules
}

namespace Cognition {
    // Cognition modules
}

namespace Sensory {
    // Sensory modules
}

namespace Motor {
    // Motor modules
}

namespace Prediction {
    // Prediction modules
}

namespace Performance {
    // Performance modules
}

namespace Environment {
    // Environment modules
}

namespace Experiments {
    // Experiment modules
}

namespace Visualization {
    // Visualization modules
}

namespace Common {
    // Common utilities that can be used across all modules
}

} // namespace nlm