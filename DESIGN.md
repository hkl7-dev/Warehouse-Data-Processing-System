# Design Document

## Task 2 — K-Way Heap Merge Flowchart

```
[![K-Way Merge Flowchart](https://github.com/user-attachments/assets/ad61c5c3-f2a4-4f80-87ce-a3b306c76016)](https://github.com/user-attachments/assets/ad61c5c3-f2a4-4f80-87ce-a3b306c76016)```

---

## Pseudocode

### Task 1 — Merge Sort — O(N log N)

```
FUNCTION merge_sort(file):
    IF the file has 1 or fewer records THEN
        return the file as-is (already sorted)

    split the file down the middle into left_half and right_half

    sorted_left  = merge_sort(left_half)
    sorted_right = merge_sort(right_half)

    return merge(sorted_left, sorted_right)

FUNCTION merge(left, right):
    result = empty list
    i = 0   (pointer for left)
    j = 0   (pointer for right)

    WHILE there are still records in both left and right:
        convert left[i] timestamp to seconds  (HH*3600 + MM*60 + SS)
        convert right[j] timestamp to seconds

        IF left[i] timestamp <= right[j] timestamp THEN
            add left[i] to result
            move i forward by 1
        ELSE
            add right[j] to result
            move j forward by 1

    add any remaining records from left to result
    add any remaining records from right to result

    return result

RUN FOR EACH of the 4 team files:
    sorted_file = merge_sort(team_file)
```

---

### Task 2 — K-Way Heap Merge — O(N)

```
FUNCTION kway_merge(file_A, file_B, file_C, file_D):
    files    = [file_A, file_B, file_C, file_D]
    pointers = [0, 0, 0, 0]   (one index pointer per file, all start at record 0)
    heap     = empty min-heap
    dispatch = empty list

    -- Seed the heap with the first record from each file
    FOR each file i from 0 to 3:
        IF file i is not empty THEN
            push (timestamp_in_seconds, file_index=i, record) onto heap

    -- Repeatedly pull the earliest record across all files
    WHILE heap is not empty:
        pop the record with the smallest timestamp off the heap
        call it (time, file_index, record)

        add record to dispatch list
        move that file's pointer forward by 1

        IF that file still has records left THEN
            push the next record from that file onto the heap

    return dispatch
```

---

### Task 3 — Binary Search — O(log N)

```
FUNCTION build_key_list(dispatch):
    keys = empty list
    FOR each record in dispatch:
        key = record.HH * 3600 + record.MM * 60 + record.SS
        add key to keys
    return keys   (same length as dispatch, same order)

FUNCTION binary_search(dispatch, keys, target_time, product_code):
    convert target_time (HH:MM:SS) to target_seconds

    lo = 0
    hi = length of dispatch

    -- Narrow down to the earliest position at or after target_time
    WHILE lo < hi:
        mid = (lo + hi) divided by 2, rounded down

        IF keys[mid] < target_seconds THEN
            lo = mid + 1        (target is in the right half, discard left)
        ELSE
            hi = mid            (mid could be the answer, keep it in range)

    -- lo is now the index of the first record at or after the target time
    IF lo is past the end of the list THEN
        return NOT FOUND

    IF no product_code was given THEN
        return dispatch[lo]     (earliest record at or after that time)

    -- Scan forward from lo to find the matching product
    FOR i from lo to end of dispatch:
        IF dispatch[i].product_code = product_code THEN
            return dispatch[i]

    return NOT FOUND
```

---

### Task 4 — Hash Map Count Report — O(N)

```
FUNCTION count_per_container(dispatch):
    container_map = empty dictionary

    FOR each record in dispatch:
        cid = record.container_id

        IF cid is not already in container_map THEN
            container_map[cid] = {
                count        : 0,
                weight_total : 0.0,
                temp_total   : 0,
                couriers     : empty set
            }

        container_map[cid].count        += 1
        container_map[cid].weight_total += record.weight
        container_map[cid].temp_total   += record.temperature
        add record.courier_id to container_map[cid].couriers

    -- Build the final report from the accumulated totals
    report = empty list

    FOR each container_id in container_map:
        n = container_map[container_id].count

        add to report:
            container_id  : container_id
            product_count : n
            avg_weight    : container_map[container_id].weight_total / n
            avg_temp      : container_map[container_id].temp_total   / n
            couriers_used : container_map[container_id].couriers

    return report
```
