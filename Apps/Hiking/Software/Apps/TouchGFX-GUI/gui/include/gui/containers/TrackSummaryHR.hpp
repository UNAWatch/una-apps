#ifndef TRACKSUMMARYHR_HPP
#define TRACKSUMMARYHR_HPP

#include <gui_generated/containers/TrackSummaryHRBase.hpp>

class TrackSummaryHR : public TrackSummaryHRBase
{
public:
    TrackSummaryHR();
    virtual ~TrackSummaryHR() {}

    virtual void initialize();

    void setMaxHR(uint32_t hr);
    void setAvgHR(uint32_t hr);

protected:
};

#endif // TRACKSUMMARYHR_HPP
