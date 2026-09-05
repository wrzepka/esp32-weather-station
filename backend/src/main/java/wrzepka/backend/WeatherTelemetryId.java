package wrzepka.backend;

import java.io.Serializable;
import java.time.OffsetDateTime;
import java.util.Objects;

/**
 * Complex key used in weather telemetry records.
 * It uses device identifier and datetime offset.
 */
public class WeatherTelemetryId implements Serializable {

    /**
     * Measurement timestamp SQL: TIMESTAMPTZ
     */
    private OffsetDateTime dateTime;

    /**
     * Weather station identifier, downloaded from message topic: `DEVICECODE`_`MAC_ADDR`
     */
    private String deviceId;

    public WeatherTelemetryId() {

    }

    public WeatherTelemetryId(OffsetDateTime dateTime, String deviceId) {
        this.dateTime = dateTime;
        this.deviceId = deviceId;
    }

    public OffsetDateTime getDateTime() {
        return dateTime;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDateTime(OffsetDateTime dateTime) {
        this.dateTime = dateTime;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        WeatherTelemetryId that = (WeatherTelemetryId) o;
        return Objects.equals(dateTime, that.dateTime) && Objects.equals(deviceId, that.deviceId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(dateTime, deviceId);
    }
}
