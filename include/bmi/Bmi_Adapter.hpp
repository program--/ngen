#pragma once

#include "bmi.hpp"

#define NGEN_BMI_DECLARE_OVERRIDES \
  void Initialize(std::string config_file) override; \
  void Update() override; \
  void UpdateUntil(double time) override; \
  void Finalize() override; \
  std::string GetComponentName() override; \
  int GetInputItemCount() override; \
  int GetOutputItemCount() override; \
  std::vector<std::string> GetInputVarNames() override; \
  std::vector<std::string> GetOutputVarNames() override; \
  int GetVarGrid(std::string name) override; \
  std::string GetVarType(std::string name) override; \
  std::string GetVarUnits(std::string name) override; \
  int GetVarItemsize(std::string name) override; \
  int GetVarNbytes(std::string name) override; \
  std::string GetVarLocation(std::string name) override; \
  double GetCurrentTime() override; \
  double GetStartTime() override; \
  double GetEndTime() override; \
  std::string GetTimeUnits() override; \
  double GetTimeStep() override; \
  void GetValue(std::string name, void *dest) override; \
  void *GetValuePtr(std::string name) override; \
  void GetValueAtIndices(std::string name, void *dest, int *inds, int count) override; \
  void SetValue(std::string name, void *src) override; \
  void SetValueAtIndices(std::string name, int *inds, int count, void *src) override; \
  int GetGridRank(const int grid) override; \
  int GetGridSize(const int grid) override; \
  std::string GetGridType(const int grid) override; \
  void GetGridShape(const int grid, int *shape) override; \
  void GetGridSpacing(const int grid, double *spacing) override; \
  void GetGridOrigin(const int grid, double *origin) override; \
  void GetGridX(const int grid, double *x) override; \
  void GetGridY(const int grid, double *y) override; \
  void GetGridZ(const int grid, double *z) override; \
  int GetGridNodeCount(const int grid) override; \
  int GetGridEdgeCount(const int grid) override; \
  int GetGridFaceCount(const int grid) override; \
  void GetGridEdgeNodes(const int grid, int *edge_nodes) override; \
  void GetGridFaceEdges(const int grid, int *face_edges) override; \
  void GetGridFaceNodes(const int grid, int *face_nodes) override; \
  void GetGridNodesPerFace(const int grid, int *nodes_per_face) override

namespace ngen {

struct Bmi_Adapter : public ::bmi::Bmi
{
    Bmi_Adapter()                   = delete;
    Bmi_Adapter(Bmi_Adapter const&) = delete;
    Bmi_Adapter(Bmi_Adapter&&)      = delete;

    ~Bmi_Adapter() override = default;

    bool initialized() const noexcept;
    const std::string& name() const noexcept;

  protected:
    Bmi_Adapter(std::string model_name);

    void set_initialized() noexcept;

  private:
    virtual void construct_backing_model() = 0;

    friend struct Model;

    bool initialized_;
    std::string model_name_;
};

} // namespace ngen
