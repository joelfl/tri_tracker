/******************************************************************************
** Program : maxima_locator.h
** Author  : Neil Massey
** Date    : 10/07/13
** Purpose : class inherited from extrema_locator that searches for maxima 
**           in data regridded onto a regional hierarchical triangular mesh
******************************************************************************/

#ifndef MAXIMA_LOCATOR_H
#define MAXIMA_LOCATOR_H

#include "extrema_locator.h"

/*****************************************************************************/

class maxima_locator : public extrema_locator
{
	public:
		maxima_locator(void);
		~maxima_locator(void);	
		// Virtual functions that require overloading
		virtual void parse_arg_string(std::string method);
		virtual bool is_extrema(indexed_force_tri_3D* tri, int t_step);
		virtual bool is_in_object(indexed_force_tri_3D* O_TRI, 
								  indexed_force_tri_3D* C_TRI, int t_step);
		virtual bool process_data(void);
		virtual FP_TYPE calculate_point_weight(FP_TYPE V, FP_TYPE min_v, FP_TYPE max_v);		
		virtual indexed_force_tri_3D* get_original_triangle(int o, int t);
};

#endif