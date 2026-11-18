#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

int** G1 = NULL;
int** G2 = NULL;
int n1 = 0;
int n2 = 0;

int** create_matrix(int size) {
    int** m = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        m[i] = (int*)malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) m[i][j] = 0;
    }
    return m;
}

void free_matrix(int** m, int size) {
    if (m == NULL) return;
    for (int i = 0; i < size; i++) free(m[i]);
    free(m);
}

void print_matrix(int** m, int size, const char* name) {
    printf("\n%s (%dx%d):\n   ", name, size, size);
    for (int j = 0; j < size; j++) printf("%3d", j + 1);
    printf("\n");
    for (int i = 0; i < size; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < size; j++) printf("%3d", m[i][j]);
        printf("\n");
    }
}

//функ для удадения
int** remove_isolated_vertices(int** M, int* size, const char* matrix_name) {
    int* to_delete = (int*)malloc((*size) * sizeof(int));
    int delete_count = 0;

    // поиск ив
    for (int i = 0; i < *size; i++) {
        int has_edges = 0;
        for (int j = 0; j < *size; j++) {
            if (M[i][j] == 1) { has_edges = 1; break; }
        }
        if (!has_edges) {
            to_delete[delete_count++] = i;
        }
    }

    if (delete_count == 0) {
        free(to_delete);
        return M;
    }

    int new_size = *size - delete_count;
    if (new_size == 0) {
        printf("\n%s: все вершины изолированы (пустой граф)\n", matrix_name);
        free(to_delete);
        free_matrix(M, *size);
        *size = 0;
        return NULL;
    }

    int** new_M = create_matrix(new_size);
    int r = 0;
    for (int i = 0; i < *size; i++) {
        int skip_i = 0;
        for (int k = 0; k < delete_count; k++) {
            if (to_delete[k] == i) { skip_i = 1; break; }
        }
        if (skip_i) continue;

        int c = 0;
        for (int j = 0; j < *size; j++) {
            int skip_j = 0;
            for (int k = 0; k < delete_count; k++) {
                if (to_delete[k] == j) { skip_j = 1; break; }
            }
            if (skip_j) continue;

            new_M[r][c] = M[i][j];
            c++;
        }
        r++;
    }

    free_matrix(M, *size);
    *size = new_size;
    free(to_delete);
    return new_M;
}

// зад 1 ген и списк смеж
void generate_graphs(void) {
    printf("Введите число вершин для G1: ");
    (void)scanf("%d", &n1);
    if (n1 < 2) { printf("Минимум 2 вершины для G1\n"); return; }

    printf("Введите число вершин для G2: ");
    (void)scanf("%d", &n2);
    if (n2 < 2) { printf("Минимум 2 вершины для G2\n"); return; }

    if (G1) free_matrix(G1, n1);
    if (G2) free_matrix(G2, n2);

    G1 = create_matrix(n1);
    G2 = create_matrix(n2);

    srand((unsigned)time(NULL));

    // зап г1
    for (int i = 0; i < n1; i++) {
        for (int j = i + 1; j < n1; j++) {
            if (rand() % 2 == 1) { G1[i][j] = 1; G1[j][i] = 1; }
        }
    }

    // зап г2
    for (int i = 0; i < n2; i++) {
        for (int j = i + 1; j < n2; j++) {
            if (rand() % 2 == 1) { G2[i][j] = 1; G2[j][i] = 1; }
        }
    }

    // ген петель
    for (int i = 0; i < n1; i++) {
        if (rand() % 2 == 1) G1[i][i] = 1;
    }
    for (int i = 0; i < n2; i++) {
        if (rand() % 2 == 1) G2[i][i] = 1;
    }

    print_matrix(G1, n1, "Матрица G1");
    print_matrix(G2, n2, "Матрица G2");
}

// Список смежности
void print_adj_list(int** m, int size, const char* name) {
    printf("\nСписок смежности %s:\n", name);
    for (int i = 0; i < size; i++) {
        printf("Вершина %d: ", i + 1);
        int has = 0;
        for (int j = 0; j < size; j++) {
            if (m[i][j] == 1) { printf("%d ", j + 1); has = 1; }
        }
        if (!has) printf("(нет связей)");
        printf("\n");
    }
}

