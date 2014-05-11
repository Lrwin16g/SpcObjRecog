#!/usr/bin/python
# coding: UTF-8

import os, sys

def shell(command):
    print command
    os.system(command)

if len(sys.argv) != 3:
    print 'Usage: python ' + sys.argv[0] + ' <descriptor_path> <dirpath>'
    sys.exit()

exe = sys.argv[1]
dirpath = sys.argv[2]

for root, dirs, files in os.walk(dirpath):
    for file in files:
        ext = os.path.splitext(file)[1]
        if ext == '.jpg':
            command = exe + ' ' + root + os.sep + file + ' ' + root + os.sep + file.replace(ext, '.surf')
            shell(command)
