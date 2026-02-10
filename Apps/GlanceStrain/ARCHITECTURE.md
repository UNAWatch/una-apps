# GlanceStrain Architecture

## Problem Statement

Provide a glance-friendly strain summary that logs heart-rate-derived strain only while the glance is active. Logging must be resilient to off-hand transitions and recover some daily activity from persisted FIT data.

## Program Architecture

### 1) Kernel entry and service lifetime

- `Service` is constructed with a reference to the kernel (`SDK::Kernel`) and runs as the glance service entry point.
- `Service::run()` is the main loop. It blocks on `mKernel.comm.getMessage()` and reacts to kernel events and commands.
- The service exits when the glance stops (`EVENT_GLANCE_STOP`) or when the app is forcibly stopped (`COMMAND_APP_STOP`).

### 2) Kernel message loop

`Service::run()` handles the following message types in order:

- `EVENT_GLANCE_START`
  - Configure glance UI bounds (`configGui()`), build controls (`createGuiControls()`), mark `mGlanceActive = true`.
  - Connect to sensors (`connect()`), handle day rollover (`checkDayRollover()`), and trigger an initial FIT save (`saveFit(true, false)`).
- `EVENT_GLANCE_TICK`
  - Refresh display and data (`onGlanceTick()`).
- `EVENT_SENSOR_LAYER_DATA`
  - Dispatch batched sensor samples into `onSdlNewData()`.
- `EVENT_GLANCE_STOP`
  - Save, disconnect, and return from `run()`.
- `COMMAND_APP_STOP`
  - Finalize the FIT session for the day, disconnect, and return from `run()`.

### 3) UI layer (Glance)

- `SDK::Glance::Form` (`mGlanceUI`) owns controls.
- `createGuiControls()` builds the UI layout:
  - Icon image
  - Title text ("Strain Score")
  - Value text for current strain score
- `onGlanceTick()` updates the value text and issues a `RequestGlanceUpdate` when the UI is invalid.

### 4) Sensor layer

- `SDK::Sensor::Connection` instances subscribe to:
  - `HEART_RATE` → `SensorDataParserHeartRate`
  - `ACTIVITY` → `SensorDataParserActivity`
  - `TOUCH_DETECT` → `SensorDataParserTouch`
- `onSdlNewData()` routes incoming batches by sensor handle:
  - Touch updates `mIsOnHand` and triggers `saveFit(true, false)` on off-hand transitions.
  - Activity updates active minutes (`mActiveMin`).
  - Heart rate updates strain accumulators and the most recent valid HR value.

### 5) Core strain logic

- Samples are accepted only when HR is in the valid range [50, 220].
- Each HR sample contributes a normalized delta:
  - `norm = (hr - 60) / 120`
  - `delta = max(0, norm) * 0.75`
- Running aggregates:
  - `mTotalStrain`, `mSumHR`, `mMaxHR`, `mSampleCount`, `mLastHr`
- On each glance tick, if glance is active and on-hand, a pending FIT record is captured every 5 seconds.

### 6) Persistence (FIT)

- A daily FIT file is created with name `strain_YYYY-MM-DD.fit` in the app filesystem.
- `saveFit(force, finalizeDay)` appends pending records, and optionally adds session summary if `finalizeDay` is true.
- FIT file structure is built using `SDK::Component::FitHelper` definitions for file ID, developer data, events, records, sessions, and activity summaries.
- The file header is rewritten after appends and CRC is recomputed on each save.

## Implementation Walktrought

1. **App layout**
   - Service entry point and logic live in [`Software/Libs/Header/Service.hpp`](Examples/Apps/GlanceStrain/Software/Libs/Header/Service.hpp:1) and [`Software/Libs/Source/Service.cpp`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:1).
   - App build wiring is under [`Software/App/GlanceStrain-CMake/CMakeLists.txt`](Examples/Apps/GlanceStrain/Software/App/GlanceStrain-CMake/CMakeLists.txt).
   - Glance assets are compiled from headers like [`Software/Libs/Header/icon_60x60.h`](Examples/Apps/GlanceStrain/Software/Libs/Header/icon_60x60.h:1) and the PNGs under [`Resources/`](Examples/Apps/GlanceStrain/Resources:1).

