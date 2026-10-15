# NLM Architecture Definition Guide

## Overview
NLM's architecture definition guide provides comprehensive instructions for defining neural network architectures in NLM. This guide covers the architecture definition process, API documentation, examples, and best practices.

## Architecture Definition Process

### 1. Architecture Definition Types

NLM supports several architecture definition types:

#### Neural Network Architecture
Defines the structure and connectivity of neural networks.

#### Region Architecture
Defines the organization of brain regions and their interactions.

#### Layer Architecture
Defines the layered structure of neural networks.

#### Modular Architecture
Defines modular components and their interconnections.

### 2. Architecture Definition Workflow

#### Step 1: Understand Architecture Definition
- Review architecture definition interfaces and APIs
- Understand architecture definition and management
- Learn about architecture configuration

#### Step 2: Choose Architecture Type
- Select the appropriate architecture type for your needs
- Define architecture requirements and specifications
- Design architecture structure

#### Step 3: Define Architecture
- Define architecture using NLM's architecture definition language
- Add architecture components and properties
- Configure architecture parameters

#### Step 4: Validate Architecture
- Validate architecture using NLM's validation tools
- Check for errors and inconsistencies
- Optimize architecture performance

#### Step 5: Save Architecture
- Save architecture to file
- Export architecture for distribution
- Import architecture from file

### 3. Architecture Definition Examples

### 1. Basic Neural Network Architecture
```ini
# Basic neural network architecture
[architecture]
name = "BasicNeuralNetwork"
type = "neural_network"

[architecture.parameters]
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001

[architecture.layers]
input_layer_neurons = 100
hidden_layer_neurons = 500
output_layer_neurons = 100

[architecture.connections]
input_to_hidden = 0.15
hidden_to_output = 0.15
```

### 2. Brain Region Architecture
```ini
# Brain region architecture
[architecture]
name = "BrainRegionArchitecture"
type = "region"

[architecture.parameters]
region_count = 3
region_size = 333
connection_probability = 0.1

[architecture.regions]
region1_type = "sensory"
region2_type = "cognition"
region3_type = "motor"

[architecture.connections]
region1_to_region2 = 0.1
region2_to_region3 = 0.1
region3_to_region1 = 0.05
```

### 3. Layered Architecture
```ini
# Layered architecture
[architecture]
name = "LayeredArchitecture"
type = "layer"

[architecture.parameters]
layer_count = 4
neurons_per_layer = 250
connection_probability = 0.2

[architecture.layers]
layer1_type = "input"
layer2_type = "hidden"
layer3_type = "hidden"
layer4_type = "output"

[architecture.connections]
layer1_to_layer2 = 0.2
layer2_to_layer3 = 0.2
layer3_to_layer4 = 0.2
```

### 4. Modular Architecture
```ini
# Modular architecture
[architecture]
name = "ModularArchitecture"
type = "modular"

[architecture.modules]
module1_type = "neuron_model"
module2_type = "plasticity_rule"
module3_type = "memory_system"
module4_type = "prediction_system"

[architecture.connections]
module1_to_module2 = 0.1
module2_to_module3 = 0.1
module3_to_module4 = 0.1
```

## Architecture Definition API

### Architecture Definition Interface

#### Architecture Definition Class
```cpp
class ArchitectureDefinition {
public:
    ArchitectureDefinition();
    ~ArchitectureDefinition();
    
    // Architecture name
    std::string getName() const;
    void setName(const std::string& name);
    
    // Architecture type
    std::string getType() const;
    void setType(const std::string& type);
    
    // Architecture parameters
    std::map<std::string, std::string> getParameters() const;
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
    
    // Architecture components
    std::vector<Layer> getLayers() const;
    std::vector<Region> getRegions() const;
    std::vector<Module> getModules() const;
    
    // Architecture connections
    std::vector<Connection> getConnections() const;
    
    // Architecture validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Architecture saving
    bool saveToFile(const std::string& filename) const;
    bool saveToYAML(const std::string& filename) const;
    bool saveToJSON(const std::string& filename) const;
    
    // Architecture loading
    static ArchitectureDefinition loadFromFile(const std::string& filename);
    static ArchitectureDefinition loadFromYAML(const std::string& filename);
    static ArchitectureDefinition loadFromJSON(const std::string& filename);
};
```

