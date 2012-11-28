#!/bin/sh
wget_path=$(type -p wget);
wget_ex=$?;
if [ -x src/bartlby ];
then
	if [ -x src/libs/mysql.so ];
	then
		echo "##########################################################################################";
		echo "# congratulations your compile seems to be successfully                                  #";
		echo "# for helping us improving bartlby, do you wan't your compilation to be counted online?  #";
		echo "# no data will be collected only a simple hit-counter is incemented                      #"; 
		echo "##########################################################################################";
		echo -ne "If you dont want to be counted type 'no' otherwise just hit enter [Y]";
		read tk;
		if [ "x$tk"  = "x" ] 
		then
			tk="y";
		fi;
		if [ $tk != "n"  ]; 
		then
			if [ $tk != "N"  ]; 
			then
				if [ $tk != "N"  ]; 
				then
					if [ $wget_ex = 0 ];
					then
						echo "hitting...bartlby.org";
						$wget_path -O /dev/null "http://www.bartlby.org/stat.php?type=compile&result=1&extra=";
						echo "Thank you, we hope you enjoy bartlby";
					fi;
			fi;	fi;
		fi;
		
		
		echo "##########################################################################################";
		echo "# Installation completed                                                                 #";
		echo "##########################################################################################";
		
	fi;
fi
echo "";
