// PluginFactory.h
// Factory patterns for plugin creation

#pragma once

#include "PluginInterface.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <typeindex>

namespace nlm {

    // Plugin factory function type
    template<typename PluginType>
    using PluginFactoryFunc = std::function<std::shared_ptr<PluginType>()>;

    // Plugin factory registry
    class PluginFactory {
    public:
        PluginFactory() = default;
        ~PluginFactory() = default;

        // Plugin registration
        template<typename PluginType>
        bool registerPlugin(const std::string& name, const std::string& version = "1.0.0",
                           const std::string& description = "",
                           const std::string& author = "",
                           const std::vector<std::string>& dependencies = {});

        template<typename PluginType>
        bool unregisterPlugin(const std::string& name);

        // Plugin creation
        template<typename PluginType>
        std::shared_ptr<PluginType> createPlugin(const std::string& name);

        template<typename PluginType>
        std::shared_ptr<PluginType> createPlugin(const PluginMetadata& metadata);

        // Plugin lookup
        bool hasPlugin(const std::string& name) const;
        std::vector<std::string> getPluginNames() const;
        std::vector<std::string> getPluginNamesByType(const std::type_index& type) const;

        // Plugin information
        const PluginMetadata* getPluginMetadata(const std::string& name) const;
        std::map<std::string, PluginMetadata> getAllPluginMetadata() const;

        // Plugin type management
        template<typename PluginType>
        bool registerPluginType(const std::string& typeName);

        bool isPluginTypeRegistered(const std::string& typeName) const;
        std::vector<std::string> getRegisteredPluginTypes() const;

        // Plugin capabilities
        std::vector<PluginCapability> getPluginCapabilities(const std::string& name) const;
        bool addPluginCapability(const std::string& name, PluginCapability capability);

        // Plugin metadata management
        bool updatePluginMetadata(const std::string& name, const PluginMetadata& metadata);
        bool validatePluginMetadata(const PluginMetadata& metadata) const;

        // Plugin statistics
        size_t getPluginCount() const;
        std::map<std::string, std::any> getPluginStats(const std::string& name) const;

        // Plugin configuration
        bool configurePlugin(const std::string& name, const std::map<std::string, std::any>& config);
        std::map<std::string, std::any> getPluginConfig(const std::string& name) const;

        // Plugin lifecycle
        bool initializePlugin(const std::string& name);
        bool startPlugin(const std::string& name);
        bool stopPlugin(const std::string& name);
        bool unloadPlugin(const std::string& name);
        bool unloadPluginCompletely(const std::string& name);

        // Plugin interface access
        std::shared_ptr<PluginInterface> getPlugin(const std::string& name) const;
        std::shared_ptr<NeuralComponentPlugin> getNeuralComponentPlugin(const std::string& name) const;
        std::shared_ptr<MemoryPlugin> getMemoryPlugin(const std::string& name) const;
        std::shared_ptr<ProcessingPlugin> getProcessingPlugin(const std::string& name) const;

        // Plugin search and filtering
        std::vector<std::string> searchPlugins(const std::string& query) const;
        std::vector<std::string> findPluginsByCapability(PluginCapability capability) const;
        std::vector<std::string> findPluginsByCategory(const std::string& category) const;

        // Plugin dependency management
        bool addPluginDependency(const std::string& pluginName, const std::string& dependencyName);
        bool removePluginDependency(const std::string& pluginName, const std::string& dependencyName);
        std::vector<std::string> getPluginDependencies(const std::string& name) const;
        bool checkPluginDependencies(const std::string& name) const;

        // Plugin version management
        bool setPluginVersion(const std::string& name, const std::string& version);
        std::string getPluginVersion(const std::string& name) const;

        // Plugin error handling
        std::string getPluginError(const std::string& name) const;
        bool hasPluginError(const std::string& name) const;

        // Plugin thread safety
        void lock() const;
        void unlock() const;

        // Plugin cleanup
        bool cleanupAllPlugins();
        bool saveFactoryState(const std::string& filePath) const;
        bool loadFactoryState(const std::string& filePath);

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;

