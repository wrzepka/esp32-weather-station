package wrzepka.backend;

import com.fasterxml.jackson.annotation.JsonProperty;
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

    public WeatherTelemetry(OffsetDateTime dateTime, String deviceId, float pressure, float temperature, float humidity, int lightIntensity) {
//        this.dateTime = dateTime;
//        this.deviceId = deviceId;
//        this.pressure = pressure;
//        this.temperature = temperature;
//        this.humidity = humidity;
//        this.lightIntensity = lightIntensity;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public OffsetDateTime getDateTime() {
        return dateTime;
    }

    public float getPressure() {
        return pressure;
    }

    public float getTemperature() {
        return temperature;
    }

    public float getHumidity() {
        return humidity;
    }

    public int getLightIntensity() {
        return lightIntensity;
    }

    public void setDateTime(OffsetDateTime dateTime) {
        this.dateTime = dateTime;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public void setPressure(float pressure) {
        this.pressure = pressure;
    }

    public void setTemperature(float temperature) {
        this.temperature = temperature;
    }

    public void setHumidity(float humidity) {
        this.humidity = humidity;
    }

    public void setLightIntensity(int lightIntensity) {
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
