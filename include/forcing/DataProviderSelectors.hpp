#ifndef NGEN_DATA_SELECTORS_HPP
#define NGEN_DATA_SELECTORS_HPP

#include <chrono>
#include <string>

struct DataSelector
{
    using clock_type = std::chrono::steady_clock;
  
    std::string variable_name;
    clock_type::time_point init_time;
    clock_type::duration duration;
    std::string output_units;
};

struct CatchmentAggrDataSelector
  : public DataSelector
{
    std::string catchment_id; //< the catchment to access data for
};

struct GriddedDataSelector
  : public DataSelector
{};

#endif
