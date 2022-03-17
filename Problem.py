import random
import math

class Problem:
    def __init__(self):
        self.adjacencyMatrix = None
        self.solution = None
        self.optimalSolution = None
        self.points = None
        self.valid = False

    def loadProblemFromFile(self, filename):
        pass

    def loadSolutionFromFile(self, filename):
        pass

    def printAdjecency(self):
        if self.adjacencyMatrix == None:
            print("Empty")
            return

        width = len(self.adjacencyMatrix)
        for y in range(width):
            for x in range(width):
                print(self.adjacencyMatrix[y][x], end=' ')
            print('')
        print('')

    def _printArray(self, array):
        for i in range(len(array)):
            print(array[i], end=' ')
        print('')

    def printSolution(self):
        if self.solution == None:
            print("Problem not yet solved.")
            return
        if self.points != None:
            pass
        else: 
            self._printArray(self.solution)
        
    def printOptimalSolution(self):
        if self.optimalSolution == None:
            print("No optimal solution provided.")
            return
        self._printArray(self.optimalSolution)

    def calcSolutionLen(self, solution):
        distance = 0
        for i in range(len(solution)-1):
            distance += self.adjacencyMatrix[solution[i]][solution[i+1]]
        distance += self.adjacencyMatrix[solution[len(solution)-1]][solution[0]]
        return distance

    def PRD(self):
        if self.solution == None:
            print("No solution provided.")
            return
        if self.optimalSolution == None:
            print("No optimal solution provided.")
            return 
        fx = self.calcSolutionLen(self.solution)
        fref = self.calcSolutionLen(self.optimalSolution)
        return (fx-fref)/fref

    def generateAsAsymetric(self, size, seed, minimumWeight, maximumWeight):
        random.seed(seed)
        self.valid = True
        self.adjacencyMatrix = [[random.randint(minimumWeight,maximumWeight) for x in range(size)] for y in range(size)]
        for i in range(size):
            self.adjacencyMatrix[i][i] = 0

    def generateAsSymteric(self, size, seed,minimumWeight, maximumWeight):
        self.generateAsymetric(size, seed, minimumWeight, maximumWeight)
        for x in range(0,size):
            for y in range(x+1, size):
                self.adjacencyMatrix[x][y] = self.adjacencyMatrix[y][x]

    def generateAsEuclid2D(self, size, seed, width):
        random.seed(seed)
        self.valid = True
        self.points = [[random.random()*width for i in range(2)] for j in range(size)]
        self.adjacencyMatrix = [[0 for x in range(size)] for y in range(size)]
        for p in range(size):
            for o in range(size):
                self.adjacencyMatrixtrix[p][o] = round(math.sqrt(
                    math.pow(self.points[p][0] - self.points[o][0],2) + math.pow(self.points[p][1] - self.points[o][1],2)
                ))