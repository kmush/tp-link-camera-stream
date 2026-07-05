# Logging and MQTT topics

This project writes structured logs and publishes MQTT topics for monitoring and integration.

Files:
- logs/mqtt.log — all MQTT publishes and receives (JSON lines)
- logs/events.log — internal events, motion detections, and recording actions

MQTT topics:
- `vehicle/camera/status` — periodic status messages
- `vehicle/camera/alert` — alerts such as motion detections
- `vehicle/camera/cmd` — simple command topic (e.g., `record_on`, `record_off`)

Example: tail the MQTT log

```bash
tail -F logs/mqtt.log
```

Example: monitor status topic

```bash
mosquitto_sub -h test.mosquitto.org -t "vehicle/camera/status"
```
