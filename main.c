/**********************************************************************************************************************
 *  Monash University, Electrical and Computer Systems Engineering
 *  Final Year Project, 2019-2020
 *  Engineer: Luke Andrew Gaschk, 26946475
 *  Supervisor: Malin Premaratne
 *  Atmospheric Nightlight
 *  Main Application Program Code
 /**********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <time.h>

/**********************************************************************************************************************/

int TFTVisual = 0;
int ProjVisual = 0;
int enterFlag = 0;
int startFlag = 1;
int powerFlag = 0;
int count = 1;
int nScene = 121;
int nDuration = 141;
int mScene = 221;
int wakeupTime = 241;
int bright = 321;
int mornOn = 341;
int brightPercent = 32;
int brightChange = 0;
long unsigned nightSeconds = 0;
long unsigned mornSeconds = 86399;
int Projecting = 0;
long unsigned ProjectionCount = 0;
int waitingForMorn = 0;
int nightRoutinePlaying = 0;

int Standby(void);
int TFTLookup(void);
int TurnHDMIOff(void);
int TurnTFTOff(void);
int PowerRoutine(void);
int PowerOn(void);
#define IGNORE_CHANGE_BELOW_USEC 300000
struct timeval last_change;
struct timeval startTime;

#define SEC_PER_DAY   86400
#define SEC_PER_HOUR  3600
#define SEC_PER_MIN   60
struct timeval tv;
struct timezone tz;
long secSinceMidnight = 0;
int dummyMornMode = 1;

int startLED = 0;
int pwrLED = 0;

int pin_Up = 26;
int pin_Down = 22;
int pin_Left = 23;
int pin_Right = 27;
int pin_Enter = 24;
int pin_StartButton = 28;
int pin_PwrButton = 29;
int pin_StartLED = 4;
int pin_PwrLED = 5;
int pin_LCD = 1;

/**********************************************************************************************************************/

PI_THREAD(ProjectionTimer) {
        for(;;) {
        printf("TIMER INTERRUPT FUNCTIONING\n");
        if (Projecting) {
            ProjectionCount++;
        }
        delay(1000);
        if (nightRoutinePlaying) {
            if (ProjectionCount > nightSeconds) {
                TurnHDMIOff();
                nightRoutinePlaying = 0;
            }
        }
    }
return 0;}

/**********************************************************************************************************************/
void PressUp(void) { if (powerFlag){
    if (!startFlag) {
        struct timeval now;
        unsigned long diff;
        gettimeofday(&now,NULL);
        diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
        if (diff > IGNORE_CHANGE_BELOW_USEC) {
            if(!enterFlag) {
                TFTVisual/=20;
                TFTVisual--;
                if(TFTVisual == 4) {TFTVisual = 7;}
                if(TFTVisual == 9) {TFTVisual = 12;}
                if(TFTVisual == 14) {TFTVisual = 17;}
                TFTVisual*=20;
            }
            else {
                TFTVisual--;
                if(TFTVisual == 120){TFTVisual = 124;}
                if(TFTVisual == 140){TFTVisual = 148;}
                if(TFTVisual == 220){TFTVisual = 224;}
                if(TFTVisual == 240){TFTVisual = 257;}
                if(TFTVisual == 320){TFTVisual = 324;}
                if(TFTVisual == 340){TFTVisual = 342;}
            }
            TFTLookup();
        }
        last_change = now;
    }
}}

/**********************************************************************************************************************/

void PressDown(void) { if (powerFlag){
    if (!startFlag) {
        struct timeval now;
        unsigned long diff;
        gettimeofday(&now,NULL);
        diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
        if (diff > IGNORE_CHANGE_BELOW_USEC) {
            if(!enterFlag) {
                TFTVisual/=20;
                TFTVisual++;
                if(TFTVisual == 8) {TFTVisual = 5;}
                if(TFTVisual == 13) {TFTVisual = 10;}
                if(TFTVisual == 18) {TFTVisual = 15;}
                TFTVisual*=20;
            }
            else {
                TFTVisual++;
                if(TFTVisual == 125){TFTVisual = 121;}
                if(TFTVisual == 149){TFTVisual = 141;}
                if(TFTVisual == 225){TFTVisual = 221;}
                if(TFTVisual == 258){TFTVisual = 241;}
                if(TFTVisual == 325){TFTVisual = 321;}
                if(TFTVisual == 343){TFTVisual = 341;}
            }
            TFTLookup();
        }
        last_change = now;
    }
}}