#### Layer Class
```cpp
class Layer {
public:
    Layer();
    ~Layer();
    
    // Layer properties
    std::string getName() const;
    void setName(const std::string& name);
    
    std::string getType() const;
    void setType(const std::string& type);
    
    int getNeuronCount() const;
    void setNeuronCount(int count);
    
    // Layer parameters
    std::map<std::string, std::string> getParameters() const;
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
};
```

#### Region Class
```cpp
class Region {
public:
    Region();
    ~Region();
    
    // Region properties
    std::string getName() const;
    void setName(const std::string& name);
    
    std::string getType() const;
    void setType(const std::string& type);
    
    int getSize() const;
    void setSize(int size);
    
    // Region parameters
    std::map<std::string, std::string> getParameters() const;
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
};
```

#### Module Class
```cpp
class Module {
public:
    Module();
    ~Module();
    
    // Module properties
    std::string getName() const;
    void setName(const std::string& name);
    
    std::string getType() const;
    void setType(const std::string& type);
    
    std::string getPluginName() const;
    void setPluginName(const std::string& pluginName);
    
    // Module parameters
    std::map<std::string, std::string> getParameters() const;
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
};
```

#### Connection Class
```cpp
class Connection {
public:
    Connection();
    ~Connection();
    
    // Connection properties
    std::string getSource() const;
    void setSource(const std::string& source);
    
    std::string getTarget() const;
    void setTarget(const std::string& target);
    
    float getStrength() const;
    void setStrength(float strength);
    
    // Connection parameters
    std::map<std::string, std::string> getParameters() const;
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
};
```

## Architecture Definition Examples

### 1. Basic Neural Network Architecture
```cpp
// Basic neural network architecture definition
#include "ArchitectureDefinition.hpp"

ArchitectureDefinition createBasicNeuralNetworkArchitecture() {
    ArchitectureDefinition architecture;
    
    // Set architecture name and type
    architecture.setName("BasicNeuralNetwork");
    architecture.setType("neural_network");
    
    // Set architecture parameters
    architecture.setParameter("neuron_count", "1000");
    architecture.setParameter("region_count", "2");
    architecture.setParameter("connection_probability", "0.15");
    architecture.setParameter("simulation_timestep", "0.001");
    
    // Define layers
    Layer inputLayer;
    inputLayer.setName("InputLayer");
    inputLayer.setType("input");
    inputLayer.setNeuronCount(100);
    architecture.addLayer(inputLayer);
    
    Layer hiddenLayer;
    hiddenLayer.setName("HiddenLayer");
    hiddenLayer.setType("hidden");
    hiddenLayer.setNeuronCount(500);
    architecture.addLayer(hiddenLayer);
    
    Layer outputLayer;
    outputLayer.setName("OutputLayer");
    outputLayer.setType("output");
    outputLayer.setNeuronCount(100);
    architecture.addLayer(outputLayer);
    
    // Define connections
    Connection inputToHidden;
    inputToHidden.setSource("InputLayer");
    inputToHidden.setTarget("HiddenLayer");
    inputToHidden.setStrength(0.15);
    architecture.addConnection(inputToHidden);
    
    Connection hiddenToOutput;
    hiddenToOutput.setSource("HiddenLayer");
    hiddenToOutput.setTarget("OutputLayer");
    hiddenToOutput.setStrength(0.15);
    architecture.addConnection(hiddenToOutput);
    
    return architecture;
}
```

