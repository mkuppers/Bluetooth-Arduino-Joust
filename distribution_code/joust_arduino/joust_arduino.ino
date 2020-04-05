#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>
#include <SoftwareSerial.h> 

// define the wiring of the LED screen
const uint8_t CLK  = 11;
const uint8_t LAT = 10;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;
const uint8_t D = A3;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 7;
const int BUTTON_PIN_NUMBER = 7;

const int POTENTIOMETER_PIN_NUMBER_2 = 6;
const int BUTTON_PIN_NUMBER_2 = 6;


int bluetoothTx = 51;  
int bluetoothRx = 50;

int stored_value = 3;
bool jump_blue = false;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

const int Y_DIMENSION = 32;
const int X_DIMENSION = 64;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);
const Color BROWN(150, 75, 0);


class Platform{
  public:
    Platform(){
      int pixels = 0;
    }

    void draw_level_1(){
      int c = 0;
      for(int i = -7; i < X_DIMENSION + 7; i++){
        matrix.drawPixel(i, 0, PURPLE.to_333());
        //Add X coordinates
        coord_array[c][0] = i;
        //Add Y coordinates
        coord_array[c][1] = 0;
        c++;
      }

      for(int i = -4; i < X_DIMENSION/3; i++){
        matrix.drawPixel(i, Y_DIMENSION/2, PURPLE.to_333());
        coord_array[c][0] = i;
        coord_array[c][1] = Y_DIMENSION/2;
        c++;
      }  

      for(int i = X_DIMENSION + 4; i > 2 * X_DIMENSION/3; i--){
        matrix.drawPixel(i, Y_DIMENSION/2, PURPLE.to_333());
        coord_array[c][0] = i;
        coord_array[c][1] = Y_DIMENSION/2;
        c++;
      }
      pixels = c;  
    }

    void draw_level_2(){
      int c = 0;
      for(int i = X_DIMENSION + 2 - X_DIMENSION/5; i > X_DIMENSION/5; i--){
        matrix.drawPixel(i, Y_DIMENSION/2, PURPLE.to_333());
        coord_array[c][0] = i;
        coord_array[c][1] = Y_DIMENSION/2;
        c++;
      }  
      pixels = c;
    }

    void draw_level_3(){
      int c = 0;

      for(int i = X_DIMENSION - X_DIMENSION/3; i > X_DIMENSION/3; i--){
        matrix.drawPixel(i, 0, PURPLE.to_333());        
        coord_array[c][0] = i;
        coord_array[c][1] = 0;
        c++;
      }

      for(int i = 4; i < X_DIMENSION/3; i++){
        matrix.drawPixel(i, Y_DIMENSION/2, PURPLE.to_333());
        coord_array[c][0] = i;
        coord_array[c][1] = Y_DIMENSION/2;
        c++;
      }  

      for(int i = X_DIMENSION - 4; i > 2 * X_DIMENSION/3; i--){
        matrix.drawPixel(i, Y_DIMENSION/2, PURPLE.to_333());
        coord_array[c][0] = i;
        coord_array[c][1] = Y_DIMENSION/2;
        c++;
      }
      pixels = c;  
    }

    bool is_part_of_platform(int x, int y){
      for(int i = 0; i < pixels; i++){
        if(coord_array[i][0] == x){
          if(coord_array[i][1] == y){
            return true;  
          }
        } 
      }
      return false;
    }
    
  private:
    int coord_array[250][2];
    int pixels;
};

class Player {
  public:
    Player() {
      x = X_DIMENSION/2;
      y = 1;      
      lives = 3;
      facing_left = true;
      on_ground = true;
      still = true;
    }

