#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_RECORDS 1000

// the datastructure
typedef struct {
    int   container_id;  
    int   HH, MM, SS;     
    float weight;        
    int   product_code;
    int   temperature;    
    int   courier_id;     
} Record;

// record timestamp to seconds for easier comparison
int to_seconds(const Record *r) {
    return r->HH * 3600 + r->MM * 60 + r->SS;
}
int parseTime(char *timeStr, int *HH, int *MM, int *SS) {
    sscanf(timeStr, "%d:%d:%d", HH, MM, SS);
    return (*HH)*3600 + (*MM)*60 + (*SS);
}

int loadCSV(const char *filename, Record *all) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file\n");
        return 0;
    }

    char line[256];
    int count = 0;

    fgets(line, sizeof(line), f); // skip header

    while (fgets(line, sizeof(line), f)) {
        char timeStr[32];

        sscanf(line, "%d,%[^,],%f,%d,%d,%d",
               &all[count].container_id,
               timeStr,
               &all[count].weight,
               &all[count].product_code,
               &all[count].temperature,
               &all[count].courier_id);

        parseTime(timeStr,
                  &all[count].HH,
                  &all[count].MM,
                  &all[count].SS);

        count++;
    }
    fclose(f);
    return count;
}
void print_record(const Record *r) {
    printf("  [C%d] %02d:%02d:%02d  product=%-5d  weight=%5.1fkg  temp=%2dC  courier=%d\n",
           r->container_id,
           r->HH, r->MM, r->SS,
           r->product_code,
           r->weight,
           r->temperature,
           r->courier_id);
}
// Task 1 - merge sort
// Merge two sorted halves of arr[] back into arr[]
static void merge(Record *arr, Record *tmp, int lo, int mid, int hi) {
    int i = lo, j = mid, k = lo;

    while (i < mid && j < hi) {
        if (to_seconds(&arr[i]) <= to_seconds(&arr[j]))
            tmp[k++] = arr[i++];
        else
            tmp[k++] = arr[j++];
    }
    while (i < mid) tmp[k++] = arr[i++];
    while (j < hi)  tmp[k++] = arr[j++];

    /* copy merged result back into arr */
    for (int x = lo; x < hi; x++)
        arr[x] = tmp[x];
}

// Recursive merge sort
static void merge_sort_helper(Record *arr, Record *tmp, int lo, int hi) {
    if (hi - lo <= 1) return;          /* single record, already sorted */
    int mid = (lo + hi) / 2;
    merge_sort_helper(arr, tmp, lo, mid);
    merge_sort_helper(arr, tmp, mid, hi);
    merge(arr, tmp, lo, mid, hi);
}

void task1_merge_sort(Record *arr, int n) {
    if (n <= 1) return;
    Record *tmp = malloc(n * sizeof(Record));
    if (!tmp) { fprintf(stderr, "malloc failed in merge sort\n"); exit(1); }
    merge_sort_helper(arr, tmp, 0, n);
    free(tmp);
}


// Task 2 -  K-way heap merge
#define K 4   

typedef struct {
    int    time_sec;   
    int    file_idx;    
    Record record;
} HeapNode;

//min heap helper
static void heap_swap(HeapNode *h, int a, int b) {
    HeapNode tmp = h[a]; h[a] = h[b]; h[b] = tmp;
}
static void heap_push(HeapNode *heap, int *size, HeapNode node) {
    heap[(*size)++] = node;
    int i = *size - 1;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent].time_sec <= heap[i].time_sec) break;
        heap_swap(heap, parent, i);
        i = parent;
    }
}

static HeapNode heap_pop(HeapNode *heap, int *size) {
    HeapNode top = heap[0];
    heap[0] = heap[--(*size)];
    int i = 0;
    while (1) {
        int left = 2*i+1, right = 2*i+2, smallest = i;
        if (left  < *size && heap[left].time_sec  < heap[smallest].time_sec) smallest = left;
        if (right < *size && heap[right].time_sec < heap[smallest].time_sec) smallest = right;
        if (smallest == i) break;
        heap_swap(heap, i, smallest);
        i = smallest;
    }
    return top;
}

