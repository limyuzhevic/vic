#include "cli/options/LoggingOptions.hpp"
#include <algorithm>

using namespace nlm::cli::options;

LoggingOptions::LoggingOptions()
    : verbose(false)
    , debug(false)
    , trace(false) {
    logLevel = "info";
}

void LoggingOptions::parse(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key == "log-level") {
                    logLevel = value;
                } else if (key == "log-file") {
                    logFile = value;
                }
            } else {
                if (arg == "--verbose") verbose = true;
                else if (arg == "--debug") debug = true;
                else if (arg == "--trace") trace = true;
            }
        }
    }
}
