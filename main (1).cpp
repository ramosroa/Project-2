//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
//=====[Defines]===============================================================

#define LIGHTSENSOR_NUMBER_OF_AVG_SAMPLES 10

//=====[Declaration and initialization of public global objects]===============

DigitalIn ignitionButton(BUTTON1);
DigitalIn driverSeat(D5);
DigitalIn passengerSeat(D4);
DigitalIn driverBelt(D7);
DigitalIn passengerBelt(D6);

AnalogIn headlightMode(A2);
AnalogIn lightSensor(A1); 

DigitalOut ignitionLED(LED1);
DigitalOut engineLED(LED2);
DigitalOut headlightLED(D3);
DigitalInOut alarmBuzzer(PE_10);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration and initialization of public global variables]=============

bool driverState = OFF; //keeps track of the driver seat
bool engineState = OFF; //Keeps track of the engine
bool alarmON = OFF; // Keeps track of the alarm 
bool endPrint = OFF; // helps stop from endless prints
float lightLevel = 0.0;

bool ignitionPressed = false;  // Tracks if button was pressed

float lightReadingsArray[LIGHTSENSOR_NUMBER_OF_AVG_SAMPLES]; //Keeps track of light readings from the light sensor

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();
void ledActivation();
void headlightControl();
int getHeadlightMode();
void ledInit();
void ledOn();
float lightSensorUpdate();

//=====[Main function]=========================================================

int main()
{
    inputsInit();
    outputsInit();
    
    while (true) {
        ledActivation();
        headlightControl();
    }
}

//=====[Implementations of public functions]===================================

void inputsInit() // Initializes all the buttons used to pull down behavior and turns off the buzzer
{
    driverSeat.mode(PullDown);
    passengerSeat.mode(PullDown);
    driverBelt.mode(PullDown);
    passengerBelt.mode(PullDown);
    ignitionButton.mode(PullDown);
    alarmBuzzer.mode(OpenDrain);
    alarmBuzzer.input();
}

void outputsInit() // Makes sure all the LEDS used are off for a clean start
{
    ignitionLED = OFF;
    engineLED = OFF;
    headlightLED = OFF;
}

void ledInit() //Helps with giving a clean start when choosing AUTO mode
{ 
    headlightLED = OFF;
}

void ledOn() // Uses lightSensor Average readings to find the light level and then use it to figure out when to turn on the light
{
    lightLevel = lightSensorUpdate(); //Uses locally stores the updated light sensor data

    if (lightLevel <= 0.6)
        headlightLED = ON;
    else
        headlightLED = OFF;
}

float lightSensorUpdate()
{
    static int lightSampleIndex = 0;
    float lightReadingsSum = 0.0;

    lightReadingsArray[lightSampleIndex] = lightSensor.read(); //This is when the lightsensor poitns are aquired
    lightSampleIndex++;
    
    if (lightSampleIndex >= LIGHTSENSOR_NUMBER_OF_AVG_SAMPLES) {
        lightSampleIndex = 0;
    }

    for (int i = 0; i < LIGHTSENSOR_NUMBER_OF_AVG_SAMPLES; i++) {
        lightReadingsSum += lightReadingsArray[i];
    }
    
    return lightReadingsSum / LIGHTSENSOR_NUMBER_OF_AVG_SAMPLES; // Returns the average light sensor readings
}

void ledActivation() //Takes care of activating the LEDS and also of hanfling Terminal Prints To Compress our code
{
    static bool ignitionPressed = false;  //Tracks if button was pressed
    static bool engineWasRunning = false; //Tracks if engine was previously ON

    if (driverSeat && driverState == OFF)
    {
        driverState = ON;
        uartUsb.write("Welcome to enhanced alarm system model 218-W25\r\n", 48);
    }

    if (driverSeat && passengerSeat && driverBelt && passengerBelt && !engineState)
    {  
        ignitionLED = ON;
    }
    else
        ignitionLED = OFF;

    if (ignitionLED == OFF && engineLED == OFF && ignitionButton && alarmON == OFF) 
    {
        alarmBuzzer.output();
        alarmBuzzer = LOW;
        alarmON = ON;
    }

    if (ignitionButton.read() == 1 && !ignitionPressed) //This helps detect when the button is pressed and keep it here until the button is release
    {
        ignitionPressed = true; //Helps marking when the button is pressed 
    }

    if (ignitionButton.read() == 0 && ignitionPressed) //This makes it so engine starts and stops are all done when a full press and release is achieve
    {
        ignitionPressed = false; //Resets button press tracking
        
        if (!engineState && ignitionLED)  //Only start engine if ignition LED is ON
        {
            engineLED = ON;
            engineState = ON;
            engineWasRunning = true;  
            ignitionLED = OFF; 
            uartUsb.write("Engine started\r\n", 16);
            alarmBuzzer.mode(OpenDrain);
            alarmBuzzer.input();
        }
        else if (engineState) //Turn off engine if engine is ON
        {
            engineLED = OFF;
            engineState = OFF;
            ignitionLED = OFF; 
            uartUsb.write("Engine stopped\r\n", 16);
        }
    }

    if (alarmON == ON && ignitionButton == ON && !endPrint) // Allows for printing comments on fail scenarios depening on the issues 
    {
        alarmON == OFF;
        
        uartUsb.write("Ignition inhibited\r\n", 20);
        
    
        if (driverSeat == OFF) {
            uartUsb.write("Driver seat is not occupied\r\n", 30);
        }

        if (passengerSeat == OFF) {
            uartUsb.write("Passenger seat is not occupied\r\n", 32);
        }

        if (driverBelt == OFF) {
            uartUsb.write("Driver belt is not fastened\r\n", 30);
        }

        if (passengerBelt == OFF) {
            uartUsb.write("Passenger belt is not fastened\r\n", 32);
        }
        endPrint = true;
    }
}
    


int getHeadlightMode() //Reads the potentiometer and assigns a mode depending on the range of the potentionmeter reading
{
    float modeValue = headlightMode.read();
    if (modeValue < 0.33)
        return 0; // OFF
    else if (modeValue < 0.66)
        return 1; // ON
    else
        return 2; // AUTO
}

void headlightControl() // Controls the headlight action depending on the returned value from the getHeadlightMode()
{
    if (!engineState)
    {
        headlightLED = OFF;
        return; //If the engine is not On then the headlightControl function ends here and also the LED is turned OFF
    }
    
    int mode = getHeadlightMode();
    
    if (mode == 0) // OFF
    {
        headlightLED = OFF;
    }
    else if (mode == 1) // ON
    {
        headlightLED = ON;
    }
    else if (mode == 2) // AUTO
    {
        ledInit();
        ledOn();
    }
}
