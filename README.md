This main function is control relay via url parameter, with available parameter:

1. http://**[WEMOS_IP]**/?p=**[INTEGER_VALUE]** to change selected pin state to binary value, for example http://**[WEMOS_IP]**/?p=6 will make pin 1 and pin 2 turn on and another pin turn off because 6 (integer) is 00000110 (binary)
2. http://**[WEMOS_IP]**/0/?p=**[PIN_NUMBER]** to turn off selected pin state addresed by p, for example http://**[WEMOS_IP]**/0/?p=1&p=2 will make pin 1 and pin 2 turn off and another pin unchanged
3. http://**[WEMOS_IP]**/1/?p=**[PIN_NUMBER]** to turn on selected pin state addresed by p, for example http://**[WEMOS_IP]**/1/?p=1&p=2 will make pin 1 and pin 2 turn on and another pin unchanged
4. http://**[WEMOS_IP]**/2/?p=**[PIN_NUMBER]** to turn toggle selected pin state addresed by p, for example http://**[WEMOS_IP]**/2/?p=1&p=2 will make pin 1 and pin 2 turn toggle and another pin unchanged
5. http://**[WEMOS_IP]**/ui will show you user interface of this control
