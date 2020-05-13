///@file PixieCfd.hpp
///@brief Same CFD algorithm implemented by Xia LLC but offline. Based on XiaCfd but rewritten for modern PAASS 
///@author T. T. King,  S. V. Paulauskas
///@date Feb 06, 2020
#ifndef PIXIESUITE_PIXIECFD_HPP
#define PIXIESUITE_PIXIECFD_HPP

#include <tuple> 
#include <vector>
#include <utility>

#include "Exceptions.hpp" 
#include "TimingDriver.hpp" 

class PixieCfd : public TimingDriver {
public:
    PixieCfd() {};

    ~PixieCfd() {};

    /// Perform CFD analysis on the waveform using the XIA algorithm.
    double CalculatePhase(const std::vector<double> &data,
                          const std::tuple<double,double, double> &pars,
                          const std::pair<unsigned int, unsigned int> &range,
                          const std::pair<double, double> baseline);


};

#endif //PIXIESUITE_PIXIECFD_HPP
