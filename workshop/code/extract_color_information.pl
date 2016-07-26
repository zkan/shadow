#!/usr/bin/perl

# Reset the workshop so that we all are at the same initial state.
system("perl reset.pl");

# Run the scripts to extract the color information of the object pixels and 
# shadow pixels.
system("perl script-write-hallway-fg-data-to-file.pl");
system("perl script-write-hallway-sh-data-to-file.pl");

# Generate the Matlab/Octave code for plotting the distributions and computing 
# the means and standard deviations.
system("perl script-gen-data-for-plotting.pl ../data/working/fg_data_file fg");
system("perl script-gen-data-for-plotting.pl ../data/working/sh_data_file sh");


