#ifndef SCROLLSMALL_HPP
#define SCROLLSMALL_HPP

#include <gui_generated/containers/SideBarSmallBase.hpp>

class SideBarSmall : public SideBarSmallBase
{
public:
    SideBarSmall();
    virtual ~SideBarSmall() {}

    virtual void initialize();

    void setCount(uint16_t cnt);
    void setActiveId(uint16_t p);
    void animateToId(int16_t p, int16_t animationSteps = -1);
    uint16_t getActiveId();
    virtual void handleTickEvent()
    {
        nextAnimationStep();
    }
protected:
    const float kHandleLen = 10;
    const float kHandleMin = 252;
    const float kHandleMax = 288;

    uint16_t mCount = 1;
    uint16_t mPosition = 0;

    bool mAnimationRunning { };
    uint16_t mAnimationCounter { };
    uint16_t mAnimationDuration { };
    float mAnimationStartPos { };
    float mAnimationEndPos { };

    float mAnimationFadeEndPos { };

    bool mAnimationOvf {};
    float mAnimationOvfStartPos { };

    void nextAnimationStep();
    float getStartAngle(uint16_t p);
};

#endif // SCROLLSMALL_HPP
