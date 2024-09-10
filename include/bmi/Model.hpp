#pragma once

#include "bmi/Bmi_Adapter.hpp"
#include "bmi/Grid.hpp"
#include "bmi/Variable.hpp"

#include <functional>
#include <memory>

#include <boost/container/flat_map.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/core/span.hpp>

namespace ngen {

struct Model
{
  using hook_type = std::function<void(Model&)>;

  //! @param prehook Function called before initializing the model
  //! @param posthook Function called after initializing the model
  void initialize(hook_type prehook = nullptr, hook_type posthook = nullptr);

  void update();

  void update_until(double time);

  void finalize();

  std::string name() const;

  boost::span<std::pair<std::string, Variable>> input_variables();
  boost::span<std::pair<std::string, Variable>> output_variables();

  boost::span<const std::pair<std::string, Variable>> input_variables() const;
  boost::span<const std::pair<std::string, Variable>> output_variables() const;

  Variable& input_variable(const std::string& name);
  Variable& output_variable(const std::string& name);
  Grid& grid(int id);

  private:
    /* Private constructors */
    Model() = default;
    Model(std::unique_ptr<Bmi_Adapter>&& model, std::string init_config);

    /* Private member variables */
    std::unique_ptr<Bmi_Adapter> model_ = nullptr;
    std::string init_config_ = {};

    boost::container::flat_map<std::string, Variable> input_variables_ = {};
    boost::container::flat_map<std::string, Variable> output_variables_ = {};
    boost::container::flat_map<int, std::unique_ptr<Grid>> grids_ = {};

  // ==========================================================================
  public:
    /* Named constructor functions */
    static Model make_c(
      std::string model_name,
      std::string init_config,
      std::string library_path,
      std::string registration_function
    );

    static Model make_cpp(
      std::string model_name,
      std::string init_config,
      std::string library_path,
      std::string registration_function
    );
  
    static Model make_fortran(
      std::string model_name,
      std::string init_config,
      std::string library_path,
      std::string registration_function
    );

    static Model make_python(
      std::string model_name,
      std::string init_config,
      std::string classpath
    );
};

} // namespace ngen
