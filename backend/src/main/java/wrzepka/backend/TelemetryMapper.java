package wrzepka.backend;

import org.springframework.stereotype.Component;

import java.time.OffsetDateTime;
import java.util.Objects;

/**
 * Maps raw MQTT telemetry payload into the database entity representation.
 * Handles fixed-point integer decoding into floating-point physical units.
 */
@Component
public class TelemetryMapper {

    public static final int PRESSURE_MIN_VALUE = 300;
    public static final int PRESSURE_MAX_VALUE = 1100;
    public static final float MIN_TEMPERATURE_VALUE = -40.0f;
    public static final float MAX_TEMPERATURE_VALUE = 85.0f;
    public static final int MIN_HUMIDITY_VALUE = 0;
    public static final float MAX_HUMIDITY_VALUE = 100.0f;
    public static final int MIN_LIGHT_INTENSITY_VALUE = 0;
    public static final int MAX_LIGHT_INTENSITY_VALUE = 65535;

    /**
     * Converts raw sensor payload into a persistent entity.
     *
     * @param payload  decoded JSON payload containing raw sensor measurements
     * @param deviceId unique node identifier extracted from the MQTT topic
     * @param dateTime measurement timestamp (UTC)
     * @return populated {@link WeatherTelemetry} entity ready for persistence
     */
    public WeatherTelemetry toEntity(TelemetryPayload payload, String deviceId, OffsetDateTime dateTime) {
        Objects.requireNonNull(payload, "Payload cannot be null");
        Objects.requireNonNull(dateTime, "DateTime cannot be null");
        if (deviceId == null || deviceId.isBlank()) {
            throw new IllegalArgumentException("Device ID cannot be null or blank");
        }

        final float divisor = 100.0f;

        Float pressure = scaleValue(payload.pressure(), divisor, PRESSURE_MIN_VALUE, PRESSURE_MAX_VALUE);
        Float temperature = scaleValue(payload.temperature(), divisor, MIN_TEMPERATURE_VALUE, MAX_TEMPERATURE_VALUE);
        Float humidity = scaleValue(payload.humidity(), divisor, MIN_HUMIDITY_VALUE, MAX_HUMIDITY_VALUE);
        Long lightIntensity = null;

        if (payload.lightIntensity() != null) {
            if (payload.lightIntensity() >= MIN_LIGHT_INTENSITY_VALUE && payload.lightIntensity() <= MAX_LIGHT_INTENSITY_VALUE)
                lightIntensity = payload.lightIntensity();
        }

        return new WeatherTelemetry(dateTime, deviceId, pressure, temperature, humidity, lightIntensity);
    }

    private Float scaleValue(Number rawValue, float divisor, float minValue, float maxValue) {
        if (rawValue == null) return null;
        float scaledValue = rawValue.floatValue() / divisor;

        if (scaledValue >= minValue && scaledValue <= maxValue) {
            return scaledValue;
        }
        return null;
    }
}
