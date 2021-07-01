#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <vector>
#include <iostream>
#include <algorithm>

#define MAXN 100

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Please enter the input file name!\n");
        return -1;
    }
    FILE *fp = NULL;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("File doesn't exist!\n");
        return -1;
    }

    int n;
    int e;
    int i, j;

    int net[MAXN][MAXN];

    printf("Network:\n");
    int v = fscanf(fp, "%d", &n);
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            int v = fscanf(fp, "%d", &net[i][j]);
            printf("%d ", net[i][j]);
        }
        printf("\n");
    }
    printf("\n");   
    fclose(fp);

    bool valid[MAXN];
    std::vector<int> fragment[MAXN];
    int distance[MAXN][MAXN];
    for (int i = 0; i < n; i++) {
        fragment[i].push_back(i);
        valid[i] = 1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) distance[i][j] = INT_MAX;
            else {
                distance[i][j] = net[fragment[i][0]][fragment[j][0]];
            }
        }
    }

    for (int t=0; t<n-1; t++) {
        int min_distance = INT_MAX;
        int min_i, min_j;
        for (int i = 0; i < n; i++) {
            for (int j=0; j<n; j++) {
                if (i != j && valid[i] && valid[j] && distance[i][j] < min_distance) {
                    min_distance = distance[i][j];
                    min_i = i;
                    min_j = j;
                }
            }
        }

        printf("%d %d\n", min_i, min_j);

        if (net[fragment[min_i].front()][fragment[min_j].front()] == min_distance) {
            std::reverse(fragment[min_i].begin(), fragment[min_i].end());
            fragment[min_i].insert(fragment[min_i].end(), fragment[min_j].begin(), fragment[min_j].end());
            valid[min_j] = false;
        } else if (net[fragment[min_i].front()][fragment[min_j].back()] == min_distance) {
            std::reverse(fragment[min_i].begin(), fragment[min_i].end());
            std::reverse(fragment[min_j].begin(), fragment[min_j].end());
            fragment[min_i].insert(fragment[min_i].end(), fragment[min_j].begin(), fragment[min_j].end());
            valid[min_j] = false;
        } else if (net[fragment[min_i].back()][fragment[min_j].front()] == min_distance) {
            fragment[min_i].insert(fragment[min_i].end(), fragment[min_j].begin(), fragment[min_j].end());
            valid[min_j] = false;
        }
        else if (net[fragment[min_i].back()][fragment[min_j].back()] == min_distance) {
            std::reverse(fragment[min_j].begin(), fragment[min_j].end());
            fragment[min_i].insert(fragment[min_i].end(), fragment[min_j].begin(), fragment[min_j].end());
            valid[min_j] = false;
        }

        for (int j=0; j<n; j++) {
            if (valid[j] && min_i!=j) {
                min_distance = net[fragment[min_i].front()][fragment[j].front()];

                if (net[fragment[min_i].front()][fragment[j].back()] < min_distance)
                    min_distance = net[fragment[min_i].front()][fragment[j].back()];

                if (net[fragment[min_i].back()][fragment[j].front()] < min_distance)
                    min_distance = net[fragment[min_i].back()][fragment[j].front()];

                if (net[fragment[min_i].back()][fragment[j].back()] < min_distance)
                    min_distance = net[fragment[min_i].back()][fragment[j].back()];

                distance[min_i][j] = min_distance;
                distance[j][min_i] = min_distance;
            }
        }
    }

    int cost = 0;
    for (int i=0; i<n; i++) {
        if (valid[i]) {
            //fragment[i].push_back(fragment[i][0]);
            for (int j=0; j<fragment[i].size()-1; j++) {
                cost += net[fragment[i][j]][fragment[i][j+1]];
                printf("%d - ", fragment[i][j]);
            }
            printf("%d\n", fragment[i][n-1]);
            cost += net[fragment[i][0]][fragment[i][n-1]];
            break;
        }
    }

    printf("Cost: %d\n", cost);


    return 0;
}