int task2_kway_merge(Record *files[K], int sizes[K], Record *out) {
    HeapNode heap[K];
    int      heap_size = 0;
    int      ptrs[K];
    int      total = 0;

    // seed heap with first record from each non-empty file
    for (int i = 0; i < K; i++) {
        ptrs[i] = 0;
        if (sizes[i] > 0) {
            HeapNode node = {
                to_seconds(&files[i][0]),
                i,
                files[i][0]
            };
            heap_push(heap, &heap_size, node);
            ptrs[i] = 1;
        }
    }

    while (heap_size > 0) {
        HeapNode winner = heap_pop(heap, &heap_size);
        out[total++] = winner.record;

        int fi = winner.file_idx;
        if (ptrs[fi] < sizes[fi]) {
            HeapNode node = {
                to_seconds(&files[fi][ptrs[fi]]),
                fi,
                files[fi][ptrs[fi]]
            };
            heap_push(heap, &heap_size, node);
            ptrs[fi]++;
        }
    }

    return total;
}


// Task 3 - Binary search
// Build the parallel key array -- call once after Task 2 
int *task3_build_keys(const Record *dispatch, int n) {
    int *keys = malloc(n * sizeof(int));
    if (!keys) { fprintf(stderr, "malloc failed building keys\n"); exit(1); }
    for (int i = 0; i < n; i++)
        keys[i] = to_seconds(&dispatch[i]);
    return keys;
}
int task3_binary_search(const int *keys, int n,
                        const Record *dispatch,
                        int target_sec, int product_code) {
    int lo = 0, hi = n;

    // standard left-boundary binary search
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (keys[mid] < target_sec)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo >= n) return -1;  



    if (product_code < 0)
        return lo;            

    // scan forward
    for (int i = lo; i < n; i++) {
        if (dispatch[i].product_code == product_code)
            return i;
    }

    return -1;
}
//Task 4 - Hash map count 
typedef struct {
    int   container_id;
    int   count;
    float avg_weight;
    float avg_temp;
    int   courier_flags;  
} ContainerReport;

void task4_count_report(const Record *dispatch, int n,
                        ContainerReport reports[K]) {

    /*accumulators */
    int   counts[K]       = {0};
    float weight_sum[K]   = {0.0f};
    float temp_sum[K]     = {0.0f};
    int   courier_seen[K] = {0};

    for (int i = 0; i < n; i++) {
        int idx = dispatch[i].container_id - 1; 
        counts[idx]++;
        weight_sum[idx]   += dispatch[i].weight;
        temp_sum[idx]     += dispatch[i].temperature;
        courier_seen[idx] |= (1 << (dispatch[i].courier_id - 1));
    }

    for (int i = 0; i < K; i++) {
        reports[i].container_id   = i + 1;
        reports[i].count          = counts[i];
        reports[i].avg_weight     = counts[i] > 0 ? weight_sum[i] / counts[i] : 0.0f;
        reports[i].avg_temp       = counts[i] > 0 ? temp_sum[i]   / counts[i] : 0.0f;
        reports[i].courier_flags  = courier_seen[i];
    }
}

void print_report(const ContainerReport reports[K]) {
    printf("\n  %-14s %8s %12s %10s %10s\n",
           "Container", "Products", "Avg weight", "Avg temp", "Couriers");
    printf("  %s\n", "------------------------------------------------------------");
    for (int i = 0; i < K; i++) {
        printf("  Container %02d   %8d %9.1f kg %8.1f C   ",
               reports[i].container_id,
               reports[i].count,
               reports[i].avg_weight,
               reports[i].avg_temp);
        for (int c = 0; c < K; c++)
            if (reports[i].courier_flags & (1 << c))
                printf("%d ", c + 1);
        printf("\n");
    }
}


// wait for enter before moving to the next step
void pause(void) {
    printf("\n  Press ENTER to continue...");
    while (getchar() != '\n');
}

void print_header(int task, const char *desc) {
    printf("\n-------------------------------------------------\n");
    printf(" TASK %d -- %s\n", task, desc);
    printf("-------------------------------------------------\n");
}

