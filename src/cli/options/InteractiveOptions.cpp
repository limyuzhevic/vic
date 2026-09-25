#include "cli/options/InteractiveOptions.hpp"

using namespace nlm::cli::options;

InteractiveOptions::InteractiveOptions()
    : interactive(false)
    , watch(false)
    , monitor(false) {
}

void InteractiveOptions::parse(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg == "--interactive") interactive = true;
        else if (arg == "--watch") watch = true;
        else if (arg == "--monitor") monitor = true;
    }
}
