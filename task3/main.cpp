#include <chrono>
#include <cstdlib>
#include <thread>

#include <iomanip>
#include <iostream>
#include <vector>

void Minor(int **matrix, int size, int row, int col, int **minor_matrix);

int Det(int **matrix, int size);

void RandomiseMatrix(int **matrix, int n);

void PrintMatrix(int **matrix, int n);

void Compute(int **matrix, int n, int i, int j, int **result);

int main() {
    int n;
    std::cout << "Enter matrix size > 0: \n";
    std::cin >> n;
    int **matrix;
    int **result;

    matrix = new int *[n];
    result = new int *[n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
        result[i] = new int[n];
    }
    RandomiseMatrix(matrix, n);
    int threadsNum = std::thread::hardware_concurrency();
    std::cout << "Enter threads count: \n";
    std::cin >> threadsNum;


    auto start = std::chrono::high_resolution_clock::now();// начальное время
    //omp_set_num_threads(threadsNum);
    //#pragma omp parallel for shared(matrix, result) private(i, j)
    //std::vector<std::thread> futures(threadsNum);
    std::thread *thr[threadsNum];
    int cntr = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            thr[cntr++] = new std::thread{Compute, matrix, n, i, j, result};
            //futures.push_back(std::async(Compute, matrix, n, i, j, result));
            // Compute(matrix, n, i, j, result);
            if (cntr == threadsNum) {
                // Когда количество запушенных потоков == максимуму, ждем пока все выполнятся.
                // Этот метод был самым простым и эффективным, на 2 секунды эффективнее openMp
                for (int m = 0; m < threadsNum; m++) {
                    thr[m]->join();
                    delete thr[m];
                }
                cntr = 0;
            }
        }
    }
    for (int i = 0; i < cntr; i++) {
        thr[i]->join();
        delete thr[i];
    }
    std::cout << "Matrix:\n";
    PrintMatrix(matrix, n);
    std::cout << "Result:\n";
    PrintMatrix(result, n);

    for (int l = 0; l < n; l++) {
        delete[] result[l];
        delete[] matrix[l];
    }
    delete[] result;
    delete[] matrix;

    // время работы программы
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "runtime = " << elapsed.count() * 1e-9 << " seconds\n";
}

void Minor(int **matrix, int size, int row, int col, int **minor_matrix) {
    int offset_row = 0;
    int offset_col = 0;
    for (int i = 0; i < size - 1; i++) {
        if (i == row) {
            offset_row = 1;
        }

        offset_col = 0;
        for (int j = 0; j < size - 1; j++) {
            if (j == col) {
                offset_col = 1;
            }
            minor_matrix[i][j] = matrix[i + offset_row][j + offset_col];
        }
    }
}

//Вычисление определителя матрицы разложение по первой строке
int Det(int **matrix, int size) {
    int det = 0;
    int degree = 1;
    if (size == 1) {
        return matrix[0][0];
    } else {


        for (int j = 0; j < size; j++) {
            int **new_matrix = new int *[size - 1];
            for (int i = 0; i < size - 1; i++) {
                new_matrix[i] = new int[size - 1];
            }


            Minor(matrix, size, 0, j, new_matrix);

            det = det + (degree * matrix[0][j] * Det(new_matrix, size - 1));
            degree = -degree;


            for (int i = 0; i < size - 1; i++) {
                delete[] new_matrix[i];
            }
            delete[] new_matrix;
        }


    }

    return det;
}

void RandomiseMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = std::rand() % 31;// NOLINT
        }
    }
}

void PrintMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << std::setw(11) << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void Compute(int **matrix, int n, int i, int j, int **result) {
    result[i][j] = 0;
    int degree = (i + j) % 2 == 0 ? 1 : -1;

    int **new_matrix = new int *[n - 1];
    for (int k = 0; k < n - 1; ++k) {
        new_matrix[k] = new int[n - 1];
    }
    Minor(matrix, n, i, j, new_matrix);
    result[i][j] = degree * Det(new_matrix, n - 1);

    for (int l = 0; l < n - 1; l++) {
        delete[] new_matrix[l];
    }
    delete[] new_matrix;
}