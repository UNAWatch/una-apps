# Strain Calculation Service Architecture for GlanceStrain App

## High-Level Design Diagram

```mermaid
flowchart TD
    A[Service::run - Message Loop] --> B{Message Type?}
    B -->|GLANCE_START| C[configGui & createGuiControls<br/>Load daily JSON if exists<br/>connect HEART_RATE & ACTIVITY]
    B -->|GLANCE_TICK| D[onGlanceTick<br/>Check day rollover<br/>Update mGlanceValue with total_strain<br/>Update Glance UI if invalid<br/>Save JSON periodically]
    B -->|SENSOR_LAYER_DATA| E{onSdlNewData<br/>handle?}
    E -->|HEART_RATE| F[Parse HR data<br/>Compute strain_delta = dt_min * ((hr-60)/(180-60)) * 0.75<br/>Accumulate stats<br/>Add to samples vector]
    E -->|ACTIVITY| G[Parse active minutes<br/>Update mActiveMin]
    B -->|GLANCE_STOP / APP_STOP| H[disconnect sensors<br/>Save JSON to /storage/strain_YYYY-MM-DD.json]
    style A fill:#f9f,stroke:#333
```

## Detailed Class Diagram

```mermaid
classDiagram
    class Service {
        <<final>>
        -const SDK::Kernel& mKernel
        -const char* mName = \"Strain Score\"
        -SDK::Glance::Form mGlanceUI
        -SDK::Glance::ControlText mGlanceTitle
        -SDK::Glance::ControlText mGlanceValue
        -SDK::Sensor::Connection mSensorActivity[SDK::Sensor::Type::ACTIVITY]
        -SDK::Sensor::Connection mSensorHR[SDK::Sensor::Type::HEART_RATE]
        -float mTotalStrain = 0.0f
        -float mSumHR = 0.0f
        -uint16_t mMaxHR = 0
        -uint32_t mActiveMin = 0
        -uint32_t mSampleCount = 0
        -uint64_t mLastHRTimeMs = 0
        -std::vector~StrainSample~ mSamples
        -std::string mCurrentDate
        -char mJsonFilename[64]
        +Service(SDK::Kernel&)
        +~Service()
        +void run()
        +void connect()
        +void disconnect()
        +void onSdlNewData(uint16_t handle, const SDK::Sensor::Data* data, uint16_t count, uint16_t stride) override
        +void onGlanceTick()
        +bool configGui()
        +void createGuiControls()
        -void loadDailyStrain()
        -void saveDailyStrain()
        -void checkDayRollover()
        -float computeStrainDelta(uint16_t hr, float dt_min)
    }
    class StrainSample {
        +uint64_t timestamp_ms
        +uint16_t hr_bpm
        +float strain_delta
    }
    Service ..|> SDK::Interface::ISensorDataListener
    Service --> StrainSample
```

## Pseudocode for Key Methods

### Service::run()
```
while (true) {
    SDK::MessageBase* msg;
    if (!mKernel.comm.getMessage(msg)) continue;
    switch (msg->getType()) {
        case EVENT_GLANCE_START:
            if (configGui()) {
                createGuiControls();  // Title: \"Strain Score\"
                loadDailyStrain();    // Init from JSON if same day
                connect();
            } else {
                mKernel.sys.exit(0);
            }
            break;
        case EVENT_GLANCE_STOP:
        case COMMAND_APP_STOP:
            disconnect();
            saveDailyStrain();
            mKernel.comm.releaseMessage(msg);
            return;
        case EVENT_GLANCE_TICK:
            onGlanceTick();
            break;
        case EVENT_SENSOR_LAYER_DATA:
            auto event = static_cast<SDK::Message::Sensor::EventData*>(msg);
            onSdlNewData(event->handle, event->data, event->count, event->stride);
            break;
    }
    mKernel.comm.releaseMessage(msg);
}
```

### Service::onSdlNewData(...)
```
SDK::Sensor::DataBatch batch(data, count, stride);
if (mSensorActivity.matchesDriver(handle)) {
    SDK::SensorDataParser::Activity p(batch[0]);
    if (p.isDataValid()) mActiveMin = p.getDuration();
}
if (mSensorHR.matchesDriver(handle)) {
    SDK::SensorDataParser::HeartRate p(batch[0]);
    if (p.isDataValid()) {
        uint64_t ts = p.getTimestamp();
        uint16_t hr = p.getBpm();
        if (mLastHRTimeMs != 0 && hr > 50) {  // Valid HR
            float dt_sec = (ts - mLastHRTimeMs) / 1000.0f;
            float dt_min = dt_sec / 60.0f;
            float delta = computeStrainDelta(hr, dt_min);
            mTotalStrain += delta;
            mSumHR += static_cast<float>(hr) * dt_min;
            if (hr > mMaxHR) mMaxHR = hr;
            mSampleCount++;
            if (mSamples.size() < 1440) {  // Limit ~1 per min
                mSamples.push_back({ts, hr, delta});
            }
        }
        mLastHRTimeMs = ts;
    }
}
```

