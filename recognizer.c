#include "recognizer.h"
#include "recognizer.h"
#include <math.h>
#include <stdlib.h>
#include <limits.h>

static double _distance(Point *p0, Point *p1);
static void _centroid(Point *pts, Point *c);
static double _distance_at_angle(Point *pts0, Point *pts1, double rotation);

static double _distance(Point *p0, Point *p1)
{
	double dx, dy;
	dx = p1->X - p0->X;
	dy = p1->Y - p0->Y;
	return sqrt((dx * dx) + (dy * dy));
}

static void _centroid(Point *pts, Point *c)
{
	int i;
	c->X = 0.0;
	c->Y = 0.0;
	for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
	{
		c->X += pts[i].X;
		c->Y += pts[i].Y;
	}

	c->X /= RECOGNIZER_RESAMPLE_POINTS;
	c->Y /= RECOGNIZER_RESAMPLE_POINTS;
}

static double _distance_at_angle(Point *pts0, Point *pts1, double rotation)
{
	int i;
	double distance, sin_v, cos_v;
	Point p, c;
	distance = 0.0;
	sin_v = sin(rotation);
	cos_v = cos(rotation);
	_centroid(pts0, &c);
	for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
	{
		p.X = (pts0[i].X - c.X) * cos_v -
			(pts0[i].Y - c.Y) * sin_v + c.X;

		p.Y = (pts0[i].X - c.X) * sin_v +
			(pts0[i].Y - c.Y) * cos_v + c.Y;

		distance += _distance(&p, pts1 + i);
	}

	return distance / RECOGNIZER_RESAMPLE_POINTS;
}

void recognizer_normalize(Point *in, Point *out, int n)
{
	int i;
	Point c;

	{
		int idx = 0;
		Point p;
		double interval, d0, d1;

		/* Resample */
		d0 = 0.0;
		for(i = 1; i < n; ++i)
		{
			d0 += _distance(&in[i - 1], &in[i]);
		}

		interval = d0 / (RECOGNIZER_RESAMPLE_POINTS - 1);
		d0 = d1 = 0.0;
		out[idx++] = in[0];
		for(i = 1; i < n; i++)
		{
			d0 = _distance(&in[i - 1], &in[i]);
			if((d1 + d0) >= interval)
			{
				int j;

				p.X = in[i - 1].X +
					((interval - d1) / d0) *
					(in[i].X - in[i - 1].X);

				p.Y = in[i - 1].Y +
					((interval - d1) / d0) *
					(in[i].Y - in[i - 1].Y);

				out[idx++] = p;

				for(j = n; j > i; --j)
				{
					in[j] = in[j - 1];
				}

				in[i] = p;
				++n;
				d1 = 0.0;
			}
			else
			{
				d1 += d0;
			}
		}

		if(idx == RECOGNIZER_RESAMPLE_POINTS - 1)
		{
			out[idx++] = in[n - 1];
		}
	}

#if defined(RECOGNIZER_ROTATION_INVARIANT) && RECOGNIZER_ROTATION_INVARIANT
	{
		double rotation, sin_v, cos_v;

		/* Indicative Angle */
		_centroid(out, &c);
		rotation = -atan2(
			c.Y - out[0].Y,
			c.X - out[0].X);

		sin_v = sin(rotation);
		cos_v = cos(rotation);

		/* Rotate */
		for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
		{
			out[i].X = (out[i].X - c.X) * cos_v -
				(out[i].Y - c.Y) * sin_v + c.X;

			out[i].Y = (out[i].X - c.X) * sin_v +
				(out[i].Y - c.Y) * cos_v + c.Y;
		}
	}
#endif

	{
		double min_x, max_x, min_y, max_y;

		/* Bounding Box */
		min_x = min_y = INFINITY;
		max_x = max_y = -INFINITY;
		for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
		{
			if(out[i].X < min_x)
			{
				min_x = out[i].X;
			}

			if(out[i].X > max_x)
			{
				max_x = out[i].X;
			}

			if(out[i].Y < min_y)
			{
				min_y = out[i].Y;
			}

			if(out[i].Y > max_y)
			{
				max_y = out[i].Y;
			}
		}

		/* Scale */
		max_x -= min_x;
		max_y -= min_y;
		for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
		{
			out[i].X *= (RECOGNIZER_SCALE_SIZE / max_x);
			out[i].Y *= (RECOGNIZER_SCALE_SIZE / max_y);
		}
	}

	{
		/* Translate */
		_centroid(out, &c);
		for(i = 0; i < RECOGNIZER_RESAMPLE_POINTS; ++i)
		{
			out[i].X -= c.X;
			out[i].Y -= c.Y;
		}
	}
}

int recognizer_compare(Point *a, Point *b)
{
	double rs, re, x0, x1, f0, f1;

	rs = -RECOGNIZER_ANGLE_RANGE;
	re = RECOGNIZER_ANGLE_RANGE;

	x0 = RECOGNIZER_GOLDEN_RATIO * rs +
		(1.0 - RECOGNIZER_GOLDEN_RATIO) * re;

	f0 = _distance_at_angle(a, b, x0);

	x1 = RECOGNIZER_GOLDEN_RATIO * re +
		(1.0 - RECOGNIZER_GOLDEN_RATIO) * rs;

	f1 = _distance_at_angle(a, b, x1);
	while(fabs(re - rs) > RECOGNIZER_ANGLE_PRECISION)
	{
		if(f0 < f1)
		{
			re = x1;
			x1 = x0;
			f1 = f0;
			x0 = RECOGNIZER_GOLDEN_RATIO * rs +
				(1.0 - RECOGNIZER_GOLDEN_RATIO) * re;

			f0 = _distance_at_angle(a, b, x0);
		}
		else
		{
			rs = x0;
			x0 = x1;
			f0 = f1;
			x1 = RECOGNIZER_GOLDEN_RATIO * re +
				(1.0 - RECOGNIZER_GOLDEN_RATIO) * rs;

			f1 = _distance_at_angle(a, b, x1);
		}
	}

	return (1.0 - ((f0 < f1 ? f0 : f1) / RECOGNIZER_HALF_DIAGONAL)) * 100;
}
