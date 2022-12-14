#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h> 
#include <avr/interrupt.h>
#include <avr/io.h>

#define LED_BUILT_IN 13

int ledontime = 200;    // default led on time
int ledofftime = 800;   // default led off time

String command;
QueueHandle_t queue;
SemaphoreHandle_t xSerialSemaphore;

/*#if DEBUG == 0
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif*/

void DefaultTask(void *pvParameters);  // 1.Task
void DegiskenTask(void *pvParameters); // 2.Task

void setup()
{

  // Seriali ac
  Serial.begin(115200); // Enable serial communication library.
  Serial.println("Kurulum Tamamlandi");

  while (!Serial)
  {
    ; 
  }
  //
  queue = xQueueCreate(10, sizeof(char));
  if (queue == NULL)
  {
  }

  if (xSerialSemaphore == NULL) 
  {
    xSerialSemaphore = xSemaphoreCreateMutex(); 
    if ((xSerialSemaphore) != NULL)
      xSemaphoreGive((xSerialSemaphore)); 
  }

  // Gorevler yaratiliyor.
  xTaskCreate(DefaultTask, "DefaultTask", 128, NULL, 2, NULL);
  xTaskCreate(DegiskenTask, "DegiskenTask", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop()
{
  // bu kisma birsey yapmiyoruz...
}

//---------------------- TASKS ---------------------//

void DefaultTask(void *pvParameters) // 
{
  (void)pvParameters;

  pinMode(LED_BUILT_IN, OUTPUT);

  while (1)
  {

    Serial.println(F("DefaulTask"));

    digitalWrite(LED_BUILT_IN, HIGH);            //
    vTaskDelay(ledontime / portTICK_PERIOD_MS);  // 
    digitalWrite(LED_BUILT_IN, LOW);             // 
    vTaskDelay(ledofftime / portTICK_PERIOD_MS); // 

    vTaskDelay(1);
  }
}
void DegiskenTask(void *pvParameters)
{
  (void)pvParameters;
  // Serial.begin(115200);

  while (1)
  {
    if (Serial.available())
    {

      Serial.println(F("DegiskenTask"));


      command = Serial.readStringUntil('\n');  
      command.trim();                          // bo??luklari temizle
      int commandLength;                       // gelen komutun uzunlu??unu al??yorum d??ng?? i??in
      commandLength = command.length();        // bu de??i??kene e??itliyorum
      bool inEqual;
      String metod;
      int data;                       // e??ittir var m?? yok mu adl?? de??i??ken
      for (int i = 0; i < commandLength; i++) // komutun i??inde "=" var m?? kontrol edece??im
      {
        if (command[i] == '=')
        {
          inEqual = true;
          break;
        }
        else
        {
          inEqual = false;
        }
      } // bu d??ng?? ile e??er komutun i??inde "=" var ise kurgumu ona g??re y??nlendirece??im.

      if (inEqual == false) // bu komutun i??inde "=" yok ise
      {
        if (command.equals("stop")) // gelen emir stop ise
        {
          ledontime = 1000;
          ledofftime = 1000;
        }
        else if (command.equals("start")) // gelen emir start ise
        {
          ledontime = 200;
          ledofftime = 800;
        }
        else
        {
          Serial.println("yanlis komut");
        }
      }
      else // komutun i??inde "=" var ise
      {
        String ilk="";
        String ikinci ="";

        int esittirKacinci;
        esittirKacinci = command.indexOf("=");
        for(int i=0; i<esittirKacinci;i++)
        {
          ilk += command[i];
        }
        for (int i = esittirKacinci+1 ; i < commandLength; i++)
        {
          ikinci += command[i];
        }
        metod = ilk;
        data = ikinci.toInt();

        if (metod=="ledon")
        {
          Serial.println(ilk);
          Serial.println(ikinci);
          ledontime =data;
          
        }
        else if (metod=="ledoff")
        {
          
          Serial.println(ikinci);
          ledofftime = data;  
        }
         else 
        {
        Serial.println("yanlis komut");
        }
      }
      
      Serial.print("Komut: ");
      Serial.println(command);
    }

    vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

// 
/*ISR(USART_RX_vect)
{
  temp=UDR0;// read the received data byte in temp
}*/


// bu metod ile komutun i??inde "=" var m?? yok mu ona bak??yorum.
bool komutKontrol(String komut){
  bool inEqual;
  for (int i = 0; i < komut.length(); i++) // komutun i??inde "=" var m?? kontrol edece??im
      {
        if (komut[i] == '=')
        {
          inEqual = true;
          break;
        }
        else
        {
          inEqual = false;
        }
      }
    return inEqual;
}




//////////////////////////
