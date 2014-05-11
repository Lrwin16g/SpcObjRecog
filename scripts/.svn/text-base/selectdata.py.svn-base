#!/usr/bin/python
# coding: UTF-8

import os, sys

def shell(command):
    print command
    os.system(command)

if len(sys.argv) != 3:
    print 'Usage: python ' + sys.argv[0] + ' <indirpath> <outdirpath>'
    sys.exit()

indirpath = sys.argv[1] + os.sep
outdirpath = sys.argv[2] + os.sep

dirs = []
for dir in os.listdir(indirpath):
	if os.path.isdir(indirpath + dir):
		dirs.append(dir)
for dir in dirs:
    shell('mkdir -p ' + outdirpath + dir)

for dir in dirs:
    path = indirpath + dir + os.sep
    files = []
    for file in os.listdir(path):
        if os.path.isfile(path + file):
            ext = os.path.splitext(file)[1]
            if ext == '.jpg':
                files.append(file)
    files.sort()
    for file in files[:10]:
        shell('cp ' + path + file + ' ' + outdirpath + dir + os.sep + file)
