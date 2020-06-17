
To compile the source code, use the bash script debug/build.sh (you must be
in the debug folder for the paths to work)

To compile it for yourself, use
    g++ main.cpp [-o executable_name] -std=c++11

The program takes the following arguments:

    ./main.exe netlist [-output output_file_name] [-iterations iteration_count]
            [-silent] [-profile]

        netlist: the name of the SPICE netlist to simulate
        output_file_name: specify the name of an output file to write the
            simulation results to
        iteration_count: the number of simulation iterations to run (used for
            profiling)
        silent: use this flag if you don't want the simulation results to appear
        profile: prints the total time (and the time per iteration) at the end
            of the simulation

By default the simulation results will be piped to std::cout

There are test scripts included in the tests/ folder
