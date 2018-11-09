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

#include "RecoveryDevice.h"
#include "Configuration.h"

void RecoveryDevice::init(int id, int pin, EjectionType type) {
	log("Init Recovery Device " + String(id) + " on pin " + String(pin));
	this->relayPin = pin;
	this->id = id;
	this->type = type;
	switch(type){
		case kPyro:  pinMode(pin, OUTPUT); break;
		case kServo: servo.attach(pin); break;
		case kNoEjection: break;
	}
	reset();
};

void RecoveryDevice::enable() 
{
	deployed = true;
	deploymentTime = millis();
	relayState = ON;
	switch(type){
	  case kPyro:  digitalWrite(relayPin, HIGH);; break;
	  case kServo: servo.write(kMinServoAngle); break;
	  case kNoEjection: break;
	}
	log("Deploying Chute #" + String(id) );
};


void RecoveryDevice::disable()
 {
	deployed = false;
	relayState = OFF;
	switch(type){
	  case kPyro:  digitalWrite(relayPin, LOW);; break;
	  case kServo: servo.write(kMaxServoAngle); break;
	  case kNoEjection: break;
	}
	log("Disabling Chute #" + String(id) );
};

void RecoveryDevice::reset() {
	deployed = false;
	deploymentTime = 0;
	timedReset = false;
	disable();
};