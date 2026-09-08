package wrzepka.backend;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.Test;

import static org.assertj.core.api.Assertions.assertThat;

public class TelemetryPayloadTests {
    private final ObjectMapper objectMapper = new ObjectMapper();

    @Test
    void shouldDeserializeValidSnakeCaseJson() throws Exception {
        String json = """
            {
                "light_intensity": 120,
                "temperature": 2150,
                "humidity": 5000,
                "pressure": 101325
            }
            """;

        TelemetryPayload payload = objectMapper.readValue(json, TelemetryPayload.class);

        assertThat(payload).isNotNull();
        assertThat(payload.lightIntensity()).isEqualTo(120L);
        assertThat(payload.temperature()).isEqualTo(2150);
        assertThat(payload.humidity()).isEqualTo(5000L);
        assertThat(payload.pressure()).isEqualTo(101325L);
    }

    @Test
    void shouldHandleLackOfMeasurement() throws Exception {
        String json = """
            {
                "light_intensity": 120,
                "temperature": 2150,
                "humidity": 5000
            }
            """;

        TelemetryPayload payload = objectMapper.readValue(json, TelemetryPayload.class);

        assertThat(payload).isNotNull();
        assertThat(payload.lightIntensity()).isEqualTo(120L);
        assertThat(payload.temperature()).isEqualTo(2150);
        assertThat(payload.humidity()).isEqualTo(5000L);
        assertThat(payload.pressure()).isNull();
    }

    @Test
    void shouldHandleNotKnownKeys() throws Exception {
        String json = """
            {
                "light_intensity": 120,
                "temperature": 2150,
                "humidity": 5000,
                "pressure": 101325,
                "wind_speed" : 60123
            }
            """;

        TelemetryPayload payload = objectMapper.readValue(json, TelemetryPayload.class);

        assertThat(payload).isNotNull();
        assertThat(payload.lightIntensity()).isEqualTo(120L);
        assertThat(payload.temperature()).isEqualTo(2150);
        assertThat(payload.humidity()).isEqualTo(5000L);
        assertThat(payload.pressure()).isEqualTo(101325L);
    }
}