int main(void) {
    Record all[MAX_RECORDS];
    int total_records = loadCSV("Testdata Algorithms.csv", all);

    if (total_records == 0) {
        printf("No records loaded -- check the CSV file is present.\n");
        return 1;
    }

    Record file_A[MAX_RECORDS], file_B[MAX_RECORDS],
           file_C[MAX_RECORDS], file_D[MAX_RECORDS];
    int nA=0, nB=0, nC=0, nD=0;

    for (int i = 0; i < total_records; i++) {
        if      (all[i].container_id == 1) file_A[nA++] = all[i];
        else if (all[i].container_id == 2) file_B[nB++] = all[i];
        else if (all[i].container_id == 3) file_C[nC++] = all[i];
        else if (all[i].container_id == 4) file_D[nD++] = all[i];
    }

    // step 1 -- raw data as loaded from the csv
    print_header(0, "Raw test data from CSV");
    printf("  %d records loaded from file.\n\n", total_records);
    printf("  %-4s %-10s %-8s %-8s %-5s %-8s\n",
           "Cont", "Time", "Weight", "Product", "Temp", "Courier");
    printf("  %s\n", "------------------------------------------------------");
    for (int i = 0; i < total_records; i++)
        print_record(&all[i]);

    pause();

    // step 2 -- task 1: each file sorted by timestamp
    print_header(1, "Each file sorted by time  O(N log N)");

    task1_merge_sort(file_A, nA);
    task1_merge_sort(file_B, nB);
    task1_merge_sort(file_C, nC);
    task1_merge_sort(file_D, nD);

    printf("\n  Container 1 -- %d records:\n", nA);
    for (int i = 0; i < nA; i++) print_record(&file_A[i]);
    printf("\n  Container 2 -- %d records:\n", nB);
    for (int i = 0; i < nB; i++) print_record(&file_B[i]);
    printf("\n  Container 3 -- %d records:\n", nC);
    for (int i = 0; i < nC; i++) print_record(&file_C[i]);
    printf("\n  Container 4 -- %d records:\n", nD);
    for (int i = 0; i < nD; i++) print_record(&file_D[i]);

    pause();

    // step 3 -- task 2: single merged dispatch list
    print_header(2, "Merged dispatch list  O(N)");

    Record dispatch[MAX_RECORDS];
    Record *files[K] = { file_A, file_B, file_C, file_D };
    int sizes[K] = { nA, nB, nC, nD };

    int n = task2_kway_merge(files, sizes, dispatch);

    printf("\n  %d records merged into one time-ordered list:\n\n", n);
    for (int i = 0; i < n; i++) print_record(&dispatch[i]);

    pause();

    // step 4 -- task 3: interactive search
    print_header(3, "Search dispatch list  O(log N)");

    int *keys = task3_build_keys(dispatch, n);

    char again = 'y';
    while (again == 'y' || again == 'Y') {
        char time_input[32];
        int  prod_input;
        int  hh, mm, ss;

        printf("\n  Enter search time (HH:MM:SS): ");
        scanf("%31s", time_input);
        printf("  Enter product code (-1 for any): ");
        scanf("%d", &prod_input);
        // clear leftover newline so pause() works afterwards
        while (getchar() != '\n');

        if (sscanf(time_input, "%d:%d:%d", &hh, &mm, &ss) != 3
                || hh < 0 || hh > 23 || mm < 0 || mm > 59 || ss < 0 || ss > 59) {
            printf("  Invalid time format -- please use HH:MM:SS\n");
        } else {
            int target = hh * 3600 + mm * 60 + ss;
            int idx = task3_binary_search(keys, n, dispatch, target, prod_input);

            if (idx >= 0) {
                printf("\n  Result (dispatch index %d):\n", idx);
                print_record(&dispatch[idx]);
            } else {
                printf("\n  No record found matching those criteria.\n");
            }
        }

        printf("\n  Search again? (y/n): ");
        scanf(" %c", &again);
        while (getchar() != '\n');
    }

    free(keys);

    pause();

    // step 5 -- task 4: container summary report
    print_header(4, "Container product count report  O(N)");

    ContainerReport reports[K];
    task4_count_report(dispatch, n, reports);
    print_report(reports);

    printf("\n-------------------------------------------------\n");
    printf(" Demo complete.\n");
    printf("-------------------------------------------------\n\n");

    return 0;
}