Hi Guys, I am trying to get Stall homing of Bigtreetechs TMC5160s to work with my ESP32s3. 
Does anyone have any idea on what's going wrong or where the problem might be? The Homing sequence doesn't work but the Motor moves as expected and if I set the SG_Normal value to 5 it's always 0 and if I set it to 6 it's alwazs 1024, 
even if I put a load on the motor.
I've been trying for days now and nothing worked so far. Any ideas are appreciated 😊 I am Using NEMA14s with 24VDC at the VM and GND pins of the stepper. SPI connection was successeful, as Microstepping was set.
