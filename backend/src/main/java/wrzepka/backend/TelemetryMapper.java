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

    /**
     * Converts raw sensor payload into a persistent entity.
     *
     * @param payload decoded JSON payload containing raw sensor measurements
     * @param deviceId unique node identifier extracted from the MQTT topic
     * @param dateTime measurement timestamp (UTC)
     * @return populated {@link WeatherTelemetry} entity ready for persistence
     */
    public WeatherTelemetry toEntity(TelemetryPayload payload, String deviceId, OffsetDateTime dateTime){
        Objects.requireNonNull(payload, "Payload cannot be null");
        Objects.requireNonNull(dateTime, "DateTime cannot be null");
        if (deviceId == null || deviceId.isBlank()) {
            throw new IllegalArgumentException("Device ID cannot be null or blank");
        }

        final float divisor = 100.0f;

        Float pressure = scaleValue(payload.pressure(), divisor, 300, 1100);
        Float temperature = scaleValue(payload.temperature(), divisor, -40.0f, 85.0f);
        Float humidity = scaleValue(payload.humidity(), divisor, 0 , 100.0f);
        Long lightIntensity = null;

        if (payload.lightIntensity() != null){
            if (payload.lightIntensity() >= 0 && payload.lightIntensity() <= 65535) lightIntensity = payload.lightIntensity();
        }

        return new WeatherTelemetry(dateTime, deviceId, pressure, temperature, humidity, lightIntensity);
    }

    private Float scaleValue(Number rawValue, float divisor, float minValue, float maxValue){
        if (rawValue == null) return null;
        float scaledValue = rawValue.floatValue() / divisor;

        if (scaledValue >= minValue && scaledValue <= maxValue){
            return scaledValue;
        }
        return null;
    }
}
