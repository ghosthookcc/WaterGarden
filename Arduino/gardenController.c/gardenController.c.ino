const int readMoisturePin = A0;
const int relaySwitchPin = 13;

const int maxWetValue = 195;
const int minWetValue = 475; 

const int shouldWaterForTimeInMS = 3000;

bool isWatering = false;

int wateringStartTime = 0;
int currentWateringTime = 0;

void setup()
{
    Serial.begin(9600);

    pinMode(readMoisturePin, INPUT);
    pinMode(relaySwitchPin, OUTPUT);
}

void loop() 
{
    int sensorValue = analogRead(readMoisturePin);
    int humidityPercentage = map(sensorValue, maxWetValue, minWetValue, 100, 0);
    humidityPercentage = constrain(humidityPercentage, 0, 100);

    /*
    Serial.print("Soil Moisture: ");
    Serial.print(humidityPercentage);
    Serial.print("%\n");

    Serial.print("Currently watering -- ");
    Serial.print(isWatering);
    Serial.print("\n");

    Serial.print("CurrentTime -- ");
    Serial.print(currentWateringTime);
    Serial.print("\n");
    */

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
        if ((currentWateringTime - wateringStartTime) >= shouldWaterForTimeInMS)
        {
            digitalWrite(relaySwitchPin, LOW);
            currentWateringTime = 0;
            isWatering = false;
            Serial.print("STOPPED WATERING\n");
            delay(5000);
        }
    }
    delay(500);
}