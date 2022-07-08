# by josetiagobsouza@gmail.com
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EasyBuzzer.h"
#include "Imagens.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <ESP32Time.h>
#include "AppConfig.h"
#include <EEPROM.h>

//--#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

//*Memoria Interna*//
#define EEPROM_SIZE 1

/*DEFINE*/
#define Password_Length 8
char *Dispositivo[] = { " DKH5-54H6-LHRF " };

/*configurações do menu*/

int ponteiroMenu = 0;
int ponteiroSubMenu = 0;
int isform = 0;
int subMenuAtivo = 0;
int SubMenuMax;
int ledDiplayOn = 1;
int statusGiroFlex = 0;
/*LCD*/
int signalPin = 12;
byte seta_esquerda[] = { 0x00, 0x02, 0x06, 0x0F, 0x1F, 0x0F, 0x06, 0x02 };
byte seta_direita[] = { 0x00, 0x08, 0x0C, 0x1E, 0x1F, 0x1E, 0x0C, 0x08 };
byte customA[] = { 0x00, 0x0C, 0x12, 0x12, 0x12, 0x1E, 0x12, 0x12 };
byte customB[] = { B00000, B01110, B01001, B01001, B01110, B01001, B01001,
B01110 };
byte check[] =
		{ B00000, B00001, B00011, B10110, B11100, B01000, B00000, B00000 };
byte seta_opcao[] = { 0x00, 0x00, 0x00, 0x1F, 0x11, 0x1F, 0x00, 0x00 };
byte seta_opcao2[] = { B00000, B00000, B00000, B11111, B11111, B11111, B00000,
B00000 };
byte customDoisPontos[] = { B00000, B00000, B01100, B01100, B00000, B01100,
B01100, B00000 };

/*SOM BUZZER*/
unsigned int frequency = 1000;
unsigned int duration = 10;
const int pinoBuzzer = 32; //PINO DIGITAL UTILIZADO PELO BUZZER

/*DISPOSITIVOS CONECTADOS*/
const int pinoPeriferico = 33; //PINO DIGITAL UTILIZADO PELO GIROFLEX

//*TECLADO DE MEMBRADA DO DISPOSITIVO*/
char Data[Password_Length];
char Master[Password_Length] = "123456";

byte data_count = 0, data_count_Key = 0, master_count = 0;
bool Pass_is_good;
char keyPressed;

byte d1 = 15, d2 = 2, d3 = 4, d4 = 16, d5 = 17, d6 = 5, d7 = 18, d8 = 19;
const byte Linhas = 4; //4 Linhas
const byte Colunas = 4; //4 Colunas
char hexaKeys[Linhas][Colunas] = { { '1', '2', '3', 'A' },
		{ '4', '5', '6', 'B' }, { '7', '8', '9', 'C' }, { '*', '0', '#', 'D' } };
byte PinoLinha[Linhas] = { d1, d2, d3, d4 }; //linhas do teclado detalhe preto
byte PinoColuna[Colunas] = { d5, d6, d7, d8 }; //colunas do teclado detalhe branco
Keypad Teclado = Keypad(makeKeymap(hexaKeys), PinoLinha, PinoColuna, Linhas,
		Colunas);
/*FIM DO CÓDIGO TECLADO DE MEMBRADA*/

/*TECLADO*/
//#define COUNTDOWN_TIME 12000 //4hour esta em milisegundos
int countTime;

char szString[20];
byte mins, secs;

unsigned long timeTemp, timeNow, timeStart, timeElapsed, timeLeft;

/*DISPLAY*/

//LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x27, 16, 2);
Imagens img;

//PiscarLed led(15, 100, 400);

//ESP32Time rtc;

void setup() {
	Serial.begin(115200);
	pinMode(pinoBuzzer, OUTPUT); //DEFINE O PINO COMO SAÍDA
	lcd.init();
	lcd.backlight();
	pinMode(signalPin, OUTPUT);
	EasyBuzzer.setPin(pinoBuzzer);
	scanIC2();

	/*CRIA CARATER ESPECIAL*/

	lcd.createChar(0, seta_esquerda);
	lcd.createChar(1, seta_direita);
	lcd.createChar(2, customA);
	lcd.createChar(3, customB);
	lcd.createChar(4, seta_opcao);
	lcd.createChar(5, seta_opcao2);
	lcd.createChar(6, customDoisPontos);

	timeStart = millis();
	mins = 1;
	secs = 1;

	/*PINO DO GIRO FLEX*/
	pinMode(pinoPeriferico, OUTPUT);

	//EEPROM

	//ledcAttachPin(pinoBuzzer,0);



	// 0 => tempo do cronometro
	EEPROM.write(1, 0);
	EEPROM.commit();

	EEPROM.begin(EEPROM_SIZE);
	countTime =  3;
	if (countTime > 255) {
		countTime = 1;
	}
	//rtc.setTime(30, 43, 01, 26, 1, 1970);  // 26/01/2021 01:40

	//EasyBuzzer.beep(frequency, 1, doneBuzzer);
	//rtc.setTime(30, 43, 01, 26, 1, 1970);  // 26/01/2021 01:40
	/*if(rtc.getTime("%d/%m/%Y") == "01/01/2021"){
	 done(2, 1, 1000);
	 rtc.setTime(30, 01, 02, 26, 1, 2021);  // 26/01/2021 01:40
	 }*/

	/*WiFi.mode(WIFI_STA);
	 WiFiManager wm;
	 bool res;
	 res = wm.autoConnect(); // password protected ap

	 if (!res) {
	 Serial.println("Failed to connect");
	 // ESP.restart();
	 } else {
	 //if you get here you have connected to the WiFi
	 Serial.println("connected...yeey :)");
	 }*/

	//NTP.begin();
}

