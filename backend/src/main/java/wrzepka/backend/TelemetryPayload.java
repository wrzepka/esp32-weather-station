package wrzepka.backend;


import com.fasterxml.jackson.databind.PropertyNamingStrategies;
import com.fasterxml.jackson.databind.annotation.JsonNaming;

/**
 * Record used for acquiring raw station measurements from MQTT protocol.
 * It takes light intensity, temperature, humidity and pressure.
 *
 * @param lightIntensity Light intensity in lx.
 * @param temperature Ambient temperature in Celsius.
 * @param humidity Relative percentage humidity
 * @param pressure Absolute pressure in pascals.
 */
@JsonNaming(PropertyNamingStrategies.SnakeCaseStrategy.class)
public record TelemetryPayload(
        Long lightIntensity,
        Integer temperature,
        Long humidity,
        Long pressure
) {
}
