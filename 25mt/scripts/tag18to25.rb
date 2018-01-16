#!/usr/local/bin/ruby

# ruby script to convert older tag files to v2x format

File.open("skj10.tag", "r") do |tagfile|
	tagfile.each_line { |line| 
		if line =~ /^#/ or line =~ /^$/ then
			puts line
		else
			array = line.split 

			fleet = array[0]
			year = array[1] 
			month = array[2]
			release_cohort = array[3]
			recapture_long = array[4].to_i + 180 #make it go eastward [0,360) instead of [-180, 180]
			recapture_lat = array[5]
			tags = array[6]

			$stdout << fleet << "\t" << year << "\t" << month << "\t" << release_cohort << "\t" << recapture_long << "\t" << recapture_lat << "\t" << tags << "\n"
		end
	}
end
