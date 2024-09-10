#pragma once

#include "Bmi_Dyn_Adapter.hpp"

#include "bmi.h"

#include <memory>

namespace ngen {

namespace detail {
using CBmi = ::Bmi;
} // namespace detail

struct Bmi_C_Adapter final : public Bmi_Dyn_Adapter
{
    using Bmi_Dyn_Adapter::Bmi_Dyn_Adapter;

    ~Bmi_C_Adapter() override = default;

    NGEN_BMI_DECLARE_OVERRIDES;

  private:

    void construct_backing_model() override;

    std::unique_ptr<detail::CBmi> ptr_ = nullptr;
};

} // namespace ngen