    Player(Color c, int x_coord, int y_coord, bool facing_direction) {
      main_color = c;
      x = x_coord;
      y = y_coord;
      lives = 3;
      facing_left = facing_direction;
      on_ground = true;     
    }
   
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }

    void set_on_ground(bool b){
      on_ground = b;  
    }

    bool get_direction(){
      return facing_left;  
    }
   
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }

    void set_y(int y_arg) {
      y = y_arg;
    }

    void set_direction(bool b){
      facing_left = b;
    }
   
    // Modifies: lives
    void die() {
      lives--;     
    }

    void player_null() {
      lives = -1;
      x = -500;
      y = -500;
    }

    void reset_lives() {
      lives = 3;
    }
   
    void reset(){
      x = X_DIMENSION/2;
      y = 0;  
    }

    void gravity(int frames){
      if(frames % 3 == 0){
        erase_left_frame();
        erase_right_frame();
        y--;
        if(y >= Y_DIMENSION){
          y = 1;
        }
        if(y < 0){
          y = Y_DIMENSION;
        }          
      }             
    }

    void fly(int frames){
      if(frames % 3 == 0){
        erase_left_frame();
        erase_right_frame();
        y += 4;
        on_ground = false;
      }                 
    }

    void move(int dial_value, int frame){
      int mod = 1;    
      //When potentiometer is pointed left
      if(dial_value < 460){
        still = false;
        if(dial_value < 200){
          mod = 2;  
        }
        if(facing_left){
          erase_right_frame();
        }       
        facing_left = false;
        if(on_ground){
          if(frame%12==0){
            erase_left_frame();
            x += 1 * mod;
            walk_left_frame1();
          }
          else if(frame%6==0){
            erase_left_frame();
            x += 1 * mod;
            still_left_frame();
          }
          else if(frame%3==0){
            erase_left_frame();
            x += 1 * mod;
            walk_left_frame2();
          }
        }                           
        else if(!on_ground){
          if(frame%12==0){
            erase_left_frame();
            x += 1 * mod;
            fly_left_frame1();
          }
          else if(frame%6==0){
            erase_left_frame();
            x += 1 * mod;
            draw_left_base(BROWN, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_left_frame();
            x += 1 * mod;
            fly_left_frame2();
          }
        }
      }
     //When potentiometer is pointed right
     else if(dial_value > 600){
      still = false;
      if(dial_value > 800){
        mod = 2;  
      }
      if(on_ground){
        if(!facing_left){
          erase_left_frame();
        }     
        facing_left = true;
        if(frame%12==0){
          erase_right_frame();
          x -= 1 * mod;
          walk_right_frame1();
        }
        else if(frame%6==0){
          erase_right_frame();
          x -= 1 * mod;
          still_right_frame();
        }
        else if(frame%3==0){
          erase_right_frame();
          x -= 1 * mod;
          walk_right_frame2();
        }
      }
      else if(!on_ground){
          if(frame%12==0){
            erase_right_frame();
            x -= 1 * mod;
            fly_right_frame1();
          }
          else if(frame%6==0){
            erase_right_frame();
            x -= 1 * mod;
            draw_right_base(BROWN, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_right_frame();
            x -= 1 * mod;
            fly_right_frame2();
          }
      }
     }
     //If potentiometer is neither left or right
     else{      
       if(facing_left){
        if(on_ground){
          if(!still){
            erase_right_frame();
          }
          still_right_frame();
        }
        else if(!on_ground){
          if(frame%12==0){
            erase_right_frame();
            fly_right_frame1();
          }
          else if(frame%6==0){
            erase_right_frame();
            draw_right_base(BROWN, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_right_frame();
            fly_right_frame2();
          }
        }                
       }
       else{
         if(on_ground){
            if(!still){
              erase_left_frame();
            }
            still_left_frame();
          }
          else if(!on_ground){
            if(frame%12==0){
              erase_left_frame();
              fly_left_frame1();
            }
            else if(frame%6==0){
              erase_left_frame();
              draw_left_base(BROWN, WHITE, main_color, YELLOW);
            }
            else if(frame%3==0){
              erase_left_frame();
              fly_left_frame2();
            }
          }           
        }
        still = true;
      }

      //Loop around
      if(x < -7){
        erase_right_frame();
        x = X_DIMENSION;  
      }
      if(x > X_DIMENSION){
        x = 0;  
      }
    }

    void move_bluetooth(int slider_value, int frame){
      int mod = 1;    
      //When potentiometer is pointed left
      if(slider_value < 4){
        still = false;
        if(slider_value < 2){
          mod = 2;  
        }
        if(facing_left){
          erase_right_frame();
        }       
        facing_left = false;
        if(on_ground){
          if(frame%12==0){
            erase_left_frame();
            x += 1 * mod;
            walk_left_frame1();
          }
          else if(frame%6==0){
            erase_left_frame();
            x += 1 * mod;
            still_left_frame();
          }
          else if(frame%3==0){
            erase_left_frame();
            x += 1 * mod;
            walk_left_frame2();
          }
        }                           
        else if(!on_ground){
          if(frame%12==0){
            erase_left_frame();
            x += 1 * mod;
            fly_left_frame1();
          }
          else if(frame%6==0){
            erase_left_frame();
            x += 1 * mod;
            draw_left_base(BROWN, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_left_frame();
            x += 1 * mod;
            fly_left_frame2();
          }
        }
      }
     //When potentiometer is pointed right
     else if(slider_value > 5){
      still = false;
      if(slider_value > 7){
        mod = 2;  
      }
      if(on_ground){
        if(!facing_left){
          erase_left_frame();
        }     
        facing_left = true;
        if(frame%12==0){
          erase_right_frame();
          x -= 1 * mod;
          walk_right_frame1();
        }
        else if(frame%6==0){
          erase_right_frame();
          x -= 1 * mod;
          still_right_frame();
        }
        else if(frame%3==0){
          erase_right_frame();
          x -= 1 * mod;
          walk_right_frame2();
        }
      }
      else if(!on_ground){
          if(frame%12==0){
            erase_right_frame();
            x -= 1 * mod;
            fly_right_frame1();
          }
          else if(frame%6==0){
            erase_right_frame();
            x -= 1 * mod;
            draw_right_base(BROWN, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_right_frame();
            x -= 1 * mod;
            fly_right_frame2();
          }
      }
     }
     //If potentiometer is neither left or right
     else{      
       if(facing_left){
        if(on_ground){
          if(!still){
            erase_right_frame();
          }
          still_right_frame();
        }
        else if(!on_ground){
          if(frame%12==0){
            erase_right_frame();
            fly_right_frame1();
          }
          else if(frame%6==0){
            erase_right_frame();
            draw_right_base(BROWN, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_right_frame();
            fly_right_frame2();
          }
        }                
       }
       else{
         if(on_ground){
            if(!still){
              erase_left_frame();
            }
            still_left_frame();
          }
          else if(!on_ground){
            if(frame%12==0){
              erase_left_frame();
              fly_left_frame1();
            }
            else if(frame%6==0){
              erase_left_frame();
              draw_left_base(BROWN, WHITE, main_color, YELLOW);
            }
            else if(frame%3==0){
              erase_left_frame();
              fly_left_frame2();
            }
          }           
        }
        still = true;
      }

      //Loop around
      if(x < -7){
        erase_right_frame();
        x = X_DIMENSION;  
      }
      if(x > X_DIMENSION){
        x = 0;  
      }
    }

    void bounce(){
      if(facing_left){
        erase_right_frame();
        x += 10;
      }
      else{
        erase_left_frame();
        x -= 10;
      }  
    }

  private:
    int x;
    int y;
    int lives;   
    bool on_ground;
    bool still;
    bool facing_left;
    Color main_color;

    //Draw functions
    void draw_left_base(Color emu_color, Color saber_color, Color player_color, Color eye_color){
      matrix.drawPixel(x, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 1, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 2, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 3, y + 2, emu_color.to_333());

      matrix.drawPixel(x, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 1, y + 3, player_color.to_333());
      matrix.drawPixel(x + 2, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 3, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 3, emu_color.to_333());

      matrix.drawPixel(x, y + 4, player_color.to_333());
      matrix.drawPixel(x + 1, y + 4, player_color.to_333());
      matrix.drawPixel(x + 4, y + 4, emu_color.to_333());
     
      matrix.drawPixel(x, y + 5, player_color.to_333());
      matrix.drawPixel(x + 1, y + 5, player_color.to_333());
      matrix.drawPixel(x + 2, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 3, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 4, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 5, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 6, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 7, y + 5, saber_color.to_333());

      matrix.drawPixel(x, y + 6, player_color.to_333());
      matrix.drawPixel(x + 1, y + 6, player_color.to_333());
      matrix.drawPixel(x + 4, y + 6, emu_color.to_333());

      matrix.drawPixel(x, y + 7, player_color.to_333());
      matrix.drawPixel(x + 1, y + 7, eye_color.to_333());
      matrix.drawPixel(x + 3, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 5, y + 7, emu_color.to_333());

      matrix.drawPixel(x, y + 8, player_color.to_333());
      matrix.drawPixel(x + 1, y + 8, player_color.to_333());
      matrix.drawPixel(x + 3, y + 8, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 8, emu_color.to_333());
    }

    void draw_right_base(Color emu_color, Color saber_color, Color player_color, Color eye_color){
      matrix.drawPixel(x + 4, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 5, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 6, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 7, y + 2, emu_color.to_333());

      matrix.drawPixel(x + 7, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 6, y + 3, player_color.to_333());
      matrix.drawPixel(x + 3, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 5, y + 3, emu_color.to_333());

      matrix.drawPixel(x + 6, y + 4, player_color.to_333());
      matrix.drawPixel(x + 7, y + 4, player_color.to_333());
      matrix.drawPixel(x + 3, y + 4, emu_color.to_333());
     
      matrix.drawPixel(x + 6, y + 5, player_color.to_333());
      matrix.drawPixel(x + 7, y + 5, player_color.to_333());
      matrix.drawPixel(x, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 1, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 2, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 3, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 4, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 5, y + 5, saber_color.to_333());

      matrix.drawPixel(x + 6, y + 6, player_color.to_333());
      matrix.drawPixel(x + 7, y + 6, player_color.to_333());
      matrix.drawPixel(x + 3, y + 6, emu_color.to_333());

      matrix.drawPixel(x + 7, y + 7, player_color.to_333());
      matrix.drawPixel(x + 6, y + 7, eye_color.to_333());
      matrix.drawPixel(x + 2, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 3, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 7, emu_color.to_333());

      matrix.drawPixel(x + 6, y + 8, player_color.to_333());
      matrix.drawPixel(x + 7, y + 8, player_color.to_333());
      matrix.drawPixel(x + 3, y + 8, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 8, emu_color.to_333());
     
    }

    void still_right_frame() {
      draw_right_base(BROWN, WHITE, main_color, YELLOW);
      //bottom
      matrix.drawPixel(x + 6, y, BROWN.to_333());
      matrix.drawPixel(x + 6, y + 1, BROWN.to_333());    
    }

    void still_left_frame() {
      draw_left_base(BROWN, WHITE, main_color, YELLOW);
      //bottom
      matrix.drawPixel(x + 1, y, BROWN.to_333());
      matrix.drawPixel(x + 1, y + 1, BROWN.to_333());    
    }

    void erase_right_frame() {
      draw_right_base(BLACK, BLACK, BLACK, BLACK);
      matrix.drawPixel(x + 4, y, BLACK.to_333());
      matrix.drawPixel(x + 5, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 6, y, BLACK.to_333());
      matrix.drawPixel(x + 6, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 5, y, BLACK.to_333());
      matrix.drawPixel(x + 7, y, BLACK.to_333());
      matrix.drawPixel(x + 6, y + 2, BLACK.to_333());
      matrix.drawPixel(x + 7, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 7, y + 2, BLACK.to_333());
      matrix.drawPixel(x + 5, y, BLACK.to_333());
    }

    void erase_left_frame() {
      draw_left_base(BLACK, BLACK, BLACK, BLACK);
      //bottom
      matrix.drawPixel(x + 1, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 1, BLACK.to_333());  
      matrix.drawPixel(x + 3, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 2, y + 1, BLACK.to_333());  
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 2, BLACK.to_333());
      matrix.drawPixel(x, y + 1, BLACK.to_333());
      matrix.drawPixel(x, y + 2, BLACK.to_333());
      matrix.drawPixel(x + 2, y, BLACK.to_333());
    }
   
    void walk_left_frame1() {
      draw_left_base(BROWN, WHITE, main_color, YELLOW);
      //bottom
      matrix.drawPixel(x + 1, y, BROWN.to_333());
      matrix.drawPixel(x + 3, y, BROWN.to_333());

      matrix.drawPixel(x + 1, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 2, y + 1, BROWN.to_333());    
    }

    void walk_left_frame2() {
      draw_left_base(BROWN, WHITE, main_color, YELLOW);
     
      //Draw wings
      matrix.drawPixel(x, y, BROWN.to_333());
      matrix.drawPixel(x + 2, y, BROWN.to_333());

      matrix.drawPixel(x + 1, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 2, y + 1, BROWN.to_333());      
    }

    void fly_left_frame1() {
      draw_left_base(BROWN, WHITE, main_color, YELLOW);
      
      //draw wings
      matrix.drawPixel(x, y + 2, BROWN.to_333());
      matrix.drawPixel(x, y + 3, BROWN.to_333());
      matrix.drawPixel(x + 1, y + 3, BROWN.to_333());
      matrix.drawPixel(x, y + 4, BROWN.to_333());  
      matrix.drawPixel(x, y + 5, BROWN.to_333());  
    }

    void fly_left_frame2() {
      draw_left_base(BROWN, WHITE, main_color, YELLOW);
     
      //Draw wings
      matrix.drawPixel(x, y, BROWN.to_333());
      matrix.drawPixel(x, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 1, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 2, y + 1, BROWN.to_333());
      matrix.drawPixel(x, y + 2, BROWN.to_333());
      matrix.drawPixel(x, y + 3, BROWN.to_333());        
    }

    void walk_right_frame1(){
      draw_right_base(BROWN, WHITE, main_color, YELLOW);

      matrix.drawPixel(x + 4, y, BROWN.to_333());
      matrix.drawPixel(x + 6, y, BROWN.to_333());

      matrix.drawPixel(x + 5, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 6, y + 1, BROWN.to_333());
    }

    void walk_right_frame2(){
      draw_right_base(BROWN, WHITE, main_color, YELLOW);

      matrix.drawPixel(x + 5, y, BROWN.to_333());
      matrix.drawPixel(x + 7, y, BROWN.to_333());

      matrix.drawPixel(x + 5, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 6, y + 1, BROWN.to_333());
    }

    void fly_right_frame1() {
      draw_right_base(BROWN, WHITE, main_color, YELLOW);
      
      //draw wings
      matrix.drawPixel(x + 7, y + 2, BROWN.to_333());
      matrix.drawPixel(x + 7, y + 3, BROWN.to_333());
      matrix.drawPixel(x + 6, y + 3, BROWN.to_333());
      matrix.drawPixel(x + 7, y + 4, BROWN.to_333());  
      matrix.drawPixel(x + 7, y + 5, BROWN.to_333());  
    }

    void fly_right_frame2() {
      draw_right_base(BROWN, WHITE, main_color, YELLOW);
     
      //Draw wings     
      matrix.drawPixel(x + 7, y, BROWN.to_333());
      matrix.drawPixel(x + 7, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 6, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 5, y + 1, BROWN.to_333());
      matrix.drawPixel(x + 7, y + 2, BROWN.to_333());
      matrix.drawPixel(x + 7, y + 3, BROWN.to_333());        
    }
};

class Enemy {
  public:
    Enemy() {
      x = X_DIMENSION/2;
      y = 1;
      facing_left = false;
      on_ground = true;
      alive = true;
      move_value = 500;
      still = true;
    }
    
    Enemy(Color c, int x_coord, int y_coord, bool facing_direction) {
      main_color = c;
      x = x_coord;
      y = y_coord;
      facing_left = facing_direction;
      on_ground = true;
      alive = true;
      still = true;
    }
   
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool get_status(){
      return alive;
    }
    void set_on_ground(bool b){
      on_ground = b;  
    }
    bool get_direction(){
      return facing_left;  
    }
   
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }

    void set_y(int y_arg) {
      y = y_arg;
    }

    void set_direction(bool b){
      facing_left = b;
    }
   
    void die() {
      alive = false;     
      erase_left_frame();
      erase_right_frame();             
    }

    void reset(){
      x = X_DIMENSION/2;
      y = 0;
      alive = true;  
    }
    
    void gravity(int frames){
      if(frames % 3 == 0){
        erase_left_frame();
        erase_right_frame();
        y--;
        if(y >= Y_DIMENSION){
          y = 1;
        }
        if(y < 0){
          y = Y_DIMENSION;
        }          
      }            
    }
    
    void fly(int frames){
      if(frames % 3 == 0){
        erase_left_frame();
        erase_right_frame();
        y += 4;
        on_ground = false;
      }                
    }
    void move(int frame){
      if(frame % 70 == 0){
         move_value = rand() % 1024;
      }  
      int mod = 1;
      //left
      if(move_value < 460){
        if(move_value < 200){
          mod = 2;
        }
        if(facing_left){
          erase_right_frame();
        }      
        facing_left = false;
        if(on_ground){
          if(frame%12==0){
            erase_left_frame();
            x += 1 * mod;
            walk_left_frame1();
          }
          else if(frame%6==0){
            erase_left_frame();
            x += 1 * mod;
            still_left_frame();
          }
          else if(frame%3==0){
            erase_left_frame();
            x += 1 * mod;
            walk_left_frame2();
          }
        }                          
        else if(!on_ground){
          if(frame%12==0){
            erase_left_frame();
            x += 1 * mod;
            fly_left_frame1();
          }
          else if(frame%6==0){
            erase_left_frame();
            x += 1 * mod;
            draw_left_base(RED, WHITE, main_color, ORANGE);
          }
          else if(frame%3==0){
            erase_left_frame();
            x += 1 * mod;
            fly_left_frame2();
          }
        }
      }
     //right
     else if(move_value > 600){
      if(move_value > 800){
        mod = 2;
      }
      if(on_ground){
        if(!facing_left){
          erase_left_frame();
        }    
        facing_left = true;
        if(frame%12==0){
          erase_right_frame();
          x -= 1 * mod;
          walk_right_frame1();
        }
        else if(frame%6==0){
          erase_right_frame();
          x -= 1 * mod;
          still_right_frame();
        }
        else if(frame%3==0){
          erase_right_frame();
          x -= 1 * mod;
          walk_right_frame2();
        }
      }
      else if(!on_ground){
          if(frame%12==0){
            erase_right_frame();
            x -= 1 * mod;
            fly_right_frame1();
          }
          else if(frame%6==0){
            erase_right_frame();
            x -= 1 * mod;
            draw_right_base(RED, WHITE, main_color, ORANGE);
          }
          else if(frame%3==0){
            erase_right_frame();
            x -= 1 * mod;
            fly_right_frame2();
          }
      }
     }
     //If potentiometer is neither left or right
     else{      
       if(facing_left){
        if(on_ground){
          if(!still){
            erase_right_frame();
          }
          still_right_frame();
        }
        else if(!on_ground){
          if(frame%12==0){
            erase_right_frame();
            fly_right_frame1();
          }
          else if(frame%6==0){
            erase_right_frame();
            draw_right_base(RED, WHITE, main_color, YELLOW);
          }
          else if(frame%3==0){
            erase_right_frame();
            fly_right_frame2();
          }
        }                
       }
       else{
         if(on_ground){
            if(!still){
              erase_left_frame();
            }
            still_left_frame();
          }
          else if(!on_ground){
            if(frame%12==0){
              erase_left_frame();
              fly_left_frame1();
            }
            else if(frame%6==0){
              erase_left_frame();
              draw_left_base(RED, WHITE, main_color, YELLOW);
            }
            else if(frame%3==0){
              erase_left_frame();
              fly_left_frame2();
            }
          }           
        }
        still = true;
      }
      //Loop around
      if(x < -7){
        erase_right_frame();
        x = X_DIMENSION;  
      }
      if(x > X_DIMENSION){
        x = 0;  
      }
    }
    
    void bounce(){
      if(facing_left){
        erase_right_frame();
        x += 10;
      }
      else{
        erase_left_frame();
        x -= 10;
      }  
    }
    
  private:
    int x;
    int y;
    int move_value;    
    bool alive;
    bool still;   
    bool on_ground;
    bool facing_left;    
    Color main_color;   

    //Draw fuctions
    void draw_left_base(Color emu_color, Color saber_color, Color enemy_color, Color eye_color){
      matrix.drawPixel(x, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 1, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 2, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 3, y + 2, emu_color.to_333());
      matrix.drawPixel(x, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 1, y + 3, enemy_color.to_333());
      matrix.drawPixel(x + 2, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 3, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 3, emu_color.to_333());
      matrix.drawPixel(x, y + 4, enemy_color.to_333());
      matrix.drawPixel(x + 1, y + 4, enemy_color.to_333());
      matrix.drawPixel(x + 4, y + 4, emu_color.to_333());
     
      matrix.drawPixel(x, y + 5, enemy_color.to_333());
      matrix.drawPixel(x + 1, y + 5, enemy_color.to_333());
      matrix.drawPixel(x + 2, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 3, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 4, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 5, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 6, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 7, y + 5, saber_color.to_333());
      matrix.drawPixel(x, y + 6, enemy_color.to_333());
      matrix.drawPixel(x + 1, y + 6, enemy_color.to_333());
      matrix.drawPixel(x + 4, y + 6, emu_color.to_333());
      matrix.drawPixel(x, y + 7, enemy_color.to_333());
      matrix.drawPixel(x + 1, y + 7, eye_color.to_333());
      matrix.drawPixel(x + 3, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 5, y + 7, emu_color.to_333());
      matrix.drawPixel(x, y + 8, enemy_color.to_333());
      matrix.drawPixel(x + 1, y + 8, enemy_color.to_333());
      matrix.drawPixel(x + 3, y + 8, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 8, emu_color.to_333());
    }
    void draw_right_base(Color emu_color, Color saber_color, Color enemy_color, Color eye_color){
      matrix.drawPixel(x + 4, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 5, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 6, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 7, y + 2, emu_color.to_333());
      matrix.drawPixel(x + 7, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 6, y + 3, enemy_color.to_333());
      matrix.drawPixel(x + 3, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 5, y + 3, emu_color.to_333());
      matrix.drawPixel(x + 6, y + 4, enemy_color.to_333());
      matrix.drawPixel(x + 7, y + 4, enemy_color.to_333());
      matrix.drawPixel(x + 3, y + 4, emu_color.to_333());
     
      matrix.drawPixel(x + 6, y + 5, enemy_color.to_333());
      matrix.drawPixel(x + 7, y + 5, enemy_color.to_333());
      matrix.drawPixel(x, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 1, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 2, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 3, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 4, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 5, y + 5, saber_color.to_333());
      matrix.drawPixel(x + 6, y + 6, enemy_color.to_333());
      matrix.drawPixel(x + 7, y + 6, enemy_color.to_333());
      matrix.drawPixel(x + 3, y + 6, emu_color.to_333());
      matrix.drawPixel(x + 7, y + 7, enemy_color.to_333());
      matrix.drawPixel(x + 6, y + 7, eye_color.to_333());
      matrix.drawPixel(x + 2, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 3, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 7, emu_color.to_333());
      matrix.drawPixel(x + 6, y + 8, enemy_color.to_333());
      matrix.drawPixel(x + 7, y + 8, enemy_color.to_333());
      matrix.drawPixel(x + 3, y + 8, emu_color.to_333());
      matrix.drawPixel(x + 4, y + 8, emu_color.to_333());     
    }
    
    void still_right_frame() {
      draw_right_base(RED, WHITE, main_color, ORANGE);
      //bottom
      matrix.drawPixel(x + 6, y, RED.to_333());
      matrix.drawPixel(x + 6, y + 1, RED.to_333());    
    }
    
    void still_left_frame() {
      draw_left_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x + 1, y, RED.to_333());
      matrix.drawPixel(x + 1, y + 1, RED.to_333());    
    }
    
    void erase_right_frame() {
      draw_right_base(BLACK, BLACK, BLACK, BLACK);
      matrix.drawPixel(x + 4, y, BLACK.to_333());
      matrix.drawPixel(x + 5, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 6, y, BLACK.to_333());
      matrix.drawPixel(x + 6, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 5, y, BLACK.to_333());
      matrix.drawPixel(x + 7, y, BLACK.to_333());
      matrix.drawPixel(x + 6, y + 2, BLACK.to_333());
      matrix.drawPixel(x + 7, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 7, y + 2, BLACK.to_333());
      matrix.drawPixel(x + 5, y, BLACK.to_333());
    }
    void erase_left_frame() {
      draw_left_base(BLACK, BLACK, BLACK, BLACK);
      matrix.drawPixel(x + 1, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 1, BLACK.to_333());  
      matrix.drawPixel(x + 3, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 1, BLACK.to_333());
      matrix.drawPixel(x + 2, y + 1, BLACK.to_333());  
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 2, BLACK.to_333());
      matrix.drawPixel(x, y + 1, BLACK.to_333());
      matrix.drawPixel(x, y + 2, BLACK.to_333());
      matrix.drawPixel(x + 2, y, BLACK.to_333());
    }
   
    // draws the player
    void walk_left_frame1() {
      draw_left_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x + 1, y, RED.to_333());
      matrix.drawPixel(x + 3, y, RED.to_333());
      matrix.drawPixel(x + 1, y + 1, RED.to_333());
      matrix.drawPixel(x + 2, y + 1, RED.to_333());    
    }
    void walk_left_frame2() {
      draw_left_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x, y, RED.to_333());
      matrix.drawPixel(x + 2, y, RED.to_333());
      matrix.drawPixel(x + 1, y + 1, RED.to_333());
      matrix.drawPixel(x + 2, y + 1, RED.to_333());      
    }
    void fly_left_frame1() {
      draw_left_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x, y + 2, RED.to_333());
      matrix.drawPixel(x, y + 3, RED.to_333());
      matrix.drawPixel(x + 1, y + 3, RED.to_333());
      matrix.drawPixel(x, y + 4, RED.to_333());  
      matrix.drawPixel(x, y + 5, RED.to_333());  
    }
    void fly_left_frame2() {
      draw_left_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x, y, RED.to_333());
      matrix.drawPixel(x, y + 1, RED.to_333());
      matrix.drawPixel(x + 1, y + 1, RED.to_333());
      matrix.drawPixel(x + 2, y + 1, RED.to_333());
      matrix.drawPixel(x, y + 2, RED.to_333());
      matrix.drawPixel(x, y + 3, RED.to_333());        
    }
    void walk_right_frame1(){
      draw_right_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x + 4, y, RED.to_333());
      matrix.drawPixel(x + 6, y, RED.to_333());
      matrix.drawPixel(x + 5, y + 1, RED.to_333());
      matrix.drawPixel(x + 6, y + 1, RED.to_333());
    }
    void walk_right_frame2(){
      draw_right_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x + 5, y, RED.to_333());
      matrix.drawPixel(x + 7, y, RED.to_333());
      matrix.drawPixel(x + 5, y + 1, RED.to_333());
      matrix.drawPixel(x + 6, y + 1, RED.to_333());
    }
    void fly_right_frame1() {
      draw_right_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x + 7, y + 2, RED.to_333());
      matrix.drawPixel(x + 7, y + 3, RED.to_333());
      matrix.drawPixel(x + 6, y + 3, RED.to_333());
      matrix.drawPixel(x + 7, y + 4, RED.to_333());  
      matrix.drawPixel(x + 7, y + 5, RED.to_333());  
    }
    void fly_right_frame2() {
      draw_right_base(RED, WHITE, main_color, ORANGE);
      matrix.drawPixel(x + 7, y, RED.to_333());
      matrix.drawPixel(x + 7, y + 1, RED.to_333());
      matrix.drawPixel(x + 6, y + 1, RED.to_333());
      matrix.drawPixel(x + 5, y + 1, RED.to_333());
      matrix.drawPixel(x + 7, y + 2, RED.to_333());
      matrix.drawPixel(x + 7, y + 3, RED.to_333());        
    }
};

