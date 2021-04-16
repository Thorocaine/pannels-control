#define DIAL_INC 0xA
#define DIAL_DEC 0x6
#define DIAL_PSH 0x8
#define SWITCH   0x2

const int LOW_INC  = 0005;
const int HIGH_INC = 1000;

const long min = 118000;
const long max = 136975;
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
  int button = getCurrentButton();
  if (button == 0) return;
  //delay(200);
  setValue(button);
  // Serial.println(button);
  if (button == DIAL_PSH) setting = setting == HIGH_INC ? LOW_INC : HIGH_INC;
  if (button == SWITCH) Serial.println("SWITCH");
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
