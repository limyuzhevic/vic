// Final implementation file to ensure all symbols are defined
#include "AdvancedConfiguration.hpp"
#include "ProfileManager.hpp"
#include "EnvVarManager.hpp"
#include "PerformanceOptimizer.hpp"
#include "ConfigurationManager.hpp"
#include "VisualizationInterface.hpp"
#include "ConfigurationValidation.hpp"
#include "ErrorHandling.hpp"

// This file includes all the implementation files to ensure proper linking
#include "AdvancedConfiguration.cpp"
#include "ProfileManager.cpp"
#include "EnvVarManager.cpp"
#include "PerformanceOptimizer.cpp"
#include "VisualizationInterface.cpp"
#include "VisualizationEvent.cpp"
#include "ConfigurationManager.cpp"
#include "ConfigurationValidation.cpp"
#include "ErrorHandling.cpp"

// This ensures that all template instantiations are included
namespace nlm {
    // Template instantiations that were moved to .cpp files
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
    template std::optional<float> Config::get<float>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
    template float Config::getOr<float>(const std::string&, const float&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}
