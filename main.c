#define F_CPU 16000000UL //definisco la frequenza di clock a 16 MHz

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>

//includo le mie librerie
#include "common.h" //contiene le definizioni delle variabili globali
#include "led/led.h" //contiene le funzioni per gestire i led
#include "servo/servo.h" //contiene le funzioni per gestire il servo
#include "sensori/sensori.h" //contiene le funzioni per gestire i sensori
#include "uart/uart.h" //contiene le funzioni per gestire la comunicazione seriale
#include "tastiera/tastiera.h" //contiene le funzioni per gestire l'inserimento da tastiera


const uint8_t codice_segreto[3] = {1, 2, 3};  //password di ingresso
int posti_liberi = MAX_POSTI; //MAX_POSTI è definito in common.h e vale 5


//FUNZIONE PER GESTIRE IL TRANSITO (auto che entra o esce)
void gestisci_transito(uint8_t pin_partenza, uint8_t pin_arrivo, int variazione_posti) {
    bool passata = true;  //indica se l'auto è passata o no
    open_barrier();  
    led_on(led_verde);
    led_off(led_rosso);

    int timer = 0;  //cronometro di sicurezza

    // aspetto che l'auto arrivi al secondo sensore
    while(!rilevato_ostacolo(pin_arrivo)){
        _delay_ms(50);
        timer++;
        
        // metto in pausa il timer se l'auto resta ferma sul primo sensore
        while(rilevato_ostacolo(pin_partenza)){
            _delay_ms(50);
        }
        
        //se scade il timer l'auto ha fatto retromarcia e passata diventa false
        if(timer >= 100){
            passata = false;
            break; 
        }
    }

    //controllo se l'auto è effettivamente passata
    if(passata == true){
        // L'auto è sul secondo sensore, aspetto che lo liberi
        while(rilevato_ostacolo(pin_arrivo)){
            _delay_ms(50);
        }

        // L'auto ha liberato l'area di transito, posso scalare il posto
        posti_liberi += variazione_posti; 
        
        // stampo il numero di posti liberi
        char buffer[30];
        sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi);
        uart_print(buffer);
    }
    else {
        //auto fa retromarcia dopo aver scritto pin corretto
        uart_print("Transito annullato (retromarcia).\r\n");
        char buffer[30];
        sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi);
        uart_print(buffer);
    }
    //Chiusura rapida (se ci sta un auto accodata dietro gli viene chiusa la sbarra)
    close_barrier();
    led_on(led_rosso);
    led_off(led_verde);
}







//Funzione per la stampa in caso di parcheggio pieno
void avviso_parcheggio_pieno(bool ostacolo_in, bool ostacolo_out, bool *avviso_stampato) {
    // se c'è un auto in ingresso e non in uscita
    if(ostacolo_in && !ostacolo_out){
        if(*avviso_stampato == false){
            uart_print("Ingresso negato: Parcheggio Pieno!\r\n"); //stampo l'avviso
            *avviso_stampato = true;
        }
    } 
    // se non ci sono più macchine ne in ingresso ne in uscita (l'auto se ne è andata)
    else if (!ostacolo_in && !ostacolo_out) {
        if(*avviso_stampato == true){
            // Ritardo per evitare 'sfarfallio' dei sensori
            _delay_ms(500);
            
            //Rileggo i sensori per sicurezza
            if(!rilevato_ostacolo(sensore_ingresso) && !rilevato_ostacolo(sensore_uscita)){
                char buffer[30];
                sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi); //stampo il numero di posti liberi
                uart_print(buffer);
                *avviso_stampato = false;
            }
        }
    }
}





// Funzione per la stampa in caso di parcheggio vuoto
void avviso_parcheggio_vuoto(bool ostacolo_in, bool ostacolo_out, bool *avviso_stampato) {
    // se c'è un'auto in uscita ma il parcheggio è vuoto
    if(!ostacolo_in && ostacolo_out){
        if(*avviso_stampato == false){
            uart_print("Errore: Il parcheggio e' gia' vuoto!\r\n"); // stampo l'errore
            *avviso_stampato = true;
        }
    } 
    // se non ci sono più macchine (l'auto in uscita ha fatto retromarcia e se n'è andata)
    else if (!ostacolo_in && !ostacolo_out) {
        if(*avviso_stampato == true){
            // Ritardo per evitare 'sfarfallio' dei sensori
            _delay_ms(500);
            
            // Rileggo i sensori per sicurezza
            if(!rilevato_ostacolo(sensore_ingresso) && !rilevato_ostacolo(sensore_uscita)){
                char buffer[30];
                sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi); // ristampo i posti
                uart_print(buffer);
                *avviso_stampato = false;
            }
        }
    }
}





