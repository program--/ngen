#include "bmi/Bmi_Python_Adapter.hpp"

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "utilities/python/InterpreterUtil.hpp"

extern std::string parse_variable_format(const std::string& type);

namespace ngen {

using Adapter = Bmi_Python_Adapter;

Adapter::Bmi_Python_Adapter(
    std::string model_name,
    std::string classpath
)
 : Bmi_Adapter(std::move(model_name))
 , python_classpath_(std::move(classpath))
 , model_(pybind11::none{})
{}

void Adapter::construct_backing_model() {
    // <classpath>.<classname>
    // Use +1 for classname to ignore the '.' character
    std::size_t last_delimiter = python_classpath_.find_last_of('.');
    std::string classpath = python_classpath_.substr(0, last_delimiter);
    std::string classname = python_classpath_.substr(last_delimiter + 1);

    pybind11::object cls = utils::ngenPy::InterpreterUtil::getPyModule({ std::move(classpath), std::move(classname) });
    model_ = cls();
}

void Adapter::Initialize(std::string config_file)
{
    model_.attr("initialize")(config_file);

    Bmi_Adapter::set_initialized();
}

void Adapter::Update()
{
    model_.attr("update")();
}

void Adapter::UpdateUntil(double time)
{
    model_.attr("update_until")(time);
}

void Adapter::Finalize()
{
    model_.attr("finalize")();
}

std::string Adapter::GetComponentName()
{
    return model_.attr("get_component_name")().cast<std::string>();
}

int Adapter::GetInputItemCount()
{
    return model_.attr("get_input_item_count")().cast<int>();
}

int Adapter::GetOutputItemCount()
{
    return model_.attr("get_output_item_count")().cast<int>();
}

std::vector<std::string> Adapter::GetInputVarNames()
{
    return model_.attr("get_input_var_names")().cast<std::vector<std::string>>();
}

std::vector<std::string> Adapter::GetOutputVarNames()
{
    return model_.attr("get_output_var_names")().cast<std::vector<std::string>>();
}

int Adapter::GetVarGrid(std::string name)
{
    return model_.attr("get_var_grid")(name).cast<int>();
}

std::string Adapter::GetVarType(std::string name)
{
    return model_.attr("get_var_type")(name).cast<std::string>();
}

std::string Adapter::GetVarUnits(std::string name)
{
    return model_.attr("get_var_units")(name).cast<std::string>();
}

int Adapter::GetVarItemsize(std::string name)
{
    return model_.attr("get_var_itemsize")(name).cast<int>();
}

int Adapter::GetVarNbytes(std::string name)
{
    return model_.attr("get_var_nbytes")(name).cast<int>();
}

std::string Adapter::GetVarLocation(std::string name)
{
    return model_.attr("get_var_location")(name).cast<std::string>();
}

double Adapter::GetCurrentTime()
{
    return model_.attr("get_current_time")().cast<double>();
}

double Adapter::GetStartTime()
{
    return model_.attr("get_start_time")().cast<double>();
}

double Adapter::GetEndTime()
{
    return model_.attr("get_end_time")().cast<double>();
}

std::string Adapter::GetTimeUnits()
{
    return model_.attr("get_time_units")().cast<std::string>();
}

double Adapter::GetTimeStep()
{
    return model_.attr("get_time_step")().cast<double>();
}

void Adapter::GetValue(std::string name, void *dest)
{
    const int itemsize = GetVarItemsize(name);
    const int size = GetVarNbytes(name);

    py::buffer_info buffer{dest, itemsize, parse_variable_format(GetVarType(name)), itemsize / size};
    py::array dest_array{buffer};
    model_.attr("get_value")(name, dest_array);
}

void* Adapter::GetValuePtr(std::string name)
{
    return model_.attr("get_value_ptr")(name).cast<py::array>().request().ptr;
}

void Adapter::GetValueAtIndices(std::string name, void *dest, int *inds, int count)
{
    const int itemsize = GetVarItemsize(name);
    py::array dest_array { py::buffer_info{dest, itemsize, parse_variable_format(GetVarType(name)), count } };
    py::array inds_array { py::buffer_info{inds, count} };
    model_.attr("get_value_at_indices")(name, dest_array, inds_array);
}

void Adapter::SetValue(std::string name, void *src)
{
    const int itemsize = GetVarItemsize(name);
    const int size = GetVarNbytes(name);
    py::array src_array{ py::buffer_info{src, itemsize, parse_variable_format(GetVarType(name)), size } };
    model_.attr("set_value")(name, src_array);
}

void Adapter::SetValueAtIndices(std::string name, int *inds, int count, void *src)
{
    const int itemsize = GetVarItemsize(name);
    py::array src_array { py::buffer_info{src, itemsize, parse_variable_format(GetVarType(name)), count } };
    py::array inds_array { py::buffer_info{inds, count} };
    model_.attr("set_value_at_indices")(name, inds_array, src_array);
}

int Adapter::GetGridRank(const int grid)
{
    return model_.attr("get_grid_rank")(grid).cast<int>();
}

int Adapter::GetGridSize(const int grid)
{
    return model_.attr("get_grid_size")(grid).cast<int>();
}

std::string Adapter::GetGridType(const int grid)
{
    return model_.attr("get_grid_type")(grid).cast<std::string>();
}

void Adapter::GetGridShape(const int grid, int *shape)
{
    py::array_t<int> py_shape(GetGridRank(grid));
    model_.attr("get_grid_shape")(grid, py_shape);
    std::copy(py_shape.data(), py_shape.data() + py_shape.size(), shape);
}

void Adapter::GetGridSpacing(const int grid, double *spacing)
{
    py::array_t<double> py_spacing(GetGridRank(grid));
    model_.attr("get_grid_spacing")(grid, py_spacing);
    std::copy(py_spacing.data(), py_spacing.data() + py_spacing.size(), spacing);
}

void Adapter::GetGridOrigin(const int grid, double *origin)
{
    py::array_t<double> py_origin(GetGridRank(grid));
    model_.attr("get_grid_rank")(grid, py_origin);
    std::copy(py_origin.data(), py_origin.data() + py_origin.size(), origin);
}

void Adapter::GetGridXYZ(const int grid, double* coords, int index)
{
    const auto rank = GetGridRank(grid);
    std::vector<int> shape(rank);
    GetGridShape(grid, shape.data());

    py::function get_grid_;

    switch(index) {
        case 0:
            get_grid_ = model_.attr("get_grid_x");
            break;
        case 1:
            get_grid_ = model_.attr("get_grid_y");
            break;
        case 2:
            get_grid_ = model_.attr("get_grid_z");
            break;
    }

    py::array_t<double> py_coords(shape[rank - index - 1]);
    get_grid_(grid, py_coords);
    std::copy(py_coords.data(), py_coords.data() + py_coords.size(), coords);
}

void Adapter::GetGridX(const int grid, double *x)
{
    GetGridXYZ(grid, x, 0);
}

void Adapter::GetGridY(const int grid, double *y)
{
    GetGridXYZ(grid, y, 1);
}

void Adapter::GetGridZ(const int grid, double *z)
{
    GetGridXYZ(grid, z, 2);
}

int Adapter::GetGridNodeCount(const int grid)
{
    return model_.attr("get_grid_node_count")(grid).cast<int>();
}

int Adapter::GetGridEdgeCount(const int grid)
{
    return model_.attr("get_grid_edge_count")(grid).cast<int>();
}

int Adapter::GetGridFaceCount(const int grid)
{
    return model_.attr("get_grid_face_count")(grid).cast<int>();
}

void Adapter::GetGridEdgeNodes(const int grid, int *edge_nodes)
{
    const int count = 2 * GetGridEdgeCount(grid);
    py::array_t<int> nodes(count);
    model_.attr("get_grid_edge_nodes")(grid, nodes);
    std::copy(nodes.data(), nodes.data() + nodes.size(), edge_nodes);
}

void Adapter::GetGridFaceEdges(const int grid, int *face_edges)
{
    const int count = GetGridFaceCount(grid);
    std::vector<int> nodes_per_face(count);
    GetGridNodesPerFace(grid, nodes_per_face.data());
    const int sum = std::accumulate(nodes_per_face.begin(), nodes_per_face.end(), 0);

    py::array_t<int> edges(sum);
    model_.attr("get_grid_face_edges")(grid, edges);
    std::copy(edges.data(), edges.data() + edges.size(), face_edges);
}

void Adapter::GetGridFaceNodes(const int grid, int *face_nodes)
{
    const int count = GetGridFaceCount(grid);
    std::vector<int> nodes_per_face(count);
    GetGridNodesPerFace(grid, nodes_per_face.data());
    const int sum = std::accumulate(nodes_per_face.begin(), nodes_per_face.end(), 0);

    py::array_t<int> nodes(sum);
    model_.attr("get_grid_face_nodes")(grid, nodes);
    std::copy(nodes.data(), nodes.data() + nodes.size(), face_nodes);
}

void Adapter::GetGridNodesPerFace(const int grid, int *nodes_per_face)
{
    const int count = GetGridFaceCount(grid);
    py::array_t<int> nodes(count);
    model_.attr("get_grid_nodes_per_face")(grid, nodes);
    std::copy(nodes.data(), nodes.data() + nodes.size(), nodes_per_face);
}


} // namespace ngen
