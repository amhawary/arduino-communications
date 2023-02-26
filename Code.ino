/*
Name: AMIN ALHAWARY
CSA Arduino Communications Coursework
Last Edited: 05.05.22 8:32
*/
int TX_led = 13;

#include <LiquidCrystal.h> //Please replace the single quote characters ('') with the parenthesis character (<>)

LiquidCrystal lcd(12, 5, 11, 10, 9, 8); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
int buzzer = 2;
int button = A3;
int led = 3;
int tilt = A2;
const int trigPin = A1;
const int echoPin = A0;
long duration;
float distance;

int ledState = LOW;             // ledState used to set the LED

#define POTENTIOMETER_PIN A3
int a = 7;
int b = 6;
int c = 4;
int d = 11;
int e = 10;
int f = 8;
int g = 9;
void display1(void)
{
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
}
// display number2
void display2(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(d, HIGH);
}
// display number3
void display3(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);

  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(g, HIGH);
}
// display number4
void display4(void)
{
  digitalWrite(f, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(c, HIGH);
}
// display number5
void display5(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
}
// display number6
void display6(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
}
// display number7
void display7(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
}
// display number8
void display8(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
}
void display9(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(f, HIGH);
}
void display0(void)
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
}
void displayError(void)
{
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
}
void clearDisplay(void)
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(g, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
}
// the setup routine runs once when you press reset:
void setup()
{
//  Serial.begin(9600);
  lcd.begin(16, 2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(c, OUTPUT);
  for (int i = 6; i <= 11; i++)
    pinMode(i, OUTPUT);
  // set the digital pin as output:
  pinMode(TX_led, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);
}


const long txInterval = 200;              // interval at which to tx bit (milliseconds)
int tx_state = 0;
int tx_char;
int chr;
unsigned long previousTxMillis = 0;        // will store last time LED was updated

int tx_string[7] = {0, 0, 0, 0, 0, 0, 0};
#define TX_START_OF_TEXT  -1
int tx_string_state = TX_START_OF_TEXT;

#define STX 0x02
#define ETX 0x03

int getTxChar()
{
  int chr;

  switch (tx_string_state)
  {
    case TX_START_OF_TEXT:
      tx_string[0] = analogRead(button);
      if (tx_string[0] < 50) {
        tx_string[0] = 1;
      }
      else {
        tx_string[0] = 0;
      }
      tx_string[1] = analogRead(tilt);
      if (tx_string[1] < 50) {
        tx_string[1] = 1;
      }
      else {
        tx_string[1] = 0;
      }
      tx_string[2] = analogRead(POTENTIOMETER_PIN) / 100;

      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance = duration * 0.034 / 2;
      tx_string[3] = (int)(distance / 100) % 100;
      tx_string[4] = (int)distance % 100;
      tx_string_state = 0;
//      Serial.println(tx_string[0]);
//      Serial.println(tx_string[1]);
//      Serial.println(tx_string[2]);
//      Serial.println(tx_string[3]);
//      Serial.println(tx_string[4]);
      return STX;
      break;

    default:
      chr = tx_string[tx_string_state];
      tx_string_state++;
      if (tx_string_state == 8)  /* End of string? */
      {
        tx_string_state = TX_START_OF_TEXT;  /* Update the tx string state to start sending the string again */
        return ETX;  /* Send End of Text character */
      }
      else
      {
        return chr;  /* Send a character in the string */
      }
      break;
  }
}

void txChar()
{
  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousTxMillis >= txInterval)
  {
    // save the last time you blinked the LED (improved)
    previousTxMillis = previousTxMillis + txInterval;  // this version catches up with itself if a delay was introduced

    switch (tx_state)
    {
      case 0:
        chr = getTxChar();
        digitalWrite(TX_led, HIGH);  /* Transmit Start bit */
        tx_state++;
        break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if ((chr & 0x40) != 0)   /* Transmit each bit in turn */
        {
          digitalWrite(TX_led, HIGH);
        }
        else
        {
          digitalWrite(TX_led, LOW);
        }
        chr = chr << 1;  /* Shift left to present the next bit */
        tx_state++;
        break;

      case 8:
        digitalWrite(TX_led, HIGH);  /* Transmit Stop bit */
        tx_state++;
        break;

      default:
        digitalWrite(TX_led, LOW);
        tx_state++;
        if (tx_state > 20) tx_state = 0;  /* Start resending the character */
        break;
    }
  }
}



