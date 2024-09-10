#include "bmi/Bmi_Fortran_Adapter.hpp"

#include <array>

#define BMI_THROW_ON_FAILURE(EXPR, MSG)    \
    do {                                   \
        if ((EXPR) != bmi::BMI_SUCCESS)        \
            throw std::runtime_error{(MSG)}; \
    } while(0)

/**
 * The extern free functions from the Nextgen common Fortran static library for integrating with Fortran BMI modules.
 * 
 * Each declaration corresponds to a free proxy function in the common integration library.  Every proxy function in 
 * turn corresponds to some BMI function, which in the Fortran module is expected to be implemented as a procedure on 
 * some Fortran object.  A proxy function will accept an opaque handle to the Fortran object, along with the other
 * expected arguments to the corresponding BMI function procedure.  The proxy then calls the appropriate procedure on
 * the passed Fortran BMI object.
 *
 * Functions provide Fortran interoperability via iso_c_binding module, and thus appear hear as C functions.
 */
extern "C" {
extern int initialize(void *fortran_bmi_handle, const char *config_file);
extern int update(void *fortran_bmi_handle);
extern int update_until(void *fortran_bmi_handle, double *then);
extern int finalize(void *fortran_bmi_handle);
extern int get_component_name(void *fortran_bmi_handle, char *name);
extern int get_input_item_count(void *fortran_bmi_handle, int *count);
extern int get_output_item_count(void *fortran_bmi_handle, int *count);
extern int get_input_var_names(void *fortran_bmi_handle, char **names);
extern int get_output_var_names(void *fortran_bmi_handle, char **names);
extern int get_var_grid(void *fortran_bmi_handle, const char *name, int *grid);
extern int get_var_type(void *fortran_bmi_handle, const char *name, char *type);
extern int get_var_units(void *fortran_bmi_handle, const char *name, char *units);
extern int get_var_itemsize(void *fortran_bmi_handle, const char *name, int *size);
extern int get_var_nbytes(void *fortran_bmi_handle, const char *name, int *nbytes);
extern int get_var_location(void *fortran_bmi_handle, const char *name, char *location);
extern int get_current_time(void *fortran_bmi_handle, double *time);
extern int get_start_time(void *fortran_bmi_handle, double *time);
extern int get_end_time(void *fortran_bmi_handle, double *time);
extern int get_time_units(void *fortran_bmi_handle, char *units);
extern int get_time_step(void *fortran_bmi_handle, double *time_step);
extern int get_value_int(void *fortran_bmi_handle, const char *name, int *dest);
extern int get_value_float(void *fortran_bmi_handle, const char *name, float *dest);
extern int get_value_double(void *fortran_bmi_handle, const char *name, double *dest);
extern int set_value_int(void *fortran_bmi_handle, const char *name, int *src);
extern int set_value_float(void *fortran_bmi_handle, const char *name, float *src);
extern int set_value_double(void *fortran_bmi_handle, const char *name, double *src);
extern int get_grid_rank(void *fortran_bmi_handle, int *grid, int *rank);
extern int get_grid_size(void *fortran_bmi_handle, int *grid, int *size);
extern int get_grid_type(void *fortran_bmi_handle, int *grid, char *type);
extern int get_grid_shape(void *fortran_bmi_handle, int *grid, int *shape);
extern int get_grid_spacing(void *fortran_bmi_handle, int *grid, double *spacing);
extern int get_grid_origin(void *fortran_bmi_handle, int *grid, double *origin);
extern int get_grid_x(void *fortran_bmi_handle, int *grid, double *x);
extern int get_grid_y(void *fortran_bmi_handle, int *grid, double *y);
extern int get_grid_z(void *fortran_bmi_handle, int *grid, double *z);
extern int get_grid_node_count(void *fortran_bmi_handle, int *grid, int *count);
extern int get_grid_edge_count(void *fortran_bmi_handle, int *grid, int *count);
extern int get_grid_face_count(void *fortran_bmi_handle, int *grid, int *count);
extern int get_grid_edge_nodes(void *fortran_bmi_handle, int *grid, int *edge_nodes);
extern int get_grid_face_edges(void *fortran_bmi_handle, int *grid, int *face_edges);
extern int get_grid_face_nodes(void *fortran_bmi_handle, int *grid, int *face_nodes);
extern int get_grid_nodes_per_face(void *fortran_bmi_handle, int *grid, int *nodes_per_face);

/* The following functions are not currently implemented in the Fortran iso_c_binding middleware, and as such
 * not declared here.  This is not a problem, since it is fairly easy to work around their absence using the primary
 * getter and setter functions.
 *
 * They can be added if needed/possible in the future, but implementations must be added to the middleware before
 * these declarations will be valid.
 */
// extern int get_value_ptr_int(void *fortran_bmi_handle, const char *name, int *dest);
// extern int get_value_ptr_float(void *fortran_bmi_handle, const char *name, float *dest);
// extern int get_value_ptr_double(void *fortran_bmi_handle, const char *name, double *dest);
// extern int get_value_at_indices_int(void *fortran_bmi_handle, const char *name, int *dest, int indices);
// extern int get_value_at_indices_float(void *fortran_bmi_handle, const char *name, float *dest, int indices);
// extern int get_value_at_indices_double(void *fortran_bmi_handle, const char *name, double *dest, int indices);
// extern int set_value_at_indices_int(void *fortran_bmi_handle, const char *name, int indices, int *src);
// extern int set_value_at_indices_float(void *fortran_bmi_handle, const char *name, int indices, float *src);
// extern int set_value_at_indices_double(void *fortran_bmi_handle, const char *name, int indices, double *src);
} // extern "C"