/**********************************************************************************************************************/

void PressLeft(void) { if (powerFlag){
    if (!startFlag) {
        struct timeval now;
        unsigned long diff;
        gettimeofday(&now,NULL);
        diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
        if (diff > IGNORE_CHANGE_BELOW_USEC) {
            if (brightChange == 1) {
                switch (bright) {
                    case 321:
                        brightPercent = 32;
                        break;
                    case 322:
                        brightPercent = 24;
                        break;
                    case 323:
                        brightPercent = 16;
                        break;
                    case 324:
                        brightPercent = 8;
                        break;
                }
                softPwmWrite(pin_LCD,brightPercent);
                brightChange = 0;
            }
            TFTVisual/=100;
            TFTVisual--;
            if(!TFTVisual) {TFTVisual = 3;}
            TFTVisual*=100;
            if(enterFlag){enterFlag = 0;}
            TFTLookup();
        }
        last_change = now;
    }
}}

/**********************************************************************************************************************/

void PressRight(void) { if (powerFlag){
    if (!startFlag) {
        struct timeval now;
        unsigned long diff;
        gettimeofday(&now,NULL);
        diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
        if (diff > IGNORE_CHANGE_BELOW_USEC) {
            if (brightChange == 1) {
                switch (bright) {
                    case 321:
                        brightPercent = 32;
                        break;
                    case 322:
                        brightPercent = 24;
                        break;
                    case 323:
                        brightPercent = 16;
                        break;
                    case 324:
                        brightPercent = 8;
                        break;
                }
                softPwmWrite(pin_LCD,brightPercent);
                brightChange = 0;
            }
            TFTVisual/=100;
            TFTVisual++;
            if(TFTVisual == 4) {TFTVisual = 1;}
            TFTVisual*=100;
            if(enterFlag){enterFlag = 0;}
            TFTLookup();
        }
        last_change = now;
    }
}}

/**********************************************************************************************************************/

void PressEnter(void) { if (powerFlag){
    if (!startFlag) {
        struct timeval now;
        unsigned long diff;
        gettimeofday(&now,NULL);
        diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
        if (diff > IGNORE_CHANGE_BELOW_USEC) {
            int temp = TFTVisual;
            if((TFTVisual == 120)&&(!enterFlag)) {
                TFTVisual = nScene;
            }
            if((TFTVisual == 140)&&(!enterFlag)) {
                TFTVisual = nDuration;
            }
            if((TFTVisual == 220)&&(!enterFlag)) {
                TFTVisual = mScene;
            }
            if((TFTVisual == 240)&&(!enterFlag)) {
                TFTVisual = wakeupTime;
            }
            if((TFTVisual == 320)&&(!enterFlag)) {
                TFTVisual = bright;
            }
            if((TFTVisual == 340)&&(!enterFlag)) {
                TFTVisual = mornOn;
            }
            if((TFTVisual/20 == 6)&&(enterFlag)) {
                nScene = TFTVisual;
                TFTVisual = 120;
            }
            if((TFTVisual/20 == 7)&&(enterFlag)) {
                nDuration = TFTVisual;
                TFTVisual = 140;
            }
            if((TFTVisual/20 == 11)&&(enterFlag)) {
                mScene = TFTVisual;
                TFTVisual = 220;
            }
            if((TFTVisual/20 == 12)&&(enterFlag)) {
                wakeupTime = TFTVisual;
                TFTVisual = 240;
            }
            if((TFTVisual/20 == 16)&&(enterFlag)) {
                bright = TFTVisual;
                TFTVisual = 320;
            }
            if((TFTVisual/20 == 17)&&(enterFlag)) {
                mornOn = TFTVisual;
                TFTVisual = 340;
            }
            if(temp != TFTVisual) {
                enterFlag = !enterFlag;
                TFTLookup();
            }
        }
        last_change = now;
    }
}}

/**********************************************************************************************************************/

