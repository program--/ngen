#include "ModelFixture.hpp"

ModelFixture::ctor_t ModelFixture::make_model_func = [] {
    return ngen::Model::make_cpp(
        "testbmicppmodel",
        "test/data/bmi/test_bmi_cpp/test_bmi_cpp_config_0.txt",
        "extern/test_bmi_cpp/cmake_build/libtestbmicppmodel.so",
        "bmi_model_create"
    );
};

ModelFixture::hook_t ModelFixture::setup_func    = nullptr;
ModelFixture::hook_t ModelFixture::teardown_func = nullptr;


TEST_F(ModelFixture, Cpp) {

    EXPECT_EQ(model().name(), "Testing BMI C++ Model");

    EXPECT_EQ(model().input_variables().size(), 2);
    EXPECT_EQ(model().output_variables().size(), 3);


    auto outputs = model().output_variables();

    constexpr const char* expected[] = {
        "OUTPUT_VAR_1:f64[align=8,len=1,grid=none]",
        "OUTPUT_VAR_2:f64[align=8,len=1,grid=none]",
        "OUTPUT_VAR_3:f64[align=8,len=3,grid=none]"
    };

    std::stringstream str;
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        str << outputs[i].first << ':' << outputs[i].second;
        EXPECT_EQ(str.str(), expected[i]);
        str.str(std::string());
    }

    model().update();
    
}
