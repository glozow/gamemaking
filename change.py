
NUMPIECES = 4
NUMPOSITIONS = 23
NUMBOARDS = 980628 # 23 choose 8 times 2

        #        1   2   3   4
        #       / \ / \ / \ / \
        #      5   6   7   8  9
        #     / \ / \ / \ / \ /
        #    10  11  12  13 14
        #   / \ / \ / \ / \ /
        # 15  16  17  18  19
        #  \ / \ / \ / \ /
        #  20  21  22  23

class Piece:
    def __init__(self, pos):
        self.position = pos

class Player:
    def __init__(self, playernum, board=None):
        self.playernum = playernum
        self.pieces = set() #list of Piece objects
        if board:
            for i in range(1, NUMPOSITIONS+1):
                if board[i] == playernum:
                    self.pieces.add(Piece(i))
        else:
            if playernum == 1: #player
                for i in range(1, NUMPIECES+1):
                    self.pieces.add(Piece(i))
            else: #computer
                for i in range(NUMPOSITIONS-NUMPIECES+1, NUMPOSITIONS+1):
                    self.pieces.add(Piece(i))

        #I'm sorry I had to hard-code this because I am dumb :'(
        self.nextdict = {}
        if playernum == 1: #player
            self.nextdict[1] = [5, 6, 10, 12, 15, 18, 23]
            self.nextdict[2] = [6, 7, 11, 13, 16, 19, 20]
            self.nextdict[3] = [7, 8, 12, 14, 17, 21]
            self.nextdict[4] = [8, 9, 13, 18, 22]
            self.nextdict[5] = [10, 11, 15, 17, 22]
            self.nextdict[6] = [11, 12, 16, 18, 20, 23]
            self.nextdict[7] = [12, 13, 17, 19, 21]
            self.nextdict[8] = [13, 14, 18, 22]
            self.nextdict[9] = [14, 19, 23]
            self.nextdict[10] = [15, 16, 21]
            self.nextdict[11] = [16, 17, 20, 22]
            self.nextdict[12] = [17, 18, 21, 23]
            self.nextdict[13] = [18, 19, 22]
            self.nextdict[14] = [19, 23]
            self.nextdict[15] = [20]
            self.nextdict[16] = [20, 21]
            self.nextdict[17] = [21, 22]
            self.nextdict[18] = [22, 23]
            self.nextdict[19] = [23]
            self.nextdict[20] = []
            self.nextdict[21] = []
            self.nextdict[22] = []
            self.nextdict[23] = []
        if playernum == 0: #computer
            self.nextdict[1] = []
            self.nextdict[2] = []
            self.nextdict[3] = []
            self.nextdict[4] = []
            self.nextdict[5] = [1]
            self.nextdict[6] = [1, 2]
            self.nextdict[7] = [2, 3]
            self.nextdict[8] = [3, 4]
            self.nextdict[9] = [4]
            self.nextdict[10] = [1, 5]
            self.nextdict[11] = [2, 5, 6]
            self.nextdict[12] = [1, 3, 6, 7]
            self.nextdict[13] = [2, 4, 7, 8]
            self.nextdict[14] = [3, 8, 9]
            self.nextdict[15] = [1, 5, 10]
            self.nextdict[16] = [2, 6, 10, 11]
            self.nextdict[17] = [3, 5, 7, 11, 12]
            self.nextdict[18] = [1, 4, 6, 8, 12, 13]
            self.nextdict[19] = [2, 7, 9, 13, 14]
            self.nextdict[20] = [2, 6, 11, 15, 16]
            self.nextdict[21] = [3, 7, 10, 12, 16, 17]
            self.nextdict[22] = [4, 5, 8, 11, 13, 17, 18]
            self.nextdict[23] = [1, 6, 9, 12, 14, 18, 19]
    def movePiece(self, curr, next):
        for piece in self.pieces:
            if piece.position == curr:
                piece.position = next

