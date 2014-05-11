#!/usr/bin/python
# coding: UTF-8

import os, sys

if len(sys.argv) != 2:
    print 'Usage: python ' + sys.argv[0] + ' <indirpath>'
    sys.exit()

indirpath = os.path.abspath(sys.argv[1]) + os.sep

list = []
for root, dirs, files in os.walk(indirpath):
    for file in files:
        if os.path.isfile(root + os.sep + file):
            ext = os.path.splitext(file)[1]
            if ext == '.jpg':
                if os.path.exists(root + os.sep + file.replace('.jpg', '.surf')):
                    list.append(root + os.sep + os.path.splitext(file)[0])
list.sort()

for elem in list:
    print elem
