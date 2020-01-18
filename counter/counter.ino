#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

//region Pins

// D1—A—F—D2—D3—B
// E—D—d—C—G—D4

#define PIN_IN_1 A1
#define PIN_IN_2 A2
#define PIN_IN_3 A3
#define PIN_IN_4 A4

#define PIN_OUT_A 12
#define PIN_OUT_B 13
#define PIN_OUT_C 8
#define PIN_OUT_D 6
#define PIN_OUT_E 7
#define PIN_OUT_F 11
#define PIN_OUT_G 10

#define PIN_OUT_PERIOD 9

#define PIN_OUT_SELECT_1 5
#define PIN_OUT_SELECT_2 4
#define PIN_OUT_SELECT_3 3
#define PIN_OUT_SELECT_4 2

#define PIN_OUT_RELAY A5

#define PIN_SERIAL_TX 0
#define PIN_SERIAL_RX 1

//endregion

//region Constants

enum State {
    Normal, Up, Down
};

const State states[] = {
        Normal,           // 0b0000
        Up,               // 0b0001
        Up,               // 0b0010
        Up,               // 0b0011
        Down,             // 0b0100
        Normal,           // 0b0101
        Normal,           // 0b0110
        Up,               // 0b0111
        Down,             // 0b1000
        Normal,           // 0b1001
        Normal,           // 0b1010
        Up,               // 0b1011
        Down,             // 0b1100
        Down,             // 0b1101
        Down,             // 0b1110
        Normal            // 0b1111
};

const unsigned int NormalValue = 4999;

const unsigned int WarningValueTop = 5350;
//const unsigned int WarningValueTop = 5010;
const unsigned int WarningValueBottom = 4650;
//const unsigned int WarningValueBottom = 4980;

const unsigned int MaxValue = 9999;
const unsigned int MinValue = 1000;

const unsigned int NormalSpeed = 2;
const unsigned int UpSpeed = 2;
const unsigned int DownSpeed = 2;

//endregion

//region Variables

unsigned int currentValue;
State currentState;

unsigned long nextUpdate;

unsigned int NormalUpdateTimeout;
unsigned int UpUpdateTimeout;
unsigned int DownUpdateTimeout;

bool warningActive = false;

SoftwareSerial alarmPlayerSoftwareSerial(PIN_SERIAL_RX, PIN_SERIAL_TX);
DFRobotDFPlayerMini alarmDFPlayer;

//endregion

//region InputFunctions

void UpdateState() {
    unsigned int result = 0;
    if (digitalRead(PIN_IN_1) == HIGH) {
        result |= 1u << 3u;
    }
    if (digitalRead(PIN_IN_2) == HIGH) {
        result |= 1u << 2u;
    }
    if (digitalRead(PIN_IN_3) == HIGH) {
        result |= 1u << 1u;
    }
    if (digitalRead(PIN_IN_4) == HIGH) {
        result |= 1u << 0u;
    }
    currentState = states[result];
//    currentState = Normal;
}

//endregion

//region Display

void SelectDigit(unsigned int index) {
    switch (index) {
        case 1:
            digitalWrite(PIN_OUT_SELECT_1, LOW);
            digitalWrite(PIN_OUT_SELECT_2, HIGH);
            digitalWrite(PIN_OUT_SELECT_3, HIGH);
            digitalWrite(PIN_OUT_SELECT_4, HIGH);
            break;
        case 2:
            digitalWrite(PIN_OUT_SELECT_1, HIGH);
            digitalWrite(PIN_OUT_SELECT_2, LOW);
            digitalWrite(PIN_OUT_SELECT_3, HIGH);
            digitalWrite(PIN_OUT_SELECT_4, HIGH);
            break;
        case 3:
            digitalWrite(PIN_OUT_SELECT_1, HIGH);
            digitalWrite(PIN_OUT_SELECT_2, HIGH);
            digitalWrite(PIN_OUT_SELECT_3, LOW);
            digitalWrite(PIN_OUT_SELECT_4, HIGH);
            break;
        case 4:
            digitalWrite(PIN_OUT_SELECT_1, HIGH);
            digitalWrite(PIN_OUT_SELECT_2, HIGH);
            digitalWrite(PIN_OUT_SELECT_3, HIGH);
            digitalWrite(PIN_OUT_SELECT_4, LOW);
            break;
        default:
            break;
    }
}

void ResetDigit() {
    digitalWrite(PIN_OUT_A, LOW);
    digitalWrite(PIN_OUT_B, LOW);
    digitalWrite(PIN_OUT_C, LOW);
    digitalWrite(PIN_OUT_D, LOW);
    digitalWrite(PIN_OUT_E, LOW);
    digitalWrite(PIN_OUT_F, LOW);
    digitalWrite(PIN_OUT_G, LOW);
    digitalWrite(PIN_OUT_PERIOD, LOW);
}