void PressStart(void) { if (powerFlag){
    startFlag = 1;
    struct timeval now;
    unsigned long diff;
    gettimeofday(&now,NULL);
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
    if (diff > IGNORE_CHANGE_BELOW_USEC*4) {
        system("sudo vcgencmd display_power 1");
        if (brightChange == 1) {
            switch (bright) {
                case 321:
                    brightPercent = 32;
                    break;
                case 322:
                    brightPercent = 24;
                    break;
                case 323:
                    brightPercent = 16;
                    break;
                case 324:
                    brightPercent = 8;
                    break;
            }
            softPwmWrite(pin_LCD,brightPercent);
            brightChange = 0;
        }
        
        if(TFTVisual) {
            digitalWrite(pin_StartLED, LOW);
            TFTVisual = 0;
            switch (nDuration) {
                case 141:
                    nightSeconds = 5;
                break;
                case 142:
                    nightSeconds = 5 * 60;
                break;
                case 143:
                    nightSeconds = 10 * 60;
                break;
                case 144:
                    nightSeconds = 20 * 60;
                break;
                case 145:
                    nightSeconds = 30 * 60;
                break;
                case 146:
                    nightSeconds = 45 * 60;
                break;
                case 147:
                    nightSeconds = 60 * 60;
                break;
                case 148:
                    nightSeconds = 90 * 60;
                break;
            }
            Projecting = 1;
        }
        else {
            Projecting = 0;
            waitingForMorn = 0;
            startFlag = 0;
            digitalWrite(pin_StartLED, HIGH);
            TFTVisual = 100;
            TurnHDMIOff();
        }
    }
    last_change = now;
}}

/**********************************************************************************************************************/

