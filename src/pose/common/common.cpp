#include "../common.h"

void FreeKeypointVector(KeypointVector *p) {
    if (p->points != NULL) {
        free(p->points);
        p->points = NULL;
    }
}

void FreeROIVector(ROIVector *p) {
    if (p->items!= NULL) {
        free(p->items);
        p->items= NULL;
    }
}
