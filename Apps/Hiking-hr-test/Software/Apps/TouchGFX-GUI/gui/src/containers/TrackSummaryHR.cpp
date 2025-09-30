#include <gui/containers/TrackSummaryHR.hpp>

TrackSummaryHR::TrackSummaryHR()
{

}

void TrackSummaryHR::initialize()
{
    TrackSummaryHRBase::initialize();
}

void TrackSummaryHR::setMaxHR(uint32_t hr)
{
    Unicode::snprintf(maxHrValueBuffer, MAXHRVALUE_SIZE, "%u", hr);
    maxHrValue.invalidate();
}

void TrackSummaryHR::setAvgHR(uint32_t hr)
{
    Unicode::snprintf(avgHrValueBuffer, AVGHRVALUE_SIZE, "%u", hr);
    avgHrValue.invalidate();
}