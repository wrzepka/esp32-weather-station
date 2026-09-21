package wrzepka.backend;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.time.OffsetDateTime;

public class TelemetryIngestionService {
    private final TelemetryMapper telemetryMapper;
    private final WeatherTelemetryRepository repository;
    private final static Logger logger = LoggerFactory.getLogger(TelemetryIngestionService.class);

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
