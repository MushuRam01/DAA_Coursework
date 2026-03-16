# Algorithm: Area of Union of Axis-Aligned Rectangles (Sweep Line Method)

## Input
A list of rectangles.

Each rectangle r is represented as

(r.min_x, r.max_x, r.min_y, r.max_y)

## Output
Total area covered by at least one rectangle.

---

## Idea

Move a vertical sweep line from left to right across the plane.  
Between two consecutive x-coordinates where rectangles start or end, the set of active rectangles does not change.  
For that vertical strip we compute how much **y-length is covered**, then multiply by the strip width.

---

## Data Representation

Create events for rectangle edges:

For each rectangle r:

add event (r.min_x, START, r.min_y, r.max_y)  
add event (r.max_x, END, r.min_y, r.max_y)

---

## Procedure

1. Build event list

    events ← empty list

    for each rectangle r
        append (r.min_x, START, r.min_y, r.max_y)
        append (r.max_x, END, r.min_y, r.max_y)

2. Sort events by x coordinate (ascending)

3. Initialize

    active_intervals ← empty list
    previous_x ← events[0].x
    total_area ← 0

4. Process events in order

    for each event e in events

        current_x ← e.x
        width ← current_x - previous_x

        covered_y ← compute_union_length(active_intervals)

        total_area ← total_area + width * covered_y

        if e.type = START
            add interval (e.min_y, e.max_y) to active_intervals
        else
            remove interval (e.min_y, e.max_y) from active_intervals

        previous_x ← current_x

5. Return

    return total_area

---

## Subroutine: compute_union_length(intervals)

Purpose: compute total y-length covered by overlapping intervals.

Procedure:

    sort intervals by start

    total ← 0
    current_start ← intervals[0].start
    current_end ← intervals[0].end

    for each interval (s, e) in intervals starting from second

        if s ≤ current_end
            current_end ← max(current_end, e)
        else
            total ← total + (current_end - current_start)
            current_start ← s
            current_end ← e

    total ← total + (current_end - current_start)

    return total

---

## Complexity

### Time Complexity

Event creation: O(n)  
Each rectangle contributes exactly two events (start and end), so building the event list requires linear time.

Sorting events: O(n log n)  
There are 2n events and sorting them by x-coordinate dominates the running time.

Processing sweep events: O(n log n)  
For each event we update active intervals and recompute the union of y-intervals, which involves sorting or merging intervals whose total operations scale with the number of rectangles.

Overall time complexity: O(n log n)  
The sorting step dominates the algorithm, while all other operations are at most logarithmic or linear per event.

---

### Space Complexity

Event storage: O(n)  
We store two events per rectangle, so the event list size grows linearly with the number of rectangles.

Active interval storage: O(n)  
In the worst case all rectangles may overlap at the same x-position, requiring all y-intervals to be stored simultaneously.

Overall space complexity: O(n)  
Both the event list and the active interval set grow proportionally with the number of rectangles.