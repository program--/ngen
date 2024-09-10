#include <bmi/DynLibrary.hpp>

#include <dlfcn.h>

namespace ngen {

DynLibrary::DynLibrary(std::string path)
  : handle_(nullptr)
  , path_(std::move(path))
{}

DynLibrary::~DynLibrary()
{
    if (handle_ != nullptr) {
        dlclose(handle_);
    }
}

void DynLibrary::open()
{
    if (handle_ != nullptr)
        return;

    handle_ = dlopen(path_.c_str(), RTLD_NOW | RTLD_LOCAL);
    const char* err = dlerror();
    if (handle_ == nullptr || err != nullptr) {
        throw dynamic_loading_error{path_, err};
    }
}

void DynLibrary::close(bool check)
{
    if (handle_ == nullptr)
        return;

    dlclose(handle_);
    handle_ = nullptr;

    // Clear the error regardless of checking it.
    const char* err = dlerror();
    if (check) {
        if (err != nullptr) {
            throw dynamic_loading_error{path_, err};
        }
    }
}

void* DynLibrary::symbol(const std::string& symbol, bool require)
{
    if (handle_ == nullptr && !require)
        return nullptr;

    if (handle_ == nullptr && require)
        throw dynamic_loading_error{path_, "library not loaded. Was DynLibrary::open() called?", symbol};

    if (symbol.empty())
        throw dynamic_loading_error{path_, "Argument `symbol` is an empty string."};

    void* sym = dlsym(handle_, symbol.c_str());
    const char* err = dlerror();
    if ((sym == nullptr || err != nullptr) && require)
        throw dynamic_loading_error{path_, err, symbol};

    return sym;
}

} // namespace ngen
