#position: number of ticks on the board
#moves: number of ticks to remove in order to win
#values {W, L}: win or lose


solved = {} #position: W or L
children = {} #position: number of children being counted
parents = {} #position: list of positions
#boolean gameover(position)
def gameover(position):
    return position <= 2

#value W or L
def game_over_value(position):
    if position == 0:
        return 'L'
    return 'W'

#new position
def do_move(position, move):
    return position - move

#list of moves possible to win
def generate_moves(position):
    moves = []
    if solve(position-1) == 'L':
        moves += [1]
    if solve(position-2) == 'L':
        moves += [2]
    return moves

def solvePrimitives():
    solved[0] = 'L'
    solved[1] = 'W'
    solved[2] = 'W'


#value W or L
def solve(position):
    solvePrimitives()
    generateFamily()
    return 'L'


for i in range(11)[::-1]:
    print("for position ", i, " we will ", solve(i))
    #print("the possible moves are ", generate_moves(i))
