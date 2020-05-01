//Fort Rush. 
//Take the fort before your friends do!
bool isTroops=false;
bool isSetup=true;
byte activeFaces[6]={1,1,1,1,1,1};
enum signalStates {INERT, GO, RESOLVE, STOP};
byte signalState = INERT;
byte team=2;
byte lastConnectedTeam = 99;
void setup() {
  // put your setup code here, to run once:
  setColor(GREEN);
}

void loop() {
  // put your main code here, to run repeatedly:
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

  //reset the pieces to fort configuration mode (to be built into communication section)
  if (buttonMultiClicked()) {
    byte clicks=0;
    clicks = buttonClickCount();
    if(clicks==3){
      setColor(GREEN);
      team=2;
      isSetup=true;
      isTroops=false;
      signalState=INERT;
      FOREACH_FACE(f){
        activeFaces[f]=1;
      }
    }
  }
  
  //sets piece to Troops Mode
  if(buttonLongPressed()){
    isTroops=true;
    isSetup=false;
    setColor(OFF);
    for(byte i=0;i<2;i++){
      setValueSentOnFace(team,i);
      setColorOnFace(RED,i);
    }
  }

  //changes Teams for Troops
  if(isTroops){
    if(buttonSingleClicked()){
      team++;
      if(team==7){
        team=3;
      }
      switch(team){
        case 3:
          setColorOnFace(CYAN,3);
          setColorOnFace(CYAN,4);
          break;
        case 4:
          setColorOnFace(MAGENTA,3);
          setColorOnFace(MAGENTA,4);
          break;
        case 5:
          setColorOnFace(BLUE,3);
          setColorOnFace(BLUE,4);
          break;
        case 6:
          setColorOnFace(YELLOW,3);
          setColorOnFace(YELLOW,4);
          break;
      }
      
      
    }
    for(byte i=0;i<2;i++){ //tell them what team you are!
        setValueSentOnFace(team,i);
    }
  }

  
  if(!isSetup && !isTroops){ //check if i'm being attacked
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

void checkCapture(){ //checks to see if a piece is captured
  byte health=6;
  FOREACH_FACE(f){
    if(activeFaces[f]==0){
      health--;
    }
  }
  if(health<1){
    switch(lastConnectedTeam){
      case 3:
        setColor(CYAN);
        break;
      case 4:
        setColor(MAGENTA);
        break;
      case 5:
        setColor(BLUE);
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
  if(buttonDoubleClicked()){
    signalState=GO;
  }
  
  FOREACH_FACE(f){ //Am I connected to other Fort pieces?
      if(!isValueReceivedOnFaceExpired(f)){ //someone there
        if(getLastValueReceivedOnFace(f)==INERT){ //sendingSignal
          setColorOnFace(OFF,f);
        }
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
        if(getLastValueReceivedOnFace(f)==GO){ //sendingSignal
          setColorOnFace(OFF,f);
          activeFaces[f]=0;
        }
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
