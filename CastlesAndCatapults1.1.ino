
#define PALE makeColorHSB(200,50,70)
#define FORT makeColorHSB(90,255,255)
#define dustblue makeColorHSB(115,255,255)
#define burntyellow makeColorHSB(35,255,255)
#define burntorange makeColorHSB(25,200,255)
#define purple makeColorHSB(200,255,255)
#define pink makeColorHSB(10,210,255)
#define teal makeColorHSB(100,255,255)
#define mint makeColorHSB(90,255,255)
#define pastelpurple makeColorHSB(190,255,255)

#define dimness 100



Color teamcolor[4]={dustblue, pastelpurple, burntorange, teal};
byte ignoredFaces[6]={0,0,0,0,0,0};
byte hp=4;
byte team=0;
byte lastConnectedTeam=0;
bool isDead=false;
bool isTroops=false;


void setup() {
  // put your setup code here, to run once:
  setColor(FORT);
  setValueSentOnAllFaces(0);
}



void loop() {
  // put your main code here, to run repeatedly:
  if(buttonSingleClicked()){
    hp=4;
    isTroops=false;
    isDead=false;
    setValueSentOnAllFaces(0);
  }

  if(buttonLongPressed()){
    isTroops=true;
    isDead=false;
    team=0;
    setValueSentOnAllFaces(team);
    teamSet();
  }

  //change teams
  if(isTroops){
    if(buttonDoubleClicked()){
        team++;
        if(team==4){
          team=0;
        }
        teamSet();
     }
  }

  if(!isDead && !isTroops){
    FOREACH_FACE(f){
      if(!isValueReceivedOnFaceExpired(f)){//isconnect
        if(getLastValueReceivedOnFace(f)!=0){//if not fort piece
          if(ignoredFaces[f]==0){ //if not ignored face
            hp--;
            lastConnectedTeam=getLastValueReceivedOnFace(f);
            if(hp<1){
              capture();
            }
          }
        }
        if(!isDead){
          setColorOnFace(PALE,f);
          ignoredFaces[f]=1;
        }
      }else{
        if(!isDead){
          ignoredFaces[f]=0;
          setColorOnFace(FORT,f);
        }
      }
    }
  }
  
}

void teamSet(){
  setColor(teamcolor[team]);
  setColorOnFace(dim(teamcolor[team],dimness),0);
  setColorOnFace(dim(teamcolor[team],dimness),2);
  setColorOnFace(dim(teamcolor[team],dimness),4);
  setValueSentOnAllFaces(team+1);
}

void capture(){
    isDead=true;
    setValueSentOnAllFaces(0);
    setColor(teamcolor[lastConnectedTeam-1]);
}
