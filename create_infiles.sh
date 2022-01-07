#! /bin/bash
zer=0

if [ $2 -ge 0 ] #checks if arguments are rigth
then
    if [ $3 -ge 0 ]
    then
        if [ $4 -ge 1 ]
        then
            dirname=$1
            if [ ! -d "$dirname" ] #checks if file exists 
            then
                echo "File doesn't exist. Creating now"
                mkdir ./$dirname
                echo "File created"
            else
                echo "File exists"
            fi
            cd $dirname

            fval=$2     
            dval=$3
            filld=$(( dval + 1 ))
            lvl=1
            flv=$4

            left=$(( fval / filld )) #calculates files to be placed under parent diectory
            pl=$(( fval % filld ))
            if [ $pl -ne $zer ]
            then
                make=$(( left + 1 ))
            else
                make=$left
            fi

            for ((i=0; i<make; i++)) #and creates them
            do
                r=$(( $RANDOM % 8 + 1 ))
                file=$(tr -cd '[:alnum:]' < /dev/urandom | head -c$r)
                r=$(( $RANDOM % 130048 + 1024 ))
                tr -cd '[:alnum:]' < /dev/urandom | head -c$r >> $file
                fval=$(( fval - 1 ))
            done
            filld=$(( filld - 1 ))
            for ((val=0; val<dval; val++))
            do
                if [ $lvl -ne $flv ]
                then
                    r=$(( $RANDOM % 8 + 1 ))
                    output=$(tr -cd '[:alnum:]' < /dev/urandom | head -c$r)
                    mkdir ./$output
                    cd ./$output

                    left=$(( fval / filld ))        #similar for the subdirectories
                    pl=$(( fval % filld ))
                    if [ $pl -ne $zer ]
                    then
                        make=$(( left + 1 ))
                    else
                        make=$left
                    fi
                    for ((i=0; i<make; i++))
                    do
                        r=$(( $RANDOM % 8 + 1 ))
                        file=$(tr -cd '[:alnum:]' < /dev/urandom | head -c$r)
                        r=$(( $RANDOM % 130048 + 1024 ))
                        tr -cd '[:alnum:]' < /dev/urandom | head -c$r >> $file
                        fval=$(( fval - 1 ))
                    done
                    filld=$(( filld - 1 ))
                    lvl=$(( lvl + 1 )) 
                else                              #if a subdirectory is at the final level 
                    r=$(( $RANDOM % 8 + 1 ))
                    output=$(tr -cd '[:alnum:]' < /dev/urandom | head -c$r)
                    mkdir ./$output
                    cd ./$output

                    left=$(( fval / filld ))
                    pl=$(( fval % filld ))
                    if [ $pl -ne $zer ]
                    then
                        make=$(( left + 1 ))
                    else
                        make=$left
                    fi
                    for ((i=0; i<make; i++))
                    do
                        r=$(( $RANDOM % 8 + 1 ))
                        file=$(tr -cd '[:alnum:]' < /dev/urandom | head -c$r)
                        r=$(( $RANDOM % 130048 + 1024 ))
                        tr -cd '[:alnum:]' < /dev/urandom | head -c$r >> $file
                        fval=$(( fval - 1 ))
                    done
                    filld=$(( filld - 1 ))
                    lvl=1
                    for ((j=0; j<flv; j++))      #it rerurns at the parent directory's level at the end
                    do
                        cd ..
                    done
                fi
            done
        fi
    fi
fi


