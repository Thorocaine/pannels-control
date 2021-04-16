#define DIAL_INC 0xA
#define DIAL_DEC 0x6
#define DIAL_PSH 0x8
#define SWITCH   0x2

const int LOW_INC  = 0005;
const int HIGH_INC = 1000;
const long min = 118000;
const long max = 136975;

int buttonHold = 0;
int setting = HIGH_INC;
long currentValue = 118100;

void setup() {
  pinMode(DIAL_INC, INPUT_PULLUP);
  pinMode(DIAL_DEC, INPUT_PULLUP);
  pinMode(DIAL_PSH, INPUT_PULLUP);
  pinMode(SWITCH, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  identify("atc-board");
  
  checkButtons();

  /*
  int button = getCurrentButton();
  if (button == 0) return;
  setValue(button);
  if (button == DIAL_PSH) setting = setting == HIGH_INC ? LOW_INC : HIGH_INC;
  if (button == SWITCH) Serial.println("SWITCH");
  */
}

void checkButtons() {
  long newValue = currentValue;
  if (checkButtonPressed(SWITCH)) Serial.println("switch");
  if (checkButtonPressed(DIAL_PSH)) setting = setting == HIGH_INC ? LOW_INC : HIGH_INC;
  if (checkButtonPressed(DIAL_INC)) newValue = currentValue + setting;
  if (checkButtonPressed(DIAL_DEC)) newValue = currentValue - setting;
  if (newValue != currentValue) Serial.println(currentValue = newValue);
}

bool checkButtonPressed(int button) {
  for(buttonHold = 0; !digitalRead(button); buttonHold++) delay(5);
  return (buttonHold >= 5);
}

bool checkButtonTurn(int button) {
  for(buttonHold = 0; buttonHold < 6 && !digitalRead(button); buttonHold++) delay(0);
  return (buttonHold >= 5);
}

void identify(String id) {
  String income = pollSerial();
  if (income == "hello") Serial.println(id);
  if (income.length() > 0) Serial.println(income);
}

String pollSerial() {
    if (!Serial.available()) return "";
    String income = Serial.readString();
    income.trim();
    return income;
}

void setValue(int button) {
  long newValue = calculateNewValue(button);
  if (newValue < min) newValue = min;
  if (newValue > max) newValue = max;
  if (newValue != currentValue) {
    currentValue = newValue;
    Serial.println(currentValue);
  }
}

long calculateNewValue(int button) {
  switch(button) {
    case DIAL_INC: return currentValue + setting;
    case DIAL_DEC: return currentValue - setting;
    default: return currentValue;
  }
}

int getCurrentButton() {
  if (!digitalRead(DIAL_INC)) return DIAL_INC;
  if (!digitalRead(DIAL_DEC)) return DIAL_DEC;
  if (!digitalRead(DIAL_PSH)) return DIAL_PSH;
  if (!digitalRead(SWITCH)) return SWITCH;
  return 0;
}