### 2. Brain Region Architecture
```cpp
// Brain region architecture definition
#include "ArchitectureDefinition.hpp"

ArchitectureDefinition createBrainRegionArchitecture() {
    ArchitectureDefinition architecture;
    
    // Set architecture name and type
    architecture.setName("BrainRegionArchitecture");
    architecture.setType("region");
    
    // Set architecture parameters
    architecture.setParameter("region_count", "3");
    architecture.setParameter("region_size", "333");
    architecture.setParameter("connection_probability", "0.1");
    
    // Define regions
    Region sensoryRegion;
    sensoryRegion.setName("SensoryRegion");
    sensoryRegion.setType("sensory");
    sensoryRegion.setSize(333);
    architecture.addRegion(sensoryRegion);
    
    Region cognitiveRegion;
    cognitiveRegion.setName("CognitiveRegion");
    cognitiveRegion.setType("cognition");
    cognitiveRegion.setSize(333);
    architecture.addRegion(cognitiveRegion);
    
    Region motorRegion;
    motorRegion.setName("MotorRegion");
    motorRegion.setType("motor");
    motorRegion.setSize(334);
    architecture.addRegion(motorRegion);
    
    // Define connections
    Connection sensoryToCognitive;
    sensoryToCognitive.setSource("SensoryRegion");
    sensoryToCognitive.setTarget("CognitiveRegion");
    sensoryToCognitive.setStrength(0.1);
    architecture.addConnection(sensoryToCognitive);
    
    Connection cognitiveToMotor;
    cognitiveToMotor.setSource("CognitiveRegion");
    cognitiveToMotor.setTarget("MotorRegion");
    cognitiveToMotor.setStrength(0.1);
    architecture.addConnection(cognitiveToMotor);
    
    Connection motorToSensory;
    motorToSensory.setSource("MotorRegion");
    motorToSensory.setTarget("SensoryRegion");
    motorToSensory.setStrength(0.05);
    architecture.addConnection(motorToSensory);
    
    return architecture;
}
```

### 3. Layered Architecture
```cpp
// Layered architecture definition
#include "ArchitectureDefinition.hpp"

ArchitectureDefinition createLayeredArchitecture() {
    ArchitectureDefinition architecture;
    
    // Set architecture name and type
    architecture.setName("LayeredArchitecture");
    architecture.setType("layer");
    
    // Set architecture parameters
    architecture.setParameter("layer_count", "4");
    architecture.setParameter("neurons_per_layer", "250");
    architecture.setParameter("connection_probability", "0.2");
    
    // Define layers
    Layer inputLayer;
    inputLayer.setName("Layer1");
    inputLayer.setType("input");
    inputLayer.setNeuronCount(250);
    architecture.addLayer(inputLayer);
    
    Layer hiddenLayer1;
    hiddenLayer1.setName("Layer2");
    hiddenLayer1.setType("hidden");
    hiddenLayer1.setNeuronCount(250);
    architecture.addLayer(hiddenLayer1);
    
    Layer hiddenLayer2;
    hiddenLayer2.setName("Layer3");
    hiddenLayer2.setType("hidden");
    hiddenLayer2.setNeuronCount(250);
    architecture.addLayer(hiddenLayer2);
    
    Layer outputLayer;
    outputLayer.setName("Layer4");
    outputLayer.setType("output");
    outputLayer.setNeuronCount(250);
    architecture.addLayer(outputLayer);
    
    // Define connections
    Connection layer1ToLayer2;
    layer1ToLayer2.setSource("Layer1");
    layer1ToLayer2.setTarget("Layer2");
    layer1ToLayer2.setStrength(0.2);
    architecture.addConnection(layer1ToLayer2);
    
    Connection layer2ToLayer3;
    layer2ToLayer3.setSource("Layer2");
    layer2ToLayer3.setTarget("Layer3");
    layer2ToLayer3.setStrength(0.2);
    architecture.addConnection(layer2ToLayer3);
    
    Connection layer3ToLayer4;
    layer3ToLayer4.setSource("Layer3");
    layer3ToLayer4.setTarget("Layer4");
    layer3ToLayer4.setStrength(0.2);
    architecture.addConnection(layer3ToLayer4);
    
    return architecture;
}
```

