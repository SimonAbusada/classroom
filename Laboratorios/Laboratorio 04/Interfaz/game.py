'''
Project name:  
    Juego laberinto controlado por microcontolador con interfaz en computador
Author: 
    Agustín Macaya
    Felipe Sanchez

Descripción: 
    Juego de laberinto contolado por un microcontrolador. 
    La idea es llevar el pundo del inicio al final sin tocar las murallas
    Si toca las murallas se envía una señal al microcontrolador
    EL movimientto es controlado por un microcontolador que manda la velocidad
    de movimiento dX y dY por serial al computador
    Los valores son recibidos de la forma (por ejemplo): 'X','1','2','0','Y','0','5','5'
    EL ejemplo anterior represente un a velocidad en X = 120 y en Y = 55
    EL juego tiene una velocidad de 30 frames por segundo.


Referencias: 
    Comunicación serial de Racoon
    CollideRect - https://inventwithpython.com/invent4thed/chapter19.html
    Mazes - http://www.mazegenerator.net/
    Buttons - https://www.geeksforgeeks.org/how-to-create-buttons-in-a-game-using-pygame/
    
'''



import sys
import warnings
import serial 
import serial.tools.list_ports
import random
from time import sleep, time
import pygame as pg
from pygame.locals import * 
import matplotlib.image as mpimg

class Comunicacion():
    def __init__(self, baudrate, puerto = None):
        self.baudrate = baudrate
        self.puerto = puerto
        
        if self.puerto:           
            self.ser = serial.Serial(self.puerto, baudrate = self.baudrate, timeout = 10)
        else:
            self.find_port()
                

    def send_value(self, valor, mostrar = False):
        '''
        valor puede ser único o lista
        '''
        valor_byte = bytes(valor)
        if mostrar:
            print(valor_byte) 

        self.ser.write(valor_byte)

    def recieve_value_ascii(self):
        message = self.ser.read(1)
        for i in message:
            return int(i)
        
    def find_port(self):
        '''
        Solo funciona en Windows
        '''
        ports = [p.device 
                 for p in serial.tools.list_ports.comports() 
                 if 'MSP' and 'UART' in p.description]
        
        if not ports:
            raise IOError('No MSP found')
        if len(ports) > 1:
            warnings.warn('Multiples MSP found - using the first')
            
        self.puerto = ports[0]            
        self.ser = serial.Serial(self.puerto, 
                                 baudrate = self.baudrate, 
                                 timeout = 10)
        

    def clear_buffer(self):
        self.ser.reset_input_buffer()
        
    def close(self):
        self.ser.close()


