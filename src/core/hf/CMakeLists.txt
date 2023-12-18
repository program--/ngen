add_library(core_hf network.cpp)
add_library(NGen::core_hf ALIAS core_hf)
target_include_directories(core_hf
    PUBLIC
        "${NGEN_INC_DIR}/core"
        "${NGEN_INC_DIR}/utilities"
)
target_link_libraries(core_hf PUBLIC Boost::boost)
