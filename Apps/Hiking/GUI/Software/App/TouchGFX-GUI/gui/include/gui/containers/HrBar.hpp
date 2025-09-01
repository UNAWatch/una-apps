#ifndef HRBAR_HPP
#define HRBAR_HPP

#include <gui_generated/containers/HrBarBase.hpp>

class HrBar : public HrBarBase
{
public:
    HrBar();
    virtual ~HrBar() {}

    virtual void initialize();

    void setHR(float hr);

protected:
};

#endif // HRBAR_HPP
