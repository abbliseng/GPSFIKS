import serial
import serial.tools.list_ports
import time

def listen_for_message():
    time.sleep(0.1)
    line = ser.read_all()
    print(line)
    if line:
        print(line.decode())
    return

def led_on_off():
    user_input = input("\n Type on / off / quit : ")
    if user_input =="on":
        print("LED is on...")
        time.sleep(0.1) 
        ser.write(b'H')
        listen_for_message()
        led_on_off()
    elif user_input =="off":
        print("LED is off...")
        time.sleep(0.1)
        ser.write(b'L')
        listen_for_message()
        led_on_off()
    elif user_input =="quit" or user_input == "q":
        print("Program Exiting")
        time.sleep(0.1)
        ser.write(b'L')
        ser.close()
    elif user_input == "save flash to csv":
        pass
    elif user_input == "clear flash":
        pass
    else:
        print("Invalid input. Type on / off / quit.")
        led_on_off()

# Define the serial port and baud rate.
# Ensure the 'COM#' corresponds to what was seen in the Windows Device Manager
arduino_port = False
ports = list(serial.tools.list_ports.comports())
for p in ports:
    if ("Arduino MKR1000" in p.description):
        arduino_port = p.name
        break
if (not arduino_port):
    print("No arduino found. Make sure it is connected to the PC and try again.")
    exit()
ser = serial.Serial(arduino_port, 9600)

time.sleep(2) # wait for the serial connection to initialize
led_on_off()