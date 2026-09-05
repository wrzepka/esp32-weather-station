package wrzepka.backend;

import org.springframework.stereotype.Component;

import java.time.OffsetDateTime;

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
        final float divisor = 100.0f;

        Float pressure = scaleValue(payload.pressure(), divisor);
        Float temperature = scaleValue(payload.temperature(), divisor);
        Float humidity = scaleValue(payload.humidity(), divisor);


        return new WeatherTelemetry(dateTime, deviceId, pressure, temperature, humidity, payload.lightIntensity());
    }

    private Float scaleValue(Number rawValue, float divisor){
        return rawValue != null ? rawValue.floatValue() / divisor : null;
    }
}
