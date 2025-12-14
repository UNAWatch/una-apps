#include <gui/containers/TrackSummaryHR.hpp>

TrackSummaryHR::TrackSummaryHR()
{

}

void TrackSummaryHR::initialize()
{
    TrackSummaryHRBase::initialize();
}

void TrackSummaryHR::setMaxHR(float hr)
{
    Unicode::snprintfFloat(maxHrValueBuffer, MAXHRVALUE_SIZE, "%.0f", hr);
    maxHrValue.invalidate();
}

void TrackSummaryHR::setAvgHR(float hr)
{
    Unicode::snprintfFloat(avgHrValueBuffer, AVGHRVALUE_SIZE, "%.0f", hr);
    avgHrValue.invalidate();
}