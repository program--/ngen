#include "bmi/Model.hpp"

#include "bmi/Grid.hpp"

#include <NGenConfig.h>

#include <unordered_map>

#include <boost/core/make_span.hpp>

namespace ngen {

/* Helper Functions */
GridType parse_grid_type(const std::string& type)
{
    switch(type[0]) {
        case 'v':
            return GridType::vector;
        case 'p':
            return GridType::points;
        case 'r':
            return GridType::uniform_rectilinear;
        case 's':
            return type[1] == 'c' ? GridType::scalar : GridType::structured_quadrilateral;
        case 'u':
            return type[2] == 's' ? GridType::unstructured : GridType::uniform_rectilinear;
        default:
            throw std::runtime_error("Invalid grid type " + type);
    }
}

Variable::dtype parse_variable_type(const std::string& type)
{
    static const std::unordered_map<std::string, Variable::dtype> typemap = {
        // int32_t //
        { "int",     Variable::dtype::i32 },
        { "integer", Variable::dtype::i32 },
        { "int32",   Variable::dtype::i32 },
        
        // int64_t //
        { "int64",    Variable::dtype::i64 },
        { "long",     Variable::dtype::i64 },
        { "long int", Variable::dtype::i64 },

        // float //
        { "float",            Variable::dtype::f32 },
        { "real",             Variable::dtype::f32 },
        { "single precision", Variable::dtype::f32 },
        { "float32",          Variable::dtype::f32 },
        { "np.float32",       Variable::dtype::f32 },
        { "numpy.float32",    Variable::dtype::f32 },
        { "np.single",        Variable::dtype::f32 },
        { "numpy.single",     Variable::dtype::f32 },

        // double //
        { "double",           Variable::dtype::f64 },
        { "float64",          Variable::dtype::f64 },
        { "np.float64",       Variable::dtype::f64 },
        { "numpy.float64",    Variable::dtype::f64 },
        { "np.double",        Variable::dtype::f64 },
        { "numpy.double",     Variable::dtype::f64 },
        { "double precision", Variable::dtype::f64 }
    };

    return typemap.at(type);
}

std::string to_string(Variable::dtype dtype) noexcept
{
    switch(dtype) {
        case Variable::dtype::i32:
            return "i32";
        case Variable::dtype::i64:
            return "i64";
        case Variable::dtype::f32:
            return "f32";
        case Variable::dtype::f64:
            return "f64";
    }

    return "unknown";
}

std::string to_string(GridType gtype) noexcept
{
    switch (gtype) {
        case GridType::scalar:
            return "scalar";
        case GridType::points:
            return "points";
        case GridType::vector:
            return "vector";
        case GridType::rectilinear:
            return "rectilinear";
        case GridType::uniform_rectilinear:
            return "uniform_rectilinear";
        case GridType::structured_quadrilateral:
            return "structured_quadrilateral";
        case GridType::unstructured:
            return "unstructured";
    }

    return "unknown";
}

Variable initialize_variable(
    Bmi_Adapter& model,
    const Grid* grid,
    const std::string& variable
)
{   
    void* data = model.GetValuePtr(variable);
    std::size_t align = model.GetVarItemsize(variable);
    std::size_t size  = model.GetVarNbytes(variable) / align;
    Variable::dtype type = parse_variable_type(model.GetVarType(variable));

    return Variable{ data, type, align, size, grid };
}


std::unique_ptr<Grid> initialize_grid(Bmi_Adapter& model, const int grid)
{
    GridType type = parse_grid_type(model.GetGridType(grid));
    const int rank = model.GetGridRank(grid);
    const int size = model.GetGridSize(grid);

    std::unique_ptr<Grid> result = nullptr;

    const bool is_unstructured = (
        type != GridType::structured_quadrilateral &&
        type != GridType::rectilinear &&
        type != GridType::uniform_rectilinear
    );

    if (is_unstructured) {
        result = std::make_unique<UnstructuredGrid>(type, rank, size);
    } else {
        result = std::make_unique<StructuredGrid>(type, rank, size);
    }

    result->initialize_underlying_grid();

    return result;
}


/* Model Implementation */

Model::Model(std::unique_ptr<Bmi_Adapter>&& model, std::string init_config)
  : model_(std::move(model))
  , init_config_(std::move(init_config))
{
    model_->construct_backing_model();
}

void Model::initialize(hook_type prehook, hook_type posthook)
{
    /* Initialize */

    if (prehook != nullptr) {
        prehook(*this);
    }

    model_->Initialize(init_config_);

    if (posthook != nullptr) {
        posthook(*this);
    }

    /* Describe */

    // Lambda that gets a pointer to an existing grid, or constructs a new one.
    // If GetVarGrid is not supported by the model, this returns a `nullptr`.
    const auto get_or_initialize_grid = [this](const std::string& varname) -> const Grid* {
        int grid_id = 0;
    
        try {
            grid_id = model_->GetVarGrid(varname);
        } catch (...) {
            return nullptr;
        }

        if (!grids_.contains(grid_id)) {
            grids_[grid_id] = initialize_grid(*model_, grid_id);
        }

        return grids_[grid_id].get();
    };

    // Inputs //
    input_variables_.reserve(model_->GetInputItemCount());
    for (std::string& input : model_->GetInputVarNames()) {
        // Initialize grid if it doesn't already exist
        const Grid* grid = get_or_initialize_grid(input);

        // Initialize input variable
        input_variables_[input] = initialize_variable(*model_, grid, input);
    }

    // Outputs //
    output_variables_.reserve(model_->GetOutputItemCount());
    for (std::string& output : model_->GetOutputVarNames()) {
        // Initialize grid if it doesn't already exist
        const Grid* grid = get_or_initialize_grid(output);

        // Initialize output variable
        output_variables_[output] = initialize_variable(*model_, grid, output);
    }
}

void Model::update()
{
    assert(model_->initialized());

    model_->Update();
}

void Model::update_until(double time)
{
    assert(model_->initialized());

    model_->UpdateUntil(time);
}

void Model::finalize()
{
    assert(model_->initialized());

    model_->Finalize();
}

std::string Model::name() const
{
    return model_->GetComponentName();
}

boost::span<std::pair<std::string, Variable>> Model::input_variables()
{
    return boost::make_span(
        input_variables_.begin().get_ptr(),
        input_variables_.end().get_ptr()
    );
}

boost::span<std::pair<std::string, Variable>> Model::output_variables()
{
    return boost::make_span(
        output_variables_.begin().get_ptr(),
        output_variables_.end().get_ptr()
    );
}

boost::span<const std::pair<std::string, Variable>> Model::input_variables() const
{
    return boost::make_span(
        input_variables_.begin().get_ptr(),
        input_variables_.end().get_ptr()
    );
}

boost::span<const std::pair<std::string, Variable>> Model::output_variables() const
{
    return boost::make_span(
        output_variables_.begin().get_ptr(),
        output_variables_.end().get_ptr()
    );
}

Variable& Model::input_variable(const std::string& name)
{
    return input_variables_[name];
}

Variable& Model::output_variable(const std::string& name)
{
    return output_variables_[name];
}

Grid& Model::grid(int id)
{
    return *grids_[id];
}

std::ostream& operator<<(std::ostream& stream, const Variable& variable)
{
    using std::to_string;

    constexpr auto max_char = bmi::MAX_VAR_NAME * 2;

    const Grid* grid = variable.grid();
    std::string grid_out = "none";
    if (grid != nullptr) {
        grid_out = to_string(grid->type())
               + ",grid_rank=" + to_string(grid->rank())
               + ",grid_size=" + to_string(grid->size());
    }

    std::string out(max_char, 0);
    const auto size = std::snprintf(&out[0], max_char,
        "%s[align=%zu,len=%zu,grid=%s]",
        to_string(variable.type()).c_str(),
        variable.align(),
        variable.size(),
        grid_out.c_str()
    );

    out = out.substr(0, size);

    stream << out;
    return stream;
}

} // namespace ngen

