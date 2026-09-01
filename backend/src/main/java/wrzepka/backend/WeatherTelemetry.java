package wrzepka.backend;

import jakarta.persistence.*;

import java.time.OffsetDateTime;
import java.util.Objects;

@Entity
@Table(name = "weather_telemetry")
@IdClass(WeatherTelemetryId.class)
public class WeatherTelemetry {

    @Id
    @Column(name = "date_time", nullable = false)
    private OffsetDateTime dateTime;

    @Id
    @Column(name = "device_id", nullable = false)
    private String deviceId;

    @Column(name = "pressure")
    private Float pressure;

    @Column(name = "temperature")
    private Float temperature;

    @Column(name = "humidity")
    private Float humidity;

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

    public void setDateTime(OffsetDateTime dateTime) {
        this.dateTime = dateTime;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
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
