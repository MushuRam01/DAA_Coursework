#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int min_x;
    int max_x;
    int min_y;
    int max_y;
} Rectangle;

typedef struct {
    int y1;
    int y2;
} Interval;

typedef struct {
    int x;
    int type; /* 1 = START, 0 = END */
    int y1;
    int y2;
} Event;

int compare_events(const void *a, const void *b) {
    const Event *ea = (const Event *)a;
    const Event *eb = (const Event *)b;

    if (ea->x < eb->x) {
        return -1;
    } else if (ea->x > eb->x) {
        return 1;
    }

    /* For the same x, process START before END to keep active set consistent */
    if (ea->type > eb->type) {
        return -1;
    } else if (ea->type < eb->type) {
        return 1;
    }

    return 0;
}

int compare_intervals(const void *a, const void *b) {
    const Interval *ia = (const Interval *)a;
    const Interval *ib = (const Interval *)b;

    if (ia->y1 < ib->y1) {
        return -1;
    } else if (ia->y1 > ib->y1) {
        return 1;
    }

    return 0;
}

int read_rectangles(const char *filename, Rectangle **rectangles_out, int *count_out) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 1;
    }

    int n;
    if (fscanf(fp, "%d", &n) != 1) {
        fclose(fp);
        return 1;
    }

    Rectangle *rectangles = (Rectangle *)malloc((size_t)n * sizeof(Rectangle));
    if (rectangles == NULL) {
        fclose(fp);
        return 1;
    }

    int i = 0;
    while (i < n) {
        int x1, y1, x2, y2;
        if (fscanf(fp, "%d %d %d %d", &x1, &y1, &x2, &y2) != 4) {
            fclose(fp);
            free(rectangles);
            return 1;
        }

        if (x1 <= x2) {
            rectangles[i].min_x = x1;
            rectangles[i].max_x = x2;
        } else {
            rectangles[i].min_x = x2;
            rectangles[i].max_x = x1;
        }

        if (y1 <= y2) {
            rectangles[i].min_y = y1;
            rectangles[i].max_y = y2;
        } else {
            rectangles[i].min_y = y2;
            rectangles[i].max_y = y1;
        }

        i = i + 1;
    }

    fclose(fp);

    *rectangles_out = rectangles;
    *count_out = n;

    return 0;
}

int compute_union_length(Interval *intervals, int count) {
    if (intervals == NULL || count <= 0) {
        return 0;
    }

    qsort(intervals, (size_t)count, sizeof(Interval), compare_intervals);

    int total = 0;
    int current_start = intervals[0].y1;
    int current_end = intervals[0].y2;

    int i = 1;
    while (i < count) {
        int s = intervals[i].y1;
        int e = intervals[i].y2;

        if (s <= current_end) {
            if (e > current_end) {
                current_end = e;
            }
        } else {
            total = total + (current_end - current_start);
            current_start = s;
            current_end = e;
        }

        i = i + 1;
    }

    total = total + (current_end - current_start);

    return total;
}

long long compute_union_area(Rectangle *rectangles, int n) {
    if (rectangles == NULL || n <= 0) {
        return 0;
    }

    int event_count = n * 2;
    Event *events = (Event *)malloc((size_t)event_count * sizeof(Event));
    if (events == NULL) {
        return 0;
    }

    int i = 0;
    while (i < n) {
        events[2 * i].x = rectangles[i].min_x;
        events[2 * i].type = 1; /* START */
        events[2 * i].y1 = rectangles[i].min_y;
        events[2 * i].y2 = rectangles[i].max_y;

        events[2 * i + 1].x = rectangles[i].max_x;
        events[2 * i + 1].type = 0; /* END */
        events[2 * i + 1].y1 = rectangles[i].min_y;
        events[2 * i + 1].y2 = rectangles[i].max_y;

        i = i + 1;
    }

    qsort(events, (size_t)event_count, sizeof(Event), compare_events);

    Interval *active = (Interval *)malloc((size_t)n * sizeof(Interval));
    if (active == NULL) {
        free(events);
        return 0;
    }

    int active_count = 0;
    int previous_x = events[0].x;
    long long total_area = 0;

    i = 0;
    while (i < event_count) {
        int current_x = events[i].x;
        int width = current_x - previous_x;

        if (width > 0 && active_count > 0) {
            int covered_y = compute_union_length(active, active_count);
            total_area = total_area + (long long)width * (long long)covered_y;
        }

        /* Process this event: update active intervals */
        if (events[i].type == 1) {
            /* START: add interval */
            if (active_count < n) {
                active[active_count].y1 = events[i].y1;
                active[active_count].y2 = events[i].y2;
                active_count = active_count + 1;
            }
        } else {
            /* END: remove interval */
            int j = 0;
            while (j < active_count) {
                if (active[j].y1 == events[i].y1 && active[j].y2 == events[i].y2) {
                    int k = j + 1;
                    while (k < active_count) {
                        active[k - 1] = active[k];
                        k = k + 1;
                    }
                    active_count = active_count - 1;
                    break;
                }
                j = j + 1;
            }
        }

        previous_x = current_x;
        i = i + 1;
    }

    free(active);
    free(events);

    return total_area;
}

int main(void) {
    Rectangle *rectangles = NULL;
    int n = 0;

    if (read_rectangles("input.txt", &rectangles, &n) != 0) {
        return 1;
    }

    long long area = compute_union_area(rectangles, n);
    printf("%lld\n", area);

    free(rectangles);
    return 0;
}