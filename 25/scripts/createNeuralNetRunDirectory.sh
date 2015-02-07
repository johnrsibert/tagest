#!/bin/bash

if [ ! -f file_nam.dat ]; then
	printf "Error: file_nam.dat not found in \"`pwd`\".\n"
	exit 0
fi

#declare -r DATESFILE=`head -n 3 file_nam.dat | tail -n 1`.dat
#if [ ! -f $DATESFILE ]; then
#	printf "Error: $DATEFILE not found in \"`pwd`\".\n"
#	exit 0
#fi

declare -r ROOTNAME=`head -n 1 file_nam.dat`
if [ ! -f $ROOTNAME.prn ]; then
	printf "Error: $ROOTNAME.prn not found in \"`pwd`\".\n"
	exit 0
fi

if [ ! -f $ROOTNAME.pos ]; then
	touch $ROOTNAME.pos
fi

declare -r TAGFILE=`head -n 2 file_nam.dat | tail -n 1`
if [ ! -f ../$TAGFILE.dat ]; then
	printf "Error: $TAGFILE.dat not found in \"`pwd`\".\n"
	exit 0
fi

declare -r FLEETFILE=`tail -n 1 file_nam.dat`
if [ ! -f $FLEETFILE.dat ]; then
	printf "Error: $FLEETFILE.dat not found in \"`pwd`\".\n"
	exit 0
fi

declare -r FLEETS=`cat $FLEETFILE.dat`
for FLEET in $FLEETS; do
	fleet=`printf ../$FLEET.dat | tr [:upper:] [:lower:]`
	if [ ! -f $fleet ]; then
		printf "Error: ../$FLEET.dat not found in \"`pwd`\".\n"
		exit 0
	fi
done

if [ ! -f $ROOTNAME.ifr ]; then
	printf "Creating effort data...";$HOME/25/makeifr &> /dev/null
	if [ -f $ROOTNAME.ifr ]; then
		printf "OK\n"
	else
		printf "FAILED\n"
		printf "Error: unable to create $ROOTNAME.ifr in \"`pwd`\".\n"
		exit 0
	fi
fi

if [ ! -f $ROOTNAME.soi ]; then
	printf "Copying soi data "; cp -v ../../habdata/soi.txt $ROOTNAME.soi
fi

if [ ! -f $ROOTNAME.dst ]; then
	printf "Creating distance data...";$HOME/25/distance &> /dev/null
	if [ -f $ROOTNAME.dst ]; then
		printf "OK\n"
		rm -f distance.log
	else
		printf "FAILED\n"
		printf "Error: unable to create $ROOTNAME.dst in \"`pwd`\".\n"
		exit 0
	fi
fi

if [ ! -f $ROOTNAME.p00 -o ! -f $ROOTNAME.tag ]; then
	printf "Creating effort data...";$HOME/25/makepar &> /dev/null
	if [ -f $ROOTNAME.p00 -a -f $ROOTNAME.tag ]; then
		printf "OK\n"
	else
		printf "FAILED\n"
		if [ ! -f $ROOTNAME.p00 ]; then
			printf "Error: unable to create $ROOTNAME.p00 in \"`pwd`\".\n"
			exit 0
		fi
		if [ ! -f $ROOTNAME.tag ]; then
			printf "Error: unable to create $ROOTNAME.tag in \"`pwd`\".\n"
			exit 0
		fi
	fi
fi

if [ ! -f $ROOTNAME.zdt -o ! -f $ROOTNAME.zdf ]; then
	printf "Creating elevation data...";$HOME/25/elevation &> /dev/null
	if [ -f $ROOTNAME.zdt -a -f $ROOTNAME.zdf ]; then
		printf "OK\n"
		rm -f elevation.log
	else
		printf "FAILED\n"
		if [ ! -f $ROOTNAME.zdt ]; then
			printf "Error: unable to create $ROOTNAME.zdt in \"`pwd`\".\n"
			exit 0
		fi
		if [ ! -f $ROOTNAME.zdf ]; then
			printf "Error: unable to create $ROOTNAME.zdf in \"`pwd`\".\n"
			exit 0
		fi
	fi
