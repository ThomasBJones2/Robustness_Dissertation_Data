import os
from subprocess import call
import argparse
import time
import glob
from multiprocessing import Pool
import sys      
from shutil import copyfile


Criticality_Data_Files = './processed_criticality_results/*'

if __name__ == "__main__":
    for file_name in glob.glob(Criticality_Data_Files):
        with open(file_name, 'r') as f: 
            l_up = 0
            l_down = 0
            l_up_count = 0
            l_down_count = 0
            for i, line in enumerate(f.readlines()):
                if i > 0:
                    vals = line.split(" ")
                    median = vals[5]
                    criticality = vals[1]
                    datapoints = vals[3]
                    if float(criticality) >= float(median):
                        l_up += float(criticality)
                        l_up_count += 1
                    else:
                        l_down += float(criticality)
                        l_down_count += 1
            leverage = float(l_up)*float(l_down_count)/(float(l_down)*float(l_up_count))\
                    if l_down > 0 and l_up_count > 0 else 1.0
            if not "ArrayIndexOutOfBoundsException" in file_name:
                print "\n\n"
                print file_name.split("/")[2] + " " + str(leverage)



