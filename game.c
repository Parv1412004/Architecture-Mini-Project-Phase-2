#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <time.h>
#define push ((volatile long *) 0xFF200050)
#define brown 41600
#define slider 65535
	
	
/* set a single pixel on the screen at x,y
 * x in [0,319], y in [0,239], and colour in [0,65535]
 */
void write_pixel(int x, int y, int colour) {
    if(x<0||x>=320) return;
    volatile short *_vga_addr = (volatile short *)(0x08000000 + (y << 10) + (x << 1));
    *_vga_addr = colour;
}

/* write a single character to the character buffer at x,y
 * x in [0,79], y in [0,59]
 */
void write_char(int x, int y, char c) {
    // VGA character buffer
    volatile char *_character_buffer = (char *)(0x09000000 + (y << 7) + x);
    *_character_buffer = c;
}

/* use write_pixel to set entire screen to black*/
void clear_screen() {
    int x, y;
    for (x = 0; x < 320; x++) {
        for (y = 0; y < 240; y++) {
            write_pixel(x, y, 0);
        }
    }
    for (x = 0; x < 80; x++) {
        for (y = 0; y < 60; y++) {
            write_char(x, y, ' ');
        }
    }
}

// Player struct
typedef struct {
    int health;
    int score;
    int st;
    int end;
} Player;

// GameLevel struct
typedef struct {
    int levelNumber;
} GameLevel;


double give_x_speed(int level){
    if(level<=5){
        double random_number = ((double)rand() / RAND_MAX)*(1.5)-0.75;
        return random_number;
    }
    else{
        double random_number = ((double)rand() / RAND_MAX)*(2) -1;
        if(random_number>=0) return random_number+0.5;
        else return random_number-0.5;
    }
}

double give_x_cordinate(){
    double random_number = ((double)rand() / RAND_MAX)*(309);
    return random_number;
}

void moveright(Player curplayer){
    if(curplayer.end+1<320){
        for(int j=230; j<240; j++){
            write_pixel(curplayer.st,j,0);
            write_pixel(curplayer.end+1,j,0);
        }
    }
}

void moveleft(Player curplayer){
    if(curplayer.st-1>=0){
        for(int j=230; j<240; j++){
            write_pixel(curplayer.end,j,0);
            write_pixel(curplayer.st-1,j,0);
        }
    }
}

void writescore(){
    char *str = "SCORE ";
    int ind=20;
    while(*str){
        write_char(ind++,58,*str);
        str++;
    }
}

void writelife(){
    char *str = "Life ";
    int ind=50;
    while(*str){
        write_char(ind++,58,*str);
        str++;
    }
}

void write_score(int score){
    write_char(26,58,' ');
    write_char(27,58,' ');
    int ind=26;
    if(score>=10){
        ind++;
        write_char(ind,58,(char)(score%10+'0'));
        ind--;
        score/=10;
        write_char(ind,58,(char)(score%10+'0'));
    }
    else{
        write_char(ind,58,score+'0');
    }
}

void write_life(int health){
    int ind=55;
    write_char(55,58,' ');
    write_char(ind,58,health+'0');
}

void blink(Player curplayer){
    int z=3;
    //printf("y");
    while(z--){
        for(int i = curplayer.st; i<=curplayer.end; i++){
            for(int j=224; j>214; j--){
                write_pixel(i,j,0);
            }
        }
        for(int i = 0; i < (4-z)*1500000; i++){}
        for(int i = curplayer.st; i<=curplayer.end; i++){
            for(int j=224; j>214; j--){
                write_pixel(i,j, slider);
            }
        }
		for(int i = 0; i < 3000000; i++){}
    }
}

void endgame(Player curplayer){
    char *str = "Game Over!";
    int ind=35;
    while(*str){
        write_char(ind++,28,*str);
        str++;
    }
    char *str2 = "Score ";
    ind=36;
    while(*str2){
        write_char(ind++,31,*str2);
        str2++;
    }

    write_char(42,31,' ');
    write_char(43,31,' ');
    ind=42;
    if(curplayer.score>=10){
        ind++;
        write_char(ind,31,(char)(curplayer.score%10+'0'));
        ind--;
        curplayer.score/=10;
        write_char(ind,31,(char)(curplayer.score%10+'0'));
    }
    else{
        write_char(ind,31,curplayer.score+'0');
    }
    for(int i=0; i<100000000; i++){}
}

