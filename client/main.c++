// Includes and Definitions
#define BLYNK_TEMPLATE_ID "TMPL2OYlTI2nq"
#define BLYNK_TEMPLATE_NAME "Transportation Band"
#define BLYNK_AUTH_TOKEN "QrByEI9WtsuTwJ3aywtoN99UYoFefZWT"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Terminal Blynk
WidgetTerminal terminal(V10);

// Motors
#define E12 27
#define I1 17
#define I2 18
#define E34 19
#define I3 21
#define I4 22
#define E56 23
#define I5 25
#define I6 26

// Sensors
#define SIND_Cubo 33
#define SIND_A0 32
#define SIND_A1 34
#define SIND_Cubo2 35

// Control Variables
bool startActivo = false;
bool calibracion = false;
int estadoSistema = 0;
int calibracionSistema = 0;
bool pausaSistema = false;
bool modoRapido = false;

// WiFi credentials
char ssid[] = "iPhone";
char pass[] = "12345678";

// LED Virtual Pins
#define LEDM1 V1
#define LEDM2 V2
#define LEDCUBO2 V3
#define LEDCUBO V4
#define LEDA0 V5
#define LEDA1 V6

// Function Prototypes
void Banda1_ON();
void Banda2_ON();
void Banda1_OFF();
void Banda2_OFF();
void BandasOFF();
void Palanca_Adelante();
void Palanca_Atras();
void Palanca_STOP();
void log(String mensaje);

// ---- Blynk Handlers ----
BLYNK_WRITE(V0) {
  bool nuevoStartActivo = param.asInt();
  if (nuevoStartActivo) {
    if (pausaSistema) {
      pausaSistema = false;
    } else {
      estadoSistema = 1;
    }
    startActivo = true;
  } else {
    startActivo = false;
    pausaSistema = true;
    BandasOFF();
    Palanca_STOP();
    log("System Paused");
  }
}

BLYNK_WRITE(V7) {
  calibracion = param.asInt();
  if (calibracion) calibracionSistema = 1;
}

BLYNK_WRITE(V8) {
  modoRapido = param.asInt();
  if (modoRapido) {
    log("Fast Mode: ON");
  } else {
    log("Slow Mode: ON");
  }
}

// ---- Setup ----
void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  terminal.clear();
  terminal.println("Terminal COM Initialized");
  terminal.flush();

  pinMode(E12, OUTPUT);
  pinMode(I1, OUTPUT);
  pinMode(I2, OUTPUT);
  pinMode(E34, OUTPUT);
  pinMode(I3, OUTPUT);
  pinMode(I4, OUTPUT);
  pinMode(E56, OUTPUT);
  pinMode(I5, OUTPUT);
  pinMode(I6, OUTPUT);

  pinMode(SIND_Cubo, INPUT);
  pinMode(SIND_A0, INPUT);
  pinMode(SIND_A1, INPUT);
  pinMode(SIND_Cubo2, INPUT);

  analogWrite(E12, 0);
  analogWrite(E34, 0);
  analogWrite(E56, 0);
}

