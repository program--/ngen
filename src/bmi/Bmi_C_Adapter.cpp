#include "bmi/Bmi_C_Adapter.hpp"
#include "Bmi_Adapter.hpp"

#include <array>

#define BMI_THROW_ON_FAILURE(EXPR, MSG)    \
    do {                                   \
        if ((EXPR) != BMI_SUCCESS)        \
            throw std::runtime_error{(MSG)}; \
    } while(0)

namespace ngen {

using Adapter = Bmi_C_Adapter;

void Adapter::construct_backing_model()
{
    using registration_func_t = detail::CBmi* (*)(detail::CBmi*);
    auto register_ = reinterpret_cast<registration_func_t>(this->registration());
    ptr_ = std::make_unique<detail::CBmi>();
    register_(ptr_.get());
}

void Adapter::Initialize(std::string config_file)
{
    BMI_THROW_ON_FAILURE(
        ptr_->initialize(ptr_.get(), config_file.c_str()),
        "Failed to initialize Bmi_C_Adapter with file '" + config_file + "'."
    );

    Bmi_Adapter::set_initialized();
}

void Adapter::Update()
{
    BMI_THROW_ON_FAILURE(
        ptr_->update(ptr_.get()),
        "Failed to update Bmi_C_Adapter."
    );
}

void Adapter::UpdateUntil(double time)
{
    BMI_THROW_ON_FAILURE(
        ptr_->update_until(ptr_.get(), time),
        "Failed to update Bmi_C_Adapter until time '" + std::to_string(time) + "'."
    );
}

void Adapter::Finalize()
{
    ptr_->finalize(ptr_.get()); // Throw on failure?
}

std::string Adapter::GetComponentName()
{
    std::array<char, BMI_MAX_COMPONENT_NAME> name;

    BMI_THROW_ON_FAILURE(
        ptr_->get_component_name(ptr_.get(), name.data()),
        "Failed to get component name for Bmi_C_Adapter."
    );

    return { name.data() };
}

int Adapter::GetInputItemCount()
{
    int item_count = 0;

    BMI_THROW_ON_FAILURE(
        ptr_->get_input_item_count(ptr_.get(), &item_count),
        "Failed to get input item count for Bmi_C_Adapter."
    );

    return item_count;
}

int Adapter::GetOutputItemCount()
{
    int item_count = 0;

    BMI_THROW_ON_FAILURE(
        ptr_->get_output_item_count(ptr_.get(), &item_count),
        "Failed to get output item count for Bmi_C_Adapter."
    );

    return item_count;
}


std::vector<std::string> GetVarName(detail::CBmi* ptr, int (*func)(detail::CBmi*, char**), int count, bool is_input)
{
    std::vector<char*> names(count);

    for (int i = 0; i < count; ++i)
        names[i] = new char[BMI_MAX_VAR_NAME];

    BMI_THROW_ON_FAILURE(
        func(ptr, names.data()),
        "Failed to get " + std::string{is_input ? "input" : "output"} + " variable names for Bmi_C_Adapter"
    );

    std::vector<std::string> result;
    result.reserve(count);

    for (int i = 0; i < count; ++i) {
        result.emplace_back(names[i]);
        delete[] names[i];
    }

    return result;
}

std::vector<std::string> Adapter::GetInputVarNames()
{
    return GetVarName(ptr_.get(), ptr_->get_input_var_names, GetInputItemCount(), true);
}

std::vector<std::string> Adapter::GetOutputVarNames()
{
    return GetVarName(ptr_.get(), ptr_->get_output_var_names, GetOutputItemCount(), false);
}

int Adapter::GetVarGrid(std::string name)
{
    int grid_id = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_var_grid(ptr_.get(), name.c_str(), &grid_id),
        "Failed to get var grid for variable " + name + " in Bmi_C_Adapter"
    );
    return grid_id;
}

std::string Adapter::GetVarType(std::string name)
{
    std::array<char, BMI_MAX_TYPE_NAME> var_type;
    BMI_THROW_ON_FAILURE(
        ptr_->get_var_type(ptr_.get(), name.c_str(), var_type.data()),
        "Failed to get type for variable " + name + " in Bmi_C_Adapter"
    );
    return { var_type.data() };
}

std::string Adapter::GetVarUnits(std::string name)
{
    std::array<char, BMI_MAX_UNITS_NAME> units_name;
    BMI_THROW_ON_FAILURE(
        ptr_->get_var_units(ptr_.get(), name.c_str(), units_name.data()),
        "Failed to get units for variable " + name + " in Bmi_C_Adapter"
    );
    return { units_name.data() };
}

int Adapter::GetVarItemsize(std::string name)
{
    int item_size = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_var_itemsize(ptr_.get(), name.c_str(), &item_size),
        "Failed to get item size for variable " + name + " in Bmi_C_Adapter"
    );
    return item_size;
}

int Adapter::GetVarNbytes(std::string name)
{
    int nbytes = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_var_nbytes(ptr_.get(), name.c_str(), &nbytes),
        "Failed to get nbytes for variable " + name + " in Bmi_C_Adapter"
    );
    return nbytes;
}

std::string Adapter::GetVarLocation(std::string name)
{
    std::array<char, BMI_MAX_LOCATION_NAME> loc;
    BMI_THROW_ON_FAILURE(
        ptr_->get_var_location(ptr_.get(), name.c_str(), loc.data()),
        "Failed to get location for variable " + name + " in Bmi_C_Adapter"
    );
    return { loc.data() };
}

