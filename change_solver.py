from change import ChangeGame
from queue import Queue

dictionary = {}

def permutation_dict(string):
    perms = {}




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