### 4. Modular Architecture
```cpp
// Modular architecture definition
#include "ArchitectureDefinition.hpp"

ArchitectureDefinition createModularArchitecture() {
    ArchitectureDefinition architecture;
    
    // Set architecture name and type
    architecture.setName("ModularArchitecture");
    architecture.setType("modular");
    
    // Define modules
    Module neuronModelModule;
    neuronModelModule.setName("NeuronModelModule");
    neuronModelModule.setType("neuron_model");
    neuronModelModule.setPluginName("CustomNeuronModel");
    architecture.addModule(neuronModelModule);
    
    Module plasticityRuleModule;
    plasticityRuleModule.setName("PlasticityRuleModule");
    plasticityRuleModule.setType("plasticity_rule");
    plasticityRuleModule.setPluginName("CustomPlasticityRule");
    architecture.addModule(plasticityRuleModule);
    
    Module memorySystemModule;
    memorySystemModule.setName("MemorySystemModule");
    memorySystemModule.setType("memory_system");
    memorySystemModule.setPluginName("CustomMemorySystem");
    architecture.addModule(memorySystemModule);
    
    Module predictionSystemModule;
    predictionSystemModule.setName("PredictionSystemModule");
    predictionSystemModule.setType("prediction_system");
    predictionSystemModule.setPluginName("CustomPredictionSystem");
    architecture.addModule(predictionSystemModule);
    
    // Define connections
    Connection module1ToModule2;
    module1ToModule2.setSource("NeuronModelModule");
    module1ToModule2.setTarget("PlasticityRuleModule");
    module1ToModule2.setStrength(0.1);
    architecture.addConnection(module1ToModule2);
    
    Connection module2ToModule3;
    module2ToModule3.setSource("PlasticityRuleModule");
    module2ToModule3.setTarget("MemorySystemModule");
    module2ToModule3.setStrength(0.1);
    architecture.addConnection(module2ToModule3);
    
    Connection module3ToModule4;
    module3ToModule4.setSource("MemorySystemModule");
    module3ToModule4.setTarget("PredictionSystemModule");
    module3ToModule4.setStrength(0.1);
    architecture.addConnection(module3ToModule4);
    
    return architecture;
}
```

## Architecture Definition File Formats

### NLM Architecture Format
```ini
# NLM architecture definition
[architecture]
name = "BasicNeuralNetwork"
type = "neural_network"

[architecture.parameters]
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001

[architecture.layers]
input_layer_neurons = 100
hidden_layer_neurons = 500
output_layer_neurons = 100

[architecture.connections]
input_to_hidden = 0.15
hidden_to_output = 0.15
```

### YAML Architecture Format
```yaml
architecture:
  name: "BasicNeuralNetwork"
  type: "neural_network"
  
  parameters:
    neuron_count: 1000
    region_count: 2
    connection_probability: 0.15
    simulation_timestep: 0.001
    
  layers:
    input_layer:
      name: "InputLayer"
      type: "input"
      neuron_count: 100
    hidden_layer:
      name: "HiddenLayer"
      type: "hidden"
      neuron_count: 500
    output_layer:
      name: "OutputLayer"
      type: "output"
      neuron_count: 100
      
  connections:
    input_to_hidden:
      source: "InputLayer"
      target: "HiddenLayer"
      strength: 0.15
    hidden_to_output:
      source: "HiddenLayer"
      target: "OutputLayer"
      strength: 0.15
```

### JSON Architecture Format
```json
{
  "architecture": {
    "name": "BasicNeuralNetwork",
    "type": "neural_network",
    "parameters": {
      "neuron_count": 1000,
      "region_count": 2,
      "connection_probability": 0.15,
      "simulation_timestep": 0.001
    },
    "layers": [
      {
        "name": "InputLayer",
        "type": "input",
        "neuron_count": 100
      },
      {
        "name": "HiddenLayer",
        "type": "hidden",
        "neuron_count": 500
      },
      {
        "name": "OutputLayer",
        "type": "output",
        "neuron_count": 100
      }
    ],
    "connections": [
      {
        "source": "InputLayer",
        "target": "HiddenLayer",
        "strength": 0.15
      },
      {
        "source": "HiddenLayer",
        "target": "OutputLayer",
        "strength": 0.15
      }
    ]
  }
}
```

## Architecture Definition Commands

### Save Architecture
```bash
# Save architecture to file
./nlm --save-architecture --architecture my_architecture.ini --output my_architecture.arch

# Save architecture to YAML
./nlm --save-architecture --architecture my_architecture.ini --output my_architecture.yaml

# Save architecture to JSON
./nlm --save-architecture --architecture my_architecture.ini --output my_architecture.json
```

### Load Architecture
```bash
# Load architecture from file
./nlm --load-architecture --architecture my_architecture.arch

# Load architecture from YAML
./nlm --load-architecture --architecture my_architecture.yaml

# Load architecture from JSON
./nlm --load-architecture --architecture my_architecture.json
```

