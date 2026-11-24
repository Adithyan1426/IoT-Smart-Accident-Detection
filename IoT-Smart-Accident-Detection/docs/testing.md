# Test plan

Record actual outcomes; do not mark tests passed until performed.

| Test | Expected result | Actual |
|---|---|---|
| MPU6050 I2C | Stable acceleration readings | Pending |
| GPS outdoors | Valid coordinates and satellite count | Pending |
| Node 1 → Node 2 | Node 2 receives 56-byte packets | Pending |
| Node 2 → Node 3 | Node 3 receives forwarded packets | Pending |
| Node 3 → Flask | Flask returns HTTP 201 | Pending |
| SQLite / API | New record appears in history | Pending |
| Dashboard | Telemetry refreshes | Pending |

The X-axis threshold is orientation-dependent and can create false positives/negatives. A robust classifier requires calibration, multi-axis acceleration and gyroscope processing, filtering, event-duration logic, and systematic validation.
