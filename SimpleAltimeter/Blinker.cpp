/*********************************************************************************
 * Simple Alitimeter 
 * 
 * Mid power rocket avionics software for alitidue recording and dual deployment
 * 
 * Copyright 2018, Jonathan Nobels
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in the 
 * Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the 
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************/

#include "Blinker.hpp"

void Blinker::blinkValue(double value, double speed, int frequency)
{
  if(isBlinking()) {
    cancelSequence();
  }

  int tempValue = value;
  bool foundDigit = false;         //Don't blink leading 0s
  int n=0;
  //If we make it past 999km then this failing is probably not a big deal
  for(int m=1000000; m>0; m=m/10) {  
    int digit = tempValue / m;
    if (digit || foundDigit){
      foundDigit = true;
      tempValue = tempValue - digit*m;
      if(digit == 0)digit = 10;
      for(int i=0;i<digit;i++) {
        sequence[n].onTime = speed;
        sequence[n].offTime = speed;
        if(n<kMaxBlinks)n++;
      }
    }
    if(foundDigit) {
      sequence[n-1].offTime = speed*2;
    }
  }
  sequence[n].onTime = speed*2;
  sequence[n].offTime = speed*2;
  //sequence[n].frequency = frequency;
  blinker->blinkSequence(sequence, n+1, true);
}


void Blinker::blinkSequence(Blink *msequence, size_t len, bool repeat)
{
  if(len > kMaxBlinks)
    log("Blink Sequece too long");
    return;
  }
  cancelSequence();
  if(msequence != this->sequence) {
    memcpy(msequence, this->sequence, len*sizeof(Blink));
  }
  state = OFF;
  position = 0;
  sequenceLen = len;
  isRunning = true;
  this->repeat = repeat;
  handleTimeout();
}


void Blinker::cancelSequence()
{
  ticker.detach();
  setHardwareState(OFF);
  if(sequence != nullptr) {
    delete sequence;
    sequence = nullptr;
    isRunning = false;
  }
}


void interrupt(Blinker *blinker)
{
  blinker->handleTimeout();
}


void Blinker::handleTimeout()
{
    int duration = 0;
    Blink b = sequence[position];
    if(state == OFF) {
      setHardwareState(ON, b.frequency);
      duration = b.onTime;
    }
    else if(state == ON) {
      setHardwareState(OFF, b.frequency);
      duration = b.offTime;
      position++;
    }

    if(position == sequenceLen) {
      if(repeat) {
        position = 0;
      }else{
        cancelSequence();
        return;
      }
    }
    ticker.once_ms(duration, interrupt, this);
}


bool Blinker::isBlinking()
{
  return isRunning;
}


void Blinker::setHardwareState(BlinkerState hwState, int frequency)
{
  //TODO - use tone(), noTone() for passive piezos.
  if(hwState==ON)
  {
    if(ledPin != NO_PIN) {
      digitalWrite(ledPin, HIGH);
    }
    if(piezoPin != NO_PIN){
       digitalWrite(piezoPin, HIGH);
    }
  }
  else if(hwState == OFF) {
    if(ledPin != NO_PIN) {
      digitalWrite(ledPin, LOW);
    }
    if(piezoPin != NO_PIN){
      digitalWrite(piezoPin, LOW);
    }
  }
  this->state = hwState;
}
