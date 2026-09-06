package wrzepka.backend;

import org.junit.jupiter.api.Test;

import java.time.OffsetDateTime;

import static org.assertj.core.api.Assertions.*;
import static org.assertj.core.api.Assertions.within;


public class TelemetryMapperTests {
    private final TelemetryMapper mapper = new TelemetryMapper();

    @Test
    void shouldCorrectlyMapMinExtremeSensorValues() {
        TelemetryPayload testPayload = new TelemetryPayload(0L, -4050, 0L, 95000L);

        WeatherTelemetry telemetry = mapper.toEntity(testPayload, "TEST", OffsetDateTime.now());

        assertThat(telemetry.getDeviceId()).isEqualTo("TEST");
        assertThat(telemetry.getDateTime()).isNotNull();
        assertThat(telemetry.getLightIntensity()).isEqualTo(0);
        assertThat(telemetry.getHumidity()).isCloseTo(0.0f, within(0.01f));
        assertThat(telemetry.getPressure()).isCloseTo(950.0f, within(0.01f));
        assertThat(telemetry.getTemperature()).isCloseTo(-40.50f, within(0.01f));
    }

    @Test
    void shouldCorrectlyMapMaxExtremeSensorValues() {
        TelemetryPayload testPayload = new TelemetryPayload(65535L, 8500, 10000L, 105000L);

        WeatherTelemetry telemetry = mapper.toEntity(testPayload, "TEST", OffsetDateTime.now());

        assertThat(telemetry.getDeviceId()).isEqualTo("TEST");
        assertThat(telemetry.getDateTime()).isNotNull();
        assertThat(telemetry.getLightIntensity()).isEqualTo(65535);
        assertThat(telemetry.getHumidity()).isCloseTo(100.0f, within(0.01f));
        assertThat(telemetry.getPressure()).isCloseTo(1050.0f, within(0.01f));
        assertThat(telemetry.getTemperature()).isCloseTo(85.00f, within(0.01f));
    }

    @Test
    void shouldCorrectlyMapNullValues() {
        TelemetryPayload testPayload = new TelemetryPayload(0L, null, null, null);

        WeatherTelemetry telemetry = mapper.toEntity(testPayload, "TEST", OffsetDateTime.now());

        assertThat(telemetry.getDeviceId()).isEqualTo("TEST");
        assertThat(telemetry.getDateTime()).isNotNull();
        assertThat(telemetry.getLightIntensity()).isEqualTo(0);
        assertThat(telemetry.getTemperature()).isNull();
        assertThat(telemetry.getPressure()).isNull();
        assertThat(telemetry.getHumidity()).isNull();
    }

    @Test
    void shouldCorrectlyHandleBlankDeviceId(){
        TelemetryPayload testPayload = new TelemetryPayload(0L, -4050, 0L, 105000L);

        assertThatThrownBy(() -> mapper.toEntity(testPayload, "", OffsetDateTime.now()))
                .isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    void shouldCorrectlyHandleNullPayload(){
        assertThatThrownBy(() -> mapper.toEntity(null, "TEST", OffsetDateTime.now()))
                .isInstanceOf(NullPointerException.class);
    }

    @Test
    void shouldCorrectlyHandleNullDateTime(){
        TelemetryPayload testPayload = new TelemetryPayload(65535L, 8500, 10000L, 105000L);

        assertThatThrownBy(() -> mapper.toEntity(testPayload, "TEST", null))
                .isInstanceOf(NullPointerException.class);
    }

    @Test
    void shouldCorrectlyHandleNonWorkingSensor(){
        TelemetryPayload testPayload = new TelemetryPayload(null, 8500, 10000L, 105000L);

        WeatherTelemetry telemetry = mapper.toEntity(testPayload, "TEST", OffsetDateTime.now());

        assertThat(telemetry.getDeviceId()).isEqualTo("TEST");
        assertThat(telemetry.getDateTime()).isNotNull();
        assertThat(telemetry.getLightIntensity()).isNull();
        assertThat(telemetry.getHumidity()).isCloseTo(100.0f, within(0.01f));
        assertThat(telemetry.getPressure()).isCloseTo(1050.0f, within(0.01f));
        assertThat(telemetry.getTemperature()).isCloseTo(85.00f, within(0.01f));
    }
}
