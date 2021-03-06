/******************************************************************************
** Program : extrema_locator.h
** Author  : Neil Massey
** Date    : 10/07/13
** Purpose : class that searches for extrema in data regridded onto a 
**           regional hierarchical triangular mesh - provided by regrid
**           this class is abstract and should be inherited from.  See
**           minima_locator.h and maxima_locator.h for an example
******************************************************************************/

#ifndef EXTREMA_LOCATOR_H
#define EXTREMA_LOCATOR_H

#include <list>
#include "indexed_force_tri_3D.h"
#include "tri_grid.h"
#include "data_store.h"
#include "extrema_list.h"
#include "ncdata.h"
#include "steering_vector.h"
#include "meta_data.h"

/*****************************************************************************/

FP_TYPE calculate_triangle_distance(indexed_force_tri_3D* O_TRI, 
                                    indexed_force_tri_3D* C_TRI);

/*****************************************************************************/

class extrema_locator
{
    public:
        extrema_locator(void);
        virtual ~extrema_locator(void);
        void save(std::string output_fname, bool save_text=false);
        void set_steering_vector(steering_vector* sv);
        void set_inputs(std::string input_fname, std::string mesh_fname,
                        int extrema_level, ADJACENCY adj_type);
        void calculate_steering_vector(int o, int t);
        virtual void parse_arg_string(std::string method_string) = 0;
        virtual void locate(void);
        
    protected:  
        /*********************************************************************/
        // control variables
        int extrema_level;
        ADJACENCY adj_type;
        
        /*********************************************************************/
        // mesh and data storage
        tri_grid tg;
        std::string ds_fname;
        data_store ds;
        extrema_list ex_list;
        steering_vector* sv;        // steering vector class
        META_DATA_TYPE meta_data;   // add the meta data as we parse the methods and steering vector
        FP_TYPE max_merge_dist;
        
        /*********************************************************************/

        // Virtual functions that require overloading
        virtual bool is_extrema(indexed_force_tri_3D* tri, int t_step) = 0;
        virtual bool is_in_object(indexed_force_tri_3D* O_TRI, 
                                  indexed_force_tri_3D* C_TRI, int t) = 0;
        
        virtual void calculate_object_position(int o, int t) = 0;
        virtual void calculate_object_intensity(int o, int t) = 0;
        virtual void calculate_object_delta(int o, int t) = 0;

        // Virtual functions that may be overloaded
        virtual void find_extrema(void);
        virtual void refine_extrema(void);
        virtual void find_objects(void);
        virtual void split_objects(void);
        
        // Functions that are applicable to all 
        void merge_objects(void);
        void ex_points_from_objects(void);
        void get_min_max_values(FP_TYPE& min, FP_TYPE& max, int o, int t);      
        bool objects_share_nodes(const LABEL_STORE* o1,
                                 const LABEL_STORE* o2);
        void tstep_out_begin(int t);
        void tstep_out_end(int t);
        void get_leaf_node_labels(QT_TRI_NODE* c_tri_node, LABEL_STORE& label_list, int max_level);
};

#endif