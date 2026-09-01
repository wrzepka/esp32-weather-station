CREATE EXTENSION IF NOT EXISTS timescaledb;

create table weather_telemetry (
	date_time timestamptz not null,
	device_id varchar(32) not null,
	pressure REAL,
	temperature REAL,
	humidity REAL,
	light_intensity INTEGER,
	constraint pk_weather_telemetry PRIMARY KEY (device_id, date_time)
);

SELECT create_hypertable('weather_telemetry', 'date_time');