// ------HYBRID ROCKET ENGINE 2017 | 2018------
// ----------Engine Startup Sequence-----------
// --------------------------------------------
//#include <Servo.h>
#define MAX_TIMERS 8

// NOTE: Relays are set up in reversed polarity
// Therefore: HIGH corresponds to OFF and LOW corresponds to ON

// Define Variables
const int PRESSURE_SENSOR = A0;   // initialize the pressure transducer to Pin A0 on Arduino board
const int VALVE_B = 7;            // initialize the Helium valve to Pin 7 on Arduino board
const int VALVE_A = 5;            // initialize the Engine valve signal to Pin 5 on Arduino board
const int FIRE_COMMIT = 2;              // initialize the start signal to Pin 2 on Arduino board
const int IGNITER = 3;            // initialize the ignition signal to Pin 3 on Arduino board
const int bVPin = 4;
const int bVClose = 0;
const int bVOpen = 120;

const double COUNTDOWN_LENGTH = 50; // seconds
const double BNOS_T = COUNTDOWN_LENGTH - 30.0; // T minus 30 seconds: pressurize NOS  (ORIGINALLY 10)
const double ENOS_T = COUNTDOWN_LENGTH - 20.0; // T minus 20 seconds: end pressurize NOS (ORIGINALLY 5)
const double BOXY_T = COUNTDOWN_LENGTH - 1.0; // T minus 2 seconds: oxidizer start
const double BIGN_T = COUNTDOWN_LENGTH - 0; // T minus 0 seconds: ignition
const double ETST_T = COUNTDOWN_LENGTH + 5; // T plus 10 seconds: end of test

class Timer{
  private:
    double final_time;
    double current_time = 0.0f;
    double dt = 0.001f;
    void (*callback)();
    int timerlist_addr = -1;

  public:
    Timer();
    Timer(double time, void (*function)());
    double time_remaining();
    double time_elapsed();
    void start_timer();
    void pause_timer();
    void reset_timer();
    void tick();
    String report();
};

Timer bnos = Timer(BNOS_T, &pressurize_nos);
Timer enos = Timer(ENOS_T, &close_helium);
Timer boxy = Timer(BOXY_T, &start_oxidizer);
Timer bign = Timer(BIGN_T, &ignition);
Timer etst = Timer(ETST_T, &end_test);

Timer *timerlist[MAX_TIMERS] = {NULL};
int num_timers = 0;

//Servo ballValveServo;

void setup() {

  Serial.begin(115200);         // initialize the pressure transducer

//  ballValveServo.attach(bVPin);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(FIRE_COMMIT, INPUT);      // START is an input signal that begins the code
  pinMode(IGNITER, OUTPUT);   // IGNITER is an output that sends the ignition signal
  pinMode(VALVE_A, OUTPUT);   // VALVE_A is an output signal to open the Engine valve
  pinMode(VALVE_B, OUTPUT);   // VALVE_B is an output signal to open the Helium valve

  digitalWrite(VALVE_A,HIGH);                   // initialize engine valve to OFF
  digitalWrite(VALVE_B,HIGH);                   // initialize the helium valve to OFF
  digitalWrite(IGNITER,HIGH);                   // initialize the igniter to OFF

  //Serial.println("Starting Rocket Controller!");

  noInterrupts();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  // set compare match register for 1hz increments
  OCR1A = 15999;// = (16*10^6) / (1*1024) - 1 (must be <65536)

  interrupts();//allow interrupts

  Serial.println("Starting Rocket Controller!");
}

ISR(TIMER1_COMPA_vect) {
  for (int i = 0; i < MAX_TIMERS; i++) {
    if (timerlist[i]) {
      timerlist[i]->tick();
    }
  }
//  Serial.flush();
}

bool valveA = false;
bool valveB = false;
bool igniter = false;

