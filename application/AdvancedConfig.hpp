{
  "name": "NLM Advanced Configuration Features",
  "version": "2.0.0",
  "description": "Advanced configuration features for NLM with inheritance, namespaces, and sophisticated validation",
  "advancedFeatures": {
    "inheritance": {
      "enabled": true,
      "baseConfigurations": ["configs/default.cfg"],
      "inheritanceMode": "additive",
      "overrideAllowed": true,
      "circularDetection": true
    },
    "namespaces": {
      "enabled": true,
      "defaultNamespace": "global",
      "allowDotNotation": true,
      "namespaceSeparator": ".",
      "namespacePrefix": "",
      "namespaceInheritValues": true
    },
    "variableSubstitution": {
      "enabled": true,
      "syntax": "${parameter_name}",
      "maxDepth": 10,
      "circularDetection": true,
      "caseSensitive": false,
      "defaultValueFallback": true
    },
    "environmentVariables": {
      "enabled": true,
      "syntax": "$VAR or ${VAR}",
      "prefix": "NLM_",
      "caseSensitive": true,
      "required": false,
      "fallbackToConfig": true
    },
    "parameterGroups": {
      "enabled": true,
      "autoGrouping": true,
      "groupValidation": true,
      "groupInheritValues": true,
      "groups": {
        "simulation": {
          "parameters": ["random_seed", "simulation_timestep", "max_simulation_steps", "simulation_time_limit"],
          "description": "Core simulation parameters",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "neurons": {
          "parameters": ["neuron_count", "region_count", "connection_probability"],
          "description": "Neural network architecture parameters",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "plasticity": {
          "parameters": ["plasticity_learning_rate", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_time_constant"],
          "description": "Synaptic plasticity mechanisms",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "development": {
          "parameters": ["development_synaptogenesis_rate", "development_pruning_rate"],
          "description": "Neural development processes",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "neuromodulation": {
          "parameters": ["dopamine_baseline", "reward_discount_factor"],
          "description": "Neuromodulation and reward systems",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "environment": {
          "parameters": ["environment_name", "environment_width", "environment_height"],
          "description": "Simulation environment settings",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "logging": {
          "parameters": ["log_level", "log_to_file", "log_filename"],
          "description": "Logging and output settings",
          "validation": {"minParameters": 1, "maxParameters": 10}
        },
        "visualization": {
          "parameters": ["visualization_enabled", "visualization_update_rate"],
          "description": "Visualization and display settings",
          "validation": {"minParameters": 1, "maxParameters": 10}
        }
      }
    },
    "validation": {
      "enabled": true,
      "strictMode": false,
      "schemaValidation": true,
      "typeChecking": true,
      "rangeValidation": true,
      "patternValidation": true,
      "enumValidation": true,
      "requiredValidation": true,
      "crossValidation": true,
      "dependencyValidation": true
    },
    "serialization": {
      "formats": ["json", "yaml", "cfg"],
      "defaultFormat": "json",
      "prettyPrint": true,
      "includeSource": true,
      "includeMetadata": true,
      "compressOutput": false,
      "maxStringLength": 1000
    },
    "performance": {
      "caching": {
        "enabled": true,
        "valueCaching": true,
        "validationCaching": true,
        "substitutionCaching": true,
        "envVarCaching": true,
        "cacheSize": 10000,
        "maxMemoryUsage": 100 * 1024 * 1024
      },
      "threadSafety": {
        "enabled": true,
        "mutexType": "recursive_mutex",
        "lockTimeout": 1000,
        "maxConcurrentAccess": 100
      },
      "loading": {
        "lazyLoading": true,
        "batchLoading": true,
        "preloadSchemas": true,
        "compileSchemas": true
      }
    },
    "debugging": {
      "enabled": true,
      "debugLevel": "detailed",
      "enableDebugger": true,
      "enableDiff": true,
      "enableSnapshots": true,
      "enableValidationReport": true,
      "enablePerformanceMetrics": true,
      "logLevel": "DEBUG"
    }
  }
}
