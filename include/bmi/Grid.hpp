#pragma once

#include <cstddef>

namespace ngen {

enum class GridType : std::size_t
{
    scalar,
    points,
    vector,
    unstructured,
    structured_quadrilateral,
    rectilinear,
    uniform_rectilinear
};


struct Grid
{
    virtual ~Grid() = default;

    GridType type() const noexcept
    {
        return type_;
    }

    std::size_t rank() const noexcept
    {
        return rank_;
    }

    std::size_t size() const noexcept
    {
        return size_;
    }

    bool is_unstructured()
    {
        return type_ != GridType::structured_quadrilateral &&
               type_ != GridType::rectilinear &&
               type_ != GridType::uniform_rectilinear;
    }

    bool is_structured()
    {
        return !is_unstructured();
    }

    virtual void initialize_underlying_grid() = 0;

  protected:
    Grid(GridType type, int rank, int size)
      : type_(type)
      , rank_(rank)
      , size_(size)
    {}

  private:
    GridType type_;
    int      rank_;
    int      size_;
};

struct StructuredGrid final : public Grid
{
    StructuredGrid(GridType type, int rank, int size)
      : Grid(type, rank, size)
    {}

    ~StructuredGrid() override = default;

    void initialize_underlying_grid() override {}

    // get_grid_shape
    // get_grid_spacing (uniform_rectilinear)
    // get_grid_origin (uniform_rectilinear)
    // get_grid_x (-uniform_rectilinear? Docs are ambiguous)
    // get_grid_y (^)
    // get_grid_z (^)
};

struct UnstructuredGrid final : public Grid
{
    UnstructuredGrid(GridType type, int rank, int size)
      : Grid(type, rank, size)
    {}

    ~UnstructuredGrid() override = default;

    void initialize_underlying_grid() override {}

    // get_grid_x
    // get_grid_y
    // get_grid_z
    // get_grid_node_count
    // get_grid_edge_count
    // get_grid_face_count
    // get_grid_edge_nodes
    // get_grid_face_edges
    // get_grid_face_nodes
    // get_grid_nodes_per_face
};

} // namespace ngen
