if ($#ARGV != 1) {
    print "usage: perl prepare_deb <version> <arch>\n";
    exit;
}

%files = (
             "deb_templates/changelog" => "deb/DEBIAN/changelog"
            ,"deb_templates/control"   => "deb/DEBIAN/control"
            ,"deb_templates/copyright" => "deb/DEBIAN/copyright"
 );

$size = (split(/\t/,`du -hs deb`))[0]; 

%vars = (
             "\\\$\\{timestamp\\}" => `date -u`
            ,"\\\$\\{version\\}" => $ARGV[0]
            ,"\\\$\\{size\\}" => $size
            ,"\\\$\\{arch\\}" => $ARGV[1] 
        );

print "replacing following values:\n";
while ( ($key, $value) = each %vars )
{
    print $key."=>".$value."\n";
}

while ( ($inFile, $outFile) = each %files )
{
    open(OF, $inFile);
    open(NF, ">$outFile");

    # read in each line of the file
    while ($line = <OF>) {
        while ( ($key, $value) = each %vars )
        {
            $line =~ s/$key/$value/g;
        }
        print NF $line;
    };

    close(OF);
    close(NF);
}