class Game {
  public:
    Game() {
      score = 0;
      high_score = 0;
      num_enemies = 3;
      wave = 1;
      level = 1;
      single_player = false;
      versus_player = false;
      enter_bluetooth = false;
      enter_menu = false;
      bluetooth_mode = false;
    }

    //Modes
    //Title Screen and animation
    void title_screen(bool button_pressed){
      matrix.setRotation(2);
      if(frames == 1){
        player_1.set_x(-8);
        player_1.set_y(1);
        matrix.fillScreen(BLACK.to_333());
      }    
      matrix.setTextSize(2);
      matrix.setCursor(3, 6);
      matrix.print("JOUST");
      matrix.setRotation(4);       
      player_1.move(700, frames);
      frames++;
      for(int i = 0; i < X_DIMENSION; i++){
        matrix.drawPixel(i, 0, PURPLE.to_333());
      }  
      if(button_pressed){
        frames = 0;
        enter_menu = true;
        matrix.fillScreen(BLACK.to_333());
        delay(500);
      }         
    }

    //Game Mode select
    void menu(int potentiometer_value, bool button_pressed){
      matrix.setRotation(2);
      matrix.setTextSize(1);
       
      matrix.setCursor(8, 6);
      matrix.print("2 Player");

      matrix.setCursor(8, 20);
      matrix.print("1 Player");

      if(potentiometer_value < 512){
        matrix.drawRect(5, Y_DIMENSION/2 - 12, X_DIMENSION - 10, 11, YELLOW.to_333()); 
        matrix.drawRect(5, Y_DIMENSION - 14, X_DIMENSION - 10, 11, BLUE.to_333());
        if(button_pressed){        
          versus_player = true;
          matrix.setRotation(4);
          matrix.fillScreen(BLACK.to_333());
          delay(750);
        }
      }
      else{
        matrix.drawRect(5, Y_DIMENSION/2 - 12, X_DIMENSION - 10, 11, BLUE.to_333()); 
        matrix.drawRect(5, Y_DIMENSION - 14, X_DIMENSION - 10, 11, YELLOW.to_333());
        if(button_pressed){          
          enter_bluetooth = true;
          single_player = true;
          matrix.setRotation(4);
          matrix.fillScreen(BLACK.to_333());
          delay(750);
        }
      }
    }

