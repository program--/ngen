#pragma once

#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/topological_sort.hpp>

#include "span.hpp"
#include "graph.hpp"
#include "traits.hpp"

namespace ngen {

struct hf_network
{
    //! Bundled Boost.Graph type representing a hydrofabric network.
    using graph_type = hf_graph_type;
    using size_type = std::size_t;
    using descriptor_type = graph_type::vertex_descriptor;

  private:
    struct graph_type_filter;

  public:

    //! Ordering enumerator
    enum class sort_order {
        //! aka https://en.wikipedia.org/wiki/Topological_sorting
        //!
        //! Traversal starts at divides (leaves) and traverses downstream toward terminal nexuses (roots).
        downstream,

        //! aka https://en.wikipedia.org/wiki/Arborescence_(graph_theory) inverse ordering
        //!
        //! Traversal starts at terminal nexuses (roots) and traverses upstream toward divides (leaves).
        upstream
    };

    hf_network();

    auto add_node(const std::string& id, hf_type type, hf_subtype subtype = hf_subtype::none)
      -> descriptor_type;

    auto add_node(const hf_node& node)
      -> descriptor_type;

    auto add_edge(descriptor_type a, descriptor_type b)
      -> std::pair<graph_type::edge_descriptor, bool>;

    auto add_edge(const std::string& a, const std::string& b)
      -> std::pair<graph_type::edge_descriptor, bool>;

    auto node(const std::string& id) const noexcept
      -> const hf_node*;

    auto node(descriptor_type index) const noexcept
      -> const hf_node*;

    auto operator[](const std::string& id) const noexcept
      -> const hf_node*;

    auto operator[](descriptor_type id) const noexcept
      -> const hf_node*;

    auto contains(const std::string& id) const noexcept
      -> bool;

    //! Get the number of nodes in this network.
    //! @return size_type
    auto size() const noexcept
      -> size_type;

    //! Project (filter) a network into a subgraph of itself.
    //!
    //! @tparam Ts Types for the type of nodes to gather
    auto project(const boost::span<const hf_type> types) const noexcept
      -> boost::filtered_graph<graph_type, graph_type_filter, graph_type_filter>;

    //! @copydoc hf_network::project(boost::span<hf_type>)
    auto project(const std::vector<hf_type>& types) const noexcept
      -> boost::filtered_graph<graph_type, graph_type_filter, graph_type_filter>;

    //! @copydoc hf_network::project(boost::span<hf_type>)
    template<
        typename... Ts,
        std::enable_if_t<
            ngen::traits::all_is_same<hf_type, Ts...>::value,
            bool
        > = true
    >
    auto project(Ts... types) const noexcept
      -> boost::filtered_graph<graph_type, graph_type_filter, graph_type_filter>
    {
        return this->project({ types... });
    }

    //! Sort network given an order
    //! @param order Sorting order
    //! @return %vector of descriptor_type
    auto sort(sort_order order) const
      -> std::vector<descriptor_type>;

  private:

    struct graph_type_filter
    {
        graph_type_filter() noexcept;
        graph_type_filter(const graph_type* g, const boost::span<const hf_type> types) noexcept;
        bool operator()(const hf_node& n) const;
        bool operator()(const graph_type::vertex_descriptor v) const;
        bool operator()(const graph_type::edge_descriptor) const;

      private:
        const graph_type* g_;
        std::vector<hf_type> types_;
    };

    graph_type graph_;
};

} // namespace ngen
