import os
import sys

dirr = "C:\\OpenCASCADE-7.5.0-vc14-64\\opencascade-7.5.0\\win64\\vc14\\lib"
dird = "C:\\OpenCASCADE-7.5.0-vc14-64\\opencascade-7.5.0\\win64\\vc14\\libd"

mylist = os.listdir(dird)

for f in mylist:
    if f[-3:] == "lib":
        print(f)