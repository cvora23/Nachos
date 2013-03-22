#!/bin/tcsh

set number1 = 5
set number2 = 10

while ($number1 <= 10)

if ($number1 == $number2) then
    echo "number1 is is equal to number2"
else
    echo "number1 is not equal to number2"
endif

set number1=('expr $number1 + 1')

end    