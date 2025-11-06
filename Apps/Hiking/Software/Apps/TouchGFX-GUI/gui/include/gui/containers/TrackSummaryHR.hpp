#ifndef TRACKSUMMARYHR_HPP
#define TRACKSUMMARYHR_HPP

#include <gui_generated/containers/TrackSummaryHRBase.hpp>

class TrackSummaryHR : public TrackSummaryHRBase
{
public:
    TrackSummaryHR();
    virtual ~TrackSummaryHR() {}

    virtual void initialize();

    void setMaxHR(float hr);
    void setAvgHR(float hr);

protected:
};

#endif // TRACKSUMMARYHR_HPP
