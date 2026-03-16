.dataseg
.codeseg
CALL @seven
CALL @print
POP
POP
CALL @forty_two
CALL @print
POP
POP
CALL @seven
CALL @forty_two
PLUS
CALL @print
POP
POP
HALT
.function seven
seven:
PUSH  7 
RETURNV
PUSH 0
RETURNV
.function forty_two
forty_two:
PUSH  42 
RETURNV
PUSH 0
RETURNV