### Service::onGlanceTick()
```
checkDayRollover();  // Save old, init new if needed
mGlanceValue.print(\"%.1f\", mTotalStrain);
if (mGlanceUI.isInvalid()) {
    auto* upd = mKernel.comm.allocateMessage~SDK::Message::RequestGlanceUpdate~();
    upd->name = mName;
    upd->controls = mGlanceUI.data();
    upd->controlsNumber = mGlanceUI.size();
    mKernel.comm.sendMessage(upd, 100);
    mKernel.comm.releaseMessage(upd);
    mGlanceUI.setValid();
}
saveDailyStrain();  // Periodic save
```

### saveDailyStrain() / loadDailyStrain()
```
// Assume simple file I/O via mKernel.sys.storage.open(path, mode)
FILE* f = fopen(mJsonFilename, mode == 'r' ? \"r\" : \"w\");
if (f) {
    if (load) {
        // Parse JSON: sscanf or simple parser for accumulators & samples
        // e.g. fscanf(f, \"{\\\"date\\\":\\\"%10s\\\",\\\"total_strain\\\":%f,...\", date, &total_strain, ...)
        // For samples: loop fscanf until ]
    } else {
        // Build JSON with fprintf
        fprintf(f, \"{\\\"date\\\":\\\"%s\\\",\\\"total_strain\\\":%.2f,\\\"avg_hr\\\":%.1f,\\\"max_hr\\\":%hu,\\\"active_min\\\":%u,\\\"samples\\\":[\", mCurrentDate.c_str(), mTotalStrain, mSampleCount>0?mSumHR/mSampleCount:0, mMaxHR, mActiveMin);
        for (auto& s : mSamples) {
            fprintf(f, \"{\\\"timestamp\\\":%llu,\\\"hr\\\":%hu,\\\"strain_delta\\\":%.3f},\", s.timestamp, s.hr, s.strain_delta);
        }
        fprintf(f, \"{}]\\\n\");
    }
    fclose(f);
}
```

## JSON Schema

```json
{
  \"type\": \"object\",
  \"properties\": {
    \"date\": { \"type\": \"string\", \"format\": \"date\" },
    \"total_strain\": { \"type\": \"number\" },
    \"avg_hr\": { \"type\": \"number\" },
    \"max_hr\": { \"type\": \"integer\", \"minimum\": 0, \"maximum\": 255 },
    \"active_min\": { \"type\": \"integer\", \"minimum\": 0 },
    \"samples\": {
      \"type\": \"array\",
      \"items\": {
        \"type\": \"object\",
        \"properties\": {
          \"timestamp\": { \"type\": \"integer\" },
          \"hr\": { \"type\": \"integer\", \"minimum\": 0, \"maximum\": 255 },
          \"strain_delta\": { \"type\": \"number\" }
        },
        \"required\": [\"timestamp\", \"hr\", \"strain_delta\"]
      }
    }
  },
  \"required\": [\"date\", \"total_strain\", \"avg_hr\", \"max_hr\", \"active_min\", \"samples\"]
}
```

## List of Includes Needed

In Service.hpp:
```
#include <vector>
#include <string>
#include <ctime>
#include <cstdio>  // For JSON sprintf/fprintf if no SDK JSON lib

#include \"SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp\"
#include \"SDK/SensorLayer/DataParsers/SensorDataParserActivity.hpp\"
```

In Service.cpp: current + above.

## Assumptions & Decisions

- **Sensors used**: HEART_RATE (primary for strain), ACTIVITY (for active minutes stat).
- **Strain formula**: TRIMP-inspired: `delta = dt_minutes * ((hr - HRrest) / (HRmax - HRrest)) * k`<br/>HRrest=60, HRmax=180, k=0.75 (gender avg).
- **Time integration**: Use timestamp diff from consecutive HR samples (assume ~1min intervals).
- **JSON structure**: As specified, daily file `/storage/strain_YYYY-MM-DD.json`. Simple fprintf build/parse (adapt Cycling FIT writer style to JSON).
- **Day rollover**: Checked on every tick using `std::time` & `strftime`.
- **Edge cases**: Init zeros on new day/no data; limit samples to 1440/day; save on stop/tick/rollover; handle disconnects gracefully.
- **Storage**: Direct file I/O (`fopen/fprintf` via SDK storage API); no full JSON lib to minimize deps (emulate Cycling serializers).
- **Glance UI**: Title \"Strain Score\", value shows `total_strain` formatted as \"%.1f\".
- **Memory**: Vector limited to ~1440 samples (1/day max), ~10KB.

This design rewrites Service.hpp/cpp removing old ACTIVITY-only logic, integrates dual-sensor accumulation, daily JSON persistence mimicking Cycling FIT record saving (lines 636-637 adapted to JSON append/stats).