// ---- Loop ----
void loop() {
  Blynk.run();

  // LEDs virtuales de sensores
  Blynk.virtualWrite(LEDA0, digitalRead(SIND_A0) == LOW ? 255 : 0);
  Blynk.virtualWrite(LEDA1, digitalRead(SIND_A1) == HIGH ? 255 : 0);
  Blynk.virtualWrite(LEDCUBO, digitalRead(SIND_Cubo) == LOW ? 255 : 0);
  Blynk.virtualWrite(LEDCUBO2, digitalRead(SIND_Cubo2) == LOW ? 255 : 0);

  if (estadoSistema == 1 && digitalRead(SIND_Cubo) == LOW) {
    Banda1_OFF();
    Serial.println("Banda1 parada por lectura directa de sensor");
  }

  if (calibracion) {
    switch (calibracionSistema) {
      case 0:
        BandasOFF();
        Palanca_STOP();
        break;
      case 1:
        log("Testing Motor 1...");
        Banda1_ON();
        delay(200);
        Banda1_OFF();
        delay(200);
        calibracionSistema = 2;
        break;
      case 2:
        log("Testing Motor 2...");
        Banda2_ON();
        delay(200);
        Banda2_OFF();
        delay(200);
        calibracionSistema = 3;
        break;
      case 3:
        log("Testing Transfer System: Out");
        if (digitalRead(SIND_A1) == LOW) {
          Palanca_Adelante();
          Banda2_OFF();
        } else {
          Palanca_STOP();
          Banda2_OFF();
          calibracionSistema = 4;
        }
        break;
      case 4:
        if (digitalRead(SIND_A1) == HIGH) {
          calibracionSistema = 5;
        }
        break;
      case 5:
        log("Testing Transfer System: In");
        if (digitalRead(SIND_A0) == HIGH) {
          Palanca_Atras();
        } else if (digitalRead(SIND_A0) == LOW) {
          Palanca_STOP();
          calibracionSistema = 0;
          calibracion = false;
          Blynk.virtualWrite(V7, 0);
          log("Calibration Complete");
        }
        break;
    }
  } else if (startActivo && !pausaSistema) {
    switch (estadoSistema) {
      case 0:
        BandasOFF();
        Palanca_STOP();
        break;

      case 1:
        if (digitalRead(SIND_Cubo) == LOW) {
          // Ya se detuvo Banda1 antes si es necesario
          Banda2_ON();
          estadoSistema = 2;
          log("Object Detected, Starting Transfer...");
        } else {
          Banda1_ON();
          Banda2_OFF();
          Palanca_STOP();
        }
        break;

      case 2:
        if (digitalRead(SIND_A1) == LOW) {
          Palanca_Adelante();
          Banda2_ON();
        } else {
          Palanca_STOP();
          estadoSistema = 3;
          log("Transfer System: Out");
        }
        break;

      case 3:
        if (digitalRead(SIND_A1) == HIGH) {
          estadoSistema = 4;
          log("Object Moved");
        }
        break;

      case 4:
        if (digitalRead(SIND_A0) == HIGH) {
          Palanca_Atras();
        } else if (digitalRead(SIND_A0) == LOW) {
          Palanca_STOP();
          if (modoRapido) {
            estadoSistema = 1;
            log("Transfer System: In (Fast Restart)");
          } else {
            estadoSistema = 5;
            log("Transfer System: In");
          }
        }
        break;

      case 5:
        Banda2_ON();
        log("Object is in Transit...");
        if (digitalRead(SIND_Cubo2) == LOW) {
          Banda2_OFF();
          estadoSistema = 1;
          log("Object has Reached Destination");
        }
        break;
    }
  } else {
    BandasOFF();
    Palanca_STOP();
  }
}

// ---- Motor and Palanca Functions ----
void Banda1_ON() {
  digitalWrite(I1, LOW);
  digitalWrite(I2, HIGH);
  analogWrite(E12, 255);
  Blynk.virtualWrite(LEDM1, 255);
}

void Banda1_OFF() {
  digitalWrite(I1, LOW);
  digitalWrite(I2, LOW);
  analogWrite(E12, 0);
  Blynk.virtualWrite(LEDM1, 0);
}

void Banda2_ON() {
  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);
  analogWrite(E34, 255);
  Blynk.virtualWrite(LEDM2, 255);
}

void Banda2_OFF() {
  digitalWrite(I3, LOW);
  digitalWrite(I4, LOW);
  analogWrite(E34, 0);
  Blynk.virtualWrite(LEDM2, 0);
}

void BandasOFF() {
  Banda1_OFF();
  Banda2_OFF();
}

void Palanca_Adelante() {
  digitalWrite(I5, LOW);
  digitalWrite(I6, HIGH);
  analogWrite(E56, 255);
}

void Palanca_Atras() {
  digitalWrite(I5, HIGH);
  digitalWrite(I6, LOW);
  analogWrite(E56, 255);
}

void Palanca_STOP() {
  digitalWrite(I5, LOW);
  digitalWrite(I6, LOW);
  analogWrite(E56, 0);
}

// ---- Logging Function ----
void log(String mensaje) {
  Serial.println(mensaje);
  terminal.println(mensaje);
  terminal.flush();
}