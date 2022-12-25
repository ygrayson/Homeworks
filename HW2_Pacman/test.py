from subprocess import PIPE, run

command_output_pairs = [
### STUDENT TESTS ###
# Modified python3 to python for running test on Windows
('python3 pacman.py -l openMaze -p SearchAgent --frameTime 0', 
'''[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
Path found with total cost of 158 in 0.0 seconds
Search nodes expanded: 315
Pacman emerges victorious! Score: 352
Average Score: 352.0
Scores:        352.0
Win Rate:      1/1 (1.00)
Record:        Win'''),
('python3 pacman.py -l tinyMaze -p SearchAgent -a fn=bfs --frameTime 0', '''[SearchAgent] using function bfs
[SearchAgent] using problem type PositionSearchProblem
Path found with total cost of 8 in 0.0 seconds
Search nodes expanded: 15
Pacman emerges victorious! Score: 502
Average Score: 502.0
Scores:        502.0
Win Rate:      1/1 (1.00)
Record:        Win'''),
('python3 pacman.py -l tinyMaze -p SearchAgent -a fn=ids --frameTime 0', '''[SearchAgent] using function ids
[SearchAgent] using problem type PositionSearchProblem
Path found with total cost of 8 in 0.0 seconds
Search nodes expanded: 398
Pacman emerges victorious! Score: 502
Average Score: 502.0
Scores:        502.0
Win Rate:      1/1 (1.00)
Record:        Win'''),
('python3 pacman.py -l tinyMaze -p SearchAgent -a fn=ucs --frameTime 0', '''[SearchAgent] using function ucs
[SearchAgent] using problem type PositionSearchProblem
Path found with total cost of 8 in 0.0 seconds
Search nodes expanded: 15
Pacman emerges victorious! Score: 502
Average Score: 502.0
Scores:        502.0
Win Rate:      1/1 (1.00)
Record:        Win'''),
('python3 pacman.py -l openMaze -p SearchAgent -a fn=astar,heuristic=manhattanHeuristic --frameTime 0', '''[SearchAgent] using function astar and heuristic manhattanHeuristic
[SearchAgent] using problem type PositionSearchProblem
Path found with total cost of 54 in 0.0 seconds
Search nodes expanded: 535
Pacman emerges victorious! Score: 456
Average Score: 456.0
Scores:        456.0
Win Rate:      1/1 (1.00)
Record:        Win'''),
]

num_correct = 0
num_tot = len(command_output_pairs)

print('Running test suite...\n')

def remove_runtime(str_in):
    beg = str_in.find(' in ')
    end = str_in.find('\n', beg)
    return str_in[:beg] + str_in[end:]

for pair in command_output_pairs:
    command, correct_out = pair
    completed_process = run(command, stdout=PIPE, stderr=PIPE, shell=True)
    student_out = completed_process.stdout.decode('UTF-8').strip()
    err = completed_process.stderr

    # remove runtime (e.g. 'in 0.1 seconds') from real & correct outputs since runtimes differ
    student_out = remove_runtime(student_out)
    correct_out = remove_runtime(correct_out)
    print(f"Running test case '{command}'...............", end='')
    if student_out:
        if student_out != correct_out:
            print(f' ✗\nYour output: \n{student_out}\n\nCorrect output: \n{correct_out}\n')
        else:
            print(' ✓')
            num_correct += 1
    else:
        print(f'ERROR.  Your program threw an error on a test case.\nTest case: {command}\nError: {err}')
    
print(f'\nYour program successfully completed {num_correct}/{num_tot} test cases.', end='')
if num_correct == num_tot:
    print(' Congratulations!', end='')
print('')
