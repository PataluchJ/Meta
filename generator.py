import random
import math
class Generator:
    def __init__(self):
        pass

    def generateAsymetric(self, size, seed, minimumWeight, maximumWeight):
        random.seed(seed)
        matrix = [[random.randint(minimumWeight,maximumWeight) for x in range(size)] for y in range(size)]
        for i in range(size):
            matrix[i][i] = 0
        return matrix

    def generateSymteric(self, size, seed,minimumWeight, maximumWeight):
        matrix = self.generateAsymetric(size, seed, minimumWeight, maximumWeight)
        for x in range(0,size):
            for y in range(x+1, size):
                matrix[x][y] = matrix[y][x]
        return matrix

    def generateEuclid2D(self, size, seed, width):
        random.seed(seed)
        points = [[random.random()*width for i in range(2)] for j in range(size)]
        matrix = [[0 for x in range(size)] for y in range(size)]
        for p in range(size):
            for o in range(size):
                matrix[p][o] = round(math.sqrt(
                    math.pow(points[p][0] - points[o][0],2) + math.pow(points[p][1] - points[o][1],2)
                ))
        return matrix