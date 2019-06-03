#include <Arduino.h>

struct HomeComponent {
    const char *room;
    const char *component;
    bool state;
    uint8_t pin;
};

HomeComponent homeComponent[] = {
        {"sdb", "shutter", false, 22},
        {"ext", "door",   true,  24},
        {"ext", "light",  true,  26}, 
        {"ext", "garage", false, 28},
        {"sdb", "light", false, 30},
        {"sdb", "radiator", true, 32},
        {"cuisine", "shutter", false, 34},
        {"cuisine", "light", true, 36},
        {"cuisine", "dishwasher", false, 38},
        {"cuisine", "washingMachine", true, 40},
        {"salon", "computer", true, 42},
        {"salon", "printer", true, 44},
        {"salon", "light", true, 46},
        {"chambre", "light", true, 48},
        {"salon", "tv", false, 50},
        {"chambre", "shutter", false, 52},
        {"salon", "shutter", false, 51}
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

void sendState(int componentIndex) {

    Serial.print("state,");
    Serial.print(homeComponent[componentIndex].room);
    Serial.print(',');
    Serial.print(homeComponent[componentIndex].component);
    Serial.print(',');
    Serial.println(homeComponent[componentIndex].state);

}
void sendStates() {

    int homeComponentLength = getHomeComponentLength();

    for(int i = 0; i < homeComponentLength; i++){

        sendState(i);

    }

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

    sendState(indexFound);

}



void readSrvCommand() {

    logArduino("readSrvCommand");

    //int size = Serial.available();
    const int maxCommandLength = 100; // max command length

    char *tab = (char *) malloc(maxCommandLength * sizeof(char));
    int i = 0;
    bool state;
    char c;

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

        sendStates();

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
    sendStates();
}

void loop() {

    if (Serial.available() > 0) {
        readSrvCommand();
    }

}