void loop() {



	//lcd.setCursor(0, 0);
	//lcd.print("GESTOR PRODUCAO");
	//tone(pinoBuzzer, 150, 10);

	keyPressed = Teclado.getKey();
	if(keyPressed == '1'){
		       if(countTime == 0){
		    	   clearLcd(1);
		    	   countTime = 3;
		    	   DoCountdown(3);


		       }else{
				countTime = 0;
				lcd.setCursor(0, 1);
				lcd.print("TAREFA CONCLUIDA");
		       };
			}
	else if (keyPressed == '#') {

		if (ponteiroMenu == 0) {

		}

		else if (ponteiroMenu == 3) {

			if (ponteiroSubMenu == 0) {
				SubMenu(3, 0, 0, "Dispositivo", 1);
				clearLcd(1);
				lcd.setCursor(0, 1);
				lcd.print(Dispositivo[0]);
			} else if (ponteiroSubMenu == 1) {

			} else if (ponteiroSubMenu == 2) {
				if (ledDiplayOn == 1) {
					ledDiplayOn = 0;
					lcd.noBacklight();
				} else {
					ledDiplayOn = 1;
					lcd.backlight();
				}

			} else if (ponteiroSubMenu == 3) {
				if(statusGiroFlex == 0){
					ligaGiroFlex();
					statusGiroFlex = 1;
				}else{
					desligaGiroFlex();
					statusGiroFlex = 0;
				}

			} else if (ponteiroSubMenu == 4) {
				ajuda();
				delay(2000);
				home();
			} else if (ponteiroSubMenu == 5) {
				lcd.clear();
				clearDataKey();
				SubMenu(3, 5, 0, "Salvar Tempo (min)", 1);
				SalvarInputKey(1);
			} else if (ponteiroSubMenu == 6) {
				SubMenu(3, 6, 0, "Data Atual", 1);
				time_t now;
				char strftime_buf[64];
				struct tm timeinfo;

				time(&now);
				// Set timezone to Brazil Standard Time
				//setenv("TZ", "UTC-3", 1);
				//tzset();

				localtime_r(&now, &timeinfo);
				strftime(strftime_buf, sizeof(strftime_buf), "%d/%m/%Y %T",
						&timeinfo);
				clearLcd(1);
				lcd.setCursor(0, 1);
				lcd.print(strftime_buf);
				//data_count = ponteiroSubMenu-1;

			} else if (ponteiroSubMenu == 7) {
				home();

			}
		}

	}

	else if ((keyPressed != NO_KEY) && (keyPressed != '#')
			&& (keyPressed != '*') && (keyPressed != 'D')) {

		switch (keyPressed) {
		case 'A':
		case 'B':
			SubMenuMax = 6;
			//data_count = ponteiroSubMenu;
			if (keyPressed == 'A') {
				Data[data_count] = keyPressed;
				data_count--;
			} else {
				Data[data_count] = keyPressed;
				data_count++;
			}

			if (subMenuAtivo == 1) {
				subMenuAtivo = 0;
				AppMenu(3, 0, 0, "CONFIGURAR");
			}

			if (ponteiroMenu == 3) {
				resetSubMenu(SubMenuMax);

				if (data_count == 0) {
					SubMenu(3, 0, 0, "Dispositivo", 0);
				} else if (data_count == 1) {
					SubMenu(3, 1, 0, "Rede", 0);
				} else if (data_count == 2) {
					if (ledDiplayOn == 1) {
						SubMenu(3, 2, 0, "Desligar LCD", 0);
					} else {
						SubMenu(3, 2, 0, "Ligar LCD", 0);
					}

				} else if (data_count == 3) {
					SubMenu(3, 3, 0, "GiroFlex", 0);

				} else if (data_count == 4) {
					SubMenu(3, 4, 0, "Ajuda", 0);
				} else if (data_count == 5) {
					SubMenu(3, 5, 0, "Tempo Producao", 0);
				} else if (data_count == 6) {
					SubMenu(3, 6, 0, "Data Atual", 0);
				} else if (data_count == 7) {
					SubMenu(3, 7, 0, "Sair", 0);
				}
			}

			break;

		case 'C':
			if (subMenuAtivo == 1) {
				subMenuAtivo = 0;
				AppMenu(3, 0, 0, "CONFIGURAR");
			}

			AppMenu(3, 0, 0, "CONFIGURAR");
			SubMenu(3, 0, 0, "Dispositivo", 0);

			break;

		default:
			if (isform == 1) {
				InputKey(1);
			}
		}

	} else if (keyPressed == 'D') {
		/*DELETA O QUE FOI DIGITADO*/
		InputKeyDelete();

	} else if (keyPressed == '*') {
		home();

	} else {
		if(countTime > 0){
			DoCountdown(countTime);
		}

	};

	/*if (data_count == Password_Length - 1) {
	 lcd.clear();

	 if (!strcmp(Data, Master)) {
	 lcd.print("CORRETO");
	 digitalWrite(signalPin, HIGH);
	 delay(5000);
	 digitalWrite(signalPin, LOW);
	 } else {
	 lcd.print(" NAO CADASTRADO!");
	 done(1, 1000, 1000);

	 }
	 lcd.clear();
	 clearData();

	 }*/
}
void scanIC2() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C encontrado no Endereco 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
  }

  delay(5000);
}

