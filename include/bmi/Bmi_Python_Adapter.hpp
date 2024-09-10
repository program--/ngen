#pragma once

#include "Bmi_Adapter.hpp"

// pybind11
#include <pybind11/pytypes.h>

namespace ngen {

struct Bmi_Python_Adapter final : public Bmi_Adapter
{
    Bmi_Python_Adapter(
        std::string model_name,
        std::string classpath
    );

    ~Bmi_Python_Adapter() override = default;

    NGEN_BMI_DECLARE_OVERRIDES;

  private:
    void GetGridXYZ(const int grid, double* coords, int dim);

    void construct_backing_model() override;

    std::string python_classpath_;
    pybind11::object model_;
};

} // namespace ngen
