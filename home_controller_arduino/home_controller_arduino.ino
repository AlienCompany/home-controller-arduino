#include <Arduino.h>

struct HomeComponent {
    const char *room;
    const char *component;
    bool state;
    uint8_t pin;
};

HomeComponent homeComponent[] = {
        {"ext", "light",  true,  22},
        {"ext", "garage", false, 24},
        {"ext", "door",   true,  26},
        {"chambre", "light", true, 28},
        {"chambre", "shutter", false, 30},
        {"salon", "computer", true, 32},
        {"salon", "light", true, 34},
        {"salon", "shutter", false, 36},
        {"salon", "tv", false, 38},
        {"salon", "printer", true, 40},
        {"cuisine", "light", true, 42},
        {"cuisine", "shutter", false, 44},
        {"cuisine", "dishwasher", false, 46},
        {"cuisine", "washing machine", true, 48},
        {"sdb", "light", false, 50},
        {"sdb", "shutter", false, 52},
        {"sdb", "radiator", true, 51}
};

void logArduino(const char *msg) {
    Serial.print("log,");
    Serial.println(msg);
}

void logArduino(const int msg) {
    Serial.print("log,");
    Serial.println(msg);
}

void logArduino(const double msg) {
    Serial.print("log,");
    Serial.println(msg);
}

int getHomeComponentLength() {
    return sizeof(homeComponent) / sizeof(HomeComponent);
}

void changeStateComponent(char *room, char *component, bool state) {

    int homeComponentLength = getHomeComponentLength();
    int indexFound = -1;

    for (int i = 0; i < homeComponentLength; i++) {

        if (strcmp(homeComponent[i].room, room) == 0) {

            if (strcmp(homeComponent[i].component, component) == 0) {

                indexFound = i;
                break;

            }
        }
    }

    if (indexFound == -1) {
        return;
    }

    if (state) {
        digitalWrite(homeComponent[indexFound].pin, HIGH);
        homeComponent[indexFound].state = true;
    } else {
        digitalWrite(homeComponent[indexFound].pin, LOW);
        homeComponent[indexFound].state = false;
    }


}

void readSrvCommand() {

    logArduino("readSrvCommand");

    //int size = Serial.available();
    const int maxCommandLength = 100; // max command length

    char *tab = (char *) malloc(maxCommandLength * sizeof(char));
    int i = 0;
    bool state;
    char c;

    int homeComponentLength = getHomeComponentLength();

    do {
        if(i >= maxCommandLength){
            Serial.flush();
            logArduino("ERROR command too big!");
            return;
        }
        while (!Serial.available()){
            delay(1);
        };
        c = (char) Serial.read(); //cast serial.read() en char
        tab[i] = c;
        i++;
    } while (c != '\n');

    tab[i - 1] = '\0';

    logArduino(tab);
    //logArduino(size);

    char *command = tab;

    char *params = strchr(tab, ',');
    if(params != nullptr) {
        params[0] = '\0';
        params += sizeof(char);//Note personnelle: on avance d'une case dans le tableau
    }

    if(strcmp(command, "change") == 0){
        char *room = params;

        char *component = strchr(room, ',');
        component[0] = '\0';
        component += sizeof(char);

        char *stateStr = strchr(component, ',');
        stateStr[0] = '\0';
        stateStr += sizeof(char);

        state = stateStr[0] == 't';
        /* Note personnelle :  pareil que :
            if(stateStr[0] == 't'){
                state = true;
            }else{
                state = false;
            }
         */


        logArduino(room);
        logArduino(component);
        logArduino(state);

        changeStateComponent(room, component, state);
    }
    if(strcmp(command, "getState") == 0){

        for(i = 0; i < homeComponentLength; i++){

            Serial.print("state,");
            Serial.print(homeComponent[i].room);
            Serial.print(',');
            Serial.print(homeComponent[i].component);
            Serial.print(',');
            Serial.println(homeComponent[i].state);

        }

    }

    delete (tab);
}


void setup() {
    Serial.begin(115200);

    for (int i = 0; i < getHomeComponentLength(); i++) {
        pinMode(homeComponent[i].pin, OUTPUT);

        if(homeComponent[i].state){
            digitalWrite(homeComponent[i].pin, HIGH);
        }else{
            digitalWrite(homeComponent[i].pin, LOW);
        }
    }

    logArduino("arduinoInit");
}

void loop() {

    if (Serial.available() > 0) {
        readSrvCommand();
    }

}