// Engine Sequence
void loop() {
  if (Serial.available()) {
    char command[100];
  //  command[0] = '\0';

//    Serial.setTimeout(10);
    int len = Serial.readBytes(command, 100);
  //  Serial.println(len);
  //  Serial.println(command);

    command[len] = '\0'; // terminates the string "command"
    if (String(command) == "reset") {
      reset_system();
    } else if (String(command) == "purge") {
      purge_system();
    } else if (String(command) == "open A") {
      open_A();
    } else if (String(command) == "close A") {
      close_A();
    } else if (String(command) == "B") {
      toggle_B();
    } else if (String(command) == "status") {
      print_status();
    } else if (String(command) == "start") {
      start();
    } else if (String(command) == "h" || String(command) == "help") {
      Serial.println("Rocket Controller Help");
      Serial.println("Recognized Commands:");
      Serial.println("\"reset\": Return system to default");
      Serial.println("\"purge\": Open all valves");
      Serial.println("\"A\": Toggle Valve A only");
      Serial.println("\"B\": Toggle Valve B only");
      Serial.println("\"status\": Report system status");
      Serial.println("\"start\": Wait for start signal, then start countdown");
    }
    else {
      Serial.print("> "); Serial.println(command);
      Serial.println("Not a recognized command!");
    }

  }
  // NOTE: Code is waiting for start signal at this stage
//  while(STATUS == 0) {
//
//    STATUS = digitalRead(START);                  // redefine status after start signal
//    CBE_PRESSURE = 5*(analogRead(PRESSURE_SENSOR)/1023);   // read values from pressure transducer
//    Serial.println(CBE_PRESSURE);                 // print values to serial monitor (Ctrl + Shift + M)
//    delay(100);                                   // delay 0.1 seconds
//  }                                       // exit main loop and end code
}

void reset_system() {
  digitalWrite(VALVE_A,HIGH);
  close_A();

  digitalWrite(VALVE_B,HIGH);
  Serial.println("> reset");

  valveA = false;
  valveB = false;
}

void purge_system() {
  digitalWrite(VALVE_A,LOW);
  open_A();
  digitalWrite(VALVE_B,LOW);

  Serial.println("> purge");

  valveA = true;
  valveB = true;
}

// void toggle_A() {
//   Serial.println("> A");
//
//   if (valveA) digitalWrite(VALVE_A,HIGH); else digitalWrite(VALVE_A,LOW);
//   valveA = !valveA;
// }

void open_A() {
  analogWrite(bVPin,255);
  valveA = true;
}

void close_A() {
  analogWrite(bVPin,0);
  valveA = false;
}

void toggle_B() {
  Serial.println("> B");

  if (valveB) digitalWrite(VALVE_B,HIGH); else digitalWrite(VALVE_B,LOW);
  valveB = !valveB;
}

void print_status() {
  Serial.println("> status");
  String a = valveA ? "OPEN": "CLOSED";
  String b = valveB ? "OPEN": "CLOSED";
  Serial.print("Valve A is: "); Serial.println(a);
  Serial.print("Valve B is: "); Serial.println(b);
}

bool countdown_GO = true;
bool test_finished = false;

void start() {
  Serial.println("> start");
  Serial.print(">");
  reset_system(); // close all valves
  countdown_GO = true;
  test_finished = false;

  while (digitalRead(FIRE_COMMIT)) {
    if (Serial.available()) {
      char command[100];
      int len = Serial.readBytes(command, 100);
      command[len] = '\0'; // terminates the string "command"
      if (String(command) == "abort" || String(command) == "a") {
        return;
      }
    }
    Serial.println("Firing Commit Switch is ON! Turn it OFF to start countdown or type 'abort'.");
  }

  Serial.println("STARTING COUNTDOWN");

  bnos.reset_timer();
  enos.reset_timer();
  boxy.reset_timer();
  bign.reset_timer();
  etst.reset_timer();

  noInterrupts();
  Serial.println("Start BNOS");
  bnos.start_timer();
  Serial.println("Start ENOS");
  enos.start_timer();
  Serial.println("Start BOXY");
  boxy.start_timer();
  Serial.println("Start BIGN");
  bign.start_timer();
  Serial.println("Start ETST");
  etst.start_timer();
  interrupts();


  int mt = -30;
  while (!test_finished) {
    // listen asynchronously for an abort signal
    if (Serial.available()) {
      char command[100];
      int len = Serial.readBytes(command, 100);
      command[len] = '\0'; // terminates the string "command"
      if (String(command) == "abort" || String(command) == "a") {
        abort_test();
        return;
      }
    }
    if (!countdown_GO) {
      Serial.println("Countdown not GO while waiting");
      abort_test();
      return;
    }
    if (mission_time() > mt + 1) {
      Serial.print("MISSION ELAPSED TIME: "); Serial.println(++mt);
      String a = valveA ? "OPEN": "CLOSED";
      String b = valveB ? "OPEN": "CLOSED";
      String g = igniter ? "ON": "OFF";
      Serial.print("Valve A is: "); Serial.println(a);
      Serial.print("Valve B is: "); Serial.println(b);
      Serial.print("Igniter is: "); Serial.println(g);
      Serial.print("Pressure is: "); Serial.println(analogRead(PRESSURE_SENSOR));

    }
  }
}

