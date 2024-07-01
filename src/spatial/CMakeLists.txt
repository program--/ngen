
add_library(spatial)
add_library(NGen::spatial ALIAS spatial)
target_include_directories(spatial PUBLIC ${NGEN_INC_DIR})
target_link_libraries(spatial PUBLIC NGen::config_header Boost::boost)

if(NGEN_WITH_SQLITE)
    target_sources(spatial
      PRIVATE
        io/ngen_sqlite/ngen_sqlite.hpp
        io/ngen_sqlite/ngen_sqlite.cpp
    )

    target_link_libraries(spatial PUBLIC sqlite3)
endif()

target_sources(spatial
    PRIVATE
        Geometry.cpp
        Feature.cpp
        FeatureCollection.cpp
        algorithm/project.cpp
        io/geojson.cpp
        io/wkb.cpp
        io/geopackage.cpp
)
