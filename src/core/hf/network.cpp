#include "hf/network.hpp"

namespace ngen {

hf_network::graph_type_filter::graph_type_filter() noexcept
  : g_(nullptr){};

hf_network::graph_type_filter::graph_type_filter(
    const graph_type* g,
    const boost::span<const hf_type> types
) noexcept
  : g_(g)
  , types_(types.begin(), types.end()){};

bool hf_network::graph_type_filter::operator()(const hf_node& n) const
{
    for (const auto type : types_)
        if (n.type == type)
            return true;

    return false;
}

bool hf_network::graph_type_filter::operator()(const graph_type::vertex_descriptor v) const
{
    return this->operator()(g_->graph()[v]);
}

bool hf_network::graph_type_filter::operator()(const graph_type::edge_descriptor) const
{
    return true;
}

hf_network::hf_network() = default;

auto hf_network::add_node(
    const std::string& id,
    hf_type type,
    hf_subtype subtype
)
  -> descriptor_type
{
    return add_node({ id, type, subtype });
}

auto hf_network::add_node(const hf_node& node)
  -> descriptor_type
{
    return graph_.add_vertex(node.id, node);
}

auto hf_network::add_edge(descriptor_type a, descriptor_type b)
  -> std::pair<graph_type::edge_descriptor, bool>
{
    return boost::add_edge(a, b, graph_);
}

auto hf_network::add_edge(const std::string& a, const std::string& b)
  -> std::pair<graph_type::edge_descriptor, bool>
{
    return boost::add_edge_by_label(a, b, graph_);
}

auto hf_network::node(const std::string& id) const noexcept
  -> const hf_node*
{
    return graph_.vertex(id) == graph_type::null_vertex() ? nullptr : &(graph_[id]);
}

auto hf_network::node(descriptor_type index) const noexcept
  -> const hf_node*
{
    return &graph_.graph()[index];
}

auto hf_network::operator[](const std::string& id) const noexcept
  -> const hf_node*
{
    return node(id);
}

auto hf_network::operator[](descriptor_type id) const noexcept
  -> const hf_node*
{
    return node(id);
}

auto hf_network::contains(const std::string& id) const noexcept
  -> bool
{
    return node(id) != nullptr;
}

auto hf_network::size() const noexcept
  -> size_type
{
    return graph_.graph().m_vertices.size();
}

auto hf_network::project(const boost::span<const hf_type> types) const noexcept
  -> boost::filtered_graph<graph_type, graph_type_filter, graph_type_filter>
{
    return boost::filtered_graph<
        graph_type,
        graph_type_filter,
        graph_type_filter
    >{
        graph_,
        graph_type_filter{&graph_, types}
    };
}

auto hf_network::project(const std::vector<hf_type>& types) const noexcept
  -> boost::filtered_graph<graph_type, graph_type_filter, graph_type_filter>
{
    return project(boost::span<const hf_type>{types});
}

auto hf_network::sort(sort_order order) const
    -> std::vector<descriptor_type>
{
    if (order == sort_order::downstream) {
        // it's faster to use a deque + front_inserter here
        // than to push into a vector and return the reverse
        std::deque<descriptor_type> result;
        boost::topological_sort(graph_, std::front_inserter(result));
        return { result.begin(), result.end() };
    } else if (order == sort_order::upstream) {
        std::vector<descriptor_type> result;
        boost::topological_sort(graph_, std::back_inserter(result));
        return result;
    } else {
        throw std::runtime_error{"Invalid order given to hf_network::sort()"};
    }
}

} // namespace ngen