void done(unsigned int tipo, int tempo, int frequency) {
	/*1 = buzzer*/
	if (tipo == 1) {
		EasyBuzzer.beep(frequency, 1);
		delay(tempo);
		EasyBuzzer.stopBeep();
	} else if (tipo == 2) {
		EasyBuzzer.beep(2, 2);
		delay(500);
		EasyBuzzer.stopBeep();
	}

}

void playTone() {
//tone(buzzPin, 150, 10);
	EasyBuzzer.beep(1, 10);
	delay(20);
	EasyBuzzer.stopBeep();
}

void ligaGiroFlex() {
	digitalWrite(pinoPeriferico, HIGH);
}

void desligaGiroFlex() {
	digitalWrite(pinoPeriferico, LOW);
}

void clearData() {
	while (data_count != 0) {
		Data[data_count--] = 0;
	}
	return;
}

void clearDataKey() {
	while (data_count_Key != 0) {
		Data[data_count_Key--] = 0;
	}
	return;
}

void clearLcd(int linha) {
	lcd.setCursor(0, linha);
	lcd.print("                 ");
}

void AppMenu(int idmenu, int idsubmenu, int ponteiroisform, const char *txt) {
	done(1, 1, 1500);

	clearData();
	lcd.clear();

	ponteiroMenu = idmenu;
	ponteiroSubMenu = idsubmenu;
	isform = ponteiroisform;
	int cursor_final = count(txt);

	lcd.setCursor(0, 0);
	lcd.print(txt);
	lcd.setCursor(cursor_final, 0);
	lcd.write(6);

	lcd.setCursor(12, 0);
	lcd.write(0);
	lcd.setCursor(13, 0);
	lcd.write(2);

	lcd.setCursor(14, 0);
	lcd.write(3);
	lcd.setCursor(15, 0);
	lcd.write(1);

}

void SubMenu(int idmenu, int idsubmenu, int ponteiroisform, const char *txt,
		int ativo) {

	if (ativo > 0) {
		subMenuAtivo = ativo;
	};

	if (subMenuAtivo == 0) {
		clearLcd(1);
		done(1, 1, 1000);
		ponteiroMenu = idmenu;
		ponteiroSubMenu = idsubmenu;
		isform = ponteiroisform;
		subMenuAtivo = 0;
		//int cursor_final = count(txt);
		lcd.setCursor(0, 1);
		lcd.write(5);
		delay(500);
		lcd.setCursor(0, 1);
		lcd.write(4);

		lcd.setCursor(1, 1);
		lcd.write(1);
		lcd.setCursor(2, 1);
		lcd.print(" ");
		lcd.setCursor(3, 1);
		lcd.print(txt);
	} else {
		clearLcd(0);
		done(1, 1, 1000);
		ponteiroMenu = idmenu;
		ponteiroSubMenu = idsubmenu;
		isform = ponteiroisform;
		subMenuAtivo = 1;
		//int cursor_final = count(txt);

		lcd.setCursor(0, 0);
		lcd.print(txt);
		lcd.setCursor(14, 0);
		lcd.write(0);
		lcd.setCursor(15, 0);
		lcd.write(2);
	}
}

