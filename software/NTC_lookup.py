#!/usr/bin/python
import math;

#########################################
# ADC defines                           #
#########################################
# ADC reference voltage
ADC_REF = 1.1
# ADC resolution
ADC_BITS = 10
# ADC optional attenuator
ADC_ATTEN = (1/(1+3.3))

#########################################
# NTC defines                           #
#########################################
# NTC reference resistance (at 25 degree celsius)
NTC_REF_R = 10000
# NTC constant A1 (see datasheet)
NTC_CONST_A1 = 3.354016E-03
# NTC constant B1 (see datasheet)
NTC_CONST_B1 = 2.569850E-04
# NTC constant C1 (see datasheet)
NTC_CONST_C1 = 3.51094E-12
# NTC constant D1 (see datasheet)
NTC_CONST_D1 = 6.38309E-15

# Fixed resistor value in voltage divider
NTC_FIXED_R = 811
# Voltage at upper end of divider
NTC_SUPPLY = 5.0

#########################################
# Lookup-table defines                  #
#########################################
# Number of entries (actual table will be one entry longer to both include ADC=0 and ADC=ADC_MAX)
LOOKUP_NUM = 32

#########################################
# !DO NOT CHANGE ANYTHING BEYOND THIS!  #
#########################################
adcMax = pow(2, ADC_BITS);
adcStep = adcMax / LOOKUP_NUM;

adc = 0;
print "const uint8_t NTC_LOOKUP[%d] PROGMEM = {" % (LOOKUP_NUM+1);
while(adc <= adcMax):
    # calculate voltage at ADC pin
    voltageADC = adc*ADC_REF/adcMax;
    # calculate voltage at NTC divider
    voltageNTC = voltageADC/ADC_ATTEN;
    if(voltageNTC == 0):
        temp = 0;
    else:    
        # calculate NTC resistance
        rNTC = NTC_SUPPLY/voltageNTC*NTC_FIXED_R - NTC_FIXED_R;
        # calculate temperature
        ln = math.log(rNTC/NTC_REF_R);
        temp = 1.0/(NTC_CONST_A1+NTC_CONST_B1*ln+NTC_CONST_C1*ln*ln+NTC_CONST_D1*ln*ln*ln)-273.16;
    if(adc+adcStep<=adcMax):
        print "\t\t0x%02x, \t// ADC=%d, Temperature=%d" % (temp, adc, temp);
    else:
        print "\t\t0x%02x \t// ADC=%d, Temperature=%d" % (temp, adc, temp);    
    adc += adcStep;
print "};"
