#!/usr/bin/env python3
#
# convert 32bit float <=> unsigned
#

import PySimpleGUI as sg
import ctypes

def fcnv(str):
    if str:
        if '.' in str:
            try:
                flt = float(str)
            except:
                print("???", str)
                flt = 0.0
            uint = (ctypes.c_uint32.from_buffer(ctypes.c_float(flt)).value)
        else:
            try:
                uint = int(str, 16)
            except:
                print("???", str)
                uint = 0
            flt = ctypes.c_float.from_buffer(ctypes.c_uint32(uint)).value
    else:
        flt, uint = 0.0, 0
    return flt, uint

def settings_window():

    table = [["",""]]

    layout = [[sg.Text('float or u32 value')],
              [sg.Input(sg.user_settings_get_entry('input', ''), k='-IN-')],
              [sg.Text('', k='-OUT-')],
              [sg.Table(values=table, headings=["  float    ", "unsigned int"], enable_events = True, expand_x=True, k='-TABLE-')],
              [sg.Button('Exit', k='Exit'), sg.Button('Clear', k='Clr'), sg.Button('Convert', k='Conv')]]
    window = sg.Window('float u32 converter', layout)

    while True:
        event, values = window.read()
        if event in (sg.WINDOW_CLOSED, 'Exit'):
            break
        if event == 'Conv':
            flt,uint = fcnv(values['-IN-'])
            table.insert(0, ["%.9g"%flt,"%08x"%uint])
            #table.append([flt, uint])
            exp = uint >> 23
            sgn = exp >> 8
            uexp = exp & 0xff
            frac = uint & 0x7ffff
            window['-OUT-'].update("%8g  %08x  %c 2^%d %06x"%(flt,uint, '-' if sgn==1 else '+', uexp-127 , frac))
            window['-TABLE-'].update(values=table)
        if event == 'Clr':
            window['-IN-'].update("")
        if event == '-TABLE-':
            if values['-TABLE-']:
                print(table[values['-TABLE-'][0]])


if __name__ == '__main__':
    settings_window()
