#!/usr/bin/python

import math;

# CPU frequency in Hz
F_CPU = 20000000;
# Maximum counter value
CNT_MAX = 255;
# Length of required PWM look-up table (additional entries will be dropped)
LOOKUP_LENGTH = 1024;

# Maximum switching frequency in Hz
MAX_FREQ = 1000000;
# Minimum switching frequency in Hz
MIN_FREQ = 78125;


# Minimum duty cycle
MIN_DUTY = 1;
# Maximum duty cycle
MAX_DUTY = 70;

# calculate min/max period register values
MIN_PERIOD = int(math.ceil(F_CPU/MAX_FREQ)) - 1;
MAX_PERIOD = int(math.floor(F_CPU/MIN_FREQ)) - 1;

if (MAX_PERIOD > CNT_MAX):
	print "Minimum frequency not possible with this counter.";
	exit();

#print "Min. period: ", MIN_PERIOD;
#print "Max. period: ", MAX_PERIOD;

PWMList = [];

for period in range(MIN_PERIOD, MAX_PERIOD):
#	print "Calculating PWM values for period: ", period;
	min_ocr = int(math.ceil(float(period+1)*MIN_DUTY/100-1));
	max_ocr = int(math.floor(float(period+1)*MAX_DUTY/100-1));
	for ocr in range(min_ocr, max_ocr):
		PWMList.append([float(ocr+1)*100/(period+1), ocr, period]);

PWMList.sort();

#remove duplicate duty cycle entries
i = 0;
while i < len(PWMList)-1:
	while (PWMList[i][0] == PWMList[i+1][0]):
		PWMList.pop(i);
	i = i + 1;

#for item in PWMList:
#	print item;

print "Found %d combination of PWM/TOP values" % len(PWMList);

# extract even spaced entries
print "Trimming it down to %d values..." % LOOKUP_LENGTH;
minDuty = PWMList[0][0];
maxDuty = PWMList[len(PWMList)-1][0]; #get last entry
stepsize = (maxDuty-minDuty)/(LOOKUP_LENGTH-1);
print "Average size between steps: ", stepsize;
# print look-up table as array of LOOKUP_LENGTH * 2
print "Lookup-table format is: {OCR, TOP}, resulting in ascending duty cycle value, starting at %.2f" % minDuty;
print "COPY FROM HERE ----------------------------->"
print "const uint8_t PWM_LOOKUP[][2] PROGMEM = {"
for steps in range(0, LOOKUP_LENGTH):
	i = 0;
	compValue = minDuty + stepsize * steps;
	while(i<len(PWMList)-1 and PWMList[i][0] < compValue):
		i = i + 1;
	#print "Step %d: %.2f %d %d" % (steps, PWMList[i][0], PWMList[i][1], PWMList[i][2]);
	if(steps < LOOKUP_LENGTH - 1):
		print "{0x%02x, 0x%02x}, \t// Duty cycle = %.2f" % (PWMList[i][1], PWMList[i][2], PWMList[i][0]);
	else:
		print "{0x%02x, 0x%02x}  \t// Duty cycle = %.2f" % (PWMList[i][1], PWMList[i][2], PWMList[i][0]);
print "};"
print "<----------------------------- COPY TILL HERE"

