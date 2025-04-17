# AquariusMattel

Follow this procedure (part1 is Chess_BAS , part2 is Chess_A):

* 2CAQ2WAV -a -f11 -q Chess_BAS.CAQ
* sox chess_BAS_11Q.WAV chess_BAS_11Q.VOC
* direct chess_BAS_11Q.VOC chess_BAS_11Q.tzx

* 2CAQ2WAV -a -f11 -q Chess_A.CAQ
* sox chess_A_11Q.WAV chess_A_11Q.VOC
* direct chess_A_11Q.VOC chess_A_11Q.tzx

or even better with tzx id11:

* 2caq2wav -a -f22 -q -r Chess_BAS.caq
* wavtotzx -pilot 0 Chess_BAS_22QR.wav Chess_BAS_id11.tzx

* 2caq2wav -a -f22 -q -r Chess_A.caq
* wavtotzx -pilot 0 Chess_A_22QR.wav Chess_A_id11.tzx