double mission_time() {
  return ETST_T - etst.time_remaining() - COUNTDOWN_LENGTH;
}

void pressurize_nos() {
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("BEGIN NOS PRESSURIZATION");
  Serial.println("================================================================");
  if (countdown_GO )
  {
    digitalWrite(VALVE_B,LOW);
    valveB = true;
  }
  else {
    Serial.println("Countdown not GO at BEGIN NOS PRESSURIZATION");
    abort_test();
  }
}
void close_helium() {
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("END NOS PRESSURIZATION");
  Serial.println("================================================================");
  if (countdown_GO )
  {
    digitalWrite(VALVE_B,HIGH);
    valveB = false;
  }
  else {
    Serial.println("Countdown not GO at END NOS PRESSURIZATION");
    abort_test();
  }
}
void start_oxidizer() {
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("START OXIDIZER FLOW");
  Serial.println("================================================================");
  countdown_GO = digitalRead(FIRE_COMMIT);
  if (countdown_GO )
  {
    digitalWrite(VALVE_A,LOW);
    valveA = true;
    open_A();
  }
  else {
    Serial.println("Countdown not GO at START OXIDIZER FLOW");
    abort_test();
  }
}
void ignition () {
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("IGNITION");
  Serial.println("================================================================");
  countdown_GO = digitalRead(FIRE_COMMIT);
  if (countdown_GO )
  {
    digitalWrite(IGNITER,LOW);
    igniter = true;
  }
  else {
    Serial.println("Countdown not GO at IGNITION");
    abort_test();
  }
}
void end_test() {
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("END TEST");
  Serial.println("================================================================");
  close_A();
  valveA = false;
  digitalWrite(IGNITER,HIGH);
  igniter = false;
}

void abort_test() {
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("ABORT SIGNALED!");
  Serial.println("================================================================");
  noInterrupts();
  reset_system();
  digitalWrite(IGNITER,HIGH);
  igniter = false;
  bnos.pause_timer();
  enos.pause_timer();
  boxy.pause_timer();
  bign.pause_timer();
  etst.pause_timer();
  interrupts();
}

Timer::Timer(double time, void (*function)()) {
  final_time = time;
  callback = function;
}
Timer::Timer() {
  final_time = 0.0f;
  callback = NULL;
}
double Timer::time_remaining() {
  double out = final_time - current_time;
  return out >= 0.0f ? out: 0.0f;
}
double Timer::time_elapsed() {
  return current_time;
}
void Timer::start_timer() {
//  Serial.println("Start Timer");
//  report();
  if (timerlist_addr < 0) {
    if (num_timers < 8) {
      timerlist[num_timers] = this;
      timerlist_addr = num_timers;
      num_timers++;
    }
    else Serial.println("ERROR: Attempted to create 9 timers");
  }

//  report();
//  Serial.println("Done Starting Timer\n");
}
void Timer::pause_timer() {
//  Serial.println("Pause Timer");
//  report();

  if (final_time > 0 && timerlist_addr >= 0) {
    for (timerlist_addr; timerlist_addr < num_timers - 2; timerlist_addr++) {
      timerlist[timerlist_addr] = timerlist[timerlist_addr + 1];
      timerlist[timerlist_addr]->timerlist_addr -= 1;
    }
    timerlist[timerlist_addr] = NULL;
    num_timers--;
    timerlist_addr = -1;
  }


//  report();
//  Serial.println("Done Pausing Timer\n");
}
void Timer::reset_timer() {
  current_time = 0.0f;
}
void Timer::tick() {
  current_time += dt;

  if ((current_time >= final_time) && (final_time > dt)) {
    this->pause_timer();
    this->reset_timer();
    callback();
  }
}
String Timer::report() {
  Serial.print("Current Time: "); Serial.println(current_time);
  Serial.print("Final Time: "); Serial.println(final_time);
  Serial.print("Timer List Address: "); Serial.println(timerlist_addr);
  Serial.print("Number of Timers: "); Serial.println(num_timers);
  return String(current_time);
}



// REMEMBER: Restart the Arduino board after every use