class ChangeGame:
    # computer = 0, player = 1. computer always goes first

    def __init__(self, board=None, turn=0):
        self.computer = Player(0, board)
        self.player = Player(1, board)
        if board:
            self.board = board
        else:
            self.board = [2 for i in range(NUMPOSITIONS+1)] #2 means empty
            self.initializeBoard()
        self.turn = turn

    def toString(self):
        string = ''
        string += str(self.turn)
        for i in self.board:
            string += str(i)
        return string

    # class method 
    def validateString(string):
        string = string[1:]
        return string.count('0') == 4 and string.count('1')  == 4

    def initializeBoard(self):
        for piece in self.computer.pieces:
            pos = piece.position
            self.board[pos] = 0
        for piece in self.player.pieces:
            self.board[piece.position] = 1

    def getLegalMoves(self):
        # turn = 0 for computer, 1 for player
        moves = []
        if self.turn == 0:
            me = self.computer
        else:
            me = self.player
        for piece in me.pieces:
            curr = piece.position
            if self.board[curr] == self.turn:
                all = me.nextdict[curr]
                moves += [(curr, next) for next in all if self.board[next] == 2] #must be unoccupied
        return moves

    def makeMove(self, move):
        curr, next = move
        self.board[next] = self.board[curr]
        self.board[curr] = 2
        if self.turn == 0:
            p = self.computer
        else:
            p = self.player
        p.movePiece(curr, next)
        self.turn = 1-self.turn

    def getNextGame(self, move):
        nextBoard = ChangeGame(self.board, 1-self.turn)
        nextBoard.makeMove(move)
        return nextBoard

    def isGameOver():
        return self.whoWins() < 2

    def whoWins(self):
        # taken opponent's home
        if self.board[1] == 1 and self.board[2] == 1 and self.board[3] == 1 and self.board[4] == 1:
            return 1
        if self.board[20] == 0 and self.board[21] == 0 and self.board[22] == 0 and self.board[23] == 0:
            return 0
        # it's my turn and I'm trapped
        if len(self.getLegalMoves(self.turn)) == 0:
            return 1-self.turn
        return 2 #nobody wins yet

    def p(self, playernum):
        if playernum == 2:
            return '-'
        elif playernum == 0:
            return 'X'
        return 'O'


    def printNumberBoard(self):
        print('        1   2   3   4')
        print('       / \\ / \\ / \\ / \\')
        print('      5   6   7   8  9')
        print('     / \\ / \\ / \\ / \\ /')
        print('    10  11  12  13 14')
        print('   / \\ / \\ / \\ / \\ /')
        print(' 15  16  17  18  19')
        print('   \\ / \\ / \\ / \\ /')
        print('   20  21  22  23')

    def printGameBoard(self):
        b = self.board
        print('       ', self.p(b[1]), ' ', self.p(b[2]), ' ', self.p(b[3]), ' ', self.p(b[4]))
        print('       / \\ / \\ / \\ / \\')
        print('     ', self.p(b[5]), ' ', self.p(b[6]), ' ', self.p(b[7]), ' ', self.p(b[8]), ' ', self.p(b[9]))
        print('     / \\ / \\ / \\ / \\ /')
        print('   ', self.p(b[10]), ' ', self.p(b[11]), ' ', self.p(b[12]), ' ', self.p(b[13]), ' ', self.p(b[14]))
        print('   / \\ / \\ / \\ / \\ /')
        print(' ', self.p(b[15]), ' ', self.p(b[16]), ' ', self.p(b[17]), ' ', self.p(b[18]), ' ', self.p(b[19]))
        print('   \\ / \\ / \\ / \\ /')
        print('   ', self.p(b[20]), ' ', self.p(b[21]), ' ', self.p(b[22]), ' ', self.p(b[23]))


game = ChangeGame()
game.printNumberBoard()
game.printGameBoard()
print(game.toString())
