# GlanceStrain Architecture

## Problem Statement

Provide a glance-friendly strain summary that logs heart-rate-derived strain only while the glance is active. Logging must be resilient to off-hand transitions and recover daily summaries from persisted FIT data.

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