    void bluetooth_mode_choice(int potentiometer_value, bool button_pressed){
      matrix.setRotation(2);
      matrix.setTextSize(1);
       
      matrix.setCursor(6, 6);
      matrix.print("Bluetooth");

      matrix.setCursor(12, 20);
      matrix.print("Classic");

      if(potentiometer_value < 512){
        matrix.drawRect(4, Y_DIMENSION/2 - 12, X_DIMENSION - 7, 10, YELLOW.to_333()); 
        matrix.drawRect(5, Y_DIMENSION - 14, X_DIMENSION - 10, 10, BLUE.to_333());
        if(button_pressed){
          bluetooth_mode = true;
          single_player = true;
          enter_bluetooth = false;
          matrix.setRotation(4);
          matrix.fillScreen(BLACK.to_333());
        }
      }
      else{
        matrix.drawRect(4, Y_DIMENSION/2 - 12, X_DIMENSION - 7, 10, BLUE.to_333()); 
        matrix.drawRect(5, Y_DIMENSION - 14, X_DIMENSION - 10, 10, YELLOW.to_333());
        if(button_pressed){
          bluetooth_mode = false;
          single_player = true;
          enter_bluetooth = false;
          matrix.setRotation(4);
          matrix.fillScreen(BLACK.to_333());          
        }
      }
    }


