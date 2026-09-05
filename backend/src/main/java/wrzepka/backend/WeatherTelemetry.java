package wrzepka.backend;

import jakarta.persistence.*;

import java.time.OffsetDateTime;
import java.util.Objects;

/**
 * Entity represents single record of weather station telemetry.
 * Mapped into timestamp table. {@code TelemetryMapper}
 * Unique via complex key {@link WeatherTelemetryId}
 */
@Entity
@Table(name = "weather_telemetry")
@IdClass(WeatherTelemetryId.class)
public class WeatherTelemetry {

    /** Measurement timestamp SQL: TIMESTAMPTZ*/
    @Id
    @Column(name = "date_time", nullable = false)
    private OffsetDateTime dateTime;

    /** Weather station identifier, downloaded from message topic: `DEVICECODE`_`MAC_ADDR`*/
    @Id
    @Column(name = "device_id", nullable = false)
    private String deviceId;

    /** Absolute pressure in hPa, measured by BME280 sensor.*/
    @Column(name = "pressure")
    private Float pressure;

    /** environment temperature in Celsius, measured by BME280 sensor.*/
    @Column(name = "temperature")
    private Float temperature;

    /** Relative humidity in percentage, measured by BME280 sensor.*/
    @Column(name = "humidity")
    private Float humidity;

    /** Light intensity in lx, measured by BH1750 sensor.*/
    @Column(name = "light_intensity")
    private Long lightIntensity;

    public WeatherTelemetry() {

    }

    public WeatherTelemetry(OffsetDateTime dateTime, String deviceId, Float pressure, Float temperature, Float humidity, Long lightIntensity) {
        this.dateTime = dateTime;
        this.deviceId = deviceId;
        this.pressure = pressure;
        this.temperature = temperature;
        this.humidity = humidity;
        this.lightIntensity = lightIntensity;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public OffsetDateTime getDateTime() {
        return dateTime;
    }

    public Float getPressure() {
        return pressure;
    }

    public Float getTemperature() {
        return temperature;
    }

    public Float getHumidity() {
        return humidity;
    }

    public Long getLightIntensity() {
        return lightIntensity;
    }

    public void setPressure(Float pressure) {
        this.pressure = pressure;
    }

    public void setTemperature(Float temperature) {
        this.temperature = temperature;
    }

    public void setHumidity(Float humidity) {
        this.humidity = humidity;
    }

    public void setLightIntensity(Long lightIntensity) {
        this.lightIntensity = lightIntensity;
    }

    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        WeatherTelemetry that = (WeatherTelemetry) o;
        return Objects.equals(dateTime, that.dateTime) && Objects.equals(deviceId, that.deviceId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(dateTime, deviceId);
    }
}
