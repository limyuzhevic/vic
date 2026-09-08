#include "PlasticityRule.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "StructuralPlasticity.hpp"
#include <memory>
#include <map>

namespace nlm {

class PlasticityRuleFactory {
public:
    using Creator = std::function<std::unique_ptr<PlasticityRule>()>;
    
    template<typename T>
    static void registerCreator(const std::string& typeName) {
        creators_[typeName] = []() { return std::make_unique<T>(); };
    }
    
    static std::unique_ptr<PlasticityRule> create(const std::string& typeName) {
        auto it = creators_.find(typeName);
        if (it != creators_.end()) {
            return it->second();
        }
        throw std::runtime_error("Unknown plasticity rule type: " + typeName);
    }
    
    static std::vector<std::string> getAvailableTypes() {
        std::vector<std::string> types;
        types.reserve(creators_.size());
        for (const auto& pair : creators_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
private:
    static inline std::map<std::string, Creator> creators_;
};

// Initialize factory with available plasticity rules
void initializePlasticityRuleFactory() {
    PlasticityRuleFactory::registerCreator<STDP>("STDP");
    PlasticityRuleFactory::registerCreator<Hebbian>("Hebbian");
    PlasticityRuleFactory::registerCreator<AntiHebbianRule>("AntiHebbian");
    PlasticityRuleFactory::registerCreator<BCMRule>("BCM");
}

} // namespace nlm