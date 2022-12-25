# Qianbo Yin
#
# search.py
# \--------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
#
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

from builtins import object
import util


class SearchProblem(object):
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return [s, s, w, s, w, w, s, w]


def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print ("Start:", problem.getStartState())
    print ("Is the start a goal?", problem.isGoalState(problem.getStartState()))
    print ("Start's successors:", problem.getSuccessors(problem.getStartState()))

    problem.getStartState(): returns a pair (tuple of size 2) that represents
    the coordinate of where the pacman starts

    problem.isGoalState((x, y)): returns a boolean True/False based on whether
    the coordinate (x, y) is a goal state.

    problem.getSuccessors((x, y)): given a position (x, y), returns a list of
    triples (list of tuples, with each tuple being size 3) of the successor
    positions of pacman.
    The tuple consists of position, action required to get to it, and the step
    cost i.e. ((35,2), 'North', 1).

    For the search functions, you are required to return a list of actions
    (that is, a list of strings such as ['North', 'North', 'West'])
    """

    # DONE - YOUR CODE HERE
    explored = []
    frontier = util.Stack()
    init_state = problem.getStartState()

    # initialize frontier
    if problem.isGoalState(init_state):
        return []
    frontier.push((init_state, None, 0)) # Node in frontier = (current_state, Action-to-get-here, step-cost)
    

    # a dictionary with what action took to get here
    # {the_state(x,y): (Action-to-get-here, parent_state)}
    history = {init_state: (None, None)} 
    
    while not frontier.isEmpty():
        # take out the frontier as current node and mark it as explored
        current = frontier.pop()
        explored.append(current[0])

        # push all successors of the current node into frontier
        for child in problem.getSuccessors(current[0]):

            # goal test
            if problem.isGoalState(child[0]):
                # backtrack and construct the solution
                solution = [child[1]]
                backtrack_state = current[0]

                # while parent_state is not None
                while history[backtrack_state][1] is not None:
                    # insert action into solution
                    solution.insert(0, history[backtrack_state][0])
                    backtrack_state = history[backtrack_state][1]
                return solution

            # if not explored, push into the frontier
            if child[0] not in explored:
                frontier.push(child)
                history[child[0]] = (child[1], current[0])

    # No solution found
    return None


def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""

    #DONE - YOUR CODE HERE

    explored = []
    frontier = util.Queue()
    init_state = problem.getStartState()
    
    # initialize frontier
    if problem.isGoalState(init_state):
        return []
    frontier.push((init_state, None, 0)) # Node in frontier = (current_state, Action-to-get-here, step-cost)

    # a dictionary with what action took to get here
    # {the_state(x,y): (Action-to-get-here, parent_state)}
    history = {init_state: (None, None)} 
    
    while not frontier.isEmpty():
        # take out the frontier as current expanding node
        current = frontier.pop()

        # mark as explored
        explored.append(current[0])

        # push all successors of the current node into frontier
        for child in problem.getSuccessors(current[0]):
            # goal test
            if problem.isGoalState(child[0]):
                # backtrack and construct the solution
                solution = [child[1]]
                backtrack_state = current[0]

                # while parent_state is not None
                while history[backtrack_state][1] is not None:
                    # insert action into solution
                    solution.insert(0, history[backtrack_state][0])
                    backtrack_state = history[backtrack_state][1]
                # DEBUG
                # print(solution)
                return solution
            
            # add into frontier and history
            if child[0] not in explored:
                frontier.push(child)
                history[child[0]] = (child[1], current[0])

    # Otherwise, No solution found
    return None


def iterativeDeepeningSearch(problem):
    """
    Search the shallowest nodes in the search tree first.
    Please use a maximum depth limit of 1000
    """
    # DONE - YOUR CODE HERE
    depth_limit = 1

    if problem.isGoalState(problem.getStartState()):
        return []

    while depth_limit <= 1000:
        # breadth first search with depth_limit

        explored = []
        # each Node in frontier = (current_state, Action-to-get-here, path-cost, depth)
        frontier = util.Queue()
        frontier.push((problem.getStartState(), None, 0, 1))
        

        # a dictionary with what action took to get here
        # {the_state(x,y): (Action-to-get-here, parent_state)}
        history = {problem.getStartState(): (None, None)} 

        while not frontier.isEmpty():
            # take out the frontier as current expanding node
            current = frontier.pop()
            # search up to depth_limit
            if current[3] > depth_limit:
                break

            # mark as explored
            explored.append(current[0])

            # push all successors of the current node into frontier
            for child in problem.getSuccessors(current[0]):
                # goal test
                if problem.isGoalState(child[0]):
                    # backtrack and construct the solution
                    solution = [child[1]]
                    backtrack_state = current[0]

                    # while parent_state is not None
                    while history[backtrack_state][1] is not None:
                        # insert action into solution
                        solution.insert(0, history[backtrack_state][0])
                        backtrack_state = history[backtrack_state][1]
                    return solution
                
                # add into frontier if not explored
                if child[0] not in explored:
                    frontier.push(child + (current[3] + 1, ))
                    history[child[0]] = (child[1], current[0])

                
        depth_limit += 1

    # No solution found
    return None


def uniformCostSearch(problem):
    """Search the node of least total cost first."""

    #DONE - YOUR CODE HERE
    explored = []
    frontier = util.PriorityQueue()
    init_state = problem.getStartState()
    # Node in frontier - (current_state, action-to-get-here, total_cost) with the priority of total_cost
    frontier.push((init_state, None, 0), 0)

    # a dictionary with what action took to get here
    # {the_state(x,y): (Action-to-get-here, parent_state)}
    history = {init_state: (None, None)} 
    
    while not frontier.isEmpty():
        # take out the frontier as current expanding node
        current = frontier.pop()

        # goal test
        if problem.isGoalState(current[0]):
            # backtrack and construct the solution
            solution = []
            backtrack_state = current[0]

            # while parent_state is not None
            while history[backtrack_state][1] is not None:
                # insert action into solution
                solution.insert(0, history[backtrack_state][0])
                backtrack_state = history[backtrack_state][1]
            return solution
        # mark as explored
        explored.append(current[0])

        # push all successors of the current node into frontier
        for child in problem.getSuccessors(current[0]):
            # add into frontier and history
            if child[0] not in explored:
                cost = child[2] + current[2]
                frontier.push((child[0], child[1], cost), cost)
                history[child[0]] = (child[1], current[0])


    # Otherwise, No solution found
    return None


def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the
    nearest goal in the provided SearchProblem.  This heuristic is trivial.
    Please do not modify this.
    """
    return 0


