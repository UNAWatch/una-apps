#ifndef HRBAR_HPP
#define HRBAR_HPP

#include <gui_generated/containers/HrBarBase.hpp>

#include <array>

class HrBar : public HrBarBase
{
public:
    HrBar();
    virtual ~HrBar() {}

    virtual void initialize();

    void setHR(float hr, const uint8_t* buff, uint8_t size);

protected:
    const std::array<touchgfx::Image*, 5> mZones;
};

#endif // HRBAR_HPP
