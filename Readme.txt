===== How to use VFLCRExcelConverter =====
VFLCRExcelConverter program created to help someone to create test LCR and G-code files.
You start by populating cells in the Excel spreadsheet. VFLCR_Example.xlsx can be used as a staring points.
VFLCRExcelConverter program only looking into colums A throug L. Other columns will be ignored.
It will also ignore first row in the spreadsheet.
Columns A though G are used to generate LCR file, while H through L are used to generate G-code file.

VFLCRExcelConverter converts columns A-G directly into LCR commands.
A: Position of the LCR command in encoder ticks
B: Laser On/Off: 0 - off, 1 - on
C: Laser power: 0 - 255
D: PWM duty cycle: 0 - 0 %, 255 - 100%
E: PWM period in usec: 0 - 255
F: SOT: put 1 for SOT command, otherwise leave blank
G: EOT: put 1 for EOT command, put 1 to SOT and EOT for end of layer, otherwise leave blank

VFLCRExcelConverter converts columns H-L to G-code file.
H: Move's from X position in mm
I: Move's from Y position in mm
J: Move's to positon in mm
K: Move's to positon in mm
L: Move's speed in mm/sec
Each line in the spreadsheet will generate three lines in the G-code file:
For example these two lines:
493.868,200.268,296.668,200.268,600
296.668,190.268,493.868,190.268,600
will generate these commands:
N0001 G90 G01 X493.868 Y200.268 F36000
N0002 M20
N0010 G90 G01 X296.668 Y200.268 F36000
N0011 G90 G01 X296.668 Y190.268 F36000 M22
N0012 M20
N0020 G90 G01 X493.868 Y190.268 F36000
Program will automatically adds these two lines at the end of G-code file:
N0021 M22
N0022 M30

To generate LCR and G-code files first save your spreadhseet using .
VFLCRExcelConverter <CVS file name>
The converter will generate 2 files with same name and files types .txt and .cnc.
Convert .txt into binary format using PacketAsciiToBinary.exe program if needed.
You can use ExcelToBinary.bat to run steps above. To use it type:
ExcelToBinary.bat <CSV file>  <---- Use only file name w/o file type
This batch file also copies .vflc and .cnc files into:
VF-00001-0001_L00001_R01_P01.vflc
VF-00001-0001_L00001_R01_P01.cnc

===== How to use VFLCR_Box.xlsx
VFLCR_Box.xlsx file has been created to generate LCR and  G-code files to lase perimeter of the box.
1. Open VFLCR_Box.xlsx file and make sure you are on VFLCR_Box_X sheet.
2. Change parameters outlined in yellow if needed.
3. Save this sheet in CSV format.
4. Switch to VFLCR_Box_Y sheet and save it CSV format.
5. Run VFLCRExcelConverter progam on each CSV file. Or, you type "VFLCR_Box.bat VFLCR_Box" This batch will generate 4 files.


