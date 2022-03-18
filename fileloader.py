import tsplib95

class FileLoader:
    def __fill_matrix_euc(self, instance, size):
        matrix = [[0 for x in range(size)] for y in range(size)]
        for i in range(size):
            for j in range(size):
                matrix[i][j] = instance.get_weight(i + 1, j + 1)
        return matrix


    def __fill_matrix_full(self, instance, size):
        matrix = [[0 for x in range(size)] for y in range(size)]
        for i in range(size):
            for j in range(size):
                matrix[i][j] = instance.get_weight(i, j)
        return matrix


    def __fill_matrix_full_twod(self, instance, size):
        return self.__fill_matrix_euc(instance, size)


    def __fill_matrix_lower(self, instance, size):
        matrix = [[0 for x in range(size)] for y in range(size)]
        for i in range(size):
            for j in range(size):
                matrix[i][j] = instance.get_weight(i, j)
        for i in range(size):
            for j in range(i, size):
                matrix[i][j] = matrix[j][i]
        return matrix


    def __fill_matrix_lower_twod(self, instance, size):
        matrix = [[0 for x in range(size)] for y in range(size)]
        for i in range(size):
            for j in range(i + 1):
                matrix[i][j] = instance.get_weight(i + 1, j + 1)
        for i in range(size):
            for j in range(i, size):
                matrix[i][j] = matrix[j][i]
        return matrix


    def loadProblemInstance(self, filepath):
        instance = tsplib95.load(filepath)
        size = instance.dimension

        if instance.edge_weight_type == "EUC_2D":
            matrix = self.__fill_matrix_euc(instance, size)
            return matrix
        elif instance.edge_weight_format == "FULL_MATRIX":
            if instance.display_data_type == "TWOD_DISPLAY":
                matrix = self.__fill_matrix_full_twod(instance, size)
            else:
                matrix = self.__fill_matrix_full(instance, size)
            return matrix
        elif instance.edge_weight_format == "LOWER_DIAG_ROW":
            if instance.display_data_type == "TWOD_DISPLAY":
                matrix = self.__fill_matrix_lower_twod(instance, size)
            else:
                matrix = self.__fill_matrix_lower(instance, size)
            return matrix

    def loadOptimalSolution(self, filepath):
        optimal = tsplib95.load(filepath)
        # Verticles are numbered from 1 in the file
        solution = optimal.tours[0]
        for i in range(len(solution)):
            solution[i] -= 1
        return solution