fi

#if [ ! -f $ROOTNAME.frg -o ! -f $ROOTNAME.hab -o ! $ROOTNAME.sst -o ! $ROOTNAME.usc -o ! $ROOTNAME.vsc ]; then
#	cp ../../habdata/habitat.inf .
#	cp ../../habdata/grid10.inf skj10.inf
#	cp ../../habdata/f_0176-1290.txt .
#	cp ../../habdata/Hskj_0176-1290.txt .
#	cp ../../habdata/t_0176-1290.txt .
#	cp ../../habdata/u_0176-1290.txt .
#	cp ../../habdata/v_0176-1290.txt .
#	printf "Creating habitat data...";$HOME/25/nn_makedata &> /dev/null
#	if [ -f $ROOTNAME.frg -a -f $ROOTNAME.hab -a $ROOTNAME.sst -a $ROOTNAME.usc -a $ROOTNAME.vsc ]; then
#		printf "OK\n"
#		rm -f habitat.inf
#		rm -f skj10.inf
#		rm -f f_0176-1290.txt
#		rm -f Hskj_0176-1290.txt
#		rm -f t_0176-1290.txt
#		rm -f u_0176-1290.txt
#		rm -f v_0176-1290.txt
#	else
#		printf "FAILED\n"
#		if [ ! -f $ROOTNAME.frg ]; then
#			printf "Error: unable to create $ROOTNAME.frg in \"`pwd`\".\n"
#			exit 0
#		fi
#		if [ ! -f $ROOTNAME.hab ]; then
#			printf "Error: unable to create $ROOTNAME.hab in \"`pwd`\".\n"
#			exit 0
#		fi
#		if [ ! -f $ROOTNAME.sst ]; then
#			printf "Error: unable to create $ROOTNAME.sst in \"`pwd`\".\n"
#			exit 0
#		fi
#		if [ ! -f $ROOTNAME.usc ]; then
#			printf "Error: unable to create $ROOTNAME.usc in \"`pwd`\".\n"
#			exit 0
#		fi
#		if [ ! -f $ROOTNAME.vsc ]; then
#			printf "Error: unable to create $ROOTNAME.vsc in \"`pwd`\".\n"
#			exit 0
#		fi
#	fi
#fi

for ID in `seq 00 99` ; do
	NID=n$ID
	if [ "$ID" -lt "10" ]; then
		NID=n0$ID
	fi
	if [ ! -d "$NID" ]; then
		printf "Created new neural net run directory \"$NID\"\n"; mkdir $NID
		echo "$ROOTNAME" > $NID/file_nam.tmp 
		break
	fi
done

declare -i TOTAL_LINES=`wc -l $ROOTNAME.p00 | awk '{ print $1 }'`
for LINE in `seq 1 $TOTAL_LINES` ; do
	TAIL=`head -n $LINE $ROOTNAME.p00 | tail -n 1`
	if [ "$TAIL" = "#  REGIONAL PARAMETERS " ]; then
		echo "#v25n" > $NID/$ROOTNAME.par
		let LINE1=$LINE-1
		let LINE2=$LINE-2
		head -n $LINE1 $ROOTNAME.p00 | tail -n $LINE2 >> $NID/$ROOTNAME.par
	fi
done

cat >> $NID/$ROOTNAME.par << EOF 
#
#Neural Networks Scaling Function Parameters
#u_slope                     u_offset
       0.0038427035608945      10.8326789505563958
#v_slope                     v_offset
       0.0869625524025018       0.5832084134280822
#sig_slope                   sig_offset
       0.2000000000000000       0.0000000000000000
#uniform_sigma
     145
#
#Neural Networks Parameter_Ranges
#min_weight      max_weight
   -10.000    10.000
