package wrzepka.backend;

import org.springframework.stereotype.Component;

import java.time.OffsetDateTime;

@Component
public class TelemetryMapper {

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
