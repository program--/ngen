#pragma once

#include "bmi.hpp"

#include "Bmi_Dyn_Adapter.hpp"

#include <memory>

namespace ngen {

struct Bmi_Cpp_Adapter final : public Bmi_Dyn_Adapter
{
    using Bmi_Dyn_Adapter::Bmi_Dyn_Adapter;

    ~Bmi_Cpp_Adapter() override = default;

    NGEN_BMI_DECLARE_OVERRIDES;

  private:

    void construct_backing_model() override;

    std::unique_ptr<::bmi::Bmi> ptr_ = nullptr;
};

} // namespace ngen
