EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L VSmile:J1B1211CCD J1
U 1 1 61544D31
P 5950 3950
F 0 "J1" H 5950 5115 50  0000 C CNN
F 1 "J1B1211CCD" H 5950 5024 50  0000 C CNN
F 2 "Connector_RJ:RJ45_Cetus_J1B1211CCD_Horizontal" H 5950 5050 50  0001 C CNN
F 3 "" H 5800 3450 50  0001 C CNN
	1    5950 3950
	1    0    0    -1  
$EndComp
Text GLabel 4500 4400 0    50   Input ~ 0
TX+
Text GLabel 4500 4000 0    50   Input ~ 0
TX-
Text GLabel 4500 4700 0    50   Input ~ 0
GND
Text GLabel 4500 4600 0    50   Input ~ 0
LED
Text GLabel 4500 3500 0    50   Input ~ 0
RX-
Text GLabel 4500 3900 0    50   Input ~ 0
RX+
Wire Wire Line
	4500 4400 5250 4400
Wire Wire Line
	4500 4000 5250 4000
Wire Wire Line
	4500 3900 5250 3900
Wire Wire Line
	4500 3500 5250 3500
Wire Wire Line
	4500 4600 5250 4600
Wire Wire Line
	4500 4700 4650 4700
Wire Wire Line
	5250 3700 5000 3700
Wire Wire Line
	5000 3700 5000 4200
Wire Wire Line
	5000 4200 5250 4200
Wire Wire Line
	4650 4700 4650 3300
Wire Wire Line
	4650 3300 5250 3300
Connection ~ 4650 4700
Wire Wire Line
	4650 4700 4850 4700
$Comp
L Device:C C12
U 1 1 6154C585
P 4850 4350
F 0 "C12" H 4700 4450 50  0000 L CNN
F 1 "0.1uF" H 4950 4350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4888 4200 50  0001 C CNN
F 3 "~" H 4850 4350 50  0001 C CNN
	1    4850 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4200 4850 4200
Connection ~ 5000 4200
Wire Wire Line
	4850 4500 4850 4700
Connection ~ 4850 4700
Wire Wire Line
	4850 4700 5250 4700
NoConn ~ 5250 4800
NoConn ~ 5250 4900
NoConn ~ 6650 4750
$EndSCHEMATC