    //PvP Mode
    void versus(int potentiometer_value, bool button_pressed, int potentiometer_value_2, bool button_pressed_2) {
      //Runs for the first round
      if(player_1.get_lives() == 3 && player_2.get_lives() == 3 && frames == 0){
        print_lives();
        countdown(); 
        set_player_positions();
      }
      //Runs for all other rounds
      else if(frames == 0){
        print_lives();
        set_player_positions();
      }
      
      //Game will update every 0.02 seconds
      delay(20);
      if(level == 1){
        platform.draw_level_1();
      }
      if(level == 2){
        platform.draw_level_2();
      }
      if(level == 3){
        platform.draw_level_3();
      }
      
      frames++;
      
      versus_collisions();    
       
      //Player 1
      if(button_pressed){
        player_1.fly(frames);
      }
      if(!player_platform_collision(player_1)){
        player_1.gravity(frames);
      }
      else{
        player_1.set_on_ground(true);
      }                              
      player_1.move(potentiometer_value, frames);

      //Player 2
      if(button_pressed_2){
        player_2.fly(frames);
      }      
      if(!player_platform_collision(player_2)){
        player_2.gravity(frames);
      }
      else{
        player_2.set_on_ground(true);
      }     
      player_2.move(potentiometer_value_2, frames);            
    }

