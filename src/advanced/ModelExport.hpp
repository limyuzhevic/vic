#ifndef NLM_MODEL_EXPORT_HPP
#define NLM_MODEL_EXPORT_HPP

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

namespace nlm {

class ModelExporter {
public:
    static bool exportModel(const std::string& filename, const std::shared_ptr<Brain>& brain);
    static bool importModel(const std::string& filename, std::shared_ptr<Brain>& brain);
    static bool exportConfig(const std::string& filename, const std::shared_ptr<Config>& config);
    static bool importConfig(const std::string& filename, std::shared_ptr<Config>& config);
    static nlohmann::json brainToJson(const std::shared_ptr<Brain>& brain);
    static std::shared_ptr<Brain> jsonToBrain(const nlohmann::json& j);
    static nlohmann::json configToJson(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Config> jsonToConfig(const nlohmann::json& j);
    
private:
    static void restoreBrainStateFromJson(const std::shared_ptr<Brain>& brain, const nlohmann::json& regionsJson);
    static void restoreNeuronsFromJson(const std::shared_ptr<NeuralRegion>& region, const nlohmann::json& neuronsJson);
    static void restoreSynapsesFromJson(const std::shared_ptr<NeuralRegion>& region, const nlohmann::json& synapsesJson, size_t neuronCount);
};

} // namespace nlm

#endif // NLM_MODEL_EXPORT_HPP