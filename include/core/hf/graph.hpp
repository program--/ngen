#pragma once

#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/subgraph.hpp>

namespace ngen {

enum class hf_type
{
    unknown,
    divide,
    nexus,
    flowpath,
    flowline
};

enum class hf_subtype
{
    none,
    network,
    internal,
    terminal,
    coastal
};

struct hf_node
{
    hf_type     type{};
    hf_subtype  subtype{};
    std::string id;

    hf_node() = default;

    hf_node(
        std::string&& id,
        hf_type type = hf_type::unknown,
        hf_subtype subtype = hf_subtype::none
    )
      : type(type)
      , subtype(subtype)
      , id(std::move(id)){};

    hf_node(
        const std::string& id,
        hf_type type = hf_type::unknown,
        hf_subtype subtype = hf_subtype::none
    )
      : type(type)
      , subtype(subtype)
      , id(id){};
};

struct hf_domain
{};

using hf_graph_type = boost::labeled_graph<
    boost::adjacency_list<
        boost::vecS,        // edge list type
        boost::vecS,        // vertex list type
        boost::directedS,   // directedness
        hf_node,            // vertex properties
        boost::no_property, // edge properties
        hf_domain           // graph properties
    >,
    std::string // label type
>;

} // namespace ngen

// ============================================================================

namespace boost {

template<>
struct graph::internal_vertex_name<ngen::hf_node>
{
    struct type {
        using result_type = decltype(ngen::hf_node::id);
        const auto& operator()(const ngen::hf_node& prop) const
        {
            return prop.id;
        }
    };
};

template<>
struct graph::internal_vertex_constructor<ngen::hf_node>
{
    struct type {
        auto operator()(const std::string& id) const
        {
            return ngen::hf_node{id, ngen::hf_type::unknown, ngen::hf_subtype::none};
        }

        auto operator()(
            const std::string& id,
            ngen::hf_type type = ngen::hf_type::unknown,
            ngen::hf_subtype subtype = ngen::hf_subtype::none
        )
        {
            return ngen::hf_node{id, type, subtype};
        }
    };
};

} // namespace boost
