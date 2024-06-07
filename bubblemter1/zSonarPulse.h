//zf240525.1038

const int zSonarPulseOn = 50;    // délai pour sonarPulse
const int zSonarPulseOff = 100;    // délai pour sonarPulse
const int zSonarPulseWait = 500;    // délai pour sonarPulse
byte zSonarPulseState = 1;    // état pour sonarPulse
long zSonarPulseNextMillis = 0;    // état pour sonarPulse


// Machine à état pour faire pulser deux fois la petite LED sans bloquer le système
void sonarPulse(){
  if (zSonarPulseNextMillis < millis()){
    switch (zSonarPulseState){
      // 1ère pulse allumée que l'on doit éteindre !
      case 1:
        digitalWrite(ledPin, HIGH);
        zSonarPulseNextMillis = millis() + zSonarPulseOff;
        zSonarPulseState = 2;
        break;
      // 1ère pulse éteinte que l'on doit allumer !
      case 2:
        digitalWrite(ledPin, LOW);
        zSonarPulseNextMillis = millis() + zSonarPulseOn;
        zSonarPulseState = 3;
        break;
      // 2e pulse allumée que l'on doit éteindre et attendre le wait !
      case 3:
        digitalWrite(ledPin, HIGH);
        zSonarPulseNextMillis = millis() + zSonarPulseWait;
        zSonarPulseState = 4;
        break;
      // 2e pulse éteinte pendant le wait que l'on doit allumer !
      case 4:
        digitalWrite(ledPin, LOW);
        zSonarPulseNextMillis = millis() + zSonarPulseOn;
        zSonarPulseState = 1;
        break;
    }
  }
}

