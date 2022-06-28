import os

interface_width = 50
previous_page = ""
current_page = "main"
exit_program = False
s = ""
COM_port = ""

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

def select(options):
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
    if (previous_page == ""):
        return
    current_page = previous_page
    previous_page = ""

def next_page(next_page):
    previous_page = current_page
    current_page = next_page

# COM FUNCTIONS
def find_arduino_port():
    pass
def list_COM_ports():
    pass
def manually_select_COM():
    # Ask user to enter a COM port
    return "COM5"
    
# MAIN FUNCTIONS
def main_menu():
    bar()
    print("|"+"GPSFIKS".center(interface_width-2)+"|")
    bar()
    options = pick_option([
        ['s', 'Search for arduino COM', find_arduino_port],
        ['l', 'List all COMs', list_COM_ports],
        ['S', 'Manually select COM', manually_select_COM]
    ], exit_option=True)
    bar()
    s = select(options)
    if s == "s":
        find_arduino_port()
    elif s == "l":
        list_COM_ports()
    elif s == "S":
        COM = manually_select_COM()
    else:
        return s
    if COM != "":
        next_page("main_connected")


# MAIN FUNCTION
while not exit_program:
    clear()
    if current_page == "main":
        s = main_menu()
    else:
        exit_program = True

    if s == "e":
        exit_program = True
    elif s == "r":
        return_to_previous()