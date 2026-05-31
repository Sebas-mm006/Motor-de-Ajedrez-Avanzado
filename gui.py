import pygame
import subprocess
import json
import os

# Constantes
ANCHO, ALTO = 640, 640
FILAS, COLUMNAS = 8, 8
TAMANO_CASILLA = ANCHO // COLUMNAS

# Colores
BLANCO = (255, 255, 255)
NEGRO = (0, 0, 0)
MARRON_CLARO = (240, 217, 181)
MARRON_OSCURO = (181, 136, 99)
RESALTE = (186, 202, 68)
ROJO = (255, 0, 0)

# Simbolos Unicode de las piezas (usar ingles para coincidir con el backend)
SIMBOLOS_PIEZAS = {
    'white': {'pawn': '♙', 'knight': '♘', 'bishop': '♗', 'rook': '♖', 'queen': '♕', 'king': '♔'},
    'black': {'pawn': '♟', 'knight': '♞', 'bishop': '♝', 'rook': '♜', 'queen': '♛', 'king': '♚'}
}

# Cargar imagenes de piezas desde la carpeta img
def cargar_imagenes_piezas():
    ruta_img = os.path.join(os.path.dirname(__file__), 'img')
    if not os.path.exists(ruta_img):
        return {}
    mapeo = {
        ('white', 'pawn'): 'wp.png',
        ('white', 'rook'): 'wR.png',
        ('white', 'knight'): 'wN.png',
        ('white', 'bishop'): 'wB.png',
        ('white', 'queen'): 'wQ.png',
        ('white', 'king'): 'wK.png',
        ('black', 'pawn'): 'bp.png',
        ('black', 'rook'): 'bR.png',
        ('black', 'knight'): 'bN.png',
        ('black', 'bishop'): 'bB.png',
        ('black', 'queen'): 'bQ.png',
        ('black', 'king'): 'bK.png'
    }
    imagenes = {}
    for (color, tipo_pieza), nombre_archivo in mapeo.items():
        ruta = os.path.join(ruta_img, nombre_archivo)
        if os.path.exists(ruta):
            try:
                imagen = pygame.image.load(ruta)
                imagen = pygame.transform.smoothscale(imagen, (TAMANO_CASILLA - 20, TAMANO_CASILLA - 20))
                imagenes[(color, tipo_pieza)] = imagen
            except pygame.error:
                pass
    return imagenes

