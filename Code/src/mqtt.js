let mqttClient;

window.addEventListener("load", (event) => {
    connectToBroker();
    subscribeToTopic();
    const numberArea = document.querySelector(".number");
});

function connectToBroker() {
    const clientId = "client" + Math.random().toString(36).substring(7);

    // Change this to point to your MQTT broker
    const host = "wss://phycom.it.kmitl.ac.th:8884";

    const options = {
        keepalive: 60,
        clientId: clientId,
        protocolId: "MQTT",
        protocolVersion: 4,
        clean: true,
        reconnectPeriod: 1000,
        connectTimeout: 30 * 1000,
    };

    mqttClient = mqtt.connect(host, options);

    mqttClient.on("error", (err) => {
        mqttClient.end();
    });

    // Received
    mqttClient.on("message", (topic, message, packet) => {
        const numberArea = document.querySelector(".number");
        numberArea.textContent  = message.toString();
    });
}

function subscribeToTopic() {
    const topic = "Timestop/BIGBADBEST"

    mqttClient.subscribe(topic, { qos: 0 });
}