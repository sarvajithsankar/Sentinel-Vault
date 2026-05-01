#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include "LogEntry.h"

using namespace std;

inline void merge(vector<LogEntry>& vault, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<LogEntry> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = vault[left + i];
    for (int j = 0; j < n2; j++) R[j] = vault[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i].timestamp <= R[j].timestamp) {
            vault[k] = L[i];
            i++;
        } else {
            vault[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vault[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vault[k] = R[j];
        j++;
        k++;
    }
}

inline void mergeSort(vector<LogEntry>& vault, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(vault, left, mid);
        mergeSort(vault, mid + 1, right);

        merge(vault, left, mid, right);
    }
}

#endif
