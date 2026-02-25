TARGET = main
MCU = atmega328p
PORT = /dev/ttyUSB0  #porta che sto utilizzando
BAUD = 115200  #velocità di trasferimento
PROGRAMMER = arduino  #linguaggio che il chip capisce per ricevere dati
F_CPU = 16000000UL  #dice al codice che il chip viaggia a 16 MH


SRCS = $(TARGET).c led/led.c servo/servo.c sensori/sensori.c uart/uart.c

#Quando scrivi make senza argomenti il computer carica questa regola: trasforma il file c in un file eseguibile, 
#avr-gcc chiama il compilatore
#-mmcu=$(MCU) indica che lavorialo per quel chip
#-DF_CPU=$(F_CPU) definisce la velocità
#-Os dice al compilatore di ottimizzare più che può il file
#-o $(TARGET).elf chiama il risultato finale accendi_led.elf
#$(TARGET).c usa come ingrediente base accendi_led.c

#avr-objcopy copia e converte il file oggetto
#-O ihex l'output deve essere in formato Intel Hex
#-R Rimuove la sezione EEPROM, non serve progrmmare la memoria permanente dati ora, risparmio tempo e spazio
#.eeprom $(TARGET).elf $(TARGET).hex prende il file.elf e restituisce il .hex
all:
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -o $(TARGET).elf $(SRCS)
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

#viene eseguita al comando make upload
#avrdude gestisce il trasferimento USB
#-F a volte i chip non rispondono all'identificazione, questo forza il caricamento
#-V disabilita il fatto che il pc dopo aver scritto rilegge per controllare
#-c -p -P -b sono paramenti di arduino
#-D disabilita la cancellazione automatica dell'intero chip
#-U flash:w:$(TARGET).hex  flash è la memoria di destinazione, w sta per write e il .hex è il file da inviare
upload: all
	avrdude -F -V -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -D -U flash:w:$(TARGET).hex

#pulisce, la cartella torna pulita con solo il codice C e il Makefile
clean:
	rm -f *.elf *.hex