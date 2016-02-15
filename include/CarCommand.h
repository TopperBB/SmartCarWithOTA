/*
 * ExampleCommand.h
 *
 */

#ifndef SMINGCORE_EXAMPLE_COMMAND_H_
#define SMINGCORE_EXAMPLE_COMMAND_H_

#include "SmingCore.h"

//https://www.gitbook.com/book/smartarduino/user-mannual-for-esp-12e-motor-shield/details

//Direcrtion
#define FW 0
#define BK 1
#define STOP 2
//#define FW_NEUTRAL 3

//Turn
#define TL 4
#define TR 5
#define STRAIGHT 6

//Motor movements
#define MOTOR_FW 1
#define MOTOR_BK 0

typedef Delegate<void()> carMotorDelegate;

class CarCommand
{
public:
	CarCommand(int leftMotorPWM, int rightMotorPWM, int leftMotorDir, int rightMotorDir);
	virtual ~CarCommand();
	void initCommand();

private:

	int lastY = 0;
	int lastX = 0;
	int currentX = 0;

	// Direction
	int dir=FW;

	// Turn Status
	int t = 0;
	int tdir = STRAIGHT;
	int tcount = 0;

//	int x = 0; //X movement percent
//	int y = 0; // Y movement percent

//	bool stopped = true;

	uint8 leftMotorPWM;
	uint8 rightMotorPWM;
	int leftMotorDir;
	int rightMotorDir;
	int duration = 200;

	//--target Speed
	int spdTargetLeft=1023;

	//--current speed
	int spdCurrentLeft=0;

	//--target Speed
	int spdTargetRight = 1023;
	//--current speed
	int spdCurrentRight = 0;

	long lastActionTime;
	Timer motorTimer;

	HardwarePWM *pwmMotors;
//	DriverPWM pwmMotors;

	int i = 0;
	bool countUp = true;
	bool countDown = false;
	int minPower = 1;

	int freq = 30;

	void processCarCommands(String commandLine, CommandOutput* commandOutput);
	void handleMotorTimer();
	void drive(int leftDir, int leftPwm, int rightDir, int rightPwm);

	void testPWM();
	void handleRegularXy();
	void handleJoystickXY(int x, int y);
	int roundMovement(int power);
	void handleCheckFreq(int pin, int freq, int pwr);
};


#endif /* SMINGCORE_DEBUG_H_ */
