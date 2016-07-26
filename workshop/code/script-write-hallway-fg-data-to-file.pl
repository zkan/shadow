#!/usr/bin/perl

my $output_name = "../data/working/data_file";
my $separate_output_name = "../data/working/fg_data_file_";
my $new_output_name = "../data/working/fg_data_file";

my $num_fg = 24;

# Extract data for each foregroud blob.
for($i = 1; $i <= $num_fg; $i++) {
    my $num = "";
    if($i < 10) {
        $num = "00";
    }
    elsif($i < 100) {
        $num = "0";
    }
    else {
        $num = "";
    }
    $num .= $i;
#    print $num . "\n";

    system("./get_data_under_mask ../data/input/bgs/hallway-$num.png ../data/input/frames/hallway-$num.png ../data/input/masks/hallway-$num-object.png");

    system("mv $output_name $separate_output_name" . $num);
}

# Concatinate all files into one file.
for($i = 1; $i <= $num_fg; $i++) {
    my $num = "";
    if($i < 10) {
        $num = "00";
    }
    elsif($i < 100) {
        $num = "0";
    }
    else {
        $num = "";
    }
    $num .= $i;
    
    system("cat $separate_output_name" . "$num >> $new_output_name");
}