double Adapter::GetCurrentTime()
{
    double current = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_current_time(ptr_.get(), &current),
        "Failed to get current time in Bmi_C_Adapter"
    );
    return current;
}

double Adapter::GetStartTime()
{
    double start = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_start_time(ptr_.get(), &start),
        "Failed to get start time in Bmi_C_Adapter"
    );
    return start;
}

double Adapter::GetEndTime()
{
    double end = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_end_time(ptr_.get(), &end),
        "Failed to get end time in Bmi_C_Adapter"
    );
    return end;
}

std::string Adapter::GetTimeUnits()
{
    std::array<char, BMI_MAX_UNITS_NAME> units;
    BMI_THROW_ON_FAILURE(
        ptr_->get_time_units(ptr_.get(), units.data()),
        "Failed to get time units in Bmi_C_Adapter"
    );
    return { units.data() };
}
double Adapter::GetTimeStep()
{
    double step = -1;
    BMI_THROW_ON_FAILURE(
        ptr_->get_time_step(ptr_.get(), &step),
        "Failed to get time step in Bmi_C_Adapter"
    );
    return step;
}

void Adapter::GetValue(std::string name, void *dest)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_value(ptr_.get(), name.c_str(), dest),
        "Failed to get value for variable " + name + " in Bmi_C_Adapter"
    );
}

void *Adapter::GetValuePtr(std::string name)
{
    void* dest = nullptr;
    BMI_THROW_ON_FAILURE(
        ptr_->get_value_ptr(ptr_.get(), name.c_str(), &dest),
        "Failed to get value pointer for variable " + name + " in Bmi_C_Adapter"
    );
    return dest;
}

void Adapter::GetValueAtIndices(std::string name, void *dest, int *inds, int count)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_value_at_indices(ptr_.get(), name.c_str(), dest, inds, count),
        "Failed to get value at indices for variable " + name + " in Bmi_C_Adapter"
    );
}

void Adapter::SetValue(std::string name, void *src)
{
    BMI_THROW_ON_FAILURE(
        ptr_->set_value(ptr_.get(), name.c_str(), src),
        "Failed to set value for variable " + name + " in Bmi_C_Adapter"
    );
}

void Adapter::SetValueAtIndices(std::string name, int *inds, int count, void *src)
{
    BMI_THROW_ON_FAILURE(
        ptr_->set_value_at_indices(ptr_.get(), name.c_str(), inds, count, src),
        "Failed to set value at indices for variable " + name + " in Bmi_C_Adapter"
    );
}

int Adapter::GetGridRank(const int grid)
{
    int grid_rank = 0;
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_rank(ptr_.get(), grid, &grid_rank),
        "Failed to get rank of grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return grid_rank;
}

int Adapter::GetGridSize(const int grid)
{
    int grid_size = 0;
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_size(ptr_.get(), grid, &grid_size),
        "Failed to get size of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
    return grid_size;
}

std::string Adapter::GetGridType(const int grid)
{
    std::array<char, BMI_MAX_TYPE_NAME> grid_type;
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_type(ptr_.get(), grid, grid_type.data()),
        "Failed to get type of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
    return { grid_type.data() };
}

void Adapter::GetGridShape(const int grid, int *shape)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_shape(ptr_.get(), grid, shape),
        "Failed to get shape of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridSpacing(const int grid, double *spacing)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_spacing(ptr_.get(), grid, spacing),
        "Failed to get spacing of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridOrigin(const int grid, double *origin)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_origin(ptr_.get(), grid, origin),
        "Failed to get origin for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridX(const int grid, double *x)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_x(ptr_.get(), grid, x),
        "Failed to get x for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridY(const int grid, double *y)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_y(ptr_.get(), grid, y),
        "Failed to get y for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridZ(const int grid, double *z)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_z(ptr_.get(), grid, z),
        "Failed to get z for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

int Adapter::GetGridNodeCount(const int grid)
{
    int count = 0;
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_node_count(ptr_.get(), grid, &count),
        "Failed to get node count for grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return count;
}

int Adapter::GetGridEdgeCount(const int grid)
{
    int count = 0;
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_edge_count(ptr_.get(), grid, &count),
        "Failed to get edge count for grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return count;
}

int Adapter::GetGridFaceCount(const int grid)
{
    int count = 0;
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_face_count(ptr_.get(), grid, &count),
        "Failed to get face count for grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return count;
}

void Adapter::GetGridEdgeNodes(const int grid, int *edge_nodes)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_edge_nodes(ptr_.get(), grid, edge_nodes),
        "Failed to get edge nodes for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}
void Adapter::GetGridFaceEdges(const int grid, int *face_edges)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_face_edges(ptr_.get(), grid, face_edges),
        "Failed to get face edges for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}
void Adapter::GetGridFaceNodes(const int grid, int *face_nodes)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_face_nodes(ptr_.get(), grid, face_nodes),
        "Failed to get face nodes for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}
void Adapter::GetGridNodesPerFace(const int grid, int *nodes_per_face)
{
    BMI_THROW_ON_FAILURE(
        ptr_->get_grid_nodes_per_face(ptr_.get(), grid, nodes_per_face),
        "Failed to get nodes per face for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

} // namespace ngen
