#position: number of ticks on the board
#moves: number of ticks to remove in order to win
#values {W, L}: win or lose



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

#value W or L
def solve(position):6
    if position == 0:
        return 'L'
    if position <= 2:
        return 'W'
    value1 = solve(position-1)
    value2 = solve(position-2)
    if value1 == 'L' or value2 == 'L':
        return 'W'
    return 'L'


for i in range(11)[::-1]:
    print("for position ", i, " we will ", solve(i))
    print("the possible moves are ", generate_moves(i))
