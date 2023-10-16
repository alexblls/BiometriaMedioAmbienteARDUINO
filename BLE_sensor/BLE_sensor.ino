// Incluir librerias
#include <bluefruit.h>

// Definir una instancia de BLEUart para comunicación Bluetooth
BLEUart bleuart;

// Definir pines para lecturas analógicas
int gasPin = 5;  // Pin para lectura de sensor de gas
int refPin = 28; // Pin para lectura de referencia
int tempPin = 29; // Pin para lectura de temperatura

//float arrayt[60];
//int cont;
 
void setup() {
  Serial.begin(115200);

  // Configurar pines como entrada
  pinMode(5, INPUT);
  pinMode(28, INPUT);
  pinMode(29, INPUT);

  // Inicializar el módulo Bluetooth
  Bluefruit.begin();

  // Configurar el nombre del dispositivo Bluetooth
  Bluefruit.setName("PROY3A-FONDO-SUR");
  Bluefruit.ScanResponse.addName();

  //for (int i = 0; i < sizeof(arrayt); i++) {
  //  arrayt[0] = 0.0;
  //}
}

void loop() {

  //Guardar la lectura analogica en una variable
  float vGas = analogRead(gasPin);
  float vRef = analogRead(refPin);
  float vTemp = analogRead(tempPin);

  //Normalizar la lectura anterior con la llamada a la funcion normalizar
  float vGasN = normalizar(vGas);
  float vRefN = normalizar(vRef);
  float vTempN = normalizar(vTemp);

  //Calculamos vMedida
  float vMedida = vGasN - vRefN;
  if (vMedida < 0) {
    vMedida = 0;
  }


//  if (cont >= (sizeof(arrayt) - 1)) {
//    cont = 0;
//    
//  }
//
//  arrayt[cont] = vMedida;
//  Serial.println();
//  Serial.println("Datos array");
//  Serial.println();
//  Serial.println("cont ");
//  Serial.println(cont);
//  Serial.println(arrayt[cont]);
//  
//  Serial.println();
//  cont++;

  

  //Calculamos la concentracion
  float m = 41.26 * 499 * pow(10, -6);

  float concentracion = vMedida / m;

  uint8_t concentracionInt = concentracion * 10;

  //Impresion de datos por el monitor serie
  Serial.print(vGas);
  Serial.println(" vGas");
  Serial.print(vRef);
  Serial.println(" vRef");
  Serial.print(vTemp);
  Serial.println(" vTemp");
  Serial.println();
  Serial.print(vGasN);
  Serial.println(" vGasN");
  Serial.print(vRefN);
  Serial.println(" vRefN");
  Serial.print(vTempN);
  Serial.println(" vTempN");
  Serial.println();
  Serial.print(calcularTemperatura(vTempN));
  Serial.println(" ºC");
  Serial.print(m);
  Serial.println(" M");
  Serial.print(vMedida);
  Serial.println(" Vmedida");
  Serial.println();
  Serial.print(concentracion);
  Serial.println(" concentracion");
  Serial.println();
  Serial.println();

  startAdvertising(concentracion);

  delay(1100);//Espera de 1100ms
}

float normalizar(float valor) {
  float respuesta = (valor * 3.3) / (pow(2, 12) - 1);
  return respuesta;
}

/**
 * @brief Función para calcular la temperatura detectada por el sensor.
 * @param valor Valor normalizado de temperatura.
 * @return Temperatura calculada en grados Celsius.
 */
float calcularTemperatura(float valor) {
  float temperatura = 87 * valor - 18;
  if (temperatura > 20) {
    float correccion = (temperatura - 20) * 0.3;
    temperatura = temperatura - correccion;
  }
  return temperatura;
}

/**
 * @brief Iniciar la publicidad Bluetooth a traves de beacons con una concentración calculada.
 * @param ppm Concentración en partes por millón (ppm).
 */
void startAdvertising(uint8_t ppm) {

  Bluefruit.Advertising.stop(); // Detiene la publicidad Bluetooth actual

  // Paquete de publicidad
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Incuimos el nombre del dispositivo
  Bluefruit.Advertising.addName();

  // Define un identificador único universal (UUID) para el dispositivo
  uint8_t beaconUUID[16] = {'P', 'R', 'O', 'Y', '3', 'A', '-', 'F', 'O', 'N', 'D', 'O', '-', 'S', 'U', 'R'};

  // Configura un objeto BLEBeacon con el UUID, el major, el minor y otro
  BLEBeacon elBeacon(beaconUUID, 98, 150, 10);
  //BLEBeacon elBeacon(beaconUUID, ppm, 150, 10);
  elBeacon.setManufacturer( 0x004c ); // aple id

  // Configura los parámetros de la publicidad Bluetooth
  Bluefruit.Advertising.setBeacon(elBeacon);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}
