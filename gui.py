import pygame
import subprocess
import json

# Constants
WIDTH, HEIGHT = 640, 640
ROWS, COLS = 8, 8
SQUARE_SIZE = WIDTH // COLS

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_BROWN = (240, 217, 181)
DARK_BROWN = (181, 136, 99)
HIGHLIGHT = (186, 202, 68)
RED = (255, 0, 0)

# Piece Unicode symbols
PIECE_SYMBOLS = {
    'white': {'pawn': '♙', 'knight': '♘', 'bishop': '♗', 'rook': '♖', 'queen': '♕', 'king': '♔'},
    'black': {'pawn': '♟', 'knight': '♞', 'bishop': '♝', 'rook': '♜', 'queen': '♛', 'king': '♚'}
}

# Optional: Try to load piece images if available
def load_piece_images():
    """Attempt to load piece images from a pieces directory"""
    try:
        import os
        pieces_dir = os.path.join(os.path.dirname(__file__), 'pieces')
        if os.path.exists(pieces_dir):
            piece_mapping = {
                ('white', 'pawn'): 'wp.png',
                ('white', 'rook'): 'wr.png',
                ('white', 'knight'): 'wn.png',
                ('white', 'bishop'): 'wb.png',
                ('white', 'queen'): 'wq.png',
                ('white', 'king'): 'wk.png',
                ('black', 'pawn'): 'bp.png',
                ('black', 'rook'): 'br.png',
                ('black', 'knight'): 'bn.png',
                ('black', 'bishop'): 'bb.png',
                ('black', 'queen'): 'bq.png',
                ('black', 'king'): 'bk.png'
            }
            
            images = {}
            for (color, piece_type), filename in piece_mapping.items():
                image_path = os.path.join(pieces_dir, filename)
                if os.path.exists(image_path):
                    try:
                        image = pygame.image.load(image_path)
                        # Scale image to fit square size with some padding
                        image = pygame.transform.smoothscale(image, (SQUARE_SIZE - 20, SQUARE_SIZE - 20))
                        images[(color, piece_type)] = image
                    except pygame.error:
                        pass  # Failed to load this image, continue with others
            return images
    except ImportError:
        pass
    return {}

# Load piece images at module level
PIECE_IMAGES = load_piece_images()

