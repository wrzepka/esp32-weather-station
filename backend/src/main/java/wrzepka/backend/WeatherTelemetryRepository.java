package wrzepka.backend;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

/**
 * JPA repository, managing telemetry points.
 */
@Repository
public interface WeatherTelemetryRepository extends JpaRepository<WeatherTelemetry, WeatherTelemetryId> {
}
