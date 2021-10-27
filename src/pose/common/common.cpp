#include "../common.h"

void FreePoseKeypointVector(PoseKeypointVector *p) {
    if (p->points != NULL) {
        free(p->points);
        p->points = NULL;
    }
}

void FreePoseROI(PoseROI *p) {
    if (p->data!= NULL) {
        free(p->data);
        p->data= NULL;
    }
}

void FreePoseROIVector(PoseROIVector *p) {
    if (p->items!= NULL) {
        for (int i=0; i < p->length; i ++) {
            FreePoseROI(&p->items[i]);
        }
        free(p->items);
        p->items= NULL;
    }
}
