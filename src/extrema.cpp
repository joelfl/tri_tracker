/******************************************************************************
** Program : extrema.cpp
** Author  : Neil Massey
** Date    : 09/07/13
** Purpose : program to read in triangular mesh generated by gen_grid and 
**           a regridded set of data generated by regrid, and locate minima
**           or maxima in the regridded field
******************************************************************************/

#include <tclap/CmdLine.h>
#include <string>
#include <sstream>
#include <fstream>
#include "minima_locator.h"
#include "maxima_locator.h"
#include "minima_background.h"
#include "steering_vector.h"
#include "geo_wind_vector.h"
#include "minima_back_wind.h"

/*****************************************************************************/

std::string get_method(std::string method_string)
{
	int brckt_start = method_string.find_first_of("(");
	std::string method = method_string.substr(0, brckt_start);
	return method;
}

/*****************************************************************************/

extrema_locator* create_extrema_locator(std::string method_string)
{
	extrema_locator* el;
	std::string method = get_method(method_string);
	// determine which class to use to do the extrema location
	if (method=="minima")
		el = new minima_locator();
	else if (method=="maxima")
		el = new maxima_locator();
	else if (method=="minima_back")		// minima with background removal
		el = new minima_background();
	else if (method=="minima_back_wind")
		el = new minima_back_wind();
	else
		throw (std::string("Unsupported extrema location method: " + method));
	return el;
}

/*****************************************************************************/

steering_vector* create_steering_vector(std::string steering_string)
{
	steering_vector* sv = NULL;
	// check whether a steering vector is to be used and set up the class if it is
	std::string steering = get_method(steering_string);
	// determine which (inherited) steering class to use
	if (steering == "geostrophic")
		sv = new geo_wind_vector();
	else if (steering != "")
		throw (std::string("Unknown steering vector method: " + steering));
	// note - this function can return NULL if the steering string is empty
	return sv;
}

/*****************************************************************************/

int main(int argc, char** argv)
{
	std::cout << "#### extrema" << std::endl;

	std::string input_fname;		// input file name from regrid
	std::string output_fname;		// output file name
	std::string mesh_fname;			// file name of grid definition file
	std::string method_string;		// method string format: = method_name(arg1, arg2,arg3)
	std::string steering_string;	// steering vector calculation method.  format as above
	int grid_level;					// level to detect extrema at
	ADJACENCY adjacency_type;		// adjacency type, 0 = POINT | 1 = EDGE
	bool text_out = false;

	try
	{
		TCLAP::CmdLine cmd("Locate extrema from data regridded onto a regional triangular mesh using regrid");
		TCLAP::ValueArg<int> g_level_arg("l", "g_level", "Level of grid to detect extremas at", false, -1, "integer", cmd);
		TCLAP::ValueArg<int> ex_adj_arg("a", "adjacency", "Adjacency type: 0 = point | 1 = edge", false, 0, "integer", cmd);
		TCLAP::ValueArg<std::string> method_arg("e", "method", "Extrema detection method", true, "", "string", cmd);
		TCLAP::ValueArg<std::string> steering_arg("s", "steering", "Steering vector calculation method", false, "", "string", cmd);
		TCLAP::SwitchArg text_out_arg("T", "text", "Output grid in text format, as well as the binary format", cmd, false);		
		TCLAP::ValueArg<std::string> out_fname_arg("o", "output", "Output file name", true, "", "string", cmd);
		TCLAP::ValueArg<std::string> mesh_fname_arg("m", "mesh_file", "Name of file containing mesh generated by gen_grid", true, "", "string", cmd);
		TCLAP::ValueArg<std::string> in_fname_arg("i", "input", "Input file of regridded data as generated by regrid", true, "", "string", cmd);
		cmd.parse(argc, argv);

		grid_level = g_level_arg.getValue();
		output_fname = out_fname_arg.getValue();
		input_fname = in_fname_arg.getValue();
		mesh_fname = mesh_fname_arg.getValue();
		adjacency_type = (ADJACENCY)(ex_adj_arg.getValue());
		method_string = method_arg.getValue();
		steering_string = steering_arg.getValue();
		text_out = text_out_arg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch exceptions
	{
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}
	catch (char const* m)           // general error message exception
	{
		std::cerr << "Error: " << m << std::endl;
		return 1;
	}
	catch (std::string &s)          // string error message
	{
		std::cerr << "Error: " << s << std::endl;
		return 1;
	}
	// create the extrema locator
	try
	{
		// create an extrema locator
		extrema_locator* el = create_extrema_locator(method_string);
		el->parse_arg_string(method_string);
		el->set_inputs(input_fname, mesh_fname, grid_level, adjacency_type);		
		// create a steering vector if necessary
		steering_vector* sv = create_steering_vector(steering_string);
		if (sv != NULL)
		{
			sv->parse_arg_string(steering_string);
			el->set_steering_vector(sv);
		}
		// do the location - pass in the arguments
		el->locate();
		// save to the output file
		el->save(output_fname, text_out);
		std::cout << "# Saved to file: " << output_fname << std::endl;
		delete el;
	}
	catch(std::string &s)
	{
		std::cerr << s << std::endl;
	}
}