#!/usr/bin/env python3

import os
import serial
import serial.tools.list_ports
import threading

#---- raw keyboard input ---- 
if os.name == 'nt':
  import msvcrt  
  esctbl = {'H':'\x1b[A','K':'\x1b[D','P':'\x1b[B','M':'\x1b[C',  # cursor keys
          'O':'\x05','S':'\b','Q':'pdn','I':'pup','G':'\x01','R':'ins'}
  def key_hit():
    return msvcrt.kbhit()
  def get_key():
    key = msvcrt.getch()
    if key == b'\xe0':  # cursor key -> esc seqyence
      key = msvcrt.getch()
      c = esctbl[key.decode()]
    elif key == b'\r':
      c = '\n' 
    else:
      try:
        c = key.decode()
      except:
        c = key
    return c
elif os.name == 'posix':
    import sys
    import select
    import tty
    import termios
    import atexit
    def key_hit():
      return select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])
    def get_key():
      return sys.stdin.read(1)
    def restore_settings():
      termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)
    atexit.register(restore_settings)
    old_settings = termios.tcgetattr(sys.stdin)
    tty.setcbreak(sys.stdin.fileno())
else:
  print(os.name(), "unknown os.")
  exit()

#---- tx data --------------------

def uart_tx(s):
  uart.write(s.encode())

def putc(c):
  print(str(c), end='', flush=True)

def serial_write():
  global uart
  
  while(1):
    if uart !='':
      try:
        #c = input()+'\n'
        c = get_key()
        uart_tx(c)
      except KeyboardInterrupt:
        return

#---- rx data ------------------
def uart_rx():
  global uart
  return uart.read(1).decode()

EOT = '\x04'
ESC = '\x1b'

def serial_read():
  global uart
  while(1):
    if uart !='':
      c = uart_rx()
      if c == ESC:
        c = uart_rx()
        if c == '<':  # .mot (S-format) download
          c = uart_rx()
          fn = ''
          while(c != '\n'):
            fn += c
            c = uart_rx()
          fn += '.mot'
          try:
            with open(fn) as f:
              print("Down loading:", fn)
              mot = f.readlines()
            for srec in mot:
              putc('.')
              uart_tx(srec)
            print()
          except:
            print(f" file '{fn}' not found.\n");
            uart_tx("S70500002000DA\n")
          uart_tx(EOT)
        else:
          putc(ESC)
          putc(c)
      else:
        putc(c)

#---- open serial port ------------------
def open_serial(baud):
  global uart
  
  # get port list
  serial_ports={}
  for i,port in enumerate(serial.tools.list_ports.comports()):
      serial_ports[str(i)]=port.device
      print(i, port, port.location)
  print(f'select port : ', end='', flush=True)
  p = get_key()
  putc(p)
  port_val = serial_ports[p]
  uart = serial.Serial(port=port_val, baudrate=baud, parity= 'N')
  print(f'\n open "{port_val}"  {baud}baud')


if __name__ == '__main__':

  uart=''
  
  #port open
  open_serial(115200)

  thread_1 = threading.Thread(target=serial_write)
  thread_2 = threading.Thread(target=serial_read)

  try:
    thread_1.start()
    thread_2.start()
  except KeyboardInterrupt:
    print('bye')
