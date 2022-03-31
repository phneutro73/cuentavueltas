#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


const byte pinReset = 8;

const byte semaforo1 = 11;
const byte semaforo2 = 12;
const byte semaforo3 = 13;

const int timeThreshold = 1000;

int reset = HIGH;

int x = 0;

volatile int cuentaVueltas1 = -1;
volatile int cuentaVueltas2 = -1;
volatile int anteriorVueltas1 = -1;
volatile int anteriorVueltas2 = -1;

float horas, minutos, segundos, decimas;
long tiempoInicio, tiempoFinal, tiempoTrans, parcial, parcialIni, startTime, tiempoAux;

long parcial1;
long parcial2;

long parcialIni1;
long parcialIni2;

long tiempoTrans1;
long tiempoTrans2;

volatile long mejorTiempo1 = 999999;
volatile long mejorTiempo2 = 999999;

int mejorVuelta1;
int mejorVuelta2;

bool record1 = false;
bool record2 = false;

byte LT[8] =
{B01111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte UB[8] =
{B11111,B11111,B11111,B00000,B00000,B00000,B00000,B00000};
byte RT[8] =
{B11110,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte LL[8] =
{B11111,B11111,B11111,B11111,B11111,B11111,B11111,B01111};
byte LB[8] =
{B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte LR[8] =
{B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11110};
byte UMB[8] =
{B11111,B11111,B11111,B00000,B00000,B00000,B11111,B11111};
byte LMB[8] =
{B11111,B00000,B00000,B00000,B00000,B11111,B11111,B11111};

void setup() {
  pinMode(semaforo1, OUTPUT);
  pinMode(semaforo2, OUTPUT);
  pinMode(semaforo3, OUTPUT);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(2), hayObstaculo1, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(3), hayObstaculo2, FALLING);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, LT);
  lcd.createChar(1, UB);
  lcd.createChar(2, RT);
  lcd.createChar(3, LL);
  lcd.createChar(4, LB);
  lcd.createChar(5, LR);
  lcd.createChar(6, UMB);
  lcd.createChar(7, LMB);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Cuentavueltas");
  lcd.setCursor(5, 1);
  lcd.print("v 2.0");
  delay(3000);

  salida();
  mostrarUnidades(0);
}

void loop() {
  
  reset = digitalRead(pinReset);
  if (reset == LOW) {

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" ==RESETEANDO==");
    delay(3000);
    lcd.clear();
    lcd.print("  Nueva salida");
    delay(3000);
    salida();
    mostrarUnidades(0);
  }
  calculaTiempo(parcialIni1, parcial1);
  
  if (cuentaVueltas1 > 0 && millis() > 1000) {

    if (cuentaVueltas1 > anteriorVueltas1) {
      if (cuentaVueltas1 > 0) {
        lcd.clear();
        mostrarUnidades(cuentaVueltas1);
      }
      
      anteriorVueltas1 = cuentaVueltas1;
      
      if (tiempoAux < mejorTiempo1){
        mejorTiempo1 = tiempoAux;
        mejorVuelta1 = cuentaVueltas1;        
      }
      lcd.setCursor(6,0);
      lcd.print("Vuelta");
      if (mejorVuelta1 < 10){
        lcd.setCursor(15,0);
      }else{
         lcd.setCursor(14,0);
      }
      lcd.print(mejorVuelta1);
        
      lcd.setCursor(9,1); 
      pintarTiempo(mejorTiempo1);
             
      parcialIni1 = millis();
    }
  }  
  
  tiempoFinal = millis();
  calculaTiempo(tiempoInicio, tiempoFinal);
  lcd.setCursor(0,1);
  pintarTiempo(tiempoAux); //Este es el reloj de la carrera y se imprime cada décima de segundo por el delay de abajo
  delay(100); //El loop se ejecuta cada décima de segundo

}

void salida() {
  lcd.clear();
  digitalWrite(semaforo1, HIGH);
  delay(1000);
  digitalWrite(semaforo2, HIGH);
  delay(1000);
  digitalWrite(semaforo3, HIGH);
  delay(1000);
  digitalWrite(semaforo1, LOW);
  digitalWrite(semaforo2, LOW);
  digitalWrite(semaforo3, LOW);

  cuentaVueltas1 = -1;
  cuentaVueltas2 = -1;
  anteriorVueltas1 = -1;
  anteriorVueltas2 = -1;
  mejorTiempo1 = 999999;
  mejorTiempo2 = 999999;
  record1 = false;
  record2 = false;
  
  tiempoInicio = millis();
  parcialIni1 = millis();
  parcialIni2 = millis();
  startTime = 0;


}


void calculaTiempo(long tiempoIni, long tiempoFin) {
  long tiempo;

  tiempo = tiempoFin - tiempoIni;

  tiempoAux =  tiempo;

}

void pintarTiempo(long tiempo) {
  long terminado;
  
  horas = int(tiempo / 3600000);
  terminado = tiempo % 3600000;
  minutos = int(terminado / 60000);
  terminado = terminado % 60000;
  segundos = int(terminado / 1000);
  decimas = terminado % 10;
  
  if (minutos < 10) {
    lcd.print("0");
    lcd.print(minutos, 0);
  } else {
    lcd.print(minutos, 0);
  }
  lcd.print(":");
  if (segundos < 10) {
    lcd.print("0");
    lcd.print(segundos, 0);
  } else {
    lcd.print(segundos, 0);
  }
  lcd.print(".");
  lcd.print(decimas, 0);
  
}



// Con estas funciones controlamos las interrupciones de los sensores
void hayObstaculo1() {

  if (millis() - startTime > timeThreshold) {

    cuentaVueltas1++;

    parcial1 = millis();

    startTime = millis();
  }
}

void hayObstaculo2() {

  if (millis() - startTime > timeThreshold) {

    cuentaVueltas2++;

    parcial2 = millis();

    startTime = millis();
  }
}


// Todo este apartado es para pintar los números grandes en pantalla

void mostrarUnidades(int unidad)
{
  int numero;
  x = 0;
  numero = unidad / 10;
  mostrarNumero(numero);

  x = 3;
  numero = unidad % 10;
  mostrarNumero(numero);
}
void custom0(){
  lcd.setCursor(x, 0); 
  lcd.write((byte)0);
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(x, 1); 
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}
void custom1(){
  lcd.setCursor(x, 0);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x + 1, 1);
  lcd.write(5);
}
void custom2(){
  lcd.setCursor(x, 0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(7);
}
void custom3(){
  lcd.setCursor(x, 0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
}
void custom4(){
  lcd.setCursor(x, 0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(2);
  lcd.setCursor(x + 2, 1);
  lcd.write(5);
}
void custom5(){
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
}
void custom6(){
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(5);
}
void custom7(){
  lcd.setCursor(x, 0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x + 1, 1);
  lcd.write((byte)0);
}
void custom8(){
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write((byte)6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(5);
}
void custom9(){
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write((byte)6);
  lcd.write((byte)2);
  lcd.setCursor(x + 2, 1);
  lcd.write((byte)5);
}

void mostrarNumero(int numero) 
{
  switch (numero)
  {
    case 0: custom0();
      break;
    case 1: custom1();
      break;
    case 2: custom2();
      break;
    case 3: custom3();
      break;
    case 4: custom4();
      break;
    case 5: custom5();
      break;
    case 6: custom6();
      break;
    case 7: custom7();
      break;
    case 8: custom8();
      break;
    case 9: custom9();
      break;

  }
}
