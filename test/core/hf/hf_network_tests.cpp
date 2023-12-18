#include "gtest/gtest.h"

#include <array>

#include <hf/network.hpp>
#include <geopackage.hpp>
#include <FileChecker.h>

struct HFNetworkTestFixture : public testing::Test {

    ~HFNetworkTestFixture() override = default;

    void SetUp() override
    {
        for (const auto& node : nodes)
            network.add_node(std::get<0>(node), std::get<1>(node), std::get<2>(node));

        for (const auto& edge : edges)
            network.add_edge(edge.first, edge.second);
    }

    void TearDown() override
    {};

    ngen::hf_network network;

    // NOLINTNEXTLINE
    static constexpr std::tuple<const char*, ngen::hf_type, ngen::hf_subtype> nodes[] = {
        { "cat-11410", ngen::hf_type::divide,   ngen::hf_subtype::none     }, // A
        { "wb-11410",  ngen::hf_type::flowpath, ngen::hf_subtype::none     }, // B
        { "nex-11223", ngen::hf_type::nexus,    ngen::hf_subtype::none     }, // C
        { "cat-11223", ngen::hf_type::divide,   ngen::hf_subtype::none     }, // D
        { "wb-11223",  ngen::hf_type::flowpath, ngen::hf_subtype::none     }, // E
        { "cat-11371", ngen::hf_type::divide,   ngen::hf_subtype::none     }, // F
        { "wb-11371",  ngen::hf_type::flowpath, ngen::hf_subtype::none     }, // G
        { "cat-11509", ngen::hf_type::divide,   ngen::hf_subtype::none     }, // H
        { "wb-11509",  ngen::hf_type::flowpath, ngen::hf_subtype::none     }, // I
        { "nex-11224", ngen::hf_type::nexus,    ngen::hf_subtype::none     }, // J
        { "cat-11224", ngen::hf_type::divide,   ngen::hf_subtype::none     }, // K
        { "wb-11224",  ngen::hf_type::flowpath, ngen::hf_subtype::none     }, // L
        { "tnx-11225", ngen::hf_type::nexus,    ngen::hf_subtype::terminal }, // M
    };

    // NOLINTNEXTLINE
    static constexpr std::pair<const char*, const char*> edges[] = {
        { "cat-11410", "wb-11410" }, // A -> B
        { "wb-11410", "nex-11223" }, // B -> C
        { "nex-11223", "wb-11223" }, // C -> E
        { "cat-11223", "wb-11223" }, // D -> E
        { "cat-11371", "wb-11371" }, // F -> G
        { "cat-11509", "wb-11509" }, // H -> I
        { "wb-11223", "nex-11224" }, // E -> J
        { "wb-11371", "nex-11224" }, // G -> J
        { "wb-11509", "nex-11224" }, // I -> J
        { "nex-11224", "wb-11224" }, // J -> L
        { "cat-11224", "wb-11224" }, // K -> L
        { "wb-11224", "tnx-11225" }, // L -> M
    };
};

constexpr decltype(HFNetworkTestFixture::nodes) HFNetworkTestFixture::nodes;
constexpr decltype(HFNetworkTestFixture::edges) HFNetworkTestFixture::edges;

TEST_F(HFNetworkTestFixture, NetworkConstruction) {
    // Network:
    // ------------------------------------------
    // A -> B -> C ->
    //           D -> E ->
    //           F -> G ->
    //           H -> I -> J ->
    //                     K -> L -> M

    ASSERT_EQ(network.size(), 13);
    EXPECT_TRUE(network.contains("wb-11224"));
    EXPECT_TRUE(network.contains("tnx-11225"));
    EXPECT_TRUE(network.contains("cat-11509"));

    decltype(auto) node = network["wb-11224"];
    ASSERT_FALSE(node == nullptr);
    EXPECT_EQ(node->id, "wb-11224");
    EXPECT_EQ(node->type, ngen::hf_type::flowpath);
    EXPECT_EQ(node->subtype, ngen::hf_subtype::none);

    node = network["tnx-11225"];
    ASSERT_FALSE(node == nullptr);
    EXPECT_EQ(node->id, "tnx-11225");
    EXPECT_EQ(node->type, ngen::hf_type::nexus);
    EXPECT_EQ(node->subtype, ngen::hf_subtype::terminal);

    node = network["fake-12345"];
    ASSERT_TRUE(node == nullptr);
}

TEST_F(HFNetworkTestFixture, NetworkConstructionGpkg)
{
    const auto gpkg = utils::FileChecker::find_first_readable({
        "data/routing/gauge_01073000.gpkg",
        "test/data/routing/gauge_01073000.gpkg"
    });

    const auto divides   = ngen::geopackage::read(gpkg, "divides", {});
    const auto flowpaths = ngen::geopackage::read(gpkg, "flowpaths", {});
    const auto nexuses   = ngen::geopackage::read(gpkg, "nexus", {});

    // std::unordered_map<ngen::hf_node, ngen::hf_node> cache;

    for (const auto& divide : *divides) {
        const auto divide_id = divide->get("divide_id").as_string();
        const auto nexus_id = divide->get("toid").as_string();
        const auto wb_id = divide->get("has_flowline").as_boolean() ? divide->get("id").as_string() : "";

        
    
        network.add_node(divide_id, ngen::hf_type::divide);

        if (!wb_id.empty()) {
            network.add_edge(divide_id, wb_id);
            network.add_edge(wb_id, nexus_id);
        }
    }

    for (const auto& flowpath : *flowpaths) {
        const auto wb_id = flowpath->get("id").as_string();
        const auto nexus_id = flowpath->get("toid").as_string();

        network.add_node(wb_id, ngen::hf_type::flowpath);
        network.add_edge(wb_id, nexus_id);
    }

    for (const auto& nexus : *nexuses) {
        const auto nexus_id = nexus->get("id").as_string();
        const auto wb_id = nexus->get("toid").as_string();
        const auto divide_id = flowpaths->get_feature(wb_id)->get("divide_id").as_string();

        network.add_node(nexus_id, ngen::hf_type::nexus);
    }
}

TEST_F(HFNetworkTestFixture, NetworkDownstreamSort)
{
    const auto order = network.sort(ngen::hf_network::sort_order::upstream);
    
    std::vector<std::string> ids;
    ids.reserve(order.size());

    std::vector<std::string> expected = {
        "cat-11410",
        "wb-11410",
        "nex-11223",
        "wb-11223",
        "cat-11223",
    };

    for (int i = 0; i < order.size(); i++) {
        std::cout << network[order[i]]->id << '\n';
        // EXPECT_EQ(network[order[i]]->id, expected[i]);
    }
    std::cout << std::endl;
}