        // Private helper methods
        std::type_index getPluginType(const std::string& name) const;
        std::string generatePluginId(const std::string& name, const std::string& version) const;
        bool validatePluginName(const std::string& name) const;
        void updatePluginStats(const std::string& name, const std::map<std::string, std::any>& stats);
        void triggerPluginEvent(const std::string& eventType, const std::string& pluginName, const std::any& data = nullptr);
    };

    // Neural component plugin factory
    class NeuralComponentPluginFactory : public PluginFactory {
    public:
        NeuralComponentPluginFactory();
        ~NeuralComponentPluginFactory() = default;

        // Neuron type registration
        template<typename NeuronType>
        bool registerNeuronType(const std::string& name,
                               const std::string& description = "",
                               const std::map<std::string, std::any>& config = {});

        template<typename SynapseType>
        bool registerSynapseType(const std::string& name,
                                const std::string& description = "",
                                const std::map<std::string, std::any>& config = {});

        template<typename DynamicsType>
        bool registerNeuralDynamicsType(const std::string& name,
                                       const std::string& description = "",
                                       const std::map<std::string, std::any>& config = {});

        template<typename PlasticityType>
        bool registerPlasticityRuleType(const std::string& name,
                                       const std::string& description = "",
                                       const std::map<std::string, std::any>& config = {});

        // Component creation
        std::shared_ptr<NeuronType> createNeuron(const std::string& typeName);
        std::shared_ptr<SynapseType> createSynapse(const std::string& typeName);
        std::shared_ptr<NeuralDynamicsType> createNeuralDynamics(const std::string& typeName);
        std::shared_ptr<PlasticityType> createPlasticityRule(const std::string& typeName);

        // Component lookup
        bool hasNeuronType(const std::string& name) const;
        bool hasSynapseType(const std::string& name) const;
        bool hasNeuralDynamicsType(const std::string& name) const;
        bool hasPlasticityRuleType(const std::string& name) const;

        // Component information
        std::string getNeuronTypeDescription(const std::string& name) const;
        std::string getSynapseTypeDescription(const std::string& name) const;
        std::string getNeuralDynamicsTypeDescription(const std::string& name) const;
        std::string getPlasticityRuleTypeDescription(const std::string& name) const;
    };

    // Memory plugin factory
    class MemoryPluginFactory : public PluginFactory {
    public:
        MemoryPluginFactory();
        ~MemoryPluginFactory() = default;

        // Memory architecture registration
        template<typename MemoryArchitecture>
        bool registerMemoryArchitecture(const std::string& name,
                                        const std::string& description = "",
                                        const std::map<std::string, std::any>& config = {});

        // Consolidation algorithm registration
        template<typename ConsolidationAlgorithm>
        bool registerConsolidationAlgorithm(const std::string& name,
                                           const std::string& description = "",
                                           const std::map<std::string, std::any>& config = {});

        // Retrieval strategy registration
        template<typename RetrievalStrategy>
        bool registerRetrievalStrategy(const std::string& name,
                                       const std::string& description = "",
                                       const std::map<std::string, std::any>& config = {});

        // Architecture creation
        std::shared_ptr<MemoryArchitecture> createMemoryArchitecture(const std::string& name);
        std::shared_ptr<ConsolidationAlgorithm> createConsolidationAlgorithm(const std::string& name);
        std::shared_ptr<RetrievalStrategy> createRetrievalStrategy(const std::string& name);
    };

    // Processing plugin factory
    class ProcessingPluginFactory : public PluginFactory {
    public:
        ProcessingPluginFactory();
        ~ProcessingPluginFactory() = default;

        // Learning algorithm registration
        template<typename LearningAlgorithm>
        bool registerLearningAlgorithm(const std::string& name,
                                      const std::string& description = "",
                                      const std::map<std::string, std::any>& config = {});

        // Prediction system registration
        template<typename PredictionSystem>
        bool registerPredictionSystem(const std::string& name,
                                     const std::string& description = "",
                                     const std::map<std::string, std::any>& config = {});

