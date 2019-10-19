// Simon Game by Mike McRoberts
// October 2019

#define  NOTE_G3     2550    // 392 Hz
#define  NOTE_A3     2272    // 440 Hz
#define  NOTE_B3     2028    // 493 Hz
#define  NOTE_C4     1912    // 523 Hz

const int speakerOut = 11; // define the speaker pin

int notes[] = { NOTE_G3, NOTE_A3,  NOTE_B3, NOTE_C4}; // array to hold the 4 frequencies
int melody[100]; // Array to hold our randomly generated melody
int melodyLength = 0;  // How many notes in the Array (-1)
int noteLength = 300;  // NoteLength in iterations of tone pulse
int currentNote = 0;  // the current note within the nelody
int NoteToPlay;  // the current note to play
int lastNote = 5;  // a check that we don't repeat notes
bool ListeningForNotes = true;   // a check that no note errors have been detected so far

int buttonState[4];             // the current reading from the input pins
int lastButtonState[4] = {LOW, LOW, LOW, LOW}; // the previous readings from the input pins

unsigned long lastDebounceTime[4] = {0, 0, 0, 0}; // the last time the output pins were toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
void(* resetFunc) (void) = 0; //declare reset function @ address 0

// ###################################################################
// ###################################################################
void setup()
{
    Serial.begin(115200);
    pinMode(speakerOut, OUTPUT);

    // set all the LED pins as outputs
    for (int led = 3; led <= 6; led++)
    {
        pinMode(led, OUTPUT);
    }

    // set all the Button pins as inputs
    for (int button = 7; button <= 10; button++)
    {
        pinMode(button, INPUT);
    }

    GetReady();     // Play the get ready sequence


    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("NEW GAME STARTED");
    Serial.println();
}


// ###################################################################
// ###################################################################
void GetReady()
{
    randomSeed(analogRead(3));  // seed our andom number generator with noise
    melody[melodyLength] = random(4);
    int index;
    for (int x = 0; x < 10; x++)  // play the below sequence 10 times
    {
        index = 0;
        for (int led = 3; led <= 6; led++)  // iterate through LED pins 3 to 6
        {
            digitalWrite(led, HIGH); // turn the sequence LED on
            for (int repeat = 0; repeat <= 10; repeat++)  // pulse the tone 10 times
            {
                NoteToPlay = notes[index];  // play the indexed note
                playTone();
            }
            digitalWrite(led, LOW);
            index++;
        }
    }
    delay(2000);
}


// ###################################################################
// ###################################################################
void playTone()
{
    // UP
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(NoteToPlay / 2);

    // DOWN
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(NoteToPlay / 2);
}


// ###################################################################
// ###################################################################
void PlayTheTune()
{
    for (int x = 0; x <= melodyLength; x++) // play each note in the sequence so far
    {
        NoteToPlay = notes[melody[x]];

        digitalWrite(melody[x] + 3, HIGH);

        for (int x = 0; x < noteLength; x++)
        {
            playTone();
        }
        digitalWrite(melody[x] + 3, LOW);
    }
}


// ###################################################################
// ###################################################################
void AddANote()
{
    Serial.println("Note Added");
    Serial.println();

    melodyLength++; // increae the length of our melody by 1
    melody[melodyLength] = random(4); // add another random note
    while (melody[melodyLength] == melody[melodyLength - 1]) // if our new note is same as last note change it
    {
        melody[melodyLength] = random(4);
    }
    noteLength = noteLength - 10; // speed things up a little
    if (noteLength < 0) noteLength = 0;
}


// ###################################################################
// ###################################################################
void listenForNotes()
{
    int index = 0;
    for (int buttonPin = 7; buttonPin <= 10; buttonPin++) // iterate through the 4 buttons
    {
        int reading = digitalRead(buttonPin); // read the current button
        if (reading != lastButtonState[index]) { // if the reading differs from last button state reset counter
            // reset the debouncing timer
            lastDebounceTime[index] = millis();
        }

        if ((millis() - lastDebounceTime[index]) > debounceDelay) { // if the debounce delay has passed......

            if (reading != buttonState[index]) { //  change the state if it differs from last one
                buttonState[index] = reading;

                if (buttonState[index] == HIGH) { // if button is pressed....
                    if (index == melody[currentNote]) // if correct note was pressed....
                    {
                        Serial.println("Note " + String(currentNote) + " is correct!");

                        NoteToPlay = notes[melody[currentNote]]; // play the note in the note pressed

                        digitalWrite(melody[currentNote] + 3, HIGH);

                        while (int reading = digitalRead(buttonPin)) // play the note as long as button held
                        {
                            playTone();

                        }
                        digitalWrite(melody[currentNote] + 3, LOW);

                        currentNote++; // increase note count
                        //  if all notes in melody correct then stop listening for button presses and add a note
                        if (currentNote > melodyLength) ListeningForNotes = false;
                    }
                    else // if wrong note pressed....
                    {
                        Serial.println("Note " + String(currentNote) + " is WRONG!");
			Serial.println("Your score was " + String(melodyLength));
                        NoteToPlay = notes[index];

                        digitalWrite(index + 3, HIGH);
                        while (int reading = digitalRead(buttonPin))
                        {

                            playTone();

                        }
                        digitalWrite(index + 3, LOW);
                        resetFunc();  //call reset
                    }
                }
            }
        }

        // save the reading. Next time through the loop, it'll be the lastButtonState:
        lastButtonState[index] = reading;
        index++;
    }

}

// ###################################################################
// ###################################################################
void loop() {
    PlayTheTune(); // Play the melody so far
    ListeningForNotes = true; // toggle for listening for button presses

    currentNote = 0; // start at the first note
    while (ListeningForNotes == true) // as long as we need to, wait for button presses
    {
        listenForNotes();
    }
    AddANote(); // if melody was played correctly, add a new note
    delay(1000);

}
