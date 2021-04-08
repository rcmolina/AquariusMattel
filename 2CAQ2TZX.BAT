rem 2CAQ2WAV Chess_A.CAQ
rem sox chess_A_2Q.WAV chess_A_2Q.VOC
rem direct /t 322 chess_A_2Q.VOC chess_A_2Q_11KHz.tzx

forfiles /m *.caq /C "cmd /c 2caq2wav @file
forfiles /m *.wav /C "cmd /c sox @file @fname.voc
del *.wav
forfiles /m *.voc /C "cmd /c direct @file @fname.tzx
del *.voc
mkdir CAQTZX
move *.tzx CAQTZX
PAUSE