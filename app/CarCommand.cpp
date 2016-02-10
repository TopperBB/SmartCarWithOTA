/*
 * CarCommand.cpp
 *
 */

#include <CarCommand.h>

CarCommand::CarCommand(int leftMotorPWM, int rightMotorPWM, int leftMotorDir, int rightMotorDir)
{
	this->leftMotorPWM = leftMotorPWM;
	this->rightMotorPWM = rightMotorPWM;
	this->leftMotorDir = leftMotorDir;
	this->rightMotorDir = rightMotorDir;

	uint8_t pins[2] = { (uint8_t)leftMotorPWM, (uint8_t)rightMotorPWM }; // List of pins that you want to connect to pwm

	pwmMotors = new HardwarePWM(pins, 2);
//	pwmMotors->
	debugf("CarCommand Instantiating");
}

CarCommand::~CarCommand()
{
}

void CarCommand::initCommand()
{
	commandHandler.registerCommand(CommandDelegate("Move","Example Command from Class","Application",commandFunctionDelegate(&CarCommand::processCarCommands,this)));

	pinMode(leftMotorDir, OUTPUT);
	digitalWrite(leftMotorDir, HIGH);
	pinMode(rightMotorDir, OUTPUT);
	digitalWrite(rightMotorDir, HIGH);

	//Check and act upon car commands
	motorTimer.setCallback(carMotorDelegate(&CarCommand::handleMotorTimer, this));
	motorTimer.setIntervalMs(100);

//	motorTimer.setCallback(carMotorDelegate(&CarCommand::testPWM, this));
//	motorTimer.setIntervalMs(1000);
//	motorTimer.start(true);
}


void CarCommand::testPWM()
{
//	if(countUp){
//		i++;
//		if(i == 100){
//			countUp = false;
//			countDown = true;
//		}
//	}
//	else{
//		i--;
//		if(i == 0){
//			countUp = true;
//			countDown = false;
//		}
//	}

	if (countUp) {

		i = 100;
		countUp = false;
		countDown = true;
		pwmMotors->analogWrite(leftMotorPWM, 1023);
	}else {
		i = 1;
		countUp = true;
		countDown = false;
		pwmMotors->analogWrite(leftMotorPWM,0);
//		pwmMotors.analogWrite(leftMotorPWM, 1);
	}

	int pp = map(i, 0, 100, 0, 1023);
	Serial.println(pp);
//	pwmMotors.analogWrite(leftMotorPWM, pp);
//	digitalWrite(leftMotorDir, LOW);
//	pwmMotors.analogWrite(rightMotorPWM, pp);
//	digitalWrite(rightMotorDir, HIGH);
}

void CarCommand::processCarCommands(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);
	int rightPwm =0;
	int leftPwm = 0;
	int rightDir = 0;
	int leftDir = 0;

	debugf("Got commandLine = %s", commandLine.c_str());
	if (numToken == 1)
	{
		commandOutput->printf("Move Commands available : \r\n");
		commandOutput->printf("stop : Show example status\r\n");
		commandOutput->printf("xyz xValue yValue: Send X,Y and Z PWR (X,Y can be negative for reverse)\n");
	}
	else
	{
		//first thing stop the "stop timer"
		if (motorTimer.isStarted()) {
			motorTimer.stop();
		}

		if (commandToken[1].startsWith("xyz")) {
			int x = commandToken[2].toInt();
			int y = commandToken[3].toInt();
			int z = 100;
			if (numToken == 5) {
				int z = commandToken[4].toInt();
			}

			debugf("ilan1:y=%i, abs(y) =%i, leftP=%i,rightP=%i",y, abs(y), leftPwm, rightPwm);
			//check direction to move(needed for knowing which side to move - wheels)
			if (y > 0) {
				dir = FW;
			} else if (y == 0) {
				dir = STOP;
			}
			else {
				dir = BK;
			}

			// if currently we just do right or left, keep the last heading (lastY)
			if (x != 0 && y == 0) {
				if (lastY != 0) {
					if (lastY > 0) {
						dir = FW;
						y = lastY;
					}
					else {
						dir = BK;
						y = lastY;
					}
				}
			}

			lastY = y;
			lastX = x;

			if (dir != STOP) {
				if (x>0) {
					debugf("!@Stop=1");
					tdir = TR;
				} else if (x<0) {
					debugf("!@Stop=2");
					tdir = TL;
				} else {
					debugf("!@Stop=3");
					tdir = STRAIGHT;
				}

				//set motors to run (power)
				leftPwm = map(abs(y), 0, 100,  0, 1023);
				rightPwm = map(abs(y), 0, 100,  0, 1023);
				debugf("ilan13: leftP=%i,rightP=%i",leftPwm, rightPwm);
				//check if we want to move right
				if (dir == FW) {
					if (tdir == TL) {
						debugf("FW1");
						rightDir = HIGH;
						leftPwm = 0;
					} else if (tdir == TR) {
						debugf("FW2");
						leftDir = HIGH;
						rightPwm = 0;
					}else if (tdir == STRAIGHT) {
						debugf("FW3");
						leftDir = HIGH;
						rightDir = HIGH;
					}
				}
				else if (dir == BK)
				{
					if (tdir == TL) {
						debugf("bk1");
						rightDir = LOW;
						leftPwm = 0;
					} else if (tdir == TR) {
						debugf("bk2");
						leftDir = LOW;
						rightPwm = 0;
					}else if (tdir == STRAIGHT) {
						debugf("bk3");
						leftDir = LOW;
						rightDir = LOW;
					}
				}
			}
			else {
				leftPwm = 0;
				rightPwm = 0;
			}

			debugf("inside command:leftD=%i,leftP=%i,rightD=%i,rightP=%i", leftDir, leftPwm, rightDir, rightPwm);
		}

		drive(leftDir, leftPwm, rightDir, rightPwm);
		motorTimer.startOnce();
	}
}

void CarCommand::drive(int leftDir, int leftPwm, int rightDir, int rightPwm) {
	debugf("drive command:leftD=%i,leftP=%i,rightD-%i,rightP=%i", leftDir, leftPwm, rightDir, rightPwm);
	spdTargetLeft = leftPwm;
	spdTargetRight = rightPwm;

	digitalWrite(leftMotorDir, leftDir);
	digitalWrite(rightMotorDir, rightDir);
	if (leftPwm == 0 ){
		pwmMotors->analogWrite(leftMotorPWM, 0);
	} else {
		pwmMotors->analogWrite(leftMotorPWM, leftPwm);
	}

	if (rightPwm == 0 ){
		pwmMotors->analogWrite(rightMotorPWM, 0);
	} else {
		pwmMotors->analogWrite(rightMotorPWM, rightPwm);
	}
}

//Stop the car
void CarCommand::handleMotorTimer() {
	drive(0,0,0,0);
};
