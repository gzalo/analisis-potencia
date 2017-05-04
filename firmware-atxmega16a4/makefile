all:
	avr-gcc -Wall -Os -mmcu=atxmega16a4 -c test_aes.c -o test_aes.o
	avr-gcc -Wall -Os -mmcu=atxmega16a4 -c aes.c -o aes.o
	
	avr-gcc -Wall -Os -mmcu=atxmega16a4 -o test_aes.elf test_aes.o aes.o

	avr-objcopy -j .text -j .data -O ihex test_aes.elf test_aes.hex

run:
	avrdude -p atxmega16a4 -P com3 -c avr109 -b 115200 -D -U flash:w:test_aes.hex
