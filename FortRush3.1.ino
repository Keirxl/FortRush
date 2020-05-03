//Fort Rush. 
//Take the fort before your friends do!



#define PALE makeColorHSB(260,40,70)
#define GUARDCOLOR makeColorRGB(160,160,160)
#define DEADTROOPS makeColorRGB(255,178,0)
#define DEFEND 15

bool isTroops=false;
bool isSetup=true;
bool isGuarded=false;
bool isCaptured=false;
bool deadTroops=false;
bool secondChance=true;
byte activeFaces[6]={1,1,1,1,1,1};
enum signalStates {INERT, GO, RESOLVE, STOP};
byte signalState = INERT;
byte team=2;
byte lastConnectedTeam = 99;
byte troopHP[4]={1,1};
Timer guardTimer;
int someNumber;


void setup() {
  // put your setup code here, to run once:
  setColor(GREEN);
}

void loop() {
  // put your main code here, to run repeatedly:


  //reset the pieces to fort configuration mode (to be built into communication section)
  if (buttonSingleClicked()) {
    byte clicks=0;
    clicks = buttonClickCount();
      setColor(GREEN);
      team=2;
      isSetup=true;
      isTroops=false;
      isCaptured=false;
      deadTroops=false;
      secondChance=true;
      signalState=INERT;
      FOREACH_FACE(f){
        activeFaces[f]=1;
      }
      for(byte i=0;i<2;i++){
       troopHP[i]=1;
      }
  }
  
  //sets piece to Troops Mode
  if(buttonLongPressed()){
    isTroops=true;
    isSetup=false;
    deadTroops=false;
    team=3;
    for(byte i=0;i<2;i++){
      troopHP[i]=1;
    }
    setColor(PALE);
    setValueSentOnAllFaces(0);
    teamSet();
  }

  

  //changes Teams for Troops
  if(isTroops && !deadTroops){

    //change teams
    if(!isSetup){
      if(buttonDoubleClicked()){
        team++;
        if(team==7){
          team=3;
        }
        teamSet();
        for(byte i=0;i<2;i++){
          troopHP[i]=1;
        }
      }
    }
      

    //lose troops if you attach to a guard piece
    for(byte f=4;f<6;f++){
      if(!isValueReceivedOnFaceExpired(f)){
        if(getLastValueReceivedOnFace(f)==10){
          setColorOnFace(DEADTROOPS,f);
          setValueSentOnFace(0,f);
          troopHP[f-4]=0;
          troopCheck();
        }
      }
    }
   }

   

  //setting up the fort formation
  if(isSetup){
    switch(signalState){
      case INERT:
        inertLoop();
        break;
      case GO:
        goLoop();
        break;
      case RESOLVE:
        resolveLoop();
        isSetup=false;
        signalState=STOP;
        break;
    }
    setValueSentOnAllFaces(signalState);
  }

  //if I'm a Fort piece check if I'm being attacked
  if(!isSetup && !isTroops){

    //Guard Timing
    if(!isCaptured){
      if(guardTimer.isExpired()) {
          isGuarded = !isGuarded;
          someNumber=int(random(6000-300));
          guardTimer.set(someNumber);
        }
        
        if(isGuarded) {
          FOREACH_FACE(f){
            if(activeFaces[f]==0){
              setColorOnFace(GUARDCOLOR,f);
              setValueSentOnFace(10,f);
            }
          }
        }else {
          FOREACH_FACE(f){
            setValueSentOnFace(0,f);
            if(activeFaces[f]==0){
              setColorOnFace(PALE,f);
            }
          }
        }
    }

    
    //setValueSentOnAllFaces(0);
    
    if(!isGuarded){
      FOREACH_FACE(f){
        if(activeFaces[f]==1){ //if the face is active
          if(!isValueReceivedOnFaceExpired(f)){ // if someone there
            if(getLastValueReceivedOnFace(f)==3){ //attacking? who is it?
              setColorOnFace(OFF,f);
              activeFaces[f]=0;
              lastConnectedTeam=3;
              checkCapture();
            }else if(getLastValueReceivedOnFace(f)==4){
              setColorOnFace(OFF,f);
              activeFaces[f]=0;
              lastConnectedTeam=4;
              checkCapture();
            }else if(getLastValueReceivedOnFace(f)==5){
              setColorOnFace(OFF,f);
              activeFaces[f]=0;
              lastConnectedTeam=5;
              checkCapture();
            }else if(getLastValueReceivedOnFace(f)==6){
              setColorOnFace(OFF,f);
              activeFaces[f]=0;
              lastConnectedTeam=6;
              checkCapture();
            }else{
              break;
            }
          }
        }
      }
    }
  }

  
//end of loop()
}

