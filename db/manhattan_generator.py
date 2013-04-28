#!/usr/bin/python

import sys # for argv

if len(sys.argv) < 7:
  print("Usage: x y num_x num_y dist_x dist_y")
  print("x,y - coords to top left corner of city")
  print("num_x(y) - number of blocks in X(Y)-axis")
  print("dist_x(y) - distance between blocks on given axis")
  sys.exit("Not enough parameters given.")

x = float(sys.argv[1])
y = float(sys.argv[2])
num_x = int(sys.argv[3])
num_y = int(sys.argv[4])
dist_x = float(sys.argv[5])
dist_y = float(sys.argv[6])

nid = 1
sid = 1

print("-- Automatically generated Manhattan-style city,")
print("--\tstarting in (top-left corner): "+str(x)+","+str(y))
print("-- number of blocks:")
print("--\tin X-axis: "+str(num_x))
print("--\tin Y-axis: "+str(num_y))
print("-- distance between blocks:")
print("--\tin X-axis: "+str(dist_x))
print("--\tin Y-axis: "+str(dist_y))
print("\n-- Author: Maciej 'mac' Grzybek\n")


print("BEGIN;")

for i in range(num_x):
  for j in range(num_y):
    x_pos = x+(i*dist_x)
    y_pos = y+(j*dist_y)
    print("INSERT INTO streetnodes VALUES("+str(nid)+","+str(x_pos)+","+str(y_pos)+");")
    if (j != 0): # if not first node in line, connect current and recent node with street
      print("INSERT INTO streets VALUES("+str(sid)+","+str(nid-1)+","+str(nid)+");") # vertical connection
      sid += 1
    if (i != 0):
      print("INSERT INTO streets VALUES("+str(sid)+","+str(nid-num_x)+","+str(nid)+");") # horizontal connection
      sid += 1
    nid += 1

print("END;")