// Funzione per simulare l'ingresso e l'uscita da tastiera
void gestisci_uart_manuale(void) {
    // Controllo se è arrivato un carattere via interrupt
    if (uart_available() > 0) {
        unsigned char comando = uart_read();  //se c'è almeno un carattere, lo estraiamo dal buffer e lo salviamo 

        // Simulo l'ingresso (comando 'i')
        if (comando == 'i') {
            //se ci sono posti liberi
            if (posti_liberi > 0) {
                posti_liberi--; // Scalo un posto
                uart_print("Auto entrata ---\r\n");
                
                //apro e chiudo la sbarra
                open_barrier();
                led_on(led_verde);
                led_off(led_rosso);
                _delay_ms(2000); // Aspetto per simulare il passaggio
                close_barrier();
                led_on(led_rosso);
                led_off(led_verde);

                // stampo i posti aggiornati
                char buffer[30];
                sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi);
                uart_print(buffer);
            } else {
                uart_print("Parcheggio Pieno! Impossibile entrare ---\r\n");
                
                // stampa dei posti
                _delay_ms(1500); 
                char buffer[30];
                sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi);
                uart_print(buffer);
            }
        } 
        // Simulo uscita(comando 'u')
        else if (comando == 'u') {
            //se i posti liberi sono minori del massimo dei posti(se ci sta almeno un'auto dentro)
            if (posti_liberi < MAX_POSTI) {
                posti_liberi++; // Aggiungo un posto
                uart_print("Auto uscita ---\r\n");

                //apro e chiudo la sbarra
                open_barrier();
                led_on(led_verde);
                led_off(led_rosso);
                _delay_ms(1500); // Aspetto un po' per simulare il passaggio
                close_barrier();
                led_on(led_rosso);
                led_off(led_verde);

                // stampo i posti aggiornati
                char buffer[30];
                sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi);
                uart_print(buffer);
            } else {
                uart_print("Errore, il parcheggio e' gia' vuoto! ---\r\n");
                
                // stampa dei posti
                _delay_ms(1500); 
                char buffer[30];
                sprintf(buffer, "Posti liberi: %d\r\n", posti_liberi);
                uart_print(buffer);
            }
        }
    }
}


 

       