void DisplayDigit(unsigned int digit) {
    switch (digit) {
        case 0:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, HIGH);
            digitalWrite(PIN_OUT_F, HIGH);
            digitalWrite(PIN_OUT_G, LOW);
            break;
        case 1:
            digitalWrite(PIN_OUT_A, LOW);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, LOW);
            digitalWrite(PIN_OUT_E, LOW);
            digitalWrite(PIN_OUT_F, LOW);
            digitalWrite(PIN_OUT_G, LOW);
            break;
        case 2:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, LOW);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, HIGH);
            digitalWrite(PIN_OUT_F, LOW);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        case 3:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, LOW);
            digitalWrite(PIN_OUT_F, LOW);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        case 4:
            digitalWrite(PIN_OUT_A, LOW);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, LOW);
            digitalWrite(PIN_OUT_E, LOW);
            digitalWrite(PIN_OUT_F, HIGH);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        case 5:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, LOW);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, LOW);
            digitalWrite(PIN_OUT_F, HIGH);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        case 6:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, LOW);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, HIGH);
            digitalWrite(PIN_OUT_F, HIGH);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        case 7:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, LOW);
            digitalWrite(PIN_OUT_E, LOW);
            digitalWrite(PIN_OUT_F, LOW);
            digitalWrite(PIN_OUT_G, LOW);
            break;
        case 8:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, HIGH);
            digitalWrite(PIN_OUT_F, HIGH);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        case 9:
            digitalWrite(PIN_OUT_A, HIGH);
            digitalWrite(PIN_OUT_B, HIGH);
            digitalWrite(PIN_OUT_C, HIGH);
            digitalWrite(PIN_OUT_D, HIGH);
            digitalWrite(PIN_OUT_E, LOW);
            digitalWrite(PIN_OUT_F, HIGH);
            digitalWrite(PIN_OUT_G, HIGH);
            break;
        default:
            break;
    }
}

void DisplayValue(unsigned int value) {
    unsigned int digit1 = value / 1000;
    unsigned int digit2 = value / 100 % 10;
    unsigned int digit3 = value / 10 % 10;
    unsigned int digit4 = value % 10;

    ResetDigit();
    SelectDigit(1);
    DisplayDigit(digit1);

    ResetDigit();
    SelectDigit(2);
    DisplayDigit(digit2);

    digitalWrite(PIN_OUT_PERIOD, HIGH);

    ResetDigit();
    SelectDigit(3);
    DisplayDigit(digit3);

    ResetDigit();
    SelectDigit(4);
    DisplayDigit(digit4);
}

//endregion

//region Warning

void ActivateWarning() {
    warningActive = true;
    digitalWrite(PIN_OUT_RELAY, HIGH);
    alarmDFPlayer.play(1);
//    alarmDFPlayer.loop(1);
}

void DisableWarning() {
    warningActive = false;
    digitalWrite(PIN_OUT_RELAY, LOW);
    alarmDFPlayer.stop();
}

//endregion

//region MainLoop

void setup() {
    pinMode(PIN_IN_1, INPUT);
    pinMode(PIN_IN_2, INPUT);
    pinMode(PIN_IN_3, INPUT);
    pinMode(PIN_IN_4, INPUT);

    pinMode(PIN_OUT_A, OUTPUT);
    pinMode(PIN_OUT_B, OUTPUT);
    pinMode(PIN_OUT_C, OUTPUT);
    pinMode(PIN_OUT_D, OUTPUT);
    pinMode(PIN_OUT_E, OUTPUT);
    pinMode(PIN_OUT_F, OUTPUT);
    pinMode(PIN_OUT_G, OUTPUT);

    pinMode(PIN_OUT_PERIOD, OUTPUT);

    pinMode(PIN_OUT_SELECT_1, OUTPUT);
    pinMode(PIN_OUT_SELECT_2, OUTPUT);
    pinMode(PIN_OUT_SELECT_3, OUTPUT);
    pinMode(PIN_OUT_SELECT_4, OUTPUT);

    pinMode(PIN_OUT_RELAY, OUTPUT);

    currentValue = NormalValue;

    nextUpdate = 0;

    NormalUpdateTimeout = 1000.0 / NormalSpeed;
    UpUpdateTimeout = 1000.0 / UpSpeed;
    DownUpdateTimeout = 1000.0 / DownSpeed;

    alarmPlayerSoftwareSerial.begin(9600);

    if (!alarmDFPlayer.begin(alarmPlayerSoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
        while(true) {
            DisplayValue(9090);
        }
    }

    alarmDFPlayer.volume(25);  //Set volume value. From 0 to 30
}

void loop() {
    if (millis() >= nextUpdate) {
        UpdateState();

        switch (currentState) {
            case Normal:
                if (currentValue > NormalValue) {
                    currentValue--;
                }
                else if (currentValue < NormalValue) {
                    currentValue++;
                }
                nextUpdate = millis() + NormalUpdateTimeout;
                break;
            case Up:
                if (currentValue < MaxValue) {
                    currentValue++;
                }
                nextUpdate = millis() + UpUpdateTimeout;
                break;
            case Down:
                if (currentValue > MinValue) {
                    currentValue--;
                }
                nextUpdate = millis() + DownUpdateTimeout;
                break;
        }
    }


    DisplayValue(currentValue);

    if (currentValue >= WarningValueTop || currentValue <= WarningValueBottom) {
        if (!warningActive) {
            ActivateWarning();
        }
    } else if (warningActive) {
        DisableWarning();
    }
}

//endregion
