/******************************************************************************
** Program : regrid.cpp
** Author  : Neil Massey
** Date    : 28/06/13
** Purpose : program to read in triangular mesh generated by gen_grid and 
**           apply to a variable in a netCDF file
******************************************************************************/

#include <tclap/CmdLine.h>
#include <string>
#include <fstream>
#include "regridder.h"

int main(int argc, char** argv)
{
	// command line arguments
	std::string mesh_fname;		// mesh from gen_grid filename
	std::string nc_fname;		// netCDF file name
	std::string nc_vname;		// netCDF variable name
	std::string out_fname;		// output filename
	int z_level;				// level to regrid in netCDF file
	bool text_out = false;
	FP_TYPE smooth_weight = 1.0;// weight of central triangle.
								// smoothing is done by assigning point adjacent triangles the weight (1.0-smooth_weight)/12
	int p = 0;					// what to do with parent triangles in regrid. 0=mean (default), 1=min, 2=max

	std::cout << "#### regrid" << std::endl;

	try
	{
		TCLAP::CmdLine cmd("Regrid data from a netCDF onto an equal area, hierarchical triangular grid mesh generated using gen_grid");
		TCLAP::ValueArg<int> z_level_arg("z", "z_level", "Z index of the surface in the netCDF variable to regrid", false, 0, "integer", cmd);
		TCLAP::ValueArg<std::string> mesh_fname_arg("m", "mesh_file", "Name of file containing mesh generated by gen_grid", true, "", "string", cmd);
		TCLAP::ValueArg<std::string> nc_fname_arg("i", "nc_file", "Name of netCDF file containing variable to regrid", true, "", "string", cmd);
		TCLAP::ValueArg<std::string> nc_vname_arg("v", "nc_var", "Name of variable in netCDF to regrid", true, "", "string", cmd);
		TCLAP::SwitchArg text_out_arg("T", "text", "Output grid in text format, as well as the binary format", cmd, false);
		TCLAP::ValueArg<FP_TYPE> smooth_weight_arg("S", "smooth", "Weight of central triangle in smoothing of grid. 1.0 = no smoothing", false, 1.0, "float", cmd);
		TCLAP::ValueArg<int> p_val_arg("p", "parent", "Method to use when calculating parent triangles. 0=mean (default), 1=min, 2=max", false, 0, "integer", cmd);
		TCLAP::ValueArg<std::string> out_fname_arg("o", "out_file", "Name of file to output results to", true, "", "string", cmd);
		cmd.parse(argc, argv);

		// get the values
		z_level = z_level_arg.getValue();
		mesh_fname = mesh_fname_arg.getValue();
		nc_fname = nc_fname_arg.getValue();
		nc_vname = nc_vname_arg.getValue();
		text_out = text_out_arg.getValue();
		smooth_weight = smooth_weight_arg.getValue();
		p = p_val_arg.getValue();
		out_fname = out_fname_arg.getValue();
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

	// create the regridder
	try
	{
		regridder rg(mesh_fname, nc_fname, nc_vname, z_level, p);
		rg.regrid();
		rg.save(out_fname);
		std::cout << "# Saved to file: " << out_fname << std::endl;
		if (text_out)
			rg.save_text(out_fname+".txt");
	}
	catch(std::string &s)
	{
		std::cerr << s << std::endl;
		return 1;
	}
	return 0;
}