namespace ngen {

using Adapter = Bmi_Fortran_Adapter;

void Adapter::construct_backing_model()
{
    using registration_func_t = void* (*)(void*);
    auto register_ = reinterpret_cast<registration_func_t>(this->registration());
    register_(ptr_);
}

void Adapter::Initialize(std::string config_file)
{
    BMI_THROW_ON_FAILURE(
        initialize(ptr_, config_file.c_str()),
        "Failed to initialize Bmi_Fortran_Adapter with file '" + config_file + "'."
    );

    Bmi_Adapter::set_initialized();
}

void Adapter::Update()
{
    BMI_THROW_ON_FAILURE(
        update(ptr_),
        "Failed to update Bmi_C_Adapter."
    );
}

void Adapter::UpdateUntil(double time)
{
    BMI_THROW_ON_FAILURE(
        update_until(ptr_, &time),
        "Failed to update Bmi_C_Adapter until time '" + std::to_string(time) + "'."
    );
}

void Adapter::Finalize()
{
    finalize(ptr_); // Throw on failure?
}

std::string Adapter::GetComponentName()
{
    std::array<char, bmi::MAX_COMPONENT_NAME> name;

    BMI_THROW_ON_FAILURE(
        get_component_name(ptr_, name.data()),
        "Failed to get component name for Bmi_C_Adapter."
    );

    return { name.data() };
}

int Adapter::GetInputItemCount()
{
    int item_count = 0;

    BMI_THROW_ON_FAILURE(
        get_input_item_count(ptr_, &item_count),
        "Failed to get input item count for Bmi_C_Adapter."
    );

    return item_count;
}

int Adapter::GetOutputItemCount()
{
    int item_count = 0;

    BMI_THROW_ON_FAILURE(
        get_output_item_count(ptr_, &item_count),
        "Failed to get output item count for Bmi_C_Adapter."
    );

    return item_count;
}


std::vector<std::string> GetVarName(void* ptr, int (*func)(void*, char**), int count, bool is_input)
{
    std::vector<char*> names(count);

    for (int i = 0; i < count; ++i)
        names[i] = new char[bmi::MAX_VAR_NAME];

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
    return GetVarName(ptr_, get_input_var_names, GetInputItemCount(), true);
}

std::vector<std::string> Adapter::GetOutputVarNames()
{
    return GetVarName(ptr_, get_output_var_names, GetOutputItemCount(), false);
}

int Adapter::GetVarGrid(std::string name)
{
    int grid_id = -1;
    BMI_THROW_ON_FAILURE(
        get_var_grid(ptr_, name.c_str(), &grid_id),
        "Failed to get var grid for variable " + name + " in Bmi_C_Adapter"
    );
    return grid_id;
}

std::string Adapter::GetVarType(std::string name)
{
    std::array<char, bmi::MAX_TYPE_NAME> var_type;
    BMI_THROW_ON_FAILURE(
        get_var_type(ptr_, name.c_str(), var_type.data()),
        "Failed to get type for variable " + name + " in Bmi_C_Adapter"
    );
    return { var_type.data() };
}

std::string Adapter::GetVarUnits(std::string name)
{
    std::array<char, bmi::MAX_UNITS_NAME> units_name;
    BMI_THROW_ON_FAILURE(
        get_var_units(ptr_, name.c_str(), units_name.data()),
        "Failed to get units for variable " + name + " in Bmi_C_Adapter"
    );
    return { units_name.data() };
}

int Adapter::GetVarItemsize(std::string name)
{
    int item_size = -1;
    BMI_THROW_ON_FAILURE(
        get_var_itemsize(ptr_, name.c_str(), &item_size),
        "Failed to get item size for variable " + name + " in Bmi_C_Adapter"
    );
    return item_size;
}

int Adapter::GetVarNbytes(std::string name)
{
    int nbytes = -1;
    BMI_THROW_ON_FAILURE(
        get_var_nbytes(ptr_, name.c_str(), &nbytes),
        "Failed to get nbytes for variable " + name + " in Bmi_C_Adapter"
    );
    return nbytes;
}

std::string Adapter::GetVarLocation(std::string name)
{
    std::array<char, bmi::MAX_LOCATION_NAME> loc;
    BMI_THROW_ON_FAILURE(
        get_var_location(ptr_, name.c_str(), loc.data()),
        "Failed to get location for variable " + name + " in Bmi_C_Adapter"
    );
    return { loc.data() };
}

double Adapter::GetCurrentTime()
{
    double current = -1;
    BMI_THROW_ON_FAILURE(
        get_current_time(ptr_, &current),
        "Failed to get current time in Bmi_C_Adapter"
    );
    return current;
}

double Adapter::GetStartTime()
{
    double start = -1;
    BMI_THROW_ON_FAILURE(
        get_start_time(ptr_, &start),
        "Failed to get start time in Bmi_C_Adapter"
    );
    return start;
}

double Adapter::GetEndTime()
{
    double end = -1;
    BMI_THROW_ON_FAILURE(
        get_end_time(ptr_, &end),
        "Failed to get end time in Bmi_C_Adapter"
    );
    return end;
}

std::string Adapter::GetTimeUnits()
{
    std::array<char, bmi::MAX_UNITS_NAME> units;
    BMI_THROW_ON_FAILURE(
        get_time_units(ptr_, units.data()),
        "Failed to get time units in Bmi_C_Adapter"
    );
    return { units.data() };
}
double Adapter::GetTimeStep()
{
    double step = -1;
    BMI_THROW_ON_FAILURE(
        get_time_step(ptr_, &step),
        "Failed to get time step in Bmi_C_Adapter"
    );
    return step;
}

void Adapter::GetValue(std::string name, void *dest)
{
    auto type = GetVarType(name);

    int result = -1;
    if (type == "int" || type == "integer") {
        result = get_value_int(ptr_, name.c_str(), static_cast<int*>(dest));
    } else if (type == "float" || type == "real") {
        result = get_value_float(ptr_, name.c_str(), static_cast<float*>(dest));
    } else if (type == "double" || type == "double precision") {
        result = get_value_double(ptr_, name.c_str(), static_cast<double*>(dest));
    } else {
        throw std::runtime_error{"Unsupported type '" + type + "' for variable " + name + " in Bmi_Fortran_Adapter"};
    }

    if (result != bmi::BMI_SUCCESS) {
        throw std::runtime_error{"Failed to get variable " + name + " of type '" + type + "' in Bmi_Fortran_Adapter"};
    }
}

void *Adapter::GetValuePtr(std::string name)
{
    throw std::runtime_error{"Fortran BMI module integration does not currently support getting value pointers"};
}

void Adapter::GetValueAtIndices(std::string name, void *dest, int *inds, int count)
{
    throw std::runtime_error{"Fortran BMI module integration does not currently support getting values by index"};
}

void Adapter::SetValue(std::string name, void *src)
{
    auto type = GetVarType(name);

    int result = -1;
    if (type == "int" || type == "integer") {
        result = set_value_int(ptr_, name.c_str(), static_cast<int*>(src));
    } else if (type == "float" || type == "real") {
        result = set_value_float(ptr_, name.c_str(), static_cast<float*>(src));
    } else if (type == "double" || type == "double precision") {
        result = set_value_double(ptr_, name.c_str(), static_cast<double*>(src));
    } else {
        throw std::runtime_error{"Unsupported type '" + type + "' for variable " + name + " in Bmi_Fortran_Adapter"};
    }

    if (result != bmi::BMI_SUCCESS) {
        throw std::runtime_error{"Failed to set variable " + name + " of type '" + type + "' in Bmi_Fortran_Adapter"};
    }
}

void Adapter::SetValueAtIndices(std::string name, int *inds, int count, void *src)
{
    throw std::runtime_error{"Fortran BMI module integration does not currently support setting values by index"};
}

int Adapter::GetGridRank(int grid)
{
    int grid_rank = 0;
    BMI_THROW_ON_FAILURE(
        get_grid_rank(ptr_, &grid, &grid_rank),
        "Failed to get rank of grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return grid_rank;
}

int Adapter::GetGridSize(int grid)
{
    int grid_size = 0;
    BMI_THROW_ON_FAILURE(
        get_grid_size(ptr_, &grid, &grid_size),
        "Failed to get size of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
    return grid_size;
}

std::string Adapter::GetGridType(int grid)
{
    std::array<char, bmi::MAX_TYPE_NAME> grid_type;
    BMI_THROW_ON_FAILURE(
        get_grid_type(ptr_, &grid, grid_type.data()),
        "Failed to get type of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
    return { grid_type.data() };
}

void Adapter::GetGridShape(int grid, int *shape)
{
    BMI_THROW_ON_FAILURE(
        get_grid_shape(ptr_, &grid, shape),
        "Failed to get shape of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridSpacing(int grid, double *spacing)
{
    BMI_THROW_ON_FAILURE(
        get_grid_spacing(ptr_, &grid, spacing),
        "Failed to get spacing of grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridOrigin(int grid, double *origin)
{
    BMI_THROW_ON_FAILURE(
        get_grid_origin(ptr_, &grid, origin),
        "Failed to get origin for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridX(int grid, double *x)
{
    BMI_THROW_ON_FAILURE(
        get_grid_x(ptr_, &grid, x),
        "Failed to get x for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridY(int grid, double *y)
{
    BMI_THROW_ON_FAILURE(
        get_grid_y(ptr_, &grid, y),
        "Failed to get y for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

void Adapter::GetGridZ(int grid, double *z)
{
    BMI_THROW_ON_FAILURE(
        get_grid_z(ptr_, &grid, z),
        "Failed to get z for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

int Adapter::GetGridNodeCount(int grid)
{
    int count = 0;
    BMI_THROW_ON_FAILURE(
        get_grid_node_count(ptr_, &grid, &count),
        "Failed to get node count for grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return count;
}

int Adapter::GetGridEdgeCount(int grid)
{
    int count = 0;
    BMI_THROW_ON_FAILURE(
        get_grid_edge_count(ptr_, &grid, &count),
        "Failed to get edge count for grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return count;
}

int Adapter::GetGridFaceCount(int grid)
{
    int count = 0;
    BMI_THROW_ON_FAILURE(
        get_grid_face_count(ptr_, &grid, &count),
        "Failed to get face count for grid " + std::to_string(grid) + " in Bmi_C_Adapter" 
    );
    return count;
}

void Adapter::GetGridEdgeNodes(int grid, int *edge_nodes)
{
    BMI_THROW_ON_FAILURE(
        get_grid_edge_nodes(ptr_, &grid, edge_nodes),
        "Failed to get edge nodes for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}
void Adapter::GetGridFaceEdges(int grid, int *face_edges)
{
    BMI_THROW_ON_FAILURE(
        get_grid_face_edges(ptr_, &grid, face_edges),
        "Failed to get face edges for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}
void Adapter::GetGridFaceNodes(int grid, int *face_nodes)
{
    BMI_THROW_ON_FAILURE(
        get_grid_face_nodes(ptr_, &grid, face_nodes),
        "Failed to get face nodes for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}
void Adapter::GetGridNodesPerFace(int grid, int *nodes_per_face)
{
    BMI_THROW_ON_FAILURE(
        get_grid_nodes_per_face(ptr_, &grid, nodes_per_face),
        "Failed to get nodes per face for grid " + std::to_string(grid) + " in Bmi_C_Adapter"
    );
}

} // namespace ngen
