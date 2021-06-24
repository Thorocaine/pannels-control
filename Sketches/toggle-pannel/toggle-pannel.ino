#include <Joystick.h>
#define LED_1 0x3
#define BTN_1 0x0

class Button
{
  public:
    Button(int ledPin, int btnPin);
    bool Check();
  private:
    int _ledPin;
    int _btnPin;
    int _pressCount;
    bool _state;
};

Button::Button(int ledPin, int btnPin)
{ 
  _state = 0;
  _pressCount = 0;
  pinMode(_ledPin = ledPin, OUTPUT);
  pinMode(_btnPin = btnPin, INPUT_PULLUP);
}

bool Button::Check()
{
  if (!digitalRead(_btnPin)) _pressCount++;
  else _pressCount = 0;

  if (_pressCount == 10) 
  {
    _state = !_state;
    digitalWrite(_ledPin, _state);
    return true;
  }
  return false;
}




Button b1(3, 0);
Button b2(5, 1);
Button b3(6, 2);
Button b4(10, 4);
Button b5(9, 7);
Joystick_ Joystick;

void setup() {
  Joystick.begin();
  Serial.begin(9600);
}

void loop() {
  if (b1.Check()) pressButton(0);
  if (b2.Check()) pressButton(1);
  if (b3.Check()) pressButton(2);
  if (b4.Check()) pressButton(3);
  if (b5.Check()) pressButton(4);  
}

void pressButton(int num) {
  Joystick.setButton(num, HIGH);
  delay(50);
  Joystick.setButton(num, LOW);
}
