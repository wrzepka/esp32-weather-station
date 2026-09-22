package wrzepka.backend;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.time.OffsetDateTime;

import static org.assertj.core.api.Assertions.assertThatThrownBy;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
public class TelemetryIngestionServiceTests {
    @Mock
    private TelemetryMapper telemetryMapper;

    @Mock
    private WeatherTelemetryRepository repository;

    @InjectMocks
    private TelemetryIngestionService service;

    @Test
    void shouldSaveNewTelemetryWhenInputIsValid() {
        TelemetryPayload payload = new TelemetryPayload(120L, 2150, 5000L, 101325L);
        String deviceId = "TEST";
        OffsetDateTime time = OffsetDateTime.parse("2020-01-01T12:00:00Z");
        WeatherTelemetry expectedEntity = new WeatherTelemetry();

        when(telemetryMapper.toEntity(payload, deviceId, time)).thenReturn(expectedEntity);

        service.saveTelemetry(payload, deviceId, time);

        verify(repository).save(expectedEntity);
    }

    @Test
    void shouldThrowIllegalArgumentExceptionWhenArgsAreNullish() {
        assertThatThrownBy(() -> service.saveTelemetry(null, null, null)).isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Arguments cannot be null.");
        verifyNoInteractions(telemetryMapper, repository);
    }

    @Test
    void shouldThrowIllegalArgumentExceptionWhenOneArgIsNullish() {
        String deviceId = "TEST";
        OffsetDateTime time = OffsetDateTime.parse("2020-01-01T12:00:00Z");
        assertThatThrownBy(() -> service.saveTelemetry(null, deviceId, time)).isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Arguments cannot be null.");
        verifyNoInteractions(telemetryMapper, repository);
    }

    @Test
    void shouldThrowIllegalArgumentExceptionWhenDeviceIdIsBlank() {
        TelemetryPayload payload = new TelemetryPayload(120L, 2150, 5000L, 101325L);
        String deviceId = "";
        OffsetDateTime time = OffsetDateTime.parse("2020-01-01T12:00:00Z");

        assertThatThrownBy(() -> service.saveTelemetry(payload, deviceId, time)).isInstanceOf(IllegalArgumentException.class).hasMessageContaining("DeviceID cannot be blank.");
        verifyNoInteractions(telemetryMapper, repository);
    }
}
