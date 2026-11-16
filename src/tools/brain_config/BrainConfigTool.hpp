# BrainConfigTool.hpp

#pragma once

#include <string>
#include <vector>
#include "Tools.hpp"

namespace nlm {

class BrainConfigToolImpl;

class BrainConfigTool {
public:
    BrainConfigTool();
    ~BrainConfigTool();
    
    bool create(const std::vector<std::pair<std::string, std::string>>& args);
    bool load(const std::string& filepath);
    bool save(const std::string& filepath, bool pretty = false);
    bool list(const std::string& pattern = "");
    bool set(const std::string& key, const std::string& value);
    bool get(const std::string& key);
    bool delete(const std::string& key);
    bool validate(const std::string& filepath);
    
    std::string getCurrentFile() const { return currentFile_; }

private:
    ToolConfig toolConfig_;
    std::shared_ptr<Config> config_;
    std::string currentFile_;
    std::unique_ptr<BrainConfigToolImpl> toolImpl_;
};

} // namespace nlm