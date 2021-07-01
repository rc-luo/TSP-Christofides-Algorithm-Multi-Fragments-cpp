#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <vector>

#include "PerfectMatching.h"
#include "GEOM/GeomPerfectMatching.h"

#define MAXN 100

int minKey(int key[], bool mstSet[], int V);
void primMST(int graph[MAXN][MAXN], int V, int mst[MAXN][MAXN]);
int findOdd(int V, int mst[MAXN][MAXN], int odd_list[MAXN]);
int buildPM(int net[MAXN][MAXN], int mst[MAXN][MAXN], int nOdd, int odd_list[MAXN]);
void traceEuler(int u, int n, int mst[MAXN][MAXN], std::vector<int> *trace);
bool isValid(int u, int v, int n, int mst[MAXN][MAXN]);
int dfsCount(int v, int n, int mst[MAXN][MAXN],int visited[MAXN]);
void skip(std::vector<int> trace, std::vector<int> *tsp);

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
    int mst[MAXN][MAXN];
    int odd_list[MAXN];
    std::vector<int> trace;
    std::vector<int> tsp;
    memset(mst, 0, MAXN * MAXN * sizeof(int));

    printf("Network:\n");
    int valid = fscanf(fp, "%d", &n);
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            int valid = fscanf(fp, "%d", &net[i][j]);
            printf("%d ", net[i][j]);
        }
        printf("\n");
    }
    printf("\n");   
    fclose(fp);

    printf("MST:\n");
    primMST(net, n, mst);
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            printf("%d ", mst[i][j]);
        }
        printf("\n");
    }
    printf("\n"); 
    
    printf("Odd_List:\n");
    int nOdd = findOdd(n, mst, odd_list);
    for (int i=0; i<nOdd; i++) {
        printf("%d ", odd_list[i]);
    }
    printf("\n\n");
    
    printf("Perfect_Matching:\n"); 
    buildPM(net, mst, nOdd, odd_list);
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            printf("%d ", mst[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("Euler_Trace:\n");
    trace.push_back(0);
    traceEuler(0, n, mst, &trace);
    for (i=0; i<trace.size(); i++) {
        if (i < trace.size()-1) {
            printf("%d - ", trace[i]);
        }
        else {
            printf("%d\n", trace[i]);
        }
    }
    printf("\n");

    printf("TSP:\n");    
    skip(trace, &tsp);
    int cost = 0;
    for (int i=0; i<tsp.size(); i++) {
        if (i < tsp.size() - 1) {
            cost += net[tsp[i]][tsp[i+1]];
            printf("%d - ", tsp[i]);
        }
        else {
            printf("%d\n", tsp[i]);
        }
    }
    printf("\n");

    printf("Cost: %d\n", cost);

    trace.clear();
    tsp.clear();

    return 0;
}

void primMST(int graph[MAXN][MAXN], int V, int mst[MAXN][MAXN]) 
{  
    int parent [MAXN]; 
    int key[MAXN]; 
      
    bool mstSet[MAXN]; 
  
    for (int i = 0; i < MAXN; i++) {
        key[i] = INT_MAX; 
        mstSet[i] = false; 
    }
 
    key[0] = 0; 
    parent[0] = -1; // First node is always root of MST 
  
    for (int count = 0; count < V - 1; count++)
    { 
        int u = minKey(key, mstSet, V); 
        mstSet[u] = true; 
  
        for (int v = 0; v < V; v++) {
            if (mstSet[v] == false && graph[u][v] < key[v]) {
                parent[v] = u;
                key[v] = graph[u][v]; 
            }
        }
    } 
  
    for (int i=1; i<V; i++) {
        mst[parent[i]][i] = 1;
        mst[i][parent[i]] = 1;
    }
} 

int buildPM(int net[MAXN][MAXN], int mst[MAXN][MAXN], int nOdd, int odd_list[MAXN])
{
    PerfectMatching *pm = new PerfectMatching(nOdd, nOdd*(nOdd-1)/2);
    for (int i=0; i<nOdd; i++) {
        for (int j=i+1; j<nOdd; j++) {
            pm->AddEdge(i, j, net[odd_list[i]][odd_list[j]]);
        }
    }
    pm->Solve();
    for (int i=0; i<nOdd; i++) {
        int j = pm->GetMatch(i);
        mst[odd_list[i]][odd_list[j]] += 1;
    }
}

int findOdd(int V, int mst[MAXN][MAXN], int odd_list[MAXN]) 
{
    int cnt = 0;
    for (int i=0; i<V; i++) {
        int sum = 0;
        for (int j=0; j<V; j++) {
            sum += mst[i][j];
        }
        if (sum % 2) {
            odd_list[cnt] = i;
            cnt++;
        }
    }
    return cnt;
}

void traceEuler(int u, int n, int mst[MAXN][MAXN], std::vector<int> *trace)
{
    for (int i=0; i<n; i++) {
        if (mst[u][i] > 0) {
            int v = i;
            if (isValid(u, v, n, mst)) {
                trace->push_back(v);
                mst[u][v] -= 1;
                mst[v][u] -= 1;
                traceEuler(v, n, mst, trace);
            }
        } 
    }
}

void skip(std::vector<int> trace, std::vector<int> *tsp)
{
    for (int i=0; i<trace.size(); i++) {
        bool first = true;
        for (int j=0; j<tsp->size(); j++) {
            if ((*tsp)[j] == trace[i]) {
                first = false; 
                break;
            }
        }
        if (first) {
            tsp->push_back(trace[i]);
        }
    }
    tsp->push_back(0); // round-trip
}

bool isValid(int u, int v, int n, int mst[MAXN][MAXN])
{
    int cnt=0; 
    for (int i=0; i<n; i++) {
        if (mst[u][i] > 0) {
            cnt++; 
        }
    }
    if (cnt==1) return true;

    int visited[MAXN];
    memset(visited, false, MAXN*sizeof(int));
    int cnt1 = dfsCount(u, n, mst, visited); 

    mst[u][v] -= 1;
    mst[v][u] -= 1;
    memset(visited, false, MAXN*sizeof(int));
    int cnt2 = dfsCount(u, n, mst, visited);

    mst[u][v] += 1;
    mst[v][u] += 1;

    return (cnt1 > cnt2)? false: true;
}

int dfsCount(int v, int n, int mst[MAXN][MAXN],int visited[MAXN]) 
{
    visited[v] = true; 
    int cnt = 1;
    for (int i=0; i<n; i++) {
        if (mst[v][i] > 0 && !visited[i]) {
            cnt += dfsCount(i, n, mst, visited);
        }
    }
    return cnt;
}

int minKey(int key[], bool mstSet[], int V) 
{ 
    // Initialize min value 
    int min = INT_MAX, min_index; 
  
    for (int v = 0; v < V; v++) 
        if (mstSet[v] == false && key[v] < min) 
            min = key[v], min_index = v; 
  
    return min_index; 
} 
