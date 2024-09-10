#pragma once

#include <gtest/gtest.h>

#include <bmi/Model.hpp>

struct ModelFixture : public testing::Test
{
    static void SetUpTestSuite()
    {
        if (setup_func != nullptr)
            setup_func();
    
        model().initialize();
    }

    static void TearDownTestSuite()
    {
        model().finalize();

        if (teardown_func != nullptr)
            teardown_func();
    }

    static ngen::Model& model() {
        static ngen::Model model = make_model_func();
        return model;
    }

    using ctor_t = std::function<ngen::Model(void)>;
    static ctor_t make_model_func;

    using hook_t = std::function<void(void)>;
    static hook_t setup_func;
    static hook_t teardown_func;
};
