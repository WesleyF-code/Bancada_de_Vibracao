#include <WiFi.h>
#include <WebServer.h>

// =====================================================
// CONFIGURAÇÃO DOS PINOS
// =====================================================

#define ENA 25
#define IN1 26
#define IN2 27

// =====================================================
// WIFI
// =====================================================

const char* ssid = "Bancada_Teste";
const char* password = "12345678";

WebServer server(80);

// =====================================================
// VARIÁVEIS
// =====================================================

int velocidade = 0;
bool motorLigado = false;

// =====================================================
// PÁGINA WEB
// =====================================================

const char paginaHTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>
<head>

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<title>Bancada de Teste</title>

<style>

body {
    font-family: Arial, sans-serif;
    text-align: center;
    margin: 0;
    padding: 20px;
    background: #f2f2f2;
}

.container {
    max-width: 500px;
    margin: auto;
    background: white;
    padding: 25px;
    border-radius: 15px;
    box-shadow: 0px 0px 15px rgba(0,0,0,0.15);
}

h1 {
    margin-bottom: 10px;
}

.status {
    font-size: 22px;
    margin: 20px;
    font-weight: bold;
}

button {

    width: 100%;
    padding: 15px;
    margin: 10px 0;

    border: none;
    border-radius: 10px;

    font-size: 18px;

    cursor: pointer;
}

.ligar {
    background: #28a745;
    color: white;
}

.desligar {
    background: #dc3545;
    color: white;
}

.slider {

    width: 100%;

}

.valor {

    font-size: 25px;
    font-weight: bold;

}

</style>

</head>

<body>

<div class="container">

<h1>Bancada de Teste</h1>

<div class="status">

Estado:
<span id="estado">
DESLIGADO
</span>

</div>

<button
    class="ligar"
    onclick="ligarMotor()">

LIGAR MOTOR

</button>

<button
    class="desligar"
    onclick="desligarMotor()">

DESLIGAR MOTOR

</button>

<h2>Velocidade</h2>

<input
    type="range"
    min="0"
    max="255"
    value="0"
    class="slider"
    id="velocidade"
    oninput="alterarVelocidade(this.value)">

<div class="valor">

PWM:
<span id="valorPWM">0</span>

</div>

</div>

<script>

function ligarMotor() {

    fetch('/ligar');

    document.getElementById("estado").innerHTML =
    "LIGADO";
}

function desligarMotor() {

    fetch('/desligar');

    document.getElementById("estado").innerHTML =
    "DESLIGADO";
}

function alterarVelocidade(valor) {

    document.getElementById("valorPWM").innerHTML =
    valor;

    fetch('/velocidade?valor=' + valor);
}

</script>

</body>
</html>

)rawliteral";

// =====================================================
// CONTROLE DO MOTOR
// =====================================================

void ligarMotor() {

    motorLigado = true;

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    analogWrite(ENA, velocidade);

}

void desligarMotor() {

    motorLigado = false;

    analogWrite(ENA, 0);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

}

// =====================================================
// ROTA PRINCIPAL
// =====================================================

void handleRoot() {

    server.send(
        200,
        "text/html",
        paginaHTML
    );

}

// =====================================================
// ROTA LIGAR
// =====================================================

void handleLigar() {

    ligarMotor();

    server.send(
        200,
        "text/plain",
        "Motor ligado"
    );

}

// =====================================================
// ROTA DESLIGAR
// =====================================================

void handleDesligar() {

    desligarMotor();

    server.send(
        200,
        "text/plain",
        "Motor desligado"
    );

}

// =====================================================
// ROTA VELOCIDADE
// =====================================================

void handleVelocidade() {

    if (server.hasArg("valor")) {

        velocidade =
            server.arg("valor").toInt();

        if (motorLigado) {

            analogWrite(
                ENA,
                velocidade
            );

        }

    }

    server.send(
        200,
        "text/plain",
        "Velocidade atualizada"
    );

}

// =====================================================
// SETUP
// =====================================================

void setup() {

    Serial.begin(115200);

    // Configuração dos pinos

    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // Motor inicialmente desligado

    desligarMotor();

    // =================================================
    // WIFI ACCESS POINT
    // =================================================

    WiFi.softAP(
        ssid,
        password
    );

    Serial.println();
    Serial.println("==============================");
    Serial.println("BANCADA DE TESTE");
    Serial.println("==============================");

    Serial.print("Rede WiFi: ");
    Serial.println(ssid);

    Serial.print("IP da bancada: ");
    Serial.println(
        WiFi.softAPIP()
    );

    // =================================================
    // SERVIDOR
    // =================================================

    server.on(
        "/",
        handleRoot
    );

    server.on(
        "/ligar",
        handleLigar
    );

    server.on(
        "/desligar",
        handleDesligar
    );

    server.on(
        "/velocidade",
        handleVelocidade
    );

    server.begin();

    Serial.println("Servidor Web iniciado!");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

    server.handleClient();

}