class MotorAjedrezGUI:
    def __init__(self):
        pygame.init()
        self.pantalla = pygame.display.set_mode((ANCHO, ALTO))
        pygame.display.set_caption("Motor de Ajedrez")
        self.imagenes_piezas = cargar_imagenes_piezas()
        self.fuente = None
        self.fuente_pequena = None

        nombres_fuentes = ['dejavusans', 'dejavu sans', 'freesserif', 'free serif',
                           'arial unicode ms', 'symbola', 'noto sans',
                           'sans-serif']
        for nombre in nombres_fuentes:
            try:
                self.fuente = pygame.font.SysFont(nombre, 48)
                self.fuente_pequena = pygame.font.SysFont(nombre, 20)
                prueba = self.fuente.render('♙', True, BLANCO)
                if prueba.get_width() > 0:
                    break
            except:
                continue

        if self.fuente is None:
            self.fuente = pygame.font.SysFont(None, 48)
            self.fuente_pequena = pygame.font.SysFont(None, 20)

        self.tablero = [[None for _ in range(COLUMNAS)] for _ in range(FILAS)]
        self.casilla_seleccionada = None
        self.casillas_resaltadas = []
        self.turno_actual = 'white'
        self.juego_terminado = False
        self.texto_estado = ""

        self.motor = None
        self.iniciar_motor()
        self.inicializar_tablero()

    def iniciar_motor(self):
        try:
            self.motor = subprocess.Popen(
                ['./bin/chess', 'backend'],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1
            )
            self.enviar_comando({"cmd": "init"})
        except Exception as e:
            self.texto_estado = f"Error al iniciar el motor: {e}"
            self.juego_terminado = True

    def enviar_comando(self, comando_dict):
        if self.motor:
            try:
                comando = json.dumps(comando_dict) + '\n'
                self.motor.stdin.write(comando)
                self.motor.stdin.flush()
                respuesta = self.motor.stdout.readline().strip()
                return json.loads(respuesta) if respuesta else {}
            except (BrokenPipeError, OSError):
                return {}
        return {}

    def inicializar_tablero(self):
        respuesta = self.enviar_comando({"cmd": "board"})
        if 'board' in respuesta:
            for elemento in respuesta['board']:
                if elemento is not None:
                    fila, col = elemento['row'], elemento['col']
                    self.tablero[fila][col] = {
                        'type': elemento['type'],
                        'color': elemento['color']
                    }
            self.turno_actual = respuesta.get('turn', 'white')

    def actualizar_estado_tablero(self):
        respuesta = self.enviar_comando({"cmd": "board"})
        if 'board' in respuesta:
            self.tablero = [[None for _ in range(COLUMNAS)] for _ in range(FILAS)]
            for elemento in respuesta['board']:
                if elemento is not None:
                    fila, col = elemento['row'], elemento['col']
                    self.tablero[fila][col] = {
                        'type': elemento['type'],
                        'color': elemento['color']
                    }
            self.turno_actual = respuesta.get('turn', 'white')

    def dibujar_tablero(self):
        self.pantalla.fill(BLANCO)
        for fila in range(FILAS):
            for col in range(COLUMNAS):
                color = MARRON_CLARO if (fila + col) % 2 == 0 else MARRON_OSCURO
                rect = pygame.Rect(col * TAMANO_CASILLA, fila * TAMANO_CASILLA, TAMANO_CASILLA, TAMANO_CASILLA)
                pygame.draw.rect(self.pantalla, color, rect)

                if (fila, col) in self.casillas_resaltadas:
                    s = pygame.Surface((TAMANO_CASILLA, TAMANO_CASILLA))
                    s.set_alpha(128)
                    s.fill(RESALTE)
                    self.pantalla.blit(s, rect.topleft)

                pieza = self.tablero[fila][col]
                if pieza:
                    clave = (pieza['color'], pieza['type'])
                    if clave in self.imagenes_piezas:
                        imagen = self.imagenes_piezas[clave]
                        rect_imagen = imagen.get_rect(center=(col * TAMANO_CASILLA + TAMANO_CASILLA // 2,
                                                               fila * TAMANO_CASILLA + TAMANO_CASILLA // 2))
                        self.pantalla.blit(imagen, rect_imagen)
                    else:
                        simbolo = SIMBOLOS_PIEZAS[pieza['color']][pieza['type']]
                        color_rgb = BLANCO if pieza['color'] == 'white' else NEGRO
                        texto = self.fuente.render(simbolo, True, color_rgb)
                        rect_texto = texto.get_rect(center=(col * TAMANO_CASILLA + TAMANO_CASILLA // 2,
                                                            fila * TAMANO_CASILLA + TAMANO_CASILLA // 2))
                        self.pantalla.blit(texto, rect_texto)

        pygame.display.flip()

    def obtener_posicion_tablero(self, pos):
        col = pos[0] // TAMANO_CASILLA
        fila = pos[1] // TAMANO_CASILLA
        return fila, col

    def fila_col_a_algebraica(self, fila, col):
        return f"{chr(ord('a') + col)}{8 - fila}"

    def manejar_clic(self, pos):
        if self.juego_terminado:
            return

        fila, col = self.obtener_posicion_tablero(pos)

        if self.casilla_seleccionada is None:
            pieza = self.tablero[fila][col]
            if pieza and pieza['color'] == self.turno_actual:
                self.casilla_seleccionada = (fila, col)
                self.casillas_resaltadas = [(fila, col)]
                self.resaltar_movimientos_validos(fila, col)
        else:
            desde = self.casilla_seleccionada
            hasta = (fila, col)

            cadena_mov = self.fila_col_a_algebraica(desde[0], desde[1]) + \
                         self.fila_col_a_algebraica(hasta[0], hasta[1])

            respuesta = self.enviar_comando({"cmd": "move", "move": cadena_mov})

            if respuesta.get('status') == 'ok':
                self.actualizar_estado_tablero()

                if not self.juego_terminado:
                    self.movimiento_ia()
            else:
                self.texto_estado = "Movimiento invalido"

            self.casilla_seleccionada = None
            self.casillas_resaltadas = []

    def resaltar_movimientos_validos(self, fila, col):
        pieza = self.tablero[fila][col]
        if not pieza:
            return

        color = pieza['color']
        tipo = pieza['type']

        def esta_vacia(f, c):
            return self.tablero[f][c] is None

        def es_oponente(f, c):
            objetivo = self.tablero[f][c]
            return objetivo is not None and objetivo['color'] != color

        movimientos = []

        if tipo == 'pawn':
            direccion = -1 if color == 'white' else 1
            fila_inicio = 6 if color == 'white' else 1
            nueva_fila = fila + direccion
            if 0 <= nueva_fila < 8 and esta_vacia(nueva_fila, col):
                movimientos.append((nueva_fila, col))
                if fila == fila_inicio:
                    nueva_fila2 = fila + 2 * direccion
                    if esta_vacia(nueva_fila2, col):
                        movimientos.append((nueva_fila2, col))
            for dc in (-1, 1):
                nueva_fila = fila + direccion
                nueva_col = col + dc
                if 0 <= nueva_fila < 8 and 0 <= nueva_col < 8:
                    if es_oponente(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))

        elif tipo == 'knight':
            movimientos_caballo = [(2, 1), (2, -1), (-2, 1), (-2, -1),
                                   (1, 2), (1, -2), (-1, 2), (-1, -2)]
            for dr, dc in movimientos_caballo:
                nueva_fila, nueva_col = fila + dr, col + dc
                if 0 <= nueva_fila < 8 and 0 <= nueva_col < 8:
                    if esta_vacia(nueva_fila, nueva_col) or es_oponente(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))

        elif tipo == 'bishop':
            direcciones = [(-1, -1), (-1, 1), (1, -1), (1, 1)]
            for dr, dc in direcciones:
                nueva_fila, nueva_col = fila + dr, col + dc
                while 0 <= nueva_fila < 8 and 0 <= nueva_col < 8:
                    if esta_vacia(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))
                    elif es_oponente(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))
                        break
                    else:
                        break
                    nueva_fila += dr
                    nueva_col += dc

        elif tipo == 'rook':
            direcciones = [(-1, 0), (1, 0), (0, -1), (0, 1)]
            for dr, dc in direcciones:
                nueva_fila, nueva_col = fila + dr, col + dc
                while 0 <= nueva_fila < 8 and 0 <= nueva_col < 8:
                    if esta_vacia(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))
                    elif es_oponente(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))
                        break
                    else:
                        break
                    nueva_fila += dr
                    nueva_col += dc

        elif tipo == 'queen':
            direcciones = [(-1, -1), (-1, 0), (-1, 1),
                           (0, -1),           (0, 1),
                           (1, -1),  (1, 0),  (1, 1)]
            for dr, dc in direcciones:
                nueva_fila, nueva_col = fila + dr, col + dc
                while 0 <= nueva_fila < 8 and 0 <= nueva_col < 8:
                    if esta_vacia(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))
                    elif es_oponente(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))
                        break
                    else:
                        break
                    nueva_fila += dr
                    nueva_col += dc

        elif tipo == 'king':
            movimientos_rey = [(-1, -1), (-1, 0), (-1, 1),
                               (0, -1),           (0, 1),
                               (1, -1),  (1, 0),  (1, 1)]
            for dr, dc in movimientos_rey:
                nueva_fila, nueva_col = fila + dr, col + dc
                if 0 <= nueva_fila < 8 and 0 <= nueva_col < 8:
                    if esta_vacia(nueva_fila, nueva_col) or es_oponente(nueva_fila, nueva_col):
                        movimientos.append((nueva_fila, nueva_col))

        self.casillas_resaltadas = [(fila, col)] + movimientos

    def movimiento_ia(self):
        import time
        self.texto_estado = "IA pensando..."
        self.dibujar_tablero()

        respuesta = self.enviar_comando({"cmd": "getmove", "depth": 3})

        if respuesta.get('status') == 'ok' and respuesta.get('move'):
            movimiento = respuesta['move']
            time.sleep(0.1)
            self.actualizar_estado_tablero()
            self.texto_estado = ""
        else:
            self.texto_estado = "Juego terminado o sin movimientos"
            self.juego_terminado = True

    def dibujar_estado(self):
        if self.texto_estado:
            texto = self.fuente_pequena.render(self.texto_estado, True, ROJO)
            self.pantalla.blit(texto, (10, ALTO - 30))
            pygame.display.flip()

    def ejecutar(self):
        reloj = pygame.time.Clock()
        ejecutando = True

        while ejecutando:
            for evento in pygame.event.get():
                if evento.type == pygame.QUIT:
                    ejecutando = False
                elif evento.type == pygame.MOUSEBUTTONDOWN:
                    self.manejar_clic(evento.pos)

            self.dibujar_tablero()
            self.dibujar_estado()
            reloj.tick(60)

        if self.motor:
            try:
                self.enviar_comando({"cmd": "quit"})
                self.motor.terminate()
            except:
                pass
        pygame.quit()


if __name__ == "__main__":
    gui = MotorAjedrezGUI()
    gui.ejecutar()