    //VS AI Mode
    void classic(int potentiometer_value, bool button_pressed) {
      player_2.player_null();
      if(player_1.get_lives() == 3 && frames == 0){
        print_lives();
        countdown(); 
      }
      else if(frames == 0){
        print_lives();
      }     
      if(level == 1){
        platform.draw_level_1();
      }
      if(level == 2){
        platform.draw_level_2();
      }
      if(level == 3){
        platform.draw_level_3();
      }
      
      frames++;

      //Player 1
      if(button_pressed){
          player_1.fly(frames);
      }   
      if(!player_platform_collision(player_1)){
        player_1.gravity(frames);
      }
      else{
        player_1.set_on_ground(true);
      }
      if(!bluetooth_mode){   
        player_1.move(potentiometer_value, frames); 
        num_enemies = 3;
      }

      if(bluetooth_mode){        
        player_1.move_bluetooth(potentiometer_value, frames);   
        num_enemies = 1;  
      }
                    
      //Computer controlled enemies
      int num_dead = 0;     
      for(int i = 0; i < num_enemies; i++){
        if(enemies[i].get_status()){
          int buttonVal;
          buttonVal = rand() % 4;
          if(buttonVal == 1){
            enemies[i].fly(frames);
          }
          if(!enemy_platform_collision(enemies[i])){
            enemies[i].gravity(frames);
          }
          else{
            enemies[i].set_on_ground(true);
          }
          enemies[i].move(frames);
          if(sword_sword_enemy_collision(enemies[i], player_1)){
            player_1.bounce();
            enemies[i].bounce();
          }
        }
        if(!enemies[i].get_status()){
         num_dead++;
        }       
      }
                                                      
      

      if(num_dead == num_enemies){
        wave++;
        level = rand() % 3 + 1;
        setup_wave();
        print_waves();
      }

      int in = 20;
      if(num_dead == 1 && !bluetooth_mode){
        in = 15;
      }
      else if(num_dead == 0 && !bluetooth_mode){
        in = 10;
      }
      player_favored_collisions();
      enemy_favored_collisions();

      delay(in);
      
    }    

    bool get_single_player_status(){
      return single_player;  
    }

    bool get_versus_player_status(){
      return versus_player;  
    }

    bool get_enter_menu(){
      return enter_menu;  
    }

    bool get_bluetooth_decision(){
      return enter_bluetooth;  
    }

    bool get_bluetooth_mode(){
      return bluetooth_mode;  
    }

    void set_frames(int x){
      frames = x;  
    }

    

  private:
    int level;
    int frames;
    int high_score;
    int score;
    int num_enemies;
    int wave;    
    bool single_player;
    bool versus_player;
    bool enter_menu;
    bool bluetooth_mode;
    bool enter_bluetooth;
    
    Platform platform;
    Player player_1 = {BLUE, X_DIMENSION - X_DIMENSION/4, 1, true};
    Player player_2 = {RED, X_DIMENSION/4, 1, false};
    Enemy enemy1 = {GREEN, X_DIMENSION/4, 1, false};
    Enemy enemy2 = {YELLOW, X_DIMENSION/2, 1, false};
    Enemy enemy3 = {BROWN, X_DIMENSION/4, Y_DIMENSION/2 + 1, false};
    Enemy enemies[3] = {enemy1, enemy2, enemy3};

    //Collisions
    void versus_collisions(){
      //red kills blue
      if(player_opposite_directions_collision(player_1, player_2)){
        level = rand() % 3 + 1;
        frames = 0;
        player_1.die();  
        delay(500); 
      }
      //blue kills red
      else if(player_opposite_directions_collision(player_2, player_1)){
        level = rand() % 3 + 1;
        frames = 0;
        player_2.die();  
        delay(500);  
      }

      
      else if(player_same_directions_collision(player_2, player_1)){
        level = rand() % 3 + 1;
        frames = 0;
        player_2.die();  
        delay(500);  
      }
      else if(player_same_directions_collision(player_1, player_2)){
        level = rand() % 3 + 1;
        frames = 0;
        player_1.die();  
        delay(500);  
      }
      
      
      else if(sword_sword_collision(player_1, player_2)){
        player_1.bounce();
        player_2.bounce();  
      }
    }

    void enemy_favored_collisions(){
        for(int i = 0; i < num_enemies; i++){
          if(enemies[i].get_status() && (player_enemy_same_directions_collision(player_1, enemies[i]) || player_enemy_opposite_directions_collision(player_1, enemies[i]))){
            frames = 0;
            player_1.die();
            delay(500);
            setup_wave(); 
          }       
        }
      }
     void player_favored_collisions(){
        for(int i = 0; i < num_enemies; i++){
          if(enemies[i].get_status() && (enemy_player_same_directions_collision(enemies[i], player_1) || enemy_player_opposite_directions_collision(enemies[i], player_1))){
            enemies[i].die();
            score++;
          }       
        }
      }

    bool player_opposite_directions_collision(Player p1, Player p2){
      int x1 = p1.get_x();
      int x2 = p2.get_x();
      
      int y1 = p1.get_y();
      int y2 = p2.get_y();
      
      if(abs(y2 - y1) < 8){
        if((p1.get_direction() != p2.get_direction())){
          if(abs(x2 - x1) < 6){
             if(y2 > y1){
                return true;
              }
           }
        }
      }
      return false;
    }

    bool player_same_directions_collision(Player p1, Player p2){
      int x1 = p1.get_x();
      int x2 = p2.get_x();
      
      int y1 = p1.get_y();
      int y2 = p2.get_y();
      
      if(abs(y2 - y1) < 8){
        if(p1.get_direction() == p2.get_direction()){
           if(p2.get_direction()){
            if(x2 > x1){
              if(abs(x2 - x1) < 8){
                 return true;
              }
            } 
           }
           if(!p2.get_direction()){
            if(x2 < x1){
              if(abs(x2 - x1) < 8){
                 return true;
              }
            } 
           }
        }
      }
      return false;
    }

    bool sword_sword_collision(Player p1, Player p2){
      int p1X = p1.get_x();
      int p2X = p2.get_x();
      if(p1.get_direction() != p2.get_direction()){
        if(p1.get_direction() == true){
          if((p1.get_y() + 5 == p2.get_y() + 5) && ((p1X + 1 == p2X + 6) || (p1X == p2X + 7) || (p1X + 2 == p2X + 7) || (p1X + 3 == p2X + 7)))
          {
            return true; 
          }
        }
        else if(p1.get_direction() == false){
          if((p1.get_y() + 5 == p2.get_y() + 5) && ((p1X + 4 == p2X + 7) || (p1X + 5 == p2X + 7) || (p1X + 6 == p2X + 1) || (p1X + 7 == p2X)))
          {
            return true; 
          }
        }
      }
        return false;
    }

    bool sword_sword_enemy_collision(Enemy p1, Player p2){
      int p1X = p1.get_x();
      int p2X = p2.get_x();
      if(p1.get_direction() != p2.get_direction()){
        if(p1.get_direction() == true){
          if((p1.get_y() + 5 == p2.get_y() + 5) && ((p1X + 1 == p2X + 6) || (p1X == p2X + 7) || (p1X + 2 == p2X + 7) || (p1X + 3 == p2X + 7))){
            return true; 
          }
        }
        else if(p1.get_direction() == false){
          if((p1.get_y() + 5 == p2.get_y() + 5) && ((p1X + 4 == p2X + 7) || (p1X + 5 == p2X + 7) || (p1X + 6 == p2X + 1) || (p1X + 7 == p2X))){
            return true; 
          }
        }
      }
        return false;
    }
    
    bool enemy_player_same_directions_collision(Enemy p1, Player p2){
      int x1 = p1.get_x();
      int x2 = p2.get_x();
      
      int y1 = p1.get_y();
      int y2 = p2.get_y();
      
      if(abs(y2 - y1) < 8){
        if(p1.get_direction() == p2.get_direction()){
           if(p2.get_direction()){
            if(x2 > x1){
              if(abs(x2 - x1) < 8){
                 return true;
              }
            } 
           }
           if(!p2.get_direction()){
            if(x2 < x1){
              if(abs(x2 - x1) < 8){
                 return true;
              }
            } 
           }
        }
      }
      return false;
    }

