from flask import Flask
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)

#mqtt_broker = "broker.hivemq.com"
mqtt_broker = "broker.mqtt.cool"
mqtt_topic = "Alic/MQTT"

sensor_data = {"temperatura": None, "humedad": None, "bateria": None}

def on_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    topic = message.topic

    if topic == mqtt_topic:
        try:
            data = json.loads(payload)
            sensor_data["temperatura"] = data.get("t")
            sensor_data["humedad"] = data.get("h")
            sensor_data["bateria"] = data.get("b")
        except json.JSONDecodeError as e:
            print(f"Error al decodificar JSON: {e}")

# Configurar el cliente MQTT
client = mqtt.Client()
#client.username_pw_set("santipc", "Santi123")
client.on_message = on_message
client.connect(mqtt_broker, 1883, 60)
client.subscribe(mqtt_topic,)
client.loop_start()

@app.route('/', methods=['GET'])
def get_sensor_data():
    return f'''
    <H1>Temperatura: {sensor_data["temperatura"]}</H1>
    <H1>Humedad: {sensor_data["humedad"]}</H1>
    <H1>Batería: {sensor_data["bateria"]}</H1>
    <img src="https://i.pinimg.com/originals/de/d5/f8/ded5f8698df8ccec3fe3a0823d086460.gif" alt="Your GIF">
    '''































@app.route('/Gracias', methods=['GET'])
def profe_gracias():
    return f'''
    <H1>Gracias por todo, que nota de curso</H1>
    <img src="https://i.pinimg.com/originals/de/d5/f8/ded5f8698df8ccec3fe3a0823d086460.gif" alt="Your GIF">
    '''
if __name__ == '__main__':
    app.run(debug=True, port=5001)
