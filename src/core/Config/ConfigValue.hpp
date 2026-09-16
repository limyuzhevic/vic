#pragma once

#include <string>
#include <variant>

namespace nlm {

// Configuration value types - used throughout the NLM system
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

} // namespace nlm