char t = ' '; // Initialisation pour éviter un comportement indéfini
int ena = 3;
int enb = 11;
int in1 = 4;
int in2 = 5;
int in3 = 9;
int in4 = 10;
void av() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, 250);
  analogWrite(enb, 250);
}


void droite() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, 50);
  analogWrite(enb, 250);
}

void gauche() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, 200);
  analogWrite(enb, 85);
}

void STOP() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  Serial.begin(9600);
  av();
  delay(2000);
gauche();
delay(2000);

}



void loop() {

}