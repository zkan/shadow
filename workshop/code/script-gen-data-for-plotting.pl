#!/usr/bin/perl

my $input = $ARGV[0];
my $type = $ARGV[1];

if($input eq "") {
    print "Usage error: Please specify the input file\n";
    print "Usage: perl script-gen-data-for-plotting.pl <input file> <type 'fg' or 'sh'>\n";
    exit;
}
if(($type ne "fg") && ($type ne "sh")) {
    print "Usage error: Please specify the type\n";
    print "Usage: perl script-gen-data-for-plotting.pl <input file> <type: 'fg' or 'sh'>\n";
    exit;
}

my $h = "h = [";
my $s = "s = [";
my $v = "v = [";

open( FILE, $input ) or die( "Cannot open file" );
@data = <FILE>;
foreach $text_line ( @data ) {
	$text_line =~ s/\n/ /g;
	@output = split( "\t", $text_line ); 
#  print $text_line . "\n";
	$h .= $output[2] . "; ";
	$s .= $output[3] . "; ";
	$v .= $output[4] . "; ";
	
#	print $output[2] . "\n";
	
#  print "\n";
}

$h .= "];\n";
$s .= "];\n";
$v .= "];\n";

if($type eq "fg") {
    open(FILE, ">foreground_h_file_to_plot.m") or die("Cannot open file");
    print FILE $h;
    print FILE "x = min(h):1:max(h);\n";
    print FILE "hist(h, x)\n";
    print FILE "fg_mu_h = mean(h)\n";
    print FILE "fg_std_h = std(h)\n";
    close(FILE);

    open(FILE, ">foreground_s_file_to_plot.m") or die("Cannot open file");
    print FILE $s;
    print FILE "x = min(s):1:max(s);\n";
    print FILE "hist(s, x)\n";
    print FILE "fg_mu_s = mean(s)\n";
    print FILE "fg_std_s = std(s)\n";
    close(FILE);

    open(FILE, ">foreground_v_file_to_plot.m") or die("Cannot open file");
    print FILE $v;
    print FILE "x = min(v):1:max(v);\n";
    print FILE "hist(v, x)\n";
    print FILE "fg_mu_v = mean(v)\n";
    print FILE "fg_std_v = std(v)\n";
    close(FILE);
}
else {
    open(FILE, ">shadow_h_file_to_plot.m") or die("Cannot open file");
    print FILE $h;
    print FILE "x = min(h):1:max(h);\n";
    print FILE "hist(h, x)\n";
    print FILE "sh_mu_h = mean(h)\n";
    print FILE "sh_std_h = std(h)\n";
    close(FILE);

    open(FILE, ">shadow_s_file_to_plot.m") or die("Cannot open file");
    print FILE $s;
    print FILE "x = min(s):1:max(s);\n";
    print FILE "hist(s, x)\n";
    print FILE "sh_mu_s = mean(s)\n";
    print FILE "sh_std_s = std(s)\n";
    close(FILE);

    open(FILE, ">shadow_v_file_to_plot.m") or die("Cannot open file");
    print FILE $v;
    print FILE "x = min(v):1:max(v);\n";
    print FILE "hist(v, x)\n";
    print FILE "sh_mu_v = mean(v)\n";
    print FILE "sh_std_v = std(v)\n";
    close(FILE);
}




