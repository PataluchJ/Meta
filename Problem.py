import random
import math
import fileloader

class Problem:
    def __init__(self):
        self.__adjacencyMatrix = None
        self.__solution = None
        self.__optimalSolution = None
        self.__points = None
        self.__loader = fileloader.FileLoader()

    def loadProblemFromFile(self, filename: str) -> None:
        self.__adjacencyMatrix = self.__loader.loadProblemInstance(filename)

    def loadSolutionFromFile(self, filename: str) -> None:
        self.__optimalSolution = self.__loader.loadOptimalSolution(filename)

    def printAdjecency(self) -> None:
        if self.__adjacencyMatrix == None:
            print("Empty")
            return

        height = len(self.__adjacencyMatrix)
        if height < 10:
            for y in range(height):
                for x in range(height):
                    print(self.__adjacencyMatrix[y][x], end=' ')
                print('')
            print('')
        else:
            cellsToPrint = [0,1,2,3,height-3,height-2,height-1]
            for y in cellsToPrint:
                for x in cellsToPrint:
                    print('{0:03}'.format(self.__adjacencyMatrix[y][x]), end=' ')
                    if x == 3:
                        print(' ... ', end='')
                print('')
                if y == 3:
                    print(' ... ')

    def __printArray(self, array: list[int]) -> None:
        for i in range(len(array)):
            print(array[i], end=' ')
        print('')

    def printSolution(self) -> None:
        if self.__solution == None:
            print("Problem not yet solved.")
            return
        if self.__points != None:
            pass
        self.__printArray(self.__solution)
        print('Weight: ', self.calcSolutionLen(self.__solution))
        
    def printOptimalSolution(self) -> None:
        if self.__optimalSolution == None:
            print("No optimal solution provided.")
            return
        self.__printArray(self.__optimalSolution)
        print('Weight: ', self.calcSolutionLen(self.__optimalSolution))

    def calcSolutionLen(self, solution: list[int]) -> int:
        distance = 0
        for i in range(len(solution)-1):
            #print("D: ", solution[i], "->", solution[i+1], " = ", end=' ')
            print( self.__adjacencyMatrix[solution[i]][solution[i+1]])
            distance += self.__adjacencyMatrix[solution[i]][solution[i+1]]
        #print("D: ", solution[len(solution)-1], "->", solution[0], " = ", self.__adjacencyMatrix[solution[len(solution)-1]][solution[0]])
        distance += self.__adjacencyMatrix[solution[len(solution)-1]][solution[0]]
        return distance

    def PRD(self) -> int:
        if self.__solution == None:
            print("No solution provided.")
            return
        if self.__optimalSolution == None:
            print("No optimal solution provided.")
            return 
        fx = self.calcSolutionLen(self.__solution)
        fref = self.calcSolutionLen(self.__optimalSolution)
        return (fx-fref)/fref

    def generateAsAsymetric(self, size: int, seed, minimumWeight: int, maximumWeight: int) -> None:
        random.seed(seed)
        self.valid = True
        self.__adjacencyMatrix = [[random.randint(minimumWeight,maximumWeight) for x in range(size)] for y in range(size)]
        for i in range(size):
            self.__adjacencyMatrix[i][i] = 0

    def generateAsSymteric(self, size: int, seed, minimumWeight: int, maximumWeight: int) -> None:
        self.generateAsymetric(size, seed, minimumWeight, maximumWeight)
        for x in range(0,size):
            for y in range(x+1, size):
                self.__adjacencyMatrix[x][y] = self.__adjacencyMatrix[y][x]

    def generateAsEuclid2D(self, size: int, seed, width: int) -> None:
        random.seed(seed)
        self.valid = True
        self.__points = [[random.random()*width for i in range(2)] for j in range(size)]
        self.__adjacencyMatrix = [[0 for x in range(size)] for y in range(size)]
        for p in range(size):
            for o in range(size):
                self.__adjacencyMatrixtrix[p][o] = round(math.sqrt(
                    math.pow(self.__points[p][0] - self.__points[o][0],2) + math.pow(self.__points[p][1] - self.__points[o][1],2)
                ))

def test():
    p = Problem()
    #p.loadSolutionFromFile("gr120.opt.tour")
    p.loadProblemFromFile("gr120.tsp")
    p.loadSolutionFromFile("gr120.opt.tour")
    p.printOptimalSolution()

if __name__ == "__main__":
    test()