
#open the file
$out_file = $ARGV[0];
$nb_width = $ARGV[1];
$nb_height = $ARGV[2];

#open output file
open(OUTFILE, ">$out_file") or die "Can't open output file \"$out_file\"";

#header
print OUTFILE "\n#obj file automatically generated using create_cloth\n\n";

#vertices
$z = 1.75;

$width = 4; 
$height = 4;

$dx = $width/$nb_width;
$dy = $height/$nb_height;

$x = -$width/2;
$y = -$height/2;
$nb_y = 0;

#vertices
while ($nb_y < $nb_height) {

	$nb_x = 0;
	$x = 0;
	
	while ($nb_x < $nb_width) {
	
		#write point
		print OUTFILE "v ".$x." ".$z." ".$y."\n";
	
		$x += $dx;		
		$nb_x++;
		
	}
	
	$y += $dy;
	$nb_y++;
	
}

#indices
$x = 0;
$c = 0;

#p1-----p2
#|       |
#|       |
#|       |
#p3-----p4
while ($x < $nb_width - 1) {

	$y = 0;

	while ($y < $nb_height - 1) {
	
		$p1 = ($y + 0)*$nb_width + ($x + 0) + 1;
		$p2 = ($y + 0)*$nb_width + ($x + 1) + 1;
		$p3 = ($y + 1)*$nb_width + ($x + 0) + 1;
		$p4 = ($y + 1)*$nb_width + ($x + 1) + 1;
		
		#write index
		if ($c % 2 eq 0) {
			$ind1 = $p1;
			$ind2 = $p4;
			$ind3 = $p3;
			$ind4 = $p1;
			$ind5 = $p2;
			$ind6 = $p4;
		} else {
			$ind1 = $p3;
			$ind2 = $p1;
			$ind3 = $p2;
			$ind4 = $p2;
			$ind5 = $p4;
			$ind6 = $p3;		
		}
		print OUTFILE "f ".$ind1." ".$ind2." ".$ind3."\n";
		print OUTFILE "f ".$ind4." ".$ind5." ".$ind6."\n";
	
		$y++;
		$c++;
		
	}
	
	$x++;
}

#done !
close(OUTFILE);