const long rxInterval = 20;              // interval at which to read bit (milliseconds)
int rx_state = 0;
int rx_char;
unsigned long previousRxMillis = 0;        // will store last time LED was updated
int rx_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int rx_string[7] = {0, 0, 0, 0, 0, 0, 0};
int rx_string_state;
int rx_error = 0;
void rxChar()
{
  unsigned long currentRxMillis = millis();
  int sensorValue;
  int i;

  if (currentRxMillis - previousRxMillis >= rxInterval)
  {
    // save the last time you read the analogue input (improved)
    previousRxMillis = previousRxMillis + rxInterval;  // this version catches up with itself if a delay was introduced

    sensorValue = analogRead(A5);
    //Serial.println(rx_state);
    //    Serial.println(rx_string_state);
    switch (rx_state)
    {
      case 0:
        if (sensorValue >= 900)
        {
          rx_bits[0]++;
          rx_state++;
        }
        break;

      case 100:
        if ((rx_bits[0] >= 6) && (rx_bits[8] >= 6))  /* Valid start and stop bits */
        {
          rx_char = 0;

          for (i = 1; i < 8; i++)
          {
            rx_char = rx_char << 1;
            if (rx_bits[i] >= 6) rx_char = rx_char | 0x01;
          }
          if (rx_char == STX) {
            rx_error = 0;
            //            Serial.print("rx before: "); Serial.println(rx_string);
            for (i = 0; i < 3; i++)
            {
              //              Serial.print(i); Serial.print(": "); Serial.println(rx_string[i]);
              rx_string[i] = 0;
            }
            rx_string_state = 0;
            //            Serial.print("rx after: "); Serial.println(rx_string);
          }
          else if (rx_char == ETX) {
            if (!rx_error) {
              //              lcd.clear(); lcd.print("H:"); lcd.print(rx_string[5]); lcd.print("% T:"); lcd.print(rx_string[4]); lcd.print("C");
              //              lcd.setCursor(0, 1);
              //              lcd.print("Dist: "); lcd.print(rx_string[3]); lcd.print("cm");
              lcd.clear(); lcd.setCursor(0, 0);
              lcd.print("b:"); lcd.print(rx_string[0]); lcd.print(" t:"); lcd.print(rx_string[1]); lcd.print(" p:"); lcd.print(rx_string[2]);
              lcd.setCursor(0, 1);
              lcd.print("A:"); lcd.print(rx_string[3]); lcd.print("B:"); lcd.print(rx_string[4]); lcd.print("C:"); lcd.print(rx_string[5]); lcd.print("D:"); lcd.print(rx_string[6]);
              if (rx_string[0] == 1) {
                digitalWrite(buzzer, HIGH);
              }
              else {
                digitalWrite(buzzer, LOW);
              }

              if (rx_string[1] == 1) {
                digitalWrite(led, HIGH);
              }
              else {
                digitalWrite(led, LOW);
              }
              clearDisplay();
              switch (rx_string[2])
              {
                case 0:
                  display0();
                  break;

                case 1:
                  display1();
                  break;

                case 2:
                  display2();
                  break;

                case 3:
                  display3();
                  break;

                case 4:
                  display4();
                  break;

                case 5:
                  display5();
                  break;
                case 6:
                  display6();
                  break;

                case 7:
                  display7();
                  break;

                case 8:
                  display8();
                  break;

                case 9:
                case 10:
                  display9();
                  break;

                default:
                  displayError();
                  break;
              }
//              Serial.println(rx_string[0]);
//              Serial.println(rx_string[1]);
//              Serial.println(rx_string[2]);
//              Serial.println(rx_string[3]);
//              Serial.println(rx_string[4]);
            }
          }
          else {
            rx_string[rx_string_state++] = rx_char;
          }
        }
        else
        {
          rx_error = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Rx error");
          clearDisplay();
          displayError();
        }
        for (i = 0; i < 10; i++)
        {
          rx_bits[i] = 0;
        }
        rx_state = 0;
        break;

      default:
        if (sensorValue >= 900)
        {
          rx_bits[rx_state / 10]++;
        }
        rx_state++;
        break;
    }
  }

}



// the loop routine runs over and over again forever:
void loop()
{
  txChar();
  rxChar();
}
