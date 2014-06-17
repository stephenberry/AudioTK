/**
 * \file ToneStackFilter.cpp
 */

#include "ToneStackFilter.h"
#include "IIRFilter.h"

namespace ATK
{
  template<typename DataType>
  ToneStackFilterCoefficients<DataType>::ToneStackFilterCoefficients()
  :TypedBaseFilter<DataType>(1, 1), R1(0), R2(0), R3(0), R4(0), C1(0), C2(0), C3(0), low(.5), middle(.5), high(.5)
  {
  }

  template<typename DataType>
  void ToneStackFilterCoefficients<DataType>::setup()
  {
    Parent::setup();
    
    coefficients_in.assign(in_order+1, 0);
    coefficients_out.assign(out_order, 0);

    DataType tempm[2] = {-2 * input_sampling_rate, 2 * input_sampling_rate};
    DataType tempp[2] = {1, 1};
    boost::math::tools::polynomial<DataType> poly1(tempm, 1);
    boost::math::tools::polynomial<DataType> poly2(tempp, 1);

    boost::math::tools::polynomial<DataType> b;
    boost::math::tools::polynomial<DataType> a;
    
    b = poly2 * poly2 * poly1 * (high*C1*R1 + middle*C3*R3 + low*(C1*R2+C2*R2) + (C1*R3 + C2*R3));
    b += poly2 * poly1 * poly1 * (high*(C1*C2*R1*R4 + C1*C3*R1*R4) - middle*middle*(C1*C3*R3*R3 + C2*C3*R3*R3) + middle*(C1*C3*R1*R3 + C1*C3*R3*R3 + C2*C3*R3*R3)
      + low*(C1*C2*R1*R2 + C1*C2*R2*R4 + C1*C3*R2*R4) + low*middle*(C1*C3*R2*R3 + C2*C3*R2*R3)
      + (C1*C2*R1*R3 + C1*C2*R3*R4 + C1*C3*R3*R4));
    b += poly1 * poly1 * poly1 * (low*middle*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4) - middle*middle*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
      + middle*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4) + high*C1*C2*C3*R1*R3*R4 - high*middle*C1*C2*C3*R1*R3*R4
      + high*low*C1*C2*C3*R1*R2*R4);

    a = poly2 * poly2 * poly2;
    a += poly2 * poly2 * poly1 * (C1*R1 + C1*R3 + C2*R3 + C2*R4 + C3*R4) + middle*C3*R3 + low*(C1*R2 + C2*R2);
    a += poly2 * poly1 * poly1 * (middle*(C1*C3*R1*R3 - C2*C3*R3*R4 + C1*C3*R3*R3 + C2*C3*R3*R3)
      + low*middle*(C1*C3*R2*R3 + C2*C3*R2*R3) - middle*(C1*C3*R3*R3 + C2*C3*R3*R3) + low*(C1*C2*R2*R4 + C1*C2*R1*R2 + C1*C3*R2*R4 + C2*C3*R2*R4)
      + (C1*C2*R1*R4 + C1*C3*R1*R4 + C1*C2*R3*R4 + C1*C2*R1*R3 + C1*C3*R3*R4 + C2*C3*R3*R4));
    a += poly1 * poly1 * poly1 * (low*middle*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4) - middle*middle*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
      + middle*(C1*C2*C3*R3*R3*R4 + C1*C2*C3*R1*R3*R3 - C1*C2*C3*R1*R3*R4)
      + low*C1*C2*C3*R1*R2*R4 + C1*C2*C3*R1*R3*R4);

    for(int i = 0; i < in_order + 1; ++i)
    {
      coefficients_in[i] = b[i] / a[out_order];
    }
    for(int i = 0; i < out_order; ++i)
    {
      coefficients_out[i] = -a[i] / a[out_order];
    }
  }

  template<typename DataType>
  void ToneStackFilterCoefficients<DataType>::set_low(DataType low)
  {
    if(low < 0 || low > 1)
    {
      throw std::range_error("Low is outside the interval [0,1]");
    }
    this->low = low;

    setup();
  }
  
  template<typename DataType>
  typename ToneStackFilterCoefficients<DataType>::DataType ToneStackFilterCoefficients<DataType>::get_low() const
  {
    return low;
  }

  template<typename DataType>
  void ToneStackFilterCoefficients<DataType>::set_middle(DataType low)
  {
    if(middle < 0 || middle > 1)
    {
      throw std::range_error("Middle is outside the interval [0,1]");
    }
    this->middle = middle;

    setup();
  }

  template<typename DataType>
  typename ToneStackFilterCoefficients<DataType>::DataType ToneStackFilterCoefficients<DataType>::get_middle() const
  {
    return middle;
  }


  template<typename DataType>
  void ToneStackFilterCoefficients<DataType>::set_high(DataType low)
  {
    if(high < 0 || high > 1)
    {
      throw std::range_error("high is outside the interval [0,1]");
    }
    this->high = high;

    setup();
  }

  template<typename DataType>
  typename ToneStackFilterCoefficients<DataType>::DataType ToneStackFilterCoefficients<DataType>::get_high() const
  {
    return high;
  }


  template<typename DataType>
  ToneStackFilterCoefficients<typename ToneStackFilterCoefficients<DataType>::DataType>* ToneStackFilterCoefficients<DataType>::buildBassmanStack()
  {
    ToneStackFilterCoefficients<DataType>* filter = new IIRFilter<ToneStackFilterCoefficients<DataType> >;
    filter->set_coefficients(250e3, 1e6, 25e3, 45e3, 250e-12, 20e-9, 20e-9);
    return filter;
  }

  template<typename DataType>
  ToneStackFilterCoefficients<typename ToneStackFilterCoefficients<DataType>::DataType>* ToneStackFilterCoefficients<DataType>::buildJCM800Stack()
  {
    ToneStackFilterCoefficients<DataType>* filter = new IIRFilter<ToneStackFilterCoefficients<DataType> >;
    filter->set_coefficients(220e3, 1e6, 22e3, 33e3, 470e-12, 22e-9, 22e-9);
    return filter;
  }

  template class ToneStackFilterCoefficients<float>;
  template class ToneStackFilterCoefficients<double>;
  
  template class IIRFilter<ToneStackFilterCoefficients<float> >;
  template class IIRFilter<ToneStackFilterCoefficients<double> >;
}