### List Architecture
```bash
# List all architectures
./nlm --list-architectures

# List architecture details
./nlm --list-architecture --architecture my_architecture.arch
```

### Validate Architecture
```bash
# Validate architecture
./nlm --validate-architecture --architecture my_architecture.arch

# Validate architecture with detailed output
./nlm --validate-architecture --architecture my_architecture.arch --verbose
```

### Export Architecture
```bash
# Export architecture to different formats
./nlm --export-architecture --architecture my_architecture.arch --format yaml
./nlm --export-architecture --architecture my_architecture.arch --format json
```

## Architecture Definition API

### ArchitectureDefinition Factory
```cpp
class ArchitectureDefinitionFactory {
public:
    static ArchitectureDefinition createFromFile(const std::string& filename);
    static ArchitectureDefinition createFromYAML(const std::string& filename);
    static ArchitectureDefinition createFromJSON(const std::string& filename);
    
    static ArchitectureDefinition createFromString(const std::string& configString);
    static ArchitectureDefinition createFromMap(const std::map<std::string, std::string>& parameters);
    
    static ArchitectureDefinition createNeuralNetworkArchitecture(const std::map<std::string, std::string>& parameters);
    static ArchitectureDefinition createBrainRegionArchitecture(const std::map<std::string, std::string>& parameters);
    static ArchitectureDefinition createLayeredArchitecture(const std::map<std::string, std::string>& parameters);
    static ArchitectureDefinition createModularArchitecture(const std::map<std::string, std::string>& parameters);
};
```

### ArchitectureDefinition Validator
```cpp
class ArchitectureDefinitionValidator {
public:
    ArchitectureDefinitionValidator();
    ~ArchitectureDefinitionValidator();
    
    void addValidationRule(const ValidationRule& rule);
    bool validate(const ArchitectureDefinition& architecture) const;
    std::vector<std::string> getValidationErrors(const ArchitectureDefinition& architecture) const;
    
    // Built-in validation rules
    static ValidationRule createNeuronCountRule(int min = 10, int max = 100000);
    static ValidationRule createRegionCountRule(int min = 1, int max = 100);
    static ValidationRule createConnectionProbabilityRule(float min = 0.0, float max = 1.0);
    static ValidationRule createSimulationTimestepRule(float min = 0.0001, float max = 0.1);
    static ValidationRule createLayerCountRule(int min = 1, int max = 100);
    static ValidationRule createModuleCountRule(int min = 1, int max = 100);
    
    // Custom validation rules
    static ValidationRule createCustomRule(const std::string& name,
                                          bool (*validator)(const ArchitectureDefinition&),
                                          const std::string& errorMessage);
};
```

### ArchitectureDefinition Exporter
```cpp
class ArchitectureDefinitionExporter {
public:
    static std::string exportToString(const ArchitectureDefinition& architecture);
    static bool exportToFile(const ArchitectureDefinition& architecture, const std::string& filename);
    static std::string exportToYAML(const ArchitectureDefinition& architecture);
    static std::string exportToJSON(const ArchitectureDefinition& architecture);
    
    // Export to specific formats
    static std::string exportToIni(const ArchitectureDefinition& architecture);
    static std::string exportToXML(const ArchitectureDefinition& architecture);
    static std::string exportToTOML(const ArchitectureDefinition& architecture);
};
```

## Architecture Definition Testing

### Architecture Definition Testing
```cpp
#include <gtest/gtest.h>
#include "ArchitectureDefinition.hpp"

TEST(ArchitectureDefinitionTest, CreateBasicNeuralNetwork) {
    ArchitectureDefinition architecture = createBasicNeuralNetworkArchitecture();
    EXPECT_EQ(architecture.getName(), "BasicNeuralNetwork");
    EXPECT_EQ(architecture.getType(), "neural_network");
    EXPECT_TRUE(architecture.validate());
}

TEST(ArchitectureDefinitionTest, CreateBrainRegionArchitecture) {
    ArchitectureDefinition architecture = createBrainRegionArchitecture();
    EXPECT_EQ(architecture.getName(), "BrainRegionArchitecture");
    EXPECT_EQ(architecture.getType(), "region");
    EXPECT_TRUE(architecture.validate());
}

TEST(ArchitectureDefinitionTest, CreateLayeredArchitecture) {
    ArchitectureDefinition architecture = createLayeredArchitecture();
    EXPECT_EQ(architecture.getName(), "LayeredArchitecture");
    EXPECT_EQ(architecture.getType(), "layer");
    EXPECT_TRUE(architecture.validate());
}

TEST(ArchitectureDefinitionTest, CreateModularArchitecture) {
    ArchitectureDefinition architecture = createModularArchitecture();
    EXPECT_EQ(architecture.getName(), "ModularArchitecture");
    EXPECT_EQ(architecture.getType(), "modular");
    EXPECT_TRUE(architecture.validate());
}
```

