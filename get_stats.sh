#! /bin/bash
tee  file1 >/dev/null

nocl=0 #the variables to be printed
bid=0
sid=999
bs=0
br=0
fs=0
fr=0
cll=0
c=0
s=0
cnt=1

echo List of clients:

while read name; do  # reading the file line by line in order to update the variables' values
    IFS=' ' read -ra ADDR <<< "$name"
    for i in "${ADDR[@]}"; do

        if [ "$i" == "id"  ]
        then
            nocl=$(( nocl + 1 ))
            c=1
            cnt=$(( cnt + 1 ))
        elif [ "$cnt" -eq 2 ]
        then
            if [ "$c" -eq 1 ]
            then
                echo $i     #prints the ids of th eclients
                if [ "$i" -ge "$bid" ]
                then
                    bid=$i 
                fi
                if [ "$cnt" -le "$sid" ]
                then
                    sid=$i
                fi
                cnt=$(( cnt + 1 ))
            fi
        elif [ "$cnt" -eq 3 ]
        then
            if [ "$c" -eq 1 ]
            then
                c=0
                cnt=1
            fi
        fi


        if [ "$i" == "received" ]
        then
            c=2
            cnt=$(( cnt + 1 ))
        elif [ "$cnt" -eq 2 ]
        then
            if [ "$c" -eq 2 ]
            then
                if [ "$i" == "bytes" ]
                then
                    s=1 
                fi
                if [ "$i" == "file" ]
                then
                    s=2
                fi
                cnt=$(( cnt + 1 ))
            fi
        elif [ "$cnt" -eq 3 ]
        then
            if [ "$c" -eq 2 ]
            then
                if [ "$s" -eq 1 ]
                then
                     br=$(( br + i ))
                fi
                if [ "$s" -eq 2 ]
                then
                    fr=$(( fr + 1 ))
                fi
                c=0
                cnt=1
            fi
        fi    

        if [ "$i" == "send" ]
        then
            c=3
            cnt=$(( cnt + 1 ))
        elif [ "$cnt" -eq 2 ]
        then
            if [ "$c" -eq 3 ]
            then
                if [ "$i" == "bytes" ]
                then
                    s=1 
                fi
                if [ "$i" == "file" ]
                then
                    s=2
                fi
                cnt=$(( cnt + 1 ))
            fi
        elif [ "$cnt" -eq 3 ]
        then
            if [ "$c" -eq 3 ]
            then
                if [ "$s" -eq 1 ]
                then
                     bs=$(( bs + i ))
                fi
                if [ "$s" -eq 2 ]
                then
                    fs=$(( fs + 1 ))
                fi
                c=0
                cnt=1
            fi
        fi

        if [ "$i" == "end" ]
        then
            cll=$(( cll + 1 ))
        fi

    done
done < file1

echo No of clients $nocl #and the final values of the variables
echo biggest id $bid
echo smallest id $sid
echo bytes send $bs
echo files send $fs
echo bytes rec $br
echo files rec $fr
echo clients left $cll