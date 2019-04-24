from change import ChangeGame

TotalPositions  =   3**24
dictionary = {}

class Queue:
    def __init__(self):
        self.front = 0
        self.back = 0
        self.items = []
        self.capacity = 0

    def push(self, item):
        self.items += [item]
        self.back += 1
        self.capacity += 1

    def pop(self):
        if self.capacity == 0:
            return None
        self.front += 1
        self.capacity -= 1
        return self.items[self.front-1]


def makeGameFromString(string): #string should be 24 chars long: turn + 23 positions
    print(string)
    t = int(string[0])
    board = []
    for i in string[1:]:
        board += [int(i)]
    return ChangeGame(board, t)

def newGame():
    game = ChangeGame()
    game.printNumberBoard()
    game.printGameBoard()

def solve(depth):
    fringe = Queue()
    fringe.push('0211112222222222222220000')
    for _ in range(depth):
        game = makeGameFromString(fringe.pop())
        game.printGameBoard()
        nextGames = [game.getNextGame(move) for move in game.getLegalMoves()]
        for g in nextGames:
            if g.toString() in dictionary:
                break
            g.printGameBoard()
            win = g.whoWins()
            if win < 2:
                dictionary[g.toString()] = win
                g.printGameBoard()
                print(win, ' wins this game.')
            else:
                fringe.push(g.toString())

solve(10)
print(dictionary)