void PressPower(void) {
    struct timeval now;
    unsigned long diff;
    gettimeofday(&now,NULL);
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
    if (diff > IGNORE_CHANGE_BELOW_USEC*4) {
        if (powerFlag == 0) {
            powerFlag = 1;
        }
        else {
            powerFlag = 0;
            Projecting = 0;
            waitingForMorn = 0;
            TurnHDMIOff();
            TurnTFTOff();
        }
    }
    last_change = now;
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

int main(int argc, char ** argv) {
    wiringPiSetup();
    softPwmCreate(pin_LCD, 0, brightPercent);
    pinMode(pin_Up, INPUT);
    pinMode(pin_Down, INPUT);
    pinMode(pin_Left, INPUT);
    pinMode(pin_Right, INPUT);
    pinMode(pin_Enter, INPUT);
    pinMode(pin_StartButton, INPUT);
    pinMode(pin_PwrButton, INPUT);
    pinMode(pin_StartLED, OUTPUT); digitalWrite(pin_StartLED, HIGH);
    pinMode(pin_PwrLED, OUTPUT); digitalWrite(pin_PwrLED, HIGH);
    
    pullUpDnControl(pin_Up, PUD_UP);
    pullUpDnControl(pin_Down, PUD_UP);
    pullUpDnControl(pin_Left, PUD_UP);
    pullUpDnControl(pin_Right, PUD_UP);
    pullUpDnControl(pin_Enter, PUD_UP);
    pullUpDnControl(pin_StartButton, PUD_UP);
    pullUpDnControl(pin_PwrButton, PUD_UP);
    
    piThreadCreate(ProjectionTimer);
    gettimeofday(&last_change, NULL);
    
    wiringPiISR(pin_Up, INT_EDGE_RISING, &PressUp);
    wiringPiISR(pin_Down, INT_EDGE_RISING, &PressDown);
    wiringPiISR(pin_Left, INT_EDGE_RISING, &PressLeft);
    wiringPiISR(pin_Right, INT_EDGE_RISING, &PressRight);
    wiringPiISR(pin_Enter, INT_EDGE_RISING, &PressEnter);
    wiringPiISR(pin_StartButton, INT_EDGE_RISING, &PressStart);
    wiringPiISR(pin_PwrButton, INT_EDGE_RISING, &PressPower);
    
    PowerRoutine();
    for(;;) {
        // Main Infinite Loop
        printf("Sleeping for 1 second. Iteration: %d\n",count);
        count++;
        sleep(1);
        
        // Check for power off
        if (!powerFlag) {PowerRoutine();}
        
        // Check for projection start
        if (Projecting == 1) {
            TurnTFTOff();
            ProjectionCount = 0;
            nightRoutinePlaying = 1;
            switch (nScene) {
                case 121:
                    system("omxplayer /home/pi/Desktop/videoContent/N1.mp4 --aspect-mode fill");
                    break;
                case 122:
                    system("omxplayer /home/pi/Desktop/videoContent/N2.mp4 --aspect-mode fill");
                    break;
                case 123:
                    system("omxplayer /home/pi/Desktop/videoContent/N3.mp4 --aspect-mode fill");
                    break;
                case 124:
                    system("omxplayer /home/pi/Desktop/videoContent/N4.mp4 --aspect-mode fill");
                    break;
            }
            TFTLookup();
            
            // Check for morning mode
            if (mornOn == 341 && TFTVisual == 0) {
                
                // Find current time
                gettimeofday(&tv, &tz);
                tz.tz_dsttime = +10;
                secSinceMidnight = tv.tv_sec % SEC_PER_DAY;
                secSinceMidnight += tz.tz_dsttime * SEC_PER_HOUR;
                secSinceMidnight -= tz.tz_minuteswest * SEC_PER_MIN;
                secSinceMidnight = (secSinceMidnight + SEC_PER_DAY) % SEC_PER_DAY;
                long int timeToWake = 0;
                
                // Check for 'morning mode override' dev feature
                if (dummyMornMode) {mornSeconds = secSinceMidnight + 5;}
                
                // Check for day rollover
                if (secSinceMidnight < mornSeconds) {
                    timeToWake = mornSeconds - secSinceMidnight;
                }
                if (secSinceMidnight > mornSeconds) {
                    timeToWake = SEC_PER_DAY - secSinceMidnight + mornSeconds;
                }
                waitingForMorn = 1;
                ProjectionCount = 0;
                
                // Enter waiting loop til morning is reached.
                while (waitingForMorn == 1) {
                    delay(500);
                    if (ProjectionCount > timeToWake) {waitingForMorn = 0;}
                    if (powerFlag == 0) {
                        Projecting = 0;
                        waitingForMorn = 0;
                        digitalWrite(pin_StartLED, HIGH);
                        break;
                    }
                }
                
                // Check if program is still running. Enter morning mode.
                if(Projecting) {
                    system("sudo vcgencmd display_power 1");
                    switch (mScene) {
                        case 221:
                            system("omxplayer /home/pi/Desktop/videoContent/M1.mp4 --aspect-mode fill");
                            break;
                        case 222:
                            system("omxplayer /home/pi/Desktop/videoContent/M2.mp4 --aspect-mode fill");
                            break;
                        case 223:
                            system("omxplayer /home/pi/Desktop/videoContent/M3.mp4 --aspect-mode fill");
                            break;
                        case 224:
                            system("omxplayer /home/pi/Desktop/videoContent/M4.mp4 --aspect-mode fill");
                            break;
                    }
                }
            }
            
            // Exit program.
            if (powerFlag) {
                system("sudo vcgencmd display_power 0");
                startFlag = 0;
                Projecting = 0;
                digitalWrite(pin_StartLED, HIGH);
                
                // Ensure the correct TFT screen is displayed.
                if (~TFTVisual) {TFTVisual = 100;}
                TFTLookup();
                
                // Turn TFT back on.
                for (int i = 1; i < brightPercent+1; i++) {
                    softPwmWrite(pin_LCD,i);
                    usleep(10000);
                }
            }
        } // Return to main infinite loop.
    }
    
    return 0;}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

int Standby() {
    int bootStatusCount = 0;
    digitalWrite(pin_PwrLED, LOW);
    while(!powerFlag) {
        bootStatusCount++;
        if (bootStatusCount == 700) {
            digitalWrite(pin_PwrLED, HIGH);
        }
        if (bootStatusCount == 1400) {
            digitalWrite(pin_PwrLED, LOW);
            bootStatusCount = 0;
        }
        usleep(1000);
    }
return 0;}

/**********************************************************************************************************************/

int PowerOn() {
    digitalWrite(pin_PwrLED, LOW);
    TFTVisual = 100;
    TFTLookup();
    brightPercent = 32;
    for (int i = 1; i < brightPercent+1; i++) {
        softPwmWrite(pin_LCD,i);
        usleep(6000);
    }
return 0;}

/**********************************************************************************************************************/

int TFTLookup() {
    switch (TFTVisual) {
        case 0:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/0_blank.png");
            break;
        case 1:
            break;
        case 100:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/3_night.png");
            break;
        case 120:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/4_nScene.png");
            break;
        case 121:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/5_nScene1.png");
            break;
        case 122:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/6_nScene2.png");
            break;
        case 123:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/7_nScene3.png");
            break;
        case 124:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/8_nScene4.png");
            printf("Night scene 4\n");
            break;
        case 140:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/9_duration.png");
            break;
        case 141:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/10_5sec.png");
            break;
        case 142:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/11_5min.png");
            break;
        case 143:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/12_10min.png");
            break;
        case 144:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/13_20min.png");
            break;
        case 145:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/14_30min.png");
            break;
        case 146:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/15_45min.png");
            break;
        case 147:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/16_1hr.png");
            break;
        case 148:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/17_1.5hr.png");
            break;
        case 200:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/18_morning.png");
            break;
        case 220:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/19_mScene.png");
            break;
        case 221:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/20_mScene1.png");
            break;
        case 222:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/21_mScene2.png");
            break;
        case 223:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/22_mScene3.png");
            break;
        case 224:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/23_mScene4.png");
            break;
        case 240:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/24_wakeupTime.png");
            break;
        case 241:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/25_0600.png");
            mornSeconds = ((6*60)+0)*60;
            break;
        case 242:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/26_0615.png");
            mornSeconds = ((6*60)+15)*60;
            break;
        case 243:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/27_0630.png");
            mornSeconds = ((6*60)+30)*60;
            break;
        case 244:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/28_0645.png");
            mornSeconds = ((6*60)+45)*60;
            break;
        case 245:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/29_0700.png");
            mornSeconds = ((7*60)+0)*60;
            break;
        case 246:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/30_0715.png");
            mornSeconds = ((7*60)+15)*60;
            break;
        case 247:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/31_0730.png");
            mornSeconds = ((7*60)+30)*60;
            break;
        case 248:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/32_0745.png");
            mornSeconds = ((7*60)+45)*60;
            break;
        case 249:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/33_0800.png");
            mornSeconds = ((8*60)+0)*60;
            break;
        case 250:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/34_0815.png");
            mornSeconds = ((8*60)+15)*60;
            break;
        case 251:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/35_0830.png");
            mornSeconds = ((8*60)+30)*60;
            break;
        case 252:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/36_0845.png");
            mornSeconds = ((8*60)+45)*60;
            break;
        case 253:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/37_0900.png");
            mornSeconds = ((9*60)+0)*60;
            break;
        case 254:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/38_0915.png");
            mornSeconds = ((9*60)+15)*60;
            break;
        case 255:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/39_0930.png");
            mornSeconds = ((9*60)+30)*60;
            break;
        case 256:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/40_0945.png");
            mornSeconds = ((9*60)+45)*60;
            break;
        case 257:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/41_1000.png");
            mornSeconds = ((10*60)+0)*60;
            break;
        case 300:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/42_settings.png");
            break;
        case 320:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/43_brightness.png");
            break;
        case 321:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/44_100.png");
            brightPercent = 32;
            softPwmWrite(pin_LCD,brightPercent);
            brightChange = 1;
            dummyMornMode = 1;
            break;
        case 322:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/45_75.png");
            brightPercent = 24;
            softPwmWrite(pin_LCD,brightPercent);
            brightChange = 1;
            break;
        case 323:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/46_50.png");
            brightPercent = 16;
            softPwmWrite(pin_LCD,brightPercent);
            brightChange = 1;
            break;
        case 324:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/47_25.png");
            brightPercent = 8;
            softPwmWrite(pin_LCD,brightPercent);
            brightChange = 1;
            dummyMornMode = 0;
            break;
        case 340:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/48_mornOnOff.png");
            break;
        case 341:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/49_mornOn.png");
            break;
        case 342:
            system("sudo fbi -T 2 -noverbose -d /dev/fb1 -a /home/pi/Desktop/menuVisuals/50_mornOff.png");
            break;
    }
return 0;}

/**********************************************************************************************************************/

int TurnHDMIOff() {
    system("sudo vcgencmd display_power 0");
    system("killall omxplayer");
    system("killall omxplayer.bin");
return 0;}

int PowerRoutine() {
    digitalWrite(pin_StartLED, HIGH);
    digitalWrite(pin_PwrLED, HIGH);
    Standby();
    PowerOn();
    startFlag = 0;
return 0;}

int TurnTFTOff() {
    for (int i = brightPercent*powerFlag; i > -1; i--) {
        softPwmWrite(pin_LCD,i);
        usleep(10000);
    }
return 0;}