class GAME():
    
    # Set up the colors.
    BLACK = (0, 0, 0)
    GREEN = (0, 255, 0)
    WHITE = (255, 255, 255)
    RED = (255, 0, 0)
    MAGENTA = (255,0,255)
    # light shade of the button 
    color_light = (170,170,170) 
      
    # dark shade of the button 
    color_dark = (100,100,100) 
    
    def __init__(self, baudrate = None, port = None):
        self.x_max = 800  
        self.y_max = 800
        self.screen = None
        self.background = None
        self.dot = None
        self.dot_size = 20
        self.dot_x = self.x_max/2
        self.dot_y = self.y_max/2
        self.difficulty = 5.0 #mas chico el numero mas facil

        
        # Para modo de KeyBoard
        self.dx = 0
        self.dy = 0
        self.speed = 30
        
        
        # Modo BoosterPack
        self.baudrate = baudrate
        self.port = port
        
        try:
            self.com = Comunicacion(self.baudrate, self.port) 
            self.mode = 'BoosterPack'
            self.lock = 3
        except:
            print('No hay microcontrolador conectado. Se activa modo KeyBoard')
            self.mode = 'KeyBoard'
            self.lock = 2
        # Niveles y modos
        self.LEVEL = 0
        self.MAX_LEVEL = 7
        self.SCORE = 0
        self.TIME = 0
        self.startTime = 0
        self.start = False
        

        
        # Wall
        self.walls = []
        self.sizes = 8
        self.screen = None
        self.offset = 50
        
        # Bounce wall
        self.choque = False

        self.bounce = self.lock
        self.dx_bounce = 0
        self.dy_bounce = 0

        
        # Finish
        self.final_wall = None
        self.final_pos = (0,0)
        self.win_level = True
        
        # Contador
        self.cant_choques = 0
        self.text_contador = None

        self.__init_display()
        # Button
        self.pos_btn_x = 800
        self.pos_btn_y = 200
        self.bnt = None
        # defining a font 
        self.smallfont = pg.font.SysFont('Corbel',35) 
        # rendering a text written in 
        # this font 
        self.text = self.smallfont.render('MODE' , True , self.WHITE) 
        self.MODE_GAME = 'Joystick'
        
        # Fruta

        self.fruit_get = False

        

    def __modeGameText(self):
        if self.MODE_GAME == 'Accelerometer':
            self.text_mode = \
                self.smallfont.render('Accelerometer', True, self.BLACK)
            self.screen.blit(self.text_mode, 
                             (self.pos_btn_x - 30, self.pos_btn_y + 60 ))     
        elif self.MODE_GAME == 'Joystick':
            self.text_mode = \
                self.smallfont.render('Joystick', True, self.BLACK)  
            self.screen.blit(self.text_mode, 
                             (self.pos_btn_x + 25, self.pos_btn_y + 60 ))
        
    def __contadorGolpes(self): 
        self.text_contador = \
                self.smallfont.render('Golpes: {}'.format(self.cant_choques), 
                                      True, self.BLACK)  
        self.screen.blit(self.text_contador, 
                         (self.pos_btn_x - 30, self.pos_btn_y + 120))                

    def __levelText(self):
        self.text_level = \
            self.smallfont.render('Nivel: {}'.format(self.LEVEL), 
                                      True, self.BLACK)  
        self.screen.blit(self.text_level, 
                         (self.pos_btn_x - 30, self.pos_btn_y + 180))    
        
    def __scoreText(self):
        self.scoreText = self.smallfont.render('Puntaje: {}'.format(self.SCORE), 
                          True, self.BLACK)  
        self.screen.blit(self.scoreText, 
             (self.pos_btn_x - 30, self.pos_btn_y + 240))  
        
    def __startText(self):
        self.startText = self.smallfont.render('START!', 
                          True, self.WHITE)  
        self.screen.blit(self.startText, 
             (self.pos_btn_x + 30, self.pos_btn_y - 50))     
        
    def __timeText(self):
        self.timeText = self.smallfont.render('Time: {:3.1f} s'.format(self.TIME), 
                          True, self.BLACK)  
        self.screen.blit(self.timeText, 
             (self.pos_btn_x - 30, self.pos_btn_y + 300))       
        
        
    def __loadFruit(self):
        self.fruit_get = False
        
        pos_x = [self.offset + 40, self.x_max - self.offset - 100]
        pos_y = [self.offset + 40, self.y_max - self.offset - 100]
        
        self.fruit = pg.Rect(pos_x[random.randint(0,1)],
                             pos_y[random.randint(0,1)],
                             15, 15)        

    def __drawFruit(self):
        pg.draw.rect(self.screen, self.MAGENTA, self.fruit)            
        
        


    def __handle_keyboard(self):
        for event in pg.event.get():
            if event.type == pg.QUIT: 
                pg.quit()
                sys.exit()
                
                return True
            
            if event.type == pg.MOUSEBUTTONDOWN:
                if (self.pos_btn_x <= self.mouse[0] <= self.pos_btn_x + 150) and \
                    (self.pos_btn_y <= self.mouse[1] <= self.pos_btn_y + 50):
                    print('Boton presionado')
                    if self.MODE_GAME == 'Accelerometer':
                        self.MODE_GAME = 'Joystick'
                        self.com.send_value([77], True) #Send 'B' for beep
                    else:
                        self.MODE_GAME = 'Accelerometer'
                        self.com.send_value([78], True) #Send 'B' for beep
                        
                if (self.pos_btn_x <= self.mouse[0] <= self.pos_btn_x + 150) \
                    and (self.pos_btn_y -60 <= self.mouse[1] \
                         <= self.pos_btn_y - 60 + 50):          
                        print('Start')
                        self.__startGame()
                        

            
            if self.mode == 'KeyBoard':
                if event.type == KEYDOWN:
                    # Change the keyboard variables.
                    if event.key == K_LEFT or event.key == K_a:
                        self.dx = -self.speed
                    if event.key == K_RIGHT or event.key == K_d:
                        self.dx = self.speed
                    if event.key == K_UP or event.key == K_w:
                        self.dy = -self.speed
                    if event.key == K_DOWN or event.key == K_s:
                        self.dy = self.speed
    
                    
                if event.type == KEYUP:
                    if event.key == K_ESCAPE:
                        pg.quit()
                        sys.exit()
                    if event.key == K_LEFT or event.key == K_a:
                        self.dx = 0
                    if event.key == K_RIGHT or event.key == K_d:
                        self.dx = 0
                    if event.key == K_UP or event.key == K_w:
                        self.dy = 0
                    if event.key == K_DOWN or event.key == K_s:
                        self.dy = 0
    
                self.__update_dot(self.dx, self.dy)    

    def __loadWalls(self, level):
        
        GREEN = (0, 255, 0)
        
        # Read image
        file = 'maze/maze{}.png'.format(level)
        
        img = mpimg.imread(file) 
        
        W = img[:,:,0]
        
        w_x = W.shape[0]
        
        self.sizes = int((self.x_max - 2*self.offset)/w_x)
        
        for i in range(W.shape[0]):
            for j in range(0,W.shape[1]):
                if W[i,j] == 0.0:
                    rect = pg.Rect(j*self.sizes + self.offset,
                                   i*self.sizes + self.offset, self.sizes, self.sizes)
                    self.walls.append(rect)
                    
        final_size = (w_x - 2)/16  
        
        print(final_size)
        
        line = int((self.x_max - 2*self.offset)/w_x) * 2
                 
        column = \
            (self.x_max - 2*self.offset - (final_size + 1) * line)/(final_size)
        
        
        
        self.final_wall = pg.Rect(self.offset+ 
                                  (-1 + final_size / 2) * column, 
                                  self.offset - 2 * line, 
                                  column * 1.5,
                                  line)
                                  
        self.__loadFruit()

        
                
    def __updateWalls(self):
        for wall in self.walls:
            pg.draw.rect(self.screen, self.BLACK, wall)
            
        pg.draw.rect(self.screen, self.RED, self.final_wall)            
        
                
    def __collideWalls(self):
        for wall in self.walls:
            if self.dot.colliderect(wall):
                self.choque = True
                return wall
            
        if self.dot.colliderect(self.final_wall) and self.fruit_get:
            self.win_level = True
            self.__changeLevel()
            
        if self.dot.colliderect(self.fruit) and not self.fruit_get:
            self.SCORE += 10
            self.fruit_get = True
            #Send 'F' for fruit
            self.com.send_value([70], True)
        
               
        else:
            return False

    def __bounceCollideWalls(self):
        '''
        Funcion que produce pequeño rebote

        '''            
        if self.bounce == self.lock:
            self.dy_bounce = self.dy
            self.dx_bounce = self.dx
            self.cant_choques += 1
            print('Choque {}!'.format(self.cant_choques))
            self.SCORE -= 1
            self.com.send_value([67], True) #Send 'C' for collide
            
            if self.cant_choques == 1:
                #Send 'G' for Green
                self.com.send_value([71], True)
                
            elif self.cant_choques == 10:
                #Send 'B' for Blue
                self.com.send_value([66], True)
                
            elif self.cant_choques == 20:
                #Send 'R' for Red
                self.com.send_value([82], True)
    
        if self.bounce == 0:
            self.bounce = self.lock
            self.choque = False
                    
        else:
            self.dx -= self.dx_bounce
            self.dy -= self.dy_bounce
            self.bounce -= 1
            self.__update_dot(self.dx, self.dy)
            
            
    def __changeLevel(self):
        #Send 'W' for win
        self.com.send_value([87], True) 
        
        self.LEVEL += 1

        
        if self.LEVEL < self.MAX_LEVEL:
            self.walls = []
            self.final_wall = None
            self.dot_x = self.x_max/2
            self.dot_y = self.y_max/2
            self.__loadWalls(self.LEVEL)
            self.__update_dot(0, 0)

            
        else:
            print('YOU WIN ! ')  
            self.win_level = False
            print('Puntaje: {}'.format(self.SCORE))

            self.start = False

                

            
    def __changeDifficult(self, value):
        self.difficulty += value
        if self.difficulty == 0:
            self.difficulty = 1
                            
        
        

    def __init_display(self):
        pg.init()
        self.mainClock = pg.time.Clock()
        
        self.screen = pg.display.set_mode((self.x_max + 200,self.y_max), 0, 32)
        pg.display.set_caption('SEP')
        

                
        
        self.screen.fill(self.WHITE)
        
        # Walls
        self.__loadWalls(self.LEVEL)
        self.__updateWalls()        
        
        # dot
        self.dot = pg.Rect(self.dot_x, self.dot_y, self.dot_size, self.dot_size)
        
        pg.draw.rect(self.screen, (0,170,228), self.dot )
        
        pg.display.update()
        
    def __drawButton(self):
        
        self.mouse = pg.mouse.get_pos()
        if (self.pos_btn_x <= self.mouse[0] <= self.pos_btn_x + 150) and \
            (self.pos_btn_y <= self.mouse[1] <= self.pos_btn_y + 50):
            pg.draw.rect(self.screen, self.color_light, [self.pos_btn_x, self.pos_btn_y, 150,50]) 
        else:
            pg.draw.rect(self.screen, self.color_dark, [self.pos_btn_x, self.pos_btn_y, 150,50]) 
            
        

        self.screen.blit(self.text, (self.pos_btn_x + 30, self.pos_btn_y + 10 ))  
        self.__modeGameText()
            
    def __drawStartButton(self):
        self.mouse = pg.mouse.get_pos()

        if (self.pos_btn_x <= self.mouse[0] <= self.pos_btn_x + 150) and \
            (self.pos_btn_y -60 <= self.mouse[1] <= self.pos_btn_y - 60 + 50):
            pg.draw.rect(self.screen, self.color_light, [self.pos_btn_x, self.pos_btn_y-60, 150,50]) 
        else:
            pg.draw.rect(self.screen, self.color_dark, [self.pos_btn_x, self.pos_btn_y - 60, 150,50]) 
            
        

        self.screen.blit(self.text, (self.pos_btn_x + 30, self.pos_btn_y + 10 ))  
        self.__modeGameText()        
        

    def __startGame(self):     
        self.__resetVariables()
        self.final_wall = None
        self.walls = []
        self.__loadWalls(self.LEVEL)
        self.start = True
        
        
    def __resetVariables(self):
        self.startTime =  time()
        self.LEVEL = 0
        self.SCORE = 0
        self.dot_x = self.x_max/2
        self.dot_y = self.y_max/2
        self.cant_choques = 0
        self.fruit_get = False


    def __update_dot(self, dx, dy):
        '''
        Actualiza la posición del punto en la pantalla acorde a dx y dy
        '''
        self.dot_x += int(dx/self.difficulty)
        
        if self.dot_x < 0:
            self.dot_x = 0
        if self.dot_x > self.x_max:
            self.dot_x = self.x_max

        self.dot_y += int(dy/self.difficulty)
        if self.dot_y < 0:
            self.dot_y = 0
        if self.dot_y > self.y_max:
            self.dot_y = self.y_max

        # Image
        self.screen.fill(self.WHITE)
        
        # Walls
        self.__updateWalls() 

        self.__contadorGolpes()

        if not self.fruit_get:
            self.__drawFruit()  

        self.__levelText()
        self.__scoreText()
        self.__timeText()
        self.__drawButton()
        self.__drawStartButton()
        self.__startText()


        # dot
        self.dot = pg.Rect(self.dot_x, self.dot_y, self.dot_size, self.dot_size)
        pg.draw.rect(self.screen, (0,170,228), self.dot )
        
        pg.display.update()
               
          
    def run(self):
        
        game_max_speed = 1/30 #seconds per frame   
        t0 = time()  
        
        while True:
            self.__handle_keyboard()
            
            #Limit frames per second so different computers act at same speed.
            while(time() - t0 < game_max_speed):
                pass
            t0 = time() 
            
            
            self.__levelText()
            self.__scoreText()
            self.__timeText()
            self.__drawButton()
            self.__drawStartButton()
            self.__startText()
                
            # dot
            self.dot = pg.Rect(self.dot_x, self.dot_y, self.dot_size, self.dot_size)
            pg.draw.rect(self.screen, (0,170,228), self.dot )
            
            pg.display.update()

            if self.start:
                self.TIME = time() - self.startTime
                self.__collideWalls()
                
                if self.choque == False:
                    if self.mode == 'BoosterPack':
                        self.boosterPack()
                    else:
                        self.__update_dot(self.dx, self.dy)                    
                else:
    
                    self.__bounceCollideWalls()
                    
                     

    
    def boosterPack(self):
        #reciebe info X
        self.com.clear_buffer()                         #limpiar buffer de mensajes acumulados para obtener el dato más actualizado posible
        t_0 = time()
        control = True
        while(self.com.recieve_value_ascii() != 88):
            if time()-t_0 > 200:                        # Si no llega info en 200ms, asumir valor 0

                self.dx = 0
                control = False
        if control:
            cen_x = self.com.recieve_value_ascii()-48
            dec_x = self.com.recieve_value_ascii()-48
            uni_x = self.com.recieve_value_ascii()-48
            dx = 100*cen_x + 10*dec_x + uni_x           # convertir carácteres recibidos a numero
            self.dx = dx - 127                          # para que este entre -127 y y 127


        #recieve info Y
        self.com.clear_buffer()                        #limpiar buffer de mensajes acumulados para obtener el dato más actualizado posible

        t_0 = time()
        control = True
        while(self.com.recieve_value_ascii() != 89):
            if time()-t_0 > 200:                        # Si no llega info en 200ms, asumir valor 0 
                self.dy = 0
                control = False
        if control:
            cen_y = self.com.recieve_value_ascii()-48
            dec_y = self.com.recieve_value_ascii()-48
            uni_y = self.com.recieve_value_ascii()-48
            dy = 100*cen_y + 10*dec_y + uni_y           # convertir carácteres recibidos a numero
            self.dy = -(dy - 127)                       # para que este entre -127 y y 127 y voltear para sentido correcto acorde a la orientacion del booster

        
        #print("Angulo X: {}     Angulo Y: {}".format(X,Y))
        
        self.__update_dot(self.dx, self.dy)             #actualizar valores de inclinación


if __name__ == '__main__': 

    from argparse import ArgumentParser
    parser = ArgumentParser(description="Game for LAB04 SEP")
    
    parser.add_argument('-b', '--baudrate', default = 57600, 
                        help='Especifica el baudrate a utilizar')
    
    parser.add_argument('-p', '--port', default = None,
                        help='Especifica el puerto a utilizar') 
    
    args = parser.parse_args()

    game = GAME(args.baudrate, args.port) 

    game.run()
    

    