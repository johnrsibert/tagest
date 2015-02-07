#!/usr/bin/gawk -f

BEGIN{
  FS="/| *"
  SUBSEP=","

  spp="YFT"
  print spp
  res="05deg" #"1deg"
  nfile = 2
  size[1]="small"
  size[2]="big"
  file[1]=""
  file[2]=""
  #sibert_recovery_all_table_SKJ_big_PTTP_1deg.txt 
  for (f = 1; f <= nfile; f++)
  {
     file[f]= "sibert_recovery_all_table_"spp"_"size[f]"_PTTP_"res".txt" 
     print file[f] 
  }
  print "finished creating file names"
  for (f = 1; f <= nfile; f++)
  {
     print file[f] 
     ARGC=3
     ARGV[f] = file[f]
  }
  print "finished printing file names"
  ofile = spp"_recaps_by_fleet.csv"
  
}

{
  print nfile
  print "starting"
  for (f = 1; f <= nfile; f++)
  { 
     print file[f] 
     while ( (getline < file[f]) > 0)
     { 
        if (strtonum($1) < 1)
        {
        #  print "1:"$1,"2:"$2,"3:"$3
        #  if ( ($7 != "NA") && ($8 != "NA") &&
        #       ($7 != "XX") && ($8 != "XX") )
           {
              fleet = toupper($8$7)
              if  (length(fleet) < 4) 
                 fleet = "NANA"
              if  (length(fleet) == 4) 
              {
                 if (f == 1)
                   tcaps[fleet,2] = 0
                 tcaps[fleet,f] = tcaps[fleet,f] +1
              #  if (fleet == "IDOT")
              #     print $0
              } 
           }
        #  print fleet,tcaps[fleet,f]
        }
     }
     print "Done"
  }
  exit
}
END{
  # print "Reached END",SUBSEP
  sum1 = 0
  for (fleet in tcaps)
  {
  #  print fleet":",tcaps[fleet]
     sum1 = sum1 + tcaps[fleet]
  }
  # print "\nsum1 = "sum1 

# print "fleet",size[1],size[2],"total"
  printf ("fleet,%s,%s",size[1],size[2]) > ofile

  pfleet=""
  start = 0
  pfleet = ""

#n = asorti(source, dest)
#for (i = 1; i <= n; i++) {
#do something with dest[i] Work with sorted indices directly
#do something with source[dest[i]] Access original array via sorted indicesI#

  n = asorti(tcaps,scaps)
  print n
# for (fleet in tcaps)
  for (i = 1; i <= n; i++)
  {
     fleet = scaps[i]
     k ++
     jj = split(fleet,ff,",")
     tfleet = ff[1]
     jj = ff[2]+0
     m = match(tfleet,/NA/) + match(tfleet,/OT/) + match(tfleet,/XX/)
     if (m > 0)
        tfleet ="*"tfleet
     print fleet,tfleet,jj,tcaps[fleet],"|",tcaps[tfleet,jj]
     if (tfleet != pfleet)
     {
        pfleet = tfleet
        start = 1
        printf("\n%s,",tfleet) > ofile
     }
     if (jj == 1)
     {
        start = start + 1
        printf("%d,",tcaps[fleet]) > ofile
     }
     if (jj == 2)
     {
          if (start == 1)
            printf(",") > ofile
       #  printf("%d (%d two ,%d)",tcaps[fleet],jj,start) > ofile
          printf("%d",tcaps[fleet]) > ofile
     }
  #  start = 0
  #  if (tfleet != pfleet)
  #  {
  #     r3 = r1 + r2
  #     if (pfleet != "")
  #       printf("\n%s,%d,%d,%d",pfleet,r1,r2,r3) > ofile
  #     pfleet = tfleet
  #     r1 = tcaps[fleet]
  #     r2 = 0
  #     sum2 = sum2 + r3
  #  }
  #  else
  #  {
  #     r2 = tcaps[fleet]
  #  }
  }
# r3 = r1 + r2
# sum2 = sum2 + r3
# printf("\n%s,%d,%d,%d",pfleet,r1,r2,r3) > ofile
}
