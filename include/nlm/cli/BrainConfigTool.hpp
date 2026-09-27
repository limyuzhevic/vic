#ifndef BRAIN_CONFIG_TOOL_HPP
#define BRAIN_CONFIG_TOOL_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {
namespace cli {

class BrainConfigTool {
public:
    BrainConfigTool();
    ~BrainConfigTool();

    bool LoadConfig(const std::string& configPath);
    bool SaveConfig(const std::string& configPath) const;
    bool ValidateConfig() const;
    bool BackupConfig(const std::string& backupPath) const;
    bool RestoreConfig(const std::string& backupPath);
    bool CompareConfigs(const std::string& configPath1, const std::string& configPath2) const;
    bool OptimizeConfig();
    bool UpdateConfig(const std::string& key, const std::string& value);
    std::string GetConfigValue(const std::string& key) const;
    std::map<std::string, std::string> GetAllConfigValues() const { return configData; }

    bool CreateBrainConfig(const std::string& brainPath);
    bool InitializeBrainConfig(const std::string& brainPath, const std::string& initData);
    bool ExportConfig(const std::string& outputPath, const std::string& format = "json") const;
    bool ImportConfig(const std::string& inputPath, const std::string& format = "json");

protected:
    std::map<std::string, std::string> configData;
    std::string currentConfigPath;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_CONFIG_TOOL_HPP