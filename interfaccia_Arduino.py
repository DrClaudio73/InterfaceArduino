import serial
import subprocess
from time import sleep as dormi
from lib_menu import menu_scelta
from lib_Arduino_Funcs import CreaDb, LeggiIn, ScriviOut, SetPinMode, GetPinMode, PrintArdStatus
import serial.tools.list_ports
from guizero import App, Text, TextBox, PushButton, Slider, Picture, Combo, CheckBox , ButtonGroup, info

debug = False #allows working on development without having Arduino connected

#MAIN PROGRAM
continua = True
db =[]
ports = list(serial.tools.list_ports.comports())

baudrate=9600 #keep a default baudrate
Autofind="No"
useDefaultUART="No"
defaultUART="/dev/ttyS0"

f = open('config_IfArduino.txt', 'r')
with f:
	configurazione=f.readlines()

for linea in configurazione:
	if "baudrate" in linea: baudrate=int(linea.strip("baudrate="))
	if "useDefaultUART" in linea: 
		useDefaultUART=linea.strip("useDefaultUART= ").upper().strip("\n")
	if "default_UART" in linea: defaultUART=linea.strip("default_UART= ").strip("\n")         
	if "Autofind" in linea: Autofind=linea.strip("Autofind= ").upper()

"""
print(baudrate)
print(useDefaultUART)
print(defaultUART)
print(Autofind)
input("vai!!!!>>>")
"""

#app = App(title="Interface Arduino on Serial Port", width=700, height=500, layout="grid")

menu_scelta_UART=[]
if "YES" in Autofind:
        arduino_ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'Arduino' in p.description
        ]
        #print("arduino_ports: ",arduino_ports)
        #print("arduino_ports[0]: ",arduino_ports[0])
        if len(arduino_ports)>0:
                menu_scelta_UART.append(" ")
                menu_scelta_UART.append(arduino_ports[0])
                menu_scelta_UART.append("None")
                numeroUART=1
                useDefaultUART="NO"
                Autofind="YES"
        else:
                if (useDefaultUART=="YES"):
                        print("No Arduino found in automode.... try to open default serial PORT specified in configuration file")
                else:
                        print("No Arduino found in automode.... please specify serial PORT")
                Autofind = "NO"
else:
        Autofind = "NO"
        
if (Autofind == "NO") and (useDefaultUART=="YES"):
	menu_scelta_UART.append(" ")
	menu_scelta_UART.append(defaultUART)
	menu_scelta_UART.append("None")
	numeroUART=1
else:
    useDefaultUART="NO"

if (Autofind == "NO") and (useDefaultUART=="NO"):
        menu_scelta_UART=["SELECTION OF SERIAL PORT NUMBER TO USE"]
        lx_win = "Windows"
        for p in ports:
                #print(p)
                #print(p[0])
                #print("DESCRIPTION: ",p.description)
                #print("NAME: ",p.name)
                menu_scelta_UART.append(p.description)
                #print("device: ",p.device)
                #print("type:", type(p.device))
                #print("ports:", type(ports))
                #if ('/dev/' in p.device): print("typeif:", type(p.device))
                if ('/dev/tty' in p.device): lx_win="Linux"
        
        ttyS0_Exists =  str(subprocess.check_output(['ls','-al','/dev']))
        #print("output",ttyS0_Exists)
        #input("output")
        if "ttyS0" in str(ttyS0_Exists): menu_scelta_UART.append("/dev/ttyS0")
		        
        menu_scelta_UART.append("Cancel")

        #numeroUART = int(menu_scelta(menu_scelta_UART,True))
		
        #if (numeroUART==len(menu_scelta_UART)-2):
        #        print("Goodbye!!! ")
        #        exit(0)

"""
print("Autofind: ",Autofind)
print("useDefaultUART: ",useDefaultUART)
print("menu_scelta_UART[numeroUART]: ",menu_scelta_UART[numeroUART])
print("menu_scelta_UART: ",menu_scelta_UART)

"""

def UART_Selected(s):
	"""
	print("combo val= ",UART_choice.get())
	print("combo vals= ",s)
	print("ports= ",ports)
	print("menu_scelta_UART= ",menu_scelta_UART)
	"""
	#ser = serial.Serial(s, baudrate, timeout=3)
	i=0
	
	for p in serial.tools.list_ports.comports():
		#print(s," YYY ",i," XXX ",p.device)
		if s in p.device:
			numeroUART=i
		i=i+1
	print("test= ", s=="/dev/ttyS0", s in "/dev/ttyS0") 
	if s=="/dev/ttyS0":
		ser = serial.Serial("/dev/ttyS0", baudrate, timeout=3)
	elif  (Autofind == "YES") or (useDefaultUART=="YES"):
		ser = serial.Serial(menu_scelta_UART[1], baudrate, timeout=3)
	else:
		ser = serial.Serial(ports[numeroUART].device, baudrate, timeout=3)

	ser.reset_input_buffer()
	dormi(2) #let Arduino get started!
	print(ser)
	CreaDb(db,ser)
	ArduinoReady.set("ricerca-google-pallini-colorati-1.png")
	return(ser)

app = App(title="Arduino Interface via Serial Port", width=700, height=500, layout="grid")

menu_scelta_UART_GUI=menu_scelta_UART[1:len(menu_scelta_UART)-1]

UART_choice = Combo(app, options=menu_scelta_UART_GUI, command = UART_Selected, grid=[0,1], align="left")
UART_text = Text(app, text="Which UART?", grid=[0,0], align="left")
ArduinoReady = Picture(app,image="download.png", grid=[0,2])

app.display()

if not(debug):
	if menu_scelta_UART[numeroUART+1]=="/dev/ttyS0":
		ser = serial.Serial("/dev/ttyS0", baudrate, timeout=3)
	elif  (Autofind == "YES") or (useDefaultUART=="YES"):
		ser = serial.Serial(menu_scelta_UART[numeroUART], baudrate, timeout=3)
	else:
		ser = serial.Serial(ports[numeroUART].device, baudrate, timeout=3)

	ser.reset_input_buffer()
	print("Port Name: ",ser.name)
	print("Port details: ", ser)
	#input("avanti")
else:
    ser = 0 
    
if not(menu_scelta_UART[numeroUART+1]=="/dev/ttyS0"):
	dormi(2) #let Arduino get started!

CreaDb(db,ser)

while continua:
    main_menu=["FUNCTION SELECTION","Read an input","Write output","Set Pin Mode","Get Pin Mode","Show Arduino I/O status","Exit"]
    scelta = menu_scelta(main_menu)
    funcs = {'1':LeggiIn, '2':ScriviOut,  '3':SetPinMode, '4':GetPinMode, '5':PrintArdStatus}
    
    if int(scelta,10) in range(len(main_menu)-1):
        func = funcs[scelta]
        res = func(db,ser)
    else:
        print("Goodbye!!! ")
        continua = False

if not(debug):
    ser.close()