#min_bias         max_bias
   -10.000    10.000
#min_slope        max_slope
     0.000     5.000
#min_offset     max_offset
   -12.000    12.000
#
#hidden_layer's diff_hwts ranges
#min_diff_hwts      max_diff_hwts
     -10.000    10.000
#
#Linear scaling function x-ranges
#xmin_uv         xmax_uv
#     0.000     0.000
#xmin_sig         xmax_sig
#  -385.000   385.000
#
#Number of Neural Networks (num_nn)
   1
# Levels in each nn (num_levels)
   3
#
#  ==>  Config. for neural_network 1 <== 
# nodes_at_layer NN1
    7 5 2
#
#    30 control flags
#
#    flags 1 - 10
#
#    INIT. WEIGHTS: 0=> from par file, 1=> with  0.01; other =>  random seed
#    |     INIT. BIAS: 0=> from par file, 1=> with  0.01; other =>  random seed
#    |     |     Squash func: 0 => none; 1 => sigmoid; 2=> linear; 3 => logistic
#    |     |     |     Unused
#    |     |     |     |     Weights: 1 => active , 0 => inactive
#    |     |     |     |     |     Bias_nodes: 1 => active , 0 => inactive
#    |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     |     |     |     
     0     0     1     0     1     1     0     0     0     0
#
#    flags 11 - 20
#
#    Year_flag:   1= active; 0  = inactive
#    |     Month_flag
#    |     |     Longitude_flag
#    |     |     |     Latitude_flag
#    |     |     |     |     Depth_flag
#    |     |     |     |     |     Numfads_flag
#    |     |     |     |     |     |     Elevation_flag
#    |     |     |     |     |     |     |     Distance_flag
#    |     |     |     |     |     |     |     |     Seasonal WStressCurl
#    |     |     |     |     |     |     |     |     |     Spatio-Temporal
#    |     |     |     |     |     |     |     |     |     WStressCurl
#    |     |     |     |     |     |     |     |     |     |     
     0     1     1     1     1     0     1     0     0     0
#
#    flags 21 - 30
#
#    SOI Index: 1=> active, 0 = inactive
#    |     Tuna forage(x0.4gWW/m^2)
#    |     |     SST field
#    |     |     |     zonal current speed [u]
#    |     |     |     |     meridional current speed [v]
#    |     |     |     |     |     Skipjack habitat field
#    |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     |     |     Unused
#    |     |     |     |     |     |     |     |     |     |     
     1     0     1     0     0     0     0     0     0     0
#
# Weights NN1
# layer 1
       -1.0813810716829979        3.9646329683219377        5.7318979159509560       -1.4781316414785866        1.8004885910436643       -0.8758572669799568        0.0069237501231336
       -7.7371386482441054       -5.6768303939184666       -3.1051425899117358        0.9894779058473790        4.0745451023725421        8.6519634309024518        5.8017872557984207
        5.2231672523546360       -1.7434991285424348        2.0703648879305456       -0.5979327702858477       -1.5678003533838141       -2.9157660564497552        0.0235349389025974
        5.9999999944576263        6.4580827369934397        2.2044272699138925       -3.8042900272373457       -7.2213005719963581       -5.4915867994388243       -7.6794633263789578
       -3.6580106565606889        1.8529516761849136       -6.8356816629952375        2.3677945147245438        1.0183220760879566       -4.1303381319198857       -2.3919741121084543
# layer 2
        4.1885470724888005        4.1643188921929593        4.1643188989849946        4.1643189070463578        5.5532418972749724
        1.3977939010424951        2.1927229392061669        2.2321530292322480        2.2321530332363642        2.2321530344088747
#
# Bias NN1
# bias 
       -1.0027194048307608       -0.9502317943430068       -6.4377874481826325        1.8059723926503581        7.2404397808754393
        5.9999871126351643        0.8930609445004200
EOF

printf "Created neural net parameter file \"$NID/$ROOTNAME.par\".\n"
