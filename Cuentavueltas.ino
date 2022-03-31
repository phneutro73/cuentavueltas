#include <LiquidCrystal_I2C.h>
#include <Wire.h>


LiquidCrystal_I2C lcd1(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x28, 16, 2);
LiquidCrystal_I2C lcd3(0x26, 16, 2);


const byte pinReset = 7;

const byte semaforo1 = 9;
const byte semaforo2 = 10;
const byte semaforo3 = 11;
const byte semaforo4 = 12;
const byte semaforo5 = 13;

const int timeThreshold = 2000;

int reset = HIGH;

int x = 0;

volatile int cuentaVueltas1 = -1;
volatile int cuentaVueltas2 = -1;
volatile int anteriorVueltas1 = -1;
volatile int anteriorVueltas2 = -1;

float horas, minutos, segundos, decimas;
long tiempoInicio, tiempoFinal, tiempoTrans, parcial, parcialIni, startTime;

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

int segundosAnt = 0;

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
  pinMode(semaforo4, OUTPUT);
  pinMode(semaforo5, OUTPUT);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(2), hayObstaculo1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), hayObstaculo2, CHANGE);

  lcd1.init();
  lcd2.init();
  lcd3.init();
  lcd1.backlight();
  lcd2.backlight();
  lcd3.backlight();

  lcd1.createChar(0, LT);
  lcd1.createChar(1, UB);
  lcd1.createChar(2, RT);
  lcd1.createChar(3, LL);
  lcd1.createChar(4, LB);
  lcd1.createChar(5, LR);
  lcd1.createChar(6, UMB);
  lcd1.createChar(7, LMB);

  lcd2.createChar(0, LT);
  lcd2.createChar(1, UB);
  lcd2.createChar(2, RT);
  lcd2.createChar(3, LL);
  lcd2.createChar(4, LB);
  lcd2.createChar(5, LR);
  lcd2.createChar(6, UMB);
  lcd2.createChar(7, LMB);

  lcd3.createChar(0, LT);
  lcd3.createChar(1, UB);
  lcd3.createChar(2, RT);
  lcd3.createChar(3, LL);
  lcd3.createChar(4, LB);
  lcd3.createChar(5, LR);
  lcd3.createChar(6, UMB);
  lcd3.createChar(7, LMB);

  lcd1.clear();
  lcd1.setCursor(1, 0);
  lcd1.print("Cuentavueltas");
  lcd1.setCursor(5, 1);
  lcd1.print("v 2.0");

  lcd2.clear();
  lcd2.setCursor(1, 0);
  lcd2.print("Cuentavueltas");
  lcd2.setCursor(5, 1);
  lcd2.print("v 2.0");

  lcd3.clear();
  lcd3.setCursor(1, 0);
  lcd3.print("Cuentavueltas");
  lcd3.setCursor(5, 1);
  lcd3.print("v 2.0");
  
  delay(2000);
  
  salida();

}

void loop() {
  long tiempo1;
  long tiempo2;
  long tiempo3;
  long parcialAux;
  
  reset = digitalRead(pinReset);
  if (reset == LOW) {
       
    lcd1.clear();
    lcd1.setCursor(0, 1);
    lcd2.clear();
    lcd2.setCursor(0, 1);
    lcd1.print(" =(RESETEANDO)=");
    lcd2.print(" =(RESETEANDO)=");
    delay(3000);

    salida();
   
  }
  tiempo1 = calculaTiempo(parcialIni1, parcial1);
  if (cuentaVueltas1 > 0 && millis() > 1000) {

    if (cuentaVueltas1 > anteriorVueltas1) {
      if (cuentaVueltas1 > 0) {
        lcd1.clear();
        mostrarUnidades1(cuentaVueltas1);
      }
      
      anteriorVueltas1 = cuentaVueltas1;
      
      if (tiempo1 < mejorTiempo1){
        mejorTiempo1 = tiempo1;
        mejorVuelta1 = cuentaVueltas1;        
      }
      lcd1.setCursor(7,0);
      lcd1.print("Vuelta");
      if (mejorVuelta1 < 10){
        lcd1.setCursor(15,0);
      }else{
         lcd1.setCursor(14,0);
      }
      lcd1.print(mejorVuelta1);
        
      lcd1.setCursor(9,1); 
      pintarTiempo1(mejorTiempo1);
             
      parcialIni1 = millis();
    }
  }
  
  tiempo2 = calculaTiempo(parcialIni2, parcial2);
  if (cuentaVueltas2 > 0 && millis() > 1000) {

    if (cuentaVueltas2 > anteriorVueltas2) {
      if (cuentaVueltas2 > 0) {
        lcd2.clear();
        mostrarUnidades2(cuentaVueltas2);
      }
      
      anteriorVueltas2 = cuentaVueltas2;
      
      if (tiempo2 < mejorTiempo2){
        mejorTiempo2 = tiempo2;
        mejorVuelta2 = cuentaVueltas2;        
      }
      lcd2.setCursor(0,0);
      lcd2.print("Vuelta");
      lcd2.setCursor(7,0);
      lcd2.print(mejorVuelta2);
    
      lcd2.setCursor(0,1); 
      pintarTiempo2(mejorTiempo2);
             
      parcialIni2 = millis();
    }
  }  
  
  parcialAux = calculaTiempo(parcial, tiempoFinal);

  tiempo3 = calculaTiempo(tiempoInicio, tiempoFinal);

  if (parcialAux > 900){ 
    lcd3.clear();
    pintarTiempo(tiempo3); //Este es el reloj de la carrera y se imprime cada décima de segundo por el delay de abajo
    parcial = millis();
  }

  delay(100); //El loop se ejecuta cada décima de segundo
  tiempoFinal = millis();
}

