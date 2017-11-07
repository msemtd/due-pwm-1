#include "tc_lib.h"
using namespace arduino_due;
#define CAPTURE_TIME_WINDOW 2500 // usecs
// capture_tc0 declaration
// IMPORTANT: Take into account that for TC0 (TC0 and channel 0) the TIOA0 is
// PB25, which is pin 2 for Arduino DUE, so  the capture pin in  this example
// is pin 2. For the correspondence between all TIOA inputs for the different 
// TC modules, you should consult uC Atmel ATSAM3X8E datasheet in section "36. 
// Timer Counter (TC)"), and the Arduino pin mapping for the DUE.
capture_tc0_declaration();
auto& capture_pin2=capture_tc0;

uint32_t ticks_per_usec = 0;
String outbuf;
uint32_t now = 0;
uint32_t status =0;
uint32_t duty =0;
uint32_t period =0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  outbuf.reserve(200);
  // capture_pin2 initialization
  capture_pin2.config(CAPTURE_TIME_WINDOW);
  Serial.println("========================================================");
  ticks_per_usec = capture_pin2.ticks_per_usec();
  Serial.print("ticks per usec: "); Serial.println(capture_pin2.ticks_per_usec());
  Serial.print("max capture window: "); 
  Serial.print(capture_pin2.max_capture_window());
  Serial.println(" usecs.");
  Serial.println("========================================================");

}

void loop() {
  // put your main code here,to run repeatedly:
  display_encoder_rotation();
}

void display_encoder_rotation() {
  // MSEMTD: typical rate-limiting code...
  static uint32_t t_last = 0;
  now = millis();
  if(now - t_last < 500)
    return;
  t_last = now;
  // MSEMTD: actual task follows...

  status = capture_pin2.get_duty_and_period(duty, period);

  //Serial.println("********************************************************");  
  /* Serial.print("\r[PIN 2] duty="); 
  Serial.print(
    static_cast<double>(duty)/
    static_cast<double>(ticks_per_usec),
    3
  );
  Serial.print(" period=");
  Serial.print(
    static_cast<double>(period)/
    static_cast<double>(ticks_per_usec),
    3
  );
  Serial.print(" ");
  Serial.print(period);
  if(capture_pin2.is_overrun(status)) Serial.print("[overrun]");
  if(capture_pin2.is_stopped(status)) Serial.print("[stopped]");
  Serial.print("         ");
*/
  // MSEMTD: on the AS5048A PWM output, the period should be 1000 uSec (+/-10%) over the full operating temperature range.
  // The period represents 4119 bit pulses that start with 12 high init bits, 4 normally high error_n bits, 4095 data bits, and 8 low exit bits
  
  // We can linerarise the data based on the measured period to get the duty bits and then calculate a rotation in degrees
  // int val = map(duty, 0, period, 0, 4119);
  // val -= 16;

  float fval = floatmap((float)duty, 0.0f, (float)period, 0.0f, 4119.0f);
  fval -= 16.0f;
  // convert to degrees...
  float fdeg  = fval * 365.0f / 4095.0f;
  
  // also write to a template buffer for output!
  outbuf = "\r  value=";
  outbuf += String(fval, 2);
  outbuf += "   deg=";
  outbuf += String(fdeg, 2);
  outbuf.concat("      ");
  Serial.print(outbuf);
}
// Same as map from WMath.h but using floats...
float floatmap(long x, float in_min, float in_max, float out_min, float out_max){ return ((float)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}

