#ifndef FEATURES_H
#define FEATURES_H

#ifdef BWT_NO_MTF
	#define FEATURE_USE_MTF 0U
#else
	#define FEATURE_USE_MTF 1U
#endif

#endif