#include "bmi/Bmi_Cpp_Adapter.hpp"

ngen::Model ngen::Model::make_cpp(
  std::string model_name,
  std::string init_config,
  std::string library_path,
  std::string registration_function
)
{
    return {
        std::make_unique<Bmi_Cpp_Adapter>(
            std::move(model_name),
            std::move(library_path),
            std::move(registration_function)
        ),
        std::move(init_config)
    };
}

#if NGEN_WITH_BMI_C
#include "bmi/Bmi_C_Adapter.hpp"

/* Named constructor functions */
ngen::Model ngen::Model::make_c(
  std::string model_name,
  std::string init_config,
  std::string library_path,
  std::string registration_function
)
{
    return {
        std::make_unique<Bmi_C_Adapter>(
            std::move(model_name),
            std::move(library_path),
            std::move(registration_function)
        ),
        std::move(init_config)
    };
}
#else
ngen::Model ngen::make_c_model(std::string, std::string, std::string, std::string)
{
    throw std::runtime_error{"NGen is not built with C support."};
}

#endif // NGEN_WITH_BMI_C

#if NGEN_WITH_BMI_FORTRAN
#include "bmi/Bmi_Fortran_Adapter.hpp"

ngen::Model ngen::Model::make_fortran(
  std::string model_name,
  std::string init_config,
  std::string library_path,
  std::string registration_function
)
{
    return {
        std::make_unique<Bmi_Fortran_Adapter>(
            std::move(model_name),
            std::move(library_path),
            std::move(registration_function)
        ),
        std::move(init_config)
    };
}
#else
ngen::Model ngen::make_fortran_model(std::string, std::string, std::string, std::string)
{
    throw std::runtime_error{"NGen is not built with Fortran support."};
}
#endif // NGEN_WITH_BMI_FORTRAN

#if NGEN_WITH_PYTHON
#include "bmi/Bmi_Python_Adapter.hpp"

std::string parse_variable_format(const std::string& type)
{
    ngen::Variable::dtype dtype = ngen::parse_variable_type(type);

    switch (dtype) {
        case ngen::Variable::dtype::i32:
            return pybind11::format_descriptor<std::int32_t>::format();
        case ngen::Variable::dtype::i64:
            return pybind11::format_descriptor<std::int64_t>::format();
        case ngen::Variable::dtype::f32:
            return pybind11::format_descriptor<float>::format();
        case ngen::Variable::dtype::f64:
            return pybind11::format_descriptor<double>::format();
    }
}

ngen::Model ngen::Model::make_python(
  std::string model_name,
  std::string init_config,
  std::string classpath
)
{
    return {
        std::make_unique<Bmi_Python_Adapter>(
            std::move(model_name),
            std::move(classpath)
        ),
        std::move(init_config)
    };
}
#else
ngen::Model ngen::make_python_model(std::string, std::string, std::string)
{
    throw std::runtime_error{"NGen is not built with Python support."};
}
#endif // NGEN_WITH_PYTHON
