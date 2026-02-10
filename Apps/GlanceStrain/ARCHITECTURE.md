# GlanceStrain Architecture

## Problem Statement

Provide a glance-friendly strain summary that logs heart-rate-derived strain only while the glance is active. Logging must be resilient to off-hand transitions and recover daily summaries from persisted FIT data.

## Program Architecture (Top-to-Bottom)

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
