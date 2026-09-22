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
@Transactional
public class TelemetryIngestionService {
    /**
     * Telemetry mapper object used for creating WeatherTelemetry entity.
     */
    private final TelemetryMapper telemetryMapper;

    /**
     * Telemetry repository object used for saving newly ingested record.
     */
    private final WeatherTelemetryRepository repository;

    /**
     * Logger object used for message logging.
     */
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
        logger.info("Telemetry saved from station: {}.", deviceId);
    }
}
