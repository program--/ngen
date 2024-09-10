#pragma once

#include <string>
#include <stdexcept>

namespace ngen {

struct dynamic_loading_error
  : public std::runtime_error
{
    dynamic_loading_error(std::string path, std::string message)
      : runtime_error("Error in '" + std::move(path) + "': " + std::move(message))
    {}

    dynamic_loading_error(std::string path, std::string message, std::string symbol)
      : runtime_error(
        "Error loading symbol `" +
        std::move(symbol) +
        "` from '" +
        std::move(path) + "': " +
        std::move(message)
      )
    {}
};

struct DynLibrary
{
    DynLibrary(std::string path);

    ~DynLibrary();

    void open();
    void close(bool check = false);
    void* symbol(const std::string& symbol, bool require = false);

  private:
    void*       handle_;
    std::string path_;
};

} // namespace ngen
