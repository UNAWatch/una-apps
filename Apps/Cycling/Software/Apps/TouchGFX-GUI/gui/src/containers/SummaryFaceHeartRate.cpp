#include <gui/containers/SummaryFaceHeartRate.hpp>
#include <texts/TextKeysAndLanguages.hpp>

SummaryFaceHeartRate::SummaryFaceHeartRate()
{
}

void SummaryFaceHeartRate::initialize()
{
    SummaryFaceHeartRateBase::initialize();
    title.set(T_TEXT_HEART_RATE_UC);
}

void SummaryFaceHeartRate::setMaxHR(float hr)
{
    Unicode::snprintfFloat(maxHrValueBuffer, MAXHRVALUE_SIZE, "%.0f", hr);
    maxHrValue.invalidate();
}

void SummaryFaceHeartRate::setAvgHR(float hr)
{
    Unicode::snprintfFloat(avgHrValueBuffer, AVGHRVALUE_SIZE, "%.0f", hr);
    avgHrValue.invalidate();
}
