#include "ModelFixture.hpp"

#include <utilities/python/InterpreterUtil.hpp>

static std::shared_ptr<utils::ngenPy::InterpreterUtil> gil_;

ModelFixture::ctor_t ModelFixture::make_model_func = [] {
    return ngen::Model::make_python(
        "testbmipythonmodel",
        "test/data/bmi/test_bmi_python/test_bmi_python_config_0.yml",
        "test_bmi_py.bmi_model"
    );
};

ModelFixture::hook_t ModelFixture::setup_func = [] {
    gil_ = utils::ngenPy::InterpreterUtil::getInstance();
    gil_->addToPyPath("extern");
};

ModelFixture::hook_t ModelFixture::teardown_func = nullptr;

TEST_F(ModelFixture, Python) {
    EXPECT_EQ(model().name(), "Test Python model for Next Generation NWM");

    auto outputs = model().output_variables();

    EXPECT_EQ(model().input_variables().size(), 3);
    EXPECT_EQ(outputs.size(), 5);

    ASSERT_NO_THROW(model().update());

    constexpr const char* expected[] = {
        "OUTPUT_VAR_1:f64[align=8,len=1,grid=scalar,grid_rank=0,grid_size=0]",
        "OUTPUT_VAR_2:f64[align=8,len=1,grid=scalar,grid_rank=0,grid_size=0]",
        "OUTPUT_VAR_3:f64[align=8,len=3,grid=scalar,grid_rank=0,grid_size=0]",
        "GRID_VAR_2:f64[align=8,len=0,grid=uniform_rectilinear,grid_rank=2,grid_size=0]",
        "GRID_VAR_3:f64[align=8,len=1,grid=uniform_rectilinear,grid_rank=3,grid_size=0]"
    };

    std::stringstream str;
    for (auto& output : outputs) {
        str << output.first << ':' << output.second;
        EXPECT_NE(std::find(&expected[0], &expected[5], str.str()), &expected[5]);
        str.str(std::string());
    }
}
