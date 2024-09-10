#include "bmi/Bmi_Dyn_Adapter.hpp"

namespace ngen {

Bmi_Dyn_Adapter::Bmi_Dyn_Adapter(
    std::string model_name,
    std::string library_path,
    std::string registration_func
)
  : Bmi_Adapter(std::move(model_name))
  , dl_(std::move(library_path))
  , registration_func_(std::move(registration_func))
{}

void* Bmi_Dyn_Adapter::registration()
{
    // If already open, this is a no-op.
    dl_.open();

    return dl_.symbol(registration_func_, true);
}

void* Bmi_Dyn_Adapter::symbol(const std::string& symbol, bool require)
{
    return dl_.symbol(symbol, require);
}

} // namespace ngen
