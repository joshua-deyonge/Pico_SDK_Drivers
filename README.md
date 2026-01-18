# Pico_SDK_Drivers
Repository Containing Drivers I made for the Pico SDK.

This is one of my first repositories to help maintain and organize the drivers I learn and develop along 
my journey to learn Embedded Engineeing. I choose the Pico as I find it interesting and just alittle 
different from learning with the Arduino. Ill keep updating and maintaining these drivers for as long as I find interest.
Thanks for comming by!

Current Driveers and their Capabilities:
BME280 Driver:
This driver is curretly very limited. It allows for reading temp and returning the value in Celcius or Fahrenheit.
You may also get a humidity reading, altho somewhat inefficient as it takes a temp reading first, inorder to get the 
t_fine value needed to complete the equation for humidity. 