int main(void){

    uart_init(9600); // inizializza la seriale a 9600 baud
    tastiera_init();//inizializza la tastiera
    led_init(led_rosso,led_verde); // inizializzo i led
    sensore_init(sensore_ingresso,sensore_uscita); // inizializzo i sensori
    servo_init(servo); // inizializzo il servo
    uart_print("Sistema Parcheggio Avviato\n");
    uart_print("Premi 'i' per simulare un ingresso, 'u' per un'uscita.\n");

    // stato iniziale
    led_on(led_rosso);
    led_off(led_verde);

    //stampo inizialmente posti liberi
    char buffer_1[30];
    sprintf(buffer_1, "Posti liberi: %d\r\n", posti_liberi);
    uart_print(buffer_1);

    bool avviso_stampato = false;
    bool avviso_vuoto_stampato = false; // Flag per l'avviso di parcheggio vuoto 

    while(1){
        gestisci_uart_manuale();  //chiamo la gestione attraverso UART
        
        // leggo lo stato dei sensori
        bool ostacolo_in = rilevato_ostacolo(sensore_ingresso);
        bool ostacolo_out = rilevato_ostacolo(sensore_uscita);

        // Se i sensori sono attivi insieme (le auto sono arrivate assieme)
        if(ostacolo_in && ostacolo_out){      
            led_off(led_verde);
            
            //lampeggio per allarme
            led_on(led_rosso);
            _delay_ms(200);
            led_off(led_rosso);
            _delay_ms(200);
        }

        // se non ci sono posti liberi
        else if(posti_liberi <= 0){
            led_on(led_rosso);
            led_off(led_verde);
            avviso_parcheggio_pieno(ostacolo_in, ostacolo_out, &avviso_stampato); // stampo l'avviso di parcheggio pieno
            
            //permetto solo l'uscita
            if(!ostacolo_in && ostacolo_out){
                _delay_ms(300);
                //ricontrollo dopo pausa per sicurezza
                if(!rilevato_ostacolo(sensore_ingresso)){ 
                    gestisci_transito(sensore_uscita, sensore_ingresso, +1);  //funzione che gestisce il transito dell'auto
                    avviso_stampato = false; 
                }
            }
        }

        //se si può sia entrare che uscire
        else {
            // se nessuna auto è presente sui sensori
            if(!ostacolo_in && !ostacolo_out){
                led_on(led_rosso);
                led_off(led_verde);
                avviso_parcheggio_vuoto(ostacolo_in, ostacolo_out, &avviso_vuoto_stampato); // Resetta l'avviso di parcheggio vuoto se la macchina si è spostata
            }
            
            //se ho ostacolo in entrata
            else if(ostacolo_in && !ostacolo_out){
                _delay_ms(200);
                if(rilevato_ostacolo(sensore_ingresso)) {
                    uart_print("Auto al varco. Inserire codice di 3 cifre:\r\n");
                    uint8_t pin_inserito[3];
                    uint8_t indice_pin = 0;
                    
                    //aspetto finché non riceve 3 tasti
                    while(indice_pin < 3) {
                        
                        // se l'auto fa retromarcia
                        if(!rilevato_ostacolo(sensore_ingresso)){
                            _delay_ms(500); 
                            if(!rilevato_ostacolo(sensore_ingresso)){
                                uart_print("Auto andata via. Inserimento annullato.\r\n");
                                
                                // ristampo i posti liberi
                                char buf_posti[30];
                                sprintf(buf_posti, "Posti liberi: %d\r\n", posti_liberi);
                                uart_print(buf_posti);
                                break;
                            }
                        }

                        // Leggiamo la tastiera 
                        KeyEvent events[MAX_EVENTS];
                        uint8_t num_events = keyScan(events);
                        
                        for (uint8_t k = 0; k < num_events; ++k){
                            if(events[k].status == 1) { 
                                pin_inserito[indice_pin] = events[k].key;
                                indice_pin++;
                                uart_print("*"); //asterisco per pin
                            }
                        }
                    } 
                    
                    
                    // Se ha digitato tre cifre giuste
                    if(indice_pin == 3){
                        uart_print("\r\n"); 
                        char buf_codice[50];
                        sprintf(buf_codice, "codice inserito:    %d%d%d\r\n", pin_inserito[0], pin_inserito[1], pin_inserito[2]);
                        uart_print(buf_codice);
                        
                        // controllo il codice
                        if(pin_inserito[0] == codice_segreto[0] &&
                           pin_inserito[1] == codice_segreto[1] &&
                           pin_inserito[2] == codice_segreto[2]) {
                            
                            uart_print("codice esatto\r\n");
                            
                            // se non ci sono ostacoli in uscita, apriamo la sbarra
                            if(!rilevato_ostacolo(sensore_uscita)){ 
                                gestisci_transito(sensore_ingresso, sensore_uscita, -1);
                                avviso_vuoto_stampato = false; 
                            }
                        } 
                        else {
                            uart_print("codice errato\r\n");
                            
                            // allarme PIN errato: triplo lampeggio rosso rapido
                            led_off(led_verde);
                            led_off(led_rosso);
                            _delay_ms(200); led_on(led_rosso);
                            _delay_ms(200); led_off(led_rosso);
                            _delay_ms(200); led_on(led_rosso);
                            
                            _delay_ms(1500); 
                            
                            char buf_posti[30];
                            sprintf(buf_posti, "Posti liberi: %d\r\n", posti_liberi);
                            uart_print(buf_posti);
                        }
                    }
                }
            }
            

            //se ho un'auto in uscita
            else if(!ostacolo_in && ostacolo_out){
                //se ci sta almeno un'auto dentro
                if(posti_liberi < MAX_POSTI){
                    // aspetto prima di aprire
                    _delay_ms(300);
                    // Se dopo 300ms l'altro sensore è libero, apro la sbarra
                    if(!rilevato_ostacolo(sensore_ingresso)){
                        gestisci_transito(sensore_uscita, sensore_ingresso, +1);  //chiamo la funzione che gestisce il transito e incrementa il numero di posti
                        avviso_stampato = false; 
                    }
                //se il parcheggio dentro è vuoto
                } else {
                    led_on(led_rosso);
                    led_off(led_verde);
                    
                    // Richiama la funzione per gestire l'errore di parcheggio vuoto
                    avviso_parcheggio_vuoto(ostacolo_in, ostacolo_out, &avviso_vuoto_stampato);
                }
            }
        }
    }
}