        // Neuromodulation strategy registration
        template<typename NeuromodulationStrategy>
        bool registerNeuromodulationStrategy(const std::string& name,
                                            const std::string& description = "",
                                            const std::map<std::string, std::any>& config = {});

        // Development process registration
        template<typename DevelopmentProcess>
        bool registerDevelopmentProcess(const std::string& name,
                                       const std::string& description = "",
                                       const std::map<std::string, std::any>& config = {});

        // Algorithm creation
        std::shared_ptr<LearningAlgorithm> createLearningAlgorithm(const std::string& name);
        std::shared_ptr<PredictionSystem> createPredictionSystem(const std::string& name);
        std::shared_ptr<NeuromodulationStrategy> createNeuromodulationStrategy(const std::string& name);
        std::shared_ptr<DevelopmentProcess> createDevelopmentProcess(const std::string& name);
    };

    // Plugin factory utility functions
    namespace PluginFactoryUtils {
        std::string generatePluginId(const std::string& prefix = "plugin", const std::string& suffix = "");
        bool validatePluginName(const std::string& name);
        std::string sanitizePluginName(const std::string& name);
        std::string generateVersion(const std::string& base = "1.0.0", int patch = 0);
        bool isVersionCompatible(const std::string& version, const std::string& minVersion);
        pybind11::dict anyToPyDict(const std::map<std::string, std::any>& config);
        std::map<std::string, std::any> pyDictToAny(const pybind11::dict& pyDict);
        std::type_index getPluginTypeIndex(const std::shared_ptr<PluginInterface>& plugin);
        std::string getPluginTypeName(const std::type_index& typeIndex);
    }

    // Plugin registration macros
    #define REGISTER_NEURAL_COMPONENT_PLUGIN(PluginClass) \
        namespace { \
            struct NeuralComponentPluginRegistrar { \
                NeuralComponentPluginRegistrar() { \
                    /* Registration logic would go here */ \
                } \
            }; \
            NeuralComponentPluginRegistrar registrar; \
        }

    #define REGISTER_MEMORY_PLUGIN(PluginClass) \
        namespace { \
            struct MemoryPluginRegistrar { \
                MemoryPluginRegistrar() { \
                    /* Registration logic would go here */ \
                } \
            }; \
            MemoryPluginRegistrar registrar; \
        }

    #define REGISTER_PROCESSING_PLUGIN(PluginClass) \
        namespace { \
            struct ProcessingPluginRegistrar { \
                ProcessingPluginRegistrar() { \
                    /* Registration logic would go here */ \
                } \
            }; \
            ProcessingPluginRegistrar registrar; \
        }

    #define REGISTER_PLUGIN_FACTORY(FactoryClass) \
        namespace { \
            struct FactoryRegistrar { \
                FactoryRegistrar() { \
                    /* Registration logic would go here */ \
                } \
            }; \
            FactoryRegistrar registrar; \
        }

} // namespace nlm

// Plugin factory implementation template specializations
namespace nlm {

    template<typename PluginType>
    inline bool PluginFactory::registerPlugin(const std::string& name, const std::string& version,
                                             const std::string& description, const std::string& author,
                                             const std::vector<std::string>& dependencies) {
        // Implementation would go here
        return false;
    }

    template<typename PluginType>
    inline bool PluginFactory::unregisterPlugin(const std::string& name) {
        // Implementation would go here
        return false;
    }

    template<typename PluginType>
    inline std::shared_ptr<PluginType> PluginFactory::createPlugin(const std::string& name) {
        // Implementation would go here
        return nullptr;
    }

    template<typename PluginType>
    inline std::shared_ptr<PluginType> PluginFactory::createPlugin(const PluginMetadata& metadata) {
        // Implementation would go here
        return nullptr;
    }

    template<typename PluginType>
    inline bool PluginFactory::registerPluginType(const std::string& typeName) {
        // Implementation would go here
        return false;
    }

} // namespace nlm