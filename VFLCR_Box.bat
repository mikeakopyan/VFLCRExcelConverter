VFLCRExcelConverter.exe %1_X.csv
PacketAsciiToBinary.exe /f %1_X.txt /o .
copy %1_X.vflc VF-00001-0001_L00001_R01_P01.vflc
copy %1_X.cnc  VF-00001-0001_L00001_R01_P01.cnc
VFLCRExcelConverter.exe %1_Y.csv
PacketAsciiToBinary.exe /f %1_Y.txt /o .
copy %1_Y.vflc VF-00001-0001_L00002_R01_P01.vflc
copy %1_Y.cnc  VF-00001-0001_L00002_R01_P01.cnc