package wrzepka.backend;


import com.fasterxml.jackson.databind.PropertyNamingStrategies;
import com.fasterxml.jackson.databind.annotation.JsonNaming;

@JsonNaming(PropertyNamingStrategies.SnakeCaseStrategy.class)
public record TelemetryPayload(
        Long lightIntensity,
        Integer temperature,
        Long humidity,
        Long pressure
) {
}
