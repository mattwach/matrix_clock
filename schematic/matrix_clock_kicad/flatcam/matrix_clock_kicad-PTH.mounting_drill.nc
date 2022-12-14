(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: matrix_clock_kicad-PTH.drl_edit_3_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Sunday, 11 December 2022 at 07:05)

(Specially modified by mattwach to work with the Sainsmart 3080 that I have.)


(TOOLS DIAMETER: )
(Tool: 1 -> Dia: 2.0)

(FEEDRATE Z: )
(Tool: 1 -> Feedrate: 120.0)

(FEEDRATE RAPIDS: )
(Tool: 1 -> Feedrate Rapids: 1500)

(Z_CUT: )
(Tool: 1 -> Z_Cut: -1.7)

(Tools Offset: )
(Tool: 1 -> Offset Z: 0.0)

(Z_MOVE: )
(Tool: 1 -> Z_Move: 2)

(Z Toolchange: 15 mm)
(X,Y Toolchange: 0.0000, 0.0000 mm)
(Z Start: None mm)
(Z End: 0.5 mm)
(X,Y End: 0.0000, 0.0000 mm)
(Steps per circle: 64)
(Preprocessor Excellon: default)

(X range:    2.7000 ...   51.8000  mm)
(Y range:  -57.7000 ...   -2.6000  mm)

(Spindle Speed: 10000 RPM)
G21
G90
G17
G94


G01 F120.00

M5             
G00 Z15.0000
G00 X0.0000 Y0.0000                
(MSG, Change to Tool Dia = 2.0000 ||| Total drills for tool T1 = 2)
G00 Z15.0000
        
G01 F120.00
M03 S10000
G00 X3.7000 Y-3.6000
G01 Z-1.7000
G01 Z0
G00 Z2.0000
G00 X50.8000 Y-56.7000
G01 Z-1.7000
G01 Z0
G00 Z2.0000
M05
G00 Z0.50
G00 X0.0 Y0.0