void salida() {
  lcd1.clear();
  lcd2.clear();
  lcd3.clear();

  lcd1.setCursor(1,0);
  lcd1.print("-=( SALIDA )=-");
  lcd2.setCursor(1,0);
  lcd2.print("-=( SALIDA )=-");
  delay(3000);
  
  digitalWrite(semaforo1, HIGH);
  delay(1000);
  digitalWrite(semaforo2, HIGH);
  delay(1000);
  digitalWrite(semaforo3, HIGH);
  delay(1000);
  digitalWrite(semaforo4, HIGH);
  delay(1000);
  digitalWrite(semaforo5, HIGH);
  delay(1000);
  
  digitalWrite(semaforo1, LOW);
  digitalWrite(semaforo2, LOW);
  digitalWrite(semaforo3, LOW);
  digitalWrite(semaforo4, LOW);
  digitalWrite(semaforo5, LOW);
 
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

  parcial = millis();
  
  lcd1.clear();
  lcd2.clear();
  
  mostrarUnidades1(0);
  mostrarUnidades2(0);
  mostrarUnidades3(0,0);
}


long calculaTiempo(long tiempoIni, long tiempoFin) {
  long tiempo;

  tiempo = tiempoFin - tiempoIni;

  return tiempo;

}

void pintarTiempo(long tiempo) {
  long terminado;
  
  horas = int(tiempo / 3600000);
  terminado = tiempo % 3600000;
  minutos = int(terminado / 60000);
  terminado = terminado % 60000;
  segundos = int(terminado / 1000);
  decimas = terminado % 10;

  mostrarUnidades3(minutos, segundos);
 
}

void pintarTiempo1(long tiempo) {
  long terminado;
  
  horas = int(tiempo / 3600000);
  terminado = tiempo % 3600000;
  minutos = int(terminado / 60000);
  terminado = terminado % 60000;
  segundos = int(terminado / 1000);
  decimas = terminado % 10;

  if (minutos < 10) {
    lcd1.print("0");
    lcd1.print(minutos, 0);
  } else {
    lcd1.print(minutos, 0);
  }
  lcd1.print(":");
  if (segundos < 10) {
    lcd1.print("0");
    lcd1.print(segundos, 0);
  } else {
    lcd1.print(segundos, 0);
  }
  lcd1.print(".");
  lcd1.print(decimas, 0);
  
}

