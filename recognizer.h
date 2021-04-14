#ifndef __RECOGNIZER_H__
#define __RECOGNIZER_H__

#define RECOGNIZER_ROTATION_INVARIANT   0

#define RECOGNIZER_RESAMPLE_POINTS     64
#define RECOGNIZER_SCALE_SIZE         128
#define RECOGNIZER_ANGLE_PRECISION      2.0
#define RECOGNIZER_ANGLE_RANGE         45.0

#define RECOGNIZER_HALF_DIAGONAL \
	(0.5 * sqrt(2 * (RECOGNIZER_SCALE_SIZE * RECOGNIZER_SCALE_SIZE)))

#define RECOGNIZER_GOLDEN_RATIO \
	(0.5 * (-1.0 + sqrt(5.0)))

typedef struct
{
	double X, Y;
}
Point;

void recognizer_normalize(Point *in, Point *out, int n);
int recognizer_compare(Point *a, Point *b);

#endif /* __RECOGNIZER_H__ */