void teamSet(){
  switch(team){
        case 3:
          setColorOnFace(dim(CYAN,100),0);
          setColorOnFace(dim(CYAN,100),3);
          setColorOnFace(CYAN,4);
          setColorOnFace(CYAN,5);
          break;
        case 4:
          setColorOnFace(dim(MAGENTA,100),0);
          setColorOnFace(dim(MAGENTA,100),3);
          setColorOnFace(MAGENTA,4);
          setColorOnFace(MAGENTA,5);
          break;
        case 5:
          setColorOnFace(dim(RED,100),0);
          setColorOnFace(dim(RED,100),3);
          setColorOnFace(RED,4);
          setColorOnFace(RED,5);
          break;
        case 6:
          setColorOnFace(dim(YELLOW,100),0);
          setColorOnFace(dim(YELLOW,100),3);
          setColorOnFace(YELLOW,4);
          setColorOnFace(YELLOW,5);
          break;
      }
      for(byte i=4;i<6;i++){ //tell them what team you are!
        setValueSentOnFace(team,i);
    }
    for(byte i=0;i<4;i++){
      setValueSentOnFace(0,i);
    }
}

void troopCheck(){
    byte hp=2;
    for(byte i=0;i<2;i++){
      if(troopHP[i]==0){
        hp--;
      }
    }
    if(hp==0){
      deadTroops=true;
      setValueSentOnAllFaces(0);
      setColor(DEADTROOPS);
    }
    
}

//checks to see if a piece is captured
void checkCapture(){ 
  byte health=6;
  FOREACH_FACE(f){
    if(activeFaces[f]==0){
      health--;
    }
  }
  if(health<1){
    isCaptured=true;
    switch(lastConnectedTeam){
      case 3:
        setColor(CYAN);
        break;
      case 4:
        setColor(MAGENTA);
        break;
      case 5:
        setColor(RED);
        break;
      case 6:
        setColor(YELLOW);
        break;
      default:
        setColor(dim(WHITE,75));
        break;
    }
  }
}

void inertLoop(){ //set up your fort!
  if(buttonMultiClicked()){
    byte clicks=buttonClickCount();
    if(clicks==3){
      signalState=GO;
    }
  }
  
  FOREACH_FACE(f){ //Am I connected to other Fort pieces?
      if(!isValueReceivedOnFaceExpired(f)){ //someone there
          setColorOnFace(OFF,f);
      }else{
        setColorOnFace(GREEN,f);
      }
    }
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getLastValueReceivedOnFace(f) == GO) {//a neighbor saying GO!
        signalState = GO;
      }
    }
  }
}

void goLoop(){
  signalState = RESOLVE;//I default to this at the start of the loop. Only if I see a problem does this not happen

  
  FOREACH_FACE(f){ //Am I connected to other Fort pieces?
      if(!isValueReceivedOnFaceExpired(f)){ //someone there
          setColorOnFace(PALE,f);
          activeFaces[f]=0;
      }else{
        setColorOnFace(GREEN,f);
        activeFaces[f]=1;
      }
    }
  //look for neighbors who have not heard the GO news
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getLastValueReceivedOnFace(f) == INERT) {//This neighbor doesn't know it's GO time. Stay in GO
        signalState = GO;
      }
    }
  }
}

void resolveLoop(){
  signalState = INERT;//I default to this at the start of the loop. Only if I see a problem does this not happen

  //look for neighbors who have not moved to RESOLVE
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getLastValueReceivedOnFace(f) == GO) {//This neighbor isn't in RESOLVE. Stay in RESOLVE
        signalState = RESOLVE;
      }
    }
  }
}
