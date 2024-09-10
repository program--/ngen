#include "bmi/Bmi_Adapter.hpp"

namespace ngen {

Bmi_Adapter::Bmi_Adapter(std::string model_name)
  : initialized_(false)
  , model_name_(std::move(model_name))
{}

bool Bmi_Adapter::initialized() const noexcept
{
    return initialized_;
}

const std::string& Bmi_Adapter::name() const noexcept
{
    return model_name_;
}

void Bmi_Adapter::set_initialized() noexcept
{
    initialized_ = true;
}

} // namespace ngen