    bool player_enemy_same_directions_collision(Player p1, Enemy p2){
      int x1 = p1.get_x();
      int x2 = p2.get_x();
      
      int y1 = p1.get_y();
      int y2 = p2.get_y();
      
      if(abs(y2 - y1) < 8){
        if(p1.get_direction() == p2.get_direction()){
           if(p2.get_direction()){
            if(x2 > x1){
              if(abs(x2 - x1) < 8){
                 return true;
              }
            } 
           }
           if(!p2.get_direction()){
            if(x2 < x1){
              if(abs(x2 - x1) < 8){
                 return true;
              }
            } 
           }
        }
      }
      return false;
    }

    bool player_enemy_opposite_directions_collision(Player p1, Enemy p2){
      int x1 = p1.get_x();
      int x2 = p2.get_x();
      
      int y1 = p1.get_y();
      int y2 = p2.get_y();
      
      if(abs(y2 - y1) < 8){
        if((p1.get_direction() != p2.get_direction())){
          if(abs(x2 - x1) < 6){
             if(y2 > y1){
                return true;
              }
           }
        }
      }
      return false;
    }

    bool enemy_player_opposite_directions_collision(Enemy p1, Player p2){
      int x1 = p1.get_x();
      int x2 = p2.get_x();
      
      int y1 = p1.get_y();
      int y2 = p2.get_y();
      
      if(abs(y2 - y1) < 8){
        if((p1.get_direction() != p2.get_direction())){
          if(abs(x2 - x1) < 6){
             if(y2 > y1){
                return true;
              }
           }
        }
      }
      return false;
    }

    bool player_platform_collision(Player p){
      int playerX = p.get_x();
      int playerY = p.get_y();
      //Left
      if(p.get_direction()){
        if(platform.is_part_of_platform(playerX + 1, playerY - 1) || platform.is_part_of_platform(playerX + 2, playerY - 1) 
        || platform.is_part_of_platform(playerX + 3, playerY - 1) || platform.is_part_of_platform(playerX + 4, playerY - 1)){
          return true;  
        }
      }
      //Right
      else{
        if(platform.is_part_of_platform(playerX + 4, playerY - 1) || platform.is_part_of_platform(playerX + 5, playerY - 1) 
        || platform.is_part_of_platform(playerX + 6, playerY - 1) || platform.is_part_of_platform(playerX + 7, playerY - 1)){
          return true;  
        }
      }
       
      return false;
    }

    bool enemy_platform_collision(Enemy e){
      int enemyX = e.get_x();
      int enemyY = e.get_y();
      //Left
      if(e.get_direction()){
        if(platform.is_part_of_platform(enemyX + 1, enemyY - 1) || platform.is_part_of_platform(enemyX + 2, enemyY - 1)
        || platform.is_part_of_platform(enemyX + 3, enemyY - 1) || platform.is_part_of_platform(enemyX + 4, enemyY - 1)){
          return true;  
        }
      }
      //Right
      else{
        if(platform.is_part_of_platform(enemyX + 4, enemyY - 1) || platform.is_part_of_platform(enemyX + 5, enemyY - 1)
        || platform.is_part_of_platform(enemyX + 6, enemyY - 1) || platform.is_part_of_platform(enemyX + 7, enemyY - 1)){
          return true;  
        }
      }
       
      return false;
    }

    //Non-Collision gameplay functions
    void print_waves(){
      matrix.setRotation(2);
      matrix.setTextSize(2);
      matrix.fillScreen(BLACK.to_333());
      matrix.setCursor(8, 1);
      matrix.setTextColor(RED.to_333()); 
      matrix.print("WAVE");           
      matrix.setCursor(X_DIMENSION/2 - 5, Y_DIMENSION/2 + 1);
      matrix.setTextColor(WHITE.to_333()); 
      matrix.print(wave);
      delay(1500);
      matrix.setRotation(4);
      matrix.fillScreen(BLACK.to_333());
    }

    //If both players still have at least 1 life, print out their lives
    void print_lives(){
        if(!win_check()){
          matrix.setRotation(2);
          matrix.setTextSize(2);
          matrix.fillScreen(BLACK.to_333());
          matrix.setCursor(8, 0);
          matrix.setTextColor(BLUE.to_333());         
          matrix.print("BLUE");
          matrix.setCursor(3, Y_DIMENSION/2);
          matrix.setTextColor(YELLOW.to_333());  
          matrix.print("LIVES");          
          delay(1500);
          matrix.fillScreen(BLACK.to_333());
          matrix.setCursor(15, Y_DIMENSION/2 - 10);
          matrix.drawRect(12, Y_DIMENSION/2 - 10, 40, 16, YELLOW.to_333()); 
          matrix.setTextColor(BLUE.to_333());
          for(int i = 0; i < player_1.get_lives(); i++){
             matrix.print("\3");
          }
          delay(1500);

          //Only runs for versus mode
          if(player_2.get_lives() != -1){
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(15, 0);
            matrix.setTextColor(RED.to_333());
            matrix.print("RED");
            matrix.setCursor(3, Y_DIMENSION/2);
            matrix.setTextColor(WHITE.to_333());  
            matrix.print("LIVES");
            delay(1500);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(15, Y_DIMENSION/2 - 10);
            matrix.drawRect(12, Y_DIMENSION/2 - 10, 40, 16, WHITE.to_333()); 
            matrix.setTextColor(RED.to_333());
            for(int i = 0; i < player_2.get_lives(); i++){
               matrix.print("\3");
            }
            delay(1500);
          }
          matrix.fillScreen(BLACK.to_333());
          matrix.setRotation(4);
        }       
    }

    //Resets enemies and player positions
    void setup_wave(){
      for(int i = 0; i < 3; i++){
        enemies[i].reset();
      }      
      set_player_positions();
    }

    //Counts down to game start
    void countdown(){
      matrix.setRotation(2);
      matrix.setTextSize(2);
      matrix.fillScreen(BLACK.to_333());
      matrix.setCursor(3, Y_DIMENSION/2 - 8);
      matrix.setTextColor(RED.to_333()); 
      matrix.print("READY");           
      delay(2000);
      matrix.setCursor(15, Y_DIMENSION/2 - 5);
      matrix.fillScreen(BLACK.to_333()); 
      matrix.setTextColor(BLUE.to_333());
      matrix.drawCircle(X_DIMENSION/2, Y_DIMENSION/2, 20, YELLOW.to_333()); 
      matrix.print("\\3/");
      delay(900);
      matrix.setCursor(15, Y_DIMENSION/2 - 5); 
      matrix.fillScreen(BLACK.to_333());
      matrix.setTextColor(YELLOW.to_333());
      matrix.drawRect(15, 10, 34, 16, BLUE.to_333()); 
      matrix.print("-2-");
      delay(700);
      matrix.setCursor(15, Y_DIMENSION/2 - 5); 
      matrix.fillScreen(BLACK.to_333());
      matrix.setTextColor(GREEN.to_333());
      matrix.drawLine(0, Y_DIMENSION, X_DIMENSION/2, 0, PURPLE.to_333()); 
      matrix.drawLine(X_DIMENSION, Y_DIMENSION, X_DIMENSION/2, 0, PURPLE.to_333()); 
      matrix.print("/1\\");
      delay(500);
      matrix.setCursor(4, Y_DIMENSION/2 - 8); 
      matrix.fillScreen(BLACK.to_333());
      matrix.setTextColor(WHITE.to_333()); 
      matrix.print("DUEL!");
      delay(750);    
      matrix.setRotation(4);
      matrix.fillScreen(BLACK.to_333());
    }