// 2а Отождествление вершин u и v
void identify_vertices(int** A, int size, int u, int v) {
    if (u < 0 || u >= size || v < 0 || v >= size || u == v) {
        printf("Неверные номера вершин!\n"); return;
    }
    int new_n = size - 1;
    int** C = create_matrix(new_n);
    int mapping[100];
    int idx = 0;
    for (int i = 0; i < size; i++) {
        if (i != u && i != v) mapping[i] = idx++;
    }
    int z = idx;

    for (int i = 0; i < size; i++) {
        if (i == u || i == v) continue;
        for (int j = 0; j < size; j++) {
            if (j == u || j == v) continue;
            C[mapping[i]][mapping[j]] = A[i][j];
        }
    }
    for (int i = 0; i < size; i++) {
        if (i == u || i == v) continue;
        if (A[u][i] == 1 || A[v][i] == 1) {
            C[z][mapping[i]] = 1;
            C[mapping[i]][z] = 1;
        }
    }
    print_matrix(C, new_n, "Отождествление вершин");
    free_matrix(C, new_n);
}

// 2б. Стягивание ребра 
void contract_edge(int** A, int size, int u, int v) {
    if (u < 0 || u >= size || v < 0 || v >= size) {
        printf("Неверные номера вершин!\n"); return;
    }
    if (A[u][v] == 0) {
        printf("Вершины %d и %d не смежны — стягивание невозможно!\n", u + 1, v + 1);
        return;
    }
    printf("Стягивание ребра (%d, %d):\n", u + 1, v + 1);
    identify_vertices(A, size, u, v);
}

// 2в. Расщепление вершины u
void split_vertex(int** A, int size, int u) {
    if (u < 0 || u >= size) { printf("Неверная вершина!\n"); return; }

    int has_edges = 0;
    for (int i = 0; i < size; i++) {
        if (A[u][i] == 1) { has_edges = 1; break; }
    }
    if (!has_edges) { printf("У вершины нет связей — расщеплять нечего!\n"); return; }

    int new_n = size + 1;
    int** C = create_matrix(new_n);

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            C[i][j] = A[i][j];

    for (int i = 0; i < size; i++) {
        if (A[u][i] == 1) {
            C[new_n - 1][i] = 1;
            C[i][new_n - 1] = 1;
        }
    }

    if (A[u][u] == 1) {
        C[new_n - 1][new_n - 1] = 1;
    }

    C[u][new_n - 1] = 1;
    C[new_n - 1][u] = 1;

    print_matrix(C, new_n, "Расщепление вершины");
    free_matrix(C, new_n);
}

// зад 3 бин операции 
void binary_ops(void) {
    if (G1 == NULL || G2 == NULL) { printf("Сначала сгенерируйте графы (пункт 1)!\n"); return; }

    int max_n = (n1 > n2) ? n1 : n2;
    int min_n = (n1 < n2) ? n1 : n2;

    if (n1 != n2) {
        printf("\nВНИМАНИЕ: Размерности графов разные (G1: %dx%d, G2: %dx%d).\n", n1, n1, n2, n2);
    }

    // объед
    int** OR_m = create_matrix(max_n);
    for (int i = 0; i < max_n; i++) {
        for (int j = 0; j < max_n; j++) {
            int val1 = (i < n1 && j < n1) ? G1[i][j] : 0;
            int val2 = (i < n2 && j < n2) ? G2[i][j] : 0;
            OR_m[i][j] = val1 | val2;
        }
    }
    print_matrix(OR_m, max_n, "Объединение G1 U G2");
    free_matrix(OR_m, max_n);

    // пересечение 
    int** AND_m = create_matrix(min_n);
    for (int i = 0; i < min_n; i++) {
        for (int j = 0; j < min_n; j++) {
            AND_m[i][j] = G1[i][j] & G2[i][j];
        }
    }
    
    print_matrix(AND_m, min_n, "Пересечение G1 ∩ G2");
    free_matrix(AND_m, min_n);

    // кольц
    int** XOR_m = create_matrix(max_n);
    for (int i = 0; i < max_n; i++) {
        for (int j = 0; j < max_n; j++) {
            int val1 = (i < n1 && j < n1) ? G1[i][j] : 0;
            int val2 = (i < n2 && j < n2) ? G2[i][j] : 0;
            XOR_m[i][j] = val1 ^ val2;
        }
    }
    // удаление 
    int xor_size = max_n;
    XOR_m = remove_isolated_vertices(XOR_m, &xor_size, "Кольцевая сумма G1 ⊕ G2");
    if (xor_size > 0) {
        print_matrix(XOR_m, xor_size, "Кольцевая сумма G1 ⊕ G2 (без изолированных)");
        free_matrix(XOR_m, xor_size);
    }
}

