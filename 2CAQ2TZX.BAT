rem 2CAQ2WAV -a -f11 -q Chess_A.CAQ
rem sox chess_A_11Q.WAV chess_A_11Q.VOC
rem direct chess_A_11Q.VOC chess_A_11Q.tzx

forfiles /m *.caq /C "cmd /c 2caq2wav -a -f11 -q @file
forfiles /m *.wav /C "cmd /c sox @file @fname.voc
del *.wav
forfiles /m *.voc /C "cmd /c direct @file @fname.tzx
del *.voc
mkdir CAQTZX
move *.tzx CAQTZX
PAUSE