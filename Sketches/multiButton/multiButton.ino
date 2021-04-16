#define LED_1 0xB
#define BTN_1 0xA
#define LED_2 0x9
#define BTN_2 0x8
#define LED_3 0x7
#define BTN_3 0x6
#define LED_4 0x5
#define BTN_4 0x4
#define LED_5 0x3
#define BTN_5 0x2

int LED_1_on = 0;
int LED_2_on = 0;
int LED_3_on = 0;
int LED_4_on = 0;
int LED_5_on = 0;

int BTN_1_dn = 0;
int BTN_2_dn = 0;
int BTN_3_dn = 0;
int BTN_4_dn = 0;
int BTN_5_dn = 0;


void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(BTN_5, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  pollSerial();
  interogateButton(BTN_5, &BTN_1_dn, 'A');
  interogateButton(BTN_4, &BTN_2_dn, 'B');
  interogateButton(BTN_3, &BTN_3_dn, 'C');
  interogateButton(BTN_2, &BTN_4_dn, 'D');
  interogateButton(BTN_1, &BTN_5_dn, 'E');
  delay(5);
}

void interogateButton(int btn, int *dn, char onKey) {
  int buttonState = !digitalRead(btn);
  *dn = buttonState ? (*dn + 1) : 0;
  if (*dn == 5) Serial.print(onKey);
}

void pollSerial() {
  if (!Serial.available()) return;
  // turnAll(HIGH);
  String income = Serial.readString();
  if (income == "hello") identify();
  if (income == "A1") digitalWrite(LED_5, HIGH);
  if (income == "A0") digitalWrite(LED_5, LOW);
  if (income == "B1") digitalWrite(LED_4, HIGH);
  if (income == "B0") digitalWrite(LED_4, LOW);
  if (income == "C1") digitalWrite(LED_3, HIGH);
  if (income == "C0") digitalWrite(LED_3, LOW);
  if (income == "D1") digitalWrite(LED_2, HIGH);
  if (income == "D0") digitalWrite(LED_2, LOW);
  if (income == "E1") digitalWrite(LED_1, HIGH);
  if (income == "E0") digitalWrite(LED_1, LOW);
  // turnAll(LOW);
  Serial.end();
  Serial.begin(9600);
}

void identify() {
  Serial.println("go");
  flash();
}

void flash() {
  for (int i = 0; i < 5; i++) {
    turnAll(HIGH);
    delay(100);
    turnAll(LOW);
    delay(100);
  }
}

void turnAll(bool state) {
  digitalWrite(LED_1, state);
  digitalWrite(LED_2, state);
  digitalWrite(LED_3, state);
  digitalWrite(LED_4, state);
  digitalWrite(LED_5, state);
}
