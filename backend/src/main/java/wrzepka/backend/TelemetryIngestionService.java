package wrzepka.backend;

import jakarta.transaction.Transactional;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.time.OffsetDateTime;

/**
 * MQTT message ingestion service.
 * It creates new WeatherTelemetry entity using {@code TelemetryMapper} and then saves it to the database via {@code WeatherTelemetryRepository}.
 */
@Service
public class TelemetryIngestionService {

    private final TelemetryMapper telemetryMapper;
    private final WeatherTelemetryRepository repository;
    private static final Logger logger = LoggerFactory.getLogger(TelemetryIngestionService.class);

    public TelemetryIngestionService(TelemetryMapper telemetryMapper, WeatherTelemetryRepository repository) {
        this.telemetryMapper = telemetryMapper;
        this.repository = repository;
    }

    public void saveTelemetry(TelemetryPayload payload, String deviceId, OffsetDateTime time) {
        if (payload == null || time == null || deviceId == null) {
            throw new IllegalArgumentException("Arguments cannot be null.");
        }

        if (deviceId.isBlank()) {
            throw new IllegalArgumentException("DeviceID cannot be blank.");
        }

        WeatherTelemetry weatherTelemetry = telemetryMapper.toEntity(payload, deviceId, time);

        repository.save(weatherTelemetry);
        logger.debug("Telemetry saved from station: {}.", deviceId);
    }
}
