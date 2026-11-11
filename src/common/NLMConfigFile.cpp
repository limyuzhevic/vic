// Configuration file parser implementation for NLM

#include "common/NLMConfigFile.hpp"

namespace nlm {
namespace {

bool ConfigFile::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace

} // namespace nlm