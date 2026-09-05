package wrzepka.backend;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

/**
 * JPA repository, managing telemetry points. {@link WeatherTelemetry}
 */
@Repository
public interface WeatherTelemetryRepository extends JpaRepository<WeatherTelemetry, WeatherTelemetryId> {
}
