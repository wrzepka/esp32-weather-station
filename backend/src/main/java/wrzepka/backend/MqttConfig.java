package wrzepka.backend;

import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import org.jspecify.annotations.NonNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.eclipse.paho.mqttv5.client.MqttConnectionOptions;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.core.Mqttv5ClientManager;
import org.springframework.integration.mqtt.inbound.Mqttv5PahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.support.MqttHeaders;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import org.springframework.messaging.MessagingException;
import com.fasterxml.jackson.databind.ObjectMapper;

import java.time.OffsetDateTime;

@Configuration
public class MqttConfig {

    private final WeatherTelemetryRepository repository;
    private final ObjectMapper objectMapper;
    private final TelemetryMapper telemetryMapper;
    private final static Logger logger = LoggerFactory.getLogger(MqttConfig.class);


    public MqttConfig(WeatherTelemetryRepository repository, TelemetryMapper telemetryMapper) {
        this.repository = repository;
        this.objectMapper = new ObjectMapper();
        this.objectMapper.registerModule(new JavaTimeModule());
        this.telemetryMapper = telemetryMapper;
    }

    @Bean
    public Mqttv5ClientManager mqttv5ClientManager() {
        MqttConnectionOptions options = new MqttConnectionOptions();
        options.setAutomaticReconnect(true);
        options.setCleanStart(true);
        options.setServerURIs(new String[]{"tcp://mosquitto:1883"});

        return new Mqttv5ClientManager(options, "spring-weather-consumer");
    }

    @Bean
    public MessageChannel mqttInputChannel() {
        return new DirectChannel();
    }

    @Bean
    public MessageProducer inbound() {
        Mqttv5PahoMessageDrivenChannelAdapter adapter = new Mqttv5PahoMessageDrivenChannelAdapter(
                mqttv5ClientManager(),
                "iot/weather/+"
        );

        adapter.setCompletionTimeout(5000);
        adapter.setQos(1);
        adapter.setOutputChannel(mqttInputChannel());

        return adapter;
    }

    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler handler() {
        return new MessageHandler() {
            @Override
            public void handleMessage(@NonNull Message<?> message) throws MessagingException {
                String payload;
                String topic = message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC, String.class);

                if (topic == null || !topic.contains("/")){
                    throw new IllegalArgumentException();
                }

                String deviceId = topic.substring(topic.lastIndexOf('/') + 1);

                if (deviceId.isBlank()){
                    throw new MessagingException("DeviceId in MQTT topic is blank");
                }

                if (message.getPayload() instanceof byte[]) {
                    payload = new String((byte[]) message.getPayload());
                } else {
                    payload = message.getPayload().toString();
                }

                try {
                    TelemetryPayload telemetryPayload = objectMapper.readValue(payload, TelemetryPayload.class);

                    WeatherTelemetry weatherTelemetry = telemetryMapper.toEntity(telemetryPayload, deviceId, OffsetDateTime.now());

                    repository.save(weatherTelemetry);
                    logger.info("Telemetry saved from station: {}.", deviceId);

                } catch (Exception e) {
                    logger.error("Error during message passing.", e);
                }
            }
        };
    }
}
