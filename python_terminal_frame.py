from calendar import prcal
import os
import serial
import serial.tools.list_ports
import time
from colorama import Fore, Back, Style


# ██▒▒▒▒▒▒▒▒ 20%

interface_width = 50
previous_page = ""
current_page = "main"
exit_program = False
s = ""
arduino_port = False
serial_connected = False
ser = False

# UTIL FUNCTIONS
def clear():
    os.system('cls' if os.name == 'nt' else 'clear')

def bar():
    print("-"*interface_width)

def pick_option(options = [], exit_option = False, return_option = False):
    if return_option: options.append(['b', 'Return', return_to_previous])
    if exit_option: options.append(['e', 'Exit program', close_program])
    for option in options:
        option_string = option[0] + " - " + option[1]
        print("| "+option_string.ljust(interface_width-4)+" |")
    return options

def select(options = [["", ""]], override = False):
    if override:
        input('| > ')
        return
    
    valid_option = False
    while not valid_option:
        i = input('| > ').strip()
        for option in options:
            if option[0] == i:
                valid_option = True
                break
        if not valid_option:
            print("Please enter a valid option...")
    return i
    

def close_program():
    exit_program = True

def return_to_previous():
    global previous_page, current_page
    if (previous_page == ""):
        return
    current_page = previous_page
    previous_page = ""

def next_page(next_page):
    global previous_page, current_page
    previous_page = current_page
    current_page = next_page

# COM FUNCTIONS
def find_arduino_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if ("Arduino MKR1000" in p.description):
            return p.name
    if (not arduino_port):
        print("No arduino found. Make sure it is connected to the PC and try again.")
        return False

def list_COM_ports(no_pause = False):
    clear()
    bar()
    ports_options = []
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        ports_options.append([port.name, port.description])
        x = "| "+port.name+" - "+port.description
        print(x.ljust(interface_width-2),"|")
    bar()
    if not no_pause:
        select(override=True)
    else:
        return ports_options

def manually_select_COM():
    options = list_COM_ports(no_pause=True)
    s = select(options)
    return s

# SERIAL FUNCTIONS
def connect_serial():
    global ser, arduino_port
    ser = serial.Serial(arduino_port, 9600, timeout=0.1)
    time.sleep(2)
    return

def disconnect_serial():
    global ser
    ser.close()
    time.sleep(2)
    return

# MAIN FUNCTIONS
def main_menu():
    options = pick_option([
        ['s', 'Search for arduino COM', find_arduino_port],
        ['l', 'List all COMs', list_COM_ports],
        ['S', 'Manually select COM', manually_select_COM]
    ], exit_option=True)
    bar()
    return select(options)

def main_connected():
    if not ser:
        options = pick_option([
            ['c', 'Connect to serial']
        ], exit_option=1, return_option=1)
    else:
        options = pick_option([
            ['d', 'Disconnect serial'],
            ['C', 'Clear arduino flash'],
            ['s', 'Save arduino flash to CSV'],
            ['i', 'Display arduino flash information']
        ], exit_option=True)
    bar()
    return select(options)

# MAIN FUNCTION
while not exit_program:
    clear()
    bar()
    print("|"+(Fore.LIGHTCYAN_EX+"GPSFIKS"+Style.RESET_ALL).center(interface_width+7)+"|")
    bar()
    if arduino_port:
        print("|"+(" Arduino port: "+arduino_port).ljust(interface_width-2)+"|")
        if not serial_connected:
            print("|"+(" Serial connection status: "+(Fore.RED+"█"*4+Style.RESET_ALL)).ljust(interface_width+7)+"|")
        else:
            print("|"+(" Serial connection status: "+(Fore.GREEN+"█"*4+Style.RESET_ALL)).ljust(interface_width+7)+"|")
        bar()
    if current_page == "main":
        s = main_menu()
        if s == "s":
            arduino_port = find_arduino_port()
            if arduino_port:
                next_page("main_connected")
        elif s == "l":
            list_COM_ports()
        elif s == "S":
            arduino_port = manually_select_COM()
            if arduino_port:
                next_page("main_connected")
    elif current_page == "main_connected":
        s = main_connected()
        if s == "c":
            connect_serial()
            if ser.is_open:
                serial_connected = True
        elif s == "d":
            disconnect_serial()
            if not ser.is_open:
                serial_connected = False
                ser = False
    else:
        exit_program = True

    if s == "e":
        exit_program = True
    elif s == "b":
        return_to_previous()

    clear()
    print(Style.RESET_ALL)

    if exit_program:
        clear()
        bar()
        print("|"+"Exiting program...".center(interface_width-2)+"|")
        bar()
        if ser != False:
            ser.close()
        time.sleep(2)