def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""

    # DONE - YOUR CODE HERE
    explored = []
    frontier = util.PriorityQueue()
    init_state = problem.getStartState()
    # Node in frontier - (current_state, action-to-get-here, total_cost) with the priority of total_cost
    frontier.push((init_state, None, 0), heuristic(init_state, problem))

    # a dictionary with what action took to get here
    # {the_state(x,y): (Action-to-get-here, parent_state)}
    history = {init_state: (None, None)} 
    
    while not frontier.isEmpty():
        # take out the frontier as current expanding node
        current = frontier.pop()
        # goal test
        if problem.isGoalState(current[0]):
            # backtrack and construct the solution
            solution = []
            backtrack_state = current[0]

            # while parent_state is not None
            while history[backtrack_state][1] is not None:
                # insert action into solution
                solution.insert(0, history[backtrack_state][0])
                backtrack_state = history[backtrack_state][1]
            return solution

        # mark as explored
        explored.append(current[0])

        # push all successors of the current node into frontier
        for child in problem.getSuccessors(current[0]):
            # add children into frontier and history
            if child[0] not in explored:
                cost = child[2] + current[2]
                frontier.push((child[0], child[1], cost), cost + heuristic(child[0], problem))
                history[child[0]] = (child[1], current[0])


    # Otherwise, No solution found
    return None


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
ids = iterativeDeepeningSearch
astar = aStarSearch
ucs = uniformCostSearch