2. **Entry point**
   - `Service::run()` in [`Service.cpp`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:197) is the main loop and the only entry point called by the kernel.
   - The loop blocks on `mKernel.comm.getMessage()` and switches on `SDK::MessageType` values (glance start/stop, tick, sensor data, app stop).

3. **UI**
   - `EVENT_GLANCE_START` calls `configGui()` to fetch sizing via `SDK::Message::RequestGlanceConfig` and initializes [`mGlanceUI`](Examples/Apps/GlanceStrain/Software/Libs/Header/Service.hpp:56).
   - `createGuiControls()` builds the icon, title, and value text controls and stores them in `mGlanceUI` (see [`Service::createGuiControls()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:424)).
   - `onGlanceTick()` updates `mGlanceValue` and dispatches `RequestGlanceUpdate` when the form is invalid (see [`Service::onGlanceTick()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:368)).

4. **Sensor connections**
   - The service owns `SDK::Sensor::Connection` members for heart rate, activity, and touch (see [`mSensorHR`](Examples/Apps/GlanceStrain/Software/Libs/Header/Service.hpp:60), [`mSensorActivity`](Examples/Apps/GlanceStrain/Software/Libs/Header/Service.hpp:61), and [`mSensorTouch`](Examples/Apps/GlanceStrain/Software/Libs/Header/Service.hpp:62)).
   - `connect()` is called on glance start and `disconnect()` on stop/app stop (see [`Service::connect()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:268) and [`Service::disconnect()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:287)).

5. **Handle incoming sensor messages**
   - `EVENT_SENSOR_LAYER_DATA` forwards sensor batches into `Service::onSdlNewData()` (see [`Service::run()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:197)).
   - `onSdlNewData()` uses `SDK::Sensor::DataBatch` and the parser types `SensorDataParser::Touch`, `::Activity`, and `::HeartRate` to validate and decode frames (see [`Service::onSdlNewData()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:305)).
   - Touch updates `mIsOnHand` and forces an immediate FIT save when the watch is removed (off-hand transition).
   - Activity updates `mActiveMin` from the parsed duration.

6. **Apply the strain calculation and accumulate state**
   - Heart-rate samples are accepted only in [50, 220]. For each valid sample, `norm = (hr - 60) / 120` and `delta = max(0, norm) * 0.75` (see [`Service::onSdlNewData()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:344)).
   - Running totals are stored in `mTotalStrain`, `mSumHR`, `mMaxHR`, `mSampleCount`, and `mLastHr` (see [`Service.hpp`](Examples/Apps/GlanceStrain/Software/Libs/Header/Service.hpp:68)).

7. **Emit samples and refresh the UI on ticks**
   - `onGlanceTick()` enforces the `skSamplePeriodSec` cadence (5 seconds) and appends pending records to `mPendingRecords` while glance is active and on-hand (see [`Service::onGlanceTick()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:368)).
   - If `mGlanceActive` is true, `saveFit(false, false)` is invoked to persist at most once per `skSaveIntervalSec` (3600 seconds).

