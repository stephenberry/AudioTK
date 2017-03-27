/**
 * \file FIRFilter.h
 */

#ifndef ATK_EQ_FIRFILTER_H
#define ATK_EQ_FIRFILTER_H

#include <cassert>
#include <vector>

#include <ATK/config.h>
#include "config.h"

namespace ATK
{
  /**
   * FIR filter template class
   */
  template<class Coefficients >
  class ATK_EQ_EXPORT FIRFilter: public Coefficients
  {
  public:
    typedef Coefficients Parent;
    using typename Parent::DataType;
    using Parent::converted_inputs_size;
    using Parent::outputs_size;
    using Parent::converted_inputs;
    using Parent::outputs;
    using Parent::coefficients_in;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
    
    using Parent::in_order;
    using Parent::input_delay;
    using Parent::setup;
    
  public:
    void setup()
    {
      Parent::setup();
      input_delay = in_order;
    }
    
    virtual void process_impl(std::size_t size) const override
    {
      assert(input_sampling_rate == output_sampling_rate);
      assert(nb_input_ports == nb_output_ports);
      assert(coefficients_in.data());

      const DataType* ATK_RESTRICT coefficients_in_ptr = coefficients_in.data();

      for (unsigned int channel = 0; channel < nb_input_ports; ++channel)
      {
        const DataType* ATK_RESTRICT input = converted_inputs[channel] - static_cast<int64_t>(in_order);
        DataType* ATK_RESTRICT output = outputs[channel];

        for (std::size_t i = 0; i < size; ++i)
        {
          output[i] = 0;
        }

        for (unsigned int j = 0; j < in_order + 1; ++j)
        {
          for (std::size_t i = 0; i < size; ++i)
          {
            output[i] += coefficients_in_ptr[j] * input[i + j];
          }
        }
      }

    }
    
    const std::vector<DataType>& get_coefficients_in() const
    {
      return coefficients_in;
    }
  };
  
}

#endif
