const int readMoisturePin = A0;
const int relaySwitchPin = 13;
const int flowPin = 2;

const int maxWetValue = 195;
const int minWetValue = 475; 

const int shouldWaterForTimeInMS = 3000;

unsigned long lastRunTimeInMS = 0;
unsigned long currentRunTimeInMS = 0;

bool isWatering = false;

int wateringStartTime = 0;
int currentWateringTime = 0;

volatile unsigned long pulseCounter;

float flowPulseCalibrationFactor = 100000.0; // Pulses per liter for YF-S401
volatile float totalLitersDrained = 0.0;

void countPulse() { pulseCounter++; }

void setup()
{
    Serial.begin(9600);

    pinMode(readMoisturePin, INPUT);
    pinMode(relaySwitchPin, OUTPUT);
    pinMode(flowPin, INPUT_PULLUP);

    pulseCounter = 0;

    attachInterrupt(digitalPinToInterrupt(flowPin), countPulse, FALLING);
}

void loop() 
{
    currentRunTimeInMS = millis();

    int sensorValue = analogRead(readMoisturePin);
    int humidityPercentage = map(sensorValue, maxWetValue, minWetValue, 100, 0);
    humidityPercentage = constrain(humidityPercentage, 0, 100);

    Serial.print("Soil Moisture: ");
    Serial.print(humidityPercentage);
    Serial.print("%\n");

    if (humidityPercentage < 25 and !isWatering)
    {
        wateringStartTime = millis();
        isWatering = true;

        digitalWrite(relaySwitchPin, HIGH);
        Serial.print("STARTED WATERING\n");
    }

    if (isWatering)
    {
        currentWateringTime = millis();
        Serial.print("CURRENTLY WATERING\n");

        if (currentRunTimeInMS - lastRunTimeInMS >= 250)
        {
            detachInterrupt(digitalPinToInterrupt(flowPin));

            float litersDrainedThisWatering = pulseCounter / flowPulseCalibrationFactor;
            float litersDrainedPerMinute = litersDrainedThisWatering * 60.0;

            totalLitersDrained += litersDrainedThisWatering;

            Serial.print("Flow rate: ");
            Serial.print(litersDrainedPerMinute, 3);
            Serial.print(" L/min\n");

            pulseCounter = 0;
            lastRunTimeInMS = millis();

            attachInterrupt(digitalPinToInterrupt(flowPin), countPulse, FALLING);
        }

        if ((currentWateringTime - wateringStartTime) >= shouldWaterForTimeInMS)
        {
            digitalWrite(relaySwitchPin, LOW);
            currentWateringTime = 0;
            isWatering = false;
            Serial.print("STOPPED WATERING\n");

            Serial.print("Total: ");
            Serial.print(totalLitersDrained, 3);
            Serial.println(" L");

            totalLitersDrained = 0;
            delay(5000);
        }
    }
    delay(500);
}