import sys
import time
from tkinter.tix import MAX
from traceback import print_tb
import serial
import random
import subprocess

MAX_WISDOM_LEN = 80

NOTIFY_CMD = '''
on run argv
  say (item 2 of argv) using "Alex" speaking rate 200 pitch 90 modulation 60
  display notification (item 2 of argv) with title (item 1 of argv)
end run
'''

wisdom = []

def load_wisdom():
    global wisdom

    current_wisdom = ''

    with open('wisdom.txt', 'r') as wisdom_file:
        for line in wisdom_file.readlines():
            if len(line.strip()) == 0:
                if len(current_wisdom) < MAX_WISDOM_LEN:
                    wisdom += [current_wisdom]
                current_wisdom = ''
            else:
                current_wisdom += line

load_wisdom()

poses = [
  0b0000,
  0b0001,
  0b1001,
  0b1101,
  0b0101,
  0b0100,
  0b1100,
  0b1000,
]

if len(sys.argv) != 2:
    print('Must specify port name')
    sys.exit(1)

port_name = sys.argv[1]

print('Connecting to cat on port {}'.format(port_name))
print()

port = serial.Serial(port_name, 115200)

def notify(title, text):
  subprocess.call(['osascript', '-e', NOTIFY_CMD, title, text])

def go_to_pose(index):
    if index < 0 or index >= len(poses):
        raise Exception('Pose index out of range')

    msg ='{}\n'.format(str(poses[index]))
    port.write(msg.encode('utf8'))

pose_idx = 0
def dispense_wisdom():
    global pose_idx
    pose_idx += 1

    if pose_idx >= len(poses):
        pose_idx = 0

    go_to_pose(pose_idx)

    wisdom_idx = int(random_between(0, len(wisdom)))
    what_the_cat_says = wisdom[wisdom_idx]
    print('The cat says:\n\n{}\n'.format(what_the_cat_says))
    notify('Cat head says', what_the_cat_says)

def random_between(rmin, rmax):
    return rmin + random.random() * (rmax - rmin)

while True:
    dispense_wisdom()
    time.sleep(random_between(5, 30));
