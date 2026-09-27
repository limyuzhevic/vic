{
  "name": "NLM Application Configuration Manager",
  "version": "2.0.0",
  "description": "Application-level configuration management for NLM with enhanced features",
  "configuration": {
    "global": {
      "autoSave": true,
      "backupCount": 5,
      "serializationFormat": "json",
      "validationOnLoad": true,
      "strictMode": false
    },
    "performance": {
      "enableCaching": true,
      "cacheSize": 1000,
      "threadPoolSize": 4,
      "lazyLoading": true,
      "valueCaching": true
    },
    "monitoring": {
      "enableMetrics": true,
      "metricsInterval": 1000,
      "logChanges": true,
      "trackUsage": true,
      "debugMode": false
    },
    "security": {
      "enableValidation": true,
      "maxStringLength": 255,
      "allowUnknownParameters": false,
      "sanitizeInput": true
    },
    "features": {
      "enableVariableSubstitution": true,
      "enableEnvironmentVariables": true,
      "enableInheritance": true,
      "enableNamespaces": true,
      "enableParameterGroups": true
    }
  },
  "development": {
    "debugTools": {
      "enableDebugger": true,
      "enableDiff": true,
      "enableSnapshots": true,
      "enableValidationReport": true
    },
    "validation": {
      "strictTypeChecking": true,
      "rangeChecking": true,
      "patternValidation": true,
      "enumValidation": true
    }
  }
}
