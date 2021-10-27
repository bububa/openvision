#include "../common.h"

void FreePoseKeypointVector(PoseKeypointVector *p) {
    if (p->points != NULL) {
        free(p->points);
        p->points = NULL;
    }
}

void FreePoseROIVector(PoseROIVector *p) {
    if (p->items!= NULL) {
        free(p->items);
        p->items= NULL;
    }
}
