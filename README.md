# soracom-mkr-iot-alarm

This example reads a Seeed Studio Ultrasonic Grove sensor and publishes a JSON message to the Soracom Unified endpoint when an object is within a specified range. 

When Soracom Funk is configured on a Group, the message can be published to an AWS Lambda function to kick off an email alert. The function can be modified to perform custom operations, such as posting to a Slack webhook.  

## Activate SIM

Before inserting it into your device, the SIM card needs to be registered to an account and activated. It also needs to be attached to a Group in order to configure Soracom services such as Soracom Funk.

1. Navigate to https://console.soracom.io/ to sign in or register a Soracom account.
2. Choose **Register SIM**.
3. Enter the **ICCID** and **PUK** (written on the back of the SIM card) and name your SIM - ideally something that signifies the device associated with it.
4. Choose **Create** from the **Group** drop-down and name it something like "Arduino".
5. Click **Register**.

## Download the Arduino IDE

Download and install the Arduino IDE for your operating system.
https://www.arduino.cc/en/Main/Software



## Install libraries

To install libraries:

1. Open the Arduino IDE
2. Go to **Sketch** > **Manage Libraries**.
3. Use the search bar to find and install the libraries listed below. 

### Required Arduino Libraries

- [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient/)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Ultrasonic](https://github.com/Seeed-Studio/Seeed_Arduino_UltrasonicRanger)

### Cellular Modem Support

Install the library that corresponds to your device. 

- Arduino MKR GSM 1400 [MKRGSM](https://github.com/arduino-libraries/MKRGSM)
- Arduino MKR NB 1500 [MKRNB](https://github.com/arduino-libraries/MKRNB)

## Install Board dependancies 

To program the Arduino MKR GSM 1400 or MKR NB 1500 you'll need to install support for [Arduino SAMD Boards](https://github.com/arduino/ArduinoCore-samd).

1. Open the Arduino IDE
2. Go to **Tools** > **Board** > **Boards Manager**.
3. Search "Arduino SAMD Boards" and choose **Install**.


## Deploy AWS Lambda Sample Alert App

1. Login to the [AWS Console](http://console.aws.amazon.com/).
2. Navigate to [soracom-sns-alert](https://console.aws.amazon.com/lambda/home#/create/app?applicationId=arn:aws:serverlessrepo:us-east-1:438100121539:applications/soracom-sns-alert) serverless app in the Serverless Application Repository.
3. Under **Application Settings** provide an email address that you have access to.
4. Check the box next to "I acknowledge that this app creates custom IAM roles."
5. Click **Deploy**.
6. Once fully deployed, make a note of the ARN for the Lambda function listed under resources.
7. Navigate to [AWS Identity and Access Management console](https://console.aws.amazon.com/iam).
8. Click on **Users** from the Menu and then click **Add User**.
9. Check **Programattic Access** and then click **Next**.
10. Click **Attach existing policies directly**.
11. Use the Search filter and type in "SoracomAlertManagedPolicy" and add select it from the list.
12. Click through to **Create User**.
13. Make note of the **Access Key ID** and **Secret Access Key**.



## Configure Soracom Funk

1. Navigate to [Groups](https://console.soracom.io/#/groups) in the Soracom Console.
2. Select the group created for the SIM card.
3. Under Soracom Funk, select AWS Lambda for service. 
4. Create a new pair of credentials using the Access Key ID and Secret Access Key.
5. Add the Lambda function ARN from the serverless app deployment.
6. Click Save.


## Program Arduino

1. Open the Arduino IDE
2. Create a new sketch. File > New
3. Copy the contents of the example sketch corresponding to your Arduino MKR device.
4. Plugin the Arduino MKR device to your computer using a USB cable. 
5. Select the corresponding port in the Arduino IDE. Tools > Port
6. Press upload, this compiles, verifies, and flashes the device with the new firmware. Sketch > Upload
7. Open the Serial Monitor by clicking the magnifying lens icon. This will allow you to see what the device logs, such as errors, connection status, and any messages it publishes.

Once connected, if an object is detected within 10 CM of the rangefinder a message is published to the Soracom Unified Endpoint. Soracom Funk will then invoke the Lambda function and then notify the email set in the SAR app. 