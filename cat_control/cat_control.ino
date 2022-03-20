const int PIN_AIN1 = 5;
const int PIN_AIN2 = 6;
const int PIN_PWMA = 10;

const int PIN_POS0 = A0;
const int PIN_POS1 = A1;
const int PIN_POS2 = A2;
const int PIN_POS3 = A3;

const int CAT_SPEED = 250;

int catPoses[] = {
  0b0000,
  0b0001,
  0b1001,
  0b1101,
  0b0101,
  0b0100,
  0b1100,
  0b1000,
};

bool moving = false;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);

  pinMode(PIN_POS0, OUTPUT);
  pinMode(PIN_POS1, INPUT_PULLUP);
  pinMode(PIN_POS2, INPUT_PULLUP);
  pinMode(PIN_POS3, INPUT_PULLUP);
}

void startMoving() {
  analogWrite(PIN_PWMA, CAT_SPEED);
  digitalWrite(PIN_AIN1, HIGH);
  moving = true;
}

void stopMoving() {
  analogWrite(PIN_PWMA, 0);
  digitalWrite(PIN_AIN1, LOW);
  moving = false;
}

int readCurrentPose() {
  int pos = 0;

  pos |= digitalRead(PIN_POS1);
  pos |= digitalRead(PIN_POS2) << 2;
  pos |= digitalRead(PIN_POS3) << 3;

  return pos;
}

bool validPose(int pose) {
  for (int i = 0; i < sizeof(catPoses) / sizeof(int); i++) {
    if (catPoses[i] == pose) {
      return true;
    }
  }

  return false;
}

void loop() {
  static int lastPose = -1;
  static int targetPose = -1;

  if (Serial.available()) {
    int candidatePose = Serial.parseInt();
    
    if (validPose(candidatePose)) {
      targetPose = candidatePose;
    } else {
      Serial.println("Unrecognized pose, stopping");
      stopMoving();
    }
  }

  int currentPose = readCurrentPose();

  if (!moving && targetPose != -1 && targetPose != currentPose) {
    startMoving();
    Serial.println("Moving to pose");
  }

  if (currentPose != lastPose) {
    if (currentPose == targetPose) {
      stopMoving();
      Serial.println("Reached pose");
    }
  }

  lastPose = currentPose;

  delay(1);
}
