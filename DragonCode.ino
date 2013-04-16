#include <Servo.h>
#include <Math.h> 

//PART  			-- Port #
//
//Mouth Servo		-- Digital 6
//Potentiometer		-- Analog  0
//Eyes				-- Digital 7
//Left Arm			-- Digital 12
//Left Hand			-- Digital 13
//Right Arm			-- Digital 1
//Right Hand		-- Digital 0
//Power Down Button	--  S2 pin 6

//Head Servos
Servo eyes;
Servo mouthServo;

//Arm Servos
Servo leftArm, leftHand;
Servo rightArm, rightHand;
 
int pos = 0;		// variable to store the servo position 
int potpin = 0;		// analog pin used to connect the potentiometer
long counter;

int stateOne = 0;
int stateTwo = 0;
int secondCounter = 0;
int thirdCounter = 0;
int armMoveUp = 160;		//douche code
int armMoveDown = 120;

boolean powerOn;
int key_s6; 

void setup() 
{
	//Head Stuff
	eyes.attach(7);  
	mouthServo.attach(6);

	//Arm Stuff
	leftArm.attach(12);		//Left Arm Elbow
	leftHand.attach(13);	//Left Hand Fingers (180 is open, 90 is closed gripped)
	rightArm.attach(1);		//Right Arm Elbow
	rightHand.attach(0);	//Right Hand FIngers (180 is open, 90 is closed gripped)

	counter = 0;
	key_s6 = 2;
	Serial.begin(9600);
	powerOn = true;
	pinMode(key_s6, INPUT);
} 

void jawCode(int minJawOp, int maxJawOp)
{
	//TODO: Throw error minJawOp < maxJawOp <--- there are no exceptions...
	//if (minJawOp >= maxJawOp)	return;	//A silent-fail solution

	int val = analogRead(potpin);		// reads the value of the potentiometer (value between 0 and 1023)
	val = floor((double) val / 10) * 10;

	val = map(val, 0, 1023, 0, 384);
	val = val - 87;

	if (val <= maxJawOp && val >= minJawOp)
		mouthServo.write(val);			// sets the servo position according to the scaled value
	//Serial.print();
}

void writeToServo(Servo &servoName, int count, boolean invert)
{
	Serial.print("Location: ");
	Serial.println(count);
	if (invert)
		servoName.write(count);
	else
		servoName.write(invertServoPosition(count));
} 

void movement(Servo &servoItem, boolean invert, int &state, int moveSpeed, int startPos, int endPos, int &innerCounter, int timeDelay, int timeDelay2, int moveSpeedTwo)
{
	//Serial.print("State = ");
	//Serial.println(state);
	switch (state)
	{
		case 0:
			//start state moving from beginner position
			if(counter%moveSpeed == 0)
			{
				writeToServo(servoItem, (startPos+innerCounter), invert);
				innerCounter = innerCounter + 1;
			}
			if((startPos + innerCounter) >= endPos)
			{
				//Serial.println("startPos + innerCounter = endPos");
				state = 1;
				innerCounter = 0;
			}
			break;
		case 1:
			//the first delay
			if( innerCounter < timeDelay)
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
			if((counter%moveSpeedTwo) == 0)
			{		
				writeToServo(servoItem, (endPos - innerCounter), invert);
				innerCounter++;
			}
			if((endPos - innerCounter) <= startPos)
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

int raInnerCount = 0;
int raStateCount = 0;
int rHInnerCount = 0;
int rHStateCount = 0;
int lHInnerCount = 0;
int lHStateCount = 0;

int stopCounter = 100;

void loop()
{
	boolean stopped = false;
  
	if(digitalRead(key_s6) == 0)
	powerOn = !powerOn;

	if (powerOn)
	{
		//mouth close, mouth open                  
		jawCode(invertServoPosition(155) + 5, invertServoPosition(100));
		//Eyes
		movement(eyes, false, stateOne, 10, 0, 43, secondCounter, 1000, 100, 10);
		//Arms
		//TODO: Check the arm drop
		movement(leftArm, false, stateTwo, 2, armMoveDown, armMoveUp, thirdCounter, 3000, 3000, 10);
		//	movement(leftArm, false, stateTwo, 2, 100, 120, thirdCounter, 3000, 3000, 10);
		//movement(rightArm, true, raStateCount, 2, 100, 140, raInnerCount, 3000, 3000, 10);

		if(stateTwo == 0 && thirdCounter == 0)
		{
			armMoveUp = random(140, 160);
			armMoveDown = random(135, 145);
		}
	
		//Hands
		//TODO: Finish the hands.
		movement(leftHand, true, lHStateCount, 2, 90, 180, lHInnerCount, 3000, 3000, 10);
		movement(rightHand, true, rHStateCount, 2, 90, 180, rHInnerCount, 3000, 3000, 10);
	
		delay(1);
		counter++;
	}
	else
	{
		//Lower limbs to safe position & give 30 secs to unplug power.
		if (!stopped)
		{
			powerDown(stopCounter);
			//stopped = true;
		}
	}
}

int invertServoPosition(int pos)
{
	return abs(180 - pos);
}

void powerDown(int &pos)
{
	//TODO: do something about magic numbers
	leftHand.write(90);
	rightHand.write(90);
  
	if(pos < 150)
	{
		leftArm.write(pos);
		rightArm.write(invertServoPosition(pos));
		pos++;
		delay(20);
	}
}
