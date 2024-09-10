#pragma once

#include "Bmi_Dyn_Adapter.hpp"

namespace ngen {

struct Bmi_Fortran_Adapter final : public Bmi_Dyn_Adapter
{
    using Bmi_Dyn_Adapter::Bmi_Dyn_Adapter;

    ~Bmi_Fortran_Adapter() override = default;

    NGEN_BMI_DECLARE_OVERRIDES;

  private:
    void construct_backing_model() override;

    using fortran_handle_t = void*;
    fortran_handle_t ptr_ = nullptr;
};

} // namespace ngen