int count(const char *txt) {
	int count = 0;
	for (int i = 0; i < strlen(txt); i++) {
		if (txt[i] != ' ')
			count++;
	}

	return count;
}

void resetSubMenu(int maxSubMenu) {
	if (data_count > maxSubMenu) {
		data_count = 0;
	}

}

void home() {
	done(1, 1, 1000);
	clearData();
	lcd.clear();
	ponteiroMenu = 0;
	ponteiroSubMenu = 0;
	subMenuAtivo = 0;
	isform = 0;

	lcd.setCursor(0, 0);
	lcd.print("COPAC - SOLDA");
	lcd.setCursor(0, 1);
	lcd.print("        SOLDA 01");
}

void ajuda() {
	done(1, 1, 1000);
	clearData();
	lcd.clear();
	ponteiroMenu = 0;
	ponteiroSubMenu = 0;
	isform = 0;

	lcd.setCursor(0, 0);
	lcd.print("AJUDA?");
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("(#) = Confirmar");
	delay(3000);
	clearLcd(1);
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("(*) = Cancelar");
	delay(3000);
	clearLcd(1);
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("(A) = Proximo");
	delay(3000);
	clearLcd(1);
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("(B) = Anterior");
	delay(3000);
	clearLcd(1);
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("(D) = Deletar");
	delay(3000);
	clearLcd(1);
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("www.mavvi.com.br");
	delay(3000);
	clearLcd(1);
	done(1, 1, 1000);
	lcd.setCursor(0, 1);
	lcd.print("  versao 2.1.2");
	delay(3000);

	clearLcd(1);
	lcd.setCursor(0, 1);
	done(1, 1, 2000);
	lcd.print("By :), Obrigado!");
	delay(1000);
}

void InputKey(int ispform) {
	//data_count = 0;
	isform = ispform;
	if (keyPressed != '#') {
		playTone();
		//clearLcd(1);
		Data[data_count_Key] = keyPressed;
		Serial.println(Data);
		lcd.setCursor(data_count_Key, 1);
		lcd.print(Data[data_count_Key]);
		data_count_Key++;
		return;
	}

}

void SalvarInputKey(int ispform) {
	//data_count = 0;

	isform = ispform;
	if (keyPressed != '#') {
		playTone();
		//clearLcd(1);
		Data[data_count_Key] = keyPressed;
		int ia = (int)Data;
		//int someInt = strtoul(Data);
		Serial.println(Data);
		lcd.setCursor(data_count_Key, 1);
		lcd.print(Data[data_count_Key]);
		data_count_Key++;
		EEPROM.write(0, 10);
		EEPROM.commit();

		return;
	}

}

void InputKeyDelete() {
	if (isform == 1) {
		done(1, 1, 1000);
		//Data[data_count_Key] = keyPressed;
		//Serial.println(Data);
		if (data_count_Key > 0) {
			lcd.setCursor(data_count_Key - 1, 1);
			lcd.print("_");
			data_count_Key--;
		} else {
			clearData();
			//lcd.clear();
		}
	}
}

void setTime() {
	//rtc.setTime(30, 39, 01, 26, 1, 2021);  // 17th Jan 2021 15:24:30
}

void DoCountdown(int tempo) {

	//ponteiroMenu = idmenu;
	//ponteiroSubMenu = idsubmenu;
	//isform = ponteiroisform;

	int COUNTDOWN_TIME = tempo * 60000;
	static unsigned long lastTimeNow = 0;
	static byte lastsecs = 1;

	timeNow = millis();
	timeElapsed = timeNow - timeStart;

	if (mins == 0 && secs == 0)
		return;

	timeLeft = COUNTDOWN_TIME - timeElapsed;

	mins = (byte) (timeLeft / 60000ul);
	timeTemp = timeLeft - (mins * 60000);
	secs = (byte) (timeTemp / 1000ul);
	timeTemp = timeTemp - (secs * 1000ul);

	if (mins == 0 && secs == 0) {
		lcd.clear();
		sprintf(szString, "**FIM DA TAREFA**");
		lcd.setCursor(0, 0);
		lcd.print("TEMPO RESTANTE");
		lcd.setCursor(0, 1);
		lcd.print(szString);
		// Ativamos o pino 12 (colocando 5v nele)
		//digitalWrite(12, HIGH);

		//Serial.println(szString);
		// delay(1000);

	}       //if
	else if (secs != lastsecs) {
		lastsecs = secs;
		sprintf(szString, "%02d:%02d", mins, secs);
		//lcd.clear();
		if (ponteiroMenu == 0) {
			lcd.setCursor(0, 0);
			lcd.print("TEMPO RESTANTE");
			done(1, 1, 1000);
			lcd.setCursor(0, 1);
			lcd.print(szString);
		}

		//delay(3000);
		//clearLcd(1);
		//Serial.println(szString);

	}       //if

}