void pintarTiempo2(long tiempo) {
  long terminado;
  
  horas = int(tiempo / 3600000);
  terminado = tiempo % 3600000;
  minutos = int(terminado / 60000);
  terminado = terminado % 60000;
  segundos = int(terminado / 1000);
  decimas = terminado % 10;
  
  if (minutos < 10) {
    lcd2.print("0");
    lcd2.print(minutos, 0);
  } else {
    lcd2.print(minutos, 0);
  }
  lcd2.print(":");
  if (segundos < 10) {
    lcd2.print("0");
    lcd2.print(segundos, 0);
  } else {
    lcd2.print(segundos, 0);
  }
  lcd2.print(".");
  lcd2.print(decimas, 0);
  
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
void mostrarUnidades1(int unidad)
{
  int numero;
  x = 0;
  numero = unidad / 10;
  mostrarNumero1(numero);

  x = 3;
  numero = unidad % 10;
  mostrarNumero1(numero);
}
void custom01(){
  lcd1.setCursor(x, 0); 
  lcd1.write((byte)0);
  lcd1.write(1); 
  lcd1.write(2);
  lcd1.setCursor(x, 1); 
  lcd1.write(3);
  lcd1.write(4);
  lcd1.write(5);
}
void custom11(){
  lcd1.setCursor(x, 0);
  lcd1.write(1);
  lcd1.write(2);
  lcd1.setCursor(x + 1, 1);
  lcd1.write(5);
}
void custom21(){
  lcd1.setCursor(x, 0);
  lcd1.write(6);
  lcd1.write(6);
  lcd1.write(2);
  lcd1.setCursor(x, 1);
  lcd1.write(3);
  lcd1.write(7);
  lcd1.write(7);
}
void custom31(){
  lcd1.setCursor(x, 0);
  lcd1.write(6);
  lcd1.write(6);
  lcd1.write(2);
  lcd1.setCursor(x, 1);
  lcd1.write(7);
  lcd1.write(7);
  lcd1.write(5);
}
void custom41(){
  lcd1.setCursor(x, 0);
  lcd1.write(3);
  lcd1.write(4);
  lcd1.write(2);
  lcd1.setCursor(x + 2, 1);
  lcd1.write(5);
}
void custom51(){
  lcd1.setCursor(x, 0);
  lcd1.write((byte)0);
  lcd1.write(6);
  lcd1.write(6);
  lcd1.setCursor(x, 1);
  lcd1.write(7);
  lcd1.write(7);
  lcd1.write(5);
}
void custom61(){
  lcd1.setCursor(x, 0);
  lcd1.write((byte)0);
  lcd1.write(6);
  lcd1.write(6);
  lcd1.setCursor(x, 1);
  lcd1.write(3);
  lcd1.write(7);
  lcd1.write(5);
}
void custom71(){
  lcd1.setCursor(x, 0);
  lcd1.write(1);
  lcd1.write(1);
  lcd1.write(2);
  lcd1.setCursor(x + 1, 1);
  lcd1.write((byte)0);
}
void custom81(){
  lcd1.setCursor(x, 0);
  lcd1.write((byte)0);
  lcd1.write((byte)6);
  lcd1.write(2);
  lcd1.setCursor(x, 1);
  lcd1.write(3);
  lcd1.write(7);
  lcd1.write(5);
}
void custom91(){
  lcd1.setCursor(x, 0);
  lcd1.write((byte)0);
  lcd1.write((byte)6);
  lcd1.write((byte)2);
  lcd1.setCursor(x + 2, 1);
  lcd1.write((byte)5);
}

void mostrarNumero1(int numero) 
{
  switch (numero)
  {
    case 0: custom01();
      break;
    case 1: custom11();
      break;
    case 2: custom21();
      break;
    case 3: custom31();
      break;
    case 4: custom41();
      break;
    case 5: custom51();
      break;
    case 6: custom61();
      break;
    case 7: custom71();
      break;
    case 8: custom81();
      break;
    case 9: custom91();
      break;

  }
}

void mostrarUnidades2(int unidad)
{
  int numero;
  x = 10;
  numero = unidad / 10;
  mostrarNumero2(numero);

  x = 13;
  numero = unidad % 10;
  mostrarNumero2(numero);
}
void custom02(){
  lcd2.setCursor(x, 0); 
  lcd2.write((byte)0);
  lcd2.write(1); 
  lcd2.write(2);
  lcd2.setCursor(x, 1); 
  lcd2.write(3);
  lcd2.write(4);
  lcd2.write(5);
}
void custom12(){
  lcd2.setCursor(x, 0);
  lcd2.write(1);
  lcd2.write(2);
  lcd2.setCursor(x + 1, 1);
  lcd2.write(5);
}
void custom22(){
  lcd2.setCursor(x, 0);
  lcd2.write(6);
  lcd2.write(6);
  lcd2.write(2);
  lcd2.setCursor(x, 1);
  lcd2.write(3);
  lcd2.write(7);
  lcd2.write(7);
}
void custom32(){
  lcd2.setCursor(x, 0);
  lcd2.write(6);
  lcd2.write(6);
  lcd2.write(2);
  lcd2.setCursor(x, 1);
  lcd2.write(7);
  lcd2.write(7);
  lcd2.write(5);
}
void custom42(){
  lcd2.setCursor(x, 0);
  lcd2.write(3);
  lcd2.write(4);
  lcd2.write(2);
  lcd2.setCursor(x + 2, 1);
  lcd2.write(5);
}
void custom52(){
  lcd2.setCursor(x, 0);
  lcd2.write((byte)0);
  lcd2.write(6);
  lcd2.write(6);
  lcd2.setCursor(x, 1);
  lcd2.write(7);
  lcd2.write(7);
  lcd2.write(5);
}
void custom62(){
  lcd2.setCursor(x, 0);
  lcd2.write((byte)0);
  lcd2.write(6);
  lcd2.write(6);
  lcd2.setCursor(x, 1);
  lcd2.write(3);
  lcd2.write(7);
  lcd2.write(5);
}
void custom72(){
  lcd2.setCursor(x, 0);
  lcd2.write(1);
  lcd2.write(1);
  lcd2.write(2);
  lcd2.setCursor(x + 1, 1);
  lcd2.write((byte)0);
}
void custom82(){
  lcd2.setCursor(x, 0);
  lcd2.write((byte)0);
  lcd2.write((byte)6);
  lcd2.write(2);
  lcd2.setCursor(x, 1);
  lcd2.write(3);
  lcd2.write(7);
  lcd2.write(5);
}
void custom92(){
  lcd2.setCursor(x, 0);
  lcd2.write((byte)0);
  lcd2.write((byte)6);
  lcd2.write((byte)2);
  lcd2.setCursor(x + 2, 1);
  lcd2.write((byte)5);
}

void mostrarNumero2(int numero) 
{
  switch (numero)
  {
    case 0: custom02();
      break;
    case 1: custom12();
      break;
    case 2: custom22();
      break;
    case 3: custom32();
      break;
    case 4: custom42();
      break;
    case 5: custom52();
      break;
    case 6: custom62();
      break;
    case 7: custom72();
      break;
    case 8: custom82();
      break;
    case 9: custom92();
      break;

  }
}

void mostrarUnidades3(int minutos, int segundos)
{
  int numero;
  if (segundosAnt != segundos){
    lcd3.clear();
  
    x = 1;
    numero = minutos / 10;
    mostrarNumero3(numero);
  
    x = 4;
    numero = minutos % 10;
    mostrarNumero3(numero);
  
    x = 8;
    customDot();
  
    x = 10;
    numero = segundos / 10;
    mostrarNumero3(numero);
  
    x = 13;
    numero = segundos % 10;
    mostrarNumero3(numero);
    segundosAnt = segundos;
  }
}
void custom03(){
  lcd3.setCursor(x, 0); 
  lcd3.write((byte)0);
  lcd3.write(1); 
  lcd3.write(2);
  lcd3.setCursor(x, 1); 
  lcd3.write(3);
  lcd3.write(4);
  lcd3.write(5);
}
void custom13(){
  lcd3.setCursor(x, 0);
  lcd3.write(1);
  lcd3.write(2);
  lcd3.setCursor(x + 1, 1);
  lcd3.write(5);
}
void custom23(){
  lcd3.setCursor(x, 0);
  lcd3.write(6);
  lcd3.write(6);
  lcd3.write(2);
  lcd3.setCursor(x, 1);
  lcd3.write(3);
  lcd3.write(7);
  lcd3.write(7);
}
void custom33(){
  lcd3.setCursor(x, 0);
  lcd3.write(6);
  lcd3.write(6);
  lcd3.write(2);
  lcd3.setCursor(x, 1);
  lcd3.write(7);
  lcd3.write(7);
  lcd3.write(5);
}
void custom43(){
  lcd3.setCursor(x, 0);
  lcd3.write(3);
  lcd3.write(4);
  lcd3.write(2);
  lcd3.setCursor(x + 2, 1);
  lcd3.write(5);
}
void custom53(){
  lcd3.setCursor(x, 0);
  lcd3.write((byte)0);
  lcd3.write(6);
  lcd3.write(6);
  lcd3.setCursor(x, 1);
  lcd3.write(7);
  lcd3.write(7);
  lcd3.write(5);
}
void custom63(){
  lcd3.setCursor(x, 0);
  lcd3.write((byte)0);
  lcd3.write(6);
  lcd3.write(6);
  lcd3.setCursor(x, 1);
  lcd3.write(3);
  lcd3.write(7);
  lcd3.write(5);
}
void custom73(){
  lcd3.setCursor(x, 0);
  lcd3.write(1);
  lcd3.write(1);
  lcd3.write(2);
  lcd3.setCursor(x + 1, 1);
  lcd3.write((byte)0);
}
void custom83(){
  lcd3.setCursor(x, 0);
  lcd3.write((byte)0);
  lcd3.write((byte)6);
  lcd3.write(2);
  lcd3.setCursor(x, 1);
  lcd3.write(3);
  lcd3.write(7);
  lcd3.write(5);
}
void custom93(){
  lcd3.setCursor(x, 0);
  lcd3.write((byte)0);
  lcd3.write((byte)6);
  lcd3.write((byte)2);
  lcd3.setCursor(x + 2, 1);
  lcd3.write((byte)5);
}
void customDot(){
  lcd3.setCursor(x, 0);
  lcd3.write(4);
  lcd3.setCursor(x, 1);
  lcd3.write(4);
}

void mostrarNumero3(int numero) 
{
  switch (numero)
  {
    case 0: custom03();
      break;
    case 1: custom13();
      break;
    case 2: custom23();
      break;
    case 3: custom33();
      break;
    case 4: custom43();
      break;
    case 5: custom53();
      break;
    case 6: custom63();
      break;
    case 7: custom73();
      break;
    case 8: custom83();
      break;
    case 9: custom93();
      break;

  }
}