    //Pre-round function calls
    void start_level() {
      frames = 0;
      print_lives();          
      if(level == 1){
        platform.draw_level_1();
      }
      if(level == 2){
        platform.draw_level_2();
      }
      if(level == 3){
        platform.draw_level_3();
      }
    }

    //For Versus:
    //Checks if either player is out of lives
    //If so, prints winner to screen
    //For Classic:
    //Checks if player has lives, if not, print high score and/or score
    bool win_check() {
      if(player_1.get_lives() == 0){
        //Single Player
        if(player_2.get_lives() == -1){
          matrix.setRotation(2);
          matrix.fillScreen(BLACK.to_333());
          matrix.setCursor(8, 0);
          matrix.setTextColor(WHITE.to_333());
          matrix.print("GAME");
          matrix.setCursor(8, Y_DIMENSION/2);
          matrix.print("OVER");           
          delay(3000);         
          matrix.setTextColor(YELLOW.to_333());  
          matrix.fillScreen(BLACK.to_333());         
          if(score > high_score){
            high_score = score;           
            matrix.setCursor(15, 5);           
            matrix.print("NEW");  
            delay(1000);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(8, 5);
            matrix.print("HIGH");  
            delay(1000);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(3, 5);
            matrix.print("SCORE");  
            delay(1000);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(X_DIMENSION/2 - 10, Y_DIMENSION/2 - 10);
            matrix.print(score);  
            delay(2500); 
          }
          else{
            matrix.setCursor(3, 5);
            matrix.print("SCORE");  
            delay(1500);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(X_DIMENSION/2 - 10, Y_DIMENSION/2 - 10);
            matrix.print(score);  
            delay(2500); 
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(8, 5);
            matrix.print("HIGH");  
            delay(1000);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(3, 5);
            matrix.print("SCORE");  
            delay(1000);
            matrix.fillScreen(BLACK.to_333());
            matrix.setCursor(X_DIMENSION/2 - 10, Y_DIMENSION/2 - 10);
            matrix.print(high_score);  
            delay(1000);
          }
          
          
          matrix.setRotation(4);
          reset_game();
          return true;
        }
        //Versus
        else{
          matrix.setRotation(2);
          matrix.fillScreen(BLACK.to_333());
          matrix.setCursor(14, 0);
          matrix.setTextColor(RED.to_333());
          matrix.print("RED");  
          matrix.setCursor(8, Y_DIMENSION/2);
          matrix.setTextColor(WHITE.to_333());
          matrix.print("WINS");            
          delay(6000);
          matrix.setRotation(4);
          reset_game();
          return true;
        }        
      }

      else if(player_2.get_lives() == 0){
        matrix.setRotation(2);
        matrix.fillScreen(BLACK.to_333());
        matrix.setCursor(10, 0);
        matrix.setTextColor(BLUE.to_333());
        matrix.print("BLUE");  
        matrix.setCursor(10, Y_DIMENSION/2);
        matrix.setTextColor(YELLOW.to_333());
        matrix.print("WINS");          
        delay(6000);
        matrix.setRotation(4);
        reset_game();
        return true;
      }
      
      return false;      
    }

    //Resets everything (except high score) and makes return to title
    void reset_game(){
        score = 0;
        level = 1;
        wave = 1;
        player_1.reset_lives();
        player_2.reset_lives();
        versus_player = false;
        single_player = false;
        enter_menu = false;
        bluetooth_mode = false;
        matrix.setTextColor(WHITE.to_333());
    } 

    //Places players/enemies based on level
    void set_player_positions(){
     if(level == 1){
         player_1.set_x(X_DIMENSION - X_DIMENSION/4);        
         player_1.set_y(1);
         player_1.set_direction(true);
         if(player_2.get_lives() == -1){
          enemies[0].set_x(X_DIMENSION/4);                 
          enemies[0].set_y(1);
          enemies[0].set_direction(false);

          enemies[1].set_x(X_DIMENSION/2);                 
          enemies[1].set_y(1);
          enemies[1].set_direction(false);

          enemies[2].set_x(X_DIMENSION/4);                 
          enemies[2].set_y(2);
          enemies[2].set_direction(false);
         }
         else{
           player_2.set_y(1);
           player_2.set_x(X_DIMENSION/4);
           player_2.set_direction(false);
         }
     }
     else if(level == 2){
         player_1.set_x(X_DIMENSION - X_DIMENSION/3);         
         player_1.set_y(Y_DIMENSION/2 + 1);
         player_1.set_direction(true);
         if(player_2.get_lives() == -1){
          
         }
         else{
           player_2.set_y(Y_DIMENSION/2 + 1);
           player_2.set_x(X_DIMENSION/3);
           player_2.set_direction(false);
         }                 
     }
     else if(level == 3){
         player_1.set_x(X_DIMENSION - X_DIMENSION/4);                 
         player_1.set_y(Y_DIMENSION/2 + 1);
         player_1.set_direction(true);
         if(player_2.get_lives() == -1){         
          enemies[0].set_x(X_DIMENSION/4 - 5);                 
          enemies[0].set_y(Y_DIMENSION/2 + 1);
          enemies[0].set_direction(false);

          enemies[1].set_x(X_DIMENSION/2 - 5);                 
          enemies[1].set_y(Y_DIMENSION/2 + 1);
          enemies[1].set_direction(false);

          enemies[2].set_x(X_DIMENSION/4 - 5);                 
          enemies[2].set_y(8);
          enemies[2].set_direction(false);
         }
         else{
           player_2.set_y(Y_DIMENSION/2 + 1);
           player_2.set_x(X_DIMENSION/4 - 5);         
           player_2.set_direction(false);
         }
     }  
   }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(115200);  
  bluetooth.begin(115200); 
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  int potentiometer_value_2 = analogRead(POTENTIOMETER_PIN_NUMBER_2);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  bool button_pressed_2 = (digitalRead(BUTTON_PIN_NUMBER_2) == HIGH);
  
  if(!game.get_enter_menu()){
    game.title_screen(button_pressed_2);
  }
  else if(!game.get_versus_player_status() && !game.get_single_player_status()){   
    game.menu(potentiometer_value_2, button_pressed_2);
    game.set_frames(0);
  }
  else if(game.get_bluetooth_decision()){
    game.bluetooth_mode_choice(potentiometer_value_2, button_pressed_2);
  }
  else if(game.get_versus_player_status()){    
    game.versus(potentiometer_value_2, button_pressed_2, potentiometer_value, button_pressed);
  }
  else if(game.get_single_player_status()){
    if(game.get_bluetooth_mode()){
      for(int i = 0; i < 2; i++){
        char in = (char)bluetooth.read();      
        int input = in;
        
        input -= 48;
             
        if(input > 0 && input < 9){
          store_value(input);
        }  
        else if(input == 9){
          jump_blue = true;
        }     
        else if(input == 0){
          jump_blue = false;
        }    
      }
      potentiometer_value_2 = retrieve_value();
      button_pressed_2 == jump_blue;
    }
    else{
      jump_blue = button_pressed_2;  
    }
    game.classic(potentiometer_value_2, jump_blue);
  }
  
}

void store_value(int v){
  stored_value = v;  
}

void store_jump(bool v){
  jump_blue = v;  
}

int retrieve_value(){
  return stored_value;  
}

bool retrieve_jump(){
  return jump_blue;  
}
