#include <Servo.h>
#include <Math.h>

/* PART  			-- Port #
 *
 * Mouth Servo		-- Digital 6
 * Potentiometer	-- Analog  0
 * Eyes				-- Digital 8
 * Left Arm			-- Digital 11
 * Left Hand		-- Digital 12
 * Right Arm		-- Digital 3
 * Right Hand		-- Digital 2
 * PIN 0, 1 & 13 ARE BROKEN !!!!!!!
 */

//Head Servos
Servo eyes;
Servo mouthServo;

//Arm Servos
Servo leftArm; 
Servo leftHand;
Servo rightArm; 
Servo rightHand;
 
short pos = 0;			//variable to store the servo position 
const short potpin = 0;	//analog pin used to connect the potentiometer
long counter = 0;

int stateOne = 0;
int stateTwo = 0;
int secondCounter = 0;

//arm mins & maxes
short leftArmMin = 20;
short leftArmMax = 70;
short rightArmMin = 60;
short rightArmMax = 110;

int laInnerCount = 0;
int laStateCount = 0;
int raInnerCount = 0;
int raStateCount = 0;
int rHInnerCount = 0;
int rHStateCount = 0;
int lHInnerCount = 0;
int lHStateCount = 0;

const boolean debug = false;

void setup() 
{
	//Head Stuff
	eyes.attach(8);  
	mouthServo.attach(6);

	//Arm Stuff
	leftArm.attach(11);		//Left Arm Elbow
	leftHand.attach(12);	//Left Hand Fingers (180 is open, 90 is closed gripped)
	rightArm.attach(3);		//Right Arm Elbow
	rightHand.attach(2);	//Right Hand FIngers (180 is open, 90 is closed gripped)
	

	if (debug)
		Serial.begin(9600);
} 

void jawCode(int minJawOp, int maxJawOp)
{
	//if (minJawOp >= maxJawOp)	return;	//A silent-fail solution

	int val = analogRead(potpin);		// reads the value of the potentiometer (value between 0 and 1023)
	val = floor((double) val / 10) * 10;

	val = map(val, 0, 1023, 0, 384);
	val = val - 87;

	if (val <= maxJawOp && val >= minJawOp)
		mouthServo.write(val);			// sets the servo position according to the scaled value
}


void writeToServo(Servo &servoName, int count, boolean invert)
{
	if (debug)
	{
		Serial.print("writeToServo Location: ");
		Serial.println(count);
	}
	
	if (invert)
		servoName.write(invertServoPosition(count));
	else
		servoName.write(count);
} 

void movement(Servo &servoItem, boolean invert, int &state, int moveSpeed, int startPos, int endPos, int &innerCounter, int timeDelay, int timeDelay2, int moveSpeedTwo)
{
	if (debug)
	{
		Serial.print("movement State = ");
		Serial.println(state);
	}
	
	switch (state)
	{
		case 0:
			//start state moving from beginner position
			if (counter%moveSpeed == 0)
			{
				writeToServo(servoItem, (startPos+innerCounter), invert);
				innerCounter = innerCounter + 1;
			}

			if (startPos + innerCounter >= endPos)
			{
				if (debug)
					Serial.println("startPos + innerCounter = endPos");

				state = 1;
				innerCounter = 0;
			}
			break;
		case 1:
			//the first delay
			if (innerCounter < timeDelay)
			{
				innerCounter++;
			}
			else if (innerCounter >= timeDelay)
			{
				state = 2;
				innerCounter = 0;
			}
			break;
		case 2:
			//move back
			if (counter % moveSpeedTwo == 0)
			{		
				writeToServo(servoItem, (endPos - innerCounter), invert);
				innerCounter++;
			}
			if (endPos - innerCounter <= startPos)
			{
				state = 3;
				innerCounter = 0;
			}
			break;
		case 3:
			//the second delay
			if (innerCounter < timeDelay)
			{
				innerCounter++;
			}
			else if (innerCounter >= timeDelay)
			{
				state = 0;
				innerCounter = 0;
			}
			break;
		//default: This should NEVER happen
	}
}

void loop()
{ 
	hereBeDragons();
}

int invertServoPosition(int pos)
{
	return abs(180 - pos);
}

void optometry()
{
	if (debug)
	{
		Serial.print("stateOne: ");
		Serial.println(stateOne);
		Serial.print("secondCounter: ");
		Serial.println(secondCounter);
	}
	movement(eyes, false, stateOne, 10, 0, 43, secondCounter, 1000, 100, 10);
	delay(400);
}

void testArm(Servo &arm, boolean invert)
{
	//writeToServo(arm, 30, invert);
	leftArm.write(30);
	delay(2000);
	//writeToServo(arm, 60, invert);
	leftArm.write(60);
	delay(2000);
	//writeToServo(arm, 90, invert);
	leftArm.write(90);
	delay(2000);
	//70 - 120
	//void movement(Servo &servoItem, boolean invert, int &state, int moveSpeed, int startPos, int endPos, int &innerCounter, int timeDelay, int timeDelay2, int moveSpeedTwo)
}

void hereBeDragons()
{
	//mouth close, mouth open
	if (debug)	Serial.println("Jaw");
	jawCode(invertServoPosition(155) + 5, invertServoPosition(100));
	
	//Eyes
	movement(eyes, false, stateOne, 10, 0, 43, secondCounter, 1000, 100, 10);
	
	//Arms
	//Optimal operating range is 20 - 70
	if (debug)	Serial.println("Left Arm");
	movement(leftArm, false, stateTwo, 8, leftArmMin, leftArmMax, laInnerCount, 3000, 4000, 8);
        
	//Optimal operating range is 60 - 110
	if (debug)	Serial.println("Right Arm");
	movement(rightArm, false, raStateCount, 8, rightArmMin, rightArmMax, raInnerCount, 3000, 4000, 8);

	//Randomized arm movements
	if(stateTwo == 0 && laInnerCount == 0)
	{
		leftArmMin = random(10, 30);
		leftArmMax = random(60, 75);
	}
	
	if(raStateCount == 0 && raInnerCount == 0)
	{
		rightArmMin = random(50, 70);
		rightArmMax = random(95, 11);
	}
	
	//Hands
	movement(leftHand, false, lHStateCount, 2, 45, 135, lHInnerCount, 3000, 3000, 10);
	movement(rightHand, true, rHStateCount, 2, 45, 135, rHInnerCount, 3000, 3000, 10);

	delay(1);
	counter++;
}