## Architecture Definition Migration

### Architecture Definition Migration
```cpp
class ArchitectureDefinitionMigration {
public:
    static ArchitectureDefinition migrate(const std::string& oldFormat, const std::string& oldConfig);
    static ArchitectureDefinition migrate(const std::string& filename);
    
    // Migration from v1.0 to v2.0
    static ArchitectureDefinition migrate_v1_to_v2(const std::string& oldConfig);
    
    // Migration from v2.0 to v3.0
    static ArchitectureDefinition migrate_v2_to_v3(const std::string& oldConfig);
    
    // Migration from JSON to YAML
    static ArchitectureDefinition migrate_json_to_yaml(const std::string& jsonConfig);
    
    // Migration from YAML to JSON
    static ArchitectureDefinition migrate_yaml_to_json(const std::string& yamlConfig);
};
```

## Architecture Definition Documentation

### Architecture Definition Documentation Generator
```cpp
class ArchitectureDefinitionDocumentationGenerator {
public:
    static std::string generateArchitectureDocumentation(const ArchitectureDefinition& architecture);
    static std::string generateArchitectureSchema(const ArchitectureDefinition& architecture);
    static std::string generateArchitectureExamples(const ArchitectureDefinition& architecture);
    
    // Generate architecture documentation
    static std::string generateArchitectureDocumentation(
        const std::vector<ArchitectureDefinition>& architectures);
    
    // Generate architecture schema
    static std::string generateArchitectureSchema(
        const std::vector<ArchitectureDefinition>& architectures);
    
    // Generate architecture examples
    static std::string generateArchitectureExamples(
        const std::vector<ArchitectureDefinition>& architectures);
};
```

## Architecture Definition Best Practices

### 1. Use Standard Architecture Types
- Use standard architecture types (neural_network, region, layer, modular)
- Define custom architecture types only when necessary
- Use standard parameters when possible

### 2. Validate Architecture
- Always validate architecture before use
- Check for errors and inconsistencies
- Optimize architecture performance

### 3. Save Architecture
- Save architecture to file for future use
- Export architecture to different formats
- Import architecture from file

### 4. Document Architecture
- Document architecture with comments
- Document architecture parameters
- Document architecture components

### 5. Test Architecture
- Test architecture with different configurations
- Test architecture with different parameters
- Test architecture with different components

## Architecture Definition Support

### Architecture Definition Documentation
- **Documentation**: https://nlm.readthedocs.io/architecture-definition
- **API Reference**: https://nlm.readthedocs.io/api/architecture-definition
- **Architecture Examples**: https://github.com/nlm/architecture-definition-examples

### Architecture Definition Support
- **Issue Tracker**: https://github.com/nlm/architecture-definition/issues
- **Forum**: https://forum.nlm.ai/architecture-definition
- **Slack**: https://slack.nlm.ai/architecture-definition

### Architecture Definition Development
- **Contributing**: https://github.com/nlm/architecture-definition/blob/main/CONTRIBUTING.md
- **Code Style**: https://github.com/nlm/architecture-definition/blob/main/CODE_STYLE.md
- **Testing**: https://github.com/nlm/architecture-definition/blob/main/TESTING.md

## Conclusion

NLM's architecture definition guide provides comprehensive instructions for defining neural network architectures in NLM. This guide covers the architecture definition process, API documentation, examples, and best practices to help users effectively define neural network architectures.

For more information about specific architecture definition topics or features, use the `--help` option or refer to the NLM documentation.
