#include "bmi/Bmi_Cpp_Adapter.hpp"

namespace ngen {

using Adapter = Bmi_Cpp_Adapter;

void Adapter::construct_backing_model()
{
    using registration_func_t = ::bmi::Bmi* (*)();
    auto register_ = reinterpret_cast<registration_func_t>(this->registration());
    ptr_ = std::unique_ptr<bmi::Bmi>{ register_() };
}

void Adapter::Initialize(std::string config_file)
{
    ptr_->Initialize(std::move(config_file));

    Bmi_Adapter::set_initialized();
}

void Adapter::Update()
{
    ptr_->Update();
}

void Adapter::UpdateUntil(double time)
{
    ptr_->UpdateUntil(time);
}

void Adapter::Finalize()
{
    ptr_->Finalize();
}

std::string Adapter::GetComponentName()
{
    return ptr_->GetComponentName();
}

int Adapter::GetInputItemCount()
{
    return ptr_->GetInputItemCount();
}

int Adapter::GetOutputItemCount()
{
    return ptr_->GetOutputItemCount();
}

std::vector<std::string> Adapter::GetInputVarNames()
{
    return ptr_->GetInputVarNames();
}

std::vector<std::string> Adapter::GetOutputVarNames()
{
    return ptr_->GetOutputVarNames();
}

int Adapter::GetVarGrid(std::string name)
{
    return ptr_->GetVarGrid(std::move(name));
}

std::string Adapter::GetVarType(std::string name)
{
    return ptr_->GetVarType(std::move(name));
}

std::string Adapter::GetVarUnits(std::string name)
{
    return ptr_->GetVarUnits(std::move(name));
}

int Adapter::GetVarItemsize(std::string name)
{
    return ptr_->GetVarItemsize(std::move(name));
}

int Adapter::GetVarNbytes(std::string name)
{
    return ptr_->GetVarNbytes(std::move(name));
}

std::string Adapter::GetVarLocation(std::string name)
{
    return ptr_->GetVarLocation(std::move(name));
}

double Adapter::GetCurrentTime()
{
    return ptr_->GetCurrentTime();
}

double Adapter::GetStartTime()
{
    return ptr_->GetStartTime();
}

double Adapter::GetEndTime()
{
    return ptr_->GetEndTime();
}

std::string Adapter::GetTimeUnits()
{
    return ptr_->GetTimeUnits();
}

double Adapter::GetTimeStep()
{
    return ptr_->GetTimeStep();
}

void Adapter::GetValue(std::string name, void *dest)
{
    ptr_->GetValue(std::move(name), dest);
}

void* Adapter::GetValuePtr(std::string name)
{
    return ptr_->GetValuePtr(std::move(name));
}

void Adapter::GetValueAtIndices(std::string name, void *dest, int *inds, int count)
{
    ptr_->GetValueAtIndices(std::move(name), dest, inds, count);
}

void Adapter::SetValue(std::string name, void *src)
{
    ptr_->SetValue(std::move(name), src);
}

void Adapter::SetValueAtIndices(std::string name, int *inds, int count, void *src)
{
    ptr_->SetValueAtIndices(std::move(name), inds, count, src);
}

int Adapter::GetGridRank(const int grid)
{
    return ptr_->GetGridRank(grid);
}

int Adapter::GetGridSize(const int grid)
{
    return ptr_->GetGridSize(grid);
}

std::string Adapter::GetGridType(const int grid)
{
    return ptr_->GetGridType(grid);
}

void Adapter::GetGridShape(const int grid, int *shape)
{
    ptr_->GetGridShape(grid, shape);
}

void Adapter::GetGridSpacing(const int grid, double *spacing)
{
    ptr_->GetGridSpacing(grid, spacing);
}

void Adapter::GetGridOrigin(const int grid, double *origin)
{
    ptr_->GetGridOrigin(grid, origin);
}

void Adapter::GetGridX(const int grid, double *x)
{
    ptr_->GetGridX(grid, x);
}

void Adapter::GetGridY(const int grid, double *y)
{
    ptr_->GetGridY(grid, y);
}

void Adapter::GetGridZ(const int grid, double *z)
{
    ptr_->GetGridZ(grid, z);
}

int Adapter::GetGridNodeCount(const int grid)
{
    return ptr_->GetGridNodeCount(grid);
}

int Adapter::GetGridEdgeCount(const int grid)
{
    return ptr_->GetGridEdgeCount(grid);
}

int Adapter::GetGridFaceCount(const int grid)
{
    return ptr_->GetGridFaceCount(grid);
}

void Adapter::GetGridEdgeNodes(const int grid, int *edge_nodes)
{
    ptr_->GetGridEdgeNodes(grid, edge_nodes);
}

void Adapter::GetGridFaceEdges(const int grid, int *face_edges)
{
    ptr_->GetGridFaceEdges(grid, face_edges);
}

void Adapter::GetGridFaceNodes(const int grid, int *face_nodes)
{
    ptr_->GetGridFaceNodes(grid, face_nodes);
}

void Adapter::GetGridNodesPerFace(const int grid, int *nodes_per_face)
{
    ptr_->GetGridNodesPerFace(grid, nodes_per_face);
}

} // namespace ngen
