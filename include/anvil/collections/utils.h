#pragma once

namespace anvil {

template <typename T>
void
Swap (T *a, T *b) {
    T temp = *a;
    *a     = *b;
    *b     = temp;
}

template <typename T, typename Comparator>
T *
Partition (T *begin, T *end, Comparator cmp) {
    T *pivot = end - 1;
    T *i     = begin;

    for (T *j = begin; j < pivot; ++j) {
        if (cmp (*j, *pivot)) {
            Swap (i, j);
            i++;
        }
    }
    Swap (i, pivot);
    return i;
}

template <typename Container, typename Comparator>
void
Sort (Container &c, Comparator cmp) {
    Sort (c.Begin (), c.End (), cmp);
}

template <typename T, typename Comparator>
void
Sort (T *begin, T *end, Comparator cmp) {
    if (begin >= end || (end - begin) <= 1) {
        return;
    }

    T *pivotIndex = Partition (begin, end, cmp);

    Sort (begin, pivotIndex, cmp);
    Sort (pivotIndex + 1, end, cmp);
}

}