8. **Persist FIT data and handle day rollover**
   - `checkDayRollover()` updates `mCurrentDate`, rebuilds `mFitPath` as `strain_YYYY-MM-DD.fit`, and resets accumulators when the date changes (see [`Service::checkDayRollover()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:443)).
   - `saveFit(force, finalizeDay)` opens or creates the FIT file, writes definitions when needed, appends pending records, and optionally emits a session summary (see [`Service::saveFit()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:587)).
   - FIT message/field helpers are initialized in the constructor (`mFitFileID`, `mFitRecord`, `mFitSession`, `mFitStrainField`, `mFitActiveField`) and used by `writeFitDefinitions()`, `appendPendingRecords()`, and `writeFitSessionSummary()` (see [`Service::Service()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:142) and helper methods nearby).
   - `saveFit()` exits early if the glance is inactive, so persistence occurs only while the glance is running.

9. **Logs behavior while iterating**
   - Use log output from `Service.cpp` (`LOG_INFO`/`LOG_DEBUG`) to verify event sequencing, day rollover, and save cadence (see [`Service::run()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:197) and [`Service::checkDayRollover()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:443)).
   - Confirm that off-hand transitions force an immediate save by simulating `TOUCH_DETECT` events and watching for `saveFit(true, false)` calls in logs (see [`Service::onSdlNewData()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:305)).
   - Verify the FIT output location and daily file naming using `mFitPath` after a glance start/rollover (see [`Service::checkDayRollover()`](Examples/Apps/GlanceStrain/Software/Libs/Source/Service.cpp:443)).

## Common vs App-Specific Components

### Common services and patterns (shared across glance apps)

- Kernel integration via `SDK::Kernel` and `mKernel.comm` message loop.
- Glance UI primitives: `SDK::Glance::Form`, `ControlText`, `RequestGlanceConfig`, `RequestGlanceUpdate`.
- Sensor subscription via `SDK::Sensor::Connection` and `SDK::Interface::ISensorDataListener`.
- File access via `SDK::Interface::IFileSystem` and `SDK::Interface::IFile`.
- FIT helper/definitions via `SDK::Component::FitHelper`.

### GlanceStrain-specific services and logic

- Strain scoring formula and gating by valid HR range.
- Off-hand detection behavior (touch sensor drives immediate save).
- Daily file naming and per-day session summary emission.
- Developer fields for FIT (`strain` and `active_min`) via `mFitStrainField` and `mFitActiveField`.

## App ↔ Kernel Interaction / Execution Path

1. **Kernel starts glance** → `EVENT_GLANCE_START` → UI configured and rendered, sensors connected, daily FIT context initialized.
2. **Kernel delivers sensor batches** → `EVENT_SENSOR_LAYER_DATA` → `onSdlNewData()` updates HR/active minutes/on-hand state.
3. **Kernel ticks glance** → `EVENT_GLANCE_TICK` → `onGlanceTick()` updates the UI, pushes periodic FIT records, and triggers `saveFit()` when allowed by cadence.
4. **Kernel stops glance** → `EVENT_GLANCE_STOP` → immediate save, disconnect, exit.
5. **Kernel stops app** → `COMMAND_APP_STOP` → finalize session, disconnect, exit.

## Key Interfaces and Data Structures

- `SDK::Kernel` → kernel handle for messaging (`comm`) and filesystem (`fs`).
- `SDK::Interface::ISensorDataListener` → callback interface implemented by `Service` (`onSdlNewData`).
- `SDK::Sensor::Connection` → connects to HR/activity/touch drivers and matches incoming handles.
- `SDK::SensorDataParser::*` → typed decoding of incoming sensor frames.
- `SDK::Glance::Form` and `SDK::Glance::ControlText` → glance UI layout and updates.
- `SDK::Interface::IFile` / `IFileSystem` → FIT persistence.
- `SDK::Component::FitHelper` → FIT message definitions and writing.

## Behavior Details

### Logging cadence

- Record strain samples on a 5-second cadence while the glance is active.

### Persistence and recovery

- Persist to a daily FIT file (one file per day).
- Recover state from the daily FIT file on glance start and on day rollover so totals continue from the last saved data.

### Gating rules

- If `TOUCH_DETECT` reports unworn, treat the state as off-hand, suppress logging, and force an immediate save.
- Perform disk I/O only while the glance is active.
- Save on glance start and on glance ticks, but no more often than every 60 minutes unless an off-hand transition triggers an immediate save.