// зад 4 дек произв
void cartesian_product(void) {
    if (G1 == NULL || G2 == NULL) { printf("Сначала сгенерируйте графы (пункт 1)!\n"); return; }

    int new_n = n1 * n2;
    int** C = create_matrix(new_n);

    for (int i1 = 0; i1 < n1; i1++) {
        for (int i2 = 0; i2 < n2; i2++) {
            int v1 = i1 * n2 + i2;
            for (int j1 = 0; j1 < n1; j1++) {
                for (int j2 = 0; j2 < n2; j2++) {
                    int v2 = j1 * n2 + j2;

                    if ((i1 == j1 && G2[i2][j2] == 1) || (i2 == j2 && G1[i1][j1] == 1)) {
                        C[v1][v2] = 1;
                    }
                }
            }
        }
    }

    printf("\nДекартово произведение G1 x G2 (размер %d x %d):\n", new_n, new_n);
    print_matrix(C, new_n, "Результат");

    free_matrix(C, new_n);
}

int main() {
    setlocale(LC_ALL, "Russian");
    int choice, u, v;

    do {
        printf("\n=== ЛР 6: Операции над графами ===\n");
        printf("1 - Задание 1: Сгенерировать G1 и G2\n");
        printf("2 - Задание 1.2*: Вывести списки смежности\n");
        printf("3 - Задание 2а: Отождествление вершин\n");
        printf("4 - Задание 2б: Стягивание ребра\n");
        printf("5 - Задание 2в: Расщепление вершины\n");
        printf("6 - Задание 3: OR, AND, XOR\n");
        printf("7 - Задание 4*: Декартово произведение\n");
        printf("0 - Выход\n");
        printf("Ваш выбор: ");
        (void)scanf("%d", &choice);

        switch (choice) {
        case 1: generate_graphs(); break;
        case 2:
            if (G1) print_adj_list(G1, n1, "G1");
            if (G2) print_adj_list(G2, n2, "G2");
            break;
        case 3:
            if (!G1) { printf("Сначала создайте графы\n"); break; }
            printf("Введите 2 вершины для отождествления (от 1 до %d): ", n1);
            (void)scanf("%d %d", &u, &v);
            identify_vertices(G1, n1, u - 1, v - 1);
            break;
        case 4:
            if (!G1) { printf("Сначала создайте графы\n"); break; }
            printf("Введите 2 вершины для стягивания (от 1 до %d): ", n1);
            (void)scanf("%d %d", &u, &v);
            contract_edge(G1, n1, u - 1, v - 1);
            break;
        case 5:
            if (!G1) { printf("Сначала создайте графы\n"); break; }
            printf("Введите вершину для расщепления (от 1 до %d): ", n1);
            (void)scanf("%d", &u);
            split_vertex(G1, n1, u - 1);
            break;
        case 6: binary_ops(); break;
        case 7: cartesian_product(); break;
        case 0:
            if (G1) free_matrix(G1, n1);
            if (G2) free_matrix(G2, n2);
            printf("Выход.\n");
            break;
        default: printf("Неверный выбор\n");
        }
    } while (choice != 0);

    return 0;
}