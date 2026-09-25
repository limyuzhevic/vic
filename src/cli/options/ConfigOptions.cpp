#include "cli/options/ConfigOptions.hpp"
#include <algorithm>

using namespace nlm::cli::options;

ConfigOptions::ConfigOptions()
    : configValidate(false) {
}

void ConfigOptions::parse(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key == "config") {
                    configFile = value;
                } else if (key == "config-set") {
                    configKeyValue = value;
                } else if (key == "config-export") {
                    configExportFile = value;
                } else if (key == "config-import") {
                    configImportFile = value;
                } else if (key == "config-validate") {
                    configValidate = true;
                }
            }
        }
    }
}
