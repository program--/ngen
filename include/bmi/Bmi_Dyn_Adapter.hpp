#pragma once

#include "Bmi_Adapter.hpp"
#include "DynLibrary.hpp"

namespace ngen {

struct Bmi_Dyn_Adapter : public Bmi_Adapter
{
    Bmi_Dyn_Adapter(
        std::string model_name,
        std::string library_path,
        std::string registration_func
    );

    ~Bmi_Dyn_Adapter() override = default;

  protected:
    void* registration();

    void* symbol(const std::string& symbol, bool require = false);

  private:
    DynLibrary dl_;
    std::string registration_func_;
};

} // namespace ngen
