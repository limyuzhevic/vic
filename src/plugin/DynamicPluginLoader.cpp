#include <stdexcept>
#include <cstring>
#include <system_error>

namespace nlm {

DynamicPluginLoader::DynamicPluginLoader(const std::string& libraryPath) 
    : libraryPath(libraryPath), handle(nullptr), loaded(false) {
}

DynamicPluginLoader::~DynamicPluginLoader() {
    if (loaded) {
        unload();
    }
}

bool DynamicPluginLoader::load() {
    if (loaded) {
        return true;
    }

    handle = dlopen(libraryPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        return false;
    }

    loaded = true;
    return true;
}

void DynamicPluginLoader::unload() {
    if (!loaded || !handle) {
        return;
    }

    dlclose(handle);
    handle = nullptr;
    loaded = false;
}

void* DynamicPluginLoader::getSymbol(const std::string& symbolName) {
    if (!loaded || !handle) {
        return nullptr;
    }

    return dlsym(handle, symbolName.c_str());
}

} // namespace nlm