int main() {
    restart:
    clear_screen();
    srand(time(NULL)); // Seed random number generator
    int red=0,blue=0,green=63;
    int colour = (red << 11) + (green << 5) + blue;
    double y_speed[] = {0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.7};
    Player curplayer = {3,0,0,49};
    GameLevel game = {1};
    
    double ball_x = give_x_cordinate();
    double ball_y = 0; 
    double dx = give_x_speed(game.levelNumber);
    double dy = y_speed[game.levelNumber-1];
    int ball_size = 10;

    for (int i = 0; i < ball_size; i++) {
        for (int j = 0; j < ball_size; j++) {
            write_pixel((int)(ball_x + i), (int)(ball_y + j), colour); 
        }
    }
    for(int i = curplayer.st; i<=curplayer.end; i++){
        for(int j=224; j>214; j--){
            write_pixel(i,j, slider);
        }
    }
    for(int i=0; i<320; i++){
        for(int j=227; j>224; j--){
            write_pixel(i,j, brown);
        }
    }
    writescore();
    writelife();
    write_score(curplayer.score);
    write_life(curplayer.health);

    bool check=true;
    while (1) {
        
        if(*push==1){
            if(curplayer.end+1<320){
                for(int j=215; j<225; j++){
                    write_pixel(curplayer.st,j,0);
                    write_pixel(curplayer.end+1,j,slider);
                }
                curplayer.st+=1;
                curplayer.end+=1;
            }
        }
        else if(*push==2){
            if(curplayer.st-1>=0){
                for(int j=215; j<225; j++){
                    write_pixel(curplayer.end,j,0);
                    write_pixel(curplayer.st-1,j,slider);
                }
                curplayer.st-=1;
                curplayer.end-=1;
            }
        }
        double temp1 = ball_x;
        double temp2 = ball_y;

        // Move the ball
        ball_x += dx;
        ball_y += dy;

        // Check for wall collisions
        if (ball_x < 0 || ball_x + ball_size >= 320) {
            dx = -dx; // Reverse horizontal direction
            ball_x+=2*dx;
        }
        if (ball_y < 0 || ball_y + ball_size > 214) {
            if((int)(ball_x)>=curplayer.st&&(int)(ball_x+9)<=curplayer.end){
                curplayer.score+=game.levelNumber;
                game.levelNumber++;
                if(game.levelNumber>10){
                    break;
                }
                green-=6;
                red+=3;
                colour = (red << 11) + (green << 5) + blue;
                write_score(curplayer.score);
                write_life(curplayer.health);
            }
            else{
                curplayer.health--;
                write_score(curplayer.score);
                write_life(curplayer.health);
                if(curplayer.health==0){
                    blink(curplayer);
                    break;
                }
            }
            ball_y = 0;
            ball_x = give_x_cordinate();
            dy = y_speed[game.levelNumber-1];
            dx = give_x_speed(game.levelNumber);
            for (int i = 0; i <= ball_size; i++) {
                for (int j = 0; j <= ball_size; j++) {
                    write_pixel((int)(temp1 + i), (int)(temp2 + j), 0); // Clear pixel within the ball size
                }
            }
            
            // Draw the ball
            for (int i = 0; i < ball_size; i++) {
                for (int j = 0; j < ball_size; j++) {
                    write_pixel((int)(ball_x + i), (int)(ball_y + j), colour); // Set pixel within the ball size
                }
            }

            // Introduce delay to control the speed of the ball
            for(int i = 0; i < 100000; i++){} // Adjust this value for slower animation
            continue;
        }
        if(check&&temp2>2.0){
            write_pixel(286,2,0);
            check=false;
        }
        if(ball_x>temp1){
            for (double i = temp1; i <= ball_x; i++) {
                for (double j = temp2; j <= temp2+ball_size; j++) {
                    write_pixel((int)(i), (int)(j), 0); // Clear pixel within the ball size
                }
            }
            for (double i = temp2; i <= ball_y; i++) {
                for (double j = temp1; j <= temp1+ball_size; j++) {
                    write_pixel((int)(j), (int)(i), 0); // Clear pixel within the ball size
                }
            }
        }
        else if(ball_x<temp1){
            for (double i = temp1+ball_size; i >= ball_x+ball_size; i--) {
                for (double j = temp2; j <= temp2+ball_size; j++) {
                    write_pixel((int)(i), (int)(j), 0); // Clear pixel within the ball size
                }
            }
            for (double i = temp2; i <= ball_y; i++) {
                for (double j = temp1; j <= temp1+ball_size; j++) {
                    write_pixel((int)(j), (int)(i), 0); // Clear pixel within the ball size
                }
            }
        }
        else{
            for (double j = temp2; j <= ball_y; j++) {
                for(double i = temp1; i<=temp1+ball_size; i++){
                    write_pixel((int)(i), (int)(j), 0); // Clear pixel within the ball size
                }
            }
        }
        if(ball_y>4){
            for(int i=0; i<320; i++){
                write_pixel((int)(i), (int)(4), 0);
            }
        }
        for (int i = 0; i < ball_size; i++) {
            for (int j = 0; j < ball_size; j++) {
                write_pixel((int)(ball_x + i), (int)(ball_y + j), colour); // Set pixel within the ball size
            }
        }

        // Introduce delay to control the speed of the ball
        for(int i = 0; i < 100000; i++){} // Adjust this value for slower animation
    }
    clear_screen();
    endgame(curplayer);
     goto restart;
    return 0;
}