class ChessGUI:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        pygame.display.set_caption("Chess Engine")
        # Try to get a font that supports Unicode chess symbols
        font_names = ['dejavusans', 'dejavu sans', 'freesserif', 'free serif', 
                     'arial unicode ms', 'symbola', 'noto sans', 
                     'sans-serif']  # Generic fallback
        self.font = None
        self.small_font = None
        
        for font_name in font_names:
            try:
                self.font = pygame.font.SysFont(font_name, 48)
                self.small_font = pygame.font.SysFont(font_name, 20)
                # Test if font can render a chess symbol
                test_surf = self.font.render('♙', True, WHITE)
                if test_surf.get_width() > 0:  # Font works
                    break
            except:
                continue
        
        # Fallback to default font if none worked
        if self.font is None:
            self.font = pygame.font.SysFont(None, 48)
            self.small_font = pygame.font.SysFont(None, 20)
        
        self.board = [[None for _ in range(COLS)] for _ in range(ROWS)]
        self.selected_square = None
        self.highlighted_squares = []
        self.current_turn = 'white'
        self.game_over = False
        self.status_text = ""
         
        self.engine = None
        self.start_engine()
        self.init_board()
        # Load piece images after engine is started
        load_piece_images()
    
    def start_engine(self):
        try:
            self.engine = subprocess.Popen(
                ['./bin/chess', 'backend'],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1
            )
            self.send_command({"cmd": "init"})
        except Exception as e:
            self.status_text = f"Error starting engine: {e}"
            self.game_over = True
    
    def send_command(self, cmd_dict):
        if self.engine:
            try:
                cmd = json.dumps(cmd_dict) + '\n'
                self.engine.stdin.write(cmd)
                self.engine.stdin.flush()
                response = self.engine.stdout.readline().strip()
                return json.loads(response) if response else {}
            except (BrokenPipeError, OSError):
                return {}
        return {}
    
    def init_board(self):
        response = self.send_command({"cmd": "board"})
        if 'board' in response:
            for item in response['board']:
                if item is not None:
                    row, col = item['row'], item['col']
                    self.board[row][col] = {
                        'type': item['type'],
                        'color': item['color']
                    }
            self.current_turn = response.get('turn', 'white')
    
    def update_board_state(self):
        response = self.send_command({"cmd": "board"})
        if 'board' in response:
            self.board = [[None for _ in range(COLS)] for _ in range(ROWS)]
            for item in response['board']:
                if item is not None:
                    row, col = item['row'], item['col']
                    self.board[row][col] = {
                        'type': item['type'],
                        'color': item['color']
                    }
            self.current_turn = response.get('turn', 'white')
    
    def draw_board(self):
        self.screen.fill(WHITE)
        for row in range(ROWS):
            for col in range(COLS):
                color = LIGHT_BROWN if (row + col) % 2 == 0 else DARK_BROWN
                rect = pygame.Rect(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
                pygame.draw.rect(self.screen, color, rect)
                
                if (row, col) in self.highlighted_squares:
                    s = pygame.Surface((SQUARE_SIZE, SQUARE_SIZE))
                    s.set_alpha(128)
                    s.fill(HIGHLIGHT)
                    self.screen.blit(s, rect.topleft)
                
                piece = self.board[row][col]
                if piece:
                    symbol = PIECE_SYMBOLS[piece['color']][piece['type']]
                    # Convert color string to RGB tuple
                    color_rgb = WHITE if piece['color'] == 'white' else BLACK
                    # Render with anti-aliasing for better appearance
                    text = self.font.render(symbol, True, color_rgb)
                    # Create a background rectangle for better contrast if needed
                    text_rect = text.get_rect(center=(col * SQUARE_SIZE + SQUARE_SIZE // 2, 
                                                      row * SQUARE_SIZE + SQUARE_SIZE // 2))
                    self.screen.blit(text, text_rect)
        
        pygame.display.flip()
    
    def get_board_pos(self, pos):
        col = pos[0] // SQUARE_SIZE
        row = pos[1] // SQUARE_SIZE
        return row, col
    
    def row_col_to_algebraic(self, row, col):
        return f"{chr(ord('a') + col)}{8 - row}"
    
    def handle_click(self, pos):
        if self.game_over:
            return
            
        row, col = self.get_board_pos(pos)
        
        if self.selected_square is None:
            piece = self.board[row][col]
            if piece and piece['color'] == self.current_turn:
                self.selected_square = (row, col)
                self.highlighted_squares = [(row, col)]
                self.highlight_valid_moves(row, col)
        else:
            from_sq = self.selected_square
            to_sq = (row, col)
            
            move_str = self.row_col_to_algebraic(from_sq[0], from_sq[1]) + \
                      self.row_col_to_algebraic(to_sq[0], to_sq[1])
            
            response = self.send_command({"cmd": "move", "move": move_str})
            
            if response.get('status') == 'ok':
                self.update_board_state()
                
                if not self.game_over:
                    self.ai_move()
            else:
                self.status_text = "Invalid move"
            
            self.selected_square = None
            self.highlighted_squares = []
    
    def highlight_valid_moves(self, row, col):
        piece = self.board[row][col]
        if not piece:
            return
        
        color = piece['color']
        ptype = piece['type']
        
        # Helper functions
        def is_empty(r, c):
            return self.board[r][c] is None
        
        def is_opponent(r, c):
            target = self.board[r][c]
            return target is not None and target['color'] != color
        
        moves = []
        
        if ptype == 'pawn':
            direction = -1 if color == 'white' else 1  # white moves up (row-1), black moves down (row+1)
            start_row = 6 if color == 'white' else 1
            # One step forward
            new_row = row + direction
            if 0 <= new_row < 8 and is_empty(new_row, col):
                moves.append((new_row, col))
                # Two steps from start
                if row == start_row:
                    new_row2 = row + 2 * direction
                    if is_empty(new_row2, col):
                        moves.append((new_row2, col))
            # Captures
            for dc in (-1, 1):
                new_row = row + direction
                new_col = col + dc
                if 0 <= new_row < 8 and 0 <= new_col < 8:
                    if is_opponent(new_row, new_col):
                        moves.append((new_row, new_col))
        
        elif ptype == 'knight':
            knight_moves = [(2, 1), (2, -1), (-2, 1), (-2, -1),
                            (1, 2), (1, -2), (-1, 2), (-1, -2)]
            for dr, dc in knight_moves:
                new_row, new_col = row + dr, col + dc
                if 0 <= new_row < 8 and 0 <= new_col < 8:
                    if is_empty(new_row, new_col) or is_opponent(new_row, new_col):
                        moves.append((new_row, new_col))
        
        elif ptype == 'bishop':
            directions = [(-1, -1), (-1, 1), (1, -1), (1, 1)]
            for dr, dc in directions:
                new_row, new_col = row + dr, col + dc
                while 0 <= new_row < 8 and 0 <= new_col < 8:
                    if is_empty(new_row, new_col):
                        moves.append((new_row, new_col))
                    elif is_opponent(new_row, new_col):
                        moves.append((new_row, new_col))
                        break
                    else:
                        break  # own piece blocks
                    new_row += dr
                    new_col += dc
        
        elif ptype == 'rook':
            directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
            for dr, dc in directions:
                new_row, new_col = row + dr, col + dc
                while 0 <= new_row < 8 and 0 <= new_col < 8:
                    if is_empty(new_row, new_col):
                        moves.append((new_row, new_col))
                    elif is_opponent(new_row, new_col):
                        moves.append((new_row, new_col))
                        break
                    else:
                        break
                    new_row += dr
                    new_col += dc
        
        elif ptype == 'queen':
            directions = [(-1, -1), (-1, 0), (-1, 1),
                          (0, -1),           (0, 1),
                          (1, -1),  (1, 0),  (1, 1)]
            for dr, dc in directions:
                new_row, new_col = row + dr, col + dc
                while 0 <= new_row < 8 and 0 <= new_col < 8:
                    if is_empty(new_row, new_col):
                        moves.append((new_row, new_col))
                    elif is_opponent(new_row, new_col):
                        moves.append((new_row, new_col))
                        break
                    else:
                        break
                    new_row += dr
                    new_col += dc
        
        elif ptype == 'king':
            king_moves = [(-1, -1), (-1, 0), (-1, 1),
                          (0, -1),           (0, 1),
                          (1, -1),  (1, 0),  (1, 1)]
            for dr, dc in king_moves:
                new_row, new_col = row + dr, col + dc
                if 0 <= new_row < 8 and 0 <= new_col < 8:
                    if is_empty(new_row, new_col) or is_opponent(new_row, new_col):
                        moves.append((new_row, new_col))
        
        # Set highlighted squares: selected square plus valid moves
        self.highlighted_squares = [(row, col)] + moves
    
    def ai_move(self):
        import time
        self.status_text = "AI thinking..."
        self.draw_board()
        
        response = self.send_command({"cmd": "getmove", "depth": 3})
        
        if response.get('status') == 'ok' and response.get('move'):
            move = response['move']
            time.sleep(0.1)
            self.update_board_state()
            self.status_text = ""
        else:
            self.status_text = "Game over or no moves"
            self.game_over = True
    
    def draw_status(self):
        if self.status_text:
            text = self.small_font.render(self.status_text, True, RED)
            self.screen.blit(text, (10, HEIGHT - 30))
            pygame.display.flip()
    
    def run(self):
        clock = pygame.time.Clock()
        running = True
        
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    self.handle_click(event.pos)
            
            self.draw_board()
            self.draw_status()
            clock.tick(60)
        
        if self.engine:
            try:
                self.send_command({"cmd": "quit"})
                self.engine.terminate()
            except:
                pass
        pygame.quit()


if __name__ == "__main__":
    gui = ChessGUI()